/* Filtering of data through a subprocess.
   Copyright (C) 2001-2003, 2008-2022 Free Software Foundation, Inc.
   Written by Paolo Bonzini <bonzini@gnu.org>, 2009,
   and Bruno Haible <bruno@clisp.org>, 2009.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include "pipe-filter.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#if defined _WIN32 && ! defined __CYGWIN__
# include <windows.h>
# include <process.h> /* _beginthreadex, _endthreadex */
#else
# include <signal.h>
# include <sys/select.h>
#endif

#include "error.h"
#include "spawn-pipe.h"
#include "wait-process.h"
#include "xalloc.h"
#include "gettext.h"

#define _(str) gettext (str)

#include "pipe-filter-aux.h"

struct pipe_filter_gi
{
  /* Arguments passed to pipe_filter_gi_create.  */
  const char *progname;
  bool null_stderr;
  bool exit_on_error;
  prepare_read_fn prepare_read;
  done_read_fn done_read;
  void *private_data;

  /* Management of the subprocess.  */
  pid_t child;
  int fd[2];
  bool exited;
  int exitstatus;

  /* Status of the writer part.  */
  volatile bool writer_terminated;
  int writer_errno;
  /* Status of the reader part.  */
  volatile bool reader_terminated;
  volatile int reader_errno;

#if defined _WIN32 && ! defined __CYGWIN__
  CRITICAL_SECTION lock; /* protects the volatile fields */
  HANDLE reader_thread_handle;
#else
  struct sigaction orig_sigpipe_action;
  fd_set readfds;  /* All bits except fd[0] are always cleared.  */
  fd_set writefds; /* All bits except fd[1] are always cleared.  */
#endif
};


/* Platform dependent functions.  */

/* Perform additional initializations.
   Return 0 if successful, -1 upon failure.  */
static int filter_init (struct pipe_filter_gi *filter);

/* Write count bytes starting at buf, while at the same time invoking the
   read iterator (the functions prepare_read/done_read) when needed.  */
static void filter_loop (struct pipe_filter_gi *filter,
                         const char *wbuf, size_t count);

/* Perform cleanup actions at the end.
   finish_reading is true if there was no error, or false if some error
   occurred already.  */
static void filter_cleanup (struct pipe_filter_gi *filter,
                            bool finish_reading);


#if defined _WIN32 && ! defined __CYGWIN__
/* Native Windows API.  */

static unsigned int WINAPI
reader_thread_func (void *thread_arg)
{
  struct pipe_filter_gi *filter = (struct pipe_filter_gi *) thread_arg;

  for (;;)
    {
      size_t bufsize;
      void *buf = filter->prepare_read (&bufsize, filter->private_data);
      if (!(buf != NULL && bufsize > 0))
        /* prepare_read returned wrong values.  */
        abort ();
      {
        ssize_t nread =
          read (filter->fd[0], buf, bufsize > SSIZE_MAX ? SSIZE_MAX : bufsize);
        EnterCriticalSection (&filter->lock);
        /* If the writer already encountered an error, terminate.  */
        if (filter->writer_terminated)
          break;
        if (nread < 0)
          {
            filter->reader_errno = errno;
            break;
          }
        else if (nread > 0)
          filter->done_read (buf, nread, filter->private_data);
        else /* nread == 0 */
          break;
        LeaveCriticalSection (&filter->lock);
      }
    }

  filter->reader_terminated = true;
  LeaveCriticalSection (&filter->lock);
  _endthreadex (0); /* calls ExitThread (0) */
  abort ();
}

static int
filter_init (struct pipe_filter_gi *filter)
{
  InitializeCriticalSection (&filter->lock);
  EnterCriticalSection (&filter->lock);

  filter->reader_thread_handle =
    (HANDLE) _beginthreadex (NULL, 100000, reader_thread_func, filter,
                             0, NULL);

  if (filter->reader_thread_handle == NULL)
    {
      if (filter->exit_on_error)
        error (EXIT_FAILURE, 0, _("creation of reading thread failed"));
      return -1;
    }
  else
    return 0;
}

static void
filter_loop (struct pipe_filter_gi *filter, const char *wbuf, size_t count)
{
  if (!filter->writer_terminated)
    {
      for (;;)
        {
          ssize_t nwritten;

          /* Allow the reader thread to continue.  */
          LeaveCriticalSection (&filter->lock);

          nwritten =
            write (filter->fd[1], wbuf, count > SSIZE_MAX ? SSIZE_MAX : count);

          /* Get the lock back from the reader thread.  */
          EnterCriticalSection (&filter->lock);

          if (nwritten < 0)
            {
              /* Don't assume that the gnulib modules 'write' and 'sigpipe' are
                 used.  */
              if (GetLastError () == ERROR_NO_DATA)
                errno = EPIPE;
              filter->writer_errno = errno;
              filter->writer_terminated = true;
              break;
            }
          else if (nwritten > 0)
            {
              count -= nwritten;
              if (count == 0)
                break;
              wbuf += nwritten;
            }
          else /* nwritten == 0 */
            {
              filter->writer_terminated = true;
              break;
            }
        }
    }
}

static void
filter_cleanup (struct pipe_filter_gi *filter, bool finish_reading)
{
  if (finish_reading)
    {
      LeaveCriticalSection (&filter->lock);
      WaitForSingleObject (filter->reader_thread_handle, INFINITE);
    }
  else
    TerminateThread (filter->reader_thread_handle, 1);

  CloseHandle (filter->reader_thread_handle);
  DeleteCriticalSection (&filter->lock);
}

#else
/* Unix API.  */

static int
filter_init (struct pipe_filter_gi *filter)
{
#if !(defined _WIN32 && ! defined __CYGWIN__)
  /* When we write to the child process and it has just terminated,
     we don't want to die from a SIGPIPE signal.  So set the SIGPIPE
     handler to SIG_IGN, and handle EPIPE error codes in write().  */
  {
    struct sigaction sigpipe_action;

    sigpipe_action.sa_handler = SIG_IGN;
    sigpipe_action.sa_flags = 0;
    sigemptyset (&sigpipe_action.sa_mask);
    if (sigaction (SIGPIPE, &sigpipe_action, &filter->orig_sigpipe_action) < 0)
      abort ();
  }
#endif

  /* Enable non-blocking I/O.  This permits the read() and write() calls
     to return -1/EAGAIN without blocking; this is important for polling
     if HAVE_SELECT is not defined.  It also permits the read() and write()
     calls to return after partial reads/writes; this is important if
     HAVE_SELECT is defined, because select() only says that some data
     can be read or written, not how many.  Without non-blocking I/O,
     Linux 2.2.17 and BSD systems prefer to block instead of returning
     with partial results.  */
  {
    int fcntl_flags;

    if ((fcntl_flags = fcntl (filter->fd[1], F_GETFL, 0)) < 0
        || fcntl (filter->fd[1], F_SETFL, fcntl_flags | O_NONBLOCK) == -1
        || (fcntl_flags = fcntl (filter->fd[0], F_GETFL, 0)) < 0
        || fcntl (filter->fd[0], F_SETFL, fcntl_flags | O_NONBLOCK) == -1)
      {
        if (filter->exit_on_error)
          error (EXIT_FAILURE, errno,
                 _("cannot set up nonblocking I/O to %s subprocess"),
                 filter->progname);
        return -1;
      }
  }

  FD_ZERO (&filter->readfds);
  FD_ZERO (&filter->writefds);

  return 0;
}

static void
filter_loop (struct pipe_filter_gi *filter, const char *wbuf, size_t count)
{
  /* This function is used in two situations:
     - in order to write some data to the subprocess
       [done_writing = false],
     - in order to read the remaining data after everything was written
       [done_writing = true].  In this case buf is NULL and count is
       ignored.  */
  bool done_writing = (wbuf == NULL);

  if (!done_writing)
    {
      if (filter->writer_terminated || filter->reader_terminated)
        /* pipe_filter_gi_write was called when it should not be.  */
        abort ();
    }
  else
    {
      if (filter->reader_terminated)
        return;
    }

  /* Loop, trying to write the given buffer or reading, whichever is
     possible.  */
  for (;;)
    {
      /* Here filter->writer_terminated is false.  When it becomes true, this
         loop is terminated.  */
      /* Whereas filter->reader_terminated is initially false but may become
         true during this loop.  */
      /* Here, if !done_writing, count > 0.  When count becomes 0, this loop
         is terminated.  */
      /* Here, if done_writing, filter->reader_terminated is false.  When
         filter->reader_terminated becomes true, this loop is terminated.  */
# if HAVE_SELECT
      int n, retval;

      /* See whether reading or writing is possible.  */
      n = 1;
      if (!filter->reader_terminated)
        {
          FD_SET (filter->fd[0], &filter->readfds);
          n = filter->fd[0] + 1;
        }
      if (!done_writing)
        {
          FD_SET (filter->fd[1], &filter->writefds);
          if (n <= filter->fd[1])
            n = filter->fd[1] + 1;
        }
      /* Do EINTR handling here instead of in pipe-filter-aux.h,
         because select() cannot be referred to from an inline
         function on AIX 7.1.  */
      do
        retval = select (n,
                         (!filter->reader_terminated ? &filter->readfds : NULL),
                         (!done_writing ? &filter->writefds : NULL),
                         NULL, NULL);
      while (retval < 0 && errno == EINTR);
      n = retval;

      if (n < 0)
        {
          if (filter->exit_on_error)
            error (EXIT_FAILURE, errno,
                   _("communication with %s subprocess failed"),
                   filter->progname);
          filter->writer_errno = errno;
          filter->writer_terminated = true;
          break;
        }

      if (!done_writing && FD_ISSET (filter->fd[1], &filter->writefds))
        goto try_write;
      if (!filter->reader_terminated
          && FD_ISSET (filter->fd[0], &filter->readfds))
        goto try_read;
      /* How could select() return if none of the two descriptors is ready?  */
      abort ();
# endif

      /* Attempt to write.  */
# if HAVE_SELECT
    try_write:
# endif
      if (!done_writing)
        {
          ssize_t nwritten =
            write (filter->fd[1], wbuf, count > SSIZE_MAX ? SSIZE_MAX : count);
          if (nwritten < 0)
            {
              if (!IS_EAGAIN (errno))
                {
                  if (filter->exit_on_error)
                    error (EXIT_FAILURE, errno,
                           _("write to %s subprocess failed"),
                           filter->progname);
                  filter->writer_errno = errno;
                  filter->writer_terminated = true;
                  break;
                }
            }
          else if (nwritten > 0)
            {
              count -= nwritten;
              if (count == 0)
                break;
              wbuf += nwritten;
            }
        }
# if HAVE_SELECT
      continue;
# endif

      /* Attempt to read.  */
# if HAVE_SELECT
    try_read:
# endif
      if (!filter->reader_terminated)
        {
          size_t bufsize;
          void *buf = filter->prepare_read (&bufsize, filter->private_data);
          if (!(buf != NULL && bufsize > 0))
            /* prepare_read returned wrong values.  */
            abort ();
          {
            ssize_t nread =
              read (filter->fd[0], buf,
                    bufsize > SSIZE_MAX ? SSIZE_MAX : bufsize);
            if (nread < 0)
              {
                if (!IS_EAGAIN (errno))
                  {
                    if (filter->exit_on_error)
                      error (EXIT_FAILURE, errno,
                             _("read from %s subprocess failed"),
                             filter->progname);
                    filter->reader_errno = errno;
                    filter->reader_terminated = true;
                    break;
                  }
              }
            else if (nread > 0)
              filter->done_read (buf, nread, filter->private_data);
            else /* nread == 0 */
              {
                filter->reader_terminated = true;
                if (done_writing)
                  break;
              }
          }
      }
# if HAVE_SELECT
      continue;
# endif
    }
}

static void
filter_cleanup (struct pipe_filter_gi *filter, bool finish_reading)
{
  if (finish_reading)
    /* A select loop, with done_writing = true.  */
    filter_loop (filter, NULL, 0);

  if (sigaction (SIGPIPE, &filter->orig_sigpipe_action, NULL) < 0)
    abort ();
}

#endif


/* Terminate the child process.  Do nothing if it already exited.  */
static void
filter_terminate (struct pipe_filter_gi *filter)
{
  if (!filter->exited)
    {
      /* Tell the child there is nothing more the parent will send.  */
      close (filter->fd[1]);
      filter_cleanup (filter, !filter->reader_terminated);
      close (filter->fd[0]);
      filter->exitstatus =
        wait_subprocess (filter->child, filter->progname, true,
                         filter->null_stderr, true, filter->exit_on_error,
                         NULL);
      if (filter->exitstatus != 0 && filter->exit_on_error)
        error (EXIT_FAILURE, 0,
               _("subprocess %s terminated with exit code %d"),
               filter->progname, filter->exitstatus);
      filter->exited = true;
    }
}

/* After filter_terminate:
   Return 0 upon success, or (only if exit_on_error is false):
   - -1 with errno set upon failure,
   - the positive exit code of the subprocess if that failed.  */
static int
filter_retcode (struct pipe_filter_gi *filter)
{
  if (filter->writer_errno != 0)
    {
      errno = filter->writer_errno;
      return -1;
    }
  else if (filter->reader_errno != 0)
    {
      errno = filter->reader_errno;
      return -1;
    }
  else
    return filter->exitstatus;
}

struct pipe_filter_gi *
pipe_filter_gi_create (const char *progname,
                       const char *prog_path, const char * const *prog_argv,
                       bool null_stderr, bool exit_on_error,
                       prepare_read_fn prepare_read,
                       done_read_fn done_read,
                       void *private_data)
{
  struct pipe_filter_gi *filter;

  filter =
    (struct pipe_filter_gi *) xmalloc (sizeof (struct pipe_filter_gi));

  /* Open a bidirectional pipe to a subprocess.  */
  filter->child = create_pipe_bidi (progname, prog_path, prog_argv,
                                    NULL, null_stderr, true, exit_on_error,
                                    filter->fd);
  filter->progname = progname;
  filter->null_stderr = null_stderr;
  filter->exit_on_error = exit_on_error;
  filter->prepare_read = prepare_read;
  filter->done_read = done_read;
  filter->private_data = private_data;
  filter->exited = false;
  filter->exitstatus = 0;
  filter->writer_terminated = false;
  filter->writer_errno = 0;
  filter->reader_terminated = false;
  filter->reader_errno = 0;

  if (filter->child == -1)
    {
      /* Child process could not be created.
         Arrange for filter_retcode (filter) to be the current errno.  */
      filter->writer_errno = errno;
      filter->writer_terminated = true;
      filter->exited = true;
    }
  else if (filter_init (filter) < 0)
    filter_terminate (filter);

  return filter;
}

int
pipe_filter_gi_write (struct pipe_filter_gi *filter,
                      const void *buf, size_t size)
{
  if (buf == NULL)
    /* Invalid argument.  */
    abort ();

  if (filter->exited)
    return filter_retcode (filter);

  if (size > 0)
    {
      filter_loop (filter, buf, size);
      if (filter->writer_terminated || filter->reader_terminated)
        {
          filter_terminate (filter);
          return filter_retcode (filter);
        }
    }
  return 0;
}

int
pipe_filter_gi_close (struct pipe_filter_gi *filter)
{
  int ret;

  filter_terminate (filter);
  ret = filter_retcode (filter);
  free (filter);
  return ret;
}

/* Temporary directories and temporary files with automatic cleanup.
   Copyright (C) 2001, 2003, 2006-2007, 2009-2020 Free Software Foundation,
   Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2006.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */


#include <config.h>

/* Specification.  */
#include "clean-temp.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined _WIN32 && ! defined __CYGWIN__
# define WIN32_LEAN_AND_MEAN  /* avoid including junk */
# include <windows.h>
#endif

#include "error.h"
#include "fatal-signal.h"
#include "asyncsafe-spin.h"
#include "pathmax.h"
#include "tmpdir.h"
#include "xalloc.h"
#include "xmalloca.h"
#include "glthread/lock.h"
#include "gl_xlist.h"
#include "gl_linkedhash_list.h"
#include "gl_linked_list.h"
#include "gettext.h"
#if GNULIB_TEMPNAME
# include "tempname.h"
#endif
#if GNULIB_FWRITEERROR
# include "fwriteerror.h"
#endif
#if GNULIB_CLOSE_STREAM
# include "close-stream.h"
#endif
#if GNULIB_FCNTL_SAFER
# include "fcntl--.h"
#endif
#if GNULIB_FOPEN_SAFER
# include "stdio--.h"
#endif

#define _(str) gettext (str)

/* GNU Hurd doesn't have PATH_MAX.  Use a fallback.
   Temporary directory names are usually not that long.  */
#ifndef PATH_MAX
# define PATH_MAX 1024
#endif

#if defined _WIN32 && ! defined __CYGWIN__
/* Don't assume that UNICODE is not defined.  */
# undef OSVERSIONINFO
# define OSVERSIONINFO OSVERSIONINFOA
# undef GetVersionEx
# define GetVersionEx GetVersionExA
#endif


/* The use of 'volatile' in the types below (and ISO C 99 section 5.1.2.3.(5))
   ensure that while constructing or modifying the data structures, the field
   values are written to memory in the order of the C statements.  So the
   signal handler can rely on these field values to be up to date.  */


/* Lock that protects the file_cleanup_list from concurrent modification in
   different threads.  */
gl_lock_define_initialized (static, file_cleanup_list_lock)

/* List of all temporary files without temporary directories.  */
static gl_list_t /* <char *> */ volatile file_cleanup_list;


/* Registry for a single temporary directory.
   'struct temp_dir' from the public header file overlaps with this.  */
struct tempdir
{
  /* The absolute pathname of the directory.  */
  char * volatile dirname;
  /* Whether errors during explicit cleanup are reported to standard error.  */
  bool cleanup_verbose;
  /* Absolute pathnames of subdirectories.  */
  gl_list_t /* <char *> */ volatile subdirs;
  /* Absolute pathnames of files.  */
  gl_list_t /* <char *> */ volatile files;
};

/* Lock that protects the dir_cleanup_list from concurrent modification in
   different threads.  */
gl_lock_define_initialized (static, dir_cleanup_list_lock)

/* List of all temporary directories.  */
static struct
{
  struct tempdir * volatile * volatile tempdir_list;
  size_t volatile tempdir_count;
  size_t tempdir_allocated;
} dir_cleanup_list /* = { NULL, 0, 0 } */;


/* A file descriptor to be closed.
   In multithreaded programs, it is forbidden to close the same fd twice,
   because you never know what unrelated open() calls are being executed in
   other threads. So, the 'close (fd)' must be guarded by a once-only guard.  */
struct closeable_fd
{
  /* The file descriptor to close.  */
  int volatile fd;
  /* Set to true when it has been closed.  */
  bool volatile closed;
  /* Lock that protects the fd from being closed twice.  */
  asyncsafe_spinlock_t lock;
  /* Tells whether this list element has been done and can be freed.  */
  bool volatile done;
};

/* Lock that protects the descriptors list from concurrent modification in
   different threads.  */
gl_lock_define_initialized (static, descriptors_lock)

/* List of all open file descriptors to temporary files.  */
static gl_list_t /* <closeable_fd *> */ volatile descriptors;


/* For the subdirs and for the files, we use a gl_list_t of type LINKEDHASH.
   Why?  We need a data structure that

     1) Can contain an arbitrary number of 'char *' values.  The strings
        are compared via strcmp, not pointer comparison.
     2) Has insertion and deletion operations that are fast: ideally O(1),
        or possibly O(log n).  This is important for GNU sort, which may
        create a large number of temporary files.
     3) Allows iteration through all elements from within a signal handler.
     4) May or may not allow duplicates.  It doesn't matter here, since
        any file or subdir can only be removed once.

   Criterion 1) would allow any gl_list_t or gl_oset_t implementation.

   Criterion 2) leaves only GL_LINKEDHASH_LIST, GL_TREEHASH_LIST, or
   GL_TREE_OSET.

   Criterion 3) puts at disadvantage GL_TREEHASH_LIST and GL_TREE_OSET.
   Namely, iteration through the elements of a binary tree requires access
   to many ->left, ->right, ->parent pointers. However, the rebalancing
   code for insertion and deletion in an AVL or red-black tree is so
   complicated that we cannot assume that >left, ->right, ->parent pointers
   are in a consistent state throughout these operations.  Therefore, to
   avoid a crash in the signal handler, all destructive operations to the
   lists would have to be protected by a
       block_fatal_signals ();
       ...
       unblock_fatal_signals ();
   pair.  Which causes extra system calls.

   Criterion 3) would also discourage GL_ARRAY_LIST and GL_CARRAY_LIST,
   if they were not already excluded.  Namely, these implementations use
   xrealloc(), leaving a time window in which in the list->elements pointer
   points to already deallocated memory.  To avoid a crash in the signal
   handler at such a moment, all destructive operations would have to
   protected by block/unblock_fatal_signals (), in this case too.

   A list of type GL_LINKEDHASH_LIST without duplicates fulfills all
   requirements:
     2) Insertion and deletion are O(1) on average.
     3) The gl_list_iterator, gl_list_iterator_next implementations do
        not trigger memory allocations, nor other system calls, and are
        therefore safe to be called from a signal handler.
        Furthermore, since SIGNAL_SAFE_LIST is defined, the implementation
        of the destructive functions ensures that the list structure is
        safe to be traversed at any moment, even when interrupted by an
        asynchronous signal.
 */

/* String equality and hash code functions used by the lists.  */

static bool
string_equals (const void *x1, const void *x2)
{
  const char *s1 = (const char *) x1;
  const char *s2 = (const char *) x2;
  return strcmp (s1, s2) == 0;
}

#define SIZE_BITS (sizeof (size_t) * CHAR_BIT)

/* A hash function for NUL-terminated char* strings using
   the method described by Bruno Haible.
   See https://www.haible.de/bruno/hashfunc.html.  */
static size_t
string_hash (const void *x)
{
  const char *s = (const char *) x;
  size_t h = 0;

  for (; *s; s++)
    h = *s + ((h << 9) | (h >> (SIZE_BITS - 9)));

  return h;
}


/* The set of fatal signal handlers.
   Cached here because we are not allowed to call get_fatal_signal_set ()
   from a signal handler.  */
static const sigset_t *fatal_signal_set /* = NULL */;

static void
init_fatal_signal_set (void)
{
  if (fatal_signal_set == NULL)
    fatal_signal_set = get_fatal_signal_set ();
}


/* Close a file descriptor.
   Avoids race conditions with normal thread code or signal-handler code that
   might want to close the same file descriptor.  */
static _GL_ASYNC_SAFE int
asyncsafe_close (struct closeable_fd *element)
{
  sigset_t saved_mask;
  int ret;
  int saved_errno;

  asyncsafe_spin_lock (&element->lock, fatal_signal_set, &saved_mask);
  if (!element->closed)
    {
      ret = close (element->fd);
      saved_errno = errno;
      element->closed = true;
    }
  else
    {
      ret = 0;
      saved_errno = 0;
    }
  asyncsafe_spin_unlock (&element->lock, &saved_mask);
  element->done = true;

  errno = saved_errno;
  return ret;
}

/* Close a file descriptor and the stream that contains it.
   Avoids race conditions with signal-handler code that might want to close the
   same file descriptor.  */
static int
asyncsafe_fclose_variant (struct closeable_fd *element, FILE *fp,
                          int (*fclose_variant) (FILE *))
{
  if (fileno (fp) != element->fd)
    abort ();

  /* Flush buffered data first, to minimize the duration of the spin lock.  */
  fflush (fp);

  sigset_t saved_mask;
  int ret;
  int saved_errno;

  asyncsafe_spin_lock (&element->lock, fatal_signal_set, &saved_mask);
  if (!element->closed)
    {
      ret = fclose_variant (fp); /* invokes close (element->fd) */
      saved_errno = errno;
      element->closed = true;
    }
  else
    {
      ret = 0;
      saved_errno = 0;
    }
  asyncsafe_spin_unlock (&element->lock, &saved_mask);
  element->done = true;

  errno = saved_errno;
  return ret;
}

/* The signal handler.  It gets called asynchronously.  */
static _GL_ASYNC_SAFE void
cleanup_action (int sig _GL_UNUSED)
{
  size_t i;

  /* First close all file descriptors to temporary files.  */
  {
    gl_list_t fds = descriptors;

    if (fds != NULL)
      {
        gl_list_iterator_t iter;
        const void *element;

        iter = gl_list_iterator (fds);
        while (gl_list_iterator_next (&iter, &element, NULL))
          {
            asyncsafe_close ((struct closeable_fd *) element);
          }
        gl_list_iterator_free (&iter);
      }
  }

  {
    gl_list_t files = file_cleanup_list;

    if (files != NULL)
      {
        gl_list_iterator_t iter;
        const void *element;

        iter = gl_list_iterator (files);
        while (gl_list_iterator_next (&iter, &element, NULL))
          {
            const char *file = (const char *) element;
            unlink (file);
          }
        gl_list_iterator_free (&iter);
      }
  }

  for (i = 0; i < dir_cleanup_list.tempdir_count; i++)
    {
      struct tempdir *dir = dir_cleanup_list.tempdir_list[i];

      if (dir != NULL)
        {
          gl_list_iterator_t iter;
          const void *element;

          /* First cleanup the files in the subdirectories.  */
          iter = gl_list_iterator (dir->files);
          while (gl_list_iterator_next (&iter, &element, NULL))
            {
              const char *file = (const char *) element;
              unlink (file);
            }
          gl_list_iterator_free (&iter);

          /* Then cleanup the subdirectories.  */
          iter = gl_list_iterator (dir->subdirs);
          while (gl_list_iterator_next (&iter, &element, NULL))
            {
              const char *subdir = (const char *) element;
              rmdir (subdir);
            }
          gl_list_iterator_free (&iter);

          /* Then cleanup the temporary directory itself.  */
          rmdir (dir->dirname);
        }
    }
}


/* Initializes this facility.  */
static void
do_init_clean_temp (void)
{
  /* Initialize the data used by the cleanup handler.  */
  init_fatal_signal_set ();
  /* Register the cleanup handler.  */
  at_fatal_signal (&cleanup_action);
}

/* Ensure that do_init_clean_temp is called once only.  */
gl_once_define(static, clean_temp_once)

/* Initializes this facility upon first use.  */
static void
init_clean_temp (void)
{
  gl_once (clean_temp_once, do_init_clean_temp);
}


/* ============= Temporary files without temporary directories ============= */

/* Register the given ABSOLUTE_FILE_NAME as being a file that needs to be
   removed.
   Should be called before the file ABSOLUTE_FILE_NAME is created.  */
void
register_temporary_file (const char *absolute_file_name)
{
  gl_lock_lock (file_cleanup_list_lock);

  /* Make sure that this facility and the file_cleanup_list are initialized.  */
  if (file_cleanup_list == NULL)
    {
      init_clean_temp ();
      file_cleanup_list =
        gl_list_create_empty (GL_LINKEDHASH_LIST,
                              string_equals, string_hash, NULL, false);
    }

  /* Add absolute_file_name to file_cleanup_list, without duplicates.  */
  if (gl_list_search (file_cleanup_list, absolute_file_name) == NULL)
    gl_list_add_first (file_cleanup_list, xstrdup (absolute_file_name));

  gl_lock_unlock (file_cleanup_list_lock);
}

/* Unregister the given ABSOLUTE_FILE_NAME as being a file that needs to be
   removed.
   Should be called when the file ABSOLUTE_FILE_NAME could not be created.  */
void
unregister_temporary_file (const char *absolute_file_name)
{
  gl_lock_lock (file_cleanup_list_lock);

  gl_list_t list = file_cleanup_list;
  if (list != NULL)
    {
      gl_list_node_t node = gl_list_search (list, absolute_file_name);
      if (node != NULL)
        {
          char *old_string = (char *) gl_list_node_value (list, node);

          gl_list_remove_node (list, node);
          free (old_string);
        }
    }

  gl_lock_unlock (file_cleanup_list_lock);
}

/* Remove a file, with optional error message.
   Return 0 upon success, or -1 if there was some problem.  */
static int
do_unlink (const char *absolute_file_name, bool cleanup_verbose)
{
  if (unlink (absolute_file_name) < 0 && cleanup_verbose
      && errno != ENOENT)
    {
      error (0, errno,
             _("cannot remove temporary file %s"), absolute_file_name);
      return -1;
    }
  return 0;
}

/* Remove the given ABSOLUTE_FILE_NAME and unregister it.
   CLEANUP_VERBOSE determines whether errors are reported to standard error.
   Return 0 upon success, or -1 if there was some problem.  */
int
cleanup_temporary_file (const char *absolute_file_name, bool cleanup_verbose)
{
  int err;

  err = do_unlink (absolute_file_name, cleanup_verbose);
  unregister_temporary_file (absolute_file_name);

  return err;
}


/* ========= Temporary directories and temporary files inside them ========= */

/* Create a temporary directory.
   PREFIX is used as a prefix for the name of the temporary directory. It
   should be short and still give an indication about the program.
   PARENTDIR can be used to specify the parent directory; if NULL, a default
   parent directory is used (either $TMPDIR or /tmp or similar).
   CLEANUP_VERBOSE determines whether errors during explicit cleanup are
   reported to standard error.
   Return a fresh 'struct temp_dir' on success.  Upon error, an error message
   is shown and NULL is returned.  */
struct temp_dir *
create_temp_dir (const char *prefix, const char *parentdir,
                 bool cleanup_verbose)
{
  gl_lock_lock (dir_cleanup_list_lock);

  struct tempdir * volatile *tmpdirp = NULL;
  struct tempdir *tmpdir;
  size_t i;
  char *xtemplate;
  char *tmpdirname;

  /* See whether it can take the slot of an earlier temporary directory
     already cleaned up.  */
  for (i = 0; i < dir_cleanup_list.tempdir_count; i++)
    if (dir_cleanup_list.tempdir_list[i] == NULL)
      {
        tmpdirp = &dir_cleanup_list.tempdir_list[i];
        break;
      }
  if (tmpdirp == NULL)
    {
      /* See whether the array needs to be extended.  */
      if (dir_cleanup_list.tempdir_count == dir_cleanup_list.tempdir_allocated)
        {
          /* Note that we cannot use xrealloc(), because then the cleanup()
             function could access an already deallocated array.  */
          struct tempdir * volatile *old_array = dir_cleanup_list.tempdir_list;
          size_t old_allocated = dir_cleanup_list.tempdir_allocated;
          size_t new_allocated = 2 * dir_cleanup_list.tempdir_allocated + 1;
          struct tempdir * volatile *new_array =
            XNMALLOC (new_allocated, struct tempdir * volatile);

          if (old_allocated == 0)
            {
              /* First use of this facility.  */
              init_clean_temp ();
            }
          else
            {
              /* Don't use memcpy() here, because memcpy takes non-volatile
                 arguments and is therefore not guaranteed to complete all
                 memory stores before the next statement.  */
              size_t k;

              for (k = 0; k < old_allocated; k++)
                new_array[k] = old_array[k];
            }

          dir_cleanup_list.tempdir_list = new_array;
          dir_cleanup_list.tempdir_allocated = new_allocated;

          /* Now we can free the old array.  */
          /* No, we can't do that.  If cleanup_action is running in a different
             thread and has already fetched the tempdir_list pointer (getting
             old_array) but not yet accessed its i-th element, that thread may
             crash when accessing an element of the already freed old_array
             array.  */
          #if 0
          if (old_array != NULL)
            free ((struct tempdir **) old_array);
          #endif
        }

      tmpdirp = &dir_cleanup_list.tempdir_list[dir_cleanup_list.tempdir_count];
      /* Initialize *tmpdirp before incrementing tempdir_count, so that
         cleanup() will skip this entry before it is fully initialized.  */
      *tmpdirp = NULL;
      dir_cleanup_list.tempdir_count++;
    }

  /* Initialize a 'struct tempdir'.  */
  tmpdir = XMALLOC (struct tempdir);
  tmpdir->dirname = NULL;
  tmpdir->cleanup_verbose = cleanup_verbose;
  tmpdir->subdirs = gl_list_create_empty (GL_LINKEDHASH_LIST,
                                          string_equals, string_hash, NULL,
                                          false);
  tmpdir->files = gl_list_create_empty (GL_LINKEDHASH_LIST,
                                        string_equals, string_hash, NULL,
                                        false);

  /* Create the temporary directory.  */
  xtemplate = (char *) xmalloca (PATH_MAX);
  if (path_search (xtemplate, PATH_MAX, parentdir, prefix, parentdir == NULL))
    {
      error (0, errno,
             _("cannot find a temporary directory, try setting $TMPDIR"));
      goto quit;
    }
  block_fatal_signals ();
  tmpdirname = mkdtemp (xtemplate);
  int saved_errno = errno;
  if (tmpdirname != NULL)
    {
      tmpdir->dirname = tmpdirname;
      *tmpdirp = tmpdir;
    }
  unblock_fatal_signals ();
  if (tmpdirname == NULL)
    {
      error (0, saved_errno,
             _("cannot create a temporary directory using template \"%s\""),
             xtemplate);
      goto quit;
    }
  /* Replace tmpdir->dirname with a copy that has indefinite extent.
     We cannot do this inside the block_fatal_signals/unblock_fatal_signals
     block because then the cleanup handler would not remove the directory
     if xstrdup fails.  */
  tmpdir->dirname = xstrdup (tmpdirname);
  gl_lock_unlock (dir_cleanup_list_lock);
  freea (xtemplate);
  return (struct temp_dir *) tmpdir;

 quit:
  gl_lock_unlock (dir_cleanup_list_lock);
  freea (xtemplate);
  return NULL;
}

/* Register the given ABSOLUTE_FILE_NAME as being a file inside DIR, that
   needs to be removed before DIR can be removed.
   Should be called before the file ABSOLUTE_FILE_NAME is created.  */
void
register_temp_file (struct temp_dir *dir,
                    const char *absolute_file_name)
{
  struct tempdir *tmpdir = (struct tempdir *)dir;

  gl_lock_lock (dir_cleanup_list_lock);

  /* Add absolute_file_name to tmpdir->files, without duplicates.  */
  if (gl_list_search (tmpdir->files, absolute_file_name) == NULL)
    gl_list_add_first (tmpdir->files, xstrdup (absolute_file_name));

  gl_lock_unlock (dir_cleanup_list_lock);
}

/* Unregister the given ABSOLUTE_FILE_NAME as being a file inside DIR, that
   needs to be removed before DIR can be removed.
   Should be called when the file ABSOLUTE_FILE_NAME could not be created.  */
void
unregister_temp_file (struct temp_dir *dir,
                      const char *absolute_file_name)
{
  struct tempdir *tmpdir = (struct tempdir *)dir;

  gl_lock_lock (dir_cleanup_list_lock);

  gl_list_t list = tmpdir->files;
  gl_list_node_t node;

  node = gl_list_search (list, absolute_file_name);
  if (node != NULL)
    {
      char *old_string = (char *) gl_list_node_value (list, node);

      gl_list_remove_node (list, node);
      free (old_string);
    }

  gl_lock_unlock (dir_cleanup_list_lock);
}

/* Register the given ABSOLUTE_DIR_NAME as being a subdirectory inside DIR,
   that needs to be removed before DIR can be removed.
   Should be called before the subdirectory ABSOLUTE_DIR_NAME is created.  */
void
register_temp_subdir (struct temp_dir *dir,
                      const char *absolute_dir_name)
{
  struct tempdir *tmpdir = (struct tempdir *)dir;

  gl_lock_lock (dir_cleanup_list_lock);

  /* Add absolute_dir_name to tmpdir->subdirs, without duplicates.  */
  if (gl_list_search (tmpdir->subdirs, absolute_dir_name) == NULL)
    gl_list_add_first (tmpdir->subdirs, xstrdup (absolute_dir_name));

  gl_lock_unlock (dir_cleanup_list_lock);
}

/* Unregister the given ABSOLUTE_DIR_NAME as being a subdirectory inside DIR,
   that needs to be removed before DIR can be removed.
   Should be called when the subdirectory ABSOLUTE_DIR_NAME could not be
   created.  */
void
unregister_temp_subdir (struct temp_dir *dir,
                        const char *absolute_dir_name)
{
  struct tempdir *tmpdir = (struct tempdir *)dir;

  gl_lock_lock (dir_cleanup_list_lock);

  gl_list_t list = tmpdir->subdirs;
  gl_list_node_t node;

  node = gl_list_search (list, absolute_dir_name);
  if (node != NULL)
    {
      char *old_string = (char *) gl_list_node_value (list, node);

      gl_list_remove_node (list, node);
      free (old_string);
    }

  gl_lock_unlock (dir_cleanup_list_lock);
}

/* Remove a directory, with optional error message.
   Return 0 upon success, or -1 if there was some problem.  */
static int
do_rmdir (const char *absolute_dir_name, bool cleanup_verbose)
{
  if (rmdir (absolute_dir_name) < 0 && cleanup_verbose
      && errno != ENOENT)
    {
      error (0, errno,
             _("cannot remove temporary directory %s"), absolute_dir_name);
      return -1;
    }
  return 0;
}

/* Remove the given ABSOLUTE_FILE_NAME and unregister it.
   Return 0 upon success, or -1 if there was some problem.  */
int
cleanup_temp_file (struct temp_dir *dir,
                   const char *absolute_file_name)
{
  int err;

  err = do_unlink (absolute_file_name, dir->cleanup_verbose);
  unregister_temp_file (dir, absolute_file_name);

  return err;
}

/* Remove the given ABSOLUTE_DIR_NAME and unregister it.
   Return 0 upon success, or -1 if there was some problem.  */
int
cleanup_temp_subdir (struct temp_dir *dir,
                     const char *absolute_dir_name)
{
  int err;

  err = do_rmdir (absolute_dir_name, dir->cleanup_verbose);
  unregister_temp_subdir (dir, absolute_dir_name);

  return err;
}

/* Remove all registered files and subdirectories inside DIR.
   Only to be called with dir_cleanup_list_lock locked.
   Return 0 upon success, or -1 if there was some problem.  */
int
cleanup_temp_dir_contents (struct temp_dir *dir)
{
  struct tempdir *tmpdir = (struct tempdir *)dir;
  int err = 0;
  gl_list_t list;
  gl_list_iterator_t iter;
  const void *element;
  gl_list_node_t node;

  /* First cleanup the files in the subdirectories.  */
  list = tmpdir->files;
  iter = gl_list_iterator (list);
  while (gl_list_iterator_next (&iter, &element, &node))
    {
      char *file = (char *) element;

      err |= do_unlink (file, dir->cleanup_verbose);
      gl_list_remove_node (list, node);
      /* Now only we can free file.  */
      free (file);
    }
  gl_list_iterator_free (&iter);

  /* Then cleanup the subdirectories.  */
  list = tmpdir->subdirs;
  iter = gl_list_iterator (list);
  while (gl_list_iterator_next (&iter, &element, &node))
    {
      char *subdir = (char *) element;

      err |= do_rmdir (subdir, dir->cleanup_verbose);
      gl_list_remove_node (list, node);
      /* Now only we can free subdir.  */
      free (subdir);
    }
  gl_list_iterator_free (&iter);

  return err;
}

/* Remove all registered files and subdirectories inside DIR and DIR itself.
   DIR cannot be used any more after this call.
   Return 0 upon success, or -1 if there was some problem.  */
int
cleanup_temp_dir (struct temp_dir *dir)
{
  gl_lock_lock (dir_cleanup_list_lock);

  struct tempdir *tmpdir = (struct tempdir *)dir;
  int err = 0;
  size_t i;

  err |= cleanup_temp_dir_contents (dir);
  err |= do_rmdir (tmpdir->dirname, dir->cleanup_verbose);

  for (i = 0; i < dir_cleanup_list.tempdir_count; i++)
    if (dir_cleanup_list.tempdir_list[i] == tmpdir)
      {
        /* Remove dir_cleanup_list.tempdir_list[i].  */
        if (i + 1 == dir_cleanup_list.tempdir_count)
          {
            while (i > 0 && dir_cleanup_list.tempdir_list[i - 1] == NULL)
              i--;
            dir_cleanup_list.tempdir_count = i;
          }
        else
          dir_cleanup_list.tempdir_list[i] = NULL;
        /* Now only we can free the tmpdir->dirname, tmpdir->subdirs,
           tmpdir->files, and tmpdir itself.  */
        gl_list_free (tmpdir->files);
        gl_list_free (tmpdir->subdirs);
        free (tmpdir->dirname);
        free (tmpdir);
        gl_lock_unlock (dir_cleanup_list_lock);
        return err;
      }

  /* The user passed an invalid DIR argument.  */
  abort ();
}


/* ================== Opening and closing temporary files ================== */

#if defined _WIN32 && ! defined __CYGWIN__

/* On Windows, opening a file with _O_TEMPORARY has the effect of passing
   the FILE_FLAG_DELETE_ON_CLOSE flag to CreateFile(), which has the effect
   of deleting the file when it is closed - even when the program crashes.
   But (according to the Cygwin sources) it works only on Windows NT or newer.
   So we cache the info whether we are running on Windows NT or newer.  */

static bool
supports_delete_on_close ()
{
  static int known; /* 1 = yes, -1 = no, 0 = unknown */
  if (!known)
    {
      OSVERSIONINFO v;

      /* According to
         <https://docs.microsoft.com/en-us/windows/desktop/api/sysinfoapi/nf-sysinfoapi-getversionexa>
         this structure must be initialized as follows:  */
      v.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

      if (GetVersionEx (&v))
        known = (v.dwPlatformId == VER_PLATFORM_WIN32_NT ? 1 : -1);
      else
        known = -1;
    }
  return (known > 0);
}

#endif


/* Register a file descriptor to be closed.  */
static void
register_fd (int fd)
{
  gl_lock_lock (descriptors_lock);

  if (descriptors == NULL)
    descriptors = gl_list_create_empty (GL_LINKED_LIST, NULL, NULL, NULL,
                                        false);

  struct closeable_fd *element = XMALLOC (struct closeable_fd);
  element->fd = fd;
  element->closed = false;
  asyncsafe_spin_init (&element->lock);
  element->done = false;

  gl_list_add_first (descriptors, element);

  gl_lock_unlock (descriptors_lock);
}

/* Open a temporary file in a temporary directory.
   FILE_NAME must already have been passed to register_temp_file.
   Registers the resulting file descriptor to be closed.
   DELETE_ON_CLOSE indicates whether the file can be deleted when the resulting
   file descriptor or stream is closed.  */
int
open_temp (const char *file_name, int flags, mode_t mode, bool delete_on_close)
{
  int fd;
  int saved_errno;

  block_fatal_signals ();
  /* Note: 'open' here is actually open() or open_safer().  */
#if defined _WIN32 && ! defined __CYGWIN__
  /* Use _O_TEMPORARY when possible, to increase the chances that the
     temporary file is removed when the process crashes.  */
  if (delete_on_close && supports_delete_on_close ())
    fd = open (file_name, flags | _O_TEMPORARY, mode);
  else
#endif
    fd = open (file_name, flags, mode);
  saved_errno = errno;
  if (fd >= 0)
    register_fd (fd);
  unblock_fatal_signals ();
  errno = saved_errno;
  return fd;
}

/* Open a temporary file in a temporary directory.
   FILE_NAME must already have been passed to register_temp_file.
   Registers the resulting file descriptor to be closed.
   DELETE_ON_CLOSE indicates whether the file can be deleted when the resulting
   file descriptor or stream is closed.  */
FILE *
fopen_temp (const char *file_name, const char *mode, bool delete_on_close)
{
  FILE *fp;
  int saved_errno;

  block_fatal_signals ();
  /* Note: 'fopen' here is actually fopen() or fopen_safer().  */
#if defined _WIN32 && ! defined __CYGWIN__
  /* Use _O_TEMPORARY when possible, to increase the chances that the
     temporary file is removed when the process crashes.  */
  if (delete_on_close && supports_delete_on_close ())
    {
      size_t mode_len = strlen (mode);
      char *augmented_mode = (char *) xmalloca (mode_len + 2);
      memcpy (augmented_mode, mode, mode_len);
      memcpy (augmented_mode + mode_len, "D", 2);

      fp = fopen (file_name, augmented_mode);
      saved_errno = errno;

      freea (augmented_mode);
    }
  else
#endif
    {
      fp = fopen (file_name, mode);
      saved_errno = errno;
    }
  if (fp != NULL)
    {
      /* It is sufficient to register fileno (fp) instead of the entire fp,
         because at cleanup time there is no need to do an fflush (fp); a
         close (fileno (fp)) will be enough.  */
      int fd = fileno (fp);
      if (!(fd >= 0))
        abort ();
      register_fd (fd);
    }
  unblock_fatal_signals ();
  errno = saved_errno;
  return fp;
}

#if GNULIB_TEMPNAME

struct try_create_file_params
{
  int flags;
  mode_t mode;
};

static int
try_create_file (char *file_name_tmpl, void *params_)
{
  struct try_create_file_params *params = params_;
  return open (file_name_tmpl,
               (params->flags & ~O_ACCMODE) | O_RDWR | O_CREAT | O_EXCL,
               params->mode);
}

/* Open a temporary file, generating its name based on FILE_NAME_TMPL.
   FILE_NAME_TMPL must match the rules for mk[s]temp (i.e. end in "XXXXXX",
   possibly with a suffix).  The name constructed does not exist at the time
   of the call.  FILE_NAME_TMPL is overwritten with the result.
   A safe choice for MODE is S_IRUSR | S_IWUSR, a.k.a. 0600.
   Registers the file for deletion.
   Opens the file, with the given FLAGS and mode MODE.
   Registers the resulting file descriptor to be closed.  */
int
gen_register_open_temp (char *file_name_tmpl, int suffixlen,
                        int flags, mode_t mode)
{
  block_fatal_signals ();

  struct try_create_file_params params;
  params.flags = flags;
  params.mode = mode;

  int fd = try_tempname (file_name_tmpl, suffixlen, &params, try_create_file);

  int saved_errno = errno;
  if (fd >= 0)
    {
      init_clean_temp ();
      register_fd (fd);
      register_temporary_file (file_name_tmpl);
    }
  unblock_fatal_signals ();
  errno = saved_errno;
  return fd;
}

#endif

/* Close a temporary file.
   FD must have been returned by open_temp or gen_register_open_temp.
   Unregisters the previously registered file descriptor.  */
int
close_temp (int fd)
{
  if (fd < 0)
    return close (fd);

  init_fatal_signal_set ();

  int result = 0;
  int saved_errno = 0;

  gl_lock_lock (descriptors_lock);

  gl_list_t list = descriptors;
  if (list == NULL)
    /* descriptors should already contain fd.  */
    abort ();

  /* Search through the list, and clean it up on the fly.  */
  bool found = false;
  gl_list_iterator_t iter = gl_list_iterator (list);
  const void *elt;
  gl_list_node_t node;
  if (gl_list_iterator_next (&iter, &elt, &node))
    for (;;)
      {
        struct closeable_fd *element = (struct closeable_fd *) elt;

        /* Close the file descriptor, avoiding races with the signal
           handler.  */
        if (element->fd == fd)
          {
            found = true;
            result = asyncsafe_close (element);
            saved_errno = errno;
          }

        bool free_this_node = element->done;
        struct closeable_fd *element_to_free = element;
        gl_list_node_t node_to_free = node;

        bool have_next = gl_list_iterator_next (&iter, &elt, &node);

        if (free_this_node)
          {
            free (element_to_free);
            gl_list_remove_node (list, node_to_free);
          }

        if (!have_next)
          break;
      }
  gl_list_iterator_free (&iter);
  if (!found)
    /* descriptors should already contain fd.  */
    abort ();

  gl_lock_unlock (descriptors_lock);

  errno = saved_errno;
  return result;
}

static int
fclose_variant_temp (FILE *fp, int (*fclose_variant) (FILE *))
{
  int fd = fileno (fp);

  init_fatal_signal_set ();

  int result = 0;
  int saved_errno = 0;

  gl_lock_lock (descriptors_lock);

  gl_list_t list = descriptors;
  if (list == NULL)
    /* descriptors should already contain fd.  */
    abort ();

  /* Search through the list, and clean it up on the fly.  */
  bool found = false;
  gl_list_iterator_t iter = gl_list_iterator (list);
  const void *elt;
  gl_list_node_t node;
  if (gl_list_iterator_next (&iter, &elt, &node))
    for (;;)
      {
        struct closeable_fd *element = (struct closeable_fd *) elt;

        /* Close the file descriptor and the stream, avoiding races with the
           signal handler.  */
        if (element->fd == fd)
          {
            found = true;
            result = asyncsafe_fclose_variant (element, fp, fclose_variant);
            saved_errno = errno;
          }

        bool free_this_node = element->done;
        struct closeable_fd *element_to_free = element;
        gl_list_node_t node_to_free = node;

        bool have_next = gl_list_iterator_next (&iter, &elt, &node);

        if (free_this_node)
          {
            free (element_to_free);
            gl_list_remove_node (list, node_to_free);
          }

        if (!have_next)
          break;
      }
  gl_list_iterator_free (&iter);
  if (!found)
    /* descriptors should have contained fd.  */
    abort ();

  gl_lock_unlock (descriptors_lock);

  errno = saved_errno;
  return result;
}

/* Close a temporary file.
   FP must have been returned by fopen_temp, or by fdopen on a file descriptor
   returned by open_temp or gen_register_open_temp.
   Unregisters the previously registered file descriptor.  */
int
fclose_temp (FILE *fp)
{
  return fclose_variant_temp (fp, fclose);
}

#if GNULIB_FWRITEERROR
/* Like fwriteerror.
   FP must have been returned by fopen_temp, or by fdopen on a file descriptor
   returned by open_temp or gen_register_open_temp.
   Unregisters the previously registered file descriptor.  */
int
fwriteerror_temp (FILE *fp)
{
  return fclose_variant_temp (fp, fwriteerror);
}
#endif

#if GNULIB_CLOSE_STREAM
/* Like close_stream.
   FP must have been returned by fopen_temp, or by fdopen on a file descriptor
   returned by open_temp or gen_register_open_temp.
   Unregisters the previously registered file descriptor.  */
int
close_stream_temp (FILE *fp)
{
  return fclose_variant_temp (fp, close_stream);
}
#endif

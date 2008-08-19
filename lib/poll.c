/* Emulation for poll(2)
   Contributed by Paolo Bonzini.

   Copyright 2001, 2002, 2003, 2006, 2007, 2008 Free Software Foundation, Inc.

   This file is part of gnulib.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <config.h>

#include <sys/types.h>
#include "poll.h"
#include <errno.h>
#include <limits.h>

#ifdef __MSVCRT__
#include <windows.h>
#include <winsock2.h>
#include <io.h>
#include <stdio.h>
#include <conio.h>
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_FILIO_H
#include <sys/filio.h>
#endif

#include <sys/time.h>
#include <time.h>

#ifndef INFTIM
#define INFTIM (-1)
#endif

/* BeOS does not have MSG_PEEK.  */
#ifndef MSG_PEEK
#define MSG_PEEK 0
#endif

#ifdef __MSVCRT__

/* Declare data structures for ntdll functions.  */
typedef struct _FILE_PIPE_LOCAL_INFORMATION {
  ULONG NamedPipeType;
  ULONG NamedPipeConfiguration;
  ULONG MaximumInstances;
  ULONG CurrentInstances;
  ULONG InboundQuota;
  ULONG ReadDataAvailable;
  ULONG OutboundQuota;
  ULONG WriteQuotaAvailable;
  ULONG NamedPipeState;
  ULONG NamedPipeEnd;
} FILE_PIPE_LOCAL_INFORMATION, *PFILE_PIPE_LOCAL_INFORMATION;

typedef struct _IO_STATUS_BLOCK
{
  union u {
    NTSTATUS Status;
    PVOID Pointer;
  };
  ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

#define FilePipeLocalInformation 24

typedef NTSTATUS (NTAPI *PNtQueryInformationFile)
	 (HANDLE, IO_STATUS_BLOCK *, VOID *, ULONG, FILE_INFORMATION_CLASS);

#ifndef PIPE_BUF
#define PIPE_BUF	512
#endif

/* Compute revents values for file handle H.  */

static int
win32_compute_revents (HANDLE h, int sought)
{
  int i, ret, happened;
  INPUT_RECORD *irbuffer;
  DWORD avail, nbuffer;
  IO_STATUS_BLOCK iosb;
  FILE_PIPE_LOCAL_INFORMATION fpli;
  static PNtQueryInformationFile NtQueryInformationFile;

  ret = WaitForSingleObject (h, 0);
  if (ret != WAIT_OBJECT_0)
    return sought & (POLLOUT | POLLWRNORM | POLLWRBAND);

  switch (GetFileType (h))
    {
    case FILE_TYPE_PIPE:
      if (!NtQueryInformationFile)
	NtQueryInformationFile = (PNtQueryInformationFile)
	  GetProcAddress (GetModuleHandle ("ntdll.dll"),
			  "NtQueryInformationFile");

      happened = 0;
      if (!PeekNamedPipe (h, NULL, 0, NULL, &avail, NULL))
	return POLLERR;

      if (avail)
	happened |= sought & (POLLIN | POLLRDNORM);

      memset (&iosb, 0, sizeof (iosb));
      memset (&fpli, 0, sizeof (fpli));

      /* If NtQueryInformationFile fails, optimistically assume the pipe is
	 writable.  This could happen on Win9x, because NtQueryInformationFile
	 is not available, or if we inherit a pipe that doesn't permit
	 FILE_READ_ATTRIBUTES access on the write end (I think this should
	 not happen since WinXP SP2; WINE seems fine too).  Otherwise,
	 ensure that enough space is available for atomic writes.  */
      if (NtQueryInformationFile (h, &iosb, &fpli, sizeof (fpli),
				  FilePipeLocalInformation)
	  || fpli.WriteQuotaAvailable >= PIPE_BUF
	  || (fpli.OutboundQuota < PIPE_BUF &&
	      fpli.WriteQuotaAvailable == fpli.OutboundQuota))
	happened |= sought & (POLLOUT | POLLWRNORM | POLLWRBAND);

      return happened;

    case FILE_TYPE_CHAR:
      nbuffer = avail = 0;
      bRet = GetNumberOfConsoleInputEvents (h, &nbuffer);
      if (!bRet || nbuffer == 0)
        return POLLHUP;

      irbuffer = (INPUT_RECORD *) alloca (nbuffer * sizeof (INPUT_RECORD));
      bRet = PeekConsoleInput (h, irbuffer, nbuffer, &avail);
      if (!bRet || avail == 0)
        return POLLHUP;

      for (i = 0; i < avail; i++)
        if (irbuffer[i].EventType == KEY_EVENT)
          return sought & ~(POLLPRI | POLLRDBAND);

      return sought & (POLLOUT | POLLWRNORM | POLLWRBAND);

    default:
      return sought & ~(POLLPRI | POLLRDBAND);
    }
}

/* Convert fd_sets returned by select into revents values.  */

static int
win32_compute_revents_socket (SOCKET h, int sought,
                              fd_set *rfds, fd_set *wfds, fd_set *efds)
{
  int happened = 0;

  if (FD_ISSET (h, rfds))
    {
      int r, error;

      char data[64];
      WSASetLastError (0);
      r = recv (h, data, sizeof (data), MSG_PEEK);
      error = WSAGetLastError ();
      WSASetLastError (0);

      if (r == 0)
        happened |= POLLHUP;

      /* If the event happened on an unconnected server socket,
         that's fine. */
      else if (r > 0 || ( /* (r == -1) && */ error == ENOTCONN))
        happened |= (POLLIN | POLLRDNORM) & sought;

      /* Distinguish hung-up sockets from other errors.  */
      else if (error == WSAESHUTDOWN || error == WSAECONNRESET
               || error == WSAECONNABORTED || error == WSAENETRESET)
        happened |= POLLHUP;

      else
        happened |= POLLERR;
    }

  if (FD_ISSET (h, wfds))
    happened |= (POLLOUT | POLLWRNORM | POLLWRBAND) & sought;

  if (FD_ISSET (h, efds))
    happened |= (POLLPRI | POLLRDBAND) & sought;

  return happened;
}

#else /* !MinGW */

/* Convert select(2) returned fd_sets into poll(2) revents values.  */
static int
compute_revents (int fd, int sought, fd_set *rfds, fd_set *wfds, fd_set *efds)
{
  int happened;
  if (FD_ISSET (fd, rfds))
    {
      int r;
      int socket_errno;

#if defined __MACH__ && defined __APPLE__
      /* There is a bug in Mac OS X that causes it to ignore MSG_PEEK
         for some kinds of descriptors.  Detect if this descriptor is a
         connected socket, a server socket, or something else using a
         0-byte recv, and use ioctl(2) to detect POLLHUP.  */
      r = recv (fd, NULL, 0, MSG_PEEK);
      socket_errno = (r < 0) ? errno : 0;
      if (r == 0 || socket_errno == ENOTSOCK)
	ioctl (fd, FIONREAD, &r);
#else
      char data[64];
      r = recv (fd, data, sizeof (data), MSG_PEEK);
      socket_errno = (r < 0) ? errno : 0;
#endif
      if (r == 0)
	happened |= POLLHUP;

      /* If the event happened on an unconnected server socket,
         that's fine. */
      else if (r > 0 || ( /* (r == -1) && */ socket_errno == ENOTCONN))
	happened |= (POLLIN | POLLRDNORM) & sought;

      /* Distinguish hung-up sockets from other errors.  */
      else if (socket_errno == ESHUTDOWN || socket_errno == ECONNRESET
	       || socket_errno == ECONNABORTED || socket_errno == ENETRESET)
	happened |= POLLHUP;

      else
	happened |= POLLERR;
    }

  if (FD_ISSET (fd, wfds))
    happened |= (POLLOUT | POLLWRNORM | POLLWRBAND) & sought;

  if (FD_ISSET (fd, efds))
    happened |= (POLLPRI | POLLRDBAND) & sought;

  return happened;
}
#endif /* !MinGW */

int
poll (pfd, nfd, timeout)
     struct pollfd *pfd;
     nfds_t nfd;
     int timeout;
{
#ifndef __MSVCRT__
  fd_set rfds, wfds, efds;
  struct timeval tv;
  struct timeval *ptv;
  int maxfd, rc;
  nfds_t i;

#ifdef _SC_OPEN_MAX
  static int sc_open_max = -1;

  if (nfd < 0
      || (nfd > sc_open_max
          && (sc_open_max != -1
	      || nfd > (sc_open_max = sysconf (_SC_OPEN_MAX)))))
    {
      errno = EINVAL;
      return -1;
    }
#else /* !_SC_OPEN_MAX */
#ifdef OPEN_MAX
  if (nfd < 0 || nfd > OPEN_MAX)
    {
      errno = EINVAL;
      return -1;
    }
#endif /* OPEN_MAX -- else, no check is needed */
#endif /* !_SC_OPEN_MAX */

  /* EFAULT is not necessary to implement, but let's do it in the
     simplest case. */
  if (!pfd)
    {
      errno = EFAULT;
      return -1;
    }

  /* convert timeout number into a timeval structure */
  if (timeout == 0)
    {
      ptv = &tv;
      ptv->tv_sec = 0;
      ptv->tv_usec = 0;
    }
  else if (timeout > 0)
    {
      ptv = &tv;
      ptv->tv_sec = timeout / 1000;
      ptv->tv_usec = (timeout % 1000) * 1000;
    }
  else if (timeout == INFTIM)
    /* wait forever */
    ptv = NULL;
  else
    {
      errno = EINVAL;
      return -1;
    }

  /* create fd sets and determine max fd */
  maxfd = -1;
  FD_ZERO (&rfds);
  FD_ZERO (&wfds);
  FD_ZERO (&efds);
  for (i = 0; i < nfd; i++)
    {
      if (pfd[i].fd < 0)
	continue;

      if (pfd[i].events & (POLLIN | POLLRDNORM))
	FD_SET (pfd[i].fd, &rfds);

      /* see select(2): "the only exceptional condition detectable
         is out-of-band data received on a socket", hence we push
         POLLWRBAND events onto wfds instead of efds. */
      if (pfd[i].events & (POLLOUT | POLLWRNORM | POLLWRBAND))
	FD_SET (pfd[i].fd, &wfds);
      if (pfd[i].events & (POLLPRI | POLLRDBAND))
	FD_SET (pfd[i].fd, &efds);
      if (pfd[i].fd >= maxfd
	  && (pfd[i].events & (POLLIN | POLLOUT | POLLPRI
			       | POLLRDNORM | POLLRDBAND
			       | POLLWRNORM | POLLWRBAND)))
	{
	  maxfd = pfd[i].fd;

	  /* Windows use a linear array of sockets (of size FD_SETSIZE). The
	     descriptor value is not used to address the array.  */
#if defined __CYGWIN__ || (!defined _WIN32 && !defined __WIN32__)
	  if (maxfd > FD_SETSIZE)
	    {
	      errno = EOVERFLOW;
	      return -1;
	    }
#endif
	}
    }

  /* examine fd sets */
  rc = select (maxfd + 1, &rfds, &wfds, &efds, ptv);
  if (rc < 0)
    return rc;

  /* establish results */
  rc = 0;
  for (i = 0; i < nfd; i++)
    if (pfd[i].fd < 0)
      pfd[i].revents = 0;
    else
      {
        int happened = compute_revents (pfd[i].fd, pfd[i].events,
                                        &rfds, &wfds, &efds);
	if (happened)
	  {
	    pfd[i].revents = happened;
	    rc++;
	  }
      }

  return rc;
#else
  fd_set rfds, wfds, efds;
  static struct timeval tv0;
  struct timeval tv = { 0, 0 };
  struct timeval *ptv;
  static HANDLE hEvent;
  HANDLE handle_array[FD_SET_SIZE + 2];
  DWORD ret, wait_timeout, nhandles;
  int nsock;
  BOOL bRet;
  MSG msg;
  char sockbuf[256];
  int rc;
  nfds_t i;

  if (nfd < 0 || nfd > FD_SET_SIZE || timeout < 0)
    {
      errno = EINVAL;
      return -1;
    }

  if (!hEvent)
    hEvent = CreateEvent (NULL, FALSE, FALSE, NULL);

  handle_array[0] = hEvent;
  nhandles = 1;
  nsock = 0;

  /* Classify socket handles and create fd sets. */
  FD_ZERO (&rfds);
  FD_ZERO (&wfds);
  FD_ZERO (&efds);
  for (i = 0; i < nfd; i++)
    {
      if (pfd[i].fd < 0)
        continue;

      h = (HANDLE) _get_osfhandle (i);
      assert (h != NULL);
      optlen = sizeof(sockbuf);
      if ((getsockopt ((SOCKET) h, SOL_SOCKET, SO_TYPE, sockbuf, &optlen)
           != SOCKET_ERROR)
          || WSAGetLastError() != WSAENOTSOCK)
        {
          int ev = 0;

          /* see above; socket handles are mapped onto select.  */
          if (pfd[i].events & (POLLIN | POLLRDNORM))
            {
              FD_SET (pfd[i].fd, &rfds);
              ev |= FD_READ | FD_ACCEPT;
            }
          if (pfd[i].events & (POLLOUT | POLLWRNORM | POLLWRBAND))
            {
              FD_SET (pfd[i].fd, &wfds);
              ev |= FD_WRITE | FD_CONNECT;
            }
          if (pfd[i].events & (POLLPRI | POLLRDBAND))
            {
              FD_SET (pfd[i].fd, &efds);
              ev |= FD_OOB;
            }
          if (ev)
            {
              WSAEventSelect ((SOCKET) h, hEvent, ev);
              nsock++;
            }
        }
      else
        {
          if (pfd[i].events & (POLLIN | POLLRDNORM |
                               POLLOUT | POLLWRNORM | POLLWRBAND))
            handle_array[nhandles++] = h;
        }
    }

  if (timeout == INFTIM)
    wait_timeout = INFINITE;
  else
    wait_timeout = timeout;

  for (;;)
    {
      ret = MsgWaitForMultipleObjects (nhandles, handle_array, FALSE,
				       wait_timeout, QS_ALLINPUT);

      if (ret == WAIT_OBJECT_0 + nhandles)
	{
          /* new input of some other kind */
          while ((bRet = PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) != 0)
            {
              TranslateMessage (&msg);
              DispatchMessage (&msg);
            }
	}
      else
	break;
    }

  /* Now check if the sockets have some event set.  */
  select (nsock + 1, rfds, wfds, efds, &tv0);

  /* Place a sentinel at the end of the array.  */
  handle_array[nhandles] = NULL;
  nhandles = 1;
  for (i = 0; i < nfd; i++)
    {
      int happened;

      if (pfd[i].fd < 0)
        {
          pfd[i].revents = 0;
          continue;
        }

      h = (HANDLE) _get_osfhandle (i);
      if (h != handle_array[nhandles])
        {
          /* It's a socket.  */
          WSAEventSelect (h, 0, 0);
          happened = win32_compute_revents_socket ((SOCKET) h, pfd[i].events,
                                                   &rfds, &wfds, &efds);
        }
      else
        {
          /* Not a socket.  */
          nhandles++;
          happened = win32_compute_revents (h, pfd[i].events);
        }

       if (happened)
        {
          pfd[i].revents = happened;
          rc++;
        }
    }

  return rc;
#endif
}

/* Emulation for poll(2)
   Contributed by Paolo Bonzini.

   Copyright 2001, 2002, 2003 Free Software Foundation, Inc.

   This file is part of gnulib.

   gnulib is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 2.1, or (at your option)
   any later version.

   gnulib is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with gnulib; see the file COPYING.LIB.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.  
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include "poll.h"
#include <errno.h>
#include <limits.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#ifndef INFTIM
#define INFTIM (-1)
#endif

#ifndef EOVERFLOW
#define EOVERFLOW EINVAL
#endif

int
poll (pfd, nfd, timeout)
     struct pollfd *pfd;
     nfds_t nfd;
     int timeout;
{
  fd_set rfds, wfds, efds;
  struct timeval tv, *ptv;
  int maxfd, rc, happened;
  nfds_t i;
  char data[64];

#ifdef _SC_OPEN_MAX
  if (nfd > sysconf (_SC_OPEN_MAX))
    {
      errno = EINVAL;
      return -1;
    }
#else /* !_SC_OPEN_MAX */
#ifdef OPEN_MAX
  if (nfd > OPEN_MAX)
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
  ptv = &tv;
  if (timeout >= 0)
    {
      /* return immediately or after timeout */
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
	  if (maxfd > FD_SETSIZE)
	    {
	      errno = EOVERFLOW;
	      return -1;
	    }
	}
    }

  /* examine fd sets */
  rc = select (maxfd + 1, &rfds, &wfds, &efds, ptv);

  /* establish results */
  if (rc > 0)
    {
      rc = 0;
      for (i = 0; i < nfd; i++)
	{
	  pfd[i].revents = 0;
	  if (pfd[i].fd < 0)
	    continue;

	  happened = 0;
	  if (FD_ISSET (pfd[i].fd, &rfds))
	    {
	      /* support for POLLHUP.  An hung up descriptor does not
		 increase the return value! */
	      if (recv (pfd[i].fd, data, 64, MSG_PEEK) == -1)
		{
		  if (errno == ESHUTDOWN || errno == ECONNRESET
		      || errno == ECONNABORTED || errno == ENETRESET)
		    pfd[i].revents |= POLLHUP;
		}
	      else
		happened |= POLLIN | POLLRDNORM;
	    }

	  if (FD_ISSET (pfd[i].fd, &wfds))
	    happened |= POLLOUT | POLLWRNORM | POLLWRBAND;

	  if (FD_ISSET (pfd[i].fd, &efds))
	    happened |= POLLPRI | POLLRDBAND;

	  pfd[i].revents |= pfd[i].events & happened;
	  rc += (happened > 0);
	}
    }

  return rc;
}

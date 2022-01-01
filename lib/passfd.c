/* Copyright (C) 2011-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "passfd.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/socket.h>

#include "cloexec.h"

/* The code that uses CMSG_FIRSTHDR is enabled on
   Linux, Mac OS X, FreeBSD, OpenBSD, NetBSD, AIX, OSF/1, Cygwin.
   The code that uses HAVE_STRUCT_MSGHDR_MSG_ACCRIGHTS is enabled on
   HP-UX, IRIX, Solaris.  */

/* MSG_CMSG_CLOEXEC is defined only on Linux, as of 2011.  */
#ifndef MSG_CMSG_CLOEXEC
# define MSG_CMSG_CLOEXEC 0
#endif

#if HAVE_SENDMSG
/* sendfd sends the file descriptor fd along the socket
   to a process calling recvfd on the other end.

   Return 0 on success, or -1 with errno set in case of error.
*/
int
sendfd (int sock, int fd)
{
  char byte = 0;
  struct iovec iov;
  struct msghdr msg;
# if defined CMSG_FIRSTHDR && !defined __sgi
  struct cmsghdr *cmsg;
  char buf[CMSG_SPACE (sizeof fd)];
# endif

  /* send at least one char */
  memset (&msg, 0, sizeof msg);
  iov.iov_base = &byte;
  iov.iov_len = 1;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_name = NULL;
  msg.msg_namelen = 0;

# if defined CMSG_FIRSTHDR && !defined __sgi
  msg.msg_control = buf;
  msg.msg_controllen = sizeof buf;
  cmsg = CMSG_FIRSTHDR (&msg);
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;
  cmsg->cmsg_len = CMSG_LEN (sizeof fd);
  /* Initialize the payload: */
  memcpy (CMSG_DATA (cmsg), &fd, sizeof fd);
  msg.msg_controllen = cmsg->cmsg_len;
# elif HAVE_STRUCT_MSGHDR_MSG_ACCRIGHTS
  msg.msg_accrights = &fd;
  msg.msg_accrightslen = sizeof fd;
# else
  errno = ENOSYS;
  return -1;
# endif

  if (sendmsg (sock, &msg, 0) != iov.iov_len)
    return -1;
  return 0;
}
#else
int
sendfd (_GL_UNUSED int sock, _GL_UNUSED int fd)
{
  errno = ENOSYS;
  return -1;
}
#endif


#if HAVE_RECVMSG
/* recvfd receives a file descriptor through the socket.
   The flags are a bitmask, possibly including O_CLOEXEC (defined in <fcntl.h>).

   Return the fd on success, or -1 with errno set in case of error.
*/
int
recvfd (int sock, int flags)
{
  char byte = 0;
  struct iovec iov;
  struct msghdr msg;
  int fd = -1;
  ssize_t len;
# if defined CMSG_FIRSTHDR && !defined __sgi
  struct cmsghdr *cmsg;
  char buf[CMSG_SPACE (sizeof fd)];
  int flags_recvmsg = flags & O_CLOEXEC ? MSG_CMSG_CLOEXEC : 0;
# endif

  if ((flags & ~O_CLOEXEC) != 0)
    {
      errno = EINVAL;
      return -1;
    }

  /* send at least one char */
  memset (&msg, 0, sizeof msg);
  iov.iov_base = &byte;
  iov.iov_len = 1;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_name = NULL;
  msg.msg_namelen = 0;

# if defined CMSG_FIRSTHDR && !defined __sgi
  msg.msg_control = buf;
  msg.msg_controllen = sizeof buf;
  cmsg = CMSG_FIRSTHDR (&msg);
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;
  cmsg->cmsg_len = CMSG_LEN (sizeof fd);
  /* Initialize the payload: */
  memcpy (CMSG_DATA (cmsg), &fd, sizeof fd);
  msg.msg_controllen = CMSG_SPACE (sizeof fd);

  len = recvmsg (sock, &msg, flags_recvmsg);
  if (len < 0)
    return -1;
  if (len == 0)
    {
      /* fake errno: at end the file is not available */
      errno = ENOTCONN;
      return -1;
    }
  cmsg = CMSG_FIRSTHDR (&msg);
  /* be paranoiac */
  if (cmsg == NULL || cmsg->cmsg_len != CMSG_LEN (sizeof fd)
      || cmsg->cmsg_level != SOL_SOCKET || cmsg->cmsg_type != SCM_RIGHTS)
    {
      errno = EACCES;
      return -1;
    }

  memcpy (&fd, CMSG_DATA (cmsg), sizeof fd);

  /* set close-on-exec flag */
  if (!MSG_CMSG_CLOEXEC && (flags & O_CLOEXEC))
    {
      if (set_cloexec_flag (fd, true) < 0)
        {
          int saved_errno = errno;
          (void) close (fd);
          errno = saved_errno;
          return -1;
        }
    }

# elif HAVE_STRUCT_MSGHDR_MSG_ACCRIGHTS
  msg.msg_accrights = &fd;
  msg.msg_accrightslen = sizeof fd;
  if (recvmsg (sock, &msg, 0) < 0)
    return -1;

  /* set close-on-exec flag */
  if (flags & O_CLOEXEC)
    {
      if (set_cloexec_flag (fd, true) < 0)
        {
          int saved_errno = errno;
          close (fd);
          errno = saved_errno;
          return -1;
        }
    }

  if (fd < 0 && errno == 0)
    errno = ENOTCONN;
# else
  errno = ENOSYS;
# endif

  return fd;
}
#else
int
recvfd (_GL_UNUSED int sock, _GL_UNUSED int flags)
{
  errno = ENOSYS;
  return -1;
}
#endif

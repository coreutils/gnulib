/* getdomainname emulation for systems that doesn't have it.

   Copyright (C) 2003, 2006, 2008, 2010-2022 Free Software Foundation, Inc.

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

/* Written by Simon Josefsson.  */

#include <config.h>

/* Specification. */
#include <unistd.h>

#include <limits.h>
#include <string.h>
#include <errno.h>

#if HAVE_SYSINFO && HAVE_SYS_SYSTEMINFO_H /* IRIX, OSF/1, Solaris */
# include <sys/systeminfo.h>
#endif

/* Return the NIS domain name of the machine.
   WARNING! The NIS domain name is unrelated to the fully qualified host name
            of the machine.  It is also unrelated to email addresses.
   WARNING! The NIS domain name is usually the empty string or "(none)" when
            not using NIS.

   Put up to LEN bytes of the NIS domain name into NAME.
   Null terminate it if the name is shorter than LEN.
   If the NIS domain name is longer than LEN, set errno = EINVAL and return -1.
   Return 0 if successful, otherwise set errno and return -1.  */
int
getdomainname (char *name, size_t len)
#undef getdomainname
{
#if HAVE_GETDOMAINNAME                 /* Mac OS X, FreeBSD, AIX, IRIX, OSF/1 */
  extern int getdomainname (char *, int);

  if (len > INT_MAX)
    len = INT_MAX;
  return getdomainname (name, (int) len);
#elif HAVE_SYSINFO && HAVE_SYS_SYSTEMINFO_H && defined SI_SRPC_DOMAIN
                                       /* Solaris */
  int ret;

  /* The third argument is a 'long', but the return value must fit in an
     'int', therefore it's better to avoid arguments > INT_MAX.  */
  ret = sysinfo (SI_SRPC_DOMAIN, name, len > INT_MAX ? INT_MAX : len);
  if (ret < 0)
    /* errno is set here.  */
    return -1;
  if (ret > len)
    {
      errno = EINVAL;
      return -1;
    }
  return 0;
#else                                  /* HP-UX, Cygwin, mingw */
  const char *result = "";      /* Hardcode your domain name if you want.  */
  size_t result_len = strlen (result);

  if (result_len > len)
    {
      errno = EINVAL;
      return -1;
    }
  memcpy (name, result, result_len);
  if (result_len < len)
    name[result_len] = '\0';
  return 0;
#endif
}

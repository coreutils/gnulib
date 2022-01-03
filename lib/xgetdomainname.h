/* xgetdomainname.h -- Return the NIS domain name, without size limitations.
   Copyright (C) 1992, 1996, 2000-2001, 2003, 2009-2022 Free Software
   Foundation, Inc.

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

#ifndef _XGETDOMAINNAME_H
#define _XGETDOMAINNAME_H

#include <stdlib.h>

/* Return the NIS domain name of the machine, in malloc'd storage.
   WARNING! The NIS domain name is unrelated to the fully qualified host name
            of the machine.  It is also unrelated to email addresses.
   WARNING! The NIS domain name is usually the empty string or "(none)" when
            not using NIS.
   If malloc fails, exit.
   Upon any other failure, set errno and return NULL.  */
extern char *xgetdomainname (void)
  _GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_DEALLOC_FREE;

#endif /* _XGETDOMAINNAME_H */

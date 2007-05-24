/* An ftello() function that works around platform bugs.
   Copyright (C) 2007 Free Software Foundation, Inc.

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

/* Specification.  */
#include <stdio.h>

#undef ftello
#if !HAVE_FTELLO
# define ftello ftell
#endif

off_t
rpl_ftello (FILE *fp)
{
#if defined __SL64 && defined __SCLE /* Cygwin */
  if ((fp->_flags & __SL64) == 0)
    {
      /* Cygwin 1.5.0 through 1.5.24 failed to open stdin in 64-bit
	 mode; but has an ftello that requires 64-bit mode.  */
      FILE *tmp = fopen ("/dev/null", "r");
      if (!tmp)
	return -1;
      fp->_flags |= __SL64;
      fp->_seek64 = tmp->_seek64;
      fclose (tmp);
    }
#endif
  return ftello (fp);
}

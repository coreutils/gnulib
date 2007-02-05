/* Searching a string for a character among a given set of characters.
   Copyright (C) 1999, 2002, 2006-2007 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2007.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <config.h>

/* Specification.  */
#include <string.h>

#if HAVE_MBRTOWC
# include "mbuiter.h"
#endif

/* Find the first occurrence in the character string STRING of any character
   in the character string ACCEPT.  Return the pointer to it, or NULL if none
   exists.  */
char *
mbspbrk (const char *string, const char *accept)
{
  /* Optimize two cases.  */
  if (accept[0] == '\0')
    return NULL;
  if (accept[1] == '\0')
    return mbschr (string, accept[0]);
  /* General case.  */
#if HAVE_MBRTOWC
  if (MB_CUR_MAX > 1)
    {
      mbui_iterator_t iter;

      for (mbui_init (iter, string); mbui_avail (iter); mbui_advance (iter))
	{
	  if (mb_len (mbui_cur (iter)) == 1)
	    {
	      if (mbschr (accept, (unsigned char) * mbui_cur_ptr (iter)))
		return (char *) mbui_cur_ptr (iter);
	    }
	  else
	    {
	      mbui_iterator_t aiter;

	      for (mbui_init (aiter, accept);
		   mbui_avail (aiter);
		   mbui_advance (aiter))
		if (mb_equal (mbui_cur (aiter), mbui_cur (iter)))
		  return (char *) mbui_cur_ptr (iter);
	    }
	}
      return NULL;
    }
  else
#endif
    return strpbrk (string, accept);
}

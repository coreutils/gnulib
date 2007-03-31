/* Character set conversion.
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
#include <iconv.h>

#include <errno.h>
#include <string.h>
#include "c-ctype.h"

#define SIZEOF(a) (sizeof(a) / sizeof(a[0]))

/* Namespace cleanliness.  */
#define mapping_lookup rpl_iconv_open_mapping_lookup

/* The macro ICONV_FLAVOR is defined to one of these.  */

#define ICONV_FLAVOR_AIX "iconv_open-aix.h"
#define ICONV_FLAVOR_HPUX "iconv_open-hpux.h"
#define ICONV_FLAVOR_IRIX "iconv_open-irix.h"
#define ICONV_FLAVOR_OSF "iconv_open-osf.h"

#include ICONV_FLAVOR

iconv_t
rpl_iconv_open (const char *tocode, const char *fromcode)
#undef iconv_open
{
  char fromcode_upper[32];
  char tocode_upper[32];
  char *fromcode_upper_end;
  char *tocode_upper_end;

  /* Try with the original names first.
     This covers the case when fromcode or tocode is a lowercase encoding name
     that is understood by the system's iconv_open but not listed in our
     mappings table.  */
  {
    iconv_t cd = iconv_open (tocode, fromcode);
    if (cd != (iconv_t)(-1))
      return cd;
  }

  /* Convert the encodings to upper case, because
       1. in the arguments of iconv_open() on AIX, HP-UX, and OSF/1 the case
	  matters,
       2. it makes searching in the table faster.  */
  {
    const char *p = fromcode;
    char *q = fromcode_upper;
    while ((*q = c_toupper (*p)) != '\0')
      {
	p++;
	q++;
	if (q == &fromcode_upper[SIZEOF (fromcode_upper)])
	  {
	    errno = EINVAL;
	    return (iconv_t)(-1);
	  }
      }
    fromcode_upper_end = q;
  }

  {
    const char *p = tocode;
    char *q = tocode_upper;
    while ((*q = c_toupper (*p)) != '\0')
      {
	p++;
	q++;
	if (q == &tocode_upper[SIZEOF (tocode_upper)])
	  {
	    errno = EINVAL;
	    return (iconv_t)(-1);
	  }
      }
    tocode_upper_end = q;
  }

  /* Apply the mappings.  */
  {
    const struct mapping *m =
      mapping_lookup (fromcode_upper, fromcode_upper_end - fromcode_upper);

    fromcode = (m != NULL ? m->vendor_name : fromcode_upper);
  }
  {
    const struct mapping *m =
      mapping_lookup (tocode_upper, tocode_upper_end - tocode_upper);

    tocode = (m != NULL ? m->vendor_name : tocode_upper);
  }

  return iconv_open (tocode, fromcode);
}

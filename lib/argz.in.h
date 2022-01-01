/* Routines for dealing with '\0' separated arg vectors.
   Copyright (C) 1995-2000, 2004, 2007, 2009-2022 Free Software Foundation,
   Inc.
   This file is part of the GNU C Library.

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

#ifndef _ARGZ_H
#define _ARGZ_H 1


#define __need_error_t
#include <errno.h>
#include <string.h>             /* Need size_t.  */

#ifndef __error_t_defined
typedef int error_t;
#endif



/* Make a '\0' separated arg vector from a unix argv vector, returning it in
   ARGZ, and the total length in LEN.  If a memory allocation error occurs,
   ENOMEM is returned, otherwise 0.  The result can be destroyed using free.  */

extern error_t argz_create (char *const /*argv*/[], char **restrict /*argz*/,
                            size_t *restrict /*len*/);

/* Make a '\0' separated arg vector from a SEP separated list in
   STRING, returning it in ARGZ, and the total length in LEN.  If a
   memory allocation error occurs, ENOMEM is returned, otherwise 0.
   The result can be destroyed using free.  */

extern error_t argz_create_sep (const char *restrict /*string*/,
                                int /*sep*/, char **restrict /*argz*/,
                                size_t *restrict /*len*/);

/* Returns the number of strings in ARGZ.  */

extern size_t argz_count (const char * /*argz*/, size_t /*len*/);

/* Puts pointers to each string in ARGZ into ARGV, which must be large enough
   to hold them all.  */

extern void argz_extract (const char *restrict /*argz*/, size_t /*len*/,
                          char **restrict /*argv*/);

/* Make '\0' separated arg vector ARGZ printable by converting all the '\0's
   except the last into the character SEP.  */

extern void argz_stringify (char * /*argz*/, size_t /*len*/, int /*sep*/);

/* Append BUF, of length BUF_LEN to the argz vector in ARGZ & ARGZ_LEN.  */

extern error_t argz_append (char **restrict /*argz*/,
                            size_t *restrict /*argz_len*/,
                            const char *restrict /*buf*/, size_t /*buf_len*/);

/* Append STR to the argz vector in ARGZ & ARGZ_LEN.  */

extern error_t argz_add (char **restrict /*argz*/,
                         size_t *restrict /*argz_len*/,
                         const char *restrict str);

/* Append SEP separated list in STRING to the argz vector in ARGZ &
   ARGZ_LEN.  */

extern error_t argz_add_sep (char **restrict /*argz*/,
                             size_t *restrict /*argz_len*/,
                             const char *restrict /*string*/, int /*delim*/);

/* Delete ENTRY from ARGZ & ARGZ_LEN, if it appears there.  */

extern void argz_delete (char **restrict /*argz*/,
                         size_t *restrict /*argz_len*/,
                         char *restrict /*entry*/);

/* Insert ENTRY into ARGZ & ARGZ_LEN before BEFORE, which should be an
   existing entry in ARGZ; if BEFORE is NULL, ENTRY is appended to the end.
   Since ARGZ's first entry is the same as ARGZ, argz_insert (ARGZ, ARGZ_LEN,
   ARGZ, ENTRY) will insert ENTRY at the beginning of ARGZ.  If BEFORE is not
   in ARGZ, EINVAL is returned, else if memory can't be allocated for the new
   ARGZ, ENOMEM is returned, else 0.  */

extern error_t argz_insert (char **restrict /*argz*/,
                            size_t *restrict /*argz_len*/,
                            char *restrict /*before*/,
                            const char *restrict /*entry*/);

/* Replace any occurrences of the string STR in ARGZ with WITH, reallocating
   ARGZ as necessary.  If REPLACE_COUNT is non-zero, *REPLACE_COUNT will be
   incremented by number of replacements performed.  */

extern error_t argz_replace (char **restrict /*argz*/,
                             size_t *restrict /*argz_len*/,
                             const char *restrict /*str*/,
                             const char *restrict /*with*/,
                             unsigned int *restrict /*replace_count*/);

/* Returns the next entry in ARGZ & ARGZ_LEN after ENTRY, or NULL if there
   are no more.  If entry is NULL, then the first entry is returned.  This
   behavior allows two convenient iteration styles:

    char *entry = 0;
    while ((entry = argz_next (argz, argz_len, entry)))
      ...;

   or

    char *entry;
    for (entry = argz; entry; entry = argz_next (argz, argz_len, entry))
      ...;
*/

extern char *argz_next (const char *restrict /*argz*/, size_t /*argz_len*/,
                        const char *restrict /*entry*/);


#endif /* argz.h */

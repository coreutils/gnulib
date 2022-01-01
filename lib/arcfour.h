/* arcfour.h --- The arcfour stream cipher
 * Copyright (C) 2000-2005, 2009-2022 Free Software Foundation, Inc.
 *
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/* Code from Libgcrypt adapted for gnulib by Simon Josefsson. */

#ifndef ARCFOUR_H
# define ARCFOUR_H

# include <stddef.h>
# include <stdint.h>

#define ARCFOUR_SBOX_SIZE 256

typedef struct
{
  char sbox[ARCFOUR_SBOX_SIZE];
  uint8_t idx_i, idx_j;
} arcfour_context;

/* Apply ARCFOUR stream to INBUF placing the result in OUTBUF, both of
   LENGTH size.  CONTEXT must be initialized with arcfour_setkey
   before this function is called. */
extern void
arcfour_stream (arcfour_context * context,
                const char *inbuf, char *restrict outbuf, size_t length);

/* Initialize CONTEXT using encryption KEY of KEYLEN bytes.  KEY
   should be 40 bits (5 bytes) or longer.  The KEY cannot be zero
   length.  */
extern void
arcfour_setkey (arcfour_context * context, const char *key, size_t keylen);

#endif /* ARCFOUR_H */

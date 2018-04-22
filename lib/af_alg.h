/* af_alg.h - Declaration of functions to compute message digest from
   file streams using Linux kernel crypto API.
   Copyright (C) 2018 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <https://www.gnu.org/licenses/>.  */

/* Written by Matteo Croce <mcroce@redhat.com>, 2018. */

#ifndef AF_ALG_H
# define AF_ALG_H 1

# include <stdio.h>
# include <errno.h>

# ifdef __cplusplus
extern "C" {
# endif

# ifdef HAVE_LINUX_IF_ALG_H

int
afalg_stream (FILE * stream, void *resblock, const char *alg, ssize_t hashlen);

# else

static int
afalg_stream (FILE * stream, void *resblock, const char *alg, ssize_t hashlen)
{
  return -EAFNOSUPPORT;
}

# endif

# ifdef __cplusplus
}
# endif

#endif

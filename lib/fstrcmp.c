/* Functions to make fuzzy comparisons between strings
   Copyright (C) 1988-1989, 1992-1993, 1995, 2001-2003, 2006 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.


   Derived from GNU diff 2.7, analyze.c et al.

   The basic idea is to consider two vectors as similar if, when
   transforming the first vector into the second vector through a
   sequence of edits (inserts and deletes of one element each),
   this sequence is short - or equivalently, if the ordered list
   of elements that are untouched by these edits is long.  For a
   good introduction to the subject, read about the "Levenshtein
   distance" in Wikipedia.

   The basic algorithm is described in:
   "An O(ND) Difference Algorithm and its Variations", Eugene Myers,
   Algorithmica Vol. 1 No. 2, 1986, pp. 251-266;
   see especially section 4.2, which describes the variation used below.

   The basic algorithm was independently discovered as described in:
   "Algorithms for Approximate String Matching", E. Ukkonen,
   Information and Control Vol. 64, 1985, pp. 100-118.

   Unless the 'find_minimal' flag is set, this code uses the TOO_EXPENSIVE
   heuristic, by Paul Eggert, to limit the cost to O(N**1.5 log N)
   at the price of producing suboptimal output for large inputs with
   many differences.  */

#include <config.h>

/* Specification.  */
#include "fstrcmp.h"

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "lock.h"
#include "tls.h"
#include "minmax.h"
#include "xalloc.h"

#ifndef uintptr_t
# define uintptr_t unsigned long
#endif


#define ELEMENT char
#define EQUAL(x,y) ((x) == (y))
#define OFFSET int
#define EXTRA_CONTEXT_FIELDS \
  /* The number of elements inserted or deleted. */ \
  int xvec_edit_count; \
  int yvec_edit_count;
#define NOTE_DELETE(ctxt, xoff) ctxt->xvec_edit_count++
#define NOTE_INSERT(ctxt, yoff) ctxt->yvec_edit_count++
/* We don't need USE_HEURISTIC, since it is unlikely in typical uses of
   fstrcmp().  */
#include "diffseq.h"


/* Because fstrcmp is typically called multiple times, attempt to minimize
   the number of memory allocations performed.  Thus, let a call reuse the
   memory already allocated by the previous call, if it is sufficient.
   To make it multithread-safe, without need for a lock that protects the
   already allocated memory, store the allocated memory per thread.  Free
   it only when the thread exits.  */

static gl_tls_key_t buffer_key;	/* TLS key for a 'int *' */
static gl_tls_key_t bufmax_key;	/* TLS key for a 'size_t' */

static void
keys_init (void)
{
  gl_tls_key_init (buffer_key, free);
  gl_tls_key_init (bufmax_key, NULL);
  /* The per-thread initial values are NULL and 0, respectively.  */
}

/* Ensure that keys_init is called once only.  */
gl_once_define(static, keys_init_once)


/* NAME
	fstrcmp - fuzzy string compare

   SYNOPSIS
	double fstrcmp(const char *, const char *);

   DESCRIPTION
	The fstrcmp function may be used to compare two string for
	similarity.  It is very useful in reducing "cascade" or
	"secondary" errors in compilers or other situations where
	symbol tables occur.

   RETURNS
	double; 0 if the strings are entirly dissimilar, 1 if the
	strings are identical, and a number in between if they are
	similar.  */

double
fstrcmp (const char *string1, const char *string2)
{
  struct context ctxt;
  int xvec_length;
  int yvec_length;
  int i;

  size_t fdiag_len;
  int *buffer;
  size_t bufmax;

  /* set the info for each string.  */
  ctxt.xvec = string1;
  xvec_length = strlen (string1);
  ctxt.yvec = string2;
  yvec_length = strlen (string2);

  /* short-circuit obvious comparisons */
  if (xvec_length == 0 && yvec_length == 0)
    return 1.0;
  if (xvec_length == 0 || yvec_length == 0)
    return 0.0;

  /* Set TOO_EXPENSIVE to be approximate square root of input size,
     bounded below by 256.  */
  ctxt.too_expensive = 1;
  for (i = xvec_length + yvec_length;
       i != 0;
       i >>= 2)
    ctxt.too_expensive <<= 1;
  if (ctxt.too_expensive < 256)
    ctxt.too_expensive = 256;

  /* Allocate memory for fdiag and bdiag from a thread-local pool.  */
  fdiag_len = xvec_length + yvec_length + 3;
  gl_once (keys_init_once, keys_init);
  buffer = (int *) gl_tls_get (buffer_key);
  bufmax = (size_t) (uintptr_t) gl_tls_get (bufmax_key);
  if (fdiag_len > bufmax)
    {
      /* Need more memory.  */
      bufmax = 2 * bufmax;
      if (fdiag_len > bufmax)
	bufmax = fdiag_len;
      /* Calling xrealloc would be a waste: buffer's contents does not need
	 to be preserved.  */
      if (buffer != NULL)
	free (buffer);
      buffer = (int *) xnmalloc (bufmax, 2 * sizeof (int));
      gl_tls_set (buffer_key, buffer);
      gl_tls_set (bufmax_key, (void *) (uintptr_t) bufmax);
    }
  ctxt.fdiag = buffer + yvec_length + 1;
  ctxt.bdiag = ctxt.fdiag + fdiag_len;

  /* Now do the main comparison algorithm */
  ctxt.xvec_edit_count = 0;
  ctxt.yvec_edit_count = 0;
  compareseq (0, xvec_length, 0, yvec_length, 0,
	      &ctxt);

  /* The result is
	((number of chars in common) / (average length of the strings)).
     This is admittedly biased towards finding that the strings are
     similar, however it does produce meaningful results.  */
  return ((double) (xvec_length + yvec_length
		    - ctxt.yvec_edit_count - ctxt.xvec_edit_count)
	  / (xvec_length + yvec_length));
}

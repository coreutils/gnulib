/* Analyze differences between two vectors.

   Copyright (C) 1988-1989, 1992-1995, 2001-2004, 2006, 2007 Free
   Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */


/* The basic idea is to consider two vectors as similar if, when
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

/* Before including this file, you need to define:
     ELEMENT                 The element type of the vectors being compared.
     EQUAL                   A two-argument macro that tests two elements for
                             equality.
     OFFSET                  A signed integer type sufficient to hold the
                             difference between two indices. Usually
                             something like ssize_t.
     EXTRA_CONTEXT_FIELDS    Declarations of fields for 'struct context'.
     NOTE_DELETE(ctxt, xoff) Record the removal of the object xvec[xoff].
     NOTE_INSERT(ctxt, yoff) Record the insertion of the object yvec[yoff].
     USE_HEURISTIC           (Optional) Define if you want to support the
                             heuristic for large vectors.  */

/* Maximum value of type OFFSET.  */
#define OFFSET_MAX \
  ((((OFFSET)1 << (sizeof (OFFSET) * CHAR_BIT - 2)) - 1) * 2 + 1)

/* Use this to suppress gcc's `...may be used before initialized' warnings. */
#ifndef IF_LINT
# ifdef lint
#  define IF_LINT(Code) Code
# else
#  define IF_LINT(Code) /* empty */
# endif
#endif

/*
 * Context of comparison operation.
 */
struct context
{
  /* Vectors being compared.  */
  const ELEMENT *xvec;
  const ELEMENT *yvec;

  /* Extra fields.  */
  EXTRA_CONTEXT_FIELDS

  /* Vector, indexed by diagonal, containing 1 + the X coordinate of the point
     furthest along the given diagonal in the forward search of the edit
     matrix.  */
  OFFSET *fdiag;

  /* Vector, indexed by diagonal, containing the X coordinate of the point
     furthest along the given diagonal in the backward search of the edit
     matrix.  */
  OFFSET *bdiag;

  #ifdef USE_HEURISTIC
  /* This corresponds to the diff -H flag.  With this heuristic, for
     vectors with a constant small density of changes, the algorithm is
     linear in the vectors size.  */
  bool heuristic;
  #endif

  /* Edit scripts longer than this are too expensive to compute.  */
  OFFSET too_expensive;

  /* Snakes bigger than this are considered `big'.  */
  #define SNAKE_LIMIT 20
};

struct partition
{
  /* Midpoints of this partition.  */
  OFFSET xmid;
  OFFSET ymid;

  /* True if low half will be analyzed minimally.  */
  bool lo_minimal;

  /* Likewise for high half.  */
  bool hi_minimal;
};


/* Find the midpoint of the shortest edit script for a specified portion
   of the two vectors.

   Scan from the beginnings of the vectors, and simultaneously from the ends,
   doing a breadth-first search through the space of edit-sequence.
   When the two searches meet, we have found the midpoint of the shortest
   edit sequence.

   If FIND_MINIMAL is true, find the minimal edit script regardless of
   expense.  Otherwise, if the search is too expensive, use heuristics to
   stop the search and report a suboptimal answer.

   Set PART->(xmid,ymid) to the midpoint (XMID,YMID).  The diagonal number
   XMID - YMID equals the number of inserted elements minus the number
   of deleted elements (counting only elements before the midpoint).

   Set PART->lo_minimal to true iff the minimal edit script for the
   left half of the partition is known; similarly for PART->hi_minimal.

   This function assumes that the first elements of the specified portions
   of the two vectors do not match, and likewise that the last elements do not
   match.  The caller must trim matching elements from the beginning and end
   of the portions it is going to specify.

   If we return the "wrong" partitions, the worst this can do is cause
   suboptimal diff output.  It cannot cause incorrect diff output.  */

static void
diag (OFFSET xoff, OFFSET xlim, OFFSET yoff, OFFSET ylim, bool find_minimal,
      struct partition *part, struct context *ctxt)
{
  OFFSET *const fd = ctxt->fdiag;	/* Give the compiler a chance. */
  OFFSET *const bd = ctxt->bdiag;	/* Additional help for the compiler. */
  const ELEMENT *const xv = ctxt->xvec;	/* Still more help for the compiler. */
  const ELEMENT *const yv = ctxt->yvec;	/* And more and more . . . */
  const OFFSET dmin = xoff - ylim;	/* Minimum valid diagonal. */
  const OFFSET dmax = xlim - yoff;	/* Maximum valid diagonal. */
  const OFFSET fmid = xoff - yoff;	/* Center diagonal of top-down search. */
  const OFFSET bmid = xlim - ylim;	/* Center diagonal of bottom-up search. */
  OFFSET fmin = fmid;
  OFFSET fmax = fmid;		/* Limits of top-down search. */
  OFFSET bmin = bmid;
  OFFSET bmax = bmid;		/* Limits of bottom-up search. */
  OFFSET c;			/* Cost. */
  bool odd = (fmid - bmid) & 1;	/* True if southeast corner is on an odd
				   diagonal with respect to the northwest. */

  fd[fmid] = xoff;
  bd[bmid] = xlim;

  for (c = 1;; ++c)
    {
      OFFSET d;			/* Active diagonal. */
      bool big_snake = false;

      /* Extend the top-down search by an edit step in each diagonal. */
      if (fmin > dmin)
	fd[--fmin - 1] = -1;
      else
	++fmin;
      if (fmax < dmax)
	fd[++fmax + 1] = -1;
      else
	--fmax;
      for (d = fmax; d >= fmin; d -= 2)
	{
	  OFFSET x;
	  OFFSET y;
	  OFFSET tlo = fd[d - 1];
	  OFFSET thi = fd[d + 1];
	  OFFSET x0 = tlo < thi ? thi : tlo + 1;

	  for (x = x0, y = x0 - d;
	       x < xlim && y < ylim && EQUAL (xv[x], yv[y]);
	       x++, y++)
	    continue;
	  if (x - x0 > SNAKE_LIMIT)
	    big_snake = true;
	  fd[d] = x;
	  if (odd && bmin <= d && d <= bmax && bd[d] <= x)
	    {
	      part->xmid = x;
	      part->ymid = y;
	      part->lo_minimal = part->hi_minimal = true;
	      return;
	    }
	}

      /* Similarly extend the bottom-up search.  */
      if (bmin > dmin)
	bd[--bmin - 1] = OFFSET_MAX;
      else
	++bmin;
      if (bmax < dmax)
	bd[++bmax + 1] = OFFSET_MAX;
      else
	--bmax;
      for (d = bmax; d >= bmin; d -= 2)
	{
	  OFFSET x;
	  OFFSET y;
	  OFFSET tlo = bd[d - 1];
	  OFFSET thi = bd[d + 1];
	  OFFSET x0 = tlo < thi ? tlo : thi - 1;

	  for (x = x0, y = x0 - d;
	       xoff < x && yoff < y && EQUAL (xv[x - 1], yv[y - 1]);
	       x--, y--)
	    continue;
	  if (x0 - x > SNAKE_LIMIT)
	    big_snake = true;
	  bd[d] = x;
	  if (!odd && fmin <= d && d <= fmax && x <= fd[d])
	    {
	      part->xmid = x;
	      part->ymid = y;
	      part->lo_minimal = part->hi_minimal = true;
	      return;
	    }
	}

      if (find_minimal)
	continue;

#ifdef USE_HEURISTIC
      /* Heuristic: check occasionally for a diagonal that has made lots
	 of progress compared with the edit distance.  If we have any
	 such, find the one that has made the most progress and return it
	 as if it had succeeded.

	 With this heuristic, for vectors with a constant small density
	 of changes, the algorithm is linear in the vector size.  */

      if (200 < c && big_snake && ctxt->heuristic)
	{
	  OFFSET best = 0;

	  for (d = fmax; d >= fmin; d -= 2)
	    {
	      OFFSET dd = d - fmid;
	      OFFSET x = fd[d];
	      OFFSET y = x - d;
	      OFFSET v = (x - xoff) * 2 - dd;

	      if (v > 12 * (c + (dd < 0 ? -dd : dd)))
		{
		  if (v > best
		      && xoff + SNAKE_LIMIT <= x && x < xlim
		      && yoff + SNAKE_LIMIT <= y && y < ylim)
		    {
		      /* We have a good enough best diagonal; now insist
			 that it end with a significant snake.  */
		      int k;

		      for (k = 1; EQUAL (xv[x - k], yv[y - k]); k++)
			if (k == SNAKE_LIMIT)
			  {
			    best = v;
			    part->xmid = x;
			    part->ymid = y;
			    break;
			  }
		    }
		}
	    }
	  if (best > 0)
	    {
	      part->lo_minimal = true;
	      part->hi_minimal = false;
	      return;
	    }

	  best = 0;
	  for (d = bmax; d >= bmin; d -= 2)
	    {
	      OFFSET dd = d - bmid;
	      OFFSET x = bd[d];
	      OFFSET y = x - d;
	      OFFSET v = (xlim - x) * 2 + dd;

	      if (v > 12 * (c + (dd < 0 ? -dd : dd)))
		{
		  if (v > best
		      && xoff < x && x <= xlim - SNAKE_LIMIT
		      && yoff < y && y <= ylim - SNAKE_LIMIT)
		    {
		      /* We have a good enough best diagonal; now insist
			 that it end with a significant snake.  */
		      int k;

		      for (k = 0; EQUAL (xv[x + k], yv[y + k]); k++)
			if (k == SNAKE_LIMIT - 1)
			  {
			    best = v;
			    part->xmid = x;
			    part->ymid = y;
			    break;
			  }
		    }
		}
	    }
	  if (best > 0)
	    {
	      part->lo_minimal = false;
	      part->hi_minimal = true;
	      return;
	    }
	}
#endif /* USE_HEURISTIC */

      /* Heuristic: if we've gone well beyond the call of duty, give up
	 and report halfway between our best results so far.  */
      if (c >= ctxt->too_expensive)
	{
	  OFFSET fxybest;
	  OFFSET fxbest IF_LINT (= 0);
	  OFFSET bxybest;
	  OFFSET bxbest IF_LINT (= 0);

	  /* Find forward diagonal that maximizes X + Y.  */
	  fxybest = -1;
	  for (d = fmax; d >= fmin; d -= 2)
	    {
	      OFFSET x = MIN (fd[d], xlim);
	      OFFSET y = x - d;
	      if (ylim < y)
		{
		  x = ylim + d;
		  y = ylim;
		}
	      if (fxybest < x + y)
		{
		  fxybest = x + y;
		  fxbest = x;
		}
	    }

	  /* Find backward diagonal that minimizes X + Y.  */
	  bxybest = OFFSET_MAX;
	  for (d = bmax; d >= bmin; d -= 2)
	    {
	      OFFSET x = MAX (xoff, bd[d]);
	      OFFSET y = x - d;
	      if (y < yoff)
		{
		  x = yoff + d;
		  y = yoff;
		}
	      if (x + y < bxybest)
		{
		  bxybest = x + y;
		  bxbest = x;
		}
	    }

	  /* Use the better of the two diagonals.  */
	  if ((xlim + ylim) - bxybest < fxybest - (xoff + yoff))
	    {
	      part->xmid = fxbest;
	      part->ymid = fxybest - fxbest;
	      part->lo_minimal = true;
	      part->hi_minimal = false;
	    }
	  else
	    {
	      part->xmid = bxbest;
	      part->ymid = bxybest - bxbest;
	      part->lo_minimal = false;
	      part->hi_minimal = true;
	    }
	  return;
	}
    }
}


/* Compare in detail contiguous subsequences of the two vectors
   which are known, as a whole, to match each other.

   The subsequence of vector 0 is [XOFF, XLIM) and likewise for vector 1.

   Note that XLIM, YLIM are exclusive bounds.  All indices into the vectors
   are origin-0.

   If FIND_MINIMAL, find a minimal difference no matter how
   expensive it is.

   The results are recorded by invoking NOTE_DELETE and NOTE_INSERT.  */

static void
compareseq (OFFSET xoff, OFFSET xlim, OFFSET yoff, OFFSET ylim,
	    bool find_minimal, struct context *ctxt)
{
  ELEMENT const *xv = ctxt->xvec; /* Help the compiler.  */
  ELEMENT const *yv = ctxt->yvec;

  /* Slide down the bottom initial diagonal.  */
  while (xoff < xlim && yoff < ylim && EQUAL (xv[xoff], yv[yoff]))
    {
      xoff++;
      yoff++;
    }

  /* Slide up the top initial diagonal. */
  while (xoff < xlim && yoff < ylim && EQUAL (xv[xlim - 1], yv[ylim - 1]))
    {
      xlim--;
      ylim--;
    }

  /* Handle simple cases. */
  if (xoff == xlim)
    while (yoff < ylim)
      {
	NOTE_INSERT (ctxt, yoff);
	yoff++;
      }
  else if (yoff == ylim)
    while (xoff < xlim)
      {
	NOTE_DELETE (ctxt, xoff);
	xoff++;
      }
  else
    {
      struct partition part;

      /* Find a point of correspondence in the middle of the vectors.  */
      diag (xoff, xlim, yoff, ylim, find_minimal, &part, ctxt);

      /* Use the partitions to split this problem into subproblems.  */
      compareseq (xoff, part.xmid, yoff, part.ymid, part.lo_minimal, ctxt);
      compareseq (part.xmid, xlim, part.ymid, ylim, part.hi_minimal, ctxt);
    }
}

#undef ELEMENT
#undef EQUAL
#undef OFFSET
#undef EXTRA_CONTEXT_FIELDS
#undef NOTE_DELETE
#undef NOTE_INSERT
#undef USE_HEURISTIC
#undef OFFSET_MAX

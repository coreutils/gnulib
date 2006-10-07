/* Analyze differences between two sequences.
   Copyright (C) 1988-1989, 1992-1995, 2001-2004, 2006 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


/* The basic algorithm is described in:
   "An O(ND) Difference Algorithm and its Variations", Eugene Myers,
   Algorithmica Vol. 1 No. 2, 1986, pp. 251-266;
   see especially section 4.2, which describes the variation used below.
   Unless the --minimal option is specified, this code uses the TOO_EXPENSIVE
   heuristic, by Paul Eggert, to limit the cost to O(N**1.5 log N)
   at the price of producing suboptimal output for large inputs with
   many differences.

   The basic algorithm was independently discovered as described in:
   "Algorithms for Approximate String Matching", E. Ukkonen,
   Information and Control Vol. 64, 1985, pp. 100-118.  */

static lin *xvec, *yvec;	/* Vectors being compared. */
static lin *fdiag;		/* Vector, indexed by diagonal, containing
				   1 + the X coordinate of the point furthest
				   along the given diagonal in the forward
				   search of the edit matrix. */
static lin *bdiag;		/* Vector, indexed by diagonal, containing
				   the X coordinate of the point furthest
				   along the given diagonal in the backward
				   search of the edit matrix. */
static lin too_expensive;	/* Edit scripts longer than this are too
				   expensive to compute.  */

#define SNAKE_LIMIT 20	/* Snakes bigger than this are considered `big'.  */

struct partition
{
  lin xmid, ymid;	/* Midpoints of this partition.  */
  bool lo_minimal;	/* Nonzero if low half will be analyzed minimally.  */
  bool hi_minimal;	/* Likewise for high half.  */
};

/* Find the midpoint of the shortest edit script for a specified
   portion of the two files.

   Scan from the beginnings of the files, and simultaneously from the ends,
   doing a breadth-first search through the space of edit-sequence.
   When the two searches meet, we have found the midpoint of the shortest
   edit sequence.

   If FIND_MINIMAL is nonzero, find the minimal edit script regardless
   of expense.  Otherwise, if the search is too expensive, use
   heuristics to stop the search and report a suboptimal answer.

   Set PART->(xmid,ymid) to the midpoint (XMID,YMID).  The diagonal number
   XMID - YMID equals the number of inserted lines minus the number
   of deleted lines (counting only lines before the midpoint).

   Set PART->lo_minimal to true iff the minimal edit script for the
   left half of the partition is known; similarly for PART->hi_minimal.

   This function assumes that the first lines of the specified portions
   of the two files do not match, and likewise that the last lines do not
   match.  The caller must trim matching lines from the beginning and end
   of the portions it is going to specify.

   If we return the "wrong" partitions,
   the worst this can do is cause suboptimal diff output.
   It cannot cause incorrect diff output.  */

static void
diag (lin xoff, lin xlim, lin yoff, lin ylim, bool find_minimal,
      struct partition *part)
{
  lin *const fd = fdiag;	/* Give the compiler a chance. */
  lin *const bd = bdiag;	/* Additional help for the compiler. */
  lin const *const xv = xvec;	/* Still more help for the compiler. */
  lin const *const yv = yvec;	/* And more and more . . . */
  lin const dmin = xoff - ylim;	/* Minimum valid diagonal. */
  lin const dmax = xlim - yoff;	/* Maximum valid diagonal. */
  lin const fmid = xoff - yoff;	/* Center diagonal of top-down search. */
  lin const bmid = xlim - ylim;	/* Center diagonal of bottom-up search. */
  lin fmin = fmid, fmax = fmid;	/* Limits of top-down search. */
  lin bmin = bmid, bmax = bmid;	/* Limits of bottom-up search. */
  lin c;			/* Cost. */
  bool odd = (fmid - bmid) & 1;	/* True if southeast corner is on an odd
				   diagonal with respect to the northwest. */

  fd[fmid] = xoff;
  bd[bmid] = xlim;

  for (c = 1;; ++c)
    {
      lin d;			/* Active diagonal. */
      bool big_snake = false;

      /* Extend the top-down search by an edit step in each diagonal. */
      fmin > dmin ? fd[--fmin - 1] = -1 : ++fmin;
      fmax < dmax ? fd[++fmax + 1] = -1 : --fmax;
      for (d = fmax; d >= fmin; d -= 2)
	{
	  lin x, y, oldx, tlo = fd[d - 1], thi = fd[d + 1];

	  if (tlo >= thi)
	    x = tlo + 1;
	  else
	    x = thi;
	  oldx = x;
	  y = x - d;
	  while (x < xlim && y < ylim && xv[x] == yv[y])
	    ++x, ++y;
	  if (x - oldx > SNAKE_LIMIT)
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
      bmin > dmin ? bd[--bmin - 1] = LIN_MAX : ++bmin;
      bmax < dmax ? bd[++bmax + 1] = LIN_MAX : --bmax;
      for (d = bmax; d >= bmin; d -= 2)
	{
	  lin x, y, oldx, tlo = bd[d - 1], thi = bd[d + 1];

	  if (tlo < thi)
	    x = tlo;
	  else
	    x = thi - 1;
	  oldx = x;
	  y = x - d;
	  while (x > xoff && y > yoff && xv[x - 1] == yv[y - 1])
	    --x, --y;
	  if (oldx - x > SNAKE_LIMIT)
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

      /* Heuristic: check occasionally for a diagonal that has made
	 lots of progress compared with the edit distance.
	 If we have any such, find the one that has made the most
	 progress and return it as if it had succeeded.

	 With this heuristic, for files with a constant small density
	 of changes, the algorithm is linear in the file size.  */

      if (200 < c && big_snake && speed_large_files)
	{
	  lin best = 0;

	  for (d = fmax; d >= fmin; d -= 2)
	    {
	      lin dd = d - fmid;
	      lin x = fd[d];
	      lin y = x - d;
	      lin v = (x - xoff) * 2 - dd;
	      if (v > 12 * (c + (dd < 0 ? -dd : dd)))
		{
		  if (v > best
		      && xoff + SNAKE_LIMIT <= x && x < xlim
		      && yoff + SNAKE_LIMIT <= y && y < ylim)
		    {
		      /* We have a good enough best diagonal;
			 now insist that it end with a significant snake.  */
		      int k;

		      for (k = 1; xv[x - k] == yv[y - k]; k++)
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
	      lin dd = d - bmid;
	      lin x = bd[d];
	      lin y = x - d;
	      lin v = (xlim - x) * 2 + dd;
	      if (v > 12 * (c + (dd < 0 ? -dd : dd)))
		{
		  if (v > best
		      && xoff < x && x <= xlim - SNAKE_LIMIT
		      && yoff < y && y <= ylim - SNAKE_LIMIT)
		    {
		      /* We have a good enough best diagonal;
			 now insist that it end with a significant snake.  */
		      int k;

		      for (k = 0; xv[x + k] == yv[y + k]; k++)
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

      /* Heuristic: if we've gone well beyond the call of duty,
	 give up and report halfway between our best results so far.  */
      if (c >= too_expensive)
	{
	  lin fxybest;
	  lin bxybest;
	  lin fxbest IF_LINT (= 0);
	  lin bxbest IF_LINT (= 0);

	  /* Find forward diagonal that maximizes X + Y.  */
	  fxybest = -1;
	  for (d = fmax; d >= fmin; d -= 2)
	    {
	      lin x = MIN (fd[d], xlim);
	      lin y = x - d;
	      if (ylim < y)
		x = ylim + d, y = ylim;
	      if (fxybest < x + y)
		{
		  fxybest = x + y;
		  fxbest = x;
		}
	    }

	  /* Find backward diagonal that minimizes X + Y.  */
	  bxybest = LIN_MAX;
	  for (d = bmax; d >= bmin; d -= 2)
	    {
	      lin x = MAX (xoff, bd[d]);
	      lin y = x - d;
	      if (y < yoff)
		x = yoff + d, y = yoff;
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

/* Compare in detail contiguous subsequences of the two files
   which are known, as a whole, to match each other.

   The results are recorded in the vectors files[N].changed, by
   storing 1 in the element for each line that is an insertion or deletion.

   The subsequence of file 0 is [XOFF, XLIM) and likewise for file 1.

   Note that XLIM, YLIM are exclusive bounds.
   All line numbers are origin-0 and discarded lines are not counted.

   If FIND_MINIMAL, find a minimal difference no matter how
   expensive it is.  */

static void
compareseq (lin xoff, lin xlim, lin yoff, lin ylim, bool find_minimal)
{
  lin const *xv = xvec; /* Help the compiler.  */
  lin const *yv = yvec;

  /* Slide down the bottom initial diagonal. */
  while (xoff < xlim && yoff < ylim && xv[xoff] == yv[yoff])
    ++xoff, ++yoff;
  /* Slide up the top initial diagonal. */
  while (xlim > xoff && ylim > yoff && xv[xlim - 1] == yv[ylim - 1])
    --xlim, --ylim;

  /* Handle simple cases. */
  if (xoff == xlim)
    while (yoff < ylim)
      files[1].changed[files[1].realindexes[yoff++]] = 1;
  else if (yoff == ylim)
    while (xoff < xlim)
      files[0].changed[files[0].realindexes[xoff++]] = 1;
  else
    {
      struct partition part IF_LINT (= {0});

      /* Find a point of correspondence in the middle of the files.  */
      diag (xoff, xlim, yoff, ylim, find_minimal, &part);

      /* Use the partitions to split this problem into subproblems.  */
      compareseq (xoff, part.xmid, yoff, part.ymid, part.lo_minimal);
      compareseq (part.xmid, xlim, part.ymid, ylim, part.hi_minimal);
    }
}

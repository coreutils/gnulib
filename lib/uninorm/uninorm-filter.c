/* Stream-based normalization of Unicode strings.
   Copyright (C) 2009-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

   This file is free software.
   It is dual-licensed under "the GNU LGPLv3+ or the GNU GPLv2+".
   You can redistribute it and/or modify it under either
     - the terms of the GNU Lesser General Public License as published
       by the Free Software Foundation, either version 3, or (at your
       option) any later version, or
     - the terms of the GNU General Public License as published by the
       Free Software Foundation; either version 2, or (at your option)
       any later version, or
     - the same dual license "the GNU LGPLv3+ or the GNU GPLv2+".

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License
   for more details.

   You should have received a copy of the GNU Lesser General Public
   License and of the GNU General Public License along with this
   program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "uninorm.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "unictype.h"
#include "normalize-internal.h"
#include "uninorm/decompose-internal.h"


struct uninorm_filter
{
  /* Characteristics of the normalization form.  */
  int (*decomposer) (ucs4_t uc, ucs4_t *decomposition);
  ucs4_t (*composer) (ucs4_t uc1, ucs4_t uc2);

  /* The encapsulated stream.  */
  int (*stream_func) (void *stream_data, ucs4_t uc);
  void *stream_data;

  /* The buffer for sorting.  */
  #define SORTBUF_PREALLOCATED 64
  struct ucs4_with_ccc sortbuf_preallocated[2 * SORTBUF_PREALLOCATED];
  struct ucs4_with_ccc *sortbuf; /* array of size 2 * sortbuf_allocated */
  size_t sortbuf_allocated;
  size_t sortbuf_count;
};

struct uninorm_filter *
uninorm_filter_create (uninorm_t nf,
                       int (*stream_func) (void *stream_data, ucs4_t uc),
                       void *stream_data)
{
  struct uninorm_filter *filter =
    (struct uninorm_filter *) malloc (sizeof (struct uninorm_filter));

  if (filter == NULL)
    /* errno is ENOMEM. */
    return NULL;

  filter->decomposer = nf->decomposer;
  filter->composer = nf->composer;
  filter->stream_func = stream_func;
  filter->stream_data = stream_data;
  filter->sortbuf = filter->sortbuf_preallocated;
  filter->sortbuf_allocated = SORTBUF_PREALLOCATED;
  filter->sortbuf_count = 0;

  return filter;
}

int
uninorm_filter_write (struct uninorm_filter *filter, ucs4_t uc_arg)
{
  ucs4_t decomposed[UC_DECOMPOSITION_MAX_LENGTH];
  int decomposed_count;

  /* Accept the next character.  */
  decomposed[0] = uc_arg;
  decomposed_count = 1;

  /* Decompose it, recursively.
     It would be possible to precompute the recursive decomposition
     and store it in a table.  But this would significantly increase
     the size of the decomposition tables, because for example for
     U+1FC1 the recursive canonical decomposition and the recursive
     compatibility decomposition are different.  */
  {
    int curr;

    for (curr = 0; curr < decomposed_count; )
      {
        /* Invariant: decomposed[0..curr-1] is fully decomposed, i.e.
           all elements are atomic.  */
        ucs4_t curr_decomposed[UC_DECOMPOSITION_MAX_LENGTH];
        int curr_decomposed_count;

        curr_decomposed_count =
          filter->decomposer (decomposed[curr], curr_decomposed);
        if (curr_decomposed_count >= 0)
          {
            /* Move curr_decomposed[0..curr_decomposed_count-1] over
               decomposed[curr], making room.  It's not worth using
               memcpy() here, since the counts are so small.  */
            int shift = curr_decomposed_count - 1;

            if (shift < 0)
              abort ();
            if (shift > 0)
              {
                int j;

                decomposed_count += shift;
                if (decomposed_count > UC_DECOMPOSITION_MAX_LENGTH)
                  abort ();
                for (j = decomposed_count - 1 - shift; j > curr; j--)
                  decomposed[j + shift] = decomposed[j];
              }
            for (; shift >= 0; shift--)
              decomposed[curr + shift] = curr_decomposed[shift];
          }
        else
          {
            /* decomposed[curr] is atomic.  */
            curr++;
          }
      }
  }

  {
    /* Cache sortbuf and sortbuf_count in local register variables.  */
    struct ucs4_with_ccc *sortbuf = filter->sortbuf;
    size_t sortbuf_count = filter->sortbuf_count;
    int i;

    for (i = 0; i < decomposed_count; i++)
      {
        /* Fetch the next character from the decomposition.  */
        ucs4_t uc = decomposed[i];
        int ccc = uc_combining_class (uc);

        if (ccc == 0)
          {
            size_t j;

            /* Apply the canonical ordering algorithm to the accumulated
               sequence of characters.  */
            if (sortbuf_count > 1)
              gl_uninorm_decompose_merge_sort_inplace (sortbuf, sortbuf_count,
                                                       sortbuf + sortbuf_count);

            if (filter->composer != NULL)
              {
                /* Attempt to combine decomposed characters, as specified
                   in the Unicode Standard Annex #15 "Unicode Normalization
                   Forms".  We need to check
                     1. whether the first accumulated character is a
                        "starter" (i.e. has ccc = 0).  This is usually the
                        case.  But when the string starts with a
                        non-starter, the sortbuf also starts with a
                        non-starter.  Btw, this check could also be
                        omitted, because the composition table has only
                        entries (code1, code2) for which code1 is a
                        starter; if the first accumulated character is not
                        a starter, no lookup will succeed.
                     2. If the sortbuf has more than one character, check
                        for each of these characters that are not "blocked"
                        from the starter (i.e. have a ccc that is higher
                        than the ccc of the previous character) whether it
                        can be combined with the first character.
                     3. If only one character is left in sortbuf, check
                        whether it can be combined with the next character
                        (also a starter).  */
                if (sortbuf_count > 0 && sortbuf[0].ccc == 0)
                  {
                    for (j = 1; j < sortbuf_count; )
                      {
                        if (sortbuf[j].ccc > sortbuf[j - 1].ccc)
                          {
                            ucs4_t combined =
                              filter->composer (sortbuf[0].code, sortbuf[j].code);
                            if (combined)
                              {
                                size_t k;

                                sortbuf[0].code = combined;
                                /* sortbuf[0].ccc = 0, still valid.  */
                                for (k = j + 1; k < sortbuf_count; k++)
                                  sortbuf[k - 1] = sortbuf[k];
                                sortbuf_count--;
                                continue;
                              }
                          }
                        j++;
                      }
                    if (sortbuf_count == 1)
                      {
                        ucs4_t combined =
                          filter->composer (sortbuf[0].code, uc);
                        if (combined)
                          {
                            uc = combined;
                            ccc = 0;
                            /* uc could be further combined with subsequent
                               characters.  So don't put it into sortbuf[0] in
                               this round, only in the next round.  */
                            sortbuf_count = 0;
                          }
                      }
                  }
              }

            for (j = 0; j < sortbuf_count; j++)
              {
                ucs4_t muc = sortbuf[j].code;

                /* Output muc to the encapsulated stream.  */
                int ret = filter->stream_func (filter->stream_data, muc);
                if (ret < 0)
                  {
                    /* errno is set here.  */
                    filter->sortbuf_count = 0;
                    return -1;
                  }
              }

            /* sortbuf is now empty.  */
            sortbuf_count = 0;
          }

        /* Append (uc, ccc) to sortbuf.  */
        if (sortbuf_count == filter->sortbuf_allocated)
          {
            struct ucs4_with_ccc *new_sortbuf;

            filter->sortbuf_allocated = 2 * filter->sortbuf_allocated;
            if (filter->sortbuf_allocated < sortbuf_count) /* integer overflow? */
              abort ();
            new_sortbuf =
              (struct ucs4_with_ccc *)
              malloc (2 * filter->sortbuf_allocated * sizeof (struct ucs4_with_ccc));
            if (new_sortbuf == NULL)
              {
                /* errno is ENOMEM. */
                filter->sortbuf_count = sortbuf_count;
                return -1;
              }
            memcpy (new_sortbuf, filter->sortbuf,
                    sortbuf_count * sizeof (struct ucs4_with_ccc));
            if (filter->sortbuf != filter->sortbuf_preallocated)
              free (filter->sortbuf);
            filter->sortbuf = new_sortbuf;
            /* Update cache of filter->sortbuf.  */
            sortbuf = filter->sortbuf;
          }
        sortbuf[sortbuf_count].code = uc;
        sortbuf[sortbuf_count].ccc = ccc;
        sortbuf_count++;
      }

    filter->sortbuf_count = sortbuf_count;
  }

  return 0;
}

/* Bring data buffered in the filter to its destination, the encapsulated
   stream.
   Return 0 if successful, or -1 with errno set upon failure.
   Note! If after calling this function, additional characters are written
   into the filter, the resulting character sequence in the encapsulated stream
   will not necessarily be normalized.  */
int
uninorm_filter_flush (struct uninorm_filter *filter)
{
  /* Cache sortbuf and sortbuf_count in local register variables.  */
  struct ucs4_with_ccc * const sortbuf = filter->sortbuf;
  size_t sortbuf_count = filter->sortbuf_count;
  size_t j;

  /* Apply the canonical ordering algorithm to the accumulated
     sequence of characters.  */
  if (sortbuf_count > 1)
    gl_uninorm_decompose_merge_sort_inplace (sortbuf, sortbuf_count,
                                             sortbuf + sortbuf_count);

  if (filter->composer != NULL)
    {
      /* Attempt to combine decomposed characters, as specified
         in the Unicode Standard Annex #15 "Unicode Normalization
         Forms".  We need to check
           1. whether the first accumulated character is a
              "starter" (i.e. has ccc = 0).  This is usually the
              case.  But when the string starts with a
              non-starter, the sortbuf also starts with a
              non-starter.  Btw, this check could also be
              omitted, because the composition table has only
              entries (code1, code2) for which code1 is a
              starter; if the first accumulated character is not
              a starter, no lookup will succeed.
           2. If the sortbuf has more than one character, check
              for each of these characters that are not "blocked"
              from the starter (i.e. have a ccc that is higher
              than the ccc of the previous character) whether it
              can be combined with the first character.
           3. If only one character is left in sortbuf, check
              whether it can be combined with the next character
              (also a starter).  */
      if (sortbuf_count > 0 && sortbuf[0].ccc == 0)
        {
          for (j = 1; j < sortbuf_count; )
            {
              if (sortbuf[j].ccc > sortbuf[j - 1].ccc)
                {
                  ucs4_t combined =
                    filter->composer (sortbuf[0].code, sortbuf[j].code);
                  if (combined)
                    {
                      size_t k;

                      sortbuf[0].code = combined;
                      /* sortbuf[0].ccc = 0, still valid.  */
                      for (k = j + 1; k < sortbuf_count; k++)
                        sortbuf[k - 1] = sortbuf[k];
                      sortbuf_count--;
                      continue;
                    }
                }
              j++;
            }
        }
    }

  for (j = 0; j < sortbuf_count; j++)
    {
      ucs4_t muc = sortbuf[j].code;

      /* Output muc to the encapsulated stream.  */
      int ret = filter->stream_func (filter->stream_data, muc);
      if (ret < 0)
        {
          /* errno is set here.  */
          filter->sortbuf_count = 0;
          return -1;
        }
    }

  /* sortbuf is now empty.  */
  filter->sortbuf_count = 0;

  return 0;
}

/* Bring data buffered in the filter to its destination, the encapsulated
   stream, then close and free the filter.
   Return 0 if successful, or -1 with errno set upon failure.  */
int
uninorm_filter_free (struct uninorm_filter *filter)
{
  int ret = uninorm_filter_flush (filter);

  if (ret < 0)
    /* errno is set here.  */
    return -1;

  if (filter->sortbuf_count > 0)
    abort ();
  if (filter->sortbuf != filter->sortbuf_preallocated)
    free (filter->sortbuf);
  free (filter);

  return 0;
}

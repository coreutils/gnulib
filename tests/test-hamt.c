/* Test of persistent hash array mapped trie implementation.
   Copyright (C) 2021-2022 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Marc Nieper-Wißkirchen <marc@nieper-wisskirchen.de>, 2021.  */

#include <config.h>

#include "hamt.h"
#include "macros.h"
#include "xalloc.h"

typedef struct
{
  Hamt_entry entry;
  int val;
} Element;

static int
entry_value (const void *elt)
{
  return ((Element *) elt)->val;
}

static size_t
hash_element (const void *elt)
{
  return entry_value (elt) & ~3; /* We drop the last bits so that we
                                    can test hash collisions. */
}

static bool
compare_element (const void *elt1, const void *elt2)
{
  return entry_value (elt1) == entry_value (elt2);
}

static void
free_element (Hamt_entry *elt)
{
  free (elt);
}

static Hamt_entry *
make_element (int n)
{
  Element *elt = XMALLOC (Element);
  elt->val = n;
  return hamt_element (&elt->entry);
}

static Hamt *
test_hamt_create (void)
{
  return hamt_create (hash_element, compare_element, free_element);
}


static int sum = 0;
static int flag;

static bool
proc (Hamt_entry *elt, void *data)
{
  if (data == &flag)
    {
      sum += entry_value (elt);
      return true;
    }
  if (sum > 0)
    {
      sum = 0;
      return true;
    }
  return false;
}

static void
test_general (void)
{
  Hamt *hamt = test_hamt_create ();

  Hamt_entry *x5 = make_element (5);
  Hamt_entry *p = x5;
  Hamt *hamt1 = hamt_insert (hamt, &p);
  ASSERT (hamt1 != hamt);
  ASSERT (hamt_lookup (hamt, x5) == NULL);
  ASSERT (hamt_lookup (hamt1, x5) == x5);
  hamt_free (hamt);

  Hamt_entry *y5 = make_element (5);
  p = y5;
  Hamt *hamt2 = hamt_insert (hamt1, &p);
  ASSERT (hamt2 == hamt1);
  ASSERT (p == x5);
  ASSERT (hamt_lookup (hamt1, y5) == x5);

  p = y5;
  hamt = hamt_replace (hamt1, &p);
  ASSERT (p == x5);
  ASSERT (hamt_lookup (hamt, y5) == y5);
  hamt_free (hamt);
  y5 = make_element (5);

  Hamt_entry *z37 = make_element (37);
  p = z37;
  hamt2 = hamt_insert (hamt1, &p);
  ASSERT (hamt2 != hamt1);
  ASSERT (p == z37);
  ASSERT (hamt_lookup (hamt1, z37) == NULL);
  ASSERT (hamt_lookup (hamt2, z37) == z37);
  hamt_free (hamt1);

  ASSERT (hamt_lookup (hamt2, x5) == x5);
  ASSERT (hamt_lookup (hamt2, z37) == z37);

  ASSERT (hamt_do_while (hamt2, proc, &flag) == 2);
  ASSERT (sum == 42);
  ASSERT (hamt_do_while (hamt2, proc, NULL) == 1);
  ASSERT (sum == 0);

  p = y5;
  hamt1 = hamt_remove (hamt2, &p);
  ASSERT (hamt1 != hamt2);
  ASSERT (p == x5);

  ASSERT (hamt_lookup (hamt1, x5) == NULL);
  ASSERT (hamt_lookup (hamt2, x5) == x5);

  hamt_free (hamt1);
  Hamt_entry *x4 = make_element (4);
  hamt1 = hamt_insert (hamt2, &x4);
  hamt_free (hamt2);
  Hamt_entry *x6 = make_element (6);
  hamt2 = hamt_insert (hamt1, &x6);
  hamt_free (hamt1);
  ASSERT (hamt_do_while (hamt2, proc, &flag) == 4);
  ASSERT (sum == 52);

  hamt1 = hamt_remove (hamt2, &x4);
  sum = 0;
  ASSERT (hamt_do_while (hamt2, proc, &flag) == 4);
  ASSERT (sum == 52);
  sum = 0;
  ASSERT (hamt_do_while (hamt1, proc, &flag) == 3);
  ASSERT (sum == 48);

  hamt_free (hamt1);
  hamt_free (hamt2);
  free_element (y5);
}

static bool
true_processor (_GL_ATTRIBUTE_MAYBE_UNUSED Hamt_entry *elt,
                _GL_ATTRIBUTE_MAYBE_UNUSED void *data)
{
  return true;
}

static size_t
element_count (Hamt *hamt)
{
  return hamt_do_while (hamt, true_processor, NULL);
}

struct find_values_context
{
  size_t n;
  int *elts;
  bool *found;
};

static bool
find_values_processor (Hamt_entry *entry, void *data)
{
  struct find_values_context *ctx = data;
  int val = entry_value (entry);
  for (size_t i = 0; i < ctx->n; ++i)
    if (ctx->elts [i] == val && !ctx->found [i])
      {
        ctx->found [i] = true;
        return true;
      }
  return false;
}

static bool
find_values (Hamt *hamt, size_t n, int *elts)
{
  bool *found = XCALLOC (n, bool);
  struct find_values_context ctx = {n, elts, found};
  bool res = hamt_do_while (hamt, find_values_processor, &ctx) == n;
  free (found);
  return res;
}

static size_t
insert_values (Hamt **hamt, size_t n, int *elts, bool destructive)
{
  size_t cnt = 0;
  for (size_t i = 0; i < n; ++i)
    {
      Hamt_entry *p = make_element (elts [i]);
      Hamt_entry *q = p;
      if (destructive)
        {
          if (hamt_insert_x (*hamt, &p))
            ++cnt;
          else
            free_element (q);
        }
      else
        {
          Hamt *new_hamt = hamt_insert (*hamt, &p);
          if (new_hamt != *hamt)
            {
              hamt_free (*hamt);
              *hamt = new_hamt;
              ++cnt;
            }
          else
            {
              free_element (q);
            }
        }
    }
  return cnt;
}

static size_t
replace_values (Hamt **hamt, size_t n, int *elts, bool destructive)
{
  size_t cnt = 0;
  for (size_t i = 0; i < n; ++i)
    {
      Hamt_entry *p = make_element (elts [i]);
      if (destructive)
        {
          if (hamt_replace_x (*hamt, p))
            ++cnt;
        }
      else
        {
          Hamt *new_hamt = hamt_replace (*hamt, &p);
          hamt_free (*hamt);
          *hamt = new_hamt;
          if (p != NULL)
            ++cnt;
        }
    }
  return cnt;
}

static size_t
remove_values (Hamt **hamt, size_t n, int *elts, bool destructive)
{
  size_t cnt = 0;
  for (size_t i = 0; i < n; ++i)
    {
      Hamt_entry *p = make_element (elts [i]);
      Hamt_entry *q = p;
      if (destructive)
        {
          if (hamt_remove_x (*hamt, p))
            ++cnt;
        }
      else
        {
          Hamt *new_hamt = hamt_remove (*hamt, &p);
          if (new_hamt != *hamt)
            {
              hamt_free (*hamt);
              *hamt = new_hamt;
              ++cnt;
            }
        }
      free (q);
    }
  return cnt;
}

static int val_array1 [10] = {1, 2, 3, 4, 33, 34, 35, 36, 1024, 1025};
static int val_array2 [10] = {1, 2, 34, 36, 1025, 32768, 32769, 32770, 32771,
                              32772};

static void
test_functional_update (void)
{
  Hamt *hamt = test_hamt_create ();

  ASSERT (insert_values (&hamt, 10, val_array1, false) == 10);
  ASSERT (element_count (hamt) == 10);
  ASSERT (find_values (hamt, 10, val_array1));
  ASSERT (insert_values (&hamt, 10, val_array2, false) == 5);
  ASSERT (element_count (hamt) == 15);
  ASSERT (remove_values (&hamt, 10, val_array1, false) == 10);
  ASSERT (element_count (hamt) == 5);
  ASSERT (remove_values (&hamt, 10, val_array2, false) == 5);
  ASSERT (element_count (hamt) == 0);

  ASSERT (replace_values (&hamt, 10, val_array1, false) == 0);
  ASSERT (element_count (hamt) == 10);
  ASSERT (find_values (hamt, 10, val_array1));
  ASSERT (replace_values (&hamt, 10, val_array2, false) == 5);
  ASSERT (element_count (hamt) == 15);

  hamt_free (hamt);
}

static void
test_destructive_update (void)
{
  Hamt *hamt = test_hamt_create ();

  ASSERT (insert_values (&hamt, 10, val_array1, true) == 10);
  ASSERT (element_count (hamt) == 10);
  ASSERT (find_values (hamt, 10, val_array1));
  ASSERT (insert_values (&hamt, 10, val_array2, true) == 5);
  ASSERT (element_count (hamt) == 15);
  ASSERT (remove_values (&hamt, 10, val_array1, true) == 10);
  ASSERT (element_count (hamt) == 5);
  ASSERT (remove_values (&hamt, 10, val_array2, true) == 5);
  ASSERT (element_count (hamt) == 0);

  ASSERT (replace_values (&hamt, 10, val_array1, true) == 0);
  ASSERT (element_count (hamt) == 10);
  ASSERT (find_values (hamt, 10, val_array1));
  ASSERT (replace_values (&hamt, 10, val_array2, true) == 5);
  ASSERT (element_count (hamt) == 15);

  hamt_free (hamt);
}

static void
test_iterator (void)
{
  Hamt *hamt = test_hamt_create ();
  ASSERT (insert_values (&hamt, 10, val_array1, true) == 10);
  Hamt_iterator iter[1];
  iter[0] = hamt_iterator (hamt);
  size_t cnt = 0;
  bool found [10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  Hamt_entry *p;
  while (hamt_iterator_next (iter, &p))
    {
      for (size_t i = 0; i < 10; ++i)
        if (val_array1 [i] == entry_value (p))
          {
            ASSERT (!found [i]);
            found [i] = true;
            ++cnt;
            break;
          }
    }
  ASSERT (cnt == 10);
  hamt_iterator_free (iter);
  hamt_free (hamt);
}

int
main (void)
{
  test_general ();
  test_functional_update ();
  test_destructive_update ();
  test_iterator ();
}

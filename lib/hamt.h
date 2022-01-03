/* (Persistent) hash array mapped tries.
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

/* Written by Marc Nieper-Wißkirchen <marc@nieper-wisskirchen.de>, 2021. */

/* This module provides a persistent version of hash array mapped
   tries (hamts) that can be used in place of hash tables when pure
   (functional) operations are needed.

   A hash function and an equivalence predicate has to be provided for
   the elements that can be inserted, replaced and removed in a hamt.
   A hamt cannot contain duplicates that compare equal.

   Each non-destructive updating operation returns a new hamt, which
   shares structure with the original one.  Destructive updates only
   effect the hamt, on which the destructive operation is applied.
   For example, given a hamt HAMT1, any non-destructive update
   operation (e.g. hamt_insert) will result in a new hamt HAMT2.
   Whatever further operations (destructive or not, including freeing
   a hamt) are applied to HAMT1 won't change HAMT2 and vice versa.  To
   free all the memory, hash_free has therefore to be applied to both
   HAMT1 and HAMT2.

   If persistence is not needed, transient hash tables are probably
   faster.

   See also: Phil Bagwell (2000). Ideal Hash Trees (Report). Infoscience
   Department, École Polytechnique Fédérale de Lausanne.

   http://infoscience.epfl.ch/record/64398/files/idealhashtrees.pdf  */

#ifndef _GL_HAMT_H
#define _GL_HAMT_H

#ifndef _GL_INLINE_HEADER_BEGIN
# error "Please include config.h first."
#endif
_GL_INLINE_HEADER_BEGIN
#ifndef _GL_HAMT_INLINE
# define _GL_HAMT_INLINE _GL_INLINE
#endif

/* The GL_HAMT_THREAD_SAFE flag is set if the implementation of hamts
   is thread-safe as long as two threads do not simultaneously access
   the same hamt.  This is non-trivial as different hamts may share
   some structure.
   We can define it only when the compiler supports _Atomic.  For GCC,
   it is supported starting with GCC 4.9.  */

#if (__GNUC__ + (__GNUC_MINOR >= 9) > 4) \
    && __STDC_VERSION__ >= 201112L && !defined __STD_NO_ATOMICS__ \
    && !defined __cplusplus
# define GL_HAMT_THREAD_SAFE 1
#else
# define GL_HAMT_THREAD_SAFE 0
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Hash values are of type size_t.  For each level of the trie, we use
   5 bits (corresponding to lg2 of the width of a 32-bit word.  */
#define _GL_HAMT_MAX_DEPTH ((SIZE_WIDTH + 4) / 5)

/************/
/* Elements */
/************/

/* A hamt stores pointers to elements.  Each element has to be a
   struct whose initial member is of the type Hamt_entry.  You need to
   define this struct yourself.  It will typically contain an
   Hamt_entry, a key, and, optionally, a value.

   An element is conceptually owned by a hamt as soon as it is
   inserted.  It will be automatically freed as soon as the last hamt
   containing it is freed.  */
typedef struct
{
#if GL_HAMT_THREAD_SAFE
  _Atomic
#endif
  size_t ref_count;
} Hamt_entry;

/* Initialize *ELT, which has to point to a structure as described
   above and return ELT type-cast.

   Before an element is inserted into any hamt, whether once or
   multiple times, it has to be initialized exactly once.  */
_GL_HAMT_INLINE Hamt_entry *
hamt_element (void *elt)
{
  Hamt_entry *entry = elt;
  entry->ref_count = 0;         /* This assumes that element_entry ==
                                   0.  */
  return entry;
}

/*************************/
/* Opaque Hamt Structure */
/*************************/

/* In user-code, hamts are accessed through pointers to the opaque
   Hamt type.  Two hamts are said to be the same if and only if their
   pointers are equal. */
typedef struct hamt Hamt;

/*************/
/* Interface */
/*************/

/* A hash function has to be pure, and two elements that compare equal
   have to have the same hash value.  For a hash function to be a good
   one, it is important that it uses all SIZE_WIDTH bits in
   size_t.  */
typedef size_t (Hamt_hasher) (const void *elt);

/* A comparison function has to be pure, and two elements that have
   equal pointers have to compare equal.  */
typedef bool (Hamt_comparator) (const void *elt1, const void *elt2);

/* A user-defined function that is called when the last hamt
   containing a particular element is freed.  */
typedef void (Hamt_freer) (Hamt_entry *elt);

/****************************/
/* Creation and Destruction */
/****************************/

/* Free the resources solely allocated by HAMT and all elements solely
   contained in it.  */
extern void hamt_free (Hamt *hamt);

/* Create and return a new and empty hash array mapped trie.  */
_GL_ATTRIBUTE_NODISCARD
extern Hamt *hamt_create (Hamt_hasher *hasher, Hamt_comparator *comparator,
                          Hamt_freer *freer)
  _GL_ATTRIBUTE_DEALLOC (hamt_free, 1);

/* Return a copy of HAMT, which is not the same in the sense above.
   This procedure can be used, for example, so that two threads can
   access the same data independently.  */
_GL_ATTRIBUTE_NODISCARD
extern Hamt *hamt_copy (Hamt *hamt)
  _GL_ATTRIBUTE_DEALLOC (hamt_free, 1);

/**********/
/* Lookup */
/**********/

/* If ELT matches an entry in HAMT, return this entry.  Otherwise,
   return NULL.  */
extern Hamt_entry *hamt_lookup (const Hamt *hamt, const void *elt);

/**************************/
/* Insertion and Deletion */
/**************************/

/* If *ELT_PTR matches an element already in HAMT, set *ELT_PTR to the
   existing element and return the original hamt.  Otherwise, insert
   *ELT_PTR into a copy of the hamt and return the copy.  */
_GL_ATTRIBUTE_NODISCARD
extern Hamt *hamt_insert (Hamt *hamt, Hamt_entry **elt_ptr);

/* If *ELT_PTR matches an element already in HAMT, set *ELT_PTR to the
existing element, remove the element from a copy of the hamt and
return the copy.  Otherwise, return the original hamt.  */
_GL_ATTRIBUTE_NODISCARD
extern Hamt *hamt_remove (Hamt *hamt, Hamt_entry **elt_ptr);

/* Insert *ELT_PTR into a copy of HAMT and return the copy.  If an
   existing element was replaced, set *ELT_PTR to this element, and to
   NULL otherwise.  */
_GL_ATTRIBUTE_NODISCARD
extern Hamt *hamt_replace (Hamt *hamt, Hamt_entry **elt_ptr);

/*************/
/* Iteration */
/*************/

/* A processor function is called during walking of a hamt, which
   returns true to continue the walking.  */
typedef bool (Hamt_processor) (Hamt_entry *elt, void *data);

/* Call PROC for every entry of the hamt until it returns false.  The
   first argument to the processor is the entry, the second argument
   is the value of DATA as received.  Return the number of calls that
   returned true.  During processing, the hamt mustn't be
   modified.  */
extern size_t hamt_do_while (const Hamt *hamt, Hamt_processor *proc,
                             void *data);

/* An alternative interface to iterating through the entry of a hamt
   that does not make use of a higher-order function like
   hamt_do_while uses the Hamt_iterator type, which can be allocated
   through automatic variables on the stack.  As a hamt iterator
   operates on a copy of a hamt, the original hamt can modified
   (including freeing it) without affecting the iterator.  */
struct hamt_iterator
{
  Hamt* hamt;
  int depth;
  size_t path;
  size_t position;
  Hamt_entry *entry[_GL_HAMT_MAX_DEPTH + 1];
};
typedef struct hamt_iterator Hamt_iterator;

/* Create of copy of HAMT and return an initialized ITER on the
   copy.  */
extern Hamt_iterator hamt_iterator (Hamt *hamt);

/* Free the resources allocated for ITER, including the hamt copy
   associated with it.  */
extern void hamt_iterator_free (Hamt_iterator *iter);

/* Return an independent copy of ITER that is initially in the same
   state.  Any operation on the original iterator (including freeing
   it) doesn't affect the iterator copy and vice versa.  */
extern Hamt_iterator hamt_iterator_copy (Hamt_iterator *iter);

/* Return true if ITER is not at the end, place the current element in
   *ELT_PTR and move the iterator forward.  Otherwise, return
   false.  */
extern bool hamt_iterator_next (Hamt_iterator *iter,
                                Hamt_entry **elt_ptr);

/***********************/
/* Destructive Updates */
/***********************/

/* If *ELT_PTR matches an element already in HAMT, set *ELT_PTR to the
   element from the table and return false.  Otherwise, insert *ELT_PTR
   destructively into the hamt and return true.  */
extern bool hamt_insert_x (Hamt *hamt, Hamt_entry **elt_ptr);

/* Insert ELT destructively into HAMT.  If an existing element was
   replaced, return true.  Otherwise, return false.  */
extern bool hamt_replace_x (Hamt *hamt, Hamt_entry *elt);

/* If ELT matches an element already in HAMT, remove the element
   destructively from the hamt and return true.  Otherwise, return
   false.  */
extern bool hamt_remove_x (Hamt *hamt, Hamt_entry *elt);

_GL_INLINE_HEADER_END

#endif /* _GL_HAMT_H */

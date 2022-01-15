/* Abstract ordered set data type as a C++ class.
   Copyright (C) 2006-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2006.

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

#ifndef _GL_OSET_HH
#define _GL_OSET_HH

#include "gl_oset.h"
#include "gl_xoset.h"

#include <stdlib.h>     /* because Gnulib's <stdlib.h> may '#define free ...' */

/* gl_OSet is a C++ wrapper around the gl_oset data type.
   Its element type is 'ELTYPE *'.

   It is merely a pointer, not a smart pointer. In other words:
   it does NOT do reference-counting, and the destructor does nothing.  */

template <class T> class gl_OSet;

template <class ELTYPE>
class gl_OSet<ELTYPE *>
{
public:
  // ------------------------------ Constructors ------------------------------

  gl_OSet ()
    : _ptr (NULL)
    {}

  /* Creates an empty set.
     IMPLEMENTATION is one of GL_ARRAY_OSET, GL_AVLTREE_OSET, GL_RBTREE_OSET.
     COMPAR_FN is an element comparison function or NULL.
     DISPOSE_FN is an element disposal function or NULL.  */
  gl_OSet (gl_oset_implementation_t implementation,
           int (*compar_fn) (ELTYPE * /*elt1*/, ELTYPE * /*elt2*/),
           void (*dispose_fn) (ELTYPE *))
    : _ptr (gl_oset_create_empty (implementation,
                                  reinterpret_cast<gl_setelement_compar_fn>(compar_fn),
                                  reinterpret_cast<gl_setelement_dispose_fn>(dispose_fn)))
    {}

  /* Copy constructor.  */
  gl_OSet (const gl_OSet& x)
    { _ptr = x._ptr; }

  /* Assignment operator.  */
  gl_OSet& operator= (const gl_OSet& x)
    { _ptr = x._ptr; return *this; }

  // ------------------------------- Destructor -------------------------------

  ~gl_OSet ()
    { _ptr = NULL; }

  // ----------------------- Read-only member functions -----------------------

  /* Returns the current number of elements in the ordered set.  */
  size_t size () const
    { return gl_oset_size (_ptr); }

  /* Searches whether an element is already in the ordered set.
     Returns true if found, or false if not present in the set.  */
  bool search (ELTYPE * elt) const
    { return gl_oset_search (_ptr, elt); }

  /* Searches the least element in the ordered set that compares greater or equal
     to the given THRESHOLD.  The representation of the THRESHOLD is defined
     by the THRESHOLD_FN.
     Returns true and store the found element in ELT if found, otherwise returns
     false.  */
  template <typename THT>
  bool search_atleast (bool (*threshold_fn) (ELTYPE * /*elt*/, THT * /*threshold*/),
                       THT * threshold,
                       ELTYPE *& elt) const
  { return gl_oset_search_atleast (_ptr, reinterpret_cast<gl_setelement_threshold_fn>(threshold_fn), threshold, &elt); }

  // ----------------------- Modifying member functions -----------------------

  /* Adds an element to the ordered set.
     Returns true if it was not already in the set and added, false otherwise.  */
  bool add (ELTYPE * elt)
    { return gl_oset_add (_ptr, elt); }

  /* Removes an element from the ordered set.
     Returns true if it was found and removed.  */
  bool remove (ELTYPE * elt)
    { return gl_oset_remove (_ptr, elt); }

  /* Invokes ACTION (ELT, ACTION_DATA) and updates the ordered set if,
     during this invocation, the attributes/properties of the element ELT change
     in a way that influences the comparison function.
     Warning: During the invocation of ACTION, the ordered set is inconsistent
     and must not be accessed!
     Returns 1 if the position of the element in the ordered set has changed as
     a consequence, 0 if the element stayed at the same position, or -1 if it
     collided with another element and was therefore removed.  */
  template <typename DT>
  int update (ELTYPE * elt,
              void (*action) (ELTYPE * /*elt*/, DT * /*action_data*/),
              DT *action_data)
    {
      return gl_oset_update (_ptr, elt,
                             reinterpret_cast<void (*) (const void *, void *)> (action),
                             action_data);
    }

  /* Frees the entire ordered set.
     (But this call does not free the elements of the set.  It only invokes
     the DISPOSE_FN on each of the elements of the set.)  */
  void free ()
    { gl_oset_free (_ptr); }

  // ------------------------------ Private stuff ------------------------------

private:
  gl_oset_t _ptr;

public:
  // -------------------------------- Iterators --------------------------------
  // Only a forward iterator.
  // Does not implement the STL operations (++, *, and != .end()), but a simpler
  // interface that needs only one virtual function call per iteration instead
  // of three.

  class iterator {
  public:

    /* If there is a next element, stores the next element in ELT, advances the
       iterator and returns true.  Otherwise, returns false.  */
    bool next (ELTYPE *& elt)
      {
        const void *next_elt;
        bool has_next = gl_oset_iterator_next (&_state, &next_elt);
        if (has_next)
          elt = static_cast<ELTYPE *>(next_elt);
        return has_next;
      }

    ~iterator ()
      { gl_oset_iterator_free (&_state); }

  #if defined __xlC__ || defined __HP_aCC || defined __SUNPRO_CC || defined __EDG__
  public:
  #else
  private:
    friend iterator gl_OSet::begin ();
    template <typename THT>
    friend iterator gl_OSet::begin_atleast (bool (*) (ELTYPE *, THT *), THT *);
  #endif

    iterator (gl_oset_t ptr)
      : _state (gl_oset_iterator (ptr))
      {}

    template <typename THT>
    iterator (gl_oset_t ptr,
              bool (*threshold_fn) (ELTYPE * /*elt*/, THT * /*threshold*/),
              THT * threshold)
      : _state (gl_oset_iterator_atleast (ptr, reinterpret_cast<gl_setelement_threshold_fn>(threshold_fn), threshold))
      {}

  private:

    gl_oset_iterator_t _state;
  };

  /* Creates an iterator traversing the ordered set.
     The set's contents must not be modified while the iterator is in use,
     except for removing the last returned element.  */
  iterator begin ()
    { return iterator (_ptr); }

  /* Creates an iterator traversing the tail of an ordered set, that comprises
     the elements that compare greater or equal to the given THRESHOLD.  The
     representation of the THRESHOLD is defined by the THRESHOLD_FN.
     The set's contents must not be modified while the iterator is in use,
     except for removing the last returned element.  */
  template <typename THT>
  iterator begin_atleast (bool (*threshold_fn) (ELTYPE * /*elt*/, THT * /*threshold*/),
                          THT * threshold)
    { return iterator (_ptr, threshold_fn, threshold); }
};

#endif /* _GL_OSET_HH */

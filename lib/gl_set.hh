/* Abstract set data type as a C++ class.
   Copyright (C) 2006-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2018.

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

#ifndef _GL_SET_HH
#define _GL_SET_HH

#include "gl_set.h"
#include "gl_xset.h"

#include <stdlib.h>     /* because Gnulib's <stdlib.h> may '#define free ...' */

/* gl_Set is a C++ wrapper around the gl_set data type.
   Its element type is 'ELTYPE *'.

   It is merely a pointer, not a smart pointer. In other words:
   it does NOT do reference-counting, and the destructor does nothing.  */

template <class T> class gl_Set;

template <class ELTYPE>
class gl_Set<ELTYPE *>
{
public:
  // ------------------------------ Constructors ------------------------------

  gl_Set ()
    : _ptr (NULL)
    {}

  /* Creates an empty set.
     IMPLEMENTATION is one of GL_ARRAY_SET, GL_LINKEDHASH_SET, GL_HASH_SET.
     EQUALS_FN is an element comparison function or NULL.
     HASHCODE_FN is an element hash code function or NULL.
     DISPOSE_FN is an element disposal function or NULL.  */
  gl_Set (gl_set_implementation_t implementation,
          bool (*equals_fn) (ELTYPE * /*elt1*/, ELTYPE * /*elt2*/),
          size_t (*hashcode_fn) (ELTYPE *),
          void (*dispose_fn) (ELTYPE *))
    : _ptr (gl_set_create_empty (implementation,
                                 reinterpret_cast<gl_setelement_equals_fn>(equals_fn),
                                 reinterpret_cast<gl_setelement_hashcode_fn>(hashcode_fn),
                                 reinterpret_cast<gl_setelement_dispose_fn>(dispose_fn)))
    {}

  /* Copy constructor.  */
  gl_Set (const gl_Set& x)
    { _ptr = x._ptr; }

  /* Assignment operator.  */
  gl_Set& operator= (const gl_Set& x)
    { _ptr = x._ptr; return *this; }

  // ------------------------------- Destructor -------------------------------

  ~gl_Set ()
    { _ptr = NULL; }

  // ----------------------- Read-only member functions -----------------------

  /* Returns the current number of elements in the set.  */
  size_t size () const
    { return gl_set_size (_ptr); }

  /* Searches whether an element is already in the set.
     Returns true if found, or false if not present in the set.  */
  bool search (ELTYPE * elt) const
    { return gl_set_search (_ptr, elt); }

  // ----------------------- Modifying member functions -----------------------

  /* Adds an element to the set.
     Returns true if it was not already in the set and added, false otherwise.  */
  bool add (ELTYPE * elt)
    { return gl_set_add (_ptr, elt); }

  /* Removes an element from the set.
     Returns true if it was found and removed.  */
  bool remove (ELTYPE * elt)
    { return gl_set_remove (_ptr, elt); }

  /* Frees the entire set.
     (But this call does not free the elements of the set.  It only invokes
     the DISPOSE_FN on each of the elements of the set.)  */
  void free ()
    { gl_set_free (_ptr); _ptr = NULL; }

  // ------------------------------ Private stuff ------------------------------

private:
  gl_set_t _ptr;

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
        bool has_next = gl_set_iterator_next (&_state, &next_elt);
        if (has_next)
          elt = static_cast<ELTYPE *>(next_elt);
        return has_next;
      }

    ~iterator ()
      { gl_set_iterator_free (&_state); }

  #if defined __xlC__ || defined __HP_aCC || defined __SUNPRO_CC
  public:
  #else
  private:
    friend iterator gl_Set::begin ();
  #endif

    iterator (gl_set_t ptr)
      : _state (gl_set_iterator (ptr))
      {}

  private:

    gl_set_iterator_t _state;
  };

  /* Creates an iterator traversing the set.
     The set's contents must not be modified while the iterator is in use,
     except for removing the last returned element.  */
  iterator begin ()
    { return iterator (_ptr); }
};

#endif /* _GL_SET_HH */

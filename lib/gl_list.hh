/* Abstract sequential list data type as a C++ class.
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

#ifndef _GL_LIST_HH
#define _GL_LIST_HH

#include "gl_list.h"
#include "gl_xlist.h"
#include "gl_sublist.h"
#include "gl_xsublist.h"

#include <stdlib.h>     /* because Gnulib's <stdlib.h> may '#define free ...' */

/* gl_List is a C++ wrapper around the gl_list data type.
   Its element type is 'ELTYPE *'.

   It is merely a pointer, not a smart pointer. In other words:
   it does NOT do reference-counting, and the destructor does nothing.  */

template <class T> class gl_List;

template <class ELTYPE>
class gl_List<ELTYPE *>
{
public:
  // ------------------------------ Constructors ------------------------------

  gl_List ()
    : _ptr (NULL)
    {}

  /* Creates an empty list.
     IMPLEMENTATION is one of GL_ARRAY_LIST, GL_CARRAY_LIST, GL_LINKED_LIST,
     GL_AVLTREE_LIST, GL_RBTREE_LIST, GL_LINKEDHASH_LIST, GL_AVLTREEHASH_LIST,
     GL_RBTREEHASH_LIST.
     EQUALS_FN is an element comparison function or NULL.
     HASHCODE_FN is an element hash code function or NULL.
     DISPOSE_FN is an element disposal function or NULL.
     ALLOW_DUPLICATES is false if duplicate elements shall not be allowed in
     the list. The implementation may verify this at runtime.  */
  gl_List (gl_list_implementation_t implementation,
           bool (*equals_fn) (ELTYPE * /*elt1*/, ELTYPE * /*elt2*/),
           size_t (*hashcode_fn) (ELTYPE *),
           void (*dispose_fn) (ELTYPE *),
           bool allow_duplicates)
    : _ptr (gl_list_create_empty (implementation,
                                  reinterpret_cast<gl_listelement_equals_fn>(equals_fn),
                                  reinterpret_cast<gl_listelement_hashcode_fn>(hashcode_fn),
                                  reinterpret_cast<gl_listelement_dispose_fn>(dispose_fn),
                                  allow_duplicates))
    {}

  /* Creates a list with given contents.
     IMPLEMENTATION is one of GL_ARRAY_LIST, GL_CARRAY_LIST, GL_LINKED_LIST,
     GL_AVLTREE_LIST, GL_RBTREE_LIST, GL_LINKEDHASH_LIST, GL_AVLTREEHASH_LIST,
     GL_RBTREEHASH_LIST.
     EQUALS_FN is an element comparison function or NULL.
     HASHCODE_FN is an element hash code function or NULL.
     DISPOSE_FN is an element disposal function or NULL.
     ALLOW_DUPLICATES is false if duplicate elements shall not be allowed in
     the list. The implementation may verify this at runtime.
     COUNT is the number of initial elements.
     CONTENTS[0..COUNT-1] is the initial contents.  */
  gl_List (gl_list_implementation_t implementation,
           bool (*equals_fn) (ELTYPE * /*elt1*/, ELTYPE * /*elt2*/),
           size_t (*hashcode_fn) (ELTYPE *),
           void (*dispose_fn) (ELTYPE *),
           bool allow_duplicates,
           size_t count, ELTYPE **contents)
    : _ptr (gl_list_create (implementation,
                            reinterpret_cast<gl_listelement_equals_fn>(equals_fn),
                            reinterpret_cast<gl_listelement_hashcode_fn>(hashcode_fn),
                            reinterpret_cast<gl_listelement_dispose_fn>(dispose_fn),
                            allow_duplicates,
                            count, contents))
    {}

  /* Creates a sublist of a given list.
     This is the list of elements with indices i, start_index <= i < end_index.
     The sublist is backed by the given list, which means:
       - Modifications to the sublist affect the whole list.
       - Modifications to the whole list are immediately visible in the sublist.
       - The sublist is only valid as long as the whole list is valid.
       - The sublist must not be passed to the gl_list_sortedlist_add() function.
   */
  gl_List (const gl_List& whole_list, size_t start_index, size_t end_index)
    : _ptr (gl_sublist_create (whole_list._ptr, start_index, end_index))
    {}

  /* Copy constructor.  */
  gl_List (const gl_List& x)
    { _ptr = x._ptr; }

  /* Assignment operator.  */
  gl_List& operator= (const gl_List& x)
    { _ptr = x._ptr; return *this; }

  // ------------------------------- Destructor -------------------------------

  ~gl_List ()
    { _ptr = NULL; }

  // ----------------------- Read-only member functions -----------------------

  /* Returns the current number of elements in the list.  */
  size_t size () const
    { return gl_list_size (_ptr); }

  /* Returns the element value represented by a list node.  */
  ELTYPE * node_value (gl_list_node_t node) const
    { return static_cast<ELTYPE *>(gl_list_node_value (_ptr, node)); }

  /* Returns the node immediately after the given node in the list, or NULL
     if the given node is the last (rightmost) one in the list.  */
  gl_list_node_t next_node (gl_list_node_t node) const
    { return gl_list_next_node (_ptr, node); }

  /* Returns the node immediately before the given node in the list, or NULL
     if the given node is the first (leftmost) one in the list.  */
  gl_list_node_t previous_node (gl_list_node_t node) const
    { return gl_list_previous_node (_ptr, node); }

  /* Returns the first node in the list, or NULL if the list is empty.  */
  gl_list_node_t first_node () const
    { return gl_list_first_node (_ptr); }

  /* Returns the last node in the list, or NULL if the list is empty.  */
  gl_list_node_t last_node () const
    { return gl_list_last_node (_ptr); }

  /* Returns the element at a given position in the list.
     POSITION must be >= 0 and < gl_list_size (list).  */
  ELTYPE * get_at (size_t position) const
    { return static_cast<ELTYPE *>(gl_list_get_at (_ptr, position)); }

  /* Returns the element at the first position in the list.
     The list must be non-empty.  */
  ELTYPE * get_first () const
    { return static_cast<ELTYPE *>(gl_list_get_first (_ptr)); }

  /* Returns the element at the last position in the list.
     The list must be non-empty.  */
  ELTYPE * get_last () const
    { return static_cast<ELTYPE *>(gl_list_get_last (_ptr)); }

  /* Searches whether an element is already in the list.
     Returns its node if found, or NULL if not present in the list.  */
  gl_list_node_t search (ELTYPE * elt) const
    { return gl_list_search (_ptr, elt); }

  /* Searches whether an element is already in the list,
     at a position >= START_INDEX.
     Returns its node if found, or NULL if not present in the list.  */
  gl_list_node_t search_from (size_t start_index, ELTYPE * elt) const
    { return gl_list_search_from (_ptr, start_index, elt); }

  /* Searches whether an element is already in the list,
     at a position >= START_INDEX and < END_INDEX.
     Returns its node if found, or NULL if not present in the list.  */
  gl_list_node_t search_from_to (size_t start_index, size_t end_index, ELTYPE * elt) const
    { return gl_list_search_from_to (_ptr, start_index, end_index, elt); }

  /* Searches whether an element is already in the list.
     Returns its position if found, or (size_t)(-1) if not present in the list.  */
  size_t indexof (ELTYPE * elt) const
    { return gl_list_indexof (_ptr, elt); }

  /* Searches whether an element is already in the list,
     at a position >= START_INDEX.
     Returns its position if found, or (size_t)(-1) if not present in the list.  */
  size_t indexof_from (size_t start_index, ELTYPE * elt) const
    { return gl_list_indexof_from (_ptr, start_index, elt); }

  /* Searches whether an element is already in the list,
     at a position >= START_INDEX and < END_INDEX.
     Returns its position if found, or (size_t)(-1) if not present in the list.  */
  size_t indexof_from_to (size_t start_index, size_t end_index, ELTYPE * elt) const
    { return gl_list_indexof_from_to (_ptr, start_index, end_index, elt); }

  // ----------------------- Modifying member functions -----------------------

  /* Replaces the element value represented by a list node.  */
  void node_set_value (gl_list_node_t node, ELTYPE * elt)
    { gl_list_node_set_value (_ptr, node, elt); }

  /* Replaces the element at a given position in the list.
     POSITION must be >= 0 and < gl_list_size (list).
     Returns its node.  */
  gl_list_node_t set_at (size_t position, ELTYPE * elt)
    { return gl_list_set_at (_ptr, position, elt); }

  /* Replaces the element at the first position in the list.
     Returns its node.
     The list must be non-empty.  */
  gl_list_node_t set_first (ELTYPE * elt)
    { return gl_list_set_first (_ptr, elt); }

  /* Replaces the element at the last position in the list.
     Returns its node.
     The list must be non-empty.  */
  gl_list_node_t set_last (ELTYPE * elt)
    { return gl_list_set_last (_ptr, elt); }

  /* Adds an element as the first element of the list.
     Returns its node.  */
  gl_list_node_t add_first (ELTYPE * elt)
    { return gl_list_add_first (_ptr, elt); }

  /* Adds an element as the last element of the list.
     Returns its node.  */
  gl_list_node_t add_last (ELTYPE * elt)
    { return gl_list_add_last (_ptr, elt); }

  /* Adds an element before a given element node of the list.
     Returns its node.  */
  gl_list_node_t add_before (gl_list_node_t node, ELTYPE * elt)
    { return gl_list_add_before (_ptr, node, elt); }

  /* Adds an element after a given element node of the list.
     Return its node.  */
  gl_list_node_t add_after (gl_list_node_t node, ELTYPE * elt)
    { return gl_list_add_after (_ptr, node, elt); }

  /* Adds an element at a given position in the list.
     POSITION must be >= 0 and <= gl_list_size (list).  */
  gl_list_node_t add_at (size_t position, ELTYPE * elt)
    { return gl_list_add_at (_ptr, position, elt); }

  /* Removes an element from the list.
     Returns true.  */
  bool remove_node (gl_list_node_t node)
    { return gl_list_remove_node (_ptr, node); }

  /* Removes an element at a given position from the list.
     POSITION must be >= 0 and < gl_list_size (list).
     Returns true.  */
  bool remove_at (size_t position)
    { return gl_list_remove_at (_ptr, position); }

  /* Removes the element at the first position from the list.
     Returns true if it was found and removed,
     or false if the list was empty.  */
  bool remove_first ()
    { return gl_list_remove_first (_ptr); }

  /* Removes the element at the last position from the list.
     Returns true if it was found and removed,
     or false if the list was empty.  */
  bool remove_last ()
    { return gl_list_remove_last (_ptr); }

  /* Searches and removes an element from the list.
     Returns true if it was found and removed.  */
  bool remove (ELTYPE * elt)
    { return gl_list_remove (_ptr, elt); }

  /* Frees the entire list.
     (But this call does not free the elements of the list.  It only invokes
     the DISPOSE_FN on each of the elements of the list, and only if the list
     is not a sublist.)  */
  void free ()
    { gl_list_free (_ptr); _ptr = NULL; }

  // -------------------- Member functions for sorted lists --------------------

  /* The following functions are for lists without duplicates where the
     order is given by a sort criterion.  */

  /* Searches whether an element is already in the list.
     The list is assumed to be sorted with COMPAR.
     Returns its node if found, or NULL if not present in the list.
     If the list contains several copies of ELT, the node of the leftmost one is
     returned.  */
  gl_list_node_t sortedlist_search (int (*compar) (ELTYPE * /*elt1*/, ELTYPE * /*elt2*/),
                                    ELTYPE * elt)
    { return gl_sortedlist_search (_ptr, reinterpret_cast<gl_listelement_compar_fn>(compar), elt); }

  /* Searches whether an element is already in the list.
     The list is assumed to be sorted with COMPAR.
     Only list elements with indices >= START_INDEX and < END_INDEX are
     considered; the implementation uses these bounds to minimize the number
     of COMPAR invocations.
     Returns its node if found, or NULL if not present in the list.
     If the list contains several copies of ELT, the node of the leftmost one is
     returned.  */
  gl_list_node_t sortedlist_search_from_to (int (*compar) (ELTYPE * /*elt1*/, ELTYPE * /*elt2*/),
                                            size_t start_index,
                                            size_t end_index,
                                            ELTYPE * elt)
    { return gl_sortedlist_search_from_to (_ptr, reinterpret_cast<gl_listelement_compar_fn>(compar), start_index, end_index, elt); }

  /* Searches whether an element is already in the list.
     The list is assumed to be sorted with COMPAR.
     Returns its position if found, or (size_t)(-1) if not present in the list.
     If the list contains several copies of ELT, the position of the leftmost one
     is returned.  */
  size_t sortedlist_indexof (int (*compar) (ELTYPE * /*elt1*/, ELTYPE * /*elt2*/),
                             ELTYPE * elt)
    { return gl_sortedlist_indexof (_ptr, reinterpret_cast<gl_listelement_compar_fn>(compar), elt); }

  /* Searches whether an element is already in the list.
     The list is assumed to be sorted with COMPAR.
     Only list elements with indices >= START_INDEX and < END_INDEX are
     considered; the implementation uses these bounds to minimize the number
     of COMPAR invocations.
     Returns its position if found, or (size_t)(-1) if not present in the list.
     If the list contains several copies of ELT, the position of the leftmost one
     is returned.  */
  size_t sortedlist_indexof_from_to (int (*compar) (ELTYPE * /*elt1*/, ELTYPE * /*elt2*/),
                                     size_t start_index,
                                     size_t end_index,
                                     ELTYPE * elt)
    { return gl_sortedlist_indexof_from_to (_ptr, reinterpret_cast<gl_listelement_compar_fn>(compar), start_index, end_index, elt); }

  /* Adds an element at the appropriate position in the list.
     The list is assumed to be sorted with COMPAR.
     Returns its node.  */
  gl_list_node_t sortedlist_add (int (*compar) (ELTYPE * /*elt1*/, ELTYPE * /*elt2*/),
                                 ELTYPE * elt)
    { return gl_sortedlist_add (_ptr, reinterpret_cast<gl_listelement_compar_fn>(compar), elt); }

  /* Searches and removes an element from the list.
     The list is assumed to be sorted with COMPAR.
     Returns true if it was found and removed.
     If the list contains several copies of ELT, only the leftmost one is
     removed.  */
  bool sortedlist_remove (int (*compar) (ELTYPE * /*elt1*/, ELTYPE * /*elt2*/),
                          ELTYPE * elt)
    { return gl_sortedlist_remove (_ptr, reinterpret_cast<gl_listelement_compar_fn>(compar), elt); }

  // ------------------------------ Private stuff ------------------------------

private:
  gl_list_t _ptr;

public:
  // -------------------------------- Iterators --------------------------------
  // Only a forward iterator.
  // Does not implement the STL operations (++, *, and != .end()), but a simpler
  // interface that needs only one virtual function call per iteration instead
  // of three.

  class iterator {
  public:

    /* If there is a next element, stores the next element in ELT, advances
       the iterator and returns true.
       Otherwise, returns false.  */
    bool next (ELTYPE *& elt)
      {
        const void *next_elt;
        bool has_next = gl_list_iterator_next (&_state, &next_elt, NULL);
        if (has_next)
          elt = static_cast<ELTYPE *>(next_elt);
        return has_next;
      }

    /* If there is a next element, stores the next element in ELT, stores its
       node in *NODEP if NODEP is non-NULL, advances the iterator and returns true.
       Otherwise, returns false.  */
    bool next (ELTYPE *& elt, gl_list_node_t *nodep)
      {
        const void *next_elt;
        bool has_next = gl_list_iterator_next (&_state, &next_elt, nodep);
        if (has_next)
          elt = static_cast<ELTYPE *>(next_elt);
        return has_next;
      }

    ~iterator ()
      { gl_list_iterator_free (&_state); }

  #if defined __xlC__ || defined __HP_aCC || defined __SUNPRO_CC
  public:
  #else
  private:
    friend iterator gl_List::begin ();
  #endif

    iterator (gl_list_t ptr)
      : _state (gl_list_iterator (ptr))
      {}

    iterator (gl_list_t ptr, size_t start_index, size_t end_index)
      : _state (gl_list_iterator_from_to (ptr, start_index, end_index))
      {}

  private:

    gl_list_iterator_t _state;
  };

  /* Creates an iterator traversing the list.
     The list contents must not be modified while the iterator is in use,
     except for replacing or removing the last returned element.  */
  iterator begin ()
    { return iterator (_ptr); }

  /* Creates an iterator traversing the element with indices i,
     start_index <= i < end_index, of a list.
     The list contents must not be modified while the iterator is in use,
     except for replacing or removing the last returned element.  */
  iterator begin (size_t start_index, size_t end_index)
    { return iterator (_ptr, start_index, end_index); }
};

#endif /* _GL_LIST_HH */

/* Abstract map data type as a C++ class.
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

#ifndef _GL_MAP_HH
#define _GL_MAP_HH

#include "gl_map.h"
#include "gl_xmap.h"

#include <stdlib.h>     /* because Gnulib's <stdlib.h> may '#define free ...' */

/* gl_Map is a C++ wrapper around the gl_map data type.
   Its key type is 'KEYTYPE *'.  Its value type is 'VALUETYPE *'.

   It is merely a pointer, not a smart pointer. In other words:
   it does NOT do reference-counting, and the destructor does nothing.  */

template <class K, class V> class gl_Map;

template <class KEYTYPE, class VALUETYPE>
class gl_Map<KEYTYPE *, VALUETYPE *>
{
public:
  // ------------------------------ Constructors ------------------------------

  gl_Map ()
    : _ptr (NULL)
    {}

  /* Creates an empty map.
     IMPLEMENTATION is one of GL_ARRAY_MAP, GL_LINKEDHASH_MAP, GL_HASH_MAP.
     EQUALS_FN is a key comparison function or NULL.
     HASHCODE_FN is a key hash code function or NULL.
     KDISPOSE_FN is a key disposal function or NULL.
     VDISPOSE_FN is a value disposal function or NULL.  */
  gl_Map (gl_map_implementation_t implementation,
          bool (*equals_fn) (KEYTYPE * /*key1*/, KEYTYPE * /*key2*/),
          size_t (*hashcode_fn) (KEYTYPE *),
          void (*kdispose_fn) (KEYTYPE *),
          void (*vdispose_fn) (VALUETYPE *))
    : _ptr (gl_map_create_empty (implementation,
                                 reinterpret_cast<gl_mapkey_equals_fn>(equals_fn),
                                 reinterpret_cast<gl_mapkey_hashcode_fn>(hashcode_fn),
                                 reinterpret_cast<gl_mapkey_dispose_fn>(kdispose_fn),
                                 reinterpret_cast<gl_mapvalue_dispose_fn>(vdispose_fn)))
    {}

  /* Copy constructor.  */
  gl_Map (const gl_Map& x)
    { _ptr = x._ptr; }

  /* Assignment operator.  */
  gl_Map& operator= (const gl_Map& x)
    { _ptr = x._ptr; return *this; }

  // ------------------------------- Destructor -------------------------------

  ~gl_Map ()
    { _ptr = NULL; }

  // ----------------------- Read-only member functions -----------------------

  /* Returns the current number of pairs in the map.  */
  size_t size () const
    { return gl_map_size (_ptr); }

  /* Searches whether a pair with the given key is already in the map.
     Returns the value if found, or NULL if not present in the map.  */
  VALUETYPE * get (KEYTYPE * key) const
    { return static_cast<VALUETYPE *>(gl_map_get (_ptr, key)); }

  /* Searches whether a pair with the given key is already in the map.
     Returns true and sets VALUE to the value if found.
     Returns false if not present in the map.  */
  bool search (KEYTYPE * key, VALUETYPE *& value) const
    { return gl_map_search (_ptr, key, &value); }

  // ----------------------- Modifying member functions -----------------------

  /* Adds a pair to the map.
     Returns true if a pair with the given key was not already in the map and so
     this pair was added.
     Returns false if a pair with the given key was already in the map and only
     its value was replaced.  */
  bool put (KEYTYPE * key, VALUETYPE * value)
    { return gl_map_put (_ptr, key, value); }

  /* Adds a pair to the map and retrieves the previous value.
     Returns true if a pair with the given key was not already in the map and so
     this pair was added.
     Returns false and sets OLDVALUE to the previous value, if a pair with the
     given key was already in the map and only its value was replaced.  */
  bool getput (KEYTYPE * key, VALUETYPE * value, VALUETYPE *& oldvalue)
    { return gl_map_getput (_ptr, key, value, &oldvalue); }

  /* Removes a pair from the map.
     Returns true if the key was found and its pair removed.
     Returns false otherwise.  */
  bool remove (KEYTYPE * key)
    { return gl_map_remove (_ptr, key); }

  /* Removes a pair from the map and retrieves the previous value.
     Returns true and sets OLDVALUE to the previous value, if the key was found
     and its pair removed.
     Returns false otherwise.  */
  bool getremove (KEYTYPE * key, VALUETYPE *& oldvalue)
    { return gl_map_getremove (_ptr, key, &oldvalue); }

  /* Frees the entire map.
     (But this call does not free the keys and values of the pairs in the map.
     It only invokes the KDISPOSE_FN on each key and the VDISPOSE_FN on each value
     of the pairs in the map.)  */
  void free ()
    { gl_map_free (_ptr); _ptr = NULL; }

  // ------------------------------ Private stuff ------------------------------

private:
  gl_map_t _ptr;

public:
  // -------------------------------- Iterators --------------------------------
  // Only a forward iterator.
  // Does not implement the STL operations (++, *, and != .end()), but a simpler
  // interface that needs only one virtual function call per iteration instead
  // of three.

  class iterator {
  public:

    /* If there is a next pair, stores the next pair in KEY and VALUE, advance
       the iterator, and returns true.  Otherwise, returns false.  */
    bool next (KEYTYPE *& key, VALUETYPE *& value)
      {
        const void *next_key;
        const void *next_value;
        bool has_next = gl_map_iterator_next (&_state, &next_key, &next_value);
        if (has_next)
          {
            key = static_cast<KEYTYPE *>(next_key);
            value = static_cast<VALUETYPE *>(next_value);
          }
        return has_next;
      }

    ~iterator ()
      { gl_map_iterator_free (&_state); }

  #if defined __xlC__ || defined __HP_aCC || defined __SUNPRO_CC
  public:
  #else
  private:
    friend iterator gl_Map::begin ();
  #endif

    iterator (gl_map_t ptr)
      : _state (gl_map_iterator (ptr))
      {}

  private:

    gl_map_iterator_t _state;
  };

  /* Creates an iterator traversing the map.
     The map's contents must not be modified while the iterator is in use,
     except for modifying the value of the last returned key or removing the
     last returned pair.  */
  iterator begin ()
    { return iterator (_ptr); }
};

#endif /* _GL_MAP_HH */

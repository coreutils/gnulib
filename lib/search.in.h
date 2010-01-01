/* A GNU-like <search.h>.

   Copyright (C) 2007-2010 Free Software Foundation, Inc.

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

#ifndef _GL_SEARCH_H

/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_SEARCH_H@
# if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
# endif
# @INCLUDE_NEXT@ @NEXT_SEARCH_H@
#endif

#ifndef _GL_SEARCH_H
#define _GL_SEARCH_H


/* The definition of GL_LINK_WARNING is copied here.  */

/* The definition of _GL_ARG_NONNULL is copied here.  */


#ifdef __cplusplus
extern "C" {
#endif


#if @GNULIB_TSEARCH@
# if @REPLACE_TSEARCH@
#  define tsearch rpl_tsearch
#  define tfind rpl_tfind
#  define tdelete rpl_tdelete
#  define twalk rpl_twalk
# endif
# if !@HAVE_TSEARCH@ || @REPLACE_TSEARCH@

/* See <http://www.opengroup.org/susv3xbd/search.h.html>,
       <http://www.opengroup.org/susv3xsh/tsearch.html>
   for details.  */

#  if !@HAVE_TSEARCH@
typedef enum
{
  preorder,
  postorder,
  endorder,
  leaf
}
VISIT;
#  endif

/* Searches an element in the tree *VROOTP that compares equal to KEY.
   If one is found, it is returned.  Otherwise, a new element equal to KEY
   is inserted in the tree and is returned.  */
extern void * tsearch (const void *key, void **vrootp,
                       int (*compar) (const void *, const void *))
     _GL_ARG_NONNULL ((1, 2, 3));

/* Searches an element in the tree *VROOTP that compares equal to KEY.
   If one is found, it is returned.  Otherwise, NULL is returned.  */
extern void * tfind (const void *key, void *const *vrootp,
                     int (*compar) (const void *, const void *))
     _GL_ARG_NONNULL ((1, 2, 3));

/* Searches an element in the tree *VROOTP that compares equal to KEY.
   If one is found, it is removed from the tree, and its parent node is
   returned.  Otherwise, NULL is returned.  */
extern void * tdelete (const void *key, void **vrootp,
                       int (*compar) (const void *, const void *))
     _GL_ARG_NONNULL ((1, 2, 3));

/* Perform a depth-first, left-to-right traversal of the tree VROOT.
   The ACTION function is called:
     - for non-leaf nodes: 3 times, before the left subtree traversal,
       after the left subtree traversal but before the right subtree traversal,
       and after the right subtree traversal,
     - for leaf nodes: once.
   The arguments passed to ACTION are:
     1. the node; it can be casted to a 'const void * const *', i.e. into a
        pointer to the key,
     2. an indicator which visit of the node this is,
     3. the level of the node in the tree (0 for the root).  */
extern void twalk (const void *vroot,
                   void (*action) (const void *, VISIT, int))
     _GL_ARG_NONNULL ((2));

# endif
#elif defined GNULIB_POSIXCHECK
# undef tsearch
# define tsearch(k,v,c) \
    (GL_LINK_WARNING ("tsearch is unportable - " \
                      "use gnulib module tsearch for portability"), \
     tsearch (k, v, c))
# undef tfind
# define tfind(k,v,c) \
    (GL_LINK_WARNING ("tfind is unportable - " \
                      "use gnulib module tsearch for portability"), \
     tfind (k, v, c))
# undef tdelete
# define tdelete(k,v,c) \
    (GL_LINK_WARNING ("tdelete is unportable - " \
                      "use gnulib module tsearch for portability"), \
     tdelete (k, v, c))
# undef twalk
# define twalk(v,a) \
    (GL_LINK_WARNING ("twalk is unportable - " \
                      "use gnulib module tsearch for portability"), \
     twalk (v, a))
#endif


#ifdef __cplusplus
}
#endif

#endif /* _GL_SEARCH_H */
#endif /* _GL_SEARCH_H */

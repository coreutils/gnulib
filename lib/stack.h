/* Type-safe stack data type.
   Copyright (C) 2020-2022 Free Software Foundation, Inc.

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

/* Written by Marc Nieper-Wißkirchen <marc@nieper-wisskirchen.de>, 2020.  */

/* This header file does not have include-guards as it is meant to be
   includable multiple times as long as the necessary defines have
   been set up.

   A stack is implemented with a homogenous array of elements in
   memory, which will be resized (and moved) as needed.  Elements are
   passed by value and it is the responsibility of the user-code to
   free any resources associated with individual elements.

   The exported names are all prefixed by ‘stack’ (e.g. stack_init),
   but this can be changed by setting an appropriate define.
     Type:               stack_type
     Initialization:     stack_init (&stack);
     De-initialization:  stack_destroy (&stack);
     Predicate:          bool res = stack_empty (&stack);
     Introspection:      ELEMENT *base = stack_base (&stack);
     Pushing:            stack_push (&stack, element);
     Popping:            ELEMENT element = stack_pop (&stack);
     Discarding:         stack_discard (&stack);
     Top-of-stack:       ELEMENT element = stack_top (&stack);
     Size:               idx_t size = stack_size (&stack);

   Here, ELEMENT is the type to which GL_STACK_ELEMENT was defined when
   this file was included.
*/

/* Before including this file, you need to define:
     GL_STACK_ELEMENT       The type of the elements on the stack.
     GL_STACK_STORAGECLASS  (Optional) The storage class specifier (e.g. static)
                            to use in the function definitions.
     GL_STACK_NAME          (Optional) The prefix to use for the type names
                            and functions definitions instead of the default
                            ‘stack’.

   After including this file, these names will be undefined.

   Before including this file, you also need to include:
     #include <stdbool.h>
     #include <stdlib.h>
     #include "assure.h"
     #include "xalloc.h"
*/

#ifndef GL_STACK_ELEMENT
# error "Please define GL_STACK_ELEMENT first."
#endif

#ifndef GL_STACK_STORAGECLASS
# define GL_STACK_STORAGECLASS
#endif

#ifndef GL_STACK_NAME
# define GL_STACK_NAME stack
#endif

#define _GL_STACK_PREFIX(name) _GL_CONCAT(GL_STACK_NAME, _GL_CONCAT(_, name))
#define _GL_STACK_TYPE _GL_STACK_PREFIX(type)

typedef struct
{
  GL_STACK_ELEMENT *base;
  idx_t size;
  idx_t allocated;
} _GL_STACK_TYPE;

/* Initialize a stack.  */
GL_STACK_STORAGECLASS void
_GL_STACK_PREFIX (init) (_GL_STACK_TYPE *stack)
{
  stack->base = NULL;
  stack->size = 0;
  stack->allocated = 0;
}

/* Destroy a stack by freeing the allocated space.  */
GL_STACK_STORAGECLASS void
_GL_STACK_PREFIX (destroy) (_GL_STACK_TYPE *stack)
{
  free (stack->base);
}

/* Return true if the stack currently holds no element.  */
GL_STACK_STORAGECLASS _GL_ATTRIBUTE_PURE bool
_GL_STACK_PREFIX (empty) (const _GL_STACK_TYPE *stack)
{
  return stack->size == 0;
}

/* Return the current address of the array of stack elements.

   The result is invalidated as soon as an element is added or removed
   from the stack.  */
GL_STACK_STORAGECLASS _GL_ATTRIBUTE_PURE GL_STACK_ELEMENT *
_GL_STACK_PREFIX (current_base) (const _GL_STACK_TYPE *stack)
{
  return stack->base;
}

/* Push an element to the top of the stack.  */
GL_STACK_STORAGECLASS void
_GL_STACK_PREFIX (push) (_GL_STACK_TYPE *stack, GL_STACK_ELEMENT item)
{
  if (stack->size == stack->allocated)
    stack->base = xpalloc (stack->base, &stack->allocated, 1, -1,
                           sizeof *stack->base);
  stack->base [stack->size++] = item;
}

/* Pop the element from the top of the stack, which must be non-empty,
   and return it. */
GL_STACK_STORAGECLASS GL_STACK_ELEMENT
_GL_STACK_PREFIX (pop) (_GL_STACK_TYPE *stack)
{
  affirm (!_GL_STACK_PREFIX (empty) (stack));
  return stack->base [--stack->size];
}

/* Pop the element from the top of the stack, which must be
   non-empty.  */
GL_STACK_STORAGECLASS void
_GL_STACK_PREFIX (discard) (_GL_STACK_TYPE *stack)
{
  affirm (!_GL_STACK_PREFIX (empty) (stack));
  --stack->size;
}

/* Return the element from the top of the stack, which must be
   non-empty.  */
GL_STACK_STORAGECLASS GL_STACK_ELEMENT
_GL_STACK_PREFIX (top) (const _GL_STACK_TYPE *stack)
{
  affirm (!_GL_STACK_PREFIX (empty) (stack));
  return stack->base [stack->size - 1];
}

/* Return the currently stored number of elements in the stack.  */
GL_STACK_STORAGECLASS _GL_ATTRIBUTE_PURE idx_t
_GL_STACK_PREFIX (size) (const _GL_STACK_TYPE *stack)
{
  return stack->size;
}

#undef GL_STACK_ELEMENT
#undef GL_STACK_STORAGECLASS
#undef GL_STACK_NAME
#undef _GL_STACK_PREFIX
#undef _GL_STACK_TYPE

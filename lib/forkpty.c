/* Fork a child attached to a pseudo-terminal descriptor.
   Copyright (C) 2010 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include <pty.h>

#if HAVE_DECL_FORKPTY
# undef forkpty
int
rpl_forkpty (int *amaster, char *name, struct termios const *termp,
         struct winsize const *winp)
{
  /* Cast away const, for implementations with weaker prototypes.  */
  return forkpty (amaster, name, (struct termios *) termp,
                  (struct winsize *) winp);
}
#else
# error forkpty has not been ported to your system; \
  report this to bug-gnulib@gnu.org for help
#endif

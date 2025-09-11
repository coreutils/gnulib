/* Test of getting protection of a virtual memory area.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

#include <config.h>

#include "vma-prot.h"

#include <stdlib.h>

#if HAVE_SYS_MMAN_H
# include <sys/mman.h>
# include <unistd.h>
#endif

#include "macros.h"

int dummy;

int
f (int x)
{
  return x;
}

int
main (void)
{
  int prot;

  /* Test on memory allocated through malloc().  */
  {
    char *mem = malloc (10);
    prot = get_vma_prot (mem + 2, 5);
    ASSERT (prot != -1);
    ASSERT (((VMA_PROT_READ | VMA_PROT_WRITE) & ~prot) == 0);
  }
  {
    char *mem = malloc (1000000);
    prot = get_vma_prot (mem, 1000000);
    ASSERT (prot != -1);
    ASSERT (((VMA_PROT_READ | VMA_PROT_WRITE) & ~prot) == 0);
  }

#if HAVE_SYS_MMAN_H
  /* Test on memory allocated through mmap().  */
  {
    char *mem = mmap (NULL, 1024*1024, PROT_READ | PROT_WRITE,
                      MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (mem != (char *)(-1))
      {
        prot = get_vma_prot (mem, 1024*1024);
        ASSERT (prot != -1);
        ASSERT (prot == (VMA_PROT_READ | VMA_PROT_WRITE));

        size_t pagesize = sysconf (_SC_PAGESIZE);
        if (pagesize <= 512*1024
            && munmap (mem + pagesize, pagesize) >= 0)
          {
            prot = get_vma_prot (mem, 1024*1024);
            ASSERT (prot != -1);
            ASSERT (prot == 0);
          }
      }
  }
#endif

  /* Test on a global variable.  */
  prot = get_vma_prot (&dummy, sizeof (dummy));
  ASSERT (prot != -1);
  ASSERT (((VMA_PROT_READ | VMA_PROT_WRITE) & ~prot) == 0);

  /* Test on a function.  */
  prot = get_vma_prot (&f, 1);
  ASSERT (prot != -1);
#if (defined __hppa || defined __hppa64__ \
     || defined __ia64__ \
     || defined _AIX \
     || (defined __powerpc64__ && defined __linux__ && _CALL_ELF != 2))
  /* On these platforms, a function pointer is actually a pointer to
     a struct of pointers.  */
#else
  /* On these platforms, a function pointer is a pointer to or into some
     machine instruction.  */
  ASSERT ((VMA_PROT_EXECUTE & ~prot) == 0);
#endif
#if !defined __OpenBSD__
  /* Except on OpenBSD/arm64, the machine instructions are also readable.  */
  ASSERT ((VMA_PROT_READ & ~prot) == 0);
#endif

  return test_exit_status;
}

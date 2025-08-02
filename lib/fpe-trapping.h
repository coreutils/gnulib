/* Trapping floating-point exceptions.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

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

/* HAVE_FPE_TRAPPING
   Defined to 1 when sigfpe_on_invalid is available.  */

/* sigfpe_on_invalid
   Enables a SIGFPE signal when an FE_INVALID exception occurs.
   A SIGFPE signal by default terminates the program.
   Returns >= 0 when successful, -1 upon failure.  */


#include <fenv.h>

static int
sigfpe_on_invalid ()
{
  /* Clear FE_INVALID exceptions from past operations.  */
  feclearexcept (FE_INVALID);

  /* An FE_INVALID exception shall trigger a SIGFPE signal.
     This call may fail on arm, arm64, riscv64 CPUs.
     Also, possibly a bug in glibc/sysdeps/m68k/fpu/feenablxcpt.c: it sets
     only bit 13, but should better set bits 15, 14, 13 of the control
     register together.  See
     <https://sourceware.org/PR30993>.  */
  int ret = feenableexcept (FE_INVALID);
  if (ret == -1)
    return -1;

  return 0;
}

/* But it does not work on RISC-V.  That's because the fcsr register has only
   bits for floating-point exception status, but no bits for trapping
   floating-point exceptions.  */
/* And it does not work on arm with software floating-point emulation.  */
#if !(defined __riscv || (defined __GLIBC__ && defined __arm__ && defined __SOFTFP__))
# define HAVE_FPE_TRAPPING 1
#endif

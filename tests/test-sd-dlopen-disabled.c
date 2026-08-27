/* Test the disabled sd-dlopen macros.
   Copyright 2026 Free Software Foundation, Inc.

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

#include <config.h>

#undef HAVE_ASM_IFNDEF_ENDIF_BALIGN

#include "sd-dlopen.h"

#ifdef _SD_ELF_NOTE_DLOPEN
# error "sd-dlopen implementation enabled without assembler support"
#endif

SD_ELF_NOTE_DLOPEN ("test",
                    "Test optional dependency",
                    SD_ELF_NOTE_DLOPEN_PRIORITY_SUGGESTED,
                    "libtest.so.1");

int
main (void)
{
  SD_ELF_NOTE_DLOPEN_ANCHORED (test_required,
                               "test",
                               "Test required dependency",
                               SD_ELF_NOTE_DLOPEN_PRIORITY_REQUIRED,
                               "libtest.so.1");
  return 0;
}

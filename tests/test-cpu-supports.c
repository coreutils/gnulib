/* Test parsing of GLIBC_TUNABLES environment variable.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Specification.  */
#include "cpu-supports.h"

#include <stdlib.h>

#include "macros.h"

int
main ()
{
  /* Enable reparsing the GLIBC_TUNABLES env var.  */
  hwcap_allowed_nocache = true;

  { /* unset GLIBC_TUNABLES.  */
    unsetenv ("GLIBC_TUNABLES");
    ASSERT (hwcap_allowed (NULL));
    ASSERT (hwcap_allowed ("-AVX"));
  }

  { /* empty GLIBC_TUNABLES.  */
    setenv ("GLIBC_TUNABLES", "", 1);
    ASSERT (hwcap_allowed (NULL));
    ASSERT (hwcap_allowed ("-AVX"));
  }

  { /* empty hwcaps.  */
    setenv ("GLIBC_TUNABLES", "glibc.cpu.hwcaps=", 1);
    ASSERT (hwcap_allowed (NULL));
    ASSERT (hwcap_allowed ("-AVX"));
  }

  { /* Single hwcaps with partial substring match.  */
    setenv ("GLIBC_TUNABLES", "glibc.cpu.hwcaps=-AVX2", 1);
    ASSERT (hwcap_allowed (NULL));
    ASSERT (hwcap_allowed ("-AVX"));
    ASSERT (hwcap_allowed ("-AVX2") == 0);
  }

  { /* Multiple hwcaps. */
    setenv ("GLIBC_TUNABLES", "glibc.cpu.hwcaps=-AVX,-AVX2", 1);
    ASSERT (hwcap_allowed (NULL));
    ASSERT (hwcap_allowed ("-AVX") == 0);
    ASSERT (hwcap_allowed ("-AVX2") == 0);
  }

  { /* last hwcaps takes precedence.  */
    setenv ("GLIBC_TUNABLES", "glibc.cpu.hwcaps=-AVX:"
                              "glibc.cpu.hwcaps=-AVX2", 1);
    ASSERT (hwcap_allowed (NULL));
    ASSERT (hwcap_allowed ("-AVX"));
    ASSERT (hwcap_allowed ("-AVX2") == 0);
  }

  { /* another tunable before hwcaps.  */
    setenv ("GLIBC_TUNABLES", ":foo=bar:glibc.cpu.hwcaps=-AVX,-AVX2", 1);
    ASSERT (hwcap_allowed (NULL));
    ASSERT (hwcap_allowed ("-AVX") == 0);
    ASSERT (hwcap_allowed ("-AVX2") == 0);
  }

  { /* another tunable after hwcaps.  */
    setenv ("GLIBC_TUNABLES", "glibc.cpu.hwcaps=-AVX,-AVX2:foo=bar", 1);
    ASSERT (hwcap_allowed (NULL));
    ASSERT (hwcap_allowed ("-AVX") == 0);
    ASSERT (hwcap_allowed ("-AVX2") == 0);
  }

  { /* Unsupported features not matched by cpu_*().  */
    setenv ("GLIBC_TUNABLES", "glibc.cpu.hwcaps=-FOO", 1);
    ASSERT (cpu_may_support ("foo"));
    ASSERT (hwcap_allowed ("-FOO") == 0);
  }

  return test_exit_status;
}

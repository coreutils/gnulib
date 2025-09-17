/* __builtin_cpu_supports() wrapper that honors GLIBC_TUNABLES=glibc.cpu.hwcaps

   Copyright 2025 Free Software Foundation, Inc.

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

#ifndef _CPU_SUPPORTS_H
# define _CPU_SUPPORTS_H

# include <string.h>
# include "attribute.h"

/* The main interface to this module is cpu_supports("feature"),
   which is like __builtin_cpu_supports("feature"), but we also check if
   the feature has been disabled by the GLIBC_TUNABLES env variable.

   Notes:

   You can call this with any "feature" accepted by __builtin_cpu_supports(),
   but we only check that the feature is disabled against the set listed
   in gcc_feature_to_glibc_hwcap() below.

   We don't currently support amalgamated feature checks like
   cpu_supports("feature1+feature2").

   We do not need glibc support for GLIBC_TUNABLES,
   rather mimic that functionality on all systems.  */

# define cpu_supports(feature) \
  (cpu_may_support (feature) && 0 < __builtin_cpu_supports (feature))


/* Check if the feature has been disabled by GLIBC_TUNABLES,
   but do NOT call __builtin_cpu_supports(), as some platforms
   use other interfaces like getauxval() instead.  */

# define cpu_may_support(feature) \
  (hwcap_allowed (gcc_feature_to_glibc_hwcap (feature)))



/* Return the glibc.cpu.hwcaps setting (prepended with "-"),
   corresponding to the passed gcc _builtin_cpu_supports(FEATURE).
   Supported hwcaps can be identified from the bit_cpu_* defines
   in GLIBC's sysdeps/x86/include/cpu-features.h
   Note this mapping should resolve at compile time.  */
ATTRIBUTE_PURE
static inline char const *
gcc_feature_to_glibc_hwcap (char const *feature)
{
  char const *hwcap = NULL;

  if (0) {}
  /* x64_64 */
  else if (streq (feature, "avx"))          hwcap = "-AVX";
  else if (streq (feature, "avx2"))         hwcap = "-AVX2";
  else if (streq (feature, "avx512bw"))     hwcap = "-AVX512BW";
  else if (streq (feature, "avx512f"))      hwcap = "-AVX512F";
  else if (streq (feature, "pclmul"))       hwcap = "-PCLMULQDQ";
  else if (streq (feature, "vpclmulqdq"))   hwcap = "-VPCLMULQDQ";
  /* aarch64 */
  else if (streq (feature, "pmull"))        hwcap = "-PMULL";

  return hwcap;
}

/* Support GLIBC's interface to disable features using:
    export GLIBC_TUNABLES=glibc.cpu.hwcaps=-AVX512F,-AVX2,-AVX,-PMULL
   Return true if the HWCAP is allowed.  */
extern bool hwcap_allowed (char const *glibc_hwcap);

/* Set to true to enable reparsing of GLIBC_TUNABLES on each call.
   This can be useful for testing.  Off by default.  */
extern bool hwcap_allowed_nocache;

#endif /* _CPU_SUPPORTS_H */

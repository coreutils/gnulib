/*
 * Copyright (C) 2004, 2007-2025 Free Software Foundation, Inc.
 * Written by Bruno Haible and Eric Blake
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include <wchar.h>

#include "signature.h"
SIGNATURE_CHECK (wcsstr, wchar_t *, (wchar_t const *, wchar_t const *));

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "zerosize-ptr.h"
#include "macros.h"

int
main (int argc, char *argv[])
{
#if HAVE_DECL_ALARM
  /* Declare failure if test takes too long, by using default abort
     caused by SIGALRM.  All known platforms that lack alarm also have
     a quadratic wcsstr, and the replacement wcsstr is known to not
     take too long.  */
  int alarm_value = 50;
  signal (SIGALRM, SIG_DFL);
  alarm (alarm_value);
#endif

  {
    const wchar_t input[] = L"foo";
    const wchar_t *result = wcsstr (input, L"");
    ASSERT (result == input);
  }

  {
    const wchar_t input[] = L"foo";
    const wchar_t *result = wcsstr (input, L"o");
    ASSERT (result == input + 1);
  }

  {
    /* On some platforms, the memchr() functions reads past the first
       occurrence of the byte to be searched, leading to an out-of-bounds
       read access for wcsstr().
       See <https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=521737>.
       This is a bug in memchr(), see the Austin Group's clarification
       <https://www.opengroup.org/austin/docs/austin_454.txt>.  */
    const wchar_t *fix = L"aBaaaaaaaaaaax";
    wchar_t *page_boundary = (wchar_t *) zerosize_ptr ();
    size_t len = wcslen (fix) + 1;
    wchar_t *input =
      page_boundary ? page_boundary - len : malloc (len * sizeof (wchar_t));
    const wchar_t *result;

    wcscpy (input, fix);
    result = wcsstr (input, L"B1x");
    ASSERT (result == NULL);
    if (!page_boundary)
      free (input);
  }

  {
    const wchar_t input[] = L"ABC ABCDAB ABCDABCDABDE";
    const wchar_t *result = wcsstr (input, L"ABCDABD");
    ASSERT (result == input + 15);
  }

  {
    const wchar_t input[] = L"ABC ABCDAB ABCDABCDABDE";
    const wchar_t *result = wcsstr (input, L"ABCDABE");
    ASSERT (result == NULL);
  }

  {
    const wchar_t input[] = L"ABC ABCDAB ABCDABCDABDE";
    const wchar_t *result = wcsstr (input, L"ABCDABCD");
    ASSERT (result == input + 11);
  }

  /* Check that a long periodic needle does not cause false positives.  */
  {
    const wchar_t input[] = L"F_BD_CE_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD"
                             "_C3_88_20_EF_BF_BD_EF_BF_BD_EF_BF_BD"
                             "_C3_A7_20_EF_BF_BD";
    const wchar_t need[] = L"_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD";
    const wchar_t *result = wcsstr (input, need);
    ASSERT (result == NULL);
  }
  {
    const wchar_t input[] = L"F_BD_CE_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD"
                             "_C3_88_20_EF_BF_BD_EF_BF_BD_EF_BF_BD"
                             "_C3_A7_20_EF_BF_BD_DA_B5_C2_A6_20"
                             "_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD";
    const wchar_t need[] = L"_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD";
    const wchar_t *result = wcsstr (input, need);
    ASSERT (result == input + 115);
  }

  /* Check that a very long haystack is handled quickly if the needle is
     short and occurs near the beginning.  */
  {
    size_t repeat = 10000;
    size_t m = 1000000;
    const wchar_t *needle =
      L"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
       "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    wchar_t *haystack = (wchar_t *) malloc ((m + 1) * sizeof (wchar_t));
    if (haystack != NULL)
      {
        wmemset (haystack, L'A', m);
        haystack[0] = L'B';
        haystack[m] = L'\0';

        for (; repeat > 0; repeat--)
          {
            ASSERT (wcsstr (haystack, needle) == haystack + 1);
          }

        free (haystack);
      }
  }

  /* Check that a very long needle is discarded quickly if the haystack is
     short.  */
  {
    size_t repeat = 10000;
    size_t m = 1000000;
    const wchar_t *haystack =
      L"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
       "ABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABAB";
    wchar_t *needle = (wchar_t *) malloc ((m + 1) * sizeof (wchar_t));
    if (needle != NULL)
      {
        wmemset (needle, L'A', m);
        needle[m] = L'\0';

        for (; repeat > 0; repeat--)
          {
            ASSERT (wcsstr (haystack, needle) == NULL);
          }

        free (needle);
      }
  }

  /* Check that the asymptotic worst-case complexity is not quadratic.  */
  {
    size_t m = 1000000;
    wchar_t *haystack = (wchar_t *) malloc ((2 * m + 2) * sizeof (wchar_t));
    wchar_t *needle = (wchar_t *) malloc ((m + 2) * sizeof (wchar_t));
    if (haystack != NULL && needle != NULL)
      {
        const wchar_t *result;

        wmemset (haystack, L'A', 2 * m);
        haystack[2 * m] = L'B';
        haystack[2 * m + 1] = L'\0';

        wmemset (needle, L'A', m);
        needle[m] = L'B';
        needle[m + 1] = L'\0';

        result = wcsstr (haystack, needle);
        ASSERT (result == haystack + m);
      }
    free (needle);
    free (haystack);
  }

  /* Sublinear speed is only possible in memmem; wcsstr must examine
     every character of haystack to find its length.  */


  {
    /* Ensure that with a barely periodic "short" needle, wcsstr's
       search does not mistakenly skip just past the match point.  */
    const wchar_t *haystack =
      L"\n"
       "with_build_libsubdir\n"
       "with_local_prefix\n"
       "with_gxx_include_dir\n"
       "with_cpp_install_dir\n"
       "enable_generated_files_in_srcdir\n"
       "with_gnu_ld\n"
       "with_ld\n"
       "with_demangler_in_ld\n"
       "with_gnu_as\n"
       "with_as\n"
       "enable_largefile\n"
       "enable_werror_always\n"
       "enable_checking\n"
       "enable_coverage\n"
       "enable_gather_detailed_mem_stats\n"
       "enable_build_with_cxx\n"
       "with_stabs\n"
       "enable_multilib\n"
       "enable___cxa_atexit\n"
       "enable_decimal_float\n"
       "enable_fixed_point\n"
       "enable_threads\n"
       "enable_tls\n"
       "enable_objc_gc\n"
       "with_dwarf2\n"
       "enable_shared\n"
       "with_build_sysroot\n"
       "with_sysroot\n"
       "with_specs\n"
       "with_pkgversion\n"
       "with_bugurl\n"
       "enable_languages\n"
       "with_multilib_list\n";
    const wchar_t *needle =
      L"\n"
       "with_gnu_ld\n";
    const wchar_t* p = wcsstr (haystack, needle);
    ASSERT (p - haystack == 114);
  }

  {
    /* Same bug, shorter trigger.  */
    const wchar_t *haystack = L"..wi.d.";
    const wchar_t *needle = L".d.";
    const wchar_t* p = wcsstr (haystack, needle);
    ASSERT (p - haystack == 4);
  }

  /* Test case from Yves Bastide.
     <https://www.openwall.com/lists/musl/2014/04/18/2>  */
  {
    const wchar_t input[] = L"playing play play play always";
    const wchar_t *result = wcsstr (input, L"play play play");
    ASSERT (result == input + 8);
  }

  /* Test long needles.  */
  {
    size_t m = 1024;
    wchar_t *haystack = (wchar_t *) malloc ((2 * m + 1) * sizeof (wchar_t));
    wchar_t *needle = (wchar_t *) malloc ((m + 1) * sizeof (wchar_t));
    if (haystack != NULL && needle != NULL)
      {
        const wchar_t *p;
        haystack[0] = L'x';
        wmemset (haystack + 1, L' ', m - 1);
        wmemset (haystack + m, L'x', m);
        haystack[2 * m] = L'\0';
        wmemset (needle, L'x', m);
        needle[m] = L'\0';
        p = wcsstr (haystack, needle);
        ASSERT (p);
        ASSERT (p - haystack == m);
      }
    free (needle);
    free (haystack);
  }

  return test_exit_status;
}

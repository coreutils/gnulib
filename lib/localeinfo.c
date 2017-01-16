/* locale information

   Copyright 2016-2017 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

/* Written by Paul Eggert.  */

#include <config.h>

#include <localeinfo.h>

#include <verify.h>

#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

/* The sbclen implementation relies on this.  */
verify (MB_LEN_MAX <= SCHAR_MAX);

/* Return true if the locale uses UTF-8.  */

static bool
is_using_utf8 (void)
{
  wchar_t wc;
  mbstate_t mbs = {0};
  return mbrtowc (&wc, "\xc4\x80", 2, &mbs) == 2 && wc == 0x100;
}

/* Initialize *LOCALEINFO from the current locale.  */

void
init_localeinfo (struct localeinfo *localeinfo)
{
  int i;

  localeinfo->multibyte = MB_CUR_MAX > 1;
  localeinfo->using_utf8 = is_using_utf8 ();

  for (i = CHAR_MIN; i <= CHAR_MAX; i++)
    {
      char c = i;
      unsigned char uc = i;
      mbstate_t s = {0};
      wchar_t wc;
      size_t len = mbrtowc (&wc, &c, 1, &s);
      localeinfo->sbclen[uc] = len <= 1 ? 1 : - (int) - len;
      localeinfo->sbctowc[uc] = len <= 1 ? wc : WEOF;
    }
}

/* The set of wchar_t values C such that there's a useful locale
   somewhere where C != towupper (C) && C != towlower (towupper (C)).
   For example, 0x00B5 (U+00B5 MICRO SIGN) is in this table, because
   towupper (0x00B5) == 0x039C (U+039C GREEK CAPITAL LETTER MU), and
   towlower (0x039C) == 0x03BC (U+03BC GREEK SMALL LETTER MU).  */
static short const lonesome_lower[] =
  {
    0x00B5, 0x0131, 0x017F, 0x01C5, 0x01C8, 0x01CB, 0x01F2, 0x0345,
    0x03C2, 0x03D0, 0x03D1, 0x03D5, 0x03D6, 0x03F0, 0x03F1,

    /* U+03F2 GREEK LUNATE SIGMA SYMBOL lacks a specific uppercase
       counterpart in locales predating Unicode 4.0.0 (April 2003).  */
    0x03F2,

    0x03F5, 0x1E9B, 0x1FBE,
  };

/* Verify that the worst case fits.  This is 1 for towupper, 1 for
   towlower, and 1 for each entry in LONESOME_LOWER.  */
verify (1 + 1 + sizeof lonesome_lower / sizeof *lonesome_lower
        <= CASE_FOLDED_BUFSIZE);

/* Find the characters equal to C after case-folding, other than C
   itself, and store them into FOLDED.  Return the number of characters
   stored; this is zero if C is WEOF.  */

int
case_folded_counterparts (wint_t c, wchar_t folded[CASE_FOLDED_BUFSIZE])
{
  int i;
  int n = 0;
  wint_t uc = towupper (c);
  wint_t lc = towlower (uc);
  if (uc != c)
    folded[n++] = uc;
  if (lc != uc && lc != c && towupper (lc) == uc)
    folded[n++] = lc;
  for (i = 0; i < sizeof lonesome_lower / sizeof *lonesome_lower; i++)
    {
      wint_t li = lonesome_lower[i];
      if (li != lc && li != uc && li != c && towupper (li) == uc)
        folded[n++] = li;
    }
  return n;
}

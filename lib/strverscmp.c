/* Compare strings while treating digits characters numerically.
   Copyright (C) 1997, 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

/* ISDIGIT differs from isdigit, as follows:
   - Its arg may be any int or unsigned int; it need not be an unsigned char.
   - It's guaranteed to evaluate its argument exactly once.
   - It's typically faster.
   Posix 1003.2-1992 section 2.5.2.1 page 50 lines 1556-1558 says that
   only '0' through '9' are digits.  Prefer ISDIGIT to isdigit unless
   it's important to use the locale's definition of `digit' even when the
   host does not conform to Posix.  */
#define ISDIGIT(c) ((unsigned) (c) - '0' <= 9)

/* Compare S1 and S2 as strings holding indices/version numbers,
   returning less than, equal to or greater than zero if S1 is less than,
   equal to or greater than S2 (for more info, see the texinfo doc).
*/

int
strverscmp (const char *s1, const char *s2)
{
  const unsigned char *p1 = (const unsigned char *) s1;
  const unsigned char *p2 = (const unsigned char *) s2;
  int diff;

  while (*p1 != '\0' && *p1 == *p2)
    {
      ++p1;
      ++p2;
    }
  diff = *p1 - *p2;
  if (ISDIGIT (*p1) || ISDIGIT (*p2))
    {
      while (ISDIGIT (*p1++))
	if (!ISDIGIT (*p2++))
	    return 1;
      if (ISDIGIT (*p2))
	return -1;
    }
  return diff;
}

#ifdef TESTING_STRVERSCMP

# include <stdio.h>

/*
BEGIN-DATA
1.010 1.01  1         # 10 > 1;  yes, these are non-intuitive, but see below.
1.010.tgz 1.01.tgz  1 # 10 > 1
1.007 1.01a 1         # 7 > 1
1.51a 1.507 -1        # 51 < 507
1.001 1.2 -1          # 1 < 2
foo-1.tar foo-1.2.tar -1
END-DATA

Justification from Karl Heuer:

  Yes, it's possible that 1.007 was meant to sort earlier than 1.01a,
  but anyone who's using that style is also going to want 1.507 to sort
  earlier than 1.51a, and I think that making opposite assumptions for
  these two examples is just asking for trouble.  People who want their
  version numbers to be sorted "floating-point style" should simply make
  the digits strings have fixed width; 1.007 < 1.010a unambiguously.)

Run this test like this:

  gcc -DTESTING_STRVERSCMP -Wall strverscmp.c
  sed -n '/^BEGIN-DATA/,/END-DATA/p' strverscmp.c|grep -v -e -DATA | ./a.out

*/

# define MAX_BUFF_LEN 1024
# define STREQ(s1, s2) ((strcmp (s1, s2) == 0))

int
main ()
{
  char buff[MAX_BUFF_LEN + 1];

  int fail = 0;
  buff[MAX_BUFF_LEN] = 0;
  while (fgets (buff, MAX_BUFF_LEN, stdin) && buff[0])
    {
      char v1[MAX_BUFF_LEN], v2[MAX_BUFF_LEN], result[MAX_BUFF_LEN];
      int r_expected, r_actual;
      sscanf (buff, "%s %s %s", v1, v2, result);
      r_expected = atoi (result);
      if (!(STREQ (result, "1") || STREQ (result, "-1") || STREQ (result, "0")))
	abort ();
      r_actual = strverscmp (v1, v2);
      if (r_expected != r_actual)
	{
	  printf ("FAIL: %s %s %s (actual: %d)\n", v1, v2, result, r_actual);
	  fail = 1;
	}
    }
  exit (fail);

}

#endif

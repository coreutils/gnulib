/* Transform wide string for comparison using the current locale.
   Copyright (C) 2011-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2011.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

size_t
wcsxfrm (wchar_t *s1, const wchar_t *s2, size_t n)
{
  char mbbuf2[1024];
  char *mbs2;

  {
    int saved_errno = errno;
    size_t result;

    /* Convert s2 to a multibyte string, trying to avoid malloc().  */
    {
      size_t ret;

      ret = wcstombs (mbbuf2, s2, sizeof (mbbuf2));
      if (ret == (size_t)-1)
        goto failed;
      if (ret < sizeof (mbbuf2))
        mbs2 = mbbuf2;
      else
        {
          size_t need = wcstombs (NULL, s2, 0);
          if (need == (size_t)-1)
            goto failed;
          mbs2 = (char *) malloc (need + 1);
          if (mbs2 == NULL)
            goto out_of_memory;
          ret = wcstombs (mbs2, s2, need + 1);
          if (ret != need)
            abort ();
        }
    }

    /* Transform the multibyte string.  */
    errno = 0;
    result = strxfrm ((char *)s1, mbs2, n);
    if (errno != 0)
      {
        /* An error occurred.  */
        if (mbs2 != mbbuf2)
          free (mbs2);
        return 0;
      }

    if (result < n)
      {
        /* Convert the result by mapping char[] -> wchar_t[].
           Since strcmp() compares the elements as 'unsigned char' values,
           whereas wcscmp() compares the elements as 'wchar_t' values, we need
           to map 1 'unsigned char' to 1 'wchar_t'.  (We could also map 2
           consecutive 'unsigned char' values to 1 'wchar_t' value, but this is
           not needed.  */
        wchar_t *wcp = s1 + n;
        char *cp = (char *)s1 + n;

        while (wcp > s1)
          *--wcp = (wchar_t) (unsigned char) *--cp;
      }

    if (mbs2 != mbbuf2)
      free (mbs2);

    /* No error.  */
    errno = saved_errno;
    return result;
  }

 out_of_memory:
  errno = ENOMEM;
  return 0;

 failed:
  errno = EILSEQ;
  return 0;
}

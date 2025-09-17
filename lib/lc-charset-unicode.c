/* Conversion between the current locale's character encoding and Unicode.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

/* Specification.  */
#include "lc-charset-unicode.h"

#if GL_CHAR32_T_IS_UNICODE && GL_CHAR32_T_VS_WCHAR_T_NEEDS_CONVERSION

/* We use iconv() to convert between a 'wchar_t' value and a Unicode code point.
   For performance reasons, we don't allocate an iconv_t for each conversion,
   but instead cache it for subsequent conversions.  Since an iconv_t descriptor
   can only be used in a single thread at a time, this cache must be
   per-thread.  */

# include <iconv.h>
# include <stdlib.h>
# include <string.h>
# include <wchar.h>

# include "localcharset.h"
# include "streq.h"
# include "glthread/once.h"
# include "glthread/tls.h"
# include "unistr.h"

/* Maximum length of encoding.  Attained for "ISO-8859-15".  */
# define MAX_ENCODING_LEN 11

struct converters
{
  iconv_t cd_locale_to_utf8;
  iconv_t cd_utf8_to_locale;
  /* NUL-terminated encoding name.  */
  char encoding[MAX_ENCODING_LEN + 1];
};

static gl_tls_key_t converters_key; /* TLS key for a 'struct converters *' */

/* Frees a 'struct converters *', for example when a thread terminates.  */
static void
free_converters (void *p)
{
  if (p != NULL)
    {
      struct converters *conv = p;
      iconv_close (conv->cd_locale_to_utf8);
      iconv_close (conv->cd_utf8_to_locale);
      free (conv);
    }
}

static void
key_init (void)
{
  gl_tls_key_init (converters_key, free_converters);
  /* The per-thread initial value is NULL.  */
}

/* Ensure that key_init is called once only.  */
gl_once_define(static, key_init_once)

/* Returns the per-thread 'struct converters *' that contains converters for the
   given encoding.  Returns NULL upon failure.  */
static struct converters *
get_converters (const char *encoding)
{
  if (strlen (encoding) > MAX_ENCODING_LEN)
    /* If this happens, increase MAX_ENCODING_LEN.  */
    return NULL;

  gl_once (key_init_once, key_init);
  struct converters *conv = gl_tls_get (converters_key);
  if (conv == NULL)
    {
      conv = (struct converters *) malloc (sizeof (struct converters));
      if (conv == NULL)
        /* Out of memory.  */
        return NULL;
      conv->cd_locale_to_utf8 = iconv_open ("UTF-8", encoding);
      conv->cd_utf8_to_locale = iconv_open (encoding, "UTF-8");
      if (conv->cd_locale_to_utf8 == (iconv_t)(-1)
          || conv->cd_utf8_to_locale == (iconv_t)(-1))
        {
          /* iconv does not support this encoding.  */
          if (conv->cd_locale_to_utf8 != (iconv_t)(-1))
            iconv_close (conv->cd_locale_to_utf8);
          if (conv->cd_utf8_to_locale != (iconv_t)(-1))
            iconv_close (conv->cd_utf8_to_locale);
          free (conv);
          return NULL;
        }
      strcpy (conv->encoding, encoding);
      gl_tls_set (converters_key, conv);
    }
  else if (!streq (conv->encoding, encoding))
    {
      /* The locale encoding of this thread changed.  */
      iconv_t new_cd_locale_to_utf8 = iconv_open ("UTF-8", encoding);
      iconv_t new_cd_utf8_to_locale = iconv_open (encoding, "UTF-8");
      if (new_cd_locale_to_utf8 == (iconv_t)(-1)
          || new_cd_utf8_to_locale == (iconv_t)(-1))
        {
          /* iconv does not support this encoding.  */
          if (new_cd_locale_to_utf8 != (iconv_t)(-1))
            iconv_close (new_cd_locale_to_utf8);
          if (new_cd_utf8_to_locale != (iconv_t)(-1))
            iconv_close (new_cd_utf8_to_locale);
          return NULL;
        }
      iconv_close (conv->cd_locale_to_utf8);
      iconv_close (conv->cd_utf8_to_locale);
      conv->cd_locale_to_utf8 = new_cd_locale_to_utf8;
      conv->cd_utf8_to_locale = new_cd_utf8_to_locale;
      strcpy (conv->encoding, encoding);
    }
  return conv;
}

char32_t
locale_encoding_to_unicode (wchar_t wc)
{
  /* This function is like a simplified variant of u32_conv_from_encoding,
     that uses a cached per-thread iconv_t instead of allocating an iconv_t
     at each call.  */
  if (wc == 0)
    /* Invalid argument.  */
    abort ();

  const char *encoding = locale_charset ();
  if (STREQ_OPT (encoding, "UTF-8", 'U', 'T', 'F', '-', '8', 0, 0, 0, 0))
    /* Assume that if the locale encoding is UTF-8, the wchar_t encoding is
       Unicode.  */
    return wc;
  if (STREQ_OPT (encoding, "ASCII", 'A', 'S', 'C', 'I', 'I', 0, 0, 0, 0))
    /* In the POSIX locale, avoid conversion errors.  */
    return wc;

  struct converters *conv = get_converters (encoding);
  if (conv == NULL)
    return 0;

  char mbbuf[64];
  size_t mbcnt;
  {
    mbstate_t state;
    mbszero (&state);
    mbcnt = wcrtomb (mbbuf, wc, &state);
    if (mbcnt > sizeof (mbbuf))
      /* wcrtomb did not recognize the wide character wc.  */
      abort ();
  }

  char utf8buf[6];
  size_t utf8cnt;
  {
    char *mbptr = mbbuf;
    size_t mbsize = mbcnt;
    char *utf8ptr = utf8buf;
    size_t utf8size = sizeof (utf8buf);
    size_t ret = iconv (conv->cd_locale_to_utf8,
                        (ICONV_CONST char **) &mbptr, &mbsize,
                        &utf8ptr, &utf8size);
    if (ret == (size_t)(-1))
      /* Conversion error.  */
      return 0;
    if (mbsize != 0)
      /* The input was not entirely converted.  */
      return 0;
    utf8cnt = sizeof (utf8buf) - utf8size; /* = utf8ptr - utf8buf */
    if (utf8cnt == 0)
      /* The conversion produced no output.  */
      return 0;
  }

  ucs4_t uc;
  if (u8_mbtouc (&uc, (const uint8_t *) utf8buf, utf8cnt) != utf8cnt)
    /* iconv produced an invalid UTF-8 byte sequence.  */
    abort ();

  return uc;
}

wchar_t
unicode_to_locale_encoding (char32_t uc)
{
  if (uc == 0)
    /* Invalid argument.  */
    abort ();

  /* This function is like a simplified variant of u32_conv_to_encoding
     that uses a cached per-thread iconv_t instead of allocating an iconv_t
     at each call.  */
  const char *encoding = locale_charset ();
  if (STREQ_OPT (encoding, "UTF-8", 'U', 'T', 'F', '-', '8', 0, 0, 0, 0))
    /* Assume that if the locale encoding is UTF-8, the wchar_t encoding is
       Unicode.  */
    return uc;
  if (STREQ_OPT (encoding, "ASCII", 'A', 'S', 'C', 'I', 'I', 0, 0, 0, 0))
    /* In the POSIX locale, avoid conversion errors.  */
    return uc;

  struct converters *conv = get_converters (encoding);
  if (conv == NULL)
    return 0;

  char utf8buf[6];
  int utf8cnt = u8_uctomb ((uint8_t *) utf8buf, uc, sizeof (utf8buf));
  if (utf8cnt < 0)
    /* Out-of-range Unicode character.  */
    return 0;

  char mbbuf[64];
  size_t mbcnt;
  {
    char *utf8ptr = utf8buf;
    size_t utf8size = utf8cnt;
    char *mbptr = mbbuf;
    size_t mbsize = sizeof (mbbuf);
    size_t ret = iconv (conv->cd_utf8_to_locale,
                        (ICONV_CONST char **) &utf8ptr, &utf8size,
                        &mbptr, &mbsize);
    if (ret == (size_t)(-1))
      /* Conversion error.  */
      return 0;
    if (utf8size != 0)
      /* The input was not entirely converted.  */
      return 0;
    mbcnt = sizeof (mbbuf) - mbsize; /* = mbptr - mbbuf */
    if (mbcnt == 0)
      /* The conversion produced no output.  */
      return 0;
  }

  wchar_t wc;
  {
    mbstate_t state;
    mbszero (&state);
    if (mbrtowc (&wc, mbbuf, mbcnt, &state) != mbcnt)
      /* iconv produced an invalid multibyte sequence.  */
      return 0;
  }

  return wc;
}

#else

/* This declaration is solely to ensure that after preprocessing
   this file is never empty.  */
typedef int dummy;

#endif

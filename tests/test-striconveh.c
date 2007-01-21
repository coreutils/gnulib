/* Test of character set conversion with error handling.
   Copyright (C) 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "striconveh.h"

#if HAVE_ICONV
# include <iconv.h>
#endif

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define SIZEOF(array) (sizeof (array) / sizeof (array[0]))
#define ASSERT(expr) if (!(expr)) abort ();

int
main ()
{
  static enum iconv_ilseq_handler handlers[] =
    { iconveh_error, iconveh_question_mark, iconveh_escape_sequence };
  size_t h;

#if HAVE_ICONV
  /* Assume that iconv() supports at least the encodings ASCII, ISO-8859-1,
     ISO-8859-2, and UTF-8.  */
  iconv_t cd_88591_to_88592 = iconv_open ("ISO-8859-2", "ISO-8859-1");
  iconv_t cd_88592_to_88591 = iconv_open ("ISO-8859-1", "ISO-8859-2");
  iconv_t cd_88591_to_utf8 = iconv_open ("UTF-8", "ISO-8859-1");
  iconv_t cd_utf8_to_88591 = iconv_open ("ISO-8859-1", "UTF-8");
  iconv_t cd_88592_to_utf8 = iconv_open ("UTF-8", "ISO-8859-2");
  iconv_t cd_utf8_to_88592 = iconv_open ("ISO-8859-2", "UTF-8");

  ASSERT (cd_88591_to_88592 != (iconv_t)(-1));
  ASSERT (cd_88592_to_88591 != (iconv_t)(-1));
  ASSERT (cd_88591_to_utf8 != (iconv_t)(-1));
  ASSERT (cd_utf8_to_88591 != (iconv_t)(-1));
  ASSERT (cd_88592_to_utf8 != (iconv_t)(-1));
  ASSERT (cd_utf8_to_88592 != (iconv_t)(-1));

  /* ------------------------ Test mem_cd_iconveh() ------------------------ */

  /* Test conversion from ISO-8859-2 to ISO-8859-1 with no errors.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "\304rger mit b\366sen B\374bchen ohne Augenma\337";
      static const char expected[] = "\304rger mit b\366sen B\374bchen ohne Augenma\337";
      char *result = NULL;
      size_t length = 0;
      int retval = mem_cd_iconveh (input, strlen (input),
				   cd_88592_to_88591,
				   cd_88592_to_utf8, cd_utf8_to_88591,
				   handler,
				   &result, &length);
      ASSERT (retval == 0);
      ASSERT (length == strlen (expected));
      ASSERT (result != NULL && memcmp (result, expected, strlen (expected)) == 0);
      free (result);
    }

  /* Test conversion from ISO-8859-2 to ISO-8859-1 with EILSEQ.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "Rafa\263 Maszkowski"; /* Rafał Maszkowski */
      char *result = NULL;
      size_t length = 0;
      int retval = mem_cd_iconveh (input, strlen (input),
				   cd_88592_to_88591,
				   cd_88592_to_utf8, cd_utf8_to_88591,
				   handler,
				   &result, &length);
      switch (handler)
	{
	case iconveh_error:
	  ASSERT (retval == -1 && errno == EILSEQ);
	  ASSERT (result == NULL);
	  break;
	case iconveh_question_mark:
	  {
	    static const char expected[] = "Rafa? Maszkowski";
	    ASSERT (retval == 0);
	    ASSERT (length == strlen (expected));
	    ASSERT (result != NULL && memcmp (result, expected, strlen (expected)) == 0);
	    free (result);
	  }
	  break;
	case iconveh_escape_sequence:
	  {
	    static const char expected[] = "Rafa\\u0142 Maszkowski";
	    ASSERT (retval == 0);
	    ASSERT (length == strlen (expected));
	    ASSERT (result != NULL && memcmp (result, expected, strlen (expected)) == 0);
	    free (result);
	  }
	  break;
	}
    }

  /* Test conversion from ISO-8859-1 to UTF-8 with no errors.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "\304rger mit b\366sen B\374bchen ohne Augenma\337";
      static const char expected[] = "\303\204rger mit b\303\266sen B\303\274bchen ohne Augenma\303\237";
      char *result = NULL;
      size_t length = 0;
      int retval = mem_cd_iconveh (input, strlen (input),
				   cd_88591_to_utf8,
				   cd_88591_to_utf8, (iconv_t)(-1),
				   handler,
				   &result, &length);
      ASSERT (retval == 0);
      ASSERT (length == strlen (expected));
      ASSERT (result != NULL && memcmp (result, expected, strlen (expected)) == 0);
      free (result);
    }

  /* Test conversion from UTF-8 to ISO-8859-1 with no errors.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "\303\204rger mit b\303\266sen B\303\274bchen ohne Augenma\303\237";
      static const char expected[] = "\304rger mit b\366sen B\374bchen ohne Augenma\337";
      char *result = NULL;
      size_t length = 0;
      int retval = mem_cd_iconveh (input, strlen (input),
				   cd_utf8_to_88591,
				   (iconv_t)(-1), cd_utf8_to_88591,
				   handler,
				   &result, &length);
      ASSERT (retval == 0);
      ASSERT (length == strlen (expected));
      ASSERT (result != NULL && memcmp (result, expected, strlen (expected)) == 0);
      free (result);
    }

  /* Test conversion from UTF-8 to ISO-8859-1 with EILSEQ.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "Rafa\305\202 Maszkowski"; /* Rafał Maszkowski */
      char *result = NULL;
      size_t length = 0;
      int retval = mem_cd_iconveh (input, strlen (input),
				   cd_utf8_to_88591,
				   (iconv_t)(-1), cd_utf8_to_88591,
				   handler,
				   &result, &length);
      switch (handler)
	{
	case iconveh_error:
	  ASSERT (retval == -1 && errno == EILSEQ);
	  ASSERT (result == NULL);
	  break;
	case iconveh_question_mark:
	  {
	    static const char expected[] = "Rafa? Maszkowski";
	    ASSERT (retval == 0);
	    ASSERT (length == strlen (expected));
	    ASSERT (result != NULL && memcmp (result, expected, strlen (expected)) == 0);
	    free (result);
	  }
	  break;
	case iconveh_escape_sequence:
	  {
	    static const char expected[] = "Rafa\\u0142 Maszkowski";
	    ASSERT (retval == 0);
	    ASSERT (length == strlen (expected));
	    ASSERT (result != NULL && memcmp (result, expected, strlen (expected)) == 0);
	    free (result);
	  }
	  break;
	}
    }

  /* Test conversion from UTF-8 to ISO-8859-1 with EINVAL.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "\342";
      char *result = NULL;
      size_t length = 0;
      int retval = mem_cd_iconveh (input, strlen (input),
				   cd_utf8_to_88591,
				   (iconv_t)(-1), cd_utf8_to_88591,
				   handler,
				   &result, &length);
      ASSERT (retval == 0);
      ASSERT (length == 0);
      if (result != NULL)
	free (result);
    }

  /* ------------------------ Test str_cd_iconveh() ------------------------ */

  /* Test conversion from ISO-8859-2 to ISO-8859-1 with no errors.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "\304rger mit b\366sen B\374bchen ohne Augenma\337";
      static const char expected[] = "\304rger mit b\366sen B\374bchen ohne Augenma\337";
      char *result = str_cd_iconveh (input,
				     cd_88592_to_88591,
				     cd_88592_to_utf8, cd_utf8_to_88591,
				     handler);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, expected) == 0);
      free (result);
    }

  /* Test conversion from ISO-8859-2 to ISO-8859-1 with EILSEQ.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "Rafa\263 Maszkowski"; /* Rafał Maszkowski */
      char *result = str_cd_iconveh (input,
				     cd_88592_to_88591,
				     cd_88592_to_utf8, cd_utf8_to_88591,
				     handler);
      switch (handler)
	{
	case iconveh_error:
	  ASSERT (result == NULL && errno == EILSEQ);
	  break;
	case iconveh_question_mark:
	  {
	    static const char expected[] = "Rafa? Maszkowski";
	    ASSERT (result != NULL);
	    ASSERT (strcmp (result, expected) == 0);
	    free (result);
	  }
	  break;
	case iconveh_escape_sequence:
	  {
	    static const char expected[] = "Rafa\\u0142 Maszkowski";
	    ASSERT (result != NULL);
	    ASSERT (strcmp (result, expected) == 0);
	    free (result);
	  }
	  break;
	}
    }

  /* Test conversion from ISO-8859-1 to UTF-8 with no errors.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "\304rger mit b\366sen B\374bchen ohne Augenma\337";
      static const char expected[] = "\303\204rger mit b\303\266sen B\303\274bchen ohne Augenma\303\237";
      char *result = str_cd_iconveh (input,
				     cd_88591_to_utf8,
				     cd_88591_to_utf8, (iconv_t)(-1),
				     handler);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, expected) == 0);
      free (result);
    }

  /* Test conversion from UTF-8 to ISO-8859-1 with no errors.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "\303\204rger mit b\303\266sen B\303\274bchen ohne Augenma\303\237";
      static const char expected[] = "\304rger mit b\366sen B\374bchen ohne Augenma\337";
      char *result = str_cd_iconveh (input,
				     cd_utf8_to_88591,
				     (iconv_t)(-1), cd_utf8_to_88591,
				     handler);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, expected) == 0);
      free (result);
    }

  /* Test conversion from UTF-8 to ISO-8859-1 with EILSEQ.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "Costs: 27 \342\202\254"; /* EURO SIGN */
      char *result = str_cd_iconveh (input,
				     cd_utf8_to_88591,
				     (iconv_t)(-1), cd_utf8_to_88591,
				     handler);
      switch (handler)
	{
	case iconveh_error:
	  ASSERT (result == NULL && errno == EILSEQ);
	  break;
	case iconveh_question_mark:
	  {
	    static const char expected[] = "Costs: 27 ?";
	    ASSERT (result != NULL);
	    ASSERT (strcmp (result, expected) == 0);
	    free (result);
	  }
	  break;
	case iconveh_escape_sequence:
	  {
	    static const char expected[] = "Costs: 27 \\u20AC";
	    ASSERT (result != NULL);
	    ASSERT (strcmp (result, expected) == 0);
	    free (result);
	  }
	  break;
	}
    }

  /* Test conversion from UTF-8 to ISO-8859-1 with EINVAL.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "\342";
      char *result = str_cd_iconveh (input,
				     cd_utf8_to_88591,
				     (iconv_t)(-1), cd_utf8_to_88591,
				     handler);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "") == 0);
      free (result);
    }

  iconv_close (cd_88591_to_88592);
  iconv_close (cd_88592_to_88591);
  iconv_close (cd_88591_to_utf8);
  iconv_close (cd_utf8_to_88591);
  iconv_close (cd_88592_to_utf8);
  iconv_close (cd_utf8_to_88592);

  /* ------------------------- Test str_iconveh() ------------------------- */

  /* Test conversion from ISO-8859-2 to ISO-8859-1 with no errors.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "\304rger mit b\366sen B\374bchen ohne Augenma\337";
      static const char expected[] = "\304rger mit b\366sen B\374bchen ohne Augenma\337";
      char *result = str_iconveh (input, "ISO-8859-2", "ISO-8859-1", handler);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, expected) == 0);
      free (result);
    }

  /* Test conversion from ISO-8859-2 to ISO-8859-1 with EILSEQ.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "Rafa\263 Maszkowski"; /* Rafał Maszkowski */
      char *result = str_iconveh (input, "ISO-8859-2", "ISO-8859-1", handler);
      switch (handler)
	{
	case iconveh_error:
	  ASSERT (result == NULL && errno == EILSEQ);
	  break;
	case iconveh_question_mark:
	  {
	    static const char expected[] = "Rafa? Maszkowski";
	    ASSERT (result != NULL);
	    ASSERT (strcmp (result, expected) == 0);
	    free (result);
	  }
	  break;
	case iconveh_escape_sequence:
	  {
	    static const char expected[] = "Rafa\\u0142 Maszkowski";
	    ASSERT (result != NULL);
	    ASSERT (strcmp (result, expected) == 0);
	    free (result);
	  }
	  break;
	}
    }

  /* Test conversion from ISO-8859-1 to UTF-8 with no errors.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "\304rger mit b\366sen B\374bchen ohne Augenma\337";
      static const char expected[] = "\303\204rger mit b\303\266sen B\303\274bchen ohne Augenma\303\237";
      char *result = str_iconveh (input, "ISO-8859-1", "UTF-8", handler);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, expected) == 0);
      free (result);
    }

  /* Test conversion from UTF-8 to ISO-8859-1 with no errors.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "\303\204rger mit b\303\266sen B\303\274bchen ohne Augenma\303\237";
      static const char expected[] = "\304rger mit b\366sen B\374bchen ohne Augenma\337";
      char *result = str_iconveh (input, "UTF-8", "ISO-8859-1", handler);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, expected) == 0);
      free (result);
    }

  /* Test conversion from UTF-8 to ISO-8859-1 with EILSEQ.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "Costs: 27 \342\202\254"; /* EURO SIGN */
      char *result = str_iconveh (input, "UTF-8", "ISO-8859-1", handler);
      switch (handler)
	{
	case iconveh_error:
	  ASSERT (result == NULL && errno == EILSEQ);
	  break;
	case iconveh_question_mark:
	  {
	    static const char expected[] = "Costs: 27 ?";
	    ASSERT (result != NULL);
	    ASSERT (strcmp (result, expected) == 0);
	    free (result);
	  }
	  break;
	case iconveh_escape_sequence:
	  {
	    static const char expected[] = "Costs: 27 \\u20AC";
	    ASSERT (result != NULL);
	    ASSERT (strcmp (result, expected) == 0);
	    free (result);
	  }
	  break;
	}
    }

  /* Test conversion from UTF-8 to ISO-8859-1 with EINVAL.  */
  for (h = 0; h < SIZEOF (handlers); h++)
    {
      enum iconv_ilseq_handler handler = handlers[h];
      static const char input[] = "\342";
      char *result = str_iconveh (input, "UTF-8", "ISO-8859-1", handler);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "") == 0);
      free (result);
    }

#endif

  return 0;
}

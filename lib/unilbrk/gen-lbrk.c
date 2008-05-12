/* Generate a Unicode conforming Line Break Properties tables from a
   UnicodeData file.
   Copyright (C) 2000-2002, 2004, 2008 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2000-2002.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Usage example:
     $ gen-lbrk /usr/local/share/Unidata/UnicodeData.txt \
		/usr/local/share/Unidata/EastAsianWidth.txt \
		/usr/local/share/Unidata/LineBreak.txt \
		5.0.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

/* This structure represents one line in the UnicodeData.txt file.  */
struct unicode_attribute
{
  const char *name;           /* Character name */
  const char *category;       /* General category */
  const char *combining;      /* Canonical combining classes */
  const char *bidi;           /* Bidirectional category */
  const char *decomposition;  /* Character decomposition mapping */
  const char *decdigit;       /* Decimal digit value */
  const char *digit;          /* Digit value */
  const char *numeric;        /* Numeric value */
  int mirrored;               /* mirrored */
  const char *oldname;        /* Old Unicode 1.0 name */
  const char *comment;        /* Comment */
  unsigned int upper;         /* Uppercase mapping */
  unsigned int lower;         /* Lowercase mapping */
  unsigned int title;         /* Titlecase mapping */
};

/* Missing fields are represented with "" for strings, and NONE for
   characters.  */
#define NONE (~(unsigned int)0)

/* The entire contents of the UnicodeData.txt file.  */
struct unicode_attribute unicode_attributes [0x110000];

/* Stores in unicode_attributes[i] the values from the given fields.  */
static void
fill_attribute (unsigned int i,
		const char *field1, const char *field2,
		const char *field3, const char *field4,
		const char *field5, const char *field6,
		const char *field7, const char *field8,
		const char *field9, const char *field10,
		const char *field11, const char *field12,
		const char *field13, const char *field14)
{
  struct unicode_attribute * uni;

  if (i >= 0x110000)
    {
      fprintf (stderr, "index too large\n");
      exit (1);
    }
  uni = &unicode_attributes[i];
  /* Copy the strings.  */
  uni->name          = strdup (field1);
  uni->category      = (field2[0] == '\0' ? "" : strdup (field2));
  uni->combining     = (field3[0] == '\0' ? "" : strdup (field3));
  uni->bidi          = (field4[0] == '\0' ? "" : strdup (field4));
  uni->decomposition = (field5[0] == '\0' ? "" : strdup (field5));
  uni->decdigit      = (field6[0] == '\0' ? "" : strdup (field6));
  uni->digit         = (field7[0] == '\0' ? "" : strdup (field7));
  uni->numeric       = (field8[0] == '\0' ? "" : strdup (field8));
  uni->mirrored      = (field9[0] == 'Y');
  uni->oldname       = (field10[0] == '\0' ? "" : strdup (field10));
  uni->comment       = (field11[0] == '\0' ? "" : strdup (field11));
  uni->upper = (field12[0] =='\0' ? NONE : strtoul (field12, NULL, 16));
  uni->lower = (field13[0] =='\0' ? NONE : strtoul (field13, NULL, 16));
  uni->title = (field14[0] =='\0' ? NONE : strtoul (field14, NULL, 16));
}

/* Maximum length of a field in the UnicodeData.txt file.  */
#define FIELDLEN 120

/* Reads the next field from STREAM.  The buffer BUFFER has size FIELDLEN.
   Reads up to (but excluding) DELIM.
   Returns 1 when a field was successfully read, otherwise 0.  */
static int
getfield (FILE *stream, char *buffer, int delim)
{
  int count = 0;
  int c;

  for (; (c = getc (stream)), (c != EOF && c != delim); )
    {
      /* The original unicode.org UnicodeData.txt file happens to have
	 CR/LF line terminators.  Silently convert to LF.  */
      if (c == '\r')
	continue;

      /* Put c into the buffer.  */
      if (++count >= FIELDLEN - 1)
	{
	  fprintf (stderr, "field too long\n");
	  exit (1);
	}
      *buffer++ = c;
    }

  if (c == EOF)
    return 0;

  *buffer = '\0';
  return 1;
}

/* Stores in unicode_attributes[] the entire contents of the UnicodeData.txt
   file.  */
static void
fill_attributes (const char *unicodedata_filename)
{
  unsigned int i, j;
  FILE *stream;
  char field0[FIELDLEN];
  char field1[FIELDLEN];
  char field2[FIELDLEN];
  char field3[FIELDLEN];
  char field4[FIELDLEN];
  char field5[FIELDLEN];
  char field6[FIELDLEN];
  char field7[FIELDLEN];
  char field8[FIELDLEN];
  char field9[FIELDLEN];
  char field10[FIELDLEN];
  char field11[FIELDLEN];
  char field12[FIELDLEN];
  char field13[FIELDLEN];
  char field14[FIELDLEN];
  int lineno = 0;

  for (i = 0; i < 0x110000; i++)
    unicode_attributes[i].name = NULL;

  stream = fopen (unicodedata_filename, "r");
  if (stream == NULL)
    {
      fprintf (stderr, "error during fopen of '%s'\n", unicodedata_filename);
      exit (1);
    }

  for (;;)
    {
      int n;

      lineno++;
      n = getfield (stream, field0, ';');
      n += getfield (stream, field1, ';');
      n += getfield (stream, field2, ';');
      n += getfield (stream, field3, ';');
      n += getfield (stream, field4, ';');
      n += getfield (stream, field5, ';');
      n += getfield (stream, field6, ';');
      n += getfield (stream, field7, ';');
      n += getfield (stream, field8, ';');
      n += getfield (stream, field9, ';');
      n += getfield (stream, field10, ';');
      n += getfield (stream, field11, ';');
      n += getfield (stream, field12, ';');
      n += getfield (stream, field13, ';');
      n += getfield (stream, field14, '\n');
      if (n == 0)
	break;
      if (n != 15)
	{
	  fprintf (stderr, "short line in'%s':%d\n",
		   unicodedata_filename, lineno);
	  exit (1);
	}
      i = strtoul (field0, NULL, 16);
      if (field1[0] == '<'
	  && strlen (field1) >= 9
	  && !strcmp (field1 + strlen(field1) - 8, ", First>"))
	{
	  /* Deal with a range. */
	  lineno++;
	  n = getfield (stream, field0, ';');
	  n += getfield (stream, field1, ';');
	  n += getfield (stream, field2, ';');
	  n += getfield (stream, field3, ';');
	  n += getfield (stream, field4, ';');
	  n += getfield (stream, field5, ';');
	  n += getfield (stream, field6, ';');
	  n += getfield (stream, field7, ';');
	  n += getfield (stream, field8, ';');
	  n += getfield (stream, field9, ';');
	  n += getfield (stream, field10, ';');
	  n += getfield (stream, field11, ';');
	  n += getfield (stream, field12, ';');
	  n += getfield (stream, field13, ';');
	  n += getfield (stream, field14, '\n');
	  if (n != 15)
	    {
	      fprintf (stderr, "missing end range in '%s':%d\n",
		       unicodedata_filename, lineno);
	      exit (1);
	    }
	  if (!(field1[0] == '<'
		&& strlen (field1) >= 8
		&& !strcmp (field1 + strlen (field1) - 7, ", Last>")))
	    {
	      fprintf (stderr, "missing end range in '%s':%d\n",
		       unicodedata_filename, lineno);
	      exit (1);
	    }
	  field1[strlen (field1) - 7] = '\0';
	  j = strtoul (field0, NULL, 16);
	  for (; i <= j; i++)
	    fill_attribute (i, field1+1, field2, field3, field4, field5,
			       field6, field7, field8, field9, field10,
			       field11, field12, field13, field14);
	}
      else
	{
	  /* Single character line */
	  fill_attribute (i, field1, field2, field3, field4, field5,
			     field6, field7, field8, field9, field10,
			     field11, field12, field13, field14);
	}
    }
  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error reading from '%s'\n", unicodedata_filename);
      exit (1);
    }
}

/* The width property from the EastAsianWidth.txt file.
   Each is NULL (unassigned) or "N", "A", "H", "W", "F", "Na".  */
const char * unicode_width[0x110000];

/* Stores in unicode_width[] the width property from the EastAsianWidth.txt
   file.  */
static void
fill_width (const char *width_filename)
{
  unsigned int i, j;
  FILE *stream;
  char field0[FIELDLEN];
  char field1[FIELDLEN];
  char field2[FIELDLEN];
  int lineno = 0;

  for (i = 0; i < 0x110000; i++)
    unicode_width[i] = (unicode_attributes[i].name != NULL ? "N" : NULL);

  stream = fopen (width_filename, "r");
  if (stream == NULL)
    {
      fprintf (stderr, "error during fopen of '%s'\n", width_filename);
      exit (1);
    }

  for (;;)
    {
      int n;
      int c;

      lineno++;
      c = getc (stream);
      if (c == EOF)
	break;
      if (c == '#')
	{
	  do c = getc (stream); while (c != EOF && c != '\n');
	  continue;
	}
      ungetc (c, stream);
      n = getfield (stream, field0, ';');
      n += getfield (stream, field1, ' ');
      n += getfield (stream, field2, '\n');
      if (n == 0)
	break;
      if (n != 3)
	{
	  fprintf (stderr, "short line in '%s':%d\n", width_filename, lineno);
	  exit (1);
	}
      i = strtoul (field0, NULL, 16);
      if (strstr (field0, "..") != NULL)
	{
	  /* Deal with a range.  */
	  j = strtoul (strstr (field0, "..") + 2, NULL, 16);
	  for (; i <= j; i++)
	    unicode_width[i] = strdup (field1);
	}
      else
	{
	  /* Single character line.  */
	  unicode_width[i] = strdup (field1);
	}
    }
  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error reading from '%s'\n", width_filename);
      exit (1);
    }
}

/* Line breaking classification.  */

enum
{
  /* Values >= 24 are resolved at run time. */
  LBP_BK = 24, /* mandatory break */
/*LBP_CR,         carriage return - not used here because it's a DOSism */
/*LBP_LF,         line feed - not used here because it's a DOSism */
  LBP_CM = 25, /* attached characters and combining marks */
/*LBP_NL,         next line - not used here because it's equivalent to LBP_BK */
/*LBP_SG,         surrogates - not used here because they are not characters */
  LBP_WJ =  0, /* word joiner */
  LBP_ZW = 26, /* zero width space */
  LBP_GL =  1, /* non-breaking (glue) */
  LBP_SP = 27, /* space */
  LBP_B2 =  2, /* break opportunity before and after */
  LBP_BA =  3, /* break opportunity after */
  LBP_BB =  4, /* break opportunity before */
  LBP_HY =  5, /* hyphen */
  LBP_CB = 28, /* contingent break opportunity */
  LBP_CL =  6, /* closing punctuation */
  LBP_EX =  7, /* exclamation/interrogation */
  LBP_IN =  8, /* inseparable */
  LBP_NS =  9, /* non starter */
  LBP_OP = 10, /* opening punctuation */
  LBP_QU = 11, /* ambiguous quotation */
  LBP_IS = 12, /* infix separator (numeric) */
  LBP_NU = 13, /* numeric */
  LBP_PO = 14, /* postfix (numeric) */
  LBP_PR = 15, /* prefix (numeric) */
  LBP_SY = 16, /* symbols allowing breaks */
  LBP_AI = 29, /* ambiguous (alphabetic or ideograph) */
  LBP_AL = 17, /* ordinary alphabetic and symbol characters */
  LBP_H2 = 18, /* Hangul LV syllable */
  LBP_H3 = 19, /* Hangul LVT syllable */
  LBP_ID = 20, /* ideographic */
  LBP_JL = 21, /* Hangul L Jamo */
  LBP_JV = 22, /* Hangul V Jamo */
  LBP_JT = 23, /* Hangul T Jamo */
  LBP_SA = 30, /* complex context (South East Asian) */
  LBP_XX = 31  /* unknown */
};

/* Returns the line breaking classification for ch, as a bit mask.  */
static int
get_lbp (unsigned int ch)
{
  int attr = 0;

  if (unicode_attributes[ch].name != NULL)
    {
      /* mandatory break */
      if (ch == 0x000A || ch == 0x000D || ch == 0x0085 /* newline */
	  || ch == 0x000C /* form feed */
	  || ch == 0x000B /* line tabulation */
	  || ch == 0x2028 /* LINE SEPARATOR */
	  || ch == 0x2029 /* PARAGRAPH SEPARATOR */)
	attr |= 1 << LBP_BK;

      if (ch == 0x2060 /* WORD JOINER */
	  || ch == 0xFEFF /* ZERO WIDTH NO-BREAK SPACE */)
	attr |= 1 << LBP_WJ;

      /* zero width space */
      if (ch == 0x200B /* ZERO WIDTH SPACE */)
	attr |= 1 << LBP_ZW;

      /* non-breaking (glue) */
      if (ch == 0x00A0 /* NO-BREAK SPACE */
	  || ch == 0x202F /* NARROW NO-BREAK SPACE */
	  || ch == 0x180E /* MONGOLIAN VOWEL SEPARATOR */
	  || ch == 0x034F /* COMBINING GRAPHEME JOINER */
	  || ch == 0x2007 /* FIGURE SPACE */
	  || ch == 0x2011 /* NON-BREAKING HYPHEN */
	  || ch == 0x0F08 /* TIBETAN MARK SBRUL SHAD */
	  || ch == 0x0F0C /* TIBETAN MARK DELIMITER TSHEG BSTAR */
	  || ch == 0x0F12 /* TIBETAN MARK RGYA GRAM SHAD */
	  || (ch >= 0x035C && ch <= 0x0362) /* COMBINING DOUBLE ... */)
	attr |= 1 << LBP_GL;

      /* space */
      if (ch == 0x0020 /* SPACE */)
	attr |= 1 << LBP_SP;

      /* break opportunity before and after */
      if (ch == 0x2014 /* EM DASH */)
	attr |= 1 << LBP_B2;

      /* break opportunity after */
      if (ch == 0x1680 /* OGHAM SPACE MARK */
	  || ch == 0x2000 /* EN QUAD */
	  || ch == 0x2001 /* EM QUAD */
	  || ch == 0x2002 /* EN SPACE */
	  || ch == 0x2003 /* EM SPACE */
	  || ch == 0x2004 /* THREE-PER-EM SPACE */
	  || ch == 0x2005 /* FOUR-PER-EM SPACE */
	  || ch == 0x2006 /* SIX-PER-EM SPACE */
	  || ch == 0x2008 /* PUNCTUATION SPACE */
	  || ch == 0x2009 /* THIN SPACE */
	  || ch == 0x200A /* HAIR SPACE */
	  || ch == 0x205F /* MEDIUM MATHEMATICAL SPACE */
	  || ch == 0x0009 /* tab */
	  || ch == 0x00AD /* SOFT HYPHEN */
	  || ch == 0x058A /* ARMENIAN HYPHEN */
	  || ch == 0x2010 /* HYPHEN */
	  || ch == 0x2012 /* FIGURE DASH */
	  || ch == 0x2013 /* EN DASH */
	  || ch == 0x05BE /* HEBREW PUNCTUATION MAQAF */
	  || ch == 0x0F0B /* TIBETAN MARK INTERSYLLABIC TSHEG */
	  || ch == 0x1361 /* ETHIOPIC WORDSPACE */
	  || ch == 0x17D8 /* KHMER SIGN BEYYAL */
	  || ch == 0x17DA /* KHMER SIGN KOOMUUT */
	  || ch == 0x2027 /* HYPHENATION POINT */
	  || ch == 0x007C /* VERTICAL LINE */
	  || ch == 0x16EB /* RUNIC SINGLE PUNCTUATION */
	  || ch == 0x16EC /* RUNIC MULTIPLE PUNCTUATION */
	  || ch == 0x16ED /* RUNIC CROSS PUNCTUATION */
	  || ch == 0x2056 /* THREE DOT PUNCTUATION */
	  || ch == 0x2058 /* FOUR DOT PUNCTUATION */
	  || ch == 0x2059 /* FIVE DOT PUNCTUATION */
	  || ch == 0x205A /* TWO DOT PUNCTUATION */
	  || ch == 0x205B /* FOUR DOT MARK */
	  || ch == 0x205D /* TRICOLON */
	  || ch == 0x205E /* VERTICAL FOUR DOTS */
	  || ch == 0x2E19 /* PALM BRANCH */
	  || ch == 0x2E2A /* TWO DOTS OVER ONE DOT PUNCTUATION */
	  || ch == 0x2E2B /* ONE DOT OVER TWO DOTS PUNCTUATION */
	  || ch == 0x2E2C /* SQUARED FOUR DOT PUNCTUATION */
	  || ch == 0x2E2D /* FIVE DOT PUNCTUATION */
	  || ch == 0x2E30 /* RING POINT */
	  || ch == 0x10100 /* AEGEAN WORD SEPARATOR LINE */
	  || ch == 0x10101 /* AEGEAN WORD SEPARATOR DOT */
	  || ch == 0x10102 /* AEGEAN CHECK MARK */
	  || ch == 0x1039F /* UGARITIC WORD DIVIDER */
	  || ch == 0x103D0 /* OLD PERSIAN WORD DIVIDER */
	  || ch == 0x1091F /* PHOENICIAN WORD SEPARATOR */
	  || ch == 0x12470 /* CUNEIFORM PUNCTUATION SIGN OLD ASSYRIAN WORD DIVIDER */
	  || ch == 0x0964 /* DEVANAGARI DANDA */
	  || ch == 0x0965 /* DEVANAGARI DOUBLE DANDA */
	  || ch == 0x0E5A /* THAI CHARACTER ANGKHANKHU */
	  || ch == 0x0E5B /* THAI CHARACTER KHOMUT */
	  || ch == 0x104A /* MYANMAR SIGN LITTLE SECTION */
	  || ch == 0x104B /* MYANMAR SIGN SECTION */
	  || ch == 0x1735 /* PHILIPPINE SINGLE PUNCTUATION */
	  || ch == 0x1736 /* PHILIPPINE DOUBLE PUNCTUATION */
	  || ch == 0x17D4 /* KHMER SIGN KHAN */
	  || ch == 0x17D5 /* KHMER SIGN BARIYOOSAN */
	  || ch == 0x1B5E /* BALINESE CARIK SIKI */
	  || ch == 0x1B5F /* BALINESE CARIK PAREREN */
	  || ch == 0xA8CE /* SAURASHTRA DANDA */
	  || ch == 0xA8CF /* SAURASHTRA DOUBLE DANDA */
	  || ch == 0xAA5D /* CHAM PUNCTUATION DANDA */
	  || ch == 0xAA5E /* CHAM PUNCTUATION DOUBLE DANDA */
	  || ch == 0xAA5F /* CHAM PUNCTUATION TRIPLE DANDA */
	  || ch == 0x10A56 /* KHAROSHTHI PUNCTUATION DANDA */
	  || ch == 0x10A57 /* KHAROSHTHI PUNCTUATION DOUBLE DANDA */
	  || ch == 0x0F34 /* TIBETAN MARK BSDUS RTAGS */
	  || ch == 0x0F7F /* TIBETAN SIGN RNAM BCAD */
	  || ch == 0x0F85 /* TIBETAN MARK PALUTA */
	  || ch == 0x0FBE /* TIBETAN KU RU KHA */
	  || ch == 0x0FBF /* TIBETAN KU RU KHA BZHI MIG CAN */
	  || ch == 0x0FD2 /* TIBETAN MARK NYIS TSHEG */
#if !REVISION_22
	  || ch == 0x1802 /* MONGOLIAN COMMA */
	  || ch == 0x1803 /* MONGOLIAN FULL STOP */
#endif
	  || ch == 0x1804 /* MONGOLIAN COLON */
	  || ch == 0x1805 /* MONGOLIAN FOUR DOTS */
#if !REVISION_22
	  || ch == 0x1808 /* MONGOLIAN MANCHU COMMA */
	  || ch == 0x1809 /* MONGOLIAN MANCHU FULL STOP */
#endif
	  || ch == 0x1B5A /* BALINESE PANTI */
	  || ch == 0x1B5B /* BALINESE PAMADA */
	  || ch == 0x1B5C /* BALINESE WINDU */
	  || ch == 0x1B5D /* BALINESE CARIK PAMUNGKAH */
	  || ch == 0x1B60 /* BALINESE PAMENENG */
	  || ch == 0x1C3B /* LEPCHA PUNCTUATION TA-ROL */
	  || ch == 0x1C3C /* LEPCHA PUNCTUATION NYET THYOOM TA-ROL */
	  || ch == 0x1C3D /* LEPCHA PUNCTUATION CER-WA */
	  || ch == 0x1C3E /* LEPCHA PUNCTUATION TSHOOK CER-WA */
	  || ch == 0x1C3F /* LEPCHA PUNCTUATION TSHOOK */
	  || ch == 0x1C7E /* OL CHIKI PUNCTUATION MUCAAD */
	  || ch == 0x1C7F /* OL CHIKI PUNCTUATION DOUBLE MUCAAD */
#if !REVISION_22
	  || ch == 0x2CF9 /* COPTIC OLD NUBIAN FULL STOP */
#endif
	  || ch == 0x2CFA /* COPTIC OLD NUBIAN DIRECT QUESTION MARK */
	  || ch == 0x2CFB /* COPTIC OLD NUBIAN INDIRECT QUESTION MARK */
	  || ch == 0x2CFC /* COPTIC OLD NUBIAN VERSE DIVIDER */
#if !REVISION_22
	  || ch == 0x2CFE /* COPTIC FULL STOP */
#endif
	  || ch == 0x2CFF /* COPTIC MORPHOLOGICAL DIVIDER */
	  || (ch >= 0x2E0E && ch <= 0x2E15) /* EDITORIAL CORONIS .. UPWARDS ANCORA */
	  || ch == 0x2E17 /* DOUBLE OBLIQUE HYPHEN */
	  || ch == 0xA60D /* VAI COMMA */
	  || ch == 0xA60F /* VAI QUESTION MARK */
	  || ch == 0xA92E /* KAYAH LI SIGN CWI */
	  || ch == 0xA92F /* KAYAH LI SIGN SHYA */
	  || ch == 0x10A50 /* KHAROSHTHI PUNCTUATION DOT */
	  || ch == 0x10A51 /* KHAROSHTHI PUNCTUATION SMALL CIRCLE */
	  || ch == 0x10A52 /* KHAROSHTHI PUNCTUATION CIRCLE */
	  || ch == 0x10A53 /* KHAROSHTHI PUNCTUATION CRESCENT BAR */
	  || ch == 0x10A54 /* KHAROSHTHI PUNCTUATION MANGALAM */
	  || ch == 0x10A55 /* KHAROSHTHI PUNCTUATION LOTUS */
	  /* Extra characters for compatibility with Unicode LineBreak.txt.  */
#if !REVISION_22
	  || ch == 0x1A1E /* BUGINESE PALLAWA */
#endif
	  || ch == 0x12471 /* CUNEIFORM PUNCTUATION SIGN VERTICAL COLON */
	  || ch == 0x12472 /* CUNEIFORM PUNCTUATION SIGN DIAGONAL COLON */
	  || ch == 0x12473 /* CUNEIFORM PUNCTUATION SIGN DIAGONAL TRICOLON */)
	attr |= 1 << LBP_BA;

      /* break opportunity before */
      if (ch == 0x00B4 /* ACUTE ACCENT */
#if REVISION_22
	  || ch == 0x1FFD /* GREEK OXIA */
	  || ch == 0x02DF /* MODIFIER LETTER CROSS ACCENT */
#endif
	  || ch == 0x02C8 /* MODIFIER LETTER VERTICAL LINE */
	  || ch == 0x02CC /* MODIFIER LETTER LOW VERTICAL LINE */
	  || ch == 0x0F01 /* TIBETAN MARK GTER YIG MGO TRUNCATED A */
	  || ch == 0x0F02 /* TIBETAN MARK GTER YIG MGO -UM RNAM BCAD MA */
	  || ch == 0x0F03 /* TIBETAN MARK GTER YIG MGO -UM GTER TSHEG MA */
	  || ch == 0x0F04 /* TIBETAN MARK INITIAL YIG MGO MDUN MA */
	  || ch == 0x0F06 /* TIBETAN MARK CARET YIG MGO PHUR SHAD MA */
	  || ch == 0x0F07 /* TIBETAN MARK YIG MGO TSHEG SHAD MA */
	  || ch == 0x0F09 /* TIBETAN MARK BSKUR YIG MGO */
	  || ch == 0x0F0A /* TIBETAN MARK BKA- SHOG YIG MGO */
	  || ch == 0x0FD0 /* TIBETAN MARK BSKA- SHOG GI MGO RGYAN */
	  || ch == 0x0FD1 /* TIBETAN MARK MNYAM YIG GI MGO RGYAN */
	  || ch == 0x0FD3 /* TIBETAN MARK INITIAL BRDA RNYING YIG MGO MDUN MA */
	  || ch == 0xA874 /* PHAGS-PA SINGLE HEAD MARK */
	  || ch == 0xA875 /* PHAGS-PA DOUBLE HEAD MARK */
	  || ch == 0x1806 /* MONGOLIAN TODO SOFT HYPHEN */)
	attr |= 1 << LBP_BB;

      /* hyphen */
      if (ch == 0x002D /* HYPHEN-MINUS */)
	attr |= 1 << LBP_HY;

      /* contingent break opportunity */
      if (ch == 0xFFFC /* OBJECT REPLACEMENT CHARACTER */)
	attr |= 1 << LBP_CB;

      /* closing punctuation */
      if ((unicode_attributes[ch].category[0] == 'P'
	   && unicode_attributes[ch].category[1] == 'e')
	  || ch == 0x3001 /* IDEOGRAPHIC COMMA */
	  || ch == 0x3002 /* IDEOGRAPHIC FULL STOP */
	  || ch == 0xFE11 /* PRESENTATION FORM FOR VERTICAL IDEOGRAPHIC COMMA */
	  || ch == 0xFE12 /* PRESENTATION FORM FOR VERTICAL IDEOGRAPHIC FULL STOP */
	  || ch == 0xFE50 /* SMALL COMMA */
	  || ch == 0xFE52 /* SMALL FULL STOP */
	  || ch == 0xFF0C /* FULLWIDTH COMMA */
	  || ch == 0xFF0E /* FULLWIDTH FULL STOP */
	  || ch == 0xFF61 /* HALFWIDTH IDEOGRAPHIC FULL STOP */
	  || ch == 0xFF64 /* HALFWIDTH IDEOGRAPHIC COMMA */)
	attr |= 1 << LBP_CL;

      /* exclamation/interrogation */
      if (ch == 0x0021 /* EXCLAMATION MARK */
	  || ch == 0x003F /* QUESTION MARK */
	  || ch == 0x05C6 /* HEBREW PUNCTUATION NUN HAFUKHA */
#if !REVISION_22
	  || ch == 0x060C /* ARABIC COMMA */
#endif
	  || ch == 0x061B /* ARABIC SEMICOLON */
	  || ch == 0x061E /* ARABIC TRIPLE DOT PUNCTUATION MARK */
	  || ch == 0x061F /* ARABIC QUESTION MARK */
#if !REVISION_22
	  || ch == 0x066A /* ARABIC PERCENT SIGN */
#endif
	  || ch == 0x06D4 /* ARABIC FULL STOP */
	  || ch == 0x07F9 /* NKO EXCLAMATION MARK */
	  || ch == 0x0F0D /* TIBETAN MARK SHAD */
	  || ch == 0x0F0E /* TIBETAN MARK NYIS SHAD */
	  || ch == 0x0F0F /* TIBETAN MARK TSHEG SHAD */
	  || ch == 0x0F10 /* TIBETAN MARK NYIS TSHEG SHAD */
	  || ch == 0x0F11 /* TIBETAN MARK RIN CHEN SPUNGS SHAD */
	  || ch == 0x0F14 /* TIBETAN MARK GTER TSHEG */
#if REVISION_22
	  || ch == 0x1802 /* MONGOLIAN COMMA */
	  || ch == 0x1803 /* MONGOLIAN FULL STOP */
	  || ch == 0x1808 /* MONGOLIAN MANCHU COMMA */
	  || ch == 0x1809 /* MONGOLIAN MANCHU FULL STOP */
#endif
	  || ch == 0x1944 /* LIMBU EXCLAMATION MARK */
	  || ch == 0x1945 /* LIMBU QUESTION MARK */
	  || ch == 0x2762 /* HEAVY EXCLAMATION MARK ORNAMENT */
	  || ch == 0x2763 /* HEAVY HEART EXCLAMATION MARK ORNAMENT */
#if REVISION_22
	  || ch == 0x2CF9 /* COPTIC OLD NUBIAN FULL STOP */
	  || ch == 0x2CFE /* COPTIC FULL STOP */
#endif
	  || ch == 0x2E2E /* REVERSED QUESTION MARK */
	  || ch == 0xA60C /* VAI SYLLABLE LENGTHENER */
	  || ch == 0xA60E /* VAI FULL STOP */
	  || ch == 0xA876 /* PHAGS-PA MARK SHAD */
	  || ch == 0xA877 /* PHAGS-PA MARK DOUBLE SHAD */
	  || ch == 0xFE15 /* PRESENTATION FORM FOR VERTICAL EXCLAMATION MARK */
	  || ch == 0xFE16 /* PRESENTATION FORM FOR VERTICAL QUESTION MARK */
	  || ch == 0xFE56 /* SMALL QUESTION MARK */
	  || ch == 0xFE57 /* SMALL EXCLAMATION MARK */
	  || ch == 0xFF01 /* FULLWIDTH EXCLAMATION MARK */
	  || ch == 0xFF1F /* FULLWIDTH QUESTION MARK */)
	attr |= 1 << LBP_EX;

      /* inseparable */
      if (ch == 0x2024 /* ONE DOT LEADER */
	  || ch == 0x2025 /* TWO DOT LEADER */
	  || ch == 0x2026 /* HORIZONTAL ELLIPSIS */
	  || ch == 0xFE19 /* PRESENTATION FORM FOR VERTICAL HORIZONTAL ELLIPSIS */)
	attr |= 1 << LBP_IN;

      /* non starter */
      if (ch == 0x17D6 /* KHMER SIGN CAMNUC PII KUUH */
	  || ch == 0x203C /* DOUBLE EXCLAMATION MARK */
	  || ch == 0x203D /* INTERROBANG */
	  || ch == 0x2047 /* DOUBLE QUESTION MARK */
	  || ch == 0x2048 /* QUESTION EXCLAMATION MARK */
	  || ch == 0x2049 /* EXCLAMATION QUESTION MARK */
	  || ch == 0x3005 /* IDEOGRAPHIC ITERATION MARK */
	  || ch == 0x301C /* WAVE DASH */
	  || ch == 0x303C /* MASU MARK */
	  || ch == 0x303B /* VERTICAL IDEOGRAPHIC ITERATION MARK */
	  || ch == 0x309B /* KATAKANA-HIRAGANA VOICED SOUND MARK */
	  || ch == 0x309C /* KATAKANA-HIRAGANA SEMI-VOICED SOUND MARK */
	  || ch == 0x309D /* HIRAGANA ITERATION MARK */
	  || ch == 0x309E /* HIRAGANA VOICED ITERATION MARK */
	  || ch == 0x30A0 /* KATAKANA-HIRAGANA DOUBLE HYPHEN */
	  || ch == 0x30FB /* KATAKANA MIDDLE DOT */
	  || ch == 0x30FC /* KATAKANA-HIRAGANA PROLONGED SOUND MARK */
	  || ch == 0x30FD /* KATAKANA ITERATION MARK */
	  || ch == 0x30FE /* KATAKANA VOICED ITERATION MARK */
	  || ch == 0xA015 /* YI SYLLABLE WU */
	  || ch == 0xFE54 /* SMALL SEMICOLON */
	  || ch == 0xFE55 /* SMALL COLON */
	  || ch == 0xFF1A /* FULLWIDTH COLON */
	  || ch == 0xFF1B /* FULLWIDTH SEMICOLON */
	  || ch == 0xFF65 /* HALFWIDTH KATAKANA MIDDLE DOT */
	  || ch == 0xFF70 /* HALFWIDTH KATAKANA-HIRAGANA PROLONGED SOUND MARK */
	  || ch == 0xFF9E /* HALFWIDTH KATAKANA VOICED SOUND MARK */
	  || ch == 0xFF9F /* HALFWIDTH KATAKANA SEMI-VOICED SOUND MARK */
	  || strstr (unicode_attributes[ch].name, "HIRAGANA LETTER SMALL ") != NULL
	  || strstr (unicode_attributes[ch].name, "KATAKANA LETTER SMALL ") != NULL)
	attr |= 1 << LBP_NS;

      /* opening punctuation */
      if ((unicode_attributes[ch].category[0] == 'P'
	   && unicode_attributes[ch].category[1] == 's')
#if REVISION_22
	  || ch == 0x00A1 /* INVERTED EXCLAMATION MARK */
	  || ch == 0x00BF /* INVERTED QUESTION MARK */
#endif
	  || ch == 0x2E18 /* INVERTED INTERROBANG */)
	attr |= 1 << LBP_OP;

      /* ambiguous quotation */
      if ((unicode_attributes[ch].category[0] == 'P'
	   && (unicode_attributes[ch].category[1] == 'f'
	       || unicode_attributes[ch].category[1] == 'i'))
	  || ch == 0x0022 /* QUOTATION MARK */
	  || ch == 0x0027 /* APOSTROPHE */
	  || ch == 0x275B /* HEAVY SINGLE TURNED COMMA QUOTATION MARK ORNAMENT */
	  || ch == 0x275C /* HEAVY SINGLE COMMA QUOTATION MARK ORNAMENT */
	  || ch == 0x275D /* HEAVY DOUBLE TURNED COMMA QUOTATION MARK ORNAMENT */
	  || ch == 0x275E /* HEAVY DOUBLE COMMA QUOTATION MARK ORNAMENT */
	  || ch == 0x2E00 /* RIGHT ANGLE SUBSTITUTION MARKER */
	  || ch == 0x2E01 /* RIGHT ANGLE DOTTED SUBSTITUTION MARKER */
	  || ch == 0x2E06 /* RAISED INTERPOLATION MARKER */
	  || ch == 0x2E07 /* RAISED DOTTED INTERPOLATION MARKER */
	  || ch == 0x2E08 /* DOTTED TRANSPOSITION MARKER */
	  || ch == 0x2E0B /* RAISED SQUARE */)
	attr |= 1 << LBP_QU;

      /* infix separator (numeric) */
      if (ch == 0x002C /* COMMA */
	  || ch == 0x002E /* FULL STOP */
	  || ch == 0x003A /* COLON */
	  || ch == 0x003B /* SEMICOLON */
	  || ch == 0x037E /* GREEK QUESTION MARK */
	  || ch == 0x0589 /* ARMENIAN FULL STOP */
#if REVISION_22
	  || ch == 0x060C /* ARABIC COMMA */
#endif
	  || ch == 0x060D /* ARABIC DATE SEPARATOR */
	  || ch == 0x07F8 /* NKO COMMA */
	  || ch == 0x2044 /* FRACTION SLASH */
	  || ch == 0xFE10 /* PRESENTATION FORM FOR VERTICAL COMMA */
	  || ch == 0xFE13 /* PRESENTATION FORM FOR VERTICAL COLON */
	  || ch == 0xFE14 /* PRESENTATION FORM FOR VERTICAL SEMICOLON */)
	attr |= 1 << LBP_IS;

      /* numeric */
      if ((unicode_attributes[ch].category[0] == 'N'
	   && unicode_attributes[ch].category[1] == 'd'
	   && strstr (unicode_attributes[ch].name, "FULLWIDTH") == NULL)
	  || ch == 0x066B /* ARABIC DECIMAL SEPARATOR */
	  || ch == 0x066C /* ARABIC THOUSANDS SEPARATOR */)
	attr |= 1 << LBP_NU;

      /* postfix (numeric) */
      if (ch == 0x0025 /* PERCENT SIGN */
	  || ch == 0x00A2 /* CENT SIGN */
	  || ch == 0x00B0 /* DEGREE SIGN */
	  || ch == 0x060B /* AFGHANI SIGN */
#if REVISION_22
	  || ch == 0x066A /* ARABIC PERCENT SIGN */
#endif
	  || ch == 0x2030 /* PER MILLE SIGN */
	  || ch == 0x2031 /* PER TEN THOUSAND SIGN */
	  || ch == 0x2032 /* PRIME */
	  || ch == 0x2033 /* DOUBLE PRIME */
	  || ch == 0x2034 /* TRIPLE PRIME */
	  || ch == 0x2035 /* REVERSED PRIME */
	  || ch == 0x2036 /* REVERSED DOUBLE PRIME */
	  || ch == 0x2037 /* REVERSED TRIPLE PRIME */
	  || ch == 0x20A7 /* PESETA SIGN */
	  || ch == 0x2103 /* DEGREE CELSIUS */
	  || ch == 0x2109 /* DEGREE FAHRENHEIT */
	  || ch == 0xFDFC /* RIAL SIGN */
	  || ch == 0xFE6A /* SMALL PERCENT SIGN */
	  || ch == 0xFF05 /* FULLWIDTH PERCENT SIGN */
	  || ch == 0xFFE0 /* FULLWIDTH DIGIT ZERO */)
	attr |= 1 << LBP_PO;

      /* prefix (numeric) */
      if ((unicode_attributes[ch].category[0] == 'S'
	   && unicode_attributes[ch].category[1] == 'c')
	  || ch == 0x002B /* PLUS SIGN */
	  || ch == 0x005C /* REVERSE SOLIDUS */
	  || ch == 0x00B1 /* PLUS-MINUS SIGN */
	  || ch == 0x2116 /* NUMERO SIGN */
	  || ch == 0x2212 /* MINUS SIGN */
	  || ch == 0x2213 /* MINUS-OR-PLUS SIGN */)
	if (!(attr & (1 << LBP_PO)))
	  attr |= 1 << LBP_PR;

      /* symbols allowing breaks */
      if (ch == 0x002F /* SOLIDUS */)
	attr |= 1 << LBP_SY;

      if (ch >= 0xAC00 && ch <= 0xD7A3 && ((ch - 0xAC00) % 28) == 0)
	attr |= 1 << LBP_H2;

      if (ch >= 0xAC00 && ch <= 0xD7A3 && ((ch - 0xAC00) % 28) != 0)
	attr |= 1 << LBP_H3;

      if ((ch >= 0x1100 && ch <= 0x1159) || ch == 0x115F)
	attr |= 1 << LBP_JL;

      if (ch >= 0x1160 && ch <= 0x11A2)
	attr |= 1 << LBP_JV;

      if (ch >= 0x11A8 && ch <= 0x11F9)
	attr |= 1 << LBP_JT;

      /* complex context (South East Asian) */
      if (((unicode_attributes[ch].category[0] == 'C'
	    && unicode_attributes[ch].category[1] == 'f')
	   || (unicode_attributes[ch].category[0] == 'L'
	       && (unicode_attributes[ch].category[1] == 'm'
		   || unicode_attributes[ch].category[1] == 'o'))
	   || (unicode_attributes[ch].category[0] == 'M'
	       && (unicode_attributes[ch].category[1] == 'c'
		   || unicode_attributes[ch].category[1] == 'n'))
	   /* Extra characters for compatibility with Unicode LineBreak.txt.  */
	   || ch == 0x19DE /* NEW TAI LUE SIGN LAE */
	   || ch == 0x19DF /* NEW TAI LUE SIGN LAEV */)
	  && ((ch >= 0x0E00 && ch <= 0x0EFF)
	      || (ch >= 0x1000 && ch <= 0x109F)
	      || (ch >= 0x1780 && ch <= 0x17FF)
	      || (ch >= 0x1950 && ch <= 0x19DF)))
	attr |= 1 << LBP_SA;

      /* attached characters and combining marks */
      if ((unicode_attributes[ch].category[0] == 'M'
	   && (unicode_attributes[ch].category[1] == 'c'
	       || unicode_attributes[ch].category[1] == 'e'
	       || unicode_attributes[ch].category[1] == 'n'))
	  || (unicode_attributes[ch].category[0] == 'C'
	      && (unicode_attributes[ch].category[1] == 'c'
		  || unicode_attributes[ch].category[1] == 'f')))
	if (!(attr & ((1 << LBP_BK) | (1 << LBP_BA) | (1 << LBP_GL) | (1 << LBP_SA) | (1 << LBP_WJ) | (1 << LBP_ZW))))
	  attr |= 1 << LBP_CM;

      /* ideographic */
      if ((ch >= 0x2E80 && ch <= 0x2FFF) /* CJK RADICAL, KANGXI RADICAL, IDEOGRAPHIC DESCRIPTION */
	  || ch == 0x3000 /* IDEOGRAPHIC SPACE */
	  || (ch >= 0x3040 && ch <= 0x309F) /* HIRAGANA */
	  || (ch >= 0x30A0 && ch <= 0x30FF) /* KATAKANA */
	  || (ch >= 0x3400 && ch <= 0x4DB5) /* CJK Ideograph Extension A */
	  || (ch >= 0x4E00 && ch <= 0x9FBB) /* CJK Ideograph */
	  || (ch >= 0xF900 && ch <= 0xFAD9) /* CJK COMPATIBILITY IDEOGRAPH */
	  || (ch >= 0xA000 && ch <= 0xA48F) /* YI SYLLABLE */
	  || (ch >= 0xA490 && ch <= 0xA4CF) /* YI RADICAL */
	  || ch == 0xFE62 /* SMALL PLUS SIGN */
	  || ch == 0xFE63 /* SMALL HYPHEN-MINUS */
	  || ch == 0xFE64 /* SMALL LESS-THAN SIGN */
	  || ch == 0xFE65 /* SMALL GREATER-THAN SIGN */
	  || ch == 0xFE66 /* SMALL EQUALS SIGN */
	  || (ch >= 0xFF10 && ch <= 0xFF19) /* FULLWIDTH DIGIT */
	  || (ch >= 0x20000 && ch <= 0x2A6D6) /* CJK Ideograph Extension B */
	  || (ch >= 0x2F800 && ch <= 0x2FA1D) /* CJK COMPATIBILITY IDEOGRAPH */
	  || strstr (unicode_attributes[ch].name, "FULLWIDTH LATIN ") != NULL
	  || (ch >= 0x3000 && ch <= 0x33FF
	      && !(attr & ((1 << LBP_CM) | (1 << LBP_NS) | (1 << LBP_OP) | (1 << LBP_CL))))
	  /* Extra characters for compatibility with Unicode LineBreak.txt.  */
	  || ch == 0xFE30 /* PRESENTATION FORM FOR VERTICAL TWO DOT LEADER */
	  || ch == 0xFE31 /* PRESENTATION FORM FOR VERTICAL EM DASH */
	  || ch == 0xFE32 /* PRESENTATION FORM FOR VERTICAL EN DASH */
	  || ch == 0xFE33 /* PRESENTATION FORM FOR VERTICAL LOW LINE */
	  || ch == 0xFE34 /* PRESENTATION FORM FOR VERTICAL WAVY LOW LINE */
	  || ch == 0xFE45 /* SESAME DOT */
	  || ch == 0xFE46 /* WHITE SESAME DOT */
	  || ch == 0xFE49 /* DASHED OVERLINE */
	  || ch == 0xFE4A /* CENTRELINE OVERLINE */
	  || ch == 0xFE4B /* WAVY OVERLINE */
	  || ch == 0xFE4C /* DOUBLE WAVY OVERLINE */
	  || ch == 0xFE4D /* DASHED LOW LINE */
	  || ch == 0xFE4E /* CENTRELINE LOW LINE */
	  || ch == 0xFE4F /* WAVY LOW LINE */
	  || ch == 0xFE51 /* SMALL IDEOGRAPHIC COMMA */
	  || ch == 0xFE58 /* SMALL EM DASH */
	  || ch == 0xFE5F /* SMALL NUMBER SIGN */
	  || ch == 0xFE60 /* SMALL AMPERSAND */
	  || ch == 0xFE61 /* SMALL ASTERISK */
	  || ch == 0xFE68 /* SMALL REVERSE SOLIDUS */
	  || ch == 0xFE6B /* SMALL COMMERCIAL AT */
	  || ch == 0xFF02 /* FULLWIDTH QUOTATION MARK */
	  || ch == 0xFF03 /* FULLWIDTH NUMBER SIGN */
	  || ch == 0xFF06 /* FULLWIDTH AMPERSAND */
	  || ch == 0xFF07 /* FULLWIDTH APOSTROPHE */
	  || ch == 0xFF0A /* FULLWIDTH ASTERISK */
	  || ch == 0xFF0B /* FULLWIDTH PLUS SIGN */
	  || ch == 0xFF0D /* FULLWIDTH HYPHEN-MINUS */
	  || ch == 0xFF0F /* FULLWIDTH SOLIDUS */
	  || ch == 0xFF1C /* FULLWIDTH LESS-THAN SIGN */
	  || ch == 0xFF1D /* FULLWIDTH EQUALS SIGN */
	  || ch == 0xFF1E /* FULLWIDTH GREATER-THAN SIGN */
	  || ch == 0xFF20 /* FULLWIDTH COMMERCIAL AT */
	  || ch == 0xFF3C /* FULLWIDTH REVERSE SOLIDUS */
	  || ch == 0xFF3E /* FULLWIDTH CIRCUMFLEX ACCENT */
	  || ch == 0xFF3F /* FULLWIDTH LOW LINE */
	  || ch == 0xFF40 /* FULLWIDTH GRAVE ACCENT */
	  || ch == 0xFF5C /* FULLWIDTH VERTICAL LINE */
	  || ch == 0xFF5E /* FULLWIDTH TILDE */
	  || ch == 0xFFE2 /* FULLWIDTH NOT SIGN */
	  || ch == 0xFFE3 /* FULLWIDTH MACRON */
	  || ch == 0xFFE4 /* FULLWIDTH BROKEN BAR */)
	if (!(attr & ((1 << LBP_NS) | (1 << LBP_CM))))
	  {
	    /* ambiguous (ideograph) ? */
	    if ((unicode_width[ch] != NULL
		 && unicode_width[ch][0] == 'A'
		 && ch >= 0x2000)
		|| ch == 0x24EA /* CIRCLED DIGIT ZERO */
		|| (ch >= 0x2780 && ch <= 0x2793) /* DINGBAT ... CIRCLED DIGIT ... */)
	      attr |= 1 << LBP_AI;
	    else
	      attr |= 1 << LBP_ID;
	  }

      /* ordinary alphabetic and symbol characters */
      if ((unicode_attributes[ch].category[0] == 'L'
	   && (unicode_attributes[ch].category[1] == 'u'
	       || unicode_attributes[ch].category[1] == 'l'
	       || unicode_attributes[ch].category[1] == 't'
	       || unicode_attributes[ch].category[1] == 'm'
	       || unicode_attributes[ch].category[1] == 'o'))
	  || (unicode_attributes[ch].category[0] == 'S'
	      && (unicode_attributes[ch].category[1] == 'm'
		  || unicode_attributes[ch].category[1] == 'k'
		  || unicode_attributes[ch].category[1] == 'o'))
	  || (unicode_attributes[ch].category[0] == 'N'
	      && (unicode_attributes[ch].category[1] == 'l'
		  || unicode_attributes[ch].category[1] == 'o'))
	  || (unicode_attributes[ch].category[0] == 'P'
	      && (unicode_attributes[ch].category[1] == 'c'
		  || unicode_attributes[ch].category[1] == 'd'
		  || unicode_attributes[ch].category[1] == 'o'))
	  || ch == 0x0600 /* ARABIC NUMBER SIGN */
	  || ch == 0x0601 /* ARABIC SIGN SANAH */
	  || ch == 0x0602 /* ARABIC FOOTNOTE MARKER */
	  || ch == 0x0603 /* ARABIC SIGN SAFHA */
	  || ch == 0x06DD /* ARABIC END OF AYAH */
	  || ch == 0x070F /* SYRIAC ABBREVIATION MARK */
	  || ch == 0x2061 /* FUNCTION APPLICATION */
	  || ch == 0x2062 /* INVISIBLE TIMES */
	  || ch == 0x2063 /* INVISIBLE SEPARATOR */
	  || ch == 0x2064 /* INVISIBLE PLUS */)
	if (!(attr & ((1 << LBP_GL) | (1 << LBP_B2) | (1 << LBP_BA) | (1 << LBP_BB) | (1 << LBP_HY) | (1 << LBP_CB) | (1 << LBP_CL) | (1 << LBP_EX) | (1 << LBP_IN) | (1 << LBP_NS) | (1 << LBP_OP) | (1 << LBP_QU) | (1 << LBP_IS) | (1 << LBP_NU) | (1 << LBP_PO) | (1 << LBP_PR) | (1 << LBP_SY) | (1 << LBP_H2) | (1 << LBP_H3) | (1 << LBP_JL) | (1 << LBP_JV) | (1 << LBP_JT) | (1 << LBP_SA) | (1 << LBP_ID))))
	  {
	    /* ambiguous (alphabetic) ? */
	    if ((unicode_width[ch] != NULL
		 && unicode_width[ch][0] == 'A'
		 && ch >= 0x2000
		 /* Extra exceptions for compatibility with Unicode LineBreak.txt.  */
		 && ch != 0x2022 /* BULLET */
		 && ch != 0x203E /* OVERLINE */
		 && ch != 0x2126 /* OHM SIGN */
		 && ch != 0x2153 /* VULGAR FRACTION ONE THIRD */
		 && ch != 0x215C /* VULGAR FRACTION THREE EIGHTHS */
		 && ch != 0x215D /* VULGAR FRACTION FIVE EIGHTHS */
		 && ch != 0x21B8 /* NORTH WEST ARROW TO LONG BAR */
		 && ch != 0x21B9 /* LEFTWARDS ARROW TO BAR OVER RIGHTWARDS ARROW TO BAR */
		 && ch != 0x21E7 /* UPWARDS WHITE ARROW */
		 && ch != 0x24FF /* NEGATIVE CIRCLED DIGIT ZERO */
		 && ch != 0x273D /* HEAVY TEARDROP-SPOKED ASTERISK */)
#if !REVISION_22
		|| ch == 0x00A1 /* INVERTED EXCLAMATION MARK */
		|| ch == 0x00A7 /* SECTION SIGN */
		|| ch == 0x00A8 /* DIAERESIS */
		|| ch == 0x00AA /* FEMININE ORDINAL INDICATOR */
		|| ch == 0x00B2 /* SUPERSCRIPT TWO */
		|| ch == 0x00B3 /* SUPERSCRIPT THREE */
		|| ch == 0x00B6 /* PILCROW SIGN */
		|| ch == 0x00B7 /* MIDDLE DOT */
		|| ch == 0x00B8 /* CEDILLA */
		|| ch == 0x00B9 /* SUPERSCRIPT ONE */
		|| ch == 0x00BA /* MASCULINE ORDINAL INDICATOR */
		|| ch == 0x00BC /* VULGAR FRACTION ONE QUARTER */
		|| ch == 0x00BD /* VULGAR FRACTION ONE HALF */
		|| ch == 0x00BE /* VULGAR FRACTION THREE QUARTERS */
		|| ch == 0x00BF /* INVERTED QUESTION MARK */
		|| ch == 0x00D7 /* MULTIPLICATION SIGN */
		|| ch == 0x00F7 /* DIVISION SIGN */
		|| ch == 0x02C7 /* CARON */
		|| ch == 0x02C9 /* MODIFIER LETTER MACRON */
		|| ch == 0x02CA /* MODIFIER LETTER ACUTE ACCENT */
		|| ch == 0x02CB /* MODIFIER LETTER GRAVE ACCENT */
		|| ch == 0x02CD /* MODIFIER LETTER LOW MACRON */
		|| ch == 0x02D0 /* MODIFIER LETTER TRIANGULAR COLON */
		|| ch == 0x02D8 /* BREVE */
		|| ch == 0x02D9 /* DOT ABOVE */
		|| ch == 0x02DA /* RING ABOVE */
		|| ch == 0x02DB /* OGONEK */
		|| ch == 0x02DD /* DOUBLE ACUTE ACCENT */
#endif
		|| ch == 0x24EA /* CIRCLED DIGIT ZERO */
		|| (ch >= 0x2780 && ch <= 0x2793) /* DINGBAT ... CIRCLED DIGIT ... */
		/* Extra characters for compatibility with Unicode LineBreak.txt.  */
		|| ch == 0x2155 /* VULGAR FRACTION ONE FIFTH */
		|| ch == 0x2574 /* BOX DRAWINGS LIGHT LEFT */
		|| ch == 0x2616 /* WHITE SHOGI PIECE */
		|| ch == 0x2617 /* BLACK SHOGI PIECE */)
	      attr |= 1 << LBP_AI;
	    else
	      attr |= 1 << LBP_AL;
	    attr &= ~(1 << LBP_CM);
	  }
    }

  if (attr == 0)
    /* unknown */
    attr |= 1 << LBP_XX;

  return attr;
}

/* Output the line breaking properties in a human readable format.  */
static void
debug_output_lbp (FILE *stream)
{
  unsigned int i;

  for (i = 0; i < 0x110000; i++)
    {
      int attr = get_lbp (i);
      if (attr != 1 << LBP_XX)
	{
	  fprintf (stream, "0x%04X", i);
#define PRINT_BIT(attr,bit) \
  if (attr & (1 << bit)) fprintf (stream, " " #bit);
	  PRINT_BIT(attr,LBP_BK);
	  PRINT_BIT(attr,LBP_CM);
	  PRINT_BIT(attr,LBP_WJ);
	  PRINT_BIT(attr,LBP_ZW);
	  PRINT_BIT(attr,LBP_GL);
	  PRINT_BIT(attr,LBP_SP);
	  PRINT_BIT(attr,LBP_B2);
	  PRINT_BIT(attr,LBP_BA);
	  PRINT_BIT(attr,LBP_BB);
	  PRINT_BIT(attr,LBP_HY);
	  PRINT_BIT(attr,LBP_CB);
	  PRINT_BIT(attr,LBP_CL);
	  PRINT_BIT(attr,LBP_EX);
	  PRINT_BIT(attr,LBP_IN);
	  PRINT_BIT(attr,LBP_NS);
	  PRINT_BIT(attr,LBP_OP);
	  PRINT_BIT(attr,LBP_QU);
	  PRINT_BIT(attr,LBP_IS);
	  PRINT_BIT(attr,LBP_NU);
	  PRINT_BIT(attr,LBP_PO);
	  PRINT_BIT(attr,LBP_PR);
	  PRINT_BIT(attr,LBP_SY);
	  PRINT_BIT(attr,LBP_AI);
	  PRINT_BIT(attr,LBP_AL);
	  PRINT_BIT(attr,LBP_H2);
	  PRINT_BIT(attr,LBP_H3);
	  PRINT_BIT(attr,LBP_ID);
	  PRINT_BIT(attr,LBP_JL);
	  PRINT_BIT(attr,LBP_JV);
	  PRINT_BIT(attr,LBP_JT);
	  PRINT_BIT(attr,LBP_SA);
	  PRINT_BIT(attr,LBP_XX);
#undef PRINT_BIT
	  fprintf (stream, "\n");
	}
    }
}

static void
debug_output_tables (const char *filename)
{
  FILE *stream;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  debug_output_lbp (stream);

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* The line breaking property from the LineBreak.txt file.  */
int unicode_org_lbp[0x110000];

/* Stores in unicode_org_lbp[] the line breaking property from the
   LineBreak.txt file.  */
static void
fill_org_lbp (const char *linebreak_filename)
{
  unsigned int i, j;
  FILE *stream;
  char field0[FIELDLEN];
  char field1[FIELDLEN];
  char field2[FIELDLEN];
  int lineno = 0;

  for (i = 0; i < 0x110000; i++)
    unicode_org_lbp[i] = LBP_XX;

  stream = fopen (linebreak_filename, "r");
  if (stream == NULL)
    {
      fprintf (stderr, "error during fopen of '%s'\n", linebreak_filename);
      exit (1);
    }

  for (;;)
    {
      int n;
      int c;
      int value;

      lineno++;
      c = getc (stream);
      if (c == EOF)
	break;
      if (c == '#')
	{
	  do c = getc (stream); while (c != EOF && c != '\n');
	  continue;
	}
      ungetc (c, stream);
      n = getfield (stream, field0, ';');
      n += getfield (stream, field1, ' ');
      n += getfield (stream, field2, '\n');
      if (n == 0)
	break;
      if (n != 3)
	{
	  fprintf (stderr, "short line in '%s':%d\n", linebreak_filename,
		   lineno);
	  exit (1);
	}
#define TRY(bit) else if (strcmp (field1, #bit + 4) == 0) value = bit;
      if (false) {}
      TRY(LBP_BK)
      TRY(LBP_CM)
      TRY(LBP_WJ)
      TRY(LBP_ZW)
      TRY(LBP_GL)
      TRY(LBP_SP)
      TRY(LBP_B2)
      TRY(LBP_BA)
      TRY(LBP_BB)
      TRY(LBP_HY)
      TRY(LBP_CB)
      TRY(LBP_CL)
      TRY(LBP_EX)
      TRY(LBP_IN)
      TRY(LBP_NS)
      TRY(LBP_OP)
      TRY(LBP_QU)
      TRY(LBP_IS)
      TRY(LBP_NU)
      TRY(LBP_PO)
      TRY(LBP_PR)
      TRY(LBP_SY)
      TRY(LBP_AI)
      TRY(LBP_AL)
      TRY(LBP_H2)
      TRY(LBP_H3)
      TRY(LBP_ID)
      TRY(LBP_JL)
      TRY(LBP_JV)
      TRY(LBP_JT)
      TRY(LBP_SA)
      TRY(LBP_XX)
#undef TRY
      else if (strcmp (field1, "LF") == 0) value = LBP_BK;
      else if (strcmp (field1, "CR") == 0) value = LBP_BK;
      else if (strcmp (field1, "NL") == 0) value = LBP_BK;
      else if (strcmp (field1, "SG") == 0) value = LBP_XX;
      else
	{
	  fprintf (stderr, "unknown property value \"%s\" in '%s':%d\n",
		   field1, linebreak_filename, lineno);
	  exit (1);
	}
      i = strtoul (field0, NULL, 16);
      if (strstr (field0, "..") != NULL)
	{
	  /* Deal with a range.  */
	  j = strtoul (strstr (field0, "..") + 2, NULL, 16);
	  for (; i <= j; i++)
	    unicode_org_lbp[i] = value;
	}
      else
	{
	  /* Single character line.  */
	  unicode_org_lbp[i] = value;
	}
    }
  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error reading from '%s'\n", linebreak_filename);
      exit (1);
    }
}

/* Output the line breaking properties in a human readable format.  */
static void
debug_output_org_lbp (FILE *stream)
{
  unsigned int i;

  for (i = 0; i < 0x110000; i++)
    {
      int attr = unicode_org_lbp[i];
      if (attr != LBP_XX)
	{
	  fprintf (stream, "0x%04X", i);
#define PRINT_BIT(attr,bit) \
  if (attr == bit) fprintf (stream, " " #bit);
	  PRINT_BIT(attr,LBP_BK);
	  PRINT_BIT(attr,LBP_CM);
	  PRINT_BIT(attr,LBP_WJ);
	  PRINT_BIT(attr,LBP_ZW);
	  PRINT_BIT(attr,LBP_GL);
	  PRINT_BIT(attr,LBP_SP);
	  PRINT_BIT(attr,LBP_B2);
	  PRINT_BIT(attr,LBP_BA);
	  PRINT_BIT(attr,LBP_BB);
	  PRINT_BIT(attr,LBP_HY);
	  PRINT_BIT(attr,LBP_CB);
	  PRINT_BIT(attr,LBP_CL);
	  PRINT_BIT(attr,LBP_EX);
	  PRINT_BIT(attr,LBP_IN);
	  PRINT_BIT(attr,LBP_NS);
	  PRINT_BIT(attr,LBP_OP);
	  PRINT_BIT(attr,LBP_QU);
	  PRINT_BIT(attr,LBP_IS);
	  PRINT_BIT(attr,LBP_NU);
	  PRINT_BIT(attr,LBP_PO);
	  PRINT_BIT(attr,LBP_PR);
	  PRINT_BIT(attr,LBP_SY);
	  PRINT_BIT(attr,LBP_AI);
	  PRINT_BIT(attr,LBP_AL);
	  PRINT_BIT(attr,LBP_H2);
	  PRINT_BIT(attr,LBP_H3);
	  PRINT_BIT(attr,LBP_ID);
	  PRINT_BIT(attr,LBP_JL);
	  PRINT_BIT(attr,LBP_JV);
	  PRINT_BIT(attr,LBP_JT);
	  PRINT_BIT(attr,LBP_SA);
	  PRINT_BIT(attr,LBP_XX);
#undef PRINT_BIT
	  fprintf (stream, "\n");
	}
    }
}

static void
debug_output_org_tables (const char *filename)
{
  FILE *stream;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  debug_output_org_lbp (stream);

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Construction of sparse 3-level tables.  */
#define TABLE lbp_table
#define ELEMENT unsigned char
#define DEFAULT LBP_XX
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

static void
output_lbp (FILE *stream1, FILE *stream2)
{
  unsigned int i;
  struct lbp_table t;
  unsigned int level1_offset, level2_offset, level3_offset;

  t.p = 7;
  t.q = 9;
  lbp_table_init (&t);

  for (i = 0; i < 0x110000; i++)
    {
      int attr = get_lbp (i);

      /* Now attr should contain exactly one bit.  */
      if (attr == 0 || ((attr & (attr - 1)) != 0))
	abort ();

      if (attr != 1 << LBP_XX)
	{
	  unsigned int log2_attr;
	  for (log2_attr = 0; attr > 1; attr >>= 1, log2_attr++);

	  lbp_table_add (&t, i, log2_attr);
	}
    }

  lbp_table_finalize (&t);

  level1_offset =
    5 * sizeof (uint32_t);
  level2_offset =
    5 * sizeof (uint32_t)
    + t.level1_size * sizeof (uint32_t);
  level3_offset =
    5 * sizeof (uint32_t)
    + t.level1_size * sizeof (uint32_t)
    + (t.level2_size << t.q) * sizeof (uint32_t);

  for (i = 0; i < 5; i++)
    fprintf (stream1, "#define lbrkprop_header_%d %d\n", i,
	     ((uint32_t *) t.result)[i]);
  fprintf (stream1, "\n");
  fprintf (stream1, "typedef struct\n");
  fprintf (stream1, "  {\n");
  fprintf (stream1, "    int level1[%d];\n", t.level1_size);
  fprintf (stream1, "    int level2[%d << %d];\n", t.level2_size, t.q);
  fprintf (stream1, "    unsigned char level3[%d << %d];\n", t.level3_size, t.p);
  fprintf (stream1, "  }\n");
  fprintf (stream1, "lbrkprop_t;\n");
  fprintf (stream1, "extern const lbrkprop_t unilbrkprop;\n");

  fprintf (stream2, "const lbrkprop_t unilbrkprop =\n");
  fprintf (stream2, "{\n");
  fprintf (stream2, "  {");
  if (t.level1_size > 8)
    fprintf (stream2, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
	fprintf (stream2, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      fprintf (stream2, " %5d%s",
	       offset == 0 ? -1 : (offset - level2_offset) / sizeof (uint32_t),
	       (i+1 < t.level1_size ? "," : ""));
    }
  if (t.level1_size > 8)
    fprintf (stream2, "\n ");
  fprintf (stream2, " },\n");
  fprintf (stream2, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream2, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
	fprintf (stream2, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      fprintf (stream2, " %5d%s",
	       offset == 0 ? -1 : (offset - level3_offset) / sizeof (uint8_t),
	       (i+1 < t.level2_size << t.q ? "," : ""));
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream2, "\n ");
  fprintf (stream2, " },\n");
  fprintf (stream2, "  {");
  if (t.level3_size << t.p > 8)
    fprintf (stream2, "\n   ");
  for (i = 0; i < t.level3_size << t.p; i++)
    {
      unsigned char value = ((unsigned char *) (t.result + level3_offset))[i];
      const char *value_string;
      switch (value)
	{
#define CASE(x) case x: value_string = #x; break;
	  CASE(LBP_BK);
	  CASE(LBP_CM);
	  CASE(LBP_WJ);
	  CASE(LBP_ZW);
	  CASE(LBP_GL);
	  CASE(LBP_SP);
	  CASE(LBP_B2);
	  CASE(LBP_BA);
	  CASE(LBP_BB);
	  CASE(LBP_HY);
	  CASE(LBP_CB);
	  CASE(LBP_CL);
	  CASE(LBP_EX);
	  CASE(LBP_IN);
	  CASE(LBP_NS);
	  CASE(LBP_OP);
	  CASE(LBP_QU);
	  CASE(LBP_IS);
	  CASE(LBP_NU);
	  CASE(LBP_PO);
	  CASE(LBP_PR);
	  CASE(LBP_SY);
	  CASE(LBP_AI);
	  CASE(LBP_AL);
	  CASE(LBP_H2);
	  CASE(LBP_H3);
	  CASE(LBP_ID);
	  CASE(LBP_JL);
	  CASE(LBP_JV);
	  CASE(LBP_JT);
	  CASE(LBP_SA);
	  CASE(LBP_XX);
#undef CASE
	  default:
	    abort ();
	}
      if (i > 0 && (i % 8) == 0)
	fprintf (stream2, "\n   ");
      fprintf (stream2, " %s%s", value_string,
	       (i+1 < t.level3_size << t.p ? "," : ""));
    }
  if (t.level3_size << t.p > 8)
    fprintf (stream2, "\n ");
  fprintf (stream2, " }\n");
  fprintf (stream2, "};\n");
}

static void
output_tables (const char *filename1, const char *filename2, const char *version)
{
  const char *filenames[2];
  FILE *streams[2];
  size_t i;

  filenames[0] = filename1;
  filenames[1] = filename2;

  for (i = 0; i < 2; i++)
    {
      streams[i] = fopen (filenames[i], "w");
      if (streams[i] == NULL)
	{
	  fprintf (stderr, "cannot open '%s' for writing\n", filenames[i]);
	  exit (1);
	}
    }

  for (i = 0; i < 2; i++)
    {
      FILE *stream = streams[i];

      fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
      fprintf (stream, "/* Line breaking properties of Unicode characters.  */\n");
      fprintf (stream, "/* Generated automatically by gen-lbrk for Unicode %s.  */\n",
	       version);
      fprintf (stream, "\n");

      /* Put a GPL header on it.  The gnulib module is under LGPL (although it
	 still carries the GPL header), and it's gnulib-tool which replaces the
	 GPL header with an LGPL header.  */
      fprintf (stream, "/* Copyright (C) 2000-2002, 2004, 2008 Free Software Foundation, Inc.\n");
      fprintf (stream, "\n");
      fprintf (stream, "   This program is free software: you can redistribute it and/or modify\n");
      fprintf (stream, "   it under the terms of the GNU General Public License as published by\n");
      fprintf (stream, "   the Free Software Foundation; either version 3 of the License, or\n");
      fprintf (stream, "   (at your option) any later version.\n");
      fprintf (stream, "\n");
      fprintf (stream, "   This program is distributed in the hope that it will be useful,\n");
      fprintf (stream, "   but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
      fprintf (stream, "   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
      fprintf (stream, "   GNU General Public License for more details.\n");
      fprintf (stream, "\n");
      fprintf (stream, "   You should have received a copy of the GNU General Public License\n");
      fprintf (stream, "   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */\n");
      fprintf (stream, "\n");
    }

  output_lbp (streams[0], streams[1]);

  for (i = 0; i < 2; i++)
    {
      if (ferror (streams[i]) || fclose (streams[i]))
	{
	  fprintf (stderr, "error writing to '%s'\n", filenames[i]);
	  exit (1);
	}
    }
}

int
main (int argc, char * argv[])
{
  if (argc != 5)
    {
      fprintf (stderr, "Usage: %s UnicodeData.txt EastAsianWidth.txt LineBreak.txt version\n",
	       argv[0]);
      exit (1);
    }

  fill_attributes (argv[1]);
  fill_width (argv[2]);
  fill_org_lbp (argv[3]);

  debug_output_tables ("lbrkprop.txt");
  debug_output_org_tables ("lbrkprop_org.txt");

  output_tables ("lbrkprop1.h", "lbrkprop2.h", argv[4]);

  return 0;
}

/*
 * For Emacs M-x compile
 * Local Variables:
 * compile-command: "
   gcc -O -Wall -I../unictype gen-lbrk.c -o gen-lbrk && \
   ./gen-lbrk \
        /gfs/petix/Volumes/ExtData/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/5.0.0/ucd/UnicodeData.txt \
        /gfs/petix/Volumes/ExtData/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/5.0.0/ucd/EastAsianWidth.txt \
        /gfs/petix/Volumes/ExtData/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/5.0.0/ucd/LineBreak.txt \
        5.0.0
   "
 * End:
 */

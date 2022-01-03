/* Generate Unicode conforming character classification tables and
   line break properties tables and word break property tables and
   decomposition/composition and case mapping tables from a UnicodeData file.
   Copyright (C) 2000-2002, 2004, 2007-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2000-2002.

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

/* Usage example:
     $ gen-uni-tables /usr/local/share/www.unicode.org/Public/14.0.0/ucd/UnicodeData.txt \
                      /usr/local/share/www.unicode.org/Public/14.0.0/ucd/PropList.txt \
                      /usr/local/share/www.unicode.org/Public/14.0.0/ucd/DerivedCoreProperties.txt \
                      /usr/local/share/www.unicode.org/Public/14.0.0/ucd/emoji/emoji-data.txt \
                      /usr/local/share/www.unicode.org/Public/14.0.0/ucd/ArabicShaping.txt \
                      /usr/local/share/www.unicode.org/Public/14.0.0/ucd/Scripts.txt \
                      /usr/local/share/www.unicode.org/Public/14.0.0/ucd/Blocks.txt \
                      /usr/local/share/www.unicode.org/Public/3.0-Update1/PropList-3.0.1.txt \
                      /usr/local/share/www.unicode.org/Public/14.0.0/ucd/EastAsianWidth.txt \
                      /usr/local/share/www.unicode.org/Public/14.0.0/ucd/LineBreak.txt \
                      /usr/local/share/www.unicode.org/Public/14.0.0/ucd/auxiliary/WordBreakProperty.txt \
                      /usr/local/share/www.unicode.org/Public/14.0.0/ucd/auxiliary/GraphemeBreakProperty.txt \
                      /usr/local/share/www.unicode.org/Public/14.0.0/ucd/CompositionExclusions.txt \
                      /usr/local/share/www.unicode.org/Public/14.0.0/ucd/SpecialCasing.txt \
                      /usr/local/share/www.unicode.org/Public/14.0.0/ucd/CaseFolding.txt \
                      14.0.0
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZEOF(a) (sizeof(a) / sizeof(a[0]))

/* ========================================================================= */

/* Reading UnicodeData.txt.  */
/* See UCD.html.  */

/* This structure represents one line in the UnicodeData.txt file.  */
struct unicode_attribute
{
  const char *name;           /* Character name */
  const char *category;       /* General category */
  const char *combining;      /* Canonical combining class */
  const char *bidi;           /* Bidirectional category */
  const char *decomposition;  /* Character decomposition mapping */
  const char *decdigit;       /* Decimal digit value */
  const char *digit;          /* Digit value */
  const char *numeric;        /* Numeric value */
  bool mirrored;              /* mirrored */
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
  if (strcmp (field2, "Cs") == 0)
    /* Surrogates are UTF-16 artifacts, not real characters. Ignore them.  */
    return;
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
#define FIELDLEN 160

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
          fprintf (stderr, "field longer than expected, increase FIELDLEN\n");
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
          fprintf (stderr, "short line in '%s':%d\n",
                   unicodedata_filename, lineno);
          exit (1);
        }
      i = strtoul (field0, NULL, 16);
      if (field1[0] == '<'
          && strlen (field1) >= 9
          && strcmp (field1 + strlen (field1) - 8, ", First>") == 0)
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
                && strcmp (field1 + strlen (field1) - 7, ", Last>") == 0))
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

/* ========================================================================= */

/* Output the license notice for a library file.
   This closes an open C syntax comment.  */
static void
output_library_license (FILE *stream, bool lgplv2plus)
{
  if (lgplv2plus)
    {
      /* These Gnulib modules are under the LGPLv2+ license.  */
      fprintf (stream, "   This file is free software: you can redistribute it and/or modify\n");
      fprintf (stream, "   it under the terms of the GNU Lesser General Public License as\n");
      fprintf (stream, "   published by the Free Software Foundation; either version 2.1 of the\n");
      fprintf (stream, "   License, or (at your option) any later version.\n");
      fprintf (stream, "\n");
      fprintf (stream, "   This file is distributed in the hope that it will be useful,\n");
      fprintf (stream, "   but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
      fprintf (stream, "   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
      fprintf (stream, "   GNU Lesser General Public License for more details.\n");
      fprintf (stream, "\n");
      fprintf (stream, "   You should have received a copy of the GNU Lesser General Public License\n");
      fprintf (stream, "   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */\n");
    }
  else
    {
      /* These Gnulib modules are under the 'LGPLv3+ or GPLv2+' license.  */
      fprintf (stream, "   This file is free software.\n");
      fprintf (stream, "   It is dual-licensed under \"the GNU LGPLv3+ or the GNU GPLv2+\".\n");
      fprintf (stream, "   You can redistribute it and/or modify it under either\n");
      fprintf (stream, "     - the terms of the GNU Lesser General Public License as published\n");
      fprintf (stream, "       by the Free Software Foundation, either version 3, or (at your\n");
      fprintf (stream, "       option) any later version, or\n");
      fprintf (stream, "     - the terms of the GNU General Public License as published by the\n");
      fprintf (stream, "       Free Software Foundation; either version 2, or (at your option)\n");
      fprintf (stream, "       any later version, or\n");
      fprintf (stream, "     - the same dual license \"the GNU LGPLv3+ or the GNU GPLv2+\".\n");
      fprintf (stream, "\n");
      fprintf (stream, "   This file is distributed in the hope that it will be useful,\n");
      fprintf (stream, "   but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
      fprintf (stream, "   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n");
      fprintf (stream, "   Lesser General Public License and the GNU General Public License\n");
      fprintf (stream, "   for more details.\n");
      fprintf (stream, "\n");
      fprintf (stream, "   You should have received a copy of the GNU Lesser General Public\n");
      fprintf (stream, "   License and of the GNU General Public License along with this\n");
      fprintf (stream, "   program.  If not, see <https://www.gnu.org/licenses/>.  */\n");
    }
}

/* Output the license notice for a tests file.
   This closes an open C syntax comment.  */
static void
output_tests_license (FILE *stream)
{
  /* Gnulib tests modules are under the GPLv3+ license.  */
  fprintf (stream, "   This file is free software: you can redistribute it and/or modify\n");
  fprintf (stream, "   it under the terms of the GNU General Public License as published\n");
  fprintf (stream, "   by the Free Software Foundation, either version 3 of the License,\n");
  fprintf (stream, "   or (at your option) any later version.\n");
  fprintf (stream, "\n");
  fprintf (stream, "   This file is distributed in the hope that it will be useful,\n");
  fprintf (stream, "   but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
  fprintf (stream, "   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
  fprintf (stream, "   GNU General Public License for more details.\n");
  fprintf (stream, "\n");
  fprintf (stream, "   You should have received a copy of the GNU General Public License\n");
  fprintf (stream, "   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */\n");
}

/* ========================================================================= */

/* General category.  */
/* See Unicode 3.0 book, section 4.5,
       UCD.html.  */

static bool
is_category_L (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'L');
}

static bool
is_category_LC (unsigned int ch)
{
  /* See PropertyValueAliases.txt.  */
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'L'
          && (unicode_attributes[ch].category[1] == 'u'
              || unicode_attributes[ch].category[1] == 'l'
              || unicode_attributes[ch].category[1] == 't'));
}

static bool
is_category_Lu (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'L'
          && unicode_attributes[ch].category[1] == 'u');
}

static bool
is_category_Ll (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'L'
          && unicode_attributes[ch].category[1] == 'l');
}

static bool
is_category_Lt (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'L'
          && unicode_attributes[ch].category[1] == 't');
}

static bool
is_category_Lm (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'L'
          && unicode_attributes[ch].category[1] == 'm');
}

static bool
is_category_Lo (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'L'
          && unicode_attributes[ch].category[1] == 'o');
}

static bool
is_category_M (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'M');
}

static bool
is_category_Mn (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'M'
          && unicode_attributes[ch].category[1] == 'n');
}

static bool
is_category_Mc (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'M'
          && unicode_attributes[ch].category[1] == 'c');
}

static bool
is_category_Me (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'M'
          && unicode_attributes[ch].category[1] == 'e');
}

static bool
is_category_N (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'N');
}

static bool
is_category_Nd (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'N'
          && unicode_attributes[ch].category[1] == 'd');
}

static bool
is_category_Nl (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'N'
          && unicode_attributes[ch].category[1] == 'l');
}

static bool
is_category_No (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'N'
          && unicode_attributes[ch].category[1] == 'o');
}

static bool
is_category_P (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'P');
}

static bool
is_category_Pc (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'P'
          && unicode_attributes[ch].category[1] == 'c');
}

static bool
is_category_Pd (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'P'
          && unicode_attributes[ch].category[1] == 'd');
}

static bool
is_category_Ps (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'P'
          && unicode_attributes[ch].category[1] == 's');
}

static bool
is_category_Pe (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'P'
          && unicode_attributes[ch].category[1] == 'e');
}

static bool
is_category_Pi (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'P'
          && unicode_attributes[ch].category[1] == 'i');
}

static bool
is_category_Pf (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'P'
          && unicode_attributes[ch].category[1] == 'f');
}

static bool
is_category_Po (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'P'
          && unicode_attributes[ch].category[1] == 'o');
}

static bool
is_category_S (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'S');
}

static bool
is_category_Sm (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'S'
          && unicode_attributes[ch].category[1] == 'm');
}

static bool
is_category_Sc (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'S'
          && unicode_attributes[ch].category[1] == 'c');
}

static bool
is_category_Sk (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'S'
          && unicode_attributes[ch].category[1] == 'k');
}

static bool
is_category_So (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'S'
          && unicode_attributes[ch].category[1] == 'o');
}

static bool
is_category_Z (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'Z');
}

static bool
is_category_Zs (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'Z'
          && unicode_attributes[ch].category[1] == 's');
}

static bool
is_category_Zl (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'Z'
          && unicode_attributes[ch].category[1] == 'l');
}

static bool
is_category_Zp (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'Z'
          && unicode_attributes[ch].category[1] == 'p');
}

static bool
is_category_C (unsigned int ch)
{
  return (unicode_attributes[ch].name == NULL
          || unicode_attributes[ch].category[0] == 'C');
}

static bool
is_category_Cc (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'C'
          && unicode_attributes[ch].category[1] == 'c');
}

static bool
is_category_Cf (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'C'
          && unicode_attributes[ch].category[1] == 'f');
}

static bool
is_category_Cs (unsigned int ch)
{
  return (ch >= 0xd800 && ch < 0xe000);
}

static bool
is_category_Co (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'C'
          && unicode_attributes[ch].category[1] == 'o');
}

static bool
is_category_Cn (unsigned int ch)
{
  return (unicode_attributes[ch].name == NULL
          && !(ch >= 0xd800 && ch < 0xe000));
}

/* Output a boolean property in a human readable format.  */
static void
debug_output_predicate (const char *filename, bool (*predicate) (unsigned int))
{
  FILE *stream;
  unsigned int ch;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

#if 0 /* This yields huge text output.  */
  for (ch = 0; ch < 0x110000; ch++)
    if (predicate (ch))
      {
        fprintf (stream, "0x%04X\n", ch);
      }
#else
  for (ch = 0; ch < 0x110000; ch++)
    if (predicate (ch))
      {
        unsigned int first = ch;
        unsigned int last;

        while (ch + 1 < 0x110000 && predicate (ch + 1))
          ch++;
        last = ch;
        if (first < last)
          fprintf (stream, "0x%04X..0x%04X\n", first, last);
        else
          fprintf (stream, "0x%04X\n", ch);
      }
#endif

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Output the unit test for a boolean property.  */
static void
output_predicate_test (const char *filename, bool (*predicate) (unsigned int), const char *expression)
{
  FILE *stream;
  bool need_comma;
  unsigned int ch;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Test the Unicode character type functions.\n");
  fprintf (stream, "   Copyright (C) 2007-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_tests_license (stream);
  fprintf (stream, "\n");
  fprintf (stream, "#include \"test-predicate-part1.h\"\n");
  fprintf (stream, "\n");

  need_comma = false;
  for (ch = 0; ch < 0x110000; ch++)
    if (predicate (ch))
      {
        unsigned int first = ch;
        unsigned int last;

        while (ch + 1 < 0x110000 && predicate (ch + 1))
          ch++;
        last = ch;
        if (need_comma)
          fprintf (stream, ",\n");
        fprintf (stream, "    { 0x%04X, 0x%04X }", first, last);
        need_comma = true;
      }
  if (need_comma)
    fprintf (stream, "\n");

  fprintf (stream, "\n");
  fprintf (stream, "#define PREDICATE(c) %s\n", expression);
  fprintf (stream, "#include \"test-predicate-part2.h\"\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Construction of sparse 3-level tables.  */
#define TABLE predicate_table
#define xmalloc malloc
#define xrealloc realloc
#include "3levelbit.h"

/* Output a boolean property in a three-level bitmap.  */
static void
output_predicate (const char *filename, bool (*predicate) (unsigned int), const char *name, const char *comment, const char *version)
{
  FILE *stream;
  unsigned int ch, i;
  struct predicate_table t;
  unsigned int level1_offset, level2_offset, level3_offset;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* %s of Unicode characters.  */\n", comment);
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, strcmp (filename, "unictype/categ_M.h") == 0);
  fprintf (stream, "\n");

  t.p = 4; /* or: 5 */
  t.q = 7; /* or: 6 */
  predicate_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    if (predicate (ch))
      predicate_table_add (&t, ch);

  predicate_table_finalize (&t);

  /* Offsets in t.result, in memory of this process.  */
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
    if (i != 1)
      fprintf (stream, "#define header_%d %d\n", i,
               ((uint32_t *) t.result)[i]);

  fprintf (stream, "static const\n");
  fprintf (stream, "struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int header[1];\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    short level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned int level3[%zu << %d];\n", t.level3_size, t.p);
  fprintf (stream, "  }\n");
  fprintf (stream, "%s =\n", name);
  fprintf (stream, "{\n");
  fprintf (stream, "  { %d },\n", ((uint32_t *) t.result)[1]);
  fprintf (stream, "  {");
  if (t.level1_size > 1)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 1) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu * sizeof (int) / sizeof (short) + %5zu",
                 1 + t.level1_size, (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 1)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 1)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 1) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu + %5zu * sizeof (short) / sizeof (int) + %5zu",
                 1 + t.level1_size, t.level2_size << t.q, (offset - level3_offset) / sizeof (uint32_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 1)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level3_size << t.p > 4)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level3_size << t.p; i++)
    {
      if (i > 0 && (i % 4) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " 0x%08XU",
               ((uint32_t *) (t.result + level3_offset))[i]);
      if (i+1 < t.level3_size << t.p)
        fprintf (stream, ",");
    }
  if (t.level3_size << t.p > 4)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Output all categories.  */
static void
output_categories (const char *version)
{
#define CATEGORY(C) \
  debug_output_predicate ("unictype/categ_" #C ".txt", is_category_ ## C); \
  output_predicate_test ("../tests/unictype/test-categ_" #C ".c", is_category_ ## C, "uc_is_general_category (c, UC_CATEGORY_" #C ")"); \
  output_predicate ("unictype/categ_" #C ".h", is_category_ ## C, "u_categ_" #C, "Categories", version);
  CATEGORY (L)
  CATEGORY (LC)
  CATEGORY (Lu)
  CATEGORY (Ll)
  CATEGORY (Lt)
  CATEGORY (Lm)
  CATEGORY (Lo)
  CATEGORY (M)
  CATEGORY (Mn)
  CATEGORY (Mc)
  CATEGORY (Me)
  CATEGORY (N)
  CATEGORY (Nd)
  CATEGORY (Nl)
  CATEGORY (No)
  CATEGORY (P)
  CATEGORY (Pc)
  CATEGORY (Pd)
  CATEGORY (Ps)
  CATEGORY (Pe)
  CATEGORY (Pi)
  CATEGORY (Pf)
  CATEGORY (Po)
  CATEGORY (S)
  CATEGORY (Sm)
  CATEGORY (Sc)
  CATEGORY (Sk)
  CATEGORY (So)
  CATEGORY (Z)
  CATEGORY (Zs)
  CATEGORY (Zl)
  CATEGORY (Zp)
  CATEGORY (C)
  CATEGORY (Cc)
  CATEGORY (Cf)
  CATEGORY (Cs)
  CATEGORY (Co)
  CATEGORY (Cn)
#undef CATEGORY
}

enum
{
  UC_CATEGORY_MASK_L  = 0x0000001f,
  UC_CATEGORY_MASK_LC = 0x00000007,
  UC_CATEGORY_MASK_Lu = 0x00000001,
  UC_CATEGORY_MASK_Ll = 0x00000002,
  UC_CATEGORY_MASK_Lt = 0x00000004,
  UC_CATEGORY_MASK_Lm = 0x00000008,
  UC_CATEGORY_MASK_Lo = 0x00000010,
  UC_CATEGORY_MASK_M  = 0x000000e0,
  UC_CATEGORY_MASK_Mn = 0x00000020,
  UC_CATEGORY_MASK_Mc = 0x00000040,
  UC_CATEGORY_MASK_Me = 0x00000080,
  UC_CATEGORY_MASK_N  = 0x00000700,
  UC_CATEGORY_MASK_Nd = 0x00000100,
  UC_CATEGORY_MASK_Nl = 0x00000200,
  UC_CATEGORY_MASK_No = 0x00000400,
  UC_CATEGORY_MASK_P  = 0x0003f800,
  UC_CATEGORY_MASK_Pc = 0x00000800,
  UC_CATEGORY_MASK_Pd = 0x00001000,
  UC_CATEGORY_MASK_Ps = 0x00002000,
  UC_CATEGORY_MASK_Pe = 0x00004000,
  UC_CATEGORY_MASK_Pi = 0x00008000,
  UC_CATEGORY_MASK_Pf = 0x00010000,
  UC_CATEGORY_MASK_Po = 0x00020000,
  UC_CATEGORY_MASK_S  = 0x003c0000,
  UC_CATEGORY_MASK_Sm = 0x00040000,
  UC_CATEGORY_MASK_Sc = 0x00080000,
  UC_CATEGORY_MASK_Sk = 0x00100000,
  UC_CATEGORY_MASK_So = 0x00200000,
  UC_CATEGORY_MASK_Z  = 0x01c00000,
  UC_CATEGORY_MASK_Zs = 0x00400000,
  UC_CATEGORY_MASK_Zl = 0x00800000,
  UC_CATEGORY_MASK_Zp = 0x01000000,
  UC_CATEGORY_MASK_C  = 0x3e000000,
  UC_CATEGORY_MASK_Cc = 0x02000000,
  UC_CATEGORY_MASK_Cf = 0x04000000,
  UC_CATEGORY_MASK_Cs = 0x08000000,
  UC_CATEGORY_MASK_Co = 0x10000000,
  UC_CATEGORY_MASK_Cn = 0x20000000
};

static int
general_category_byname (const char *category_name)
{
  if (category_name[0] != '\0'
      && (category_name[1] == '\0' || category_name[2] == '\0'))
    switch (category_name[0])
      {
      case 'L':
        switch (category_name[1])
          {
          case '\0': return UC_CATEGORY_MASK_L;
          case 'C': return UC_CATEGORY_MASK_LC;
          case 'u': return UC_CATEGORY_MASK_Lu;
          case 'l': return UC_CATEGORY_MASK_Ll;
          case 't': return UC_CATEGORY_MASK_Lt;
          case 'm': return UC_CATEGORY_MASK_Lm;
          case 'o': return UC_CATEGORY_MASK_Lo;
          }
        break;
      case 'M':
        switch (category_name[1])
          {
          case '\0': return UC_CATEGORY_MASK_M;
          case 'n': return UC_CATEGORY_MASK_Mn;
          case 'c': return UC_CATEGORY_MASK_Mc;
          case 'e': return UC_CATEGORY_MASK_Me;
          }
        break;
      case 'N':
        switch (category_name[1])
          {
          case '\0': return UC_CATEGORY_MASK_N;
          case 'd': return UC_CATEGORY_MASK_Nd;
          case 'l': return UC_CATEGORY_MASK_Nl;
          case 'o': return UC_CATEGORY_MASK_No;
          }
        break;
      case 'P':
        switch (category_name[1])
          {
          case '\0': return UC_CATEGORY_MASK_P;
          case 'c': return UC_CATEGORY_MASK_Pc;
          case 'd': return UC_CATEGORY_MASK_Pd;
          case 's': return UC_CATEGORY_MASK_Ps;
          case 'e': return UC_CATEGORY_MASK_Pe;
          case 'i': return UC_CATEGORY_MASK_Pi;
          case 'f': return UC_CATEGORY_MASK_Pf;
          case 'o': return UC_CATEGORY_MASK_Po;
          }
        break;
      case 'S':
        switch (category_name[1])
          {
          case '\0': return UC_CATEGORY_MASK_S;
          case 'm': return UC_CATEGORY_MASK_Sm;
          case 'c': return UC_CATEGORY_MASK_Sc;
          case 'k': return UC_CATEGORY_MASK_Sk;
          case 'o': return UC_CATEGORY_MASK_So;
          }
        break;
      case 'Z':
        switch (category_name[1])
          {
          case '\0': return UC_CATEGORY_MASK_Z;
          case 's': return UC_CATEGORY_MASK_Zs;
          case 'l': return UC_CATEGORY_MASK_Zl;
          case 'p': return UC_CATEGORY_MASK_Zp;
          }
        break;
      case 'C':
        switch (category_name[1])
          {
          case '\0': return UC_CATEGORY_MASK_C;
          case 'c': return UC_CATEGORY_MASK_Cc;
          case 'f': return UC_CATEGORY_MASK_Cf;
          case 's': return UC_CATEGORY_MASK_Cs;
          case 'o': return UC_CATEGORY_MASK_Co;
          case 'n': return UC_CATEGORY_MASK_Cn;
          }
        break;
      }
  /* Invalid category name.  */
  abort ();
}

/* Construction of sparse 3-level tables.  */
#define TABLE category_table
#define ELEMENT uint8_t
#define DEFAULT 29 /* = log2(UC_CATEGORY_MASK_Cn) */
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

/* Output the per-character category table.  */
static void
output_category (const char *filename, const char *version)
{
  FILE *stream;
  unsigned int ch, i;
  struct category_table t;
  unsigned int level1_offset, level2_offset, level3_offset;
  uint16_t *level3_packed;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Categories of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, true);
  fprintf (stream, "\n");

  t.p = 7;
  t.q = 9;
  category_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      int value;
      unsigned int log2_value;

      if (is_category_Cs (ch))
        value = UC_CATEGORY_MASK_Cs;
      else if (unicode_attributes[ch].name != NULL)
        value = general_category_byname (unicode_attributes[ch].category);
      else
        continue;

      /* Now value should contain exactly one bit.  */
      assert (value != 0 && (value & (value - 1)) == 0);

      for (log2_value = 0; value > 1; value >>= 1, log2_value++);

      assert (log2_value <= 0x1f);

      category_table_add (&t, ch, log2_value);
    }

  category_table_finalize (&t);

  /* Offsets in t.result, in memory of this process.  */
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
    fprintf (stream, "#define category_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream, "static const\n");
  fprintf (stream, "struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    short level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned short level3[%zu * %d + 1];\n", t.level3_size,
           (1 << t.p) * 5 / 16);
  fprintf (stream, "  }\n");
  fprintf (stream, "u_category =\n");
  fprintf (stream, "{\n");
  fprintf (stream, "  {");
  if (t.level1_size > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level3_offset) / sizeof (uint8_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  /* Pack the level3 array.  Each entry needs 5 bits only.  Use 16-bit units,
     not 32-bit units, in order to make the lookup function easier.  */
  level3_packed =
    (uint16_t *)
    calloc ((t.level3_size << t.p) * 5 / 16 + 1, sizeof (uint16_t));
  for (i = 0; i < t.level3_size << t.p; i++)
    {
      unsigned int j = (i * 5) / 16;
      unsigned int k = (i * 5) % 16;
      uint32_t value = ((unsigned char *) (t.result + level3_offset))[i];
      value = level3_packed[j] | (level3_packed[j+1] << 16) | (value << k);
      level3_packed[j] = value & 0xffff;
      level3_packed[j+1] = value >> 16;
    }
  fprintf (stream, "  {");
  if ((t.level3_size << t.p) * 5 / 16 + 1 > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < (t.level3_size << t.p) * 5 / 16 + 1; i++)
    {
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " 0x%04x", level3_packed[i]);
      if (i+1 < (t.level3_size << t.p) * 5 / 16 + 1)
        fprintf (stream, ",");
    }
  if ((t.level3_size << t.p) * 5 / 16 + 1 > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  free (level3_packed);
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Canonical combining class.  */
/* See Unicode 3.0 book, section 4.2,
       UCD.html.  */

/* Construction of sparse 3-level tables.  */
#define TABLE combclass_table
#define ELEMENT uint8_t
#define DEFAULT 0
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

/* Output the per-character combining class table.  */
static void
output_combclass (const char *filename, const char *version)
{
  FILE *stream;
  unsigned int ch, i;
  struct combclass_table t;
  unsigned int level1_offset, level2_offset, level3_offset;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Combining class of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, true);
  fprintf (stream, "\n");

  t.p = 7;
  t.q = 9;
  combclass_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    if (unicode_attributes[ch].name != NULL)
      {
        int value = atoi (unicode_attributes[ch].combining);
        assert (value >= 0 && value <= 255);
        combclass_table_add (&t, ch, value);
      }

  combclass_table_finalize (&t);

  /* Offsets in t.result, in memory of this process.  */
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
    fprintf (stream, "#define combclass_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream, "static const\n");
  fprintf (stream, "struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    short level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned char level3[%zu << %d];\n", t.level3_size, t.p);
  fprintf (stream, "  }\n");
  fprintf (stream, "u_combclass =\n");
  fprintf (stream, "{\n");
  fprintf (stream, "  {");
  if (t.level1_size > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level3_offset) / sizeof (uint8_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level3_size << t.p > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level3_size << t.p; i++)
    {
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " %3d", ((uint8_t *) (t.result + level3_offset))[i]);
      if (i+1 < t.level3_size << t.p)
        fprintf (stream, ",");
    }
  if (t.level3_size << t.p > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Bidirectional category.  */
/* See Unicode 3.0 book, section 4.3,
       UCD.html.  */

enum
{
  UC_BIDI_L,   /* Left-to-Right */
  UC_BIDI_LRE, /* Left-to-Right Embedding */
  UC_BIDI_LRO, /* Left-to-Right Override */
  UC_BIDI_R,   /* Right-to-Left */
  UC_BIDI_AL,  /* Right-to-Left Arabic */
  UC_BIDI_RLE, /* Right-to-Left Embedding */
  UC_BIDI_RLO, /* Right-to-Left Override */
  UC_BIDI_PDF, /* Pop Directional Format */
  UC_BIDI_EN,  /* European Number */
  UC_BIDI_ES,  /* European Number Separator */
  UC_BIDI_ET,  /* European Number Terminator */
  UC_BIDI_AN,  /* Arabic Number */
  UC_BIDI_CS,  /* Common Number Separator */
  UC_BIDI_NSM, /* Non-Spacing Mark */
  UC_BIDI_BN,  /* Boundary Neutral */
  UC_BIDI_B,   /* Paragraph Separator */
  UC_BIDI_S,   /* Segment Separator */
  UC_BIDI_WS,  /* Whitespace */
  UC_BIDI_ON,  /* Other Neutral */
  UC_BIDI_LRI, /* Left-to-Right Isolate */
  UC_BIDI_RLI, /* Right-to-Left Isolate */
  UC_BIDI_FSI, /* First Strong Isolate */
  UC_BIDI_PDI  /* Pop Directional Isolate */
};

static int
bidi_category_byname (const char *category_name)
{
  switch (category_name[0])
    {
    case 'A':
      switch (category_name[1])
        {
        case 'L':
          if (category_name[2] == '\0')
            return UC_BIDI_AL;
          break;
        case 'N':
          if (category_name[2] == '\0')
            return UC_BIDI_AN;
          break;
        }
      break;
    case 'B':
      switch (category_name[1])
        {
        case '\0':
          return UC_BIDI_B;
        case 'N':
          if (category_name[2] == '\0')
            return UC_BIDI_BN;
          break;
        }
      break;
    case 'C':
      switch (category_name[1])
        {
        case 'S':
          if (category_name[2] == '\0')
            return UC_BIDI_CS;
          break;
        }
      break;
    case 'E':
      switch (category_name[1])
        {
        case 'N':
          if (category_name[2] == '\0')
            return UC_BIDI_EN;
          break;
        case 'S':
          if (category_name[2] == '\0')
            return UC_BIDI_ES;
          break;
        case 'T':
          if (category_name[2] == '\0')
            return UC_BIDI_ET;
          break;
        }
      break;
    case 'F':
      switch (category_name[1])
        {
        case 'S':
          switch (category_name[2])
            {
            case 'I':
              if (category_name[3] == '\0')
                return UC_BIDI_FSI;
              break;
            }
        }
      break;
   case 'L':
      switch (category_name[1])
        {
        case '\0':
          return UC_BIDI_L;
        case 'R':
          switch (category_name[2])
            {
            case 'E':
              if (category_name[3] == '\0')
                return UC_BIDI_LRE;
              break;
            case 'O':
              if (category_name[3] == '\0')
                return UC_BIDI_LRO;
              break;
            case 'I':
              if (category_name[3] == '\0')
                return UC_BIDI_LRI;
              break;
           }
          break;
        }
      break;
    case 'N':
      switch (category_name[1])
        {
        case 'S':
          switch (category_name[2])
            {
            case 'M':
              if (category_name[3] == '\0')
                return UC_BIDI_NSM;
              break;
            }
          break;
        }
      break;
    case 'O':
      switch (category_name[1])
        {
        case 'N':
          if (category_name[2] == '\0')
            return UC_BIDI_ON;
          break;
        }
      break;
    case 'P':
      switch (category_name[1])
        {
        case 'D':
          switch (category_name[2])
            {
            case 'F':
              if (category_name[3] == '\0')
                return UC_BIDI_PDF;
              break;
            case 'I':
              if (category_name[3] == '\0')
                return UC_BIDI_PDI;
              break;
            }
          break;
        }
      break;
    case 'R':
      switch (category_name[1])
        {
        case '\0':
          return UC_BIDI_R;
        case 'L':
          switch (category_name[2])
            {
            case 'E':
              if (category_name[3] == '\0')
                return UC_BIDI_RLE;
              break;
            case 'O':
              if (category_name[3] == '\0')
                return UC_BIDI_RLO;
              break;
            case 'I':
              if (category_name[3] == '\0')
                return UC_BIDI_RLI;
              break;
           }
          break;
        }
      break;
    case 'S':
      if (category_name[1] == '\0')
        return UC_BIDI_S;
      break;
    case 'W':
      switch (category_name[1])
        {
        case 'S':
          if (category_name[2] == '\0')
            return UC_BIDI_WS;
          break;
        }
      break;
    }
  /* Invalid bidi category name.  */
  abort ();
}

static int
get_bidi_category (unsigned int ch)
{
  if (unicode_attributes[ch].name != NULL)
    return bidi_category_byname (unicode_attributes[ch].bidi);
  else
    {
      /* The bidi category of unassigned characters depends on the range.
         See UTR #9 and DerivedBidiClass.txt.  */
      if ((ch >= 0x0590 && ch <= 0x05FF)
          || (ch >= 0x07FB && ch <= 0x08FF)
          || (ch >= 0xFB37 && ch <= 0xFB45)
          || (ch >= 0x10800 && ch <= 0x10FFF))
        return UC_BIDI_R;
      else if ((ch >= 0x0600 && ch <= 0x07BF)
               || (ch >= 0x2064 && ch <= 0x2069)
               || (ch >= 0xFBB2 && ch <= 0xFDCF)
               || (ch >= 0xFDFE && ch <= 0xFEFE))
        return UC_BIDI_AL;
      else if ((ch >= 0xFDD0 && ch <= 0xFDEF)
               || (ch >= 0xFFF0 && ch <= 0xFFFF)
               || (ch & 0xFFFF) == 0xFFFE
               || (ch & 0xFFFF) == 0xFFFF
               || (ch >= 0xE0000 && ch <= 0xE0FFF))
        return UC_BIDI_BN;
      else
        return UC_BIDI_L;
    }
}

/* Construction of sparse 3-level tables.  */
#define TABLE bidi_category_table
#define ELEMENT uint8_t
#define DEFAULT UC_BIDI_L
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

/* Output the per-character bidi category table.  */
static void
output_bidi_category (const char *filename, const char *version)
{
  FILE *stream;
  unsigned int ch, i;
  struct bidi_category_table t;
  unsigned int level1_offset, level2_offset, level3_offset;
  uint16_t *level3_packed;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Bidi categories of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, true);
  fprintf (stream, "\n");

  t.p = 7;
  t.q = 9;
  bidi_category_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      int value = get_bidi_category (ch);

      assert (value <= 0x1f);

      bidi_category_table_add (&t, ch, value);
    }

  bidi_category_table_finalize (&t);

  /* Offsets in t.result, in memory of this process.  */
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
    fprintf (stream, "#define bidi_category_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream, "static const\n");
  fprintf (stream, "struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    short level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned short level3[%zu * %d + 1];\n", t.level3_size,
           (1 << t.p) * 5 / 16);
  fprintf (stream, "  }\n");
  fprintf (stream, "u_bidi_category =\n");
  fprintf (stream, "{\n");
  fprintf (stream, "  {");
  if (t.level1_size > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level3_offset) / sizeof (uint8_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  /* Pack the level3 array.  Each entry needs 5 bits only.  Use 16-bit units,
     not 32-bit units, in order to make the lookup function easier.  */
  level3_packed =
    (uint16_t *)
    calloc ((t.level3_size << t.p) * 5 / 16 + 1, sizeof (uint16_t));
  for (i = 0; i < t.level3_size << t.p; i++)
    {
      unsigned int j = (i * 5) / 16;
      unsigned int k = (i * 5) % 16;
      uint32_t value = ((unsigned char *) (t.result + level3_offset))[i];
      value = level3_packed[j] | (level3_packed[j+1] << 16) | (value << k);
      level3_packed[j] = value & 0xffff;
      level3_packed[j+1] = value >> 16;
    }
  fprintf (stream, "  {");
  if ((t.level3_size << t.p) * 5 / 16 + 1 > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < (t.level3_size << t.p) * 5 / 16 + 1; i++)
    {
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " 0x%04x", level3_packed[i]);
      if (i+1 < (t.level3_size << t.p) * 5 / 16 + 1)
        fprintf (stream, ",");
    }
  if ((t.level3_size << t.p) * 5 / 16 + 1 > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  free (level3_packed);
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Decimal digit value.  */
/* See Unicode 3.0 book, section 4.6.  */

static int
get_decdigit_value (unsigned int ch)
{
  if (unicode_attributes[ch].name != NULL
      && unicode_attributes[ch].decdigit[0] != '\0')
    return atoi (unicode_attributes[ch].decdigit);
  return -1;
}

/* Construction of sparse 3-level tables.  */
#define TABLE decdigit_table
#define ELEMENT uint8_t
#define DEFAULT 0
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

/* Output the unit test for the per-character decimal digit value table.  */
static void
output_decimal_digit_test (const char *filename, const char *version)
{
  FILE *stream;
  bool need_comma;
  unsigned int ch;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Decimal digit values of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_tests_license (stream);
  fprintf (stream, "\n");

  need_comma = false;
  for (ch = 0; ch < 0x110000; ch++)
    {
      int value = get_decdigit_value (ch);

      assert (value >= -1 && value < 10);

      if (value >= 0)
        {
          if (need_comma)
            fprintf (stream, ",\n");
          fprintf (stream, "    { 0x%04X, %d }", ch, value);
          need_comma = true;
        }
    }
  if (need_comma)
    fprintf (stream, "\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Output the per-character decimal digit value table.  */
static void
output_decimal_digit (const char *filename, const char *version)
{
  FILE *stream;
  unsigned int ch, i;
  struct decdigit_table t;
  unsigned int level1_offset, level2_offset, level3_offset;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Decimal digit values of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, false);
  fprintf (stream, "\n");

  t.p = 7;
  t.q = 9;
  decdigit_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      int value = 1 + get_decdigit_value (ch);

      assert (value >= 0 && value <= 10);

      decdigit_table_add (&t, ch, value);
    }

  decdigit_table_finalize (&t);

  /* Offsets in t.result, in memory of this process.  */
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
    fprintf (stream, "#define decdigit_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream, "static const\n");
  fprintf (stream, "struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    short level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned char level3[%zu << %d];\n", t.level3_size,
           t.p - 1);
  fprintf (stream, "  }\n");
  fprintf (stream, "u_decdigit =\n");
  fprintf (stream, "{\n");
  fprintf (stream, "  {");
  if (t.level1_size > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level3_offset) / sizeof (uint8_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  /* Pack the level3 array.  Each entry needs 4 bits only.  */
  fprintf (stream, "  {");
  if (t.level3_size << (t.p - 1) > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level3_size << (t.p - 1); i++)
    {
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " 0x%02x",
               ((uint8_t *) (t.result + level3_offset))[2*i]
               + (((uint8_t *) (t.result + level3_offset))[2*i+1] << 4));
      if (i+1 < t.level3_size << (t.p - 1))
        fprintf (stream, ",");
    }
  if (t.level3_size << (t.p - 1) > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Digit value.  */
/* See Unicode 3.0 book, section 4.6.  */

static int
get_digit_value (unsigned int ch)
{
  if (unicode_attributes[ch].name != NULL
      && unicode_attributes[ch].digit[0] != '\0')
    return atoi (unicode_attributes[ch].digit);
  return -1;
}

/* Output the unit test for the per-character digit value table.  */
static void
output_digit_test (const char *filename, const char *version)
{
  FILE *stream;
  bool need_comma;
  unsigned int ch;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Digit values of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_tests_license (stream);
  fprintf (stream, "\n");

  need_comma = false;
  for (ch = 0; ch < 0x110000; ch++)
    {
      int value = get_digit_value (ch);

      assert (value >= -1 && value < 10);

      if (value >= 0)
        {
          if (need_comma)
            fprintf (stream, ",\n");
          fprintf (stream, "    { 0x%04X, %d }", ch, value);
          need_comma = true;
        }
    }
  if (need_comma)
    fprintf (stream, "\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Output the per-character digit value table.  */
static void
output_digit (const char *filename, const char *version)
{
  FILE *stream;
  unsigned int ch, i;
  struct decdigit_table t;
  unsigned int level1_offset, level2_offset, level3_offset;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Digit values of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, false);
  fprintf (stream, "\n");

  t.p = 7;
  t.q = 9;
  decdigit_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      int value = 1 + get_digit_value (ch);

      assert (value >= 0 && value <= 10);

      decdigit_table_add (&t, ch, value);
    }

  decdigit_table_finalize (&t);

  /* Offsets in t.result, in memory of this process.  */
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
    fprintf (stream, "#define digit_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream, "static const\n");
  fprintf (stream, "struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    short level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned char level3[%zu << %d];\n", t.level3_size,
           t.p - 1);
  fprintf (stream, "  }\n");
  fprintf (stream, "u_digit =\n");
  fprintf (stream, "{\n");
  fprintf (stream, "  {");
  if (t.level1_size > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level3_offset) / sizeof (uint8_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  /* Pack the level3 array.  Each entry needs 4 bits only.  */
  fprintf (stream, "  {");
  if (t.level3_size << (t.p - 1) > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level3_size << (t.p - 1); i++)
    {
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " 0x%02x",
               ((uint8_t *) (t.result + level3_offset))[2*i]
               + (((uint8_t *) (t.result + level3_offset))[2*i+1] << 4));
      if (i+1 < t.level3_size << (t.p - 1))
        fprintf (stream, ",");
    }
  if (t.level3_size << (t.p - 1) > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Numeric value.  */
/* See Unicode 3.0 book, section 4.6.  */

typedef struct { int numerator; int denominator; } uc_fraction_t;

static uc_fraction_t
get_numeric_value (unsigned int ch)
{
  uc_fraction_t value;

  if (unicode_attributes[ch].name != NULL
      && unicode_attributes[ch].numeric[0] != '\0')
    {
      const char *str = unicode_attributes[ch].numeric;
      /* str is of the form "integer" or "integer/posinteger".  */
      value.numerator = atoi (str);
      if (strchr (str, '/') != NULL)
        value.denominator = atoi (strchr (str, '/') + 1);
      else
        value.denominator = 1;
    }
  else
    {
      value.numerator = 0;
      value.denominator = 0;
    }
  return value;
}

/* Output the unit test for the per-character numeric value table.  */
static void
output_numeric_test (const char *filename, const char *version)
{
  FILE *stream;
  bool need_comma;
  unsigned int ch;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Numeric values of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_tests_license (stream);
  fprintf (stream, "\n");

  need_comma = false;
  for (ch = 0; ch < 0x110000; ch++)
    {
      uc_fraction_t value = get_numeric_value (ch);

      if (value.numerator != 0 || value.denominator != 0)
        {
          if (need_comma)
            fprintf (stream, ",\n");
          fprintf (stream, "    { 0x%04X, %d, %d }",
                   ch, value.numerator, value.denominator);
          need_comma = true;
        }
    }
  if (need_comma)
    fprintf (stream, "\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Construction of sparse 3-level tables.  */
#define TABLE numeric_table
#define ELEMENT uint8_t
#define DEFAULT 0
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

/* Output the per-character numeric value table.  */
static void
output_numeric (const char *filename, const char *version)
{
  FILE *stream;
  uc_fraction_t fractions[160];
  unsigned int nfractions;
  unsigned int ch, i, j;
  struct numeric_table t;
  unsigned int level1_offset, level2_offset, level3_offset;
  uint16_t *level3_packed;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Numeric values of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, false);
  fprintf (stream, "\n");

  /* Create table of occurring fractions.  */
  nfractions = 0;
  for (ch = 0; ch < 0x110000; ch++)
    {
      uc_fraction_t value = get_numeric_value (ch);

      for (i = 0; i < nfractions; i++)
        if (value.numerator == fractions[i].numerator
            && value.denominator == fractions[i].denominator)
          break;
      if (i == nfractions)
        {
          assert (nfractions != SIZEOF (fractions));
          for (i = 0; i < nfractions; i++)
            if (value.denominator < fractions[i].denominator
                || (value.denominator == fractions[i].denominator
                    && value.numerator < fractions[i].numerator))
              break;
          for (j = nfractions; j > i; j--)
            fractions[j] = fractions[j - 1];
          fractions[i] = value;
          nfractions++;
        }
    }

  fprintf (stream, "static const uc_fraction_t u_numeric_values[%d] =\n",
           nfractions);
  fprintf (stream, "{\n");
  for (i = 0; i < nfractions; i++)
    {
      fprintf (stream, "  { %d, %d }", fractions[i].numerator,
               fractions[i].denominator);
      if (i+1 < nfractions)
        fprintf (stream, ",");
      fprintf (stream, "\n");
    }
  fprintf (stream, "};\n");

  t.p = 7;
  t.q = 9;
  numeric_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      uc_fraction_t value = get_numeric_value (ch);

      for (i = 0; i < nfractions; i++)
        if (value.numerator == fractions[i].numerator
            && value.denominator == fractions[i].denominator)
          break;
      assert (i != nfractions);

      numeric_table_add (&t, ch, i);
    }

  numeric_table_finalize (&t);

  /* Offsets in t.result, in memory of this process.  */
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
    fprintf (stream, "#define numeric_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream, "static const\n");
  fprintf (stream, "struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    short level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned short level3[%zu * %d + 1];\n", t.level3_size,
           (1 << t.p) * 8 / 16);
  fprintf (stream, "  }\n");
  fprintf (stream, "u_numeric =\n");
  fprintf (stream, "{\n");
  fprintf (stream, "  {");
  if (t.level1_size > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level3_offset) / sizeof (uint8_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  /* Pack the level3 array.  Each entry needs 8 bits only.  Use 16-bit units,
     not 32-bit units, in order to make the lookup function easier.  */
  level3_packed =
    (uint16_t *)
    calloc ((t.level3_size << t.p) * 8 / 16 + 1, sizeof (uint16_t));
  for (i = 0; i < t.level3_size << t.p; i++)
    {
      unsigned int j = (i * 8) / 16;
      unsigned int k = (i * 8) % 16;
      uint32_t value = ((unsigned char *) (t.result + level3_offset))[i];
      value = level3_packed[j] | (level3_packed[j+1] << 16) | (value << k);
      level3_packed[j] = value & 0xffff;
      level3_packed[j+1] = value >> 16;
    }
  fprintf (stream, "  {");
  if ((t.level3_size << t.p) * 8 / 16 + 1 > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < (t.level3_size << t.p) * 8 / 16 + 1; i++)
    {
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " 0x%04x", level3_packed[i]);
      if (i+1 < (t.level3_size << t.p) * 8 / 16 + 1)
        fprintf (stream, ",");
    }
  if ((t.level3_size << t.p) * 8 / 16 + 1 > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  free (level3_packed);
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Mirrored.  */
/* See Unicode 3.0 book, section 4.7,
       UAX #9.  */

/* List of mirrored character pairs.  This is a subset of the characters
   having the BidiMirrored property.  */
static unsigned int mirror_pairs[][2] =
{
  { 0x0028, 0x0029 },
  { 0x003C, 0x003E },
  { 0x005B, 0x005D },
  { 0x007B, 0x007D },
  { 0x00AB, 0x00BB },
  { 0x2039, 0x203A },
  { 0x2045, 0x2046 },
  { 0x207D, 0x207E },
  { 0x208D, 0x208E },
  { 0x2208, 0x220B },
  { 0x220A, 0x220D },
  { 0x223C, 0x223D },
  { 0x2243, 0x22CD },
  { 0x2252, 0x2253 },
  { 0x2254, 0x2255 },
  { 0x2264, 0x2265 },
  { 0x2266, 0x2267 },
  { 0x226A, 0x226B },
  { 0x2276, 0x2277 },
  { 0x2278, 0x2279 },
  { 0x227A, 0x227B },
  { 0x227C, 0x227D },
  { 0x2282, 0x2283 },
  { 0x2286, 0x2287 },
  { 0x228F, 0x2290 },
  { 0x2291, 0x2292 },
  { 0x22A2, 0x22A3 },
  { 0x22B0, 0x22B1 },
  { 0x22B2, 0x22B3 },
  { 0x22B4, 0x22B5 },
  { 0x22B6, 0x22B7 },
  { 0x22C9, 0x22CA },
  { 0x22CB, 0x22CC },
  { 0x22D0, 0x22D1 },
  { 0x22D6, 0x22D7 },
  { 0x22D8, 0x22D9 },
  { 0x22DA, 0x22DB },
  { 0x22DC, 0x22DD },
  { 0x22DE, 0x22DF },
  { 0x22F0, 0x22F1 },
  { 0x2308, 0x2309 },
  { 0x230A, 0x230B },
  { 0x2329, 0x232A },
  { 0x3008, 0x3009 },
  { 0x300A, 0x300B },
  { 0x300C, 0x300D },
  { 0x300E, 0x300F },
  { 0x3010, 0x3011 },
  { 0x3014, 0x3015 },
  { 0x3016, 0x3017 },
  { 0x3018, 0x3019 },
  { 0x301A, 0x301B }
};

static int
get_mirror_value (unsigned int ch)
{
  bool mirrored;
  unsigned int mirror_char;
  unsigned int i;

  mirrored = (unicode_attributes[ch].name != NULL
              && unicode_attributes[ch].mirrored);
  mirror_char = 0xfffd;
  for (i = 0; i < sizeof (mirror_pairs) / sizeof (mirror_pairs[0]); i++)
    if (ch == mirror_pairs[i][0])
      {
        mirror_char = mirror_pairs[i][1];
        break;
      }
    else if (ch == mirror_pairs[i][1])
      {
        mirror_char = mirror_pairs[i][0];
        break;
      }
  if (mirrored)
    return (int) mirror_char - (int) ch;
  else
    {
      assert (mirror_char == 0xfffd);
      return 0;
    }
}

/* Construction of sparse 3-level tables.  */
#define TABLE mirror_table
#define ELEMENT int32_t
#define DEFAULT 0
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

/* Output the per-character mirror table.  */
static void
output_mirror (const char *filename, const char *version)
{
  FILE *stream;
  unsigned int ch, i;
  struct mirror_table t;
  unsigned int level1_offset, level2_offset, level3_offset;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Mirrored Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, false);
  fprintf (stream, "\n");

  t.p = 7;
  t.q = 9;
  mirror_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      int value = get_mirror_value (ch);

      mirror_table_add (&t, ch, value);
    }

  mirror_table_finalize (&t);

  /* Offsets in t.result, in memory of this process.  */
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
    fprintf (stream, "#define mirror_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream, "static const\n");
  fprintf (stream, "struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    short level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    int level3[%zu << %d];\n", t.level3_size, t.p);
  fprintf (stream, "  }\n");
  fprintf (stream, "u_mirror =\n");
  fprintf (stream, "{\n");
  fprintf (stream, "  {");
  if (t.level1_size > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level3_offset) / sizeof (int32_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level3_size << t.p > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level3_size << t.p; i++)
    {
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " %5d", ((int32_t *) (t.result + level3_offset))[i]);
      if (i+1 < t.level3_size << t.p)
        fprintf (stream, ",");
    }
  if (t.level3_size << t.p > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Particular values of the word break property.  */

static bool
is_WBP_MIDNUMLET (unsigned int ch)
{
  return (ch == 0x002E || ch == 0x2018 || ch == 0x2019
          || ch == 0x2024 || ch == 0xFE52 || ch == 0xFF07 || ch == 0xFF0E);
}

static bool
is_WBP_MIDLETTER (unsigned int ch)
{
  return (ch == 0x00B7 || ch == 0x05F4 || ch == 0x2027 || ch == 0x003A
          || ch == 0x0387 || ch == 0xFE13 || ch == 0xFE55 || ch == 0xFF1A
          || ch == 0x055F);
}

/* ========================================================================= */

/* Properties.  */

/* Reading PropList.txt and DerivedCoreProperties.txt.  */
enum
{
  /* PropList.txt */
  PROP_WHITE_SPACE,
  PROP_BIDI_CONTROL,
  PROP_JOIN_CONTROL,
  PROP_DASH,
  PROP_HYPHEN,
  PROP_QUOTATION_MARK,
  PROP_TERMINAL_PUNCTUATION,
  PROP_OTHER_MATH,
  PROP_HEX_DIGIT,
  PROP_ASCII_HEX_DIGIT,
  PROP_OTHER_ALPHABETIC,
  PROP_IDEOGRAPHIC,
  PROP_DIACRITIC,
  PROP_EXTENDER,
  PROP_OTHER_LOWERCASE,
  PROP_OTHER_UPPERCASE,
  PROP_NONCHARACTER_CODE_POINT,
  PROP_OTHER_GRAPHEME_EXTEND,
  PROP_IDS_BINARY_OPERATOR,
  PROP_IDS_TRINARY_OPERATOR,
  PROP_RADICAL,
  PROP_UNIFIED_IDEOGRAPH,
  PROP_OTHER_DEFAULT_IGNORABLE_CODE_POINT,
  PROP_DEPRECATED,
  PROP_SOFT_DOTTED,
  PROP_LOGICAL_ORDER_EXCEPTION,
  PROP_OTHER_ID_START,
  PROP_OTHER_ID_CONTINUE,
  PROP_STERM,
  PROP_VARIATION_SELECTOR,
  PROP_PATTERN_WHITE_SPACE,
  PROP_PATTERN_SYNTAX,
  PROP_PREPENDED_CONCATENATION_MARK,
  PROP_REGIONAL_INDICATOR,
  /* DerivedCoreProperties.txt */
  PROP_MATH,
  PROP_ALPHABETIC,
  PROP_LOWERCASE,
  PROP_UPPERCASE,
  PROP_CASED,
  PROP_CASE_IGNORABLE,
  PROP_CHANGES_WHEN_LOWERCASED,
  PROP_CHANGES_WHEN_UPPERCASED,
  PROP_CHANGES_WHEN_TITLECASED,
  PROP_CHANGES_WHEN_CASEFOLDED,
  PROP_CHANGES_WHEN_CASEMAPPED,
  PROP_ID_START,
  PROP_ID_CONTINUE,
  PROP_XID_START,
  PROP_XID_CONTINUE,
  PROP_DEFAULT_IGNORABLE_CODE_POINT,
  PROP_GRAPHEME_EXTEND,
  PROP_GRAPHEME_BASE,
  PROP_GRAPHEME_LINK,
  /* emoji-data.txt */
  PROP_EMOJI,
  PROP_EMOJI_PRESENTATION,
  PROP_EMOJI_MODIFIER,
  PROP_EMOJI_MODIFIER_BASE,
  PROP_EMOJI_COMPONENT,
  PROP_EXTENDED_PICTOGRAPHIC
};
unsigned long long unicode_properties[0x110000];

static void
clear_properties (void)
{
  unsigned int i;

  for (i = 0; i < 0x110000; i++)
    unicode_properties[i] = 0;
}

/* Stores in unicode_properties[] the properties from the
   PropList.txt or DerivedCoreProperties.txt file.  */
static void
fill_properties (const char *proplist_filename)
{
  unsigned int i;
  FILE *stream;

  stream = fopen (proplist_filename, "r");
  if (stream == NULL)
    {
      fprintf (stderr, "error during fopen of '%s'\n", proplist_filename);
      exit (1);
    }

  for (;;)
    {
      char buf[200+1];
      unsigned int i1, i2;
      char padding[200+1];
      char propname[200+1];
      unsigned int propvalue;

      if (fscanf (stream, "%200[^\n]\n", buf) < 1)
        break;

      if (buf[0] == '\0' || buf[0] == '#')
        continue;

      if (sscanf (buf, "%X..%X%[ ;]%[^ #]", &i1, &i2, padding, propname) != 4)
        {
          if (sscanf (buf, "%X%[ ;]%[^ #]", &i1, padding, propname) != 3)
            {
              fprintf (stderr, "parse error in '%s'\n", proplist_filename);
              exit (1);
            }
          i2 = i1;
        }
#define PROP(name,value) \
      if (strcmp (propname, name) == 0) propvalue = value; else
      /* PropList.txt */
      PROP ("White_Space", PROP_WHITE_SPACE)
      PROP ("Bidi_Control", PROP_BIDI_CONTROL)
      PROP ("Join_Control", PROP_JOIN_CONTROL)
      PROP ("Dash", PROP_DASH)
      PROP ("Hyphen", PROP_HYPHEN)
      PROP ("Quotation_Mark", PROP_QUOTATION_MARK)
      PROP ("Terminal_Punctuation", PROP_TERMINAL_PUNCTUATION)
      PROP ("Other_Math", PROP_OTHER_MATH)
      PROP ("Hex_Digit", PROP_HEX_DIGIT)
      PROP ("ASCII_Hex_Digit", PROP_ASCII_HEX_DIGIT)
      PROP ("Other_Alphabetic", PROP_OTHER_ALPHABETIC)
      PROP ("Ideographic", PROP_IDEOGRAPHIC)
      PROP ("Diacritic", PROP_DIACRITIC)
      PROP ("Extender", PROP_EXTENDER)
      PROP ("Other_Lowercase", PROP_OTHER_LOWERCASE)
      PROP ("Other_Uppercase", PROP_OTHER_UPPERCASE)
      PROP ("Noncharacter_Code_Point", PROP_NONCHARACTER_CODE_POINT)
      PROP ("Other_Grapheme_Extend", PROP_OTHER_GRAPHEME_EXTEND)
      PROP ("IDS_Binary_Operator", PROP_IDS_BINARY_OPERATOR)
      PROP ("IDS_Trinary_Operator", PROP_IDS_TRINARY_OPERATOR)
      PROP ("Radical", PROP_RADICAL)
      PROP ("Unified_Ideograph", PROP_UNIFIED_IDEOGRAPH)
      PROP ("Other_Default_Ignorable_Code_Point", PROP_OTHER_DEFAULT_IGNORABLE_CODE_POINT)
      PROP ("Deprecated", PROP_DEPRECATED)
      PROP ("Soft_Dotted", PROP_SOFT_DOTTED)
      PROP ("Logical_Order_Exception", PROP_LOGICAL_ORDER_EXCEPTION)
      PROP ("Other_ID_Start", PROP_OTHER_ID_START)
      PROP ("Other_ID_Continue", PROP_OTHER_ID_CONTINUE)
      PROP ("Sentence_Terminal", PROP_STERM)
      PROP ("Variation_Selector", PROP_VARIATION_SELECTOR)
      PROP ("Pattern_White_Space", PROP_PATTERN_WHITE_SPACE)
      PROP ("Pattern_Syntax", PROP_PATTERN_SYNTAX)
      PROP ("Prepended_Concatenation_Mark", PROP_PREPENDED_CONCATENATION_MARK)
      PROP ("Regional_Indicator", PROP_REGIONAL_INDICATOR)
      /* DerivedCoreProperties.txt */
      PROP ("Math", PROP_MATH)
      PROP ("Alphabetic", PROP_ALPHABETIC)
      PROP ("Lowercase", PROP_LOWERCASE)
      PROP ("Uppercase", PROP_UPPERCASE)
      PROP ("Cased", PROP_CASED)
      PROP ("Case_Ignorable", PROP_CASE_IGNORABLE)
      PROP ("Changes_When_Lowercased", PROP_CHANGES_WHEN_LOWERCASED)
      PROP ("Changes_When_Uppercased", PROP_CHANGES_WHEN_UPPERCASED)
      PROP ("Changes_When_Titlecased", PROP_CHANGES_WHEN_TITLECASED)
      PROP ("Changes_When_Casefolded", PROP_CHANGES_WHEN_CASEFOLDED)
      PROP ("Changes_When_Casemapped", PROP_CHANGES_WHEN_CASEMAPPED)
      PROP ("ID_Start", PROP_ID_START)
      PROP ("ID_Continue", PROP_ID_CONTINUE)
      PROP ("XID_Start", PROP_XID_START)
      PROP ("XID_Continue", PROP_XID_CONTINUE)
      PROP ("Default_Ignorable_Code_Point", PROP_DEFAULT_IGNORABLE_CODE_POINT)
      PROP ("Grapheme_Extend", PROP_GRAPHEME_EXTEND)
      PROP ("Grapheme_Base", PROP_GRAPHEME_BASE)
      PROP ("Grapheme_Link", PROP_GRAPHEME_LINK)
      /* emoji-data.txt */
      PROP ("Emoji", PROP_EMOJI)
      PROP ("Emoji_Presentation", PROP_EMOJI_PRESENTATION)
      PROP ("Emoji_Modifier", PROP_EMOJI_MODIFIER)
      PROP ("Emoji_Modifier_Base", PROP_EMOJI_MODIFIER_BASE)
      PROP ("Emoji_Component", PROP_EMOJI_COMPONENT)
      PROP ("Extended_Pictographic", PROP_EXTENDED_PICTOGRAPHIC)
#undef PROP
        {
          fprintf (stderr, "unknown property named '%s' in '%s'\n", propname,
                   proplist_filename);
          exit (1);
        }
      assert (i1 <= i2 && i2 < 0x110000);

      for (i = i1; i <= i2; i++)
        unicode_properties[i] |= 1ULL << propvalue;
    }

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error reading from '%s'\n", proplist_filename);
      exit (1);
    }
}

/* Stores in array the given property from the Unicode 3.0 PropList.txt
   file.  */
static void
fill_property30 (char array[0x110000], const char *proplist_filename, const char *property_name)
{
  unsigned int i;
  FILE *stream;
  char buf[100+1];

  for (i = 0; i < 0x110000; i++)
    array[i] = 0;

  stream = fopen (proplist_filename, "r");
  if (stream == NULL)
    {
      fprintf (stderr, "error during fopen of '%s'\n", proplist_filename);
      exit (1);
    }

  /* Search for the "Property dump for: ..." line.  */
  do
    {
      if (fscanf (stream, "%100[^\n]\n", buf) < 1)
        {
          fprintf (stderr, "no property found in '%s'\n", proplist_filename);
          exit (1);
        }
    }
  while (strstr (buf, property_name) == NULL);

  for (;;)
    {
      unsigned int i1, i2;

      if (fscanf (stream, "%100[^\n]\n", buf) < 1)
        break;
      if (buf[0] == '*')
        break;
      if (strlen (buf) >= 10 && buf[4] == '.' && buf[5] == '.')
        {
          if (sscanf (buf, "%4X..%4X", &i1, &i2) < 2)
            {
              fprintf (stderr, "parse error in property in '%s'\n",
                       proplist_filename);
              exit (1);
            }
        }
      else if (strlen (buf) >= 4)
        {
          if (sscanf (buf, "%4X", &i1) < 1)
            {
              fprintf (stderr, "parse error in property in '%s'\n",
                       proplist_filename);
              exit (1);
            }
          i2 = i1;
        }
      else
        {
          fprintf (stderr, "parse error in property in '%s'\n",
                   proplist_filename);
          exit (1);
        }
      assert (i1 <= i2 && i2 < 0x110000);
      for (i = i1; i <= i2; i++)
        array[i] = 1;
    }

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error reading from '%s'\n", proplist_filename);
      exit (1);
    }
}

/* Properties from Unicode 3.0 PropList.txt file.  */

/* The paired punctuation property from the PropList.txt file.  */
char unicode_pairedpunctuation[0x110000];

/* The left of pair property from the PropList.txt file.  */
char unicode_leftofpair[0x110000];

static void
fill_properties30 (const char *proplist30_filename)
{
  fill_property30 (unicode_pairedpunctuation, proplist30_filename, "(Paired Punctuation)");
  fill_property30 (unicode_leftofpair, proplist30_filename, "(Left of Pair)");
}

/* ------------------------------------------------------------------------- */

/* See PropList.txt, UCD.html.  */
static bool
is_property_white_space (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_WHITE_SPACE)) != 0);
}

/* See Unicode 3.0 book, section 4.10,
       PropList.txt, UCD.html,
       DerivedCoreProperties.txt, UCD.html.  */
static bool
is_property_alphabetic (unsigned int ch)
{
  bool result1 =
    is_category_L (ch)
    || ((unicode_properties[ch] & (1ULL << PROP_OTHER_ALPHABETIC)) != 0)
    /* For some reason, the following are listed as having property
       Alphabetic but not as having property Other_Alphabetic.  */
    || (ch >= 0x16EE && ch <= 0x16F0) /* RUNIC SYMBOLS */
    || (ch >= 0x2160 && ch <= 0x2182) /* ROMAN NUMERALS */
    || (ch >= 0x2185 && ch <= 0x2188) /* ROMAN NUMERALS */
    || (ch >= 0x24D0 && ch <= 0x24E9) /* CIRCLED LATIN SMALL LETTER */
    || (ch == 0x3007) /* IDEOGRAPHIC NUMBER ZERO */
    || (ch >= 0x3021 && ch <= 0x3029) /* HANGZHOU NUMERAL */
    || (ch >= 0x3038 && ch <= 0x303A) /* HANGZHOU NUMERAL */
    || (ch >= 0xA6E6 && ch <= 0xA6EF) /* BAMUM LETTERS */
    || (ch >= 0x10140 && ch <= 0x10174) /* GREEK ACROPHONICS */
    || (ch == 0x10341) /* GOTHIC LETTER NINETY */
    || (ch == 0x1034A) /* GOTHIC LETTER NINE HUNDRED */
    || (ch >= 0x103D1 && ch <= 0x103D5) /* OLD PERSIAN NUMBERS */
    || (ch >= 0x12400 && ch <= 0x1246E); /* CUNEIFORM NUMERIC SIGNS */
  bool result2 =
    ((unicode_properties[ch] & (1ULL << PROP_ALPHABETIC)) != 0);

  assert (result1 == result2);
  return result1;
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_other_alphabetic (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_OTHER_ALPHABETIC)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_not_a_character (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_NONCHARACTER_CODE_POINT)) != 0);
}

/* See PropList.txt, UCD.html,
       DerivedCoreProperties.txt, UCD.html.  */
static bool
is_property_default_ignorable_code_point (unsigned int ch)
{
  bool result1 =
    (is_category_Cf (ch)
     && !(ch >= 0xFFF9 && ch <= 0xFFFB) /* Annotations */
     && !(ch >= 0x13430 && ch <= 0x13438) /* Egyptian Hieroglyph */
     && ((unicode_properties[ch] & (1ULL << PROP_PREPENDED_CONCATENATION_MARK)) == 0))
    || ((unicode_properties[ch] & (1ULL << PROP_OTHER_DEFAULT_IGNORABLE_CODE_POINT)) != 0)
    || ((unicode_properties[ch] & (1ULL << PROP_VARIATION_SELECTOR)) != 0);
  bool result2 =
    ((unicode_properties[ch] & (1ULL << PROP_DEFAULT_IGNORABLE_CODE_POINT)) != 0);

  assert (result1 == result2);
  return result1;
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_other_default_ignorable_code_point (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_OTHER_DEFAULT_IGNORABLE_CODE_POINT)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_deprecated (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_DEPRECATED)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_logical_order_exception (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_LOGICAL_ORDER_EXCEPTION)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_variation_selector (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_VARIATION_SELECTOR)) != 0);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_private_use (unsigned int ch)
{
  /* Determined through "grep 'Private Use,' UnicodeData-3.1.0.txt".  */
  return (ch >= 0xE000 && ch <= 0xF8FF)
         || (ch >= 0xF0000 && ch <= 0xFFFFD)
         || (ch >= 0x100000 && ch <= 0x10FFFD);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_unassigned_code_value (unsigned int ch)
{
  return (is_category_Cn (ch) && !is_property_not_a_character (ch));
}

/* See PropList.txt, UCD.html,
       DerivedCoreProperties.txt, UCD.html.  */
static bool
is_property_uppercase (unsigned int ch)
{
  bool result1 =
    is_category_Lu (ch)
    || ((unicode_properties[ch] & (1ULL << PROP_OTHER_UPPERCASE)) != 0);
  bool result2 =
    ((unicode_properties[ch] & (1ULL << PROP_UPPERCASE)) != 0);

  assert (result1 == result2);
  return result1;
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_other_uppercase (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_OTHER_UPPERCASE)) != 0);
}

/* See PropList.txt, UCD.html,
       DerivedCoreProperties.txt, UCD.html.  */
static bool
is_property_lowercase (unsigned int ch)
{
  bool result1 =
    is_category_Ll (ch)
    || ((unicode_properties[ch] & (1ULL << PROP_OTHER_LOWERCASE)) != 0);
  bool result2 =
    ((unicode_properties[ch] & (1ULL << PROP_LOWERCASE)) != 0);

  assert (result1 == result2);
  return result1;
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_other_lowercase (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_OTHER_LOWERCASE)) != 0);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_titlecase (unsigned int ch)
{
  return is_category_Lt (ch);
}

/* See DerivedCoreProperties.txt.  */
static bool
is_property_cased (unsigned int ch)
{
  bool result1 = (is_property_lowercase (ch)
                  || is_property_uppercase (ch)
                  || is_category_Lt (ch));
  bool result2 = ((unicode_properties[ch] & (1ULL << PROP_CASED)) != 0);

  assert (result1 == result2);
  return result1;
}

/* See DerivedCoreProperties.txt.  */
static bool
is_property_case_ignorable (unsigned int ch)
{
  bool result1 = (is_WBP_MIDLETTER (ch) || is_WBP_MIDNUMLET (ch)
                  || ch == 0x0027
                  || is_category_Mn (ch)
                  || is_category_Me (ch)
                  || is_category_Cf (ch)
                  || is_category_Lm (ch)
                  || is_category_Sk (ch));
  bool result2 = ((unicode_properties[ch] & (1ULL << PROP_CASE_IGNORABLE)) != 0);

  assert (result1 == result2);
  return result1;
}

/* See DerivedCoreProperties.txt.  */
static bool
is_property_changes_when_lowercased (unsigned int ch)
{
  bool result1 = ((unicode_properties[ch] & (1ULL << PROP_CHANGES_WHEN_LOWERCASED)) != 0);
  bool result2 = (unicode_attributes[ch].name != NULL
                  && unicode_attributes[ch].lower != NONE
                  && unicode_attributes[ch].lower != ch);

  assert (result1 == result2);
  return result1;
}

/* See DerivedCoreProperties.txt.  */
static bool
is_property_changes_when_uppercased (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_CHANGES_WHEN_UPPERCASED)) != 0);
}

/* See DerivedCoreProperties.txt.  */
static bool
is_property_changes_when_titlecased (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_CHANGES_WHEN_TITLECASED)) != 0);
}

/* See DerivedCoreProperties.txt.  */
static bool
is_property_changes_when_casefolded (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_CHANGES_WHEN_CASEFOLDED)) != 0);
}

/* See DerivedCoreProperties.txt.  */
static bool
is_property_changes_when_casemapped (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_CHANGES_WHEN_CASEMAPPED)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_soft_dotted (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_SOFT_DOTTED)) != 0);
}

/* See DerivedCoreProperties.txt, UCD.html.  */
static bool
is_property_id_start (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_ID_START)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_other_id_start (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_OTHER_ID_START)) != 0);
}

/* See DerivedCoreProperties.txt, UCD.html.  */
static bool
is_property_id_continue (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_ID_CONTINUE)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_other_id_continue (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_OTHER_ID_CONTINUE)) != 0);
}

/* See DerivedCoreProperties.txt, UCD.html.  */
static bool
is_property_xid_start (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_XID_START)) != 0);
}

/* See DerivedCoreProperties.txt, UCD.html.  */
static bool
is_property_xid_continue (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_XID_CONTINUE)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_pattern_white_space (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_PATTERN_WHITE_SPACE)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_pattern_syntax (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_PATTERN_SYNTAX)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_join_control (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_JOIN_CONTROL)) != 0);
}

/* See DerivedCoreProperties.txt, UCD.html.  */
static bool
is_property_grapheme_base (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_GRAPHEME_BASE)) != 0);
}

/* See DerivedCoreProperties.txt, UCD.html.  */
static bool
is_property_grapheme_extend (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_GRAPHEME_EXTEND)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_other_grapheme_extend (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_OTHER_GRAPHEME_EXTEND)) != 0);
}

/* See DerivedCoreProperties.txt, UCD.html.  */
static bool
is_property_grapheme_link (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_GRAPHEME_LINK)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_bidi_control (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_BIDI_CONTROL)) != 0);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_left_to_right (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_L);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_hebrew_right_to_left (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_R);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_arabic_right_to_left (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_AL);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_european_digit (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_EN);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_eur_num_separator (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_ES);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_eur_num_terminator (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_ET);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_arabic_digit (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_AN);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_common_separator (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_CS);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_block_separator (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_B);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_segment_separator (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_S);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_whitespace (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_WS);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_non_spacing_mark (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_NSM);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_boundary_neutral (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_BN);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_pdf (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_PDF);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_embedding_or_override (unsigned int ch)
{
  int category = get_bidi_category (ch);
  return (category == UC_BIDI_LRE || category == UC_BIDI_LRO
          || category == UC_BIDI_RLE || category == UC_BIDI_RLO);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_bidi_other_neutral (unsigned int ch)
{
  return (get_bidi_category (ch) == UC_BIDI_ON);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_hex_digit (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_HEX_DIGIT)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_ascii_hex_digit (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_ASCII_HEX_DIGIT)) != 0);
}

/* See Unicode 3.0 book, section 4.10,
       PropList.txt, UCD.html.  */
static bool
is_property_ideographic (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_IDEOGRAPHIC)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_unified_ideograph (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_UNIFIED_IDEOGRAPH)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_radical (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_RADICAL)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_ids_binary_operator (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_IDS_BINARY_OPERATOR)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_ids_trinary_operator (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_IDS_TRINARY_OPERATOR)) != 0);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_zero_width (unsigned int ch)
{
  return is_category_Cf (ch)
         || (unicode_attributes[ch].name != NULL
             && strstr (unicode_attributes[ch].name, "ZERO WIDTH") != NULL);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_space (unsigned int ch)
{
  return is_category_Zs (ch);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_non_break (unsigned int ch)
{
  /* This is exactly the set of characters having line breaking
     property GL.  */
  return (ch == 0x00A0 /* NO-BREAK SPACE */
          || ch == 0x034F /* COMBINING GRAPHEME JOINER */
          || ch == 0x035C /* COMBINING DOUBLE BREVE BELOW */
          || ch == 0x035D /* COMBINING DOUBLE BREVE */
          || ch == 0x035E /* COMBINING DOUBLE MACRON */
          || ch == 0x035F /* COMBINING DOUBLE MACRON BELOW */
          || ch == 0x0360 /* COMBINING DOUBLE TILDE */
          || ch == 0x0361 /* COMBINING DOUBLE INVERTED BREVE */
          || ch == 0x0362 /* COMBINING DOUBLE RIGHTWARDS ARROW BELOW */
          || ch == 0x0F08 /* TIBETAN MARK SBRUL SHAD */
          || ch == 0x0F0C /* TIBETAN MARK DELIMITER TSHEG BSTAR */
          || ch == 0x0F12 /* TIBETAN MARK RGYA GRAM SHAD */
          || ch == 0x180E /* MONGOLIAN VOWEL SEPARATOR */
          || ch == 0x2007 /* FIGURE SPACE */
          || ch == 0x2011 /* NON-BREAKING HYPHEN */
          || ch == 0x202F /* NARROW NO-BREAK SPACE */);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_iso_control (unsigned int ch)
{
  bool result1 =
    (unicode_attributes[ch].name != NULL
     && strcmp (unicode_attributes[ch].name, "<control>") == 0);
  bool result2 =
    is_category_Cc (ch);

  assert (result1 == result2);
  return result1;
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_format_control (unsigned int ch)
{
  return (is_category_Cf (ch)
          && get_bidi_category (ch) == UC_BIDI_BN
          && !is_property_join_control (ch)
          && ch != 0xFEFF);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_dash (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_DASH)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_hyphen (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_HYPHEN)) != 0);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_punctuation (unsigned int ch)
{
  return is_category_P (ch);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_line_separator (unsigned int ch)
{
  return is_category_Zl (ch);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_paragraph_separator (unsigned int ch)
{
  return is_category_Zp (ch);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_quotation_mark (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_QUOTATION_MARK)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_sentence_terminal (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_STERM)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_terminal_punctuation (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_TERMINAL_PUNCTUATION)) != 0);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_currency_symbol (unsigned int ch)
{
  return is_category_Sc (ch);
}

/* See Unicode 3.0 book, section 4.9,
       PropList.txt, UCD.html,
       DerivedCoreProperties.txt, UCD.html.  */
static bool
is_property_math (unsigned int ch)
{
  bool result1 =
    is_category_Sm (ch)
    || ((unicode_properties[ch] & (1ULL << PROP_OTHER_MATH)) != 0);
  bool result2 =
    ((unicode_properties[ch] & (1ULL << PROP_MATH)) != 0);

  assert (result1 == result2);
  return result1;
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_other_math (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_OTHER_MATH)) != 0);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_paired_punctuation (unsigned int ch)
{
  return unicode_pairedpunctuation[ch];
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_left_of_pair (unsigned int ch)
{
  return unicode_leftofpair[ch];
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_combining (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && (strcmp (unicode_attributes[ch].combining, "0") != 0
              || is_category_Mc (ch)
              || is_category_Me (ch)
              || is_category_Mn (ch)));
}

#if 0 /* same as is_property_bidi_non_spacing_mark */
/* See PropList-3.0.1.txt.  */
static bool
is_property_non_spacing (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && get_bidi_category (ch) == UC_BIDI_NSM);
}
#endif

/* See PropList-3.0.1.txt.  */
static bool
is_property_composite (unsigned int ch)
{
  /* This definition differs from the one in PropList-3.0.1.txt, but is more
     logical in some sense.  */
  if (ch >= 0xAC00 && ch <= 0xD7A4) /* Hangul Syllables */
    return true;
  if (unicode_attributes[ch].name != NULL
      && unicode_attributes[ch].decomposition != NULL)
    {
      /* Test whether the decomposition contains more than one character,
         and the first is not a space.  */
      const char *decomp = unicode_attributes[ch].decomposition;
      if (decomp[0] == '<')
        {
          decomp = strchr (decomp, '>') + 1;
          if (decomp[0] == ' ')
            decomp++;
        }
      return strchr (decomp, ' ') != NULL && strncmp (decomp, "0020 ", 5) != 0;
    }
  return false;
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_decimal_digit (unsigned int ch)
{
  return is_category_Nd (ch);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_numeric (unsigned int ch)
{
  return ((get_numeric_value (ch)).denominator > 0)
         || (ch == 0x09F8) /* BENGALI CURRENCY NUMERATOR ONE LESS THAN THE DENOMINATOR */
         || (ch == 0x2183); /* ROMAN NUMERAL REVERSED ONE HUNDRED */
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_diacritic (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_DIACRITIC)) != 0);
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_extender (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_EXTENDER)) != 0);
}

/* See PropList-3.0.1.txt.  */
static bool
is_property_ignorable_control (unsigned int ch)
{
  return ((is_category_Cc (ch) && get_bidi_category (ch) == UC_BIDI_BN)
          || is_category_Cf (ch))
         && ch != 0x0000;
}

/* See PropList.txt, UCD.html.  */
static bool
is_property_regional_indicator (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_REGIONAL_INDICATOR)) != 0);
}

/* See emoji-data.txt, UTS #51.  */
static bool
is_property_emoji (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_EMOJI)) != 0);
}

/* See emoji-data.txt, UTS #51.  */
static bool
is_property_emoji_presentation (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_EMOJI_PRESENTATION)) != 0);
}

/* See emoji-data.txt, UTS #51.  */
static bool
is_property_emoji_modifier (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_EMOJI_MODIFIER)) != 0);
}

/* See emoji-data.txt, UTS #51.  */
static bool
is_property_emoji_modifier_base (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_EMOJI_MODIFIER_BASE)) != 0);
}

/* See emoji-data.txt, UTS #51.  */
static bool
is_property_emoji_component (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_EMOJI_COMPONENT)) != 0);
}

/* See emoji-data.txt, UTS #51.  */
static bool
is_property_extended_pictographic (unsigned int ch)
{
  return ((unicode_properties[ch] & (1ULL << PROP_EXTENDED_PICTOGRAPHIC)) != 0);
}

/* ------------------------------------------------------------------------- */

/* Output all properties.  */
static void
output_properties (const char *version)
{
#define PROPERTY(P) \
  debug_output_predicate ("unictype/pr_" #P ".txt", is_property_ ## P); \
  output_predicate_test ("../tests/unictype/test-pr_" #P ".c", is_property_ ## P, "uc_is_property_" #P " (c)"); \
  output_predicate ("unictype/pr_" #P ".h", is_property_ ## P, "u_property_" #P, "Properties", version);
  PROPERTY(white_space)
  PROPERTY(alphabetic)
  PROPERTY(other_alphabetic)
  PROPERTY(not_a_character)
  PROPERTY(default_ignorable_code_point)
  PROPERTY(other_default_ignorable_code_point)
  PROPERTY(deprecated)
  PROPERTY(logical_order_exception)
  PROPERTY(variation_selector)
  PROPERTY(private_use)
  PROPERTY(unassigned_code_value)
  PROPERTY(uppercase)
  PROPERTY(other_uppercase)
  PROPERTY(lowercase)
  PROPERTY(other_lowercase)
  PROPERTY(titlecase)
  PROPERTY(cased)
  PROPERTY(case_ignorable)
  PROPERTY(changes_when_lowercased)
  PROPERTY(changes_when_uppercased)
  PROPERTY(changes_when_titlecased)
  PROPERTY(changes_when_casefolded)
  PROPERTY(changes_when_casemapped)
  PROPERTY(soft_dotted)
  PROPERTY(id_start)
  PROPERTY(other_id_start)
  PROPERTY(id_continue)
  PROPERTY(other_id_continue)
  PROPERTY(xid_start)
  PROPERTY(xid_continue)
  PROPERTY(pattern_white_space)
  PROPERTY(pattern_syntax)
  PROPERTY(join_control)
  PROPERTY(grapheme_base)
  PROPERTY(grapheme_extend)
  PROPERTY(other_grapheme_extend)
  PROPERTY(grapheme_link)
  PROPERTY(bidi_control)
  PROPERTY(bidi_left_to_right)
  PROPERTY(bidi_hebrew_right_to_left)
  PROPERTY(bidi_arabic_right_to_left)
  PROPERTY(bidi_european_digit)
  PROPERTY(bidi_eur_num_separator)
  PROPERTY(bidi_eur_num_terminator)
  PROPERTY(bidi_arabic_digit)
  PROPERTY(bidi_common_separator)
  PROPERTY(bidi_block_separator)
  PROPERTY(bidi_segment_separator)
  PROPERTY(bidi_whitespace)
  PROPERTY(bidi_non_spacing_mark)
  PROPERTY(bidi_boundary_neutral)
  PROPERTY(bidi_pdf)
  PROPERTY(bidi_embedding_or_override)
  PROPERTY(bidi_other_neutral)
  PROPERTY(hex_digit)
  PROPERTY(ascii_hex_digit)
  PROPERTY(ideographic)
  PROPERTY(unified_ideograph)
  PROPERTY(radical)
  PROPERTY(ids_binary_operator)
  PROPERTY(ids_trinary_operator)
  PROPERTY(zero_width)
  PROPERTY(space)
  PROPERTY(non_break)
  PROPERTY(iso_control)
  PROPERTY(format_control)
  PROPERTY(dash)
  PROPERTY(hyphen)
  PROPERTY(punctuation)
  PROPERTY(line_separator)
  PROPERTY(paragraph_separator)
  PROPERTY(quotation_mark)
  PROPERTY(sentence_terminal)
  PROPERTY(terminal_punctuation)
  PROPERTY(currency_symbol)
  PROPERTY(math)
  PROPERTY(other_math)
  PROPERTY(paired_punctuation)
  PROPERTY(left_of_pair)
  PROPERTY(combining)
  PROPERTY(composite)
  PROPERTY(decimal_digit)
  PROPERTY(numeric)
  PROPERTY(diacritic)
  PROPERTY(extender)
  PROPERTY(ignorable_control)
  PROPERTY(regional_indicator)
  PROPERTY(emoji)
  PROPERTY(emoji_presentation)
  PROPERTY(emoji_modifier)
  PROPERTY(emoji_modifier_base)
  PROPERTY(emoji_component)
  PROPERTY(extended_pictographic)
#undef PROPERTY
}

/* ========================================================================= */

/* Arabic Shaping.  */

enum
{
  UC_JOINING_TYPE_U, /* Non_Joining */
  UC_JOINING_TYPE_T, /* Transparent */
  UC_JOINING_TYPE_C, /* Join_Causing */
  UC_JOINING_TYPE_L, /* Left_Joining */
  UC_JOINING_TYPE_R, /* Right_Joining */
  UC_JOINING_TYPE_D  /* Dual_Joining */
};

static uint8_t unicode_joining_type[0x110000];

enum
{
  UC_JOINING_GROUP_NONE,                     /* No_Joining_Group */
  UC_JOINING_GROUP_AIN,                      /* Ain */
  UC_JOINING_GROUP_ALAPH,                    /* Alaph */
  UC_JOINING_GROUP_ALEF,                     /* Alef */
  UC_JOINING_GROUP_BEH,                      /* Beh */
  UC_JOINING_GROUP_BETH,                     /* Beth */
  UC_JOINING_GROUP_BURUSHASKI_YEH_BARREE,    /* Burushaski_Yeh_Barree */
  UC_JOINING_GROUP_DAL,                      /* Dal */
  UC_JOINING_GROUP_DALATH_RISH,              /* Dalath_Rish */
  UC_JOINING_GROUP_E,                        /* E */
  UC_JOINING_GROUP_FARSI_YEH,                /* Farsi_Yeh */
  UC_JOINING_GROUP_FE,                       /* Fe */
  UC_JOINING_GROUP_FEH,                      /* Feh */
  UC_JOINING_GROUP_FINAL_SEMKATH,            /* Final_Semkath */
  UC_JOINING_GROUP_GAF,                      /* Gaf */
  UC_JOINING_GROUP_GAMAL,                    /* Gamal */
  UC_JOINING_GROUP_HAH,                      /* Hah */
  UC_JOINING_GROUP_HE,                       /* He */
  UC_JOINING_GROUP_HEH,                      /* Heh */
  UC_JOINING_GROUP_HEH_GOAL,                 /* Heh_Goal */
  UC_JOINING_GROUP_HETH,                     /* Heth */
  UC_JOINING_GROUP_KAF,                      /* Kaf */
  UC_JOINING_GROUP_KAPH,                     /* Kaph */
  UC_JOINING_GROUP_KHAPH,                    /* Khaph */
  UC_JOINING_GROUP_KNOTTED_HEH,              /* Knotted_Heh */
  UC_JOINING_GROUP_LAM,                      /* Lam */
  UC_JOINING_GROUP_LAMADH,                   /* Lamadh */
  UC_JOINING_GROUP_MEEM,                     /* Meem */
  UC_JOINING_GROUP_MIM,                      /* Mim */
  UC_JOINING_GROUP_NOON,                     /* Noon */
  UC_JOINING_GROUP_NUN,                      /* Nun */
  UC_JOINING_GROUP_NYA,                      /* Nya */
  UC_JOINING_GROUP_PE,                       /* Pe */
  UC_JOINING_GROUP_QAF,                      /* Qaf */
  UC_JOINING_GROUP_QAPH,                     /* Qaph */
  UC_JOINING_GROUP_REH,                      /* Reh */
  UC_JOINING_GROUP_REVERSED_PE,              /* Reversed_Pe */
  UC_JOINING_GROUP_SAD,                      /* Sad */
  UC_JOINING_GROUP_SADHE,                    /* Sadhe */
  UC_JOINING_GROUP_SEEN,                     /* Seen */
  UC_JOINING_GROUP_SEMKATH,                  /* Semkath */
  UC_JOINING_GROUP_SHIN,                     /* Shin */
  UC_JOINING_GROUP_SWASH_KAF,                /* Swash_Kaf */
  UC_JOINING_GROUP_SYRIAC_WAW,               /* Syriac_Waw */
  UC_JOINING_GROUP_TAH,                      /* Tah */
  UC_JOINING_GROUP_TAW,                      /* Taw */
  UC_JOINING_GROUP_TEH_MARBUTA,              /* Teh_Marbuta */
  UC_JOINING_GROUP_TEH_MARBUTA_GOAL,         /* Teh_Marbuta_Goal */
  UC_JOINING_GROUP_TETH,                     /* Teth */
  UC_JOINING_GROUP_WAW,                      /* Waw */
  UC_JOINING_GROUP_YEH,                      /* Yeh */
  UC_JOINING_GROUP_YEH_BARREE,               /* Yeh_Barree */
  UC_JOINING_GROUP_YEH_WITH_TAIL,            /* Yeh_With_Tail */
  UC_JOINING_GROUP_YUDH,                     /* Yudh */
  UC_JOINING_GROUP_YUDH_HE,                  /* Yudh_He */
  UC_JOINING_GROUP_ZAIN,                     /* Zain */
  UC_JOINING_GROUP_ZHAIN,                    /* Zhain */
  UC_JOINING_GROUP_ROHINGYA_YEH,             /* Rohingya_Yeh */
  UC_JOINING_GROUP_STRAIGHT_WAW,             /* Straight_Waw */
  UC_JOINING_GROUP_MANICHAEAN_ALEPH,         /* Manichaean_Aleph */
  UC_JOINING_GROUP_MANICHAEAN_BETH,          /* Manichaean_Beth */
  UC_JOINING_GROUP_MANICHAEAN_GIMEL,         /* Manichaean_Gimel */
  UC_JOINING_GROUP_MANICHAEAN_DALETH,        /* Manichaean_Daleth */
  UC_JOINING_GROUP_MANICHAEAN_WAW,           /* Manichaean_Waw */
  UC_JOINING_GROUP_MANICHAEAN_ZAYIN,         /* Manichaean_Zayin */
  UC_JOINING_GROUP_MANICHAEAN_HETH,          /* Manichaean_Heth */
  UC_JOINING_GROUP_MANICHAEAN_TETH,          /* Manichaean_Teth */
  UC_JOINING_GROUP_MANICHAEAN_YODH,          /* Manichaean_Yodh */
  UC_JOINING_GROUP_MANICHAEAN_KAPH,          /* Manichaean_Kaph */
  UC_JOINING_GROUP_MANICHAEAN_LAMEDH,        /* Manichaean_Lamedh */
  UC_JOINING_GROUP_MANICHAEAN_DHAMEDH,       /* Manichaean_Dhamedh */
  UC_JOINING_GROUP_MANICHAEAN_THAMEDH,       /* Manichaean_Thamedh */
  UC_JOINING_GROUP_MANICHAEAN_MEM,           /* Manichaean_Mem */
  UC_JOINING_GROUP_MANICHAEAN_NUN,           /* Manichaean_Nun */
  UC_JOINING_GROUP_MANICHAEAN_SAMEKH,        /* Manichaean_Aleph */
  UC_JOINING_GROUP_MANICHAEAN_AYIN,          /* Manichaean_Ayin */
  UC_JOINING_GROUP_MANICHAEAN_PE,            /* Manichaean_Pe */
  UC_JOINING_GROUP_MANICHAEAN_SADHE,         /* Manichaean_Sadhe */
  UC_JOINING_GROUP_MANICHAEAN_QOPH,          /* Manichaean_Qoph */
  UC_JOINING_GROUP_MANICHAEAN_RESH,          /* Manichaean_Resh */
  UC_JOINING_GROUP_MANICHAEAN_TAW,           /* Manichaean_Taw */
  UC_JOINING_GROUP_MANICHAEAN_ONE,           /* Manichaean_One */
  UC_JOINING_GROUP_MANICHAEAN_FIVE,          /* Manichaean_Five */
  UC_JOINING_GROUP_MANICHAEAN_TEN,           /* Manichaean_Ten */
  UC_JOINING_GROUP_MANICHAEAN_TWENTY,        /* Manichaean_Twenty */
  UC_JOINING_GROUP_MANICHAEAN_HUNDRED,       /* Manichaean_Hundred */
  UC_JOINING_GROUP_AFRICAN_FEH,              /* African_Feh */
  UC_JOINING_GROUP_AFRICAN_QAF,              /* African_Qaf */
  UC_JOINING_GROUP_AFRICAN_NOON,             /* African_Noon */
  UC_JOINING_GROUP_MALAYALAM_NGA,            /* Malayalam_Nga */
  UC_JOINING_GROUP_MALAYALAM_JA,             /* Malayalam_Ja */
  UC_JOINING_GROUP_MALAYALAM_NYA,            /* Malayalam_Nya */
  UC_JOINING_GROUP_MALAYALAM_TTA,            /* Malayalam_Tta */
  UC_JOINING_GROUP_MALAYALAM_NNA,            /* Malayalam_Nna */
  UC_JOINING_GROUP_MALAYALAM_NNNA,           /* Malayalam_Nnna */
  UC_JOINING_GROUP_MALAYALAM_BHA,            /* Malayalam_Bha */
  UC_JOINING_GROUP_MALAYALAM_RA,             /* Malayalam_Ra */
  UC_JOINING_GROUP_MALAYALAM_LLA,            /* Malayalam_Lla */
  UC_JOINING_GROUP_MALAYALAM_LLLA,           /* Malayalam_Llla */
  UC_JOINING_GROUP_MALAYALAM_SSA,            /* Malayalam_Ssa */
  UC_JOINING_GROUP_HANIFI_ROHINGYA_PA,       /* Hanifi_Rohingya_Pa */
  UC_JOINING_GROUP_HANIFI_ROHINGYA_KINNA_YA, /* Hanifi_Rohingya_Kinna_Ya */
  UC_JOINING_GROUP_THIN_YEH,                 /* Thin_Yeh */
  UC_JOINING_GROUP_VERTICAL_TAIL             /* Vertical_Tail */
};

static uint8_t unicode_joining_group[0x110000];

static void
fill_arabicshaping (const char *arabicshaping_filename)
{
  FILE *stream;
  unsigned int i;
  int lineno;

  stream = fopen (arabicshaping_filename, "r");
  if (stream == NULL)
    {
      fprintf (stderr, "error during fopen of '%s'\n", arabicshaping_filename);
      exit (1);
    }

  for (i = 0; i < 0x110000; i++)
    {
      unicode_joining_type[i] = (uint8_t)~(uint8_t)0;
      unicode_joining_group[i] = UC_JOINING_GROUP_NONE;
    }

  lineno = 0;
  for (;;)
    {
      char buf[200+1];
      char separator1[200+1];
      char schematic_name[200+1];
      char separator2[200+1];
      char joining_type_name[200+1];
      char separator3[200+1];
      char joining_group_name[200+1];
      int joining_type;
      int joining_group;

      lineno++;
      if (fscanf (stream, "%200[^\n]\n", buf) < 1)
        break;

      if (buf[0] == '\0' || buf[0] == '#')
        continue;

      if (sscanf (buf, "%X%[; ]%[^;]%[; ]%[^;]%[; ]%100[^\n]",
                  &i, separator1, schematic_name, separator2, joining_type_name,
                  separator3, joining_group_name) != 7)
        {
          fprintf (stderr, "parse error in '%s':%d\n",
                   arabicshaping_filename, lineno);
          exit (1);
        }
      assert (i < 0x110000);

#define TRY(name) else if (strcmp (joining_type_name, #name + 16) == 0) joining_type = name;
      if (false) {}
      TRY(UC_JOINING_TYPE_U)
      TRY(UC_JOINING_TYPE_T)
      TRY(UC_JOINING_TYPE_C)
      TRY(UC_JOINING_TYPE_L)
      TRY(UC_JOINING_TYPE_R)
      TRY(UC_JOINING_TYPE_D)
#undef TRY
      else
        {
          fprintf (stderr, "unknown joining type value \"%s\" in '%s':%d\n",
                   joining_type_name, arabicshaping_filename, lineno);
          exit (1);
        }

      /* Remove trailing spaces.  */
      while (joining_group_name[0] != '\0'
             && joining_group_name[strlen (joining_group_name) - 1] == ' ')
        joining_group_name[strlen (joining_group_name) - 1] = '\0';

#define TRY(value,name) else if (strcmp (joining_group_name, name) == 0) joining_group = value;
      if (false) {}
      TRY(UC_JOINING_GROUP_NONE,                     "No_Joining_Group")
      TRY(UC_JOINING_GROUP_AIN,                      "AIN")
      TRY(UC_JOINING_GROUP_ALAPH,                    "ALAPH")
      TRY(UC_JOINING_GROUP_ALEF,                     "ALEF")
      TRY(UC_JOINING_GROUP_BEH,                      "BEH")
      TRY(UC_JOINING_GROUP_BETH,                     "BETH")
      TRY(UC_JOINING_GROUP_BURUSHASKI_YEH_BARREE,    "BURUSHASKI YEH BARREE")
      TRY(UC_JOINING_GROUP_DAL,                      "DAL")
      TRY(UC_JOINING_GROUP_DALATH_RISH,              "DALATH RISH")
      TRY(UC_JOINING_GROUP_E,                        "E")
      TRY(UC_JOINING_GROUP_FARSI_YEH,                "FARSI YEH")
      TRY(UC_JOINING_GROUP_FE,                       "FE")
      TRY(UC_JOINING_GROUP_FEH,                      "FEH")
      TRY(UC_JOINING_GROUP_FINAL_SEMKATH,            "FINAL SEMKATH")
      TRY(UC_JOINING_GROUP_GAF,                      "GAF")
      TRY(UC_JOINING_GROUP_GAMAL,                    "GAMAL")
      TRY(UC_JOINING_GROUP_HAH,                      "HAH")
      TRY(UC_JOINING_GROUP_HE,                       "HE")
      TRY(UC_JOINING_GROUP_HEH,                      "HEH")
      TRY(UC_JOINING_GROUP_HEH_GOAL,                 "HEH GOAL")
      TRY(UC_JOINING_GROUP_HETH,                     "HETH")
      TRY(UC_JOINING_GROUP_KAF,                      "KAF")
      TRY(UC_JOINING_GROUP_KAPH,                     "KAPH")
      TRY(UC_JOINING_GROUP_KHAPH,                    "KHAPH")
      TRY(UC_JOINING_GROUP_KNOTTED_HEH,              "KNOTTED HEH")
      TRY(UC_JOINING_GROUP_LAM,                      "LAM")
      TRY(UC_JOINING_GROUP_LAMADH,                   "LAMADH")
      TRY(UC_JOINING_GROUP_MEEM,                     "MEEM")
      TRY(UC_JOINING_GROUP_MIM,                      "MIM")
      TRY(UC_JOINING_GROUP_NOON,                     "NOON")
      TRY(UC_JOINING_GROUP_NUN,                      "NUN")
      TRY(UC_JOINING_GROUP_NYA,                      "NYA")
      TRY(UC_JOINING_GROUP_PE,                       "PE")
      TRY(UC_JOINING_GROUP_QAF,                      "QAF")
      TRY(UC_JOINING_GROUP_QAPH,                     "QAPH")
      TRY(UC_JOINING_GROUP_REH,                      "REH")
      TRY(UC_JOINING_GROUP_REVERSED_PE,              "REVERSED PE")
      TRY(UC_JOINING_GROUP_SAD,                      "SAD")
      TRY(UC_JOINING_GROUP_SADHE,                    "SADHE")
      TRY(UC_JOINING_GROUP_SEEN,                     "SEEN")
      TRY(UC_JOINING_GROUP_SEMKATH,                  "SEMKATH")
      TRY(UC_JOINING_GROUP_SHIN,                     "SHIN")
      TRY(UC_JOINING_GROUP_SWASH_KAF,                "SWASH KAF")
      TRY(UC_JOINING_GROUP_SYRIAC_WAW,               "SYRIAC WAW")
      TRY(UC_JOINING_GROUP_TAH,                      "TAH")
      TRY(UC_JOINING_GROUP_TAW,                      "TAW")
      TRY(UC_JOINING_GROUP_TEH_MARBUTA,              "TEH MARBUTA")
      TRY(UC_JOINING_GROUP_TEH_MARBUTA_GOAL,         "TEH MARBUTA GOAL")
      TRY(UC_JOINING_GROUP_TETH,                     "TETH")
      TRY(UC_JOINING_GROUP_WAW,                      "WAW")
      TRY(UC_JOINING_GROUP_YEH,                      "YEH")
      TRY(UC_JOINING_GROUP_YEH_BARREE,               "YEH BARREE")
      TRY(UC_JOINING_GROUP_YEH_WITH_TAIL,            "YEH WITH TAIL")
      TRY(UC_JOINING_GROUP_YUDH,                     "YUDH")
      TRY(UC_JOINING_GROUP_YUDH_HE,                  "YUDH HE")
      TRY(UC_JOINING_GROUP_ZAIN,                     "ZAIN")
      TRY(UC_JOINING_GROUP_ZHAIN,                    "ZHAIN")
      TRY(UC_JOINING_GROUP_ROHINGYA_YEH,             "ROHINGYA YEH")
      TRY(UC_JOINING_GROUP_STRAIGHT_WAW,             "STRAIGHT WAW")
      TRY(UC_JOINING_GROUP_MANICHAEAN_ALEPH,         "MANICHAEAN ALEPH")
      TRY(UC_JOINING_GROUP_MANICHAEAN_BETH,          "MANICHAEAN BETH")
      TRY(UC_JOINING_GROUP_MANICHAEAN_GIMEL,         "MANICHAEAN GIMEL")
      TRY(UC_JOINING_GROUP_MANICHAEAN_DALETH,        "MANICHAEAN DALETH")
      TRY(UC_JOINING_GROUP_MANICHAEAN_WAW,           "MANICHAEAN WAW")
      TRY(UC_JOINING_GROUP_MANICHAEAN_ZAYIN,         "MANICHAEAN ZAYIN")
      TRY(UC_JOINING_GROUP_MANICHAEAN_HETH,          "MANICHAEAN HETH")
      TRY(UC_JOINING_GROUP_MANICHAEAN_TETH,          "MANICHAEAN TETH")
      TRY(UC_JOINING_GROUP_MANICHAEAN_YODH,          "MANICHAEAN YODH")
      TRY(UC_JOINING_GROUP_MANICHAEAN_KAPH,          "MANICHAEAN KAPH")
      TRY(UC_JOINING_GROUP_MANICHAEAN_LAMEDH,        "MANICHAEAN LAMEDH")
      TRY(UC_JOINING_GROUP_MANICHAEAN_DHAMEDH,       "MANICHAEAN DHAMEDH")
      TRY(UC_JOINING_GROUP_MANICHAEAN_THAMEDH,       "MANICHAEAN THAMEDH")
      TRY(UC_JOINING_GROUP_MANICHAEAN_MEM,           "MANICHAEAN MEM")
      TRY(UC_JOINING_GROUP_MANICHAEAN_NUN,           "MANICHAEAN NUN")
      TRY(UC_JOINING_GROUP_MANICHAEAN_SAMEKH,        "MANICHAEAN SAMEKH")
      TRY(UC_JOINING_GROUP_MANICHAEAN_AYIN,          "MANICHAEAN AYIN")
      TRY(UC_JOINING_GROUP_MANICHAEAN_PE,            "MANICHAEAN PE")
      TRY(UC_JOINING_GROUP_MANICHAEAN_SADHE,         "MANICHAEAN SADHE")
      TRY(UC_JOINING_GROUP_MANICHAEAN_QOPH,          "MANICHAEAN QOPH")
      TRY(UC_JOINING_GROUP_MANICHAEAN_RESH,          "MANICHAEAN RESH")
      TRY(UC_JOINING_GROUP_MANICHAEAN_TAW,           "MANICHAEAN TAW")
      TRY(UC_JOINING_GROUP_MANICHAEAN_ONE,           "MANICHAEAN ONE")
      TRY(UC_JOINING_GROUP_MANICHAEAN_FIVE,          "MANICHAEAN FIVE")
      TRY(UC_JOINING_GROUP_MANICHAEAN_TEN,           "MANICHAEAN TEN")
      TRY(UC_JOINING_GROUP_MANICHAEAN_TWENTY,        "MANICHAEAN TWENTY")
      TRY(UC_JOINING_GROUP_MANICHAEAN_HUNDRED,       "MANICHAEAN HUNDRED")
      TRY(UC_JOINING_GROUP_AFRICAN_FEH,              "AFRICAN FEH")
      TRY(UC_JOINING_GROUP_AFRICAN_QAF,              "AFRICAN QAF")
      TRY(UC_JOINING_GROUP_AFRICAN_NOON,             "AFRICAN NOON")
      TRY(UC_JOINING_GROUP_MALAYALAM_NGA,            "MALAYALAM NGA")
      TRY(UC_JOINING_GROUP_MALAYALAM_JA,             "MALAYALAM JA")
      TRY(UC_JOINING_GROUP_MALAYALAM_NYA,            "MALAYALAM NYA")
      TRY(UC_JOINING_GROUP_MALAYALAM_TTA,            "MALAYALAM TTA")
      TRY(UC_JOINING_GROUP_MALAYALAM_NNA,            "MALAYALAM NNA")
      TRY(UC_JOINING_GROUP_MALAYALAM_NNNA,           "MALAYALAM NNNA")
      TRY(UC_JOINING_GROUP_MALAYALAM_BHA,            "MALAYALAM BHA")
      TRY(UC_JOINING_GROUP_MALAYALAM_RA,             "MALAYALAM RA")
      TRY(UC_JOINING_GROUP_MALAYALAM_LLA,            "MALAYALAM LLA")
      TRY(UC_JOINING_GROUP_MALAYALAM_LLLA,           "MALAYALAM LLLA")
      TRY(UC_JOINING_GROUP_MALAYALAM_SSA,            "MALAYALAM SSA")
      TRY(UC_JOINING_GROUP_HANIFI_ROHINGYA_PA,       "HANIFI ROHINGYA PA")
      TRY(UC_JOINING_GROUP_HANIFI_ROHINGYA_KINNA_YA, "HANIFI ROHINGYA KINNA YA")
      TRY(UC_JOINING_GROUP_THIN_YEH,                 "THIN YEH")
      TRY(UC_JOINING_GROUP_VERTICAL_TAIL,            "VERTICAL TAIL")
#undef TRY
      else
        {
          fprintf (stderr, "unknown joining group value \"%s\" in '%s':%d\n",
                   joining_group_name, arabicshaping_filename, lineno);
          exit (1);
        }

      unicode_joining_type[i] = joining_type;
      unicode_joining_group[i] = joining_group;
    }

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error reading from '%s'\n", arabicshaping_filename);
      exit (1);
    }
}

/* Convert a Joining_Type value to a C identifier.  */
static const char *
joining_type_as_c_identifier (int joining_type)
{
#define TRY(value) if (joining_type == value) return #value;
  TRY(UC_JOINING_TYPE_U)
  TRY(UC_JOINING_TYPE_T)
  TRY(UC_JOINING_TYPE_C)
  TRY(UC_JOINING_TYPE_L)
  TRY(UC_JOINING_TYPE_R)
  TRY(UC_JOINING_TYPE_D)
#undef TRY
  abort ();
}

static void
output_joining_type_test (const char *filename, const char *version)
{
  FILE *stream;
  bool need_comma;
  unsigned int ch;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Arabic joining type of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_tests_license (stream);
  fprintf (stream, "\n");

  need_comma = false;
  for (ch = 0; ch < 0x110000; ch++)
    {
      int value = unicode_joining_type[ch];

      if (value != (uint8_t)~(uint8_t)0)
        {
          if (need_comma)
            fprintf (stream, ",\n");
          fprintf (stream, "    { 0x%04X, %s }", ch, joining_type_as_c_identifier (value));
          need_comma = true;
        }
    }
  if (need_comma)
    fprintf (stream, "\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Construction of sparse 3-level tables.  */
#define TABLE joining_type_table
#define ELEMENT uint8_t
#define DEFAULT (uint8_t)~(uint8_t)0
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

static void
output_joining_type (const char *filename, const char *version)
{
  FILE *stream;
  unsigned int ch, i;
  struct joining_type_table t;
  unsigned int level1_offset, level2_offset, level3_offset;
  uint8_t *level3_packed;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Arabic joining type of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, true);
  fprintf (stream, "\n");

  t.p = 7;
  t.q = 9;
  joining_type_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      uint8_t value = unicode_joining_type[ch];

      assert (value == (uint8_t)~(uint8_t)0 || value <= 0x0f);

      joining_type_table_add (&t, ch, value);
    }

  joining_type_table_finalize (&t);

  /* Offsets in t.result, in memory of this process.  */
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
    fprintf (stream, "#define joining_type_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream, "static const\n");
  fprintf (stream, "struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    short level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned char level3[%zu * %d];\n", t.level3_size,
           (1 << t.p) * 4 / 8);
  fprintf (stream, "  }\n");
  fprintf (stream, "u_joining_type =\n");
  fprintf (stream, "{\n");
  fprintf (stream, "  {");
  if (t.level1_size > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level3_offset) / sizeof (uint8_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  /* Pack the level3 array.  Each entry needs 4 bits only.  */
  level3_packed =
    (uint8_t *) calloc ((t.level3_size << t.p) * 4 / 8, sizeof (uint8_t));
  for (i = 0; i < t.level3_size << t.p; i++)
    {
      unsigned int j = (i * 4) / 8;
      unsigned int k = (i * 4) % 8;
      uint32_t value = ((unsigned char *) (t.result + level3_offset))[i] & 0x0f;
      level3_packed[j] |= (value << k);
    }
  fprintf (stream, "  {");
  if ((t.level3_size << t.p) * 4 / 8 > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < (t.level3_size << t.p) * 4 / 8; i++)
    {
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " 0x%02x", level3_packed[i]);
      if (i+1 < (t.level3_size << t.p) * 4 / 8)
        fprintf (stream, ",");
    }
  if ((t.level3_size << t.p) * 4 / 8 > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  free (level3_packed);
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Convert a Joining_Group value to a C identifier.  */
static const char *
joining_group_as_c_identifier (int joining_group)
{
#define TRY(value) if (joining_group == value) return #value;
  TRY(UC_JOINING_GROUP_NONE)
  TRY(UC_JOINING_GROUP_AIN)
  TRY(UC_JOINING_GROUP_ALAPH)
  TRY(UC_JOINING_GROUP_ALEF)
  TRY(UC_JOINING_GROUP_BEH)
  TRY(UC_JOINING_GROUP_BETH)
  TRY(UC_JOINING_GROUP_BURUSHASKI_YEH_BARREE)
  TRY(UC_JOINING_GROUP_DAL)
  TRY(UC_JOINING_GROUP_DALATH_RISH)
  TRY(UC_JOINING_GROUP_E)
  TRY(UC_JOINING_GROUP_FARSI_YEH)
  TRY(UC_JOINING_GROUP_FE)
  TRY(UC_JOINING_GROUP_FEH)
  TRY(UC_JOINING_GROUP_FINAL_SEMKATH)
  TRY(UC_JOINING_GROUP_GAF)
  TRY(UC_JOINING_GROUP_GAMAL)
  TRY(UC_JOINING_GROUP_HAH)
  TRY(UC_JOINING_GROUP_HE)
  TRY(UC_JOINING_GROUP_HEH)
  TRY(UC_JOINING_GROUP_HEH_GOAL)
  TRY(UC_JOINING_GROUP_HETH)
  TRY(UC_JOINING_GROUP_KAF)
  TRY(UC_JOINING_GROUP_KAPH)
  TRY(UC_JOINING_GROUP_KHAPH)
  TRY(UC_JOINING_GROUP_KNOTTED_HEH)
  TRY(UC_JOINING_GROUP_LAM)
  TRY(UC_JOINING_GROUP_LAMADH)
  TRY(UC_JOINING_GROUP_MEEM)
  TRY(UC_JOINING_GROUP_MIM)
  TRY(UC_JOINING_GROUP_NOON)
  TRY(UC_JOINING_GROUP_NUN)
  TRY(UC_JOINING_GROUP_NYA)
  TRY(UC_JOINING_GROUP_PE)
  TRY(UC_JOINING_GROUP_QAF)
  TRY(UC_JOINING_GROUP_QAPH)
  TRY(UC_JOINING_GROUP_REH)
  TRY(UC_JOINING_GROUP_REVERSED_PE)
  TRY(UC_JOINING_GROUP_SAD)
  TRY(UC_JOINING_GROUP_SADHE)
  TRY(UC_JOINING_GROUP_SEEN)
  TRY(UC_JOINING_GROUP_SEMKATH)
  TRY(UC_JOINING_GROUP_SHIN)
  TRY(UC_JOINING_GROUP_SWASH_KAF)
  TRY(UC_JOINING_GROUP_SYRIAC_WAW)
  TRY(UC_JOINING_GROUP_TAH)
  TRY(UC_JOINING_GROUP_TAW)
  TRY(UC_JOINING_GROUP_TEH_MARBUTA)
  TRY(UC_JOINING_GROUP_TEH_MARBUTA_GOAL)
  TRY(UC_JOINING_GROUP_TETH)
  TRY(UC_JOINING_GROUP_WAW)
  TRY(UC_JOINING_GROUP_YEH)
  TRY(UC_JOINING_GROUP_YEH_BARREE)
  TRY(UC_JOINING_GROUP_YEH_WITH_TAIL)
  TRY(UC_JOINING_GROUP_YUDH)
  TRY(UC_JOINING_GROUP_YUDH_HE)
  TRY(UC_JOINING_GROUP_ZAIN)
  TRY(UC_JOINING_GROUP_ZHAIN)
  TRY(UC_JOINING_GROUP_ROHINGYA_YEH)
  TRY(UC_JOINING_GROUP_STRAIGHT_WAW)
  TRY(UC_JOINING_GROUP_MANICHAEAN_ALEPH)
  TRY(UC_JOINING_GROUP_MANICHAEAN_BETH)
  TRY(UC_JOINING_GROUP_MANICHAEAN_GIMEL)
  TRY(UC_JOINING_GROUP_MANICHAEAN_DALETH)
  TRY(UC_JOINING_GROUP_MANICHAEAN_WAW)
  TRY(UC_JOINING_GROUP_MANICHAEAN_ZAYIN)
  TRY(UC_JOINING_GROUP_MANICHAEAN_HETH)
  TRY(UC_JOINING_GROUP_MANICHAEAN_TETH)
  TRY(UC_JOINING_GROUP_MANICHAEAN_YODH)
  TRY(UC_JOINING_GROUP_MANICHAEAN_KAPH)
  TRY(UC_JOINING_GROUP_MANICHAEAN_LAMEDH)
  TRY(UC_JOINING_GROUP_MANICHAEAN_DHAMEDH)
  TRY(UC_JOINING_GROUP_MANICHAEAN_THAMEDH)
  TRY(UC_JOINING_GROUP_MANICHAEAN_MEM)
  TRY(UC_JOINING_GROUP_MANICHAEAN_NUN)
  TRY(UC_JOINING_GROUP_MANICHAEAN_SAMEKH)
  TRY(UC_JOINING_GROUP_MANICHAEAN_AYIN)
  TRY(UC_JOINING_GROUP_MANICHAEAN_PE)
  TRY(UC_JOINING_GROUP_MANICHAEAN_SADHE)
  TRY(UC_JOINING_GROUP_MANICHAEAN_QOPH)
  TRY(UC_JOINING_GROUP_MANICHAEAN_RESH)
  TRY(UC_JOINING_GROUP_MANICHAEAN_TAW)
  TRY(UC_JOINING_GROUP_MANICHAEAN_ONE)
  TRY(UC_JOINING_GROUP_MANICHAEAN_FIVE)
  TRY(UC_JOINING_GROUP_MANICHAEAN_TEN)
  TRY(UC_JOINING_GROUP_MANICHAEAN_TWENTY)
  TRY(UC_JOINING_GROUP_MANICHAEAN_HUNDRED)
  TRY(UC_JOINING_GROUP_AFRICAN_FEH)
  TRY(UC_JOINING_GROUP_AFRICAN_QAF)
  TRY(UC_JOINING_GROUP_AFRICAN_NOON)
  TRY(UC_JOINING_GROUP_MALAYALAM_NGA)
  TRY(UC_JOINING_GROUP_MALAYALAM_JA)
  TRY(UC_JOINING_GROUP_MALAYALAM_NYA)
  TRY(UC_JOINING_GROUP_MALAYALAM_TTA)
  TRY(UC_JOINING_GROUP_MALAYALAM_NNA)
  TRY(UC_JOINING_GROUP_MALAYALAM_NNNA)
  TRY(UC_JOINING_GROUP_MALAYALAM_BHA)
  TRY(UC_JOINING_GROUP_MALAYALAM_RA)
  TRY(UC_JOINING_GROUP_MALAYALAM_LLA)
  TRY(UC_JOINING_GROUP_MALAYALAM_LLLA)
  TRY(UC_JOINING_GROUP_MALAYALAM_SSA)
  TRY(UC_JOINING_GROUP_HANIFI_ROHINGYA_PA)
  TRY(UC_JOINING_GROUP_HANIFI_ROHINGYA_KINNA_YA)
  TRY(UC_JOINING_GROUP_THIN_YEH)
  TRY(UC_JOINING_GROUP_VERTICAL_TAIL)
#undef TRY
  abort ();
}

static void
output_joining_group_test (const char *filename, const char *version)
{
  FILE *stream;
  bool need_comma;
  unsigned int ch;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Arabic joining group of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_tests_license (stream);
  fprintf (stream, "\n");

  need_comma = false;
  for (ch = 0; ch < 0x110000; ch++)
    {
      int value = unicode_joining_group[ch];

      if (value != UC_JOINING_GROUP_NONE)
        {
          if (need_comma)
            fprintf (stream, ",\n");
          fprintf (stream, "    { 0x%04X, %s }", ch, joining_group_as_c_identifier (value));
          need_comma = true;
        }
    }
  if (need_comma)
    fprintf (stream, "\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Construction of sparse 3-level tables.  */
#define TABLE joining_group_table
#define ELEMENT uint8_t
#define DEFAULT UC_JOINING_GROUP_NONE
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

static void
output_joining_group (const char *filename, const char *version)
{
  FILE *stream;
  unsigned int ch, i;
  struct joining_group_table t;
  unsigned int level1_offset, level2_offset, level3_offset;
  uint16_t *level3_packed;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Arabic joining group of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, false);
  fprintf (stream, "\n");

  t.p = 7;
  t.q = 9;
  joining_group_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      uint8_t value = unicode_joining_group[ch];

      assert (value <= 0x7f);

      joining_group_table_add (&t, ch, value);
    }

  joining_group_table_finalize (&t);

  /* Offsets in t.result, in memory of this process.  */
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
    fprintf (stream, "#define joining_group_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream, "static const\n");
  fprintf (stream, "struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    short level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned short level3[%zu * %d + 1];\n", t.level3_size,
           (1 << t.p) * 7 / 16);
  fprintf (stream, "  }\n");
  fprintf (stream, "u_joining_group =\n");
  fprintf (stream, "{\n");
  fprintf (stream, "  {");
  if (t.level1_size > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level3_offset) / sizeof (uint8_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  /* Pack the level3 array.  Each entry needs 7 bits only.  Use 16-bit units,
     not 32-bit units, in order to make the lookup function easier.  */
  level3_packed =
    (uint16_t *)
    calloc ((t.level3_size << t.p) * 7 / 16 + 1, sizeof (uint16_t));
  for (i = 0; i < t.level3_size << t.p; i++)
    {
      unsigned int j = (i * 7) / 16;
      unsigned int k = (i * 7) % 16;
      uint32_t value = ((unsigned char *) (t.result + level3_offset))[i];
      value = level3_packed[j] | (level3_packed[j+1] << 16) | (value << k);
      level3_packed[j] = value & 0xffff;
      level3_packed[j+1] = value >> 16;
    }
  fprintf (stream, "  {");
  if ((t.level3_size << t.p) * 7 / 16 + 1 > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < (t.level3_size << t.p) * 7 / 16 + 1; i++)
    {
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " 0x%04x", level3_packed[i]);
      if (i+1 < (t.level3_size << t.p) * 7 / 16 + 1)
        fprintf (stream, ",");
    }
  if ((t.level3_size << t.p) * 7 / 16 + 1 > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  free (level3_packed);
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Scripts.  */

static const char *scripts[256];
static unsigned int numscripts;

static uint8_t unicode_scripts[0x110000];

static void
fill_scripts (const char *scripts_filename)
{
  FILE *stream;
  unsigned int i;

  stream = fopen (scripts_filename, "r");
  if (stream == NULL)
    {
      fprintf (stderr, "error during fopen of '%s'\n", scripts_filename);
      exit (1);
    }

  numscripts = 0;

  for (i = 0; i < 0x110000; i++)
    unicode_scripts[i] = (uint8_t)~(uint8_t)0;

  for (;;)
    {
      char buf[200+1];
      unsigned int i1, i2;
      char padding[200+1];
      char scriptname[200+1];
      int script;

      if (fscanf (stream, "%200[^\n]\n", buf) < 1)
        break;

      if (buf[0] == '\0' || buf[0] == '#')
        continue;

      if (sscanf (buf, "%X..%X%[ ;]%[^ ]", &i1, &i2, padding, scriptname) != 4)
        {
          if (sscanf (buf, "%X%[ ;]%[^ ]", &i1, padding, scriptname) != 3)
            {
              fprintf (stderr, "parse error in '%s'\n", scripts_filename);
              exit (1);
            }
          i2 = i1;
        }
      assert (i2 >= i1);
      assert (i2 < 0x110000);

      for (script = numscripts - 1; script >= 0; script--)
        if (strcmp (scripts[script], scriptname) == 0)
          break;
      if (script < 0)
        {
          scripts[numscripts] = strdup (scriptname);
          script = numscripts;
          numscripts++;
          assert (numscripts != 256);
        }

      for (i = i1; i <= i2; i++)
        {
          if (unicode_scripts[i] != (uint8_t)~(uint8_t)0)
            fprintf (stderr, "0x%04X belongs to multiple scripts\n", i);
          unicode_scripts[i] = script;
        }
    }

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error reading from '%s'\n", scripts_filename);
      exit (1);
    }
}

/* Construction of sparse 3-level tables.  */
#define TABLE script_table
#define ELEMENT uint8_t
#define DEFAULT (uint8_t)~(uint8_t)0
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

static void
output_scripts (const char *version)
{
  const char *filename = "unictype/scripts.h";
  FILE *stream;
  unsigned int ch, s, i;
  struct script_table t;
  unsigned int level1_offset, level2_offset, level3_offset;

  typedef struct
  {
    const char *lowercase_name;
  }
  scriptinfo_t;
  scriptinfo_t scriptinfo[256];

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Unicode scripts.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, true);
  fprintf (stream, "\n");

  for (s = 0; s < numscripts; s++)
    {
      char *lcp = strdup (scripts[s]);
      char *cp;

      for (cp = lcp; *cp != '\0'; cp++)
        if (*cp >= 'A' && *cp <= 'Z')
          *cp += 'a' - 'A';

      scriptinfo[s].lowercase_name = lcp;
    }

  for (s = 0; s < numscripts; s++)
    {
      fprintf (stream, "static const uc_interval_t script_%s_intervals[] =\n",
               scriptinfo[s].lowercase_name);
      fprintf (stream, "{\n");
      i = 0;
      for (ch = 0; ch < 0x110000; ch++)
        if (unicode_scripts[ch] == s)
          {
            unsigned int start;
            unsigned int end;

            start = ch;
            while (ch + 1 < 0x110000 && unicode_scripts[ch + 1] == s)
              ch++;
            end = ch;

            if (i > 0)
              fprintf (stream, ",\n");
            if (start == end)
              fprintf (stream, "  { 0x%04X, 1, 1 }", start);
            else
              fprintf (stream, "  { 0x%04X, 1, 0 }, { 0x%04X, 0, 1 }",
                       start, end);
            i++;
          }
      fprintf (stream, "\n");
      fprintf (stream, "};\n");
    }

  fprintf (stream, "static const uc_script_t scripts[%d] =\n", numscripts);
  fprintf (stream, "{\n");
  for (s = 0; s < numscripts; s++)
    {
      fprintf (stream, "  {\n");
      fprintf (stream, "    sizeof (script_%s_intervals) / sizeof (uc_interval_t),\n",
               scriptinfo[s].lowercase_name);
      fprintf (stream, "    script_%s_intervals,\n",
               scriptinfo[s].lowercase_name);
      fprintf (stream, "    \"%s\"\n", scripts[s]);
      fprintf (stream, "  }");
      if (s+1 < numscripts)
        fprintf (stream, ",");
      fprintf (stream, "\n");
    }
  fprintf (stream, "};\n");

  t.p = 7;
  t.q = 9;
  script_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      unsigned int s = unicode_scripts[ch];
      if (s != (uint8_t)~(uint8_t)0)
        script_table_add (&t, ch, s);
    }

  script_table_finalize (&t);

  /* Offsets in t.result, in memory of this process.  */
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
    fprintf (stream, "#define script_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream, "static const\n");
  fprintf (stream, "struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    short level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned char level3[%zu << %d];\n", t.level3_size, t.p);
  fprintf (stream, "  }\n");
  fprintf (stream, "u_script =\n");
  fprintf (stream, "{\n");
  fprintf (stream, "  {");
  if (t.level1_size > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level3_offset) / sizeof (uint8_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level3_size << t.p > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level3_size << t.p; i++)
    {
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " %3d", ((uint8_t *) (t.result + level3_offset))[i]);
      if (i+1 < t.level3_size << t.p)
        fprintf (stream, ",");
    }
  if (t.level3_size << t.p > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

static void
output_scripts_byname (const char *version)
{
  const char *filename = "unictype/scripts_byname.gperf";
  FILE *stream;
  unsigned int s;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Unicode scripts.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, true);
  fprintf (stream, "\n");

  fprintf (stream, "struct named_script { int name; unsigned int index; };\n");
  fprintf (stream, "%%struct-type\n");
  fprintf (stream, "%%language=ANSI-C\n");
  fprintf (stream, "%%define hash-function-name scripts_hash\n");
  fprintf (stream, "%%define lookup-function-name uc_script_lookup\n");
  fprintf (stream, "%%readonly-tables\n");
  fprintf (stream, "%%global-table\n");
  fprintf (stream, "%%define word-array-name script_names\n");
  fprintf (stream, "%%pic\n");
  fprintf (stream, "%%define string-pool-name script_stringpool\n");
  fprintf (stream, "%%%%\n");
  for (s = 0; s < numscripts; s++)
    fprintf (stream, "%s, %u\n", scripts[s], s);

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Blocks.  */

typedef struct { unsigned int start; unsigned int end; const char *name; }
  block_t;
static block_t blocks[384];
static unsigned int numblocks;

static void
fill_blocks (const char *blocks_filename)
{
  FILE *stream;

  stream = fopen (blocks_filename, "r");
  if (stream == NULL)
    {
      fprintf (stderr, "error during fopen of '%s'\n", blocks_filename);
      exit (1);
    }

  for (;;)
    {
      char buf[200+1];
      unsigned int i1, i2;
      char padding[200+1];
      char blockname[200+1];

      if (fscanf (stream, "%200[^\n]\n", buf) < 1)
        break;

      if (buf[0] == '\0' || buf[0] == '#')
        continue;

      if (sscanf (buf, "%X..%X%[ ;]%[^\r]", &i1, &i2, padding, blockname) != 4)
        {
          fprintf (stderr, "parse error in '%s'\n", blocks_filename);
          exit (1);
        }
      blocks[numblocks].start = i1;
      blocks[numblocks].end = i2;
      blocks[numblocks].name = strdup (blockname);
      /* It must be sorted.  */
      assert (numblocks == 0 || blocks[numblocks-1].end < blocks[numblocks].start);
      numblocks++;
      assert (numblocks != SIZEOF (blocks));
    }

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error reading from '%s'\n", blocks_filename);
      exit (1);
    }
}

/* Return the smallest block index among the blocks for characters >= ch.  */
static unsigned int
block_first_index (unsigned int ch)
{
  /* Binary search.  */
  unsigned int lo = 0;
  unsigned int hi = numblocks;
  /* Invariants:
     All blocks[i], i < lo, have blocks[i].end < ch,
     all blocks[i], i >= hi, have blocks[i].end >= ch.  */
  while (lo < hi)
    {
      unsigned int mid = (lo + hi) / 2; /* >= lo, < hi */
      if (blocks[mid].end < ch)
        lo = mid + 1;
      else
        hi = mid;
    }
  return hi;
}

/* Return the largest block index among the blocks for characters <= ch,
   plus 1.  */
static unsigned int
block_last_index (unsigned int ch)
{
  /* Binary search.  */
  unsigned int lo = 0;
  unsigned int hi = numblocks;
  /* Invariants:
     All blocks[i], i < lo, have blocks[i].start <= ch,
     all blocks[i], i >= hi, have blocks[i].start > ch.  */
  while (lo < hi)
    {
      unsigned int mid = (lo + hi) / 2; /* >= lo, < hi */
      if (blocks[mid].start <= ch)
        lo = mid + 1;
      else
        hi = mid;
    }
  return hi;
}

static void
output_blocks (const char *version)
{
  const char *filename = "unictype/blocks.h";
  const unsigned int shift = 8; /* bits to shift away for array access */
  const unsigned int threshold = 0x28000; /* cut-off table here to save space */
  FILE *stream;
  unsigned int i;
  unsigned int i1;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Unicode blocks.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, false);
  fprintf (stream, "\n");

  fprintf (stream, "static const uc_block_t blocks[] =\n");
  fprintf (stream, "{\n");
  for (i = 0; i < numblocks; i++)
    {
      fprintf (stream, "  { 0x%04X, 0x%04X, \"%s\" }", blocks[i].start,
               blocks[i].end, blocks[i].name);
      if (i+1 < numblocks)
        fprintf (stream, ",");
      fprintf (stream, "\n");
    }
  fprintf (stream, "};\n");
  fprintf (stream, "#define blocks_level1_shift %d\n", shift);
  fprintf (stream, "#define blocks_level1_threshold 0x%04X\n", threshold);
  fprintf (stream, "static const uint16_t blocks_level1[%d * 2] =\n",
           threshold >> shift);
  fprintf (stream, "{\n");
  for (i1 = 0; i1 < (threshold >> shift); i1++)
    {
      unsigned int first_index = block_first_index (i1 << shift);
      unsigned int last_index = block_last_index (((i1 + 1) << shift) - 1);
      fprintf (stream, "  %3d, %3d", first_index, last_index);
      if (i1+1 < (threshold >> shift))
        fprintf (stream, ",");
      fprintf (stream, "\n");
    }
  fprintf (stream, "};\n");
  fprintf (stream, "#define blocks_upper_first_index %d\n",
           block_first_index (threshold));
  fprintf (stream, "#define blocks_upper_last_index %d\n",
           block_last_index (0x10FFFF));

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* C and Java syntax.  */

enum
{
  UC_IDENTIFIER_START,    /* valid as first or subsequent character */
  UC_IDENTIFIER_VALID,    /* valid as subsequent character only */
  UC_IDENTIFIER_INVALID,  /* not valid */
  UC_IDENTIFIER_IGNORABLE /* ignorable (Java only) */
};

/* ISO C 99 section 6.4.(3).  */
static bool
is_c_whitespace (unsigned int ch)
{
  return (ch == ' ' /* space */
          || ch == '\t' /* horizontal tab */
          || ch == '\n' || ch == '\r' /* new-line */
          || ch == '\v' /* vertical tab */
          || ch == '\f'); /* form-feed */
}

/* ISO C 99 section 6.4.2.1 and appendix D.  */
static int
c_ident_category (unsigned int ch)
{
  /* Section 6.4.2.1.  */
  if (ch >= '0' && ch <= '9')
    return UC_IDENTIFIER_VALID;
  if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_')
    return UC_IDENTIFIER_START;
  /* Appendix D.  */
  if (0
      /* Latin */
      || (ch == 0x00AA)
      || (ch == 0x00BA)
      || (ch >= 0x00C0 && ch <= 0x00D6)
      || (ch >= 0x00D8 && ch <= 0x00F6)
      || (ch >= 0x00F8 && ch <= 0x01F5)
      || (ch >= 0x01FA && ch <= 0x0217)
      || (ch >= 0x0250 && ch <= 0x02A8)
      || (ch >= 0x1E00 && ch <= 0x1E9B)
      || (ch >= 0x1EA0 && ch <= 0x1EF9)
      || (ch == 0x207F)
      /* Greek */
      || (ch == 0x0386)
      || (ch >= 0x0388 && ch <= 0x038A)
      || (ch == 0x038C)
      || (ch >= 0x038E && ch <= 0x03A1)
      || (ch >= 0x03A3 && ch <= 0x03CE)
      || (ch >= 0x03D0 && ch <= 0x03D6)
      || (ch == 0x03DA)
      || (ch == 0x03DC)
      || (ch == 0x03DE)
      || (ch == 0x03E0)
      || (ch >= 0x03E2 && ch <= 0x03F3)
      || (ch >= 0x1F00 && ch <= 0x1F15)
      || (ch >= 0x1F18 && ch <= 0x1F1D)
      || (ch >= 0x1F20 && ch <= 0x1F45)
      || (ch >= 0x1F48 && ch <= 0x1F4D)
      || (ch >= 0x1F50 && ch <= 0x1F57)
      || (ch == 0x1F59)
      || (ch == 0x1F5B)
      || (ch == 0x1F5D)
      || (ch >= 0x1F5F && ch <= 0x1F7D)
      || (ch >= 0x1F80 && ch <= 0x1FB4)
      || (ch >= 0x1FB6 && ch <= 0x1FBC)
      || (ch >= 0x1FC2 && ch <= 0x1FC4)
      || (ch >= 0x1FC6 && ch <= 0x1FCC)
      || (ch >= 0x1FD0 && ch <= 0x1FD3)
      || (ch >= 0x1FD6 && ch <= 0x1FDB)
      || (ch >= 0x1FE0 && ch <= 0x1FEC)
      || (ch >= 0x1FF2 && ch <= 0x1FF4)
      || (ch >= 0x1FF6 && ch <= 0x1FFC)
      /* Cyrillic */
      || (ch >= 0x0401 && ch <= 0x040C)
      || (ch >= 0x040E && ch <= 0x044F)
      || (ch >= 0x0451 && ch <= 0x045C)
      || (ch >= 0x045E && ch <= 0x0481)
      || (ch >= 0x0490 && ch <= 0x04C4)
      || (ch >= 0x04C7 && ch <= 0x04C8)
      || (ch >= 0x04CB && ch <= 0x04CC)
      || (ch >= 0x04D0 && ch <= 0x04EB)
      || (ch >= 0x04EE && ch <= 0x04F5)
      || (ch >= 0x04F8 && ch <= 0x04F9)
      /* Armenian */
      || (ch >= 0x0531 && ch <= 0x0556)
      || (ch >= 0x0561 && ch <= 0x0587)
      /* Hebrew */
      || (ch >= 0x05B0 && ch <= 0x05B9)
      || (ch >= 0x05BB && ch <= 0x05BD)
      || (ch == 0x05BF)
      || (ch >= 0x05C1 && ch <= 0x05C2)
      || (ch >= 0x05D0 && ch <= 0x05EA)
      || (ch >= 0x05F0 && ch <= 0x05F2)
      /* Arabic */
      || (ch >= 0x0621 && ch <= 0x063A)
      || (ch >= 0x0640 && ch <= 0x0652)
      || (ch >= 0x0670 && ch <= 0x06B7)
      || (ch >= 0x06BA && ch <= 0x06BE)
      || (ch >= 0x06C0 && ch <= 0x06CE)
      || (ch >= 0x06D0 && ch <= 0x06DC)
      || (ch >= 0x06E5 && ch <= 0x06E8)
      || (ch >= 0x06EA && ch <= 0x06ED)
      /* Devanagari */
      || (ch >= 0x0901 && ch <= 0x0903)
      || (ch >= 0x0905 && ch <= 0x0939)
      || (ch >= 0x093E && ch <= 0x094D)
      || (ch >= 0x0950 && ch <= 0x0952)
      || (ch >= 0x0958 && ch <= 0x0963)
      /* Bengali */
      || (ch >= 0x0981 && ch <= 0x0983)
      || (ch >= 0x0985 && ch <= 0x098C)
      || (ch >= 0x098F && ch <= 0x0990)
      || (ch >= 0x0993 && ch <= 0x09A8)
      || (ch >= 0x09AA && ch <= 0x09B0)
      || (ch == 0x09B2)
      || (ch >= 0x09B6 && ch <= 0x09B9)
      || (ch >= 0x09BE && ch <= 0x09C4)
      || (ch >= 0x09C7 && ch <= 0x09C8)
      || (ch >= 0x09CB && ch <= 0x09CD)
      || (ch >= 0x09DC && ch <= 0x09DD)
      || (ch >= 0x09DF && ch <= 0x09E3)
      || (ch >= 0x09F0 && ch <= 0x09F1)
      /* Gurmukhi */
      || (ch == 0x0A02)
      || (ch >= 0x0A05 && ch <= 0x0A0A)
      || (ch >= 0x0A0F && ch <= 0x0A10)
      || (ch >= 0x0A13 && ch <= 0x0A28)
      || (ch >= 0x0A2A && ch <= 0x0A30)
      || (ch >= 0x0A32 && ch <= 0x0A33)
      || (ch >= 0x0A35 && ch <= 0x0A36)
      || (ch >= 0x0A38 && ch <= 0x0A39)
      || (ch >= 0x0A3E && ch <= 0x0A42)
      || (ch >= 0x0A47 && ch <= 0x0A48)
      || (ch >= 0x0A4B && ch <= 0x0A4D)
      || (ch >= 0x0A59 && ch <= 0x0A5C)
      || (ch == 0x0A5E)
      || (ch == 0x0A74)
      /* Gujarati */
      || (ch >= 0x0A81 && ch <= 0x0A83)
      || (ch >= 0x0A85 && ch <= 0x0A8B)
      || (ch == 0x0A8D)
      || (ch >= 0x0A8F && ch <= 0x0A91)
      || (ch >= 0x0A93 && ch <= 0x0AA8)
      || (ch >= 0x0AAA && ch <= 0x0AB0)
      || (ch >= 0x0AB2 && ch <= 0x0AB3)
      || (ch >= 0x0AB5 && ch <= 0x0AB9)
      || (ch >= 0x0ABD && ch <= 0x0AC5)
      || (ch >= 0x0AC7 && ch <= 0x0AC9)
      || (ch >= 0x0ACB && ch <= 0x0ACD)
      || (ch == 0x0AD0)
      || (ch == 0x0AE0)
      /* Oriya */
      || (ch >= 0x0B01 && ch <= 0x0B03)
      || (ch >= 0x0B05 && ch <= 0x0B0C)
      || (ch >= 0x0B0F && ch <= 0x0B10)
      || (ch >= 0x0B13 && ch <= 0x0B28)
      || (ch >= 0x0B2A && ch <= 0x0B30)
      || (ch >= 0x0B32 && ch <= 0x0B33)
      || (ch >= 0x0B36 && ch <= 0x0B39)
      || (ch >= 0x0B3E && ch <= 0x0B43)
      || (ch >= 0x0B47 && ch <= 0x0B48)
      || (ch >= 0x0B4B && ch <= 0x0B4D)
      || (ch >= 0x0B5C && ch <= 0x0B5D)
      || (ch >= 0x0B5F && ch <= 0x0B61)
      /* Tamil */
      || (ch >= 0x0B82 && ch <= 0x0B83)
      || (ch >= 0x0B85 && ch <= 0x0B8A)
      || (ch >= 0x0B8E && ch <= 0x0B90)
      || (ch >= 0x0B92 && ch <= 0x0B95)
      || (ch >= 0x0B99 && ch <= 0x0B9A)
      || (ch == 0x0B9C)
      || (ch >= 0x0B9E && ch <= 0x0B9F)
      || (ch >= 0x0BA3 && ch <= 0x0BA4)
      || (ch >= 0x0BA8 && ch <= 0x0BAA)
      || (ch >= 0x0BAE && ch <= 0x0BB5)
      || (ch >= 0x0BB7 && ch <= 0x0BB9)
      || (ch >= 0x0BBE && ch <= 0x0BC2)
      || (ch >= 0x0BC6 && ch <= 0x0BC8)
      || (ch >= 0x0BCA && ch <= 0x0BCD)
      /* Telugu */
      || (ch >= 0x0C01 && ch <= 0x0C03)
      || (ch >= 0x0C05 && ch <= 0x0C0C)
      || (ch >= 0x0C0E && ch <= 0x0C10)
      || (ch >= 0x0C12 && ch <= 0x0C28)
      || (ch >= 0x0C2A && ch <= 0x0C33)
      || (ch >= 0x0C35 && ch <= 0x0C39)
      || (ch >= 0x0C3E && ch <= 0x0C44)
      || (ch >= 0x0C46 && ch <= 0x0C48)
      || (ch >= 0x0C4A && ch <= 0x0C4D)
      || (ch >= 0x0C60 && ch <= 0x0C61)
      /* Kannada */
      || (ch >= 0x0C82 && ch <= 0x0C83)
      || (ch >= 0x0C85 && ch <= 0x0C8C)
      || (ch >= 0x0C8E && ch <= 0x0C90)
      || (ch >= 0x0C92 && ch <= 0x0CA8)
      || (ch >= 0x0CAA && ch <= 0x0CB3)
      || (ch >= 0x0CB5 && ch <= 0x0CB9)
      || (ch >= 0x0CBE && ch <= 0x0CC4)
      || (ch >= 0x0CC6 && ch <= 0x0CC8)
      || (ch >= 0x0CCA && ch <= 0x0CCD)
      || (ch == 0x0CDE)
      || (ch >= 0x0CE0 && ch <= 0x0CE1)
      /* Malayalam */
      || (ch >= 0x0D02 && ch <= 0x0D03)
      || (ch >= 0x0D05 && ch <= 0x0D0C)
      || (ch >= 0x0D0E && ch <= 0x0D10)
      || (ch >= 0x0D12 && ch <= 0x0D28)
      || (ch >= 0x0D2A && ch <= 0x0D39)
      || (ch >= 0x0D3E && ch <= 0x0D43)
      || (ch >= 0x0D46 && ch <= 0x0D48)
      || (ch >= 0x0D4A && ch <= 0x0D4D)
      || (ch >= 0x0D60 && ch <= 0x0D61)
      /* Thai */
      || (ch >= 0x0E01 && ch <= 0x0E3A)
      || (ch >= 0x0E40 && ch <= 0x0E5B)
      /* Lao */
      || (ch >= 0x0E81 && ch <= 0x0E82)
      || (ch == 0x0E84)
      || (ch >= 0x0E87 && ch <= 0x0E88)
      || (ch == 0x0E8A)
      || (ch == 0x0E8D)
      || (ch >= 0x0E94 && ch <= 0x0E97)
      || (ch >= 0x0E99 && ch <= 0x0E9F)
      || (ch >= 0x0EA1 && ch <= 0x0EA3)
      || (ch == 0x0EA5)
      || (ch == 0x0EA7)
      || (ch >= 0x0EAA && ch <= 0x0EAB)
      || (ch >= 0x0EAD && ch <= 0x0EAE)
      || (ch >= 0x0EB0 && ch <= 0x0EB9)
      || (ch >= 0x0EBB && ch <= 0x0EBD)
      || (ch >= 0x0EC0 && ch <= 0x0EC4)
      || (ch == 0x0EC6)
      || (ch >= 0x0EC8 && ch <= 0x0ECD)
      || (ch >= 0x0EDC && ch <= 0x0EDD)
      /* Tibetan */
      || (ch == 0x0F00)
      || (ch >= 0x0F18 && ch <= 0x0F19)
      || (ch == 0x0F35)
      || (ch == 0x0F37)
      || (ch == 0x0F39)
      || (ch >= 0x0F3E && ch <= 0x0F47)
      || (ch >= 0x0F49 && ch <= 0x0F69)
      || (ch >= 0x0F71 && ch <= 0x0F84)
      || (ch >= 0x0F86 && ch <= 0x0F8B)
      || (ch >= 0x0F90 && ch <= 0x0F95)
      || (ch == 0x0F97)
      || (ch >= 0x0F99 && ch <= 0x0FAD)
      || (ch >= 0x0FB1 && ch <= 0x0FB7)
      || (ch == 0x0FB9)
      /* Georgian */
      || (ch >= 0x10A0 && ch <= 0x10C5)
      || (ch >= 0x10D0 && ch <= 0x10F6)
      /* Hiragana */
      || (ch >= 0x3041 && ch <= 0x3093)
      || (ch >= 0x309B && ch <= 0x309C)
      /* Katakana */
      || (ch >= 0x30A1 && ch <= 0x30F6)
      || (ch >= 0x30FB && ch <= 0x30FC)
      /* Bopomofo */
      || (ch >= 0x3105 && ch <= 0x312C)
      /* CJK Unified Ideographs */
      || (ch >= 0x4E00 && ch <= 0x9FA5)
      /* Hangul */
      || (ch >= 0xAC00 && ch <= 0xD7A3)
      /* Digits */
      || (ch >= 0x0660 && ch <= 0x0669)
      || (ch >= 0x06F0 && ch <= 0x06F9)
      || (ch >= 0x0966 && ch <= 0x096F)
      || (ch >= 0x09E6 && ch <= 0x09EF)
      || (ch >= 0x0A66 && ch <= 0x0A6F)
      || (ch >= 0x0AE6 && ch <= 0x0AEF)
      || (ch >= 0x0B66 && ch <= 0x0B6F)
      || (ch >= 0x0BE7 && ch <= 0x0BEF)
      || (ch >= 0x0C66 && ch <= 0x0C6F)
      || (ch >= 0x0CE6 && ch <= 0x0CEF)
      || (ch >= 0x0D66 && ch <= 0x0D6F)
      || (ch >= 0x0E50 && ch <= 0x0E59)
      || (ch >= 0x0ED0 && ch <= 0x0ED9)
      || (ch >= 0x0F20 && ch <= 0x0F33)
      /* Special characters */
      || (ch == 0x00B5)
      || (ch == 0x00B7)
      || (ch >= 0x02B0 && ch <= 0x02B8)
      || (ch == 0x02BB)
      || (ch >= 0x02BD && ch <= 0x02C1)
      || (ch >= 0x02D0 && ch <= 0x02D1)
      || (ch >= 0x02E0 && ch <= 0x02E4)
      || (ch == 0x037A)
      || (ch == 0x0559)
      || (ch == 0x093D)
      || (ch == 0x0B3D)
      || (ch == 0x1FBE)
      || (ch >= 0x203F && ch <= 0x2040)
      || (ch == 0x2102)
      || (ch == 0x2107)
      || (ch >= 0x210A && ch <= 0x2113)
      || (ch == 0x2115)
      || (ch >= 0x2118 && ch <= 0x211D)
      || (ch == 0x2124)
      || (ch == 0x2126)
      || (ch == 0x2128)
      || (ch >= 0x212A && ch <= 0x2131)
      || (ch >= 0x2133 && ch <= 0x2138)
      || (ch >= 0x2160 && ch <= 0x2182)
      || (ch >= 0x3005 && ch <= 0x3007)
      || (ch >= 0x3021 && ch <= 0x3029)
     )
    return UC_IDENTIFIER_START;
  return UC_IDENTIFIER_INVALID;
}

/* The Java Language Specification, 3rd edition, §3.6.
   https://docs.oracle.com/javase/specs/jls/se6/html/lexical.html#3.6  */
static bool
is_java_whitespace (unsigned int ch)
{
  return (ch == ' ' || ch == '\t' || ch == '\f'
          || ch == '\n' || ch == '\r');
}

/* The Java Language Specification, 3rd edition, §3.8.
   https://docs.oracle.com/javase/specs/jls/se6/html/lexical.html#3.8
   and Character.isJavaIdentifierStart and Character.isJavaIdentifierPart  */
static int
java_ident_category (unsigned int ch)
{
  /* FIXME: Check this against Sun's JDK implementation.  */
  if (is_category_L (ch) /* = Character.isLetter(ch) */
      || is_category_Nl (ch) /* = Character.getType(ch)==LETTER_NUMBER */
      || is_category_Sc (ch) /* currency symbol */
      || is_category_Pc (ch) /* connector punctuation */
     )
    return UC_IDENTIFIER_START;
  if (is_category_Nd (ch) /* digit */
      || is_category_Mc (ch) /* combining mark */
      || is_category_Mn (ch) /* non-spacing mark */
     )
    return UC_IDENTIFIER_VALID;
  if ((ch >= 0x0000 && ch <= 0x0008)
      || (ch >= 0x000E && ch <= 0x001B)
      || (ch >= 0x007F && ch <= 0x009F)
      || is_category_Cf (ch) /* = Character.getType(ch)==FORMAT */
     )
    return UC_IDENTIFIER_IGNORABLE;
  return UC_IDENTIFIER_INVALID;
}

/* Construction of sparse 3-level tables.  */
#define TABLE identsyntax_table
#define ELEMENT uint8_t
#define DEFAULT UC_IDENTIFIER_INVALID
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

/* Output an identifier syntax categorization in a three-level bitmap.  */
static void
output_ident_category (const char *filename, int (*predicate) (unsigned int), const char *name, const char *version)
{
  FILE *stream;
  unsigned int ch, i;
  struct identsyntax_table t;
  unsigned int level1_offset, level2_offset, level3_offset;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Language syntax properties of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, false);
  fprintf (stream, "\n");

  t.p = 7; /* or 8 */
  t.q = 5; /* or 4 */
  identsyntax_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      int syntaxcode = predicate (ch);

      assert (syntaxcode <= 0x03);

      if (syntaxcode != UC_IDENTIFIER_INVALID)
        identsyntax_table_add (&t, ch, syntaxcode);
    }

  identsyntax_table_finalize (&t);

  /* Offsets in t.result, in memory of this process.  */
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
    fprintf (stream, "#define identsyntax_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream, "static const\n");
  fprintf (stream, "struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    short level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned short level3[%zu * %d];\n", t.level3_size,
           (1 << t.p) * 2 / 16);
  fprintf (stream, "  }\n");
  fprintf (stream, "%s =\n", name);
  fprintf (stream, "{\n");
  fprintf (stream, "  {");
  if (t.level1_size > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level3_offset) / sizeof (uint8_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  /* Pack the level3 array.  Each entry needs 2 bits only.  */
  fprintf (stream, "  {");
  if ((t.level3_size << t.p) * 2 / 16 > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < (t.level3_size << t.p) * 2 / 16; i++)
    {
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " 0x%04x",
               (((uint8_t *) (t.result + level3_offset))[8 * i] << 0)
               | (((uint8_t *) (t.result + level3_offset))[8 * i + 1] << 2)
               | (((uint8_t *) (t.result + level3_offset))[8 * i + 2] << 4)
               | (((uint8_t *) (t.result + level3_offset))[8 * i + 3] << 6)
               | (((uint8_t *) (t.result + level3_offset))[8 * i + 4] << 8)
               | (((uint8_t *) (t.result + level3_offset))[8 * i + 5] << 10)
               | (((uint8_t *) (t.result + level3_offset))[8 * i + 6] << 12)
               | (((uint8_t *) (t.result + level3_offset))[8 * i + 7] << 14));
      if (i+1 < (t.level3_size << t.p) * 2 / 16)
        fprintf (stream, ",");
    }
  if ((t.level3_size << t.p) * 2 / 16 > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

static void
output_ident_properties (const char *version)
{
#define PROPERTY(P) \
  debug_output_predicate ("unictype/sy_" #P ".txt", is_ ## P); \
  output_predicate_test ("../tests/unictype/test-sy_" #P ".c", is_ ## P, "uc_is_" #P " (c)"); \
  output_predicate ("unictype/sy_" #P ".h", is_ ## P, "u_" #P, "Language syntax properties", version);
  PROPERTY(c_whitespace)
  PROPERTY(java_whitespace)
#undef PROPERTY

  output_ident_category ("unictype/sy_c_ident.h", c_ident_category, "u_c_ident", version);
  output_ident_category ("unictype/sy_java_ident.h", java_ident_category, "u_java_ident", version);
}

/* ========================================================================= */

/* Like ISO C <ctype.h> and <wctype.h>.  Compatible to glibc's
   glibc/localedata/locales/i18n file, generated by
   glibc/localedata/gen-unicode-ctype.c.  */

/* Character mappings.  */

static unsigned int
to_upper (unsigned int ch)
{
  if (unicode_attributes[ch].name != NULL
      && unicode_attributes[ch].upper != NONE)
    return unicode_attributes[ch].upper;
  else
    return ch;
}

static unsigned int
to_lower (unsigned int ch)
{
  if (unicode_attributes[ch].name != NULL
      && unicode_attributes[ch].lower != NONE)
    return unicode_attributes[ch].lower;
  else
    return ch;
}

static unsigned int
to_title (unsigned int ch)
{
  if (unicode_attributes[ch].name != NULL
      && unicode_attributes[ch].title != NONE)
    return unicode_attributes[ch].title;
  else
    return ch;
}

/* Character class properties.  */

static bool
is_upper (unsigned int ch)
{
  return (to_lower (ch) != ch);
}

static bool
is_lower (unsigned int ch)
{
  return (to_upper (ch) != ch)
         /* <U00DF> is lowercase, but without simple to_upper mapping.  */
         || (ch == 0x00DF);
}

static bool
is_alpha (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && ((unicode_attributes[ch].category[0] == 'L'
               /* Theppitak Karoonboonyanan <thep@links.nectec.or.th> says
                  <U0E2F>, <U0E46> should belong to is_punct.  */
               && (ch != 0x0E2F) && (ch != 0x0E46))
              /* Theppitak Karoonboonyanan <thep@links.nectec.or.th> says
                 <U0E31>, <U0E34>..<U0E3A>, <U0E47>..<U0E4E> are is_alpha.  */
              || (ch == 0x0E31)
              || (ch >= 0x0E34 && ch <= 0x0E3A)
              || (ch >= 0x0E47 && ch <= 0x0E4E)
              /* Avoid warning for <U0345>.  */
              || (ch == 0x0345)
              /* Avoid warnings for <U2160>..<U217F>.  */
              || (unicode_attributes[ch].category[0] == 'N'
                  && unicode_attributes[ch].category[1] == 'l')
              /* Avoid warnings for <U24B6>..<U24E9>.  */
              || (unicode_attributes[ch].category[0] == 'S'
                  && unicode_attributes[ch].category[1] == 'o'
                  && strstr (unicode_attributes[ch].name, " LETTER ")
                     != NULL)
              /* Consider all the non-ASCII digits as alphabetic.
                 ISO C 99 forbids us to have them in category "digit",
                 but we want iswalnum to return true on them.  */
              || (unicode_attributes[ch].category[0] == 'N'
                  && unicode_attributes[ch].category[1] == 'd'
                  && !(ch >= 0x0030 && ch <= 0x0039))));
}

static bool
is_digit (unsigned int ch)
{
#if 0
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'N'
          && unicode_attributes[ch].category[1] == 'd');
  /* Note: U+0BE7..U+0BEF and U+1369..U+1371 are digit systems without
     a zero.  Must add <0> in front of them by hand.  */
#else
  /* SUSV2 gives us some freedom for the "digit" category, but ISO C 99
     takes it away:
     7.25.2.1.5:
        The iswdigit function tests for any wide character that corresponds
        to a decimal-digit character (as defined in 5.2.1).
     5.2.1:
        the 10 decimal digits 0 1 2 3 4 5 6 7 8 9
   */
  return (ch >= 0x0030 && ch <= 0x0039);
#endif
}

static bool
is_alnum (unsigned int ch)
{
  return is_alpha (ch) || is_digit (ch);
}

static bool
is_blank (unsigned int ch)
{
  return (ch == 0x0009 /* '\t' */
          /* Category Zs without mention of "<noBreak>" */
          || (unicode_attributes[ch].name != NULL
              && unicode_attributes[ch].category[0] == 'Z'
              && unicode_attributes[ch].category[1] == 's'
              && !strstr (unicode_attributes[ch].decomposition, "<noBreak>")));
}

static bool
is_space (unsigned int ch)
{
  /* Don't make U+00A0 a space. Non-breaking space means that all programs
     should treat it like a punctuation character, not like a space. */
  return (ch == 0x0020 /* ' ' */
          || ch == 0x000C /* '\f' */
          || ch == 0x000A /* '\n' */
          || ch == 0x000D /* '\r' */
          || ch == 0x0009 /* '\t' */
          || ch == 0x000B /* '\v' */
          /* Categories Zl, Zp, and Zs without mention of "<noBreak>" */
          || (unicode_attributes[ch].name != NULL
              && unicode_attributes[ch].category[0] == 'Z'
              && (unicode_attributes[ch].category[1] == 'l'
                  || unicode_attributes[ch].category[1] == 'p'
                  || (unicode_attributes[ch].category[1] == 's'
                      && !strstr (unicode_attributes[ch].decomposition,
                                  "<noBreak>")))));
}

static bool
is_cntrl (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && (strcmp (unicode_attributes[ch].name, "<control>") == 0
              /* Categories Zl and Zp */
              || (unicode_attributes[ch].category[0] == 'Z'
                  && (unicode_attributes[ch].category[1] == 'l'
                      || unicode_attributes[ch].category[1] == 'p'))));
}

static bool
is_xdigit (unsigned int ch)
{
#if 0
  return is_digit (ch)
         || (ch >= 0x0041 && ch <= 0x0046)
         || (ch >= 0x0061 && ch <= 0x0066);
#else
  /* SUSV2 gives us some freedom for the "xdigit" category, but ISO C 99
     takes it away:
     7.25.2.1.12:
        The iswxdigit function tests for any wide character that corresponds
        to a hexadecimal-digit character (as defined in 6.4.4.1).
     6.4.4.1:
        hexadecimal-digit: one of 0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F
   */
  return (ch >= 0x0030 && ch <= 0x0039)
         || (ch >= 0x0041 && ch <= 0x0046)
         || (ch >= 0x0061 && ch <= 0x0066);
#endif
}

static bool
is_graph (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && strcmp (unicode_attributes[ch].name, "<control>")
          && !is_space (ch));
}

static bool
is_print (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && strcmp (unicode_attributes[ch].name, "<control>")
          /* Categories Zl and Zp */
          && !(unicode_attributes[ch].name != NULL
               && unicode_attributes[ch].category[0] == 'Z'
               && (unicode_attributes[ch].category[1] == 'l'
                   || unicode_attributes[ch].category[1] == 'p')));
}

static bool
is_punct (unsigned int ch)
{
#if 0
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'P');
#else
  /* The traditional POSIX definition of punctuation is every graphic,
     non-alphanumeric character.  */
  return (is_graph (ch) && !is_alpha (ch) && !is_digit (ch));
#endif
}

/* Output all properties.  */
static void
output_old_ctype (const char *version)
{
#define PROPERTY(P) \
  debug_output_predicate ("unictype/ctype_" #P ".txt", is_ ## P); \
  output_predicate_test ("../tests/unictype/test-ctype_" #P ".c", is_ ## P, "uc_is_" #P " (c)"); \
  output_predicate ("unictype/ctype_" #P ".h", is_ ## P, "u_is_" #P, "ISO C <ctype.h> like properties", version);
  PROPERTY(alnum)
  PROPERTY(alpha)
  PROPERTY(cntrl)
  PROPERTY(digit)
  PROPERTY(graph)
  PROPERTY(lower)
  PROPERTY(print)
  PROPERTY(punct)
  PROPERTY(space)
  PROPERTY(upper)
  PROPERTY(xdigit)
  PROPERTY(blank)
#undef PROPERTY
}

#if 0

static bool
is_combining (unsigned int ch)
{
  /* Up to Unicode 3.0.1 we took the Combining property from the PropList.txt
     file. In 3.0.1 it was identical to the union of the general categories
     "Mn", "Mc", "Me". In Unicode 3.1 this property has been dropped from the
     PropList.txt file, so we take the latter definition.  */
  return (unicode_attributes[ch].name != NULL
          && unicode_attributes[ch].category[0] == 'M'
          && (unicode_attributes[ch].category[1] == 'n'
              || unicode_attributes[ch].category[1] == 'c'
              || unicode_attributes[ch].category[1] == 'e'));
}

static bool
is_combining_level3 (unsigned int ch)
{
  return is_combining (ch)
         && !(unicode_attributes[ch].combining[0] != '\0'
              && unicode_attributes[ch].combining[0] != '0'
              && strtoul (unicode_attributes[ch].combining, NULL, 10) >= 200);
}

/* Return the UCS symbol string for a Unicode character.  */
static const char *
ucs_symbol (unsigned int i)
{
  static char buf[11+1];

  sprintf (buf, (i < 0x10000 ? "<U%04X>" : "<U%08X>"), i);
  return buf;
}

/* Return the UCS symbol range string for a Unicode characters interval.  */
static const char *
ucs_symbol_range (unsigned int low, unsigned int high)
{
  static char buf[24+1];

  strcpy (buf, ucs_symbol (low));
  strcat (buf, "..");
  strcat (buf, ucs_symbol (high));
  return buf;
}

/* Output a character class (= property) table.  */

static void
output_charclass (FILE *stream, const char *classname,
                  bool (*func) (unsigned int))
{
  char table[0x110000];
  unsigned int i;
  bool need_semicolon;
  const int max_column = 75;
  int column;

  for (i = 0; i < 0x110000; i++)
    table[i] = (int) func (i);

  fprintf (stream, "%s ", classname);
  need_semicolon = false;
  column = 1000;
  for (i = 0; i < 0x110000; )
    {
      if (!table[i])
        i++;
      else
        {
          unsigned int low, high;
          char buf[25];

          low = i;
          do
            i++;
          while (i < 0x110000 && table[i]);
          high = i - 1;

          if (low == high)
            strcpy (buf, ucs_symbol (low));
          else
            strcpy (buf, ucs_symbol_range (low, high));

          if (need_semicolon)
            {
              fprintf (stream, ";");
              column++;
            }

          if (column + strlen (buf) > max_column)
            {
              fprintf (stream, "/\n   ");
              column = 3;
            }

          fprintf (stream, "%s", buf);
          column += strlen (buf);
          need_semicolon = true;
        }
    }
  fprintf (stream, "\n");
}

/* Output a character mapping table.  */

static void
output_charmap (FILE *stream, const char *mapname,
                unsigned int (*func) (unsigned int))
{
  char table[0x110000];
  unsigned int i;
  bool need_semicolon;
  const int max_column = 75;
  int column;

  for (i = 0; i < 0x110000; i++)
    table[i] = (func (i) != i);

  fprintf (stream, "%s ", mapname);
  need_semicolon = false;
  column = 1000;
  for (i = 0; i < 0x110000; i++)
    if (table[i])
      {
        char buf[25+1];

        strcpy (buf, "(");
        strcat (buf, ucs_symbol (i));
        strcat (buf, ",");
        strcat (buf, ucs_symbol (func (i)));
        strcat (buf, ")");

        if (need_semicolon)
          {
            fprintf (stream, ";");
            column++;
          }

        if (column + strlen (buf) > max_column)
          {
            fprintf (stream, "/\n   ");
            column = 3;
          }

        fprintf (stream, "%s", buf);
        column += strlen (buf);
        need_semicolon = true;
      }
  fprintf (stream, "\n");
}

/* Output the width table.  */

static void
output_widthmap (FILE *stream)
{
}

/* Output the tables to the given file.  */

static void
output_tables (const char *filename, const char *version)
{
  FILE *stream;
  unsigned int ch;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "escape_char /\n");
  fprintf (stream, "comment_char %%\n");
  fprintf (stream, "\n");
  fprintf (stream, "%% Generated automatically by gen-uni-tables.c for Unicode %s.\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "LC_IDENTIFICATION\n");
  fprintf (stream, "title     \"Unicode %s FDCC-set\"\n", version);
  fprintf (stream, "source    \"UnicodeData.txt, PropList.txt\"\n");
  fprintf (stream, "address   \"\"\n");
  fprintf (stream, "contact   \"\"\n");
  fprintf (stream, "email     \"bug-glibc@gnu.org\"\n");
  fprintf (stream, "tel       \"\"\n");
  fprintf (stream, "fax       \"\"\n");
  fprintf (stream, "language  \"\"\n");
  fprintf (stream, "territory \"Earth\"\n");
  fprintf (stream, "revision  \"%s\"\n", version);
  {
    time_t now;
    char date[11];
    now = time (NULL);
    strftime (date, sizeof (date), "%Y-%m-%d", gmtime (&now));
    fprintf (stream, "date      \"%s\"\n", date);
  }
  fprintf (stream, "category  \"unicode:2001\";LC_CTYPE\n");
  fprintf (stream, "END LC_IDENTIFICATION\n");
  fprintf (stream, "\n");

  /* Verification. */
  for (ch = 0; ch < 0x110000; ch++)
    {
      /* toupper restriction: "Only characters specified for the keywords
         lower and upper shall be specified.  */
      if (to_upper (ch) != ch && !(is_lower (ch) || is_upper (ch)))
        fprintf (stderr,
                 "%s is not upper|lower but toupper(0x%04X) = 0x%04X\n",
                 ucs_symbol (ch), ch, to_upper (ch));

      /* tolower restriction: "Only characters specified for the keywords
         lower and upper shall be specified.  */
      if (to_lower (ch) != ch && !(is_lower (ch) || is_upper (ch)))
        fprintf (stderr,
                 "%s is not upper|lower but tolower(0x%04X) = 0x%04X\n",
                 ucs_symbol (ch), ch, to_lower (ch));

      /* alpha restriction: "Characters classified as either upper or lower
         shall automatically belong to this class.  */
      if ((is_lower (ch) || is_upper (ch)) && !is_alpha (ch))
        fprintf (stderr, "%s is upper|lower but not alpha\n", ucs_symbol (ch));

      /* alpha restriction: "No character specified for the keywords cntrl,
         digit, punct or space shall be specified."  */
      if (is_alpha (ch) && is_cntrl (ch))
        fprintf (stderr, "%s is alpha and cntrl\n", ucs_symbol (ch));
      if (is_alpha (ch) && is_digit (ch))
        fprintf (stderr, "%s is alpha and digit\n", ucs_symbol (ch));
      if (is_alpha (ch) && is_punct (ch))
        fprintf (stderr, "%s is alpha and punct\n", ucs_symbol (ch));
      if (is_alpha (ch) && is_space (ch))
        fprintf (stderr, "%s is alpha and space\n", ucs_symbol (ch));

      /* space restriction: "No character specified for the keywords upper,
         lower, alpha, digit, graph or xdigit shall be specified."
         upper, lower, alpha already checked above.  */
      if (is_space (ch) && is_digit (ch))
        fprintf (stderr, "%s is space and digit\n", ucs_symbol (ch));
      if (is_space (ch) && is_graph (ch))
        fprintf (stderr, "%s is space and graph\n", ucs_symbol (ch));
      if (is_space (ch) && is_xdigit (ch))
        fprintf (stderr, "%s is space and xdigit\n", ucs_symbol (ch));

      /* cntrl restriction: "No character specified for the keywords upper,
         lower, alpha, digit, punct, graph, print or xdigit shall be
         specified."  upper, lower, alpha already checked above.  */
      if (is_cntrl (ch) && is_digit (ch))
        fprintf (stderr, "%s is cntrl and digit\n", ucs_symbol (ch));
      if (is_cntrl (ch) && is_punct (ch))
        fprintf (stderr, "%s is cntrl and punct\n", ucs_symbol (ch));
      if (is_cntrl (ch) && is_graph (ch))
        fprintf (stderr, "%s is cntrl and graph\n", ucs_symbol (ch));
      if (is_cntrl (ch) && is_print (ch))
        fprintf (stderr, "%s is cntrl and print\n", ucs_symbol (ch));
      if (is_cntrl (ch) && is_xdigit (ch))
        fprintf (stderr, "%s is cntrl and xdigit\n", ucs_symbol (ch));

      /* punct restriction: "No character specified for the keywords upper,
         lower, alpha, digit, cntrl, xdigit or as the <space> character shall
         be specified."  upper, lower, alpha, cntrl already checked above.  */
      if (is_punct (ch) && is_digit (ch))
        fprintf (stderr, "%s is punct and digit\n", ucs_symbol (ch));
      if (is_punct (ch) && is_xdigit (ch))
        fprintf (stderr, "%s is punct and xdigit\n", ucs_symbol (ch));
      if (is_punct (ch) && (ch == 0x0020))
        fprintf (stderr, "%s is punct\n", ucs_symbol (ch));

      /* graph restriction: "No character specified for the keyword cntrl
         shall be specified."  Already checked above.  */

      /* print restriction: "No character specified for the keyword cntrl
         shall be specified."  Already checked above.  */

      /* graph - print relation: differ only in the <space> character.
         How is this possible if there are more than one space character?!
         I think susv2/xbd/locale.html should speak of "space characters",
         not "space character".  */
      if (is_print (ch) && !(is_graph (ch) || /* ch == 0x0020 */ is_space (ch)))
        fprintf (stderr,
                 "%s is print but not graph|<space>\n", ucs_symbol (ch));
      if (!is_print (ch) && (is_graph (ch) || ch == 0x0020))
        fprintf (stderr,
                 "%s is graph|<space> but not print\n", ucs_symbol (ch));
    }

  fprintf (stream, "LC_CTYPE\n");
  output_charclass (stream, "upper", is_upper);
  output_charclass (stream, "lower", is_lower);
  output_charclass (stream, "alpha", is_alpha);
  output_charclass (stream, "digit", is_digit);
  output_charclass (stream, "outdigit", is_outdigit);
  output_charclass (stream, "blank", is_blank);
  output_charclass (stream, "space", is_space);
  output_charclass (stream, "cntrl", is_cntrl);
  output_charclass (stream, "punct", is_punct);
  output_charclass (stream, "xdigit", is_xdigit);
  output_charclass (stream, "graph", is_graph);
  output_charclass (stream, "print", is_print);
  output_charclass (stream, "class \"combining\";", is_combining);
  output_charclass (stream, "class \"combining_level3\";", is_combining_level3);
  output_charmap (stream, "toupper", to_upper);
  output_charmap (stream, "tolower", to_lower);
  output_charmap (stream, "map \"totitle\";", to_title);
  output_widthmap (stream);
  fprintf (stream, "END LC_CTYPE\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

#endif

/* ========================================================================= */

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

/* ========================================================================= */

/* Non-spacing attribute and width.  */

/* The non-spacing attribute table consists of:
   * Non-spacing characters; generated from PropList.txt or
     "grep '^[^;]*;[^;]*;[^;]*;[^;]*;NSM;' UnicodeData.txt"
   * Format control characters; generated from
     "grep '^[^;]*;[^;]*;Cf;' UnicodeData.txt"
   * Zero width characters; generated from
     "grep '^[^;]*;ZERO WIDTH ' UnicodeData.txt"
   * Hangul Jamo characters that have conjoining behaviour:
       - jungseong = syllable-middle vowels
       - jongseong = syllable-final consonants
     Rationale:
     1) These characters act like combining characters. They have no
     equivalent in legacy character sets. Therefore the EastAsianWidth.txt
     file does not really matter for them; UAX #11 East Asian Width
     <https://www.unicode.org/reports/tr11/> makes it clear that it focus
     is on compatibility with traditional Japanese layout.
     By contrast, the same glyphs without conjoining behaviour are available
     in the U+3130..U+318F block, and these characters are mapped to legacy
     character sets, and traditional Japanese layout matters for them.
     2) glibc does the same thing, see
     <https://sourceware.org/bugzilla/show_bug.cgi?id=21750>
     <https://sourceware.org/bugzilla/show_bug.cgi?id=26120>
 */

static bool
is_nonspacing (unsigned int ch)
{
  return (unicode_attributes[ch].name != NULL
          && (get_bidi_category (ch) == UC_BIDI_NSM
              || is_category_Cc (ch) || is_category_Cf (ch)
              || strncmp (unicode_attributes[ch].name, "ZERO WIDTH ", 11) == 0
              || (ch >= 0x1160 && ch <= 0x11A7) || (ch >= 0xD7B0 && ch <= 0xD7C6) /* jungseong */
              || (ch >= 0x11A8 && ch <= 0x11FF) || (ch >= 0xD7CB && ch <= 0xD7FB) /* jongseong */
         )   );
}

static void
output_nonspacing_property (const char *filename, const char *version)
{
  FILE *stream;
  int ind[0x110000 / 0x200];
  unsigned int i;
  unsigned int i_max;
  int next_ind;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Table of non-spacing or control characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, false);
  fprintf (stream, "\n");

  next_ind = 0;
  for (i = 0; i < 0x110000 / 0x200; i++)
    {
      bool nontrivial = false;
      unsigned int ch;

      if (i != 0xe0000 / 0x200) /* The 0xe0000 block is handled by code.  */
        for (ch = i * 0x200; ch < (i + 1) * 0x200; ch++)
          if (is_nonspacing (ch))
            {
              nontrivial = true;
              break;
            }
      if (nontrivial)
        ind[i] = next_ind++;
      else
        ind[i] = -1;
    }

  fprintf (stream, "static const unsigned char nonspacing_table_data[%d*64] = {\n",
           next_ind);
  i_max = 0;
  for (i = 0; i < 0x110000 / 0x200; i++)
    {
      bool nontrivial = (ind[i] >= 0);

      if (nontrivial)
        {
          unsigned int j;

          fprintf (stream, "  /* 0x%04x-0x%04x */\n", i * 0x200, (i + 1) * 0x200 - 1);
          for (j = 0; j < 8; j++)
            {
              unsigned int k;

              fprintf (stream, " ");
              for (k = 0; k < 8; k++)
                {
                  unsigned int l;
                  unsigned char bits = 0;

                  for (l = 0; l < 8; l++)
                    {
                      unsigned int ch = i * 0x200 + j * 0x40 + k * 8 + l;

                      if (is_nonspacing (ch))
                        bits |= 1 << l;
                    }
                  fprintf (stream, " 0x%02x%c", bits,
                           ind[i] + 1 == next_ind && j == 8 - 1 && k == 8 - 1 ? ' ' : ',');
                }
              fprintf (stream, " /* 0x%04x-0x%04x */\n",
                       i * 0x200 + j * 0x40, i * 0x200 + (j + 1) * 0x40 - 1);
            }
          i_max = i;
        }
    }
  fprintf (stream, "};\n");

  i_max = ((i_max + 8 - 1) / 8) * 8;
  fprintf (stream, "static const signed char nonspacing_table_ind[%u] = {\n",
           i_max);
  {
    unsigned int j;

    for (j = 0; j < i_max / 8; j++)
      {
        unsigned int k;

        fprintf (stream, " ");
        for (k = 0; k < 8; k++)
          {
            i = j * 8 + k;
            fprintf (stream, " %2d%c", ind[i],
                     j == i_max / 8 - 1 && k == 8 - 1 ? ' ' : ',');
          }
        fprintf (stream, " /* 0x%04x-0x%04x */\n",
                 j * 8 * 0x200, (j + 1) * 8 * 0x200 - 1);
      }
  }
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Determines whether a character has width 2, regardless of context.
   Generated from "grep '^[^;]\+;[WF]' EastAsianWidth.txt"
   and            "grep '^[^;]\+;[^WF]' EastAsianWidth.txt"
 */
static bool
is_width2 (unsigned int ch)
{
  return ((ch >= 0x1100 && ch <= 0x115F) /* Hangul Jamo */
          || (ch >= 0x231A && ch <= 0x231B) /* Watch, Hourglass */
          || (ch >= 0x2329 && ch <= 0x232A) /* Angle Brackets */
          || (ch >= 0x23E9 && ch <= 0x23EC) /* Black double triangles */
          || ch == 0x23F0 /* Alarm clock */
          || ch == 0x23F3 /* Hourglass */
          || (ch >= 0x25FD && ch <= 0x25FE) /* Medium small squares */
          /* Miscellaneous symbols, dingbats */
          || (ch >= 0x2614 && ch <= 0x2615)
          || (ch >= 0x2648 && ch <= 0x2653)
          || ch == 0x267F
          || ch == 0x2693
          || ch == 0x26A1
          || (ch >= 0x26AA && ch <= 0x26AB)
          || (ch >= 0x26BD && ch <= 0x26BE)
          || (ch >= 0x26C4 && ch <= 0x26C5)
          || ch == 0x26CE
          || ch == 0x26D4
          || ch == 0x26EA
          || (ch >= 0x26F2 && ch <= 0x26F3)
          || ch == 0x26F5
          || ch == 0x26FA
          || ch == 0x26FD
          || ch == 0x2705
          || (ch >= 0x270A && ch <= 0x270B)
          || ch == 0x2728
          || ch == 0x274C
          || ch == 0x274E
          || (ch >= 0x2753 && ch <= 0x2755)
          || ch == 0x2757
          || (ch >= 0x2795 && ch <= 0x2797)
          || ch == 0x27B0
          || ch == 0x27BF
          || (ch >= 0x2B1B && ch <= 0x2B1C) /* Large squares */
          || ch == 0x2B50
          || ch == 0x2B55
          || (ch >= 0x2E80 && ch <= 0xA4CF  /* CJK ... Yi */
              && !(ch == 0x303F)
              && !(ch >= 0x3248 && ch <= 0x324F)
              && !(ch >= 0x4DC0 && ch <= 0x4DFF))
          || (ch >= 0xA960 && ch <= 0xA97C) /* Hangul Jamo Extended-A */
          || (ch >= 0xAC00 && ch <= 0xD7A3) /* Hangul Syllables */
          || (ch >= 0xF900 && ch <= 0xFAFF) /* CJK Compatibility Ideographs */
          || (ch >= 0xFE10 && ch <= 0xFE1F) /* Presentation Forms for Vertical */
          || (ch >= 0xFE30 && ch <= 0xFE6F) /* CJK Compatibility Forms */
          || (ch >= 0xFF00 && ch <= 0xFF60) /* Fullwidth Forms */
          || (ch >= 0xFFE0 && ch <= 0xFFE6) /* Fullwidth Signs */
          || (ch >= 0x16FE0 && ch <= 0x16FE3) /* Tangut mark, Nushu mark */
          || (ch >= 0x16FF0 && ch <= 0x16FF1) /* Vietnamese alternate reading marks */
          || (ch >= 0x17000 && ch <= 0x187F7) /* Tangut */
          || (ch >= 0x18800 && ch <= 0x18CD5) /* Tangut components */
          || (ch >= 0x18D00 && ch <= 0x18D08) /* Tangul Ideograph Supplement */
          || ((ch >= 0x1AFF0 && ch <= 0x1AFFE) /* Katakana letter Minnan */
              && ch != 0x1AFF4 && ch != 0x1AFFC)
          || (ch >= 0x1B000 && ch <= 0x1B122) /* Kana supplement, Kana Extended-A */
          || (ch >= 0x1B150 && ch <= 0x1B152) /* Small Hiragana */
          || (ch >= 0x1B164 && ch <= 0x1B167) /* Small Katakana */
          || (ch >= 0x1B170 && ch <= 0x1B2FB) /* Nushu */
          || ch == 0x1F004
          || ch == 0x1F0CF
          || ch == 0x1F18E
          || (ch >= 0x1F191 && ch <= 0x1F19A)
          /* Miscellaneous symbols and pictographs */
          || (ch >= 0x1F200 && ch <= 0x1F320)
          || (ch >= 0x1F32D && ch <= 0x1F335)
          || (ch >= 0x1F337 && ch <= 0x1F37C)
          || (ch >= 0x1F37E && ch <= 0x1F393)
          || (ch >= 0x1F3A0 && ch <= 0x1F3CA)
          || (ch >= 0x1F3CF && ch <= 0x1F3D3)
          || (ch >= 0x1F3E0 && ch <= 0x1F3F0)
          || ch == 0x1F3F4
          || (ch >= 0x1F3F8 && ch <= 0x1F43E)
          || ch == 0x1F440
          || (ch >= 0x1F442 && ch <= 0x1F4FC)
          || (ch >= 0x1F4FF && ch <= 0x1F53D)
          || (ch >= 0x1F54B && ch <= 0x1F54E)
          || (ch >= 0x1F550 && ch <= 0x1F567)
          || ch == 0x1F57A
          || (ch >= 0x1F595 && ch <= 0x1F596)
          || ch == 0x1F5A4
          || (ch >= 0x1F5FB && ch <= 0x1F64F)
          || (ch >= 0x1F680 && ch <= 0x1F6C5)
          || ch == 0x1F6CC
          || (ch >= 0x1F6D0 && ch <= 0x1F6D2)
          || (ch >= 0x1F6D5 && ch <= 0x1F6D7)
          || (ch >= 0x1F6DD && ch <= 0x1F6DF)
          || (ch >= 0x1F6EB && ch <= 0x1F6EC)
          || (ch >= 0x1F6F4 && ch <= 0x1F6FC)
          || (ch >= 0x1F7E0 && ch <= 0x1F7EB)
          || ch == 0x1F7F0
          || ((ch >= 0x1F90C && ch <= 0x1F9FF)
              && ch != 0x1F93B && ch != 0x1F946)
          || (ch >= 0x1FA70 && ch <= 0x1FA74)
          || (ch >= 0x1FA78 && ch <= 0x1FA7C)
          || (ch >= 0x1FA80 && ch <= 0x1FA86)
          || (ch >= 0x1FA90 && ch <= 0x1FAAC)
          || (ch >= 0x1FAB0 && ch <= 0x1FABA)
          || (ch >= 0x1FAC0 && ch <= 0x1FAC5)
          || (ch >= 0x1FAD0 && ch <= 0x1FAD9)
          || (ch >= 0x1FAE0 && ch <= 0x1FAE7)
          || (ch >= 0x1FAF0 && ch <= 0x1FAF6)
          || (ch >= 0x20000 && ch <= 0x2FFFF) /* Supplementary Ideographic Plane */
          || (ch >= 0x30000 && ch <= 0x3FFFF) /* Tertiary Ideographic Plane */
         );
}

static void
output_width2_property (const char *filename, const char *version)
{
  output_predicate (filename, is_width2, "u_width2", "Width 2 property", version);
}

/* Returns the width of ch as one of 0, '0', '1', '2', 'A'.  */
static char
symbolic_width (unsigned int ch)
{
  /* Test for unassigned character.  */
  if (is_property_unassigned_code_value (ch))
    {
      /* Unicode TR#11 section "Unassigned and Private-Use Characters".  */
      if (ch >= 0xE000 && ch <= 0xF8FF) /* Private Use */
        return 'A';
      if ((ch >= 0x4E00 && ch <= 0x9FFF) /* CJK Unified Ideographs block */
          || (ch >= 0x3400 && ch <= 0x4DBF) /* CJK Unified Ideographs Extension A block */
          || (ch >= 0xF900 && ch <= 0xFAFF) /* CJK Compatibility Ideographs block */
          || (ch >= 0x20000 && ch <= 0x2FFFF) /* Supplementary Ideographic Plane */
          || (ch >= 0x30000 && ch <= 0x3FFFF) /* Tertiary Ideographic Plane */)
        return '2';
      return 0;
    }
  else
    {
      /* Test for non-spacing or control character.  */
      if (is_category_Cc (ch) && ch < 0x00A0)
        return 0;
      if (is_nonspacing (ch))
        return '0';
      /* Test for double-width character.  */
      if (unicode_width[ch] != NULL
          && (strcmp (unicode_width[ch], "W") == 0
              || strcmp (unicode_width[ch], "F") == 0))
        return '2';
      /* Test for half-width character.  */
      if (unicode_width[ch] != NULL
          && strcmp (unicode_width[ch], "H") == 0)
        return '1';
    }
  /* In ancient CJK encodings, Cyrillic and most other characters are
     double-width as well.  */
  if (ch >= 0x00A1 && ch < 0x10000)
    return 'A';
  return '1';
}

static void
output_width_property_test (const char *filename)
{
  FILE *stream;
  unsigned int interval_start, interval_end, ch;
  char interval_value;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  interval_value = 0;
  interval_start = interval_end = 0; /* avoid GCC warning */
  for (ch = 0; ch < 0x110000; ch++)
    {
      char value = symbolic_width (ch);
      if (value != 0) /* skip Cc control characters and unassigned characters */
        {
          if (value == interval_value)
            /* Extend the interval.  */
            interval_end = ch;
          else
            {
              /* Terminate the interval.  */
              if (interval_value != 0)
                {
                  if (interval_end == interval_start)
                    fprintf (stream, "%04X\t\t%c\n", interval_start, interval_value);
                  else
                    fprintf (stream, "%04X..%04X\t%c\n", interval_start, interval_end, interval_value);
                }
              /* Start a new interval.  */
              interval_start = interval_end = ch;
              interval_value = value;
            }
        }
    }
  /* Terminate the last interval.  */
  if (interval_value != 0)
    {
      if (interval_end == interval_start)
        fprintf (stream, "%04X\t\t%c\n", interval_start, interval_value);
      else
        fprintf (stream, "%04X..%04X\t%c\n", interval_start, interval_end, interval_value);
    }

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Line breaking classification.
   Updated for Unicode TR #14 revision 26.  */

enum
{
  /* Values >= 33 are resolved at run time. */
  /* Values >= 100 are shorthands for several values. */
  LBP_BK  = 33, /* mandatory break */
  LBP_CR  = 34, /* carriage return */
  LBP_LF  = 35, /* line feed */
  LBP_CM  = 36, /* attached characters and combining marks */
/*LBP_NL,          next line - not used here because it's equivalent to LBP_BK */
/*LBP_SG,          surrogates - not used here because they are not characters */
  LBP_WJ  =  0, /* word joiner */
  LBP_ZW  = 37, /* zero width space */
  LBP_GL  =  1, /* non-breaking (glue) */
  LBP_SP  = 38, /* space */
  LBP_B2  =  2, /* break opportunity before and after */
  LBP_BA  =  3, /* break opportunity after */
  LBP_BB  =  4, /* break opportunity before */
  LBP_HY  =  5, /* hyphen */
  LBP_CB  = 39, /* contingent break opportunity */
  LBP_CL  =  6, /* closing punctuation */
  LBP_CP1 =  7, /* closing parenthesis, non-EastAsian character */
  LBP_CP2 =  8, /* closing parenthesis, EastAsian character */
  LBP_EX  =  9, /* exclamation/interrogation */
  LBP_IN  = 10, /* inseparable */
  LBP_NS  = 11, /* non starter */
  LBP_OP1 = 12, /* opening punctuation, non-EastAsian character */
  LBP_OP2 = 13, /* opening punctuation, EastAsian character */
  LBP_QU  = 14, /* ambiguous quotation */
  LBP_IS  = 15, /* infix separator (numeric) */
  LBP_NU  = 16, /* numeric */
  LBP_PO  = 17, /* postfix (numeric) */
  LBP_PR  = 18, /* prefix (numeric) */
  LBP_SY  = 19, /* symbols allowing breaks */
  LBP_AI  = 40, /* ambiguous (alphabetic or ideograph) */
  LBP_AL  = 20, /* ordinary alphabetic and symbol characters */
/*LBP_CJ,          conditional Japanese starter, resolved to NS */
  LBP_H2  = 21, /* Hangul LV syllable */
  LBP_H3  = 22, /* Hangul LVT syllable */
  LBP_HL  = 28, /* Hebrew letter */
  LBP_ID1 = 23, /* ideographic */
  LBP_ID2 = 24, /* ideographic and potential future emoji */
  LBP_JL  = 25, /* Hangul L Jamo */
  LBP_JV  = 26, /* Hangul V Jamo */
  LBP_JT  = 27, /* Hangul T Jamo */
  LBP_RI  = 29, /* regional indicator */
  LBP_SA  = 41, /* complex context (South East Asian) */
  LBP_ZWJ = 30, /* zero width joiner */
  LBP_EB  = 31, /* emoji base */
  LBP_EM  = 32, /* emoji modifier */
  LBP_XX  = 42, /* unknown */
  /* Artificial values that exist only in this file, not in the tables. */
  LBP_OP  = 100, /* LBP_OP1 or LBP_OP2 */
  LBP_CP  = 101, /* LBP_CP1 or LBP_CP2 */
  LBP_ID  = 102  /* LBP_ID1 or LBP_ID2 */
};

/* Returns the line breaking classification for ch, as a bit mask.  */
static int64_t
get_lbp (unsigned int ch)
{
  int64_t attr = 0;

  /* U+20BC..U+20CF are reserved for prefixes.  */
  if (unicode_attributes[ch].name == NULL && (ch >= 0x20BC && ch <= 0x20CF))
    return (int64_t) 1 << LBP_PR;

  if (unicode_attributes[ch].name != NULL)
    {
      /* mandatory break */
      if (ch == 0x000A)
        attr |= (int64_t) 1 << LBP_LF;
      if (ch == 0x000D)
        attr |= (int64_t) 1 << LBP_CR;
      if (ch == 0x0085 /* newline */
          || ch == 0x000C /* form feed */
          || ch == 0x000B /* line tabulation */
          || ch == 0x2028 /* LINE SEPARATOR */
          || ch == 0x2029 /* PARAGRAPH SEPARATOR */)
        attr |= (int64_t) 1 << LBP_BK;

      if (ch == 0x2060 /* WORD JOINER */
          || ch == 0xFEFF /* ZERO WIDTH NO-BREAK SPACE */)
        attr |= (int64_t) 1 << LBP_WJ;

      /* zero width space */
      if (ch == 0x200B /* ZERO WIDTH SPACE */)
        attr |= (int64_t) 1 << LBP_ZW;

      /* zero width joiner */
      if (ch == 0x200D /* ZERO WIDTH JOINER */)
        attr |= (int64_t) 1 << LBP_ZWJ;

      /* emoji base */
      if (ch == 0x261D /* WHITE UP POINTING INDEX */
          || ch == 0x26F9 /* PERSON WITH BALL */
          || (ch >= 0x270A && ch <= 0x270D) /* RAISED FIST..WRITING HAND */
          || ch == 0x1F385 /* FATHER CHRISTMAS */
          || (ch >= 0x1F3C2 && ch <= 0x1F3C4) /* SNOWBOARDER..SURFER */
          || ch == 0x1F3C7 /* HORSE RACING */
          || (ch >= 0x1F3CA && ch <= 0x1F3CC) /* SWIMMER..GOLFER */
          || (ch >= 0x1F442 && ch <= 0x1F443) /* EAR..NOSE */
          || (ch >= 0x1F446 && ch <= 0x1F450) /* WHITE UP POINTING BACKHAND INDEX..OPEN HANDS SIGN */
          || (ch >= 0x1F466 && ch <= 0x1F478) /* BOY..PRINCESS */
          || ch == 0x1F47C /* BABY ANGEL */
          || (ch >= 0x1F481 && ch <= 0x1F483) /* INFORMATION DESK PERSON..DANCER */
          || (ch >= 0x1F485 && ch <= 0x1F487) /* NAIL POLISH..HAIRCUT */
          || ch == 0x1F48F /* KISS */
          || ch == 0x1F491 /* COUPLE WITH HEART */
          || ch == 0x1F4AA /* FLEXED BICEPS */
          || (ch >= 0x1F574 && ch <= 0x1F575) /* MAN IN BUSINESS SUIT LEVITATING..SLEUTH OR SPY */
          || ch == 0x1F57A /* MAN DANCING */
          || ch == 0x1F590 /* RAISED HAND WITH FINGERS SPLAYED */
          || (ch >= 0x1F595 && ch <= 0x1F596) /* REVERSED HAND WITH MIDDLE FINGER EXTENDED..RAISED HAND WITH PART BETWEEN MIDDLE AND RING FINGERS */
          || (ch >= 0x1F645 && ch <= 0x1F647) /* FACE WITH NO GOOD GESTURE..PERSON BOWING DEEPLY */
          || (ch >= 0x1F64B && ch <= 0x1F64F) /* HAPPY PERSON RAISING ONE HAND..PERSON WITH FOLDED HANDS */
          || ch == 0x1F6A3 /* ROWBOAT */
          || (ch >= 0x1F6B4 && ch <= 0x1F6B6) /* BICYCLIST..PEDESTRIAN */
          || ch == 0x1F6C0 /* BATH */
          || ch == 0x1F6CC /* SLEEPING ACCOMMODATION */
          || ch == 0x1F90C /* PINCHED FINGERS */
          || ch == 0x1F90F /* PINCHING HAND */
          || (ch >= 0x1F918 && ch <= 0x1F91F) /* SIGN OF THE HORNS..I LOVE YOU HAND SIGN */
          || ch == 0x1F926 /* FACE PALM */
          || (ch >= 0x1F930 && ch <= 0x1F939) /* PREGNANT WOMAN..JUGGLING */
          || (ch >= 0x1F93C && ch <= 0x1F93E) /* WRESTLERS..HANDBALL */
          || ch == 0x1F977 /* NINJA */
          || (ch >= 0x1F9B5 && ch <= 0x1F9B6) /* LEG..FOOT */
          || (ch >= 0x1F9B8 && ch <= 0x1F9B9) /* SUPERHERO..SUPERVILLAIN */
          || ch == 0x1F9BB /* EAR WITH HEARING AID */
          || (ch >= 0x1F9CD && ch <= 0x1F9CF) /* STANDING PERSON..DEAF PERSON */
          || (ch >= 0x1F9D1 && ch <= 0x1F9DD) /* ADULT..ELF */
          || (ch >= 0x1FAC3 && ch <= 0x1FAC5) /* PREGNANT MAN..PERSON WITH CROWN */
          || (ch >= 0x1FAF0 && ch <= 0x1FAF6) /* HAND WITH INDEX FINGER AND THUMB CROSSED..HEART HANDS */)
        attr |= (int64_t) 1 << LBP_EB;

      if (((unicode_properties[ch] >> PROP_EMOJI_MODIFIER) & 1) != 0) /* EMOJI MODIFIER */
        attr |= (int64_t) 1 << LBP_EM;

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
          || (ch >= 0x035C && ch <= 0x0362) /* COMBINING DOUBLE ... */
          /* Extra characters for compatibility with Unicode LineBreak.txt.  */
          || ch == 0x0FD9 /* TIBETAN MARK LEADING MCHAN RTAGS */
          || ch == 0x0FDA /* TIBETAN MARK TRAILING MCHAN RTAGS */
          || (ch >= 0x13430 && ch <= 0x13436) /* EGYPTIAN HIEROGLYPH VERTICAL JOINER..EGYPTIAN HIEROGLYPH OVERLAY MIDDLE */
          || ch == 0x16FE4 /* KHITAN SMALL SCRIPT FILLER */)
        attr |= (int64_t) 1 << LBP_GL;

      /* space */
      if (ch == 0x0020 /* SPACE */)
        attr |= (int64_t) 1 << LBP_SP;

      /* break opportunity before and after */
      if (ch == 0x2014 /* EM DASH */
          || ch == 0x2E3A /* TWO-EM DASH */
          || ch == 0x2E3B /* THREE-EM DASH */)
        attr |= (int64_t) 1 << LBP_B2;

      /* break opportunity after */
      if (/* Breaking Spaces */
          ch == 0x1680 /* OGHAM SPACE MARK */
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
          || ch == 0x3000 /* IDEOGRAPHIC SPACE */
          /* Tabs */
          || ch == 0x0009 /* tab */
          /* Conditional Hyphens */
          || ch == 0x00AD /* SOFT HYPHEN */
          /* Breaking Hyphens */
          || ch == 0x058A /* ARMENIAN HYPHEN */
          || ch == 0x1400 /* CANADIAN SYLLABICS HYPHEN */
          || ch == 0x2010 /* HYPHEN */
          || ch == 0x2012 /* FIGURE DASH */
          || ch == 0x2013 /* EN DASH */
          /* Visible Word Dividers */
          || ch == 0x05BE /* HEBREW PUNCTUATION MAQAF */
          || ch == 0x0F0B /* TIBETAN MARK INTERSYLLABIC TSHEG */
          || ch == 0x1361 /* ETHIOPIC WORDSPACE */
          || ch == 0x17D8 /* KHMER SIGN BEYYAL */
          || ch == 0x17DA /* KHMER SIGN KOOMUUT */
          || ch == 0x2027 /* HYPHENATION POINT */
          || ch == 0x007C /* VERTICAL LINE */
          /* Historic Word Separators */
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
          || ch == 0x2E31 /* WORD SEPARATOR MIDDLE DOT */
          || ch == 0x2E33 /* RAISED DOT */
          || ch == 0x2E34 /* RAISED COMMA */
          || ch == 0x10100 /* AEGEAN WORD SEPARATOR LINE */
          || ch == 0x10101 /* AEGEAN WORD SEPARATOR DOT */
          || ch == 0x10102 /* AEGEAN CHECK MARK */
          || ch == 0x1039F /* UGARITIC WORD DIVIDER */
          || ch == 0x103D0 /* OLD PERSIAN WORD DIVIDER */
          || ch == 0x1091F /* PHOENICIAN WORD SEPARATOR */
          || ch == 0x12470 /* CUNEIFORM PUNCTUATION SIGN OLD ASSYRIAN WORD DIVIDER */
          /* Dandas */
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
          /* Tibetan */
          || ch == 0x0F34 /* TIBETAN MARK BSDUS RTAGS */
          || ch == 0x0F7F /* TIBETAN SIGN RNAM BCAD */
          || ch == 0x0F85 /* TIBETAN MARK PALUTA */
          || ch == 0x0FBE /* TIBETAN KU RU KHA */
          || ch == 0x0FBF /* TIBETAN KU RU KHA BZHI MIG CAN */
          || ch == 0x0FD2 /* TIBETAN MARK NYIS TSHEG */
          /* Other Terminating Punctuation */
          || ch == 0x1804 /* MONGOLIAN COLON */
          || ch == 0x1805 /* MONGOLIAN FOUR DOTS */
          || ch == 0x1B5A /* BALINESE PANTI */
          || ch == 0x1B5B /* BALINESE PAMADA */
          || ch == 0x1B5D /* BALINESE CARIK PAMUNGKAH */
          || ch == 0x1B60 /* BALINESE PAMENENG */
          || ch == 0x1C3B /* LEPCHA PUNCTUATION TA-ROL */
          || ch == 0x1C3C /* LEPCHA PUNCTUATION NYET THYOOM TA-ROL */
          || ch == 0x1C3D /* LEPCHA PUNCTUATION CER-WA */
          || ch == 0x1C3E /* LEPCHA PUNCTUATION TSHOOK CER-WA */
          || ch == 0x1C3F /* LEPCHA PUNCTUATION TSHOOK */
          || ch == 0x1C7E /* OL CHIKI PUNCTUATION MUCAAD */
          || ch == 0x1C7F /* OL CHIKI PUNCTUATION DOUBLE MUCAAD */
          || ch == 0x2CFA /* COPTIC OLD NUBIAN DIRECT QUESTION MARK */
          || ch == 0x2CFB /* COPTIC OLD NUBIAN INDIRECT QUESTION MARK */
          || ch == 0x2CFC /* COPTIC OLD NUBIAN VERSE DIVIDER */
          || ch == 0x2CFF /* COPTIC MORPHOLOGICAL DIVIDER */
          || (ch >= 0x2E0E && ch <= 0x2E15) /* EDITORIAL CORONIS .. UPWARDS ANCORA */
          || ch == 0x2E17 /* DOUBLE OBLIQUE HYPHEN */
          || ch == 0x2E3C /* STENOGRAPHIC FULL STOP */
          || ch == 0x2E3D /* VERTICAL SIX DOTS */
          || ch == 0x2E3E /* WIGGLY VERTICAL LINE */
          || ch == 0x2E40 /* DOUBLE HYPHEN */
          || ch == 0x2E41 /* REVERSED COMMA */
          || ch == 0x2E43 /* DASH WITH LEFT UPTURN */
          || ch == 0x2E44 /* DOUBLE SUSPENSION MARK */
          || ch == 0x2E45 /* INVERTED LOW KAVYKA */
          || ch == 0x2E46 /* INVERTED LOW KAVYKA WITH KAVYKA ABOVE */
          || ch == 0x2E47 /* LOW KAVYKA */
          || ch == 0x2E48 /* LOW KAVYKA WITH DOT */
          || ch == 0x2E49 /* DOUBLE STACKED COMMA */
          || ch == 0x2E4A /* DOTTED SOLIDUS */
          || ch == 0x2E4C /* MEDIEVAL COMMA */
          || ch == 0x2E4E /* PUNCTUS ELEVATUS MARK */
          || ch == 0x2E4F /* CORNISH VERSE DIVIDER */
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
          || ch == 0x1B7D /* BALINESE PANTI LANTANG */
          || ch == 0x1B7E /* BALINESE PAMADA LANTANG */
          || ch == 0x2D70 /* TIFINAGH SEPARATOR MARK */
          || ch == 0x2E5D /* OBLIQUE HYPHEN */
          || ch == 0xA4FE /* LISU PUNCTUATION COMMA */
          || ch == 0xA4FF /* LISU PUNCTUATION FULL STOP */
          || ch == 0xA6F3 /* BAMUM FULL STOP */
          || ch == 0xA6F4 /* BAMUM COLON */
          || ch == 0xA6F5 /* BAMUM COMMA */
          || ch == 0xA6F6 /* BAMUM SEMICOLON */
          || ch == 0xA6F7 /* BAMUM QUESTION MARK */
          || ch == 0xA9C7 /* JAVANESE PADA PANGKAT */
          || ch == 0xA9C8 /* JAVANESE PADA LINGSA */
          || ch == 0xA9C9 /* JAVANESE PADA LUNGSI */
          || ch == 0xAAF0 /* MEETEI MAYEK CHEIKHAN */
          || ch == 0xAAF1 /* MEETEI MAYEK AHANG KHUDAM */
          || ch == 0xABEB /* MEETEI MAYEK CHEIKHEI */
          || ch == 0x10857 /* IMPERIAL ARAMAIC SECTION SIGN */
          || (ch >= 0x10AF0 && ch <= 0x10AF5) /* MANICHAEAN PUNCTUATION STAR..MANICHAEAN PUNCTUATION TWO DOTS */
          || ch == 0x10B39 /* AVESTAN ABBREVIATION MARK */
          || ch == 0x10B3A /* TINY TWO DOTS OVER ONE DOT PUNCTUATION */
          || ch == 0x10B3B /* SMALL TWO DOTS OVER ONE DOT PUNCTUATION */
          || ch == 0x10B3C /* LARGE TWO DOTS OVER ONE DOT PUNCTUATION */
          || ch == 0x10B3D /* LARGE ONE DOT OVER TWO DOTS PUNCTUATION */
          || ch == 0x10B3E /* LARGE TWO RINGS OVER ONE RING PUNCTUATION */
          || ch == 0x10B3F /* LARGE ONE RING OVER TWO RINGS PUNCTUATION */
          || ch == 0x10EAD /* YEZIDI HYPHENATION MARK */
          || ch == 0x11047 /* BRAHMI DANDA */
          || ch == 0x11048 /* BRAHMI DOUBLE DANDA */
          || ch == 0x110BE /* KAITHI SECTION MARK */
          || ch == 0x110BF /* KAITHI DOUBLE SECTION MARK */
          || ch == 0x110C0 /* KAITHI DANDA */
          || ch == 0x110C1 /* KAITHI DOUBLE DANDA */
          || ch == 0x11140 /* CHAKMA SECTION MARK */
          || ch == 0x11141 /* CHAKMA DANDA */
          || ch == 0x11142 /* CHAKMA DOUBLE DANDA */
          || ch == 0x11143 /* CHAKMA QUESTION MARK */
          || ch == 0x111C5 /* SHARADA DANDA */
          || ch == 0x111C6 /* SHARADA DOUBLE DANDA */
          || ch == 0x111C8 /* SHARADA SEPARATOR */
          || (ch >= 0x111DD && ch <= 0x111DF) /* SHARADA CONTINUATION SIGN..SHARADA SECTION MARK-2 */
          || ch == 0x11238 /* KHOJKI DANDA */
          || ch == 0x11239 /* KHOJKI DOUBLE DANDA */
          || ch == 0x1123B /* KHOJKI SECTION MARK */
          || ch == 0x1123C /* KHOJKI DOUBLE SECTION MARK */
          || ch == 0x112A9 /* MULTANI SECTION MARK */
          || (ch >= 0x1144B && ch <= 0x1144E) /* NEWA DANDA..NEWA GAP FILLER */
          || ch == 0x1145A /* NEWA DOUBLE COMMA */
          || ch == 0x1145B /* NEWA PLACEHOLDER MARK */
          || ch == 0x115C2 /* SIDDHAM DANDA */
          || ch == 0x115C3 /* SIDDHAM DOUBLE DANDA */
          || (ch >= 0x115C9 && ch <= 0x115D7) /* SIDDHAM END OF TEXT MARK..SIDDHAM SECTION MARK WITH CIRCLES AND FOUR ENCLOSURES */
          || ch == 0x11641 /* MODI DANDA */
          || ch == 0x11642 /* MODI DOUBLE DANDA */
          || (ch >= 0x1173C && ch <= 0x1173E) /* AHOM SIGN SMALL SECTION..AHOM SIGN RULAI */
          || (ch >= 0x11944 && ch <= 0x11946) /* DIVES AKURU DOUBLE DANDA..DIVES AKURU END OF TEXT MARK */
          || ch == 0x11A41 /* ZANABAZAR SQUARE MARK TSHEG */
          || ch == 0x11A42 /* ZANABAZAR SQUARE MARK SHAD */
          || ch == 0x11A43 /* ZANABAZAR SQUARE MARK DOUBLE SHAD */
          || ch == 0x11A44 /* ZANABAZAR SQUARE MARK LONG TSHEG */
          || ch == 0x11A9A /* SOYOMBO MARK TSHEG */
          || ch == 0x11A9B /* SOYOMBO MARK SHAD */
          || ch == 0x11A9C /* SOYOMBO MARK DOUBLE SHAD */
          || ch == 0x11AA1 /* SOYOMBO TERMINAL MARK-1 */
          || ch == 0x11AA2 /* SOYOMBO TERMINAL MARK-2 */
          || (ch >= 0x11C41 && ch <= 0x11C45) /* BHAIKSUKI DANDA..BHAIKSUKI GAP FILLER-2 */
          || ch == 0x11FFF /* TAMIL PUNCTUATION END OF TEXT */
          || ch == 0x12471 /* CUNEIFORM PUNCTUATION SIGN VERTICAL COLON */
          || ch == 0x12472 /* CUNEIFORM PUNCTUATION SIGN DIAGONAL COLON */
          || ch == 0x12473 /* CUNEIFORM PUNCTUATION SIGN DIAGONAL TRICOLON */
          || ch == 0x12474 /* CUNEIFORM PUNCTUATION SIGN DIAGONAL QUADCOLON */
          || ch == 0x16A6E /* MRO DANDA */
          || ch == 0x16A6F /* MRO DOUBLE DANDA */
          || ch == 0x16AF5 /* BASSA VAH FULL STOP */
          || ch == 0x16B37 /* PAHAWH HMONG SIGN VOS THOM */
          || ch == 0x16B38 /* PAHAWH HMONG SIGN VOS TSHAB CEEB */
          || ch == 0x16B39 /* PAHAWH HMONG SIGN CIM CHEEM */
          || ch == 0x16B44 /* PAHAWH HMONG SIGN XAUS */
          || ch == 0x16E97 /* MEDEFAIDRIN COMMA */
          || ch == 0x16E98 /* MEDEFAIDRIN FULL STOP */
          || ch == 0x1BC9F /* DUPLOYAN PUNCTUATION CHINOOK FULL STOP */
          || (ch >= 0x1DA87 && ch <= 0x1DA8A) /* SIGNWRITING COMMA..SIGNWRITING COLON */)
        attr |= (int64_t) 1 << LBP_BA;

      /* break opportunity before */
      if (ch == 0x00B4 /* ACUTE ACCENT */
          || ch == 0x1FFD /* GREEK OXIA */
          || ch == 0x02DF /* MODIFIER LETTER CROSS ACCENT */
          || ch == 0x02C8 /* MODIFIER LETTER VERTICAL LINE */
          || ch == 0x02CC /* MODIFIER LETTER LOW VERTICAL LINE */
          || ch == 0x0C77 /* TELUGU SIGN SIDDHAM */
          || ch == 0x0C84 /* KANNADA SIGN SIDDHAM */
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
          || ch == 0xA8FC /* DEVANAGARI SIGN SIDDHAM */
          || ch == 0x1806 /* MONGOLIAN TODO SOFT HYPHEN */
          || ch == 0x11175 /* MAHAJANI SECTION MARK */
          || ch == 0x111DB /* SHARADA SIGN SIDDHAM */
          || ch == 0x115C1 /* SIDDHAM SIGN SIDDHAM */
          || (ch >= 0x11660 && ch <= 0x1166C) /* MONGOLIAN BIRGA WITH ORNAMENT..MONGOLIAN TURNED SWIRL BIRGA WITH DOUBLE ORNAMENT */
          || ch == 0x119E2 /* NANDINAGARI SIGN SIDDHAM */
          || ch == 0x11A3F /* ZANABAZAR SQUARE INITIAL HEAD MARK */
          || ch == 0x11A45 /* ZANABAZAR SQUARE INITIAL DOUBLE-LINED HEAD MARK */
          || ch == 0x11A9E /* SOYOMBO HEAD MARK WITH MOON AND SUN AND TRIPLE FLAME */
          || ch == 0x11A9F /* SOYOMBO HEAD MARK WITH MOON AND SUN AND FLAME */
          || ch == 0x11AA0 /* SOYOMBO HEAD MARK WITH MOON AND SUN */
          || ch == 0x11C70 /* MARCHEN HEAD MARK */)
        attr |= (int64_t) 1 << LBP_BB;

      /* hyphen */
      if (ch == 0x002D /* HYPHEN-MINUS */)
        attr |= (int64_t) 1 << LBP_HY;

      /* contingent break opportunity */
      if (ch == 0xFFFC /* OBJECT REPLACEMENT CHARACTER */)
        attr |= (int64_t) 1 << LBP_CB;

      /* closing parenthesis */
      if (ch == 0x0029 /* RIGHT PARENTHESIS */
          || ch == 0x005D /* RIGHT SQUARE BRACKET */)
        {
          if (unicode_width[ch] != NULL
              && (strcmp (unicode_width[ch], "W") == 0
                  || strcmp (unicode_width[ch], "F") == 0
                  || strcmp (unicode_width[ch], "H") == 0))
            attr |= (int64_t) 1 << LBP_CP2;
          else
            attr |= (int64_t) 1 << LBP_CP1;
        }

      /* closing punctuation */
      if ((unicode_attributes[ch].category[0] == 'P'
           && unicode_attributes[ch].category[1] == 'e'
           && !(attr & (((int64_t) 1 << LBP_CP1) | ((int64_t) 1 << LBP_CP2))))
          || ch == 0x3001 /* IDEOGRAPHIC COMMA */
          || ch == 0x3002 /* IDEOGRAPHIC FULL STOP */
          || ch == 0xFE11 /* PRESENTATION FORM FOR VERTICAL IDEOGRAPHIC COMMA */
          || ch == 0xFE12 /* PRESENTATION FORM FOR VERTICAL IDEOGRAPHIC FULL STOP */
          || ch == 0xFE50 /* SMALL COMMA */
          || ch == 0xFE52 /* SMALL FULL STOP */
          || ch == 0xFF0C /* FULLWIDTH COMMA */
          || ch == 0xFF0E /* FULLWIDTH FULL STOP */
          || ch == 0xFF61 /* HALFWIDTH IDEOGRAPHIC FULL STOP */
          || ch == 0xFF64 /* HALFWIDTH IDEOGRAPHIC COMMA */
          /* Extra characters for compatibility with Unicode LineBreak.txt.  */
          || ch == 0x1325B /* EGYPTIAN HIEROGLYPH O006D */
          || ch == 0x1325C /* EGYPTIAN HIEROGLYPH O006E */
          || ch == 0x1325D /* EGYPTIAN HIEROGLYPH O006F */
          || ch == 0x13282 /* EGYPTIAN HIEROGLYPH O033A */
          || ch == 0x13287 /* EGYPTIAN HIEROGLYPH O036B */
          || ch == 0x13289 /* EGYPTIAN HIEROGLYPH O036D */
          || ch == 0x1337A /* EGYPTIAN HIEROGLYPH V011B */
          || ch == 0x1337B /* EGYPTIAN HIEROGLYPH V011C */
          || ch == 0x13438 /* EGYPTIAN HIEROGLYPH END SEGMENT */
          || ch == 0x145CF /* ANATOLIAN HIEROGLYPH A410A END LOGOGRAM MARK */)
        attr |= (int64_t) 1 << LBP_CL;

      /* exclamation/interrogation */
      if (ch == 0x0021 /* EXCLAMATION MARK */
          || ch == 0x003F /* QUESTION MARK */
          || ch == 0x05C6 /* HEBREW PUNCTUATION NUN HAFUKHA */
          || ch == 0x061B /* ARABIC SEMICOLON */
          || ch == 0x061D /* ARABIC END OF TEXT MARK */
          || ch == 0x061E /* ARABIC TRIPLE DOT PUNCTUATION MARK */
          || ch == 0x061F /* ARABIC QUESTION MARK */
          || ch == 0x06D4 /* ARABIC FULL STOP */
          || ch == 0x07F9 /* NKO EXCLAMATION MARK */
          || ch == 0x0F0D /* TIBETAN MARK SHAD */
          || ch == 0x0F0E /* TIBETAN MARK NYIS SHAD */
          || ch == 0x0F0F /* TIBETAN MARK TSHEG SHAD */
          || ch == 0x0F10 /* TIBETAN MARK NYIS TSHEG SHAD */
          || ch == 0x0F11 /* TIBETAN MARK RIN CHEN SPUNGS SHAD */
          || ch == 0x0F14 /* TIBETAN MARK GTER TSHEG */
          || ch == 0x1802 /* MONGOLIAN COMMA */
          || ch == 0x1803 /* MONGOLIAN FULL STOP */
          || ch == 0x1808 /* MONGOLIAN MANCHU COMMA */
          || ch == 0x1809 /* MONGOLIAN MANCHU FULL STOP */
          || ch == 0x1944 /* LIMBU EXCLAMATION MARK */
          || ch == 0x1945 /* LIMBU QUESTION MARK */
          || ch == 0x2762 /* HEAVY EXCLAMATION MARK ORNAMENT */
          || ch == 0x2763 /* HEAVY HEART EXCLAMATION MARK ORNAMENT */
          || ch == 0x2CF9 /* COPTIC OLD NUBIAN FULL STOP */
          || ch == 0x2CFE /* COPTIC FULL STOP */
          || ch == 0x2E2E /* REVERSED QUESTION MARK */
          || ch == 0x2E53 /* MEDIEVAL EXCLAMATION MARK */
          || ch == 0x2E54 /* MEDIEVAL QUESTION MARK */
          || ch == 0xA60E /* VAI FULL STOP */
          || ch == 0xA876 /* PHAGS-PA MARK SHAD */
          || ch == 0xA877 /* PHAGS-PA MARK DOUBLE SHAD */
          || ch == 0xFE15 /* PRESENTATION FORM FOR VERTICAL EXCLAMATION MARK */
          || ch == 0xFE16 /* PRESENTATION FORM FOR VERTICAL QUESTION MARK */
          || ch == 0xFE56 /* SMALL QUESTION MARK */
          || ch == 0xFE57 /* SMALL EXCLAMATION MARK */
          || ch == 0xFF01 /* FULLWIDTH EXCLAMATION MARK */
          || ch == 0xFF1F /* FULLWIDTH QUESTION MARK */
          || ch == 0x115C4 /* SIDDHAM SEPARATOR DOT */
          || ch == 0x115C5 /* SIDDHAM SEPARATOR BAR */
          || ch == 0x11C71 /* MARCHEN MARK SHAD */)
        attr |= (int64_t) 1 << LBP_EX;

      /* inseparable */
      if (ch == 0x2024 /* ONE DOT LEADER */
          || ch == 0x2025 /* TWO DOT LEADER */
          || ch == 0x2026 /* HORIZONTAL ELLIPSIS */
          || ch == 0x22EF /* MIDLINE HORIZONTAL ELLIPSIS */
          || ch == 0xFE19 /* PRESENTATION FORM FOR VERTICAL HORIZONTAL ELLIPSIS */
          || ch == 0x10AF6 /* MANICHAEAN PUNCTUATION LINE FILLER */)
        attr |= (int64_t) 1 << LBP_IN;

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
          || ch == 0x16FE0 /* TANGUT ITERATION MARK */
          || ch == 0x16FE1 /* NUSHU ITERATION MARK */
          || ch == 0x16FE2 /* OLD CHINESE HOOK MARK */
          || ch == 0x16FE3 /* OLD CHINESE ITERATION MARK */
          || ch == 0x1F679 /* HEAVY INTERROBANG ORNAMENT */
          || ch == 0x1F67A /* SANS-SERIF INTERROBANG ORNAMENT */
          || ch == 0x1F67B /* HEAVY SANS-SERIF INTERROBANG ORNAMENT */
          || strstr (unicode_attributes[ch].name, "HIRAGANA LETTER SMALL ") != NULL
          || strstr (unicode_attributes[ch].name, "KATAKANA LETTER SMALL ") != NULL)
        attr |= (int64_t) 1 << LBP_NS;

      /* opening punctuation */
      if ((unicode_attributes[ch].category[0] == 'P'
           && unicode_attributes[ch].category[1] == 's')
          || ch == 0x00A1 /* INVERTED EXCLAMATION MARK */
          || ch == 0x00BF /* INVERTED QUESTION MARK */
          || ch == 0x2E18 /* INVERTED INTERROBANG */
          /* Extra characters for compatibility with Unicode LineBreak.txt.  */
          || ch == 0x13258 /* EGYPTIAN HIEROGLYPH O006A */
          || ch == 0x13259 /* EGYPTIAN HIEROGLYPH O006B */
          || ch == 0x1325A /* EGYPTIAN HIEROGLYPH O006C */
          || ch == 0x13286 /* EGYPTIAN HIEROGLYPH O036A */
          || ch == 0x13288 /* EGYPTIAN HIEROGLYPH O036C */
          || ch == 0x13379 /* EGYPTIAN HIEROGLYPH V011A */
          || ch == 0x13437 /* EGYPTIAN HIEROGLYPH BEGIN SEGMENT */
          || ch == 0x145CE /* ANATOLIAN HIEROGLYPH A410 BEGIN LOGOGRAM MARK */
          || (ch >= 0x1E95E && ch <= 0x1E95F) /* ADLAM INITIAL EXCLAMATION MARK..ADLAM INITIAL QUESTION MARK */)
        {
          if (unicode_width[ch] != NULL
              && (strcmp (unicode_width[ch], "W") == 0
                  || strcmp (unicode_width[ch], "F") == 0
                  || strcmp (unicode_width[ch], "H") == 0))
            attr |= (int64_t) 1 << LBP_OP2;
          else
            attr |= (int64_t) 1 << LBP_OP1;
        }

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
          || ch == 0x275F /* HEAVY LOW SINGLE COMMA QUOTATION MARK ORNAMENT */
          || ch == 0x2760 /* HEAVY LOW DOUBLE COMMA QUOTATION MARK ORNAMENT */
          || ch == 0x2E00 /* RIGHT ANGLE SUBSTITUTION MARKER */
          || ch == 0x2E01 /* RIGHT ANGLE DOTTED SUBSTITUTION MARKER */
          || ch == 0x2E06 /* RAISED INTERPOLATION MARKER */
          || ch == 0x2E07 /* RAISED DOTTED INTERPOLATION MARKER */
          || ch == 0x2E08 /* DOTTED TRANSPOSITION MARKER */
          || ch == 0x2E0B /* RAISED SQUARE */
          || ch == 0x1F676 /* SANS-SERIF HEAVY DOUBLE TURNED COMMA QUOTATION MARK ORNAMENT  */
          || ch == 0x1F677 /* SANS-SERIF HEAVY DOUBLE COMMA QUOTATION MARK ORNAMENT */
          || ch == 0x1F678 /* SANS-SERIF HEAVY LOW DOUBLE COMMA QUOTATION MARK ORNAMENT */)
        attr |= (int64_t) 1 << LBP_QU;

      /* infix separator (numeric) */
      if (ch == 0x002C /* COMMA */
          || ch == 0x002E /* FULL STOP */
          || ch == 0x003A /* COLON */
          || ch == 0x003B /* SEMICOLON */
          || ch == 0x037E /* GREEK QUESTION MARK */
          || ch == 0x0589 /* ARMENIAN FULL STOP */
          || ch == 0x060C /* ARABIC COMMA */
          || ch == 0x060D /* ARABIC DATE SEPARATOR */
          || ch == 0x07F8 /* NKO COMMA */
          || ch == 0x2044 /* FRACTION SLASH */
          || ch == 0xFE10 /* PRESENTATION FORM FOR VERTICAL COMMA */
          || ch == 0xFE13 /* PRESENTATION FORM FOR VERTICAL COLON */
          || ch == 0xFE14 /* PRESENTATION FORM FOR VERTICAL SEMICOLON */)
        attr |= (int64_t) 1 << LBP_IS;

      /* numeric */
      if ((unicode_attributes[ch].category[0] == 'N'
           && unicode_attributes[ch].category[1] == 'd'
           && strstr (unicode_attributes[ch].name, "FULLWIDTH") == NULL)
          || ch == 0x066B /* ARABIC DECIMAL SEPARATOR */
          || ch == 0x066C /* ARABIC THOUSANDS SEPARATOR */)
        attr |= (int64_t) 1 << LBP_NU;

      /* postfix (numeric) */
      if (ch == 0x0025 /* PERCENT SIGN */
          || ch == 0x00A2 /* CENT SIGN */
          || ch == 0x00B0 /* DEGREE SIGN */
          || ch == 0x060B /* AFGHANI SIGN */
          || ch == 0x066A /* ARABIC PERCENT SIGN */
          || ch == 0x2030 /* PER MILLE SIGN */
          || ch == 0x2031 /* PER TEN THOUSAND SIGN */
          || ch == 0x2032 /* PRIME */
          || ch == 0x2033 /* DOUBLE PRIME */
          || ch == 0x2034 /* TRIPLE PRIME */
          || ch == 0x2035 /* REVERSED PRIME */
          || ch == 0x2036 /* REVERSED DOUBLE PRIME */
          || ch == 0x2037 /* REVERSED TRIPLE PRIME */
          || ch == 0x20A7 /* PESETA SIGN */
          || ch == 0x20BB /* NORDIC MARK SIGN */
          || ch == 0x2103 /* DEGREE CELSIUS */
          || ch == 0x2109 /* DEGREE FAHRENHEIT */
          || ch == 0xFDFC /* RIAL SIGN */
          || ch == 0xFE6A /* SMALL PERCENT SIGN */
          || ch == 0xFF05 /* FULLWIDTH PERCENT SIGN */
          || ch == 0xFFE0 /* FULLWIDTH DIGIT ZERO */
          /* Extra characters for compatibility with Unicode LineBreak.txt.  */
          || ch == 0x0609 /* ARABIC-INDIC PER MILLE SIGN */
          || ch == 0x060A /* ARABIC-INDIC PER TEN THOUSAND SIGN */
          || ch == 0x09F2 /* BENGALI RUPEE MARK */
          || ch == 0x09F3 /* BENGALI RUPEE SIGN */
          || ch == 0x09F9 /* BENGALI CURRENCY DENOMINATOR SIXTEEN */
          || ch == 0x0D79 /* MALAYALAM DATE MARK */
          || ch == 0x20B6 /* LIVRE TOURNOIS SIGN */
          || ch == 0x20BE /* LARI SIGN */
          || ch == 0x20C0 /* SOM SIGN */
          || ch == 0xA838 /* NORTH INDIC RUPEE MARK */
          || (ch >= 0x11FDD && ch <= 0x11FE0) /* TAMIL SIGN KAACU..TAMIL SIGN VARAAKAN */
          || ch == 0x1ECAC /* INDIC SIYAQ PLACEHOLDER */
          || ch == 0x1ECB0 /* INDIC SIYAQ RUPEE MARK */)
        attr |= (int64_t) 1 << LBP_PO;

      /* prefix (numeric) */
      if ((unicode_attributes[ch].category[0] == 'S'
           && unicode_attributes[ch].category[1] == 'c')
          || ch == 0x002B /* PLUS SIGN */
          || ch == 0x005C /* REVERSE SOLIDUS */
          || ch == 0x00B1 /* PLUS-MINUS SIGN */
          || ch == 0x2116 /* NUMERO SIGN */
          || ch == 0x2212 /* MINUS SIGN */
          || ch == 0x2213 /* MINUS-OR-PLUS SIGN */)
        if (!(attr & ((int64_t) 1 << LBP_PO)))
          attr |= (int64_t) 1 << LBP_PR;

      /* symbols allowing breaks */
      if (ch == 0x002F /* SOLIDUS */)
        attr |= (int64_t) 1 << LBP_SY;

      if (ch >= 0xAC00 && ch <= 0xD7A3 && ((ch - 0xAC00) % 28) == 0)
        attr |= (int64_t) 1 << LBP_H2;

      if (ch >= 0xAC00 && ch <= 0xD7A3 && ((ch - 0xAC00) % 28) != 0)
        attr |= (int64_t) 1 << LBP_H3;

      if ((ch >= 0x05D0 && ch <= 0x05F2) || ch == 0xFB1D
          || (ch >= 0xFB1F && ch <= 0xFB28) || (ch >= 0xFB2A && ch <= 0xFB4F))
        attr |= (int64_t) 1 << LBP_HL;

      if ((ch >= 0x1100 && ch <= 0x115F) || (ch >= 0xA960 && ch <= 0xA97C))
        attr |= (int64_t) 1 << LBP_JL;

      if ((ch >= 0x1160 && ch <= 0x11A7) || (ch >= 0xD7B0 && ch <= 0xD7C6))
        attr |= (int64_t) 1 << LBP_JV;

      if ((ch >= 0x11A8 && ch <= 0x11FF) || (ch >= 0xD7CB && ch <= 0xD7FB))
        attr |= (int64_t) 1 << LBP_JT;

      if (is_property_regional_indicator (ch))
        attr |= (int64_t) 1 << LBP_RI;

      /* complex context (South East Asian) */
      if (((unicode_attributes[ch].category[0] == 'C'
            && unicode_attributes[ch].category[1] == 'f')
           || (unicode_attributes[ch].category[0] == 'L'
               && (unicode_attributes[ch].category[1] == 'm'
                   || unicode_attributes[ch].category[1] == 'o'))
           || (unicode_attributes[ch].category[0] == 'M'
               && (unicode_attributes[ch].category[1] == 'c'
                   || unicode_attributes[ch].category[1] == 'n')
               && ch != 0x1A7F /* TAI THAM COMBINING CRYPTOGRAMMIC DOT */)
           /* Extra characters for compatibility with Unicode LineBreak.txt.  */
           || ch == 0x109E /* MYANMAR SYMBOL SHAN ONE */
           || ch == 0x109F /* MYANMAR SYMBOL SHAN EXCLAMATION */
           || ch == 0x19DA /* NEW TAI LUE THAM DIGIT ONE */
           || ch == 0x19DE /* NEW TAI LUE SIGN LAE */
           || ch == 0x19DF /* NEW TAI LUE SIGN LAEV */
           || (ch >= 0x1AA0 && ch <= 0x1AAD) /* TAI THAM SIGN */
           || (ch >= 0xA9E0 && ch <= 0xA9EF) /* Myanmar */
           || (ch >= 0xA9FA && ch <= 0xA9FE) /* Myanmar */
           || (ch >= 0xAA77 && ch <= 0xAA79) /* MYANMAR SYMBOL AITON */
           || (ch >= 0xAADE && ch <= 0xAADF) /* TAI VIET SYMBOL */
           || (ch >= 0x1173A && ch <= 0x1173B) /* Ahom */
           || (ch >= 0x1173F && ch <= 0x11746) /* Ahom */)
          && ((ch >= 0x0E00 && ch <= 0x0EFF) /* Thai, Lao */
              || (ch >= 0x1000 && ch <= 0x109F) /* Myanmar */
              || (ch >= 0x1780 && ch <= 0x17FF) /* Khmer */
              || (ch >= 0x1950 && ch <= 0x19DF) /* Tai Le, New Tai Lue */
              || (ch >= 0x1A20 && ch <= 0x1AAF) /* Tai Tham */
              || (ch >= 0xA9E0 && ch <= 0xA9EF) /* Myanmar */
              || (ch >= 0xA9FA && ch <= 0xA9FE) /* Myanmar */
              || (ch >= 0xAA60 && ch <= 0xAADF) /* Myanmar Extended-A, Tai Viet */
              || (ch >= 0x11700 && ch <= 0x1171A) /* Ahom */
              || (ch >= 0x1171D && ch <= 0x1172B) /* Ahom */
              || (ch >= 0x1173A && ch <= 0x1173B) /* Ahom */
              || (ch >= 0x1173F && ch <= 0x11746) /* Ahom */))
        attr |= (int64_t) 1 << LBP_SA;

      /* attached characters and combining marks */
      if ((unicode_attributes[ch].category[0] == 'M'
           && (unicode_attributes[ch].category[1] == 'c'
               || unicode_attributes[ch].category[1] == 'e'
               || unicode_attributes[ch].category[1] == 'n'))
          || (unicode_attributes[ch].category[0] == 'C'
              && (unicode_attributes[ch].category[1] == 'c'
                  || unicode_attributes[ch].category[1] == 'f')
              && ch != 0x0890 /* ARABIC POUND MARK ABOVE */
              && ch != 0x0891 /* ARABIC PIASTRE MARK ABOVE */
              && ch != 0x08E2 /* ARABIC DISPUTED END OF AYAH */
              && ch != 0x110BD /* KAITHI NUMBER SIGN */
              && ch != 0x110CD /* KAITHI NUMBER SIGN ABOVE */
              && ch != 0x13437 /* EGYPTIAN HIEROGLYPH BEGIN SEGMENT */
              && ch != 0x13438 /* EGYPTIAN HIEROGLYPH END SEGMENT */)
          || ch == 0x3035 /* VERTICAL KANA REPEAT MARK LOWER HALF */)
        if (!(attr & (((int64_t) 1 << LBP_BK) | ((int64_t) 1 << LBP_CR) | ((int64_t) 1 << LBP_LF) | ((int64_t) 1 << LBP_BA) | ((int64_t) 1 << LBP_GL) | ((int64_t) 1 << LBP_SA) | ((int64_t) 1 << LBP_WJ) | ((int64_t) 1 << LBP_ZW) | ((int64_t) 1 << LBP_ZWJ))))
          attr |= (int64_t) 1 << LBP_CM;

      /* ideographic */
      if (ch == 0x231A /* WATCH */
          || ch == 0x231B /* HOURGLASS */
          || ch == 0x23F0 /* ALARM CLOCK */
          || ch == 0x23F1 /* STOPWATCH */
          || ch == 0x23F2 /* TIMER CLOCK */
          || ch == 0x23F3 /* HOURGLASS WITH FLOWING SAND */
          || ch == 0x2600 /* BLACK SUN WITH RAYS */
          || ch == 0x2601 /* CLOUD */
          || ch == 0x2602 /* UMBRELLA */
          || ch == 0x2603 /* SNOWMAN */
          || ch == 0x2614 /* UMBRELLA WITH RAIN DROPS */
          || ch == 0x2615 /* HOT BEVERAGE */
          || ch == 0x2618 /* SHAMROCK */
          || ch == 0x261A /* BLACK LEFT POINTING INDEX */
          || ch == 0x261B /* BLACK RIGHT POINTING INDEX */
          || ch == 0x261C /* WHITE LEFT POINTING INDEX */
          || ch == 0x261D /* WHITE UP POINTING INDEX */
          || ch == 0x261E /* WHITE RIGHT POINTING INDEX */
          || ch == 0x261F /* WHITE DOWN POINTING INDEX */
          || ch == 0x2639 /* WHITE FROWNING FACE */
          || ch == 0x263A /* WHITE SMILING FACE */
          || ch == 0x263B /* BLACK SMILING FACE */
          || ch == 0x2668 /* HOT SPRINGS */
          || ch == 0x267F /* WHEELCHAIR SYMBOL */
          || ch == 0x26BD /* SOCCER BALL */
          || ch == 0x26BE /* BASEBALL */
          || ch == 0x26BF /* SQUARED KEY */
          || ch == 0x26C0 /* WHITE DRAUGHTS MAN */
          || ch == 0x26C1 /* WHITE DRAUGHTS KING */
          || ch == 0x26C2 /* BLACK DRAUGHTS MAN */
          || ch == 0x26C3 /* BLACK DRAUGHTS KING */
          || ch == 0x26C4 /* SNOWMAN WITHOUT SNOW */
          || ch == 0x26C5 /* SUN BEHIND CLOUD */
          || ch == 0x26C6 /* RAIN */
          || ch == 0x26C7 /* BLACK SNOWMAN */
          || ch == 0x26C8 /* THUNDER CLOUD AND RAIN */
          || ch == 0x26CD /* DISABLED CAR */
          || ch == 0x26CF /* PICK */
          || ch == 0x26D0 /* CAR SLIDING */
          || ch == 0x26D1 /* HELMET WITH WHITE CROSS */
          || ch == 0x26D3 /* CHAINS */
          || ch == 0x26D4 /* NO ENTRY */
          || ch == 0x26D8 /* BLACK LEFT LANE MERGE */
          || ch == 0x26D9 /* WHITE LEFT LANE MERGE */
          || ch == 0x26DC /* LEFT CLOSED ENTRY */
          || ch == 0x26DF /* BLACK TRUCK */
          || ch == 0x26E0 /* RESTRICTED LEFT ENTRY-1 */
          || ch == 0x26E1 /* RESTRICTED LEFT ENTRY-2 */
          || ch == 0x26EA /* CHURCH */
          || ch == 0x26F1 /* UMBRELLA ON GROUND */
          || ch == 0x26F2 /* FOUNTAIN */
          || ch == 0x26F3 /* FLAG IN HOLE */
          || ch == 0x26F4 /* FERRY */
          || ch == 0x26F5 /* SAILBOAT */
          || ch == 0x26F7 /* SKIER */
          || ch == 0x26F8 /* ICE SKATE */
          || ch == 0x26F9 /* PERSON WITH BALL */
          || ch == 0x26FA /* TENT */
          || ch == 0x26FD /* FUEL PUMP */
          || ch == 0x26FE /* CUP ON BLACK SQUARE */
          || ch == 0x26FF /* WHITE FLAG WITH HORIZONTAL MIDDLE BLACK STRIPE */
          || ch == 0x2700 /* BLACK SAFETY SCISSORS */
          || ch == 0x2701 /* UPPER BLADE SCISSORS */
          || ch == 0x2702 /* BLACK SCISSORS */
          || ch == 0x2703 /* LOWER BLADE SCISSORS */
          || ch == 0x2704 /* WHITE SCISSORS */
          || ch == 0x2708 /* AIRPLANE */
          || ch == 0x2709 /* ENVELOPE */
          || ch == 0x270A /* RAISED FIST */
          || ch == 0x270B /* RAISED HAND */
          || ch == 0x270C /* VICTORY HAND */
          || ch == 0x270D /* WRITING HAND */
          || ch == 0x2764 /* HEAVY BLACK HEART */
          || (ch >= 0x2E80 && ch <= 0x2FFF) /* CJK RADICAL, KANGXI RADICAL, IDEOGRAPHIC DESCRIPTION */
          || (ch >= 0x3040 && ch <= 0x309F) /* HIRAGANA */
          || (ch >= 0x30A0 && ch <= 0x30FF) /* KATAKANA */
          || (ch >= 0x3400 && ch <= 0x4DBF) /* CJK Ideograph Extension A */
          || (ch >= 0x4E00 && ch <= 0x9FFF) /* CJK Ideograph */
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
              && !(attr & (((int64_t) 1 << LBP_BA) | ((int64_t) 1 << LBP_CM) | ((int64_t) 1 << LBP_NS) | ((int64_t) 1 << LBP_OP1) | ((int64_t) 1 << LBP_OP2) | ((int64_t) 1 << LBP_CL) | ((int64_t) 1 << LBP_CP1) | ((int64_t) 1 << LBP_CP2))))
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
          || ch == 0xFFE4 /* FULLWIDTH BROKEN BAR */
          /* Extra characters for compatibility with Unicode LineBreak.txt.  */
          || ch == 0xFF66 /* Halfwidth Katakana */
          || (ch >= 0xFF71 && ch <= 0xFF9D) /* Halfwidth Katakana */
          || (ch >= 0xFFA0 && ch <= 0xFFBE) /* Halfwidth Hangul */
          || (ch >= 0xFFC2 && ch <= 0xFFC7) /* Halfwidth Hangul */
          || (ch >= 0xFFCA && ch <= 0xFFCF) /* Halfwidth Hangul */
          || (ch >= 0xFFD2 && ch <= 0xFFD7) /* Halfwidth Hangul */
          || (ch >= 0xFFDA && ch <= 0xFFDC) /* Halfwidth Hangul */
          || (ch >= 0x17000 && ch <= 0x187F7) /* Tangut Ideograph */
          || (ch >= 0x18800 && ch <= 0x18AFF) /* Tangut Ideograph */
          || (ch >= 0x18D00 && ch <= 0x18D08) /* Tangut Ideograph Supplement */
          || (ch >= 0x1B000 && ch <= 0x1B001) /* Kana Supplement */
          || (ch >= 0x1B002 && ch <= 0x1B122) /* Hentaigana, archaic Hiragana/Katakana */
          || (ch >= 0x1B170 && ch <= 0x1B2FB) /* Nushu */
          || (ch >= 0x1F000 && ch <= 0x1F02B) /* Mahjong Tiles */
          || (ch >= 0x1F030 && ch <= 0x1F093) /* Domino Tiles */
          || (ch >= 0x1F0A0 && ch <= 0x1F0F5) /* Playing Cards */
          || (ch >= 0x1F10D && ch <= 0x1F10F) /* Circled Symbols */
          || (ch >= 0x1F16D && ch <= 0x1F16F) /* Circled Symbols */
          || ch == 0x1F1AD /* MASK WORK SYMBOL */
          || (ch >= 0x1F200 && ch <= 0x1F248) /* Enclosed Ideographic Supplement */
          || (ch >= 0x1F250 && ch <= 0x1F251) /* Enclosed Ideographic Supplement */
          || (ch >= 0x1F260 && ch <= 0x1F265) /* Rounded Symbols */
          || (ch >= 0x1F300 && ch <= 0x1F5FF /* Miscellaneous Symbols and Pictographs */
              && ch != 0x1F3B5 && ch != 0x1F3B6 && ch != 0x1F3BC
              && ch != 0x1F4A0 && ch != 0x1F4A2 && ch != 0x1F4A4
              && ch != 0x1F4AF && ch != 0x1F4B1 && ch != 0x1F4B2
              && !(ch >= 0x1F39C && ch <= 0x1F39D)
              && !(ch >= 0x1F3FB && ch <= 0x1F3FF)
              && !(ch >= 0x1F500 && ch <= 0x1F506)
              && !(ch >= 0x1F517 && ch <= 0x1F524)
              && !(ch >= 0x1F532 && ch <= 0x1F549)
              && !(ch >= 0x1F5D4 && ch <= 0x1F5DB)
              && !(ch >= 0x1F5F4 && ch <= 0x1F5F9))
          || (ch >= 0x1F600 && ch <= 0x1F64F) /* Emoticons */
          || (ch >= 0x1F680 && ch <= 0x1F6DF) /* Transport and Map Symbols */
          || (ch >= 0x1F6E0 && ch <= 0x1F6EC) /* Transport and Map Symbols */
          || (ch >= 0x1F6F0 && ch <= 0x1F6FC) /* Transport and Map Symbols */
          || (ch >= 0x1F7D5 && ch <= 0x1F7D8) /* Circled polygons */
          || (ch >= 0x1F7E0 && ch <= 0x1F7EB) /* Large circles */
          || ch == 0x1F7F0 /* Heavy equals sign */
          || (ch >= 0x1F8B0 && ch <= 0x1F8B1) /* Curved arrows */
          || (ch >= 0x1F90C && ch <= 0x1F9FF) /* Supplemental Symbols and Pictographs */
          || (ch >= 0x1FA60 && ch <= 0x1FA6D) /* Xiangqi pieces */
          || (ch >= 0x1FA70 && ch <= 0x1FA74) /* Emoticons */
          || (ch >= 0x1FA78 && ch <= 0x1FA7C) /* Medical pictographs */
          || (ch >= 0x1FA80 && ch <= 0x1FA86) /* Pictographs */
          || (ch >= 0x1FA90 && ch <= 0x1FAAC) /* Pictographs */
          || (ch >= 0x1FAB0 && ch <= 0x1FABA) /* Pictographs */
          || (ch >= 0x1FAC0 && ch <= 0x1FAC2) /* Pictographs */
          || (ch >= 0x1FAD0 && ch <= 0x1FAD9) /* Pictographs */
          || (ch >= 0x1FAE0 && ch <= 0x1FAE7) /* Pictographs */
          || (ch >= 0x2A6D7 && ch <= 0x2A6DF) /* CJK Ideograph Extension B */
          || (ch >= 0x2A700 && ch <= 0x2B738) /* CJK Ideograph Extension C */
          || (ch >= 0x2B740 && ch <= 0x2B81D) /* CJK Ideograph Extension D */
          || (ch >= 0x2B820 && ch <= 0x2CEAF) /* CJK Ideograph Extension E */
          || (ch >= 0x2CEB0 && ch <= 0x2EBE0) /* CJK Ideograph Extension F */
          || (ch >= 0x30000 && ch <= 0x3134A) /* CJK Ideograph Extension G */)
        if (!(attr & (((int64_t) 1 << LBP_NS) | ((int64_t) 1 << LBP_CM) | ((int64_t) 1 << LBP_EB))))
          {
            /* ambiguous (ideograph) ? */
            if ((unicode_width[ch] != NULL
                 && unicode_width[ch][0] == 'A'
                 && ch >= 0x2000
                 && ch != 0x2614
                 && ch != 0x2615
                 && ch != 0x261C
                 && ch != 0x261E
                 && ch != 0x2668
                 && ch != 0x26BE
                 && ch != 0x26BF
                 && !(ch >= 0x26C4 && ch <= 0x26C8)
                 && ch != 0x26CD
                 && ch != 0x26CF
                 && ch != 0x26D0
                 && ch != 0x26D1
                 && ch != 0x26D3
                 && ch != 0x26D4
                 && ch != 0x26D8
                 && ch != 0x26D9
                 && ch != 0x26DC
                 && ch != 0x26DF
                 && ch != 0x26E0
                 && ch != 0x26E1
                 && ch != 0x26EA
                 && !(ch >= 0x26F1 && ch <= 0x26F5)
                 && !(ch >= 0x26F7 && ch <= 0x26FA)
                 && !(ch >= 0x26FD && ch <= 0x26FF))
                || ch == 0x24EA /* CIRCLED DIGIT ZERO */
                || (ch >= 0x2780 && ch <= 0x2793) /* DINGBAT ... CIRCLED DIGIT ... */)
              attr |= (int64_t) 1 << LBP_AI;
            else
              attr |= (int64_t) 1 << LBP_ID1;
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
          || ch == 0x0604 /* ARABIC SIGN SAMVAT */
          || ch == 0x0605 /* ARABIC NUMBER MARK ABOVE */
          || ch == 0x06DD /* ARABIC END OF AYAH */
          || ch == 0x070F /* SYRIAC ABBREVIATION MARK */
          || ch == 0x08E2 /* ARABIC DISPUTED END OF AYAH */
          || ch == 0x2061 /* FUNCTION APPLICATION */
          || ch == 0x2062 /* INVISIBLE TIMES */
          || ch == 0x2063 /* INVISIBLE SEPARATOR */
          || ch == 0x2064 /* INVISIBLE PLUS */
          /* Extra characters for compatibility with Unicode LineBreak.txt.  */
          || ch == 0x0890 /* ARABIC POUND MARK ABOVE */
          || ch == 0x0891 /* ARABIC PIASTRE MARK ABOVE */
          || ch == 0x110BD /* KAITHI NUMBER SIGN */
          || ch == 0x110CD /* KAITHI NUMBER SIGN ABOVE */)
        if (!(attr & (((int64_t) 1 << LBP_GL) | ((int64_t) 1 << LBP_B2) | ((int64_t) 1 << LBP_BA) | ((int64_t) 1 << LBP_BB) | ((int64_t) 1 << LBP_HY) | ((int64_t) 1 << LBP_CB) | ((int64_t) 1 << LBP_CL) | ((int64_t) 1 << LBP_CP1) | ((int64_t) 1 << LBP_CP2) | ((int64_t) 1 << LBP_EX) | ((int64_t) 1 << LBP_IN) | ((int64_t) 1 << LBP_NS) | ((int64_t) 1 << LBP_OP1) | ((int64_t) 1 << LBP_OP2) | ((int64_t) 1 << LBP_QU) | ((int64_t) 1 << LBP_IS) | ((int64_t) 1 << LBP_NU) | ((int64_t) 1 << LBP_PO) | ((int64_t) 1 << LBP_PR) | ((int64_t) 1 << LBP_SY) | ((int64_t) 1 << LBP_H2) | ((int64_t) 1 << LBP_H3) | ((int64_t) 1 << LBP_HL) | ((int64_t) 1 << LBP_JL) | ((int64_t) 1 << LBP_JV) | ((int64_t) 1 << LBP_JT) | ((int64_t) 1 << LBP_RI) | ((int64_t) 1 << LBP_SA) | ((int64_t) 1 << LBP_ID1) | ((int64_t) 1 << LBP_ID2) | ((int64_t) 1 << LBP_EB) | ((int64_t) 1 << LBP_EM)))
            && ch != 0x3035 /* VERTICAL KANA REPEAT MARK LOWER HALF */)
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
                || ch == 0x24EA /* CIRCLED DIGIT ZERO */
                || (ch >= 0x2780 && ch <= 0x2793) /* DINGBAT ... CIRCLED DIGIT ... */
                /* Extra characters for compatibility with Unicode LineBreak.txt.  */
                || ch == 0x2155 /* VULGAR FRACTION ONE FIFTH */
                || ch == 0x2574 /* BOX DRAWINGS LIGHT LEFT */
                || ch == 0x2616 /* WHITE SHOGI PIECE */
                || ch == 0x2617 /* BLACK SHOGI PIECE */
                || ch == 0x2757 /* HEAVY EXCLAMATION MARK SYMBOL */
                || ch == 0x2B55 /* HEAVY LARGE CIRCLE */
                || ch == 0x1F10B /* DINGBAT CIRCLED SANS-SERIF DIGIT ZERO */
                || ch == 0x1F18E /* NEGATIVE SQUARED AB */
                || (ch >= 0x1F191 && ch <= 0x1F19A) /* SQUARED CL..SQUARED VS */
                || ch == 0x1F10C /* DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT ZERO */)
              attr |= (int64_t) 1 << LBP_AI;
            else
              attr |= (int64_t) 1 << LBP_AL;
            attr &= ~((int64_t) 1 << LBP_CM);
          }
    }
  else
    {
      /* Unassigned character.  */
      if ((ch >= 0x3400 && ch <= 0x4DBF) /* CJK Unified Ideographs Extension A */
          || (ch >= 0x4E00 && ch <= 0x9FFF) /* CJK Unified Ideographs */
          || (ch >= 0xF900 && ch <= 0xFAFF) /* CJK Compatibility Ideographs */
          || (ch >= 0x1F02C && ch <= 0x1F02F) /* reserved */
          || (ch >= 0x1F094 && ch <= 0x1F09F) /* reserved */
          || (ch >= 0x1F0AF && ch <= 0x1F0B0) /* reserved */
          || ch == 0x1F0C0 /* reserved */
          || ch == 0x1F0D0 /* reserved */
          || (ch >= 0x1F0F6 && ch <= 0x1F0FF) /* reserved */
          || (ch >= 0x1F10D && ch <= 0x1F10F) /* reserved */
          || ch == 0x1F12F /* reserved */
          || (ch >= 0x1F16C && ch <= 0x1F16F) /* reserved */
          || (ch >= 0x1F1AD && ch <= 0x1F1E5) /* reserved */
          || (ch >= 0x1F203 && ch <= 0x1F20F) /* reserved */
          || (ch >= 0x1F23C && ch <= 0x1F23F) /* reserved */
          || (ch >= 0x1F249 && ch <= 0x1F24F) /* reserved */
          || (ch >= 0x1F252 && ch <= 0x1F2FF) /* reserved */
          || (ch >= 0x1F6D3 && ch <= 0x1F6DF) /* reserved */
          || (ch >= 0x1F6ED && ch <= 0x1F6EF) /* reserved */
          || (ch >= 0x1F6F7 && ch <= 0x1F6FF) /* reserved */
          || (ch >= 0x1F774 && ch <= 0x1F77F) /* reserved */
          || (ch >= 0x1F7D5 && ch <= 0x1F7FF) /* reserved */
          || (ch >= 0x1F80C && ch <= 0x1F80F) /* reserved */
          || (ch >= 0x1F848 && ch <= 0x1F84F) /* reserved */
          || (ch >= 0x1F85A && ch <= 0x1F85F) /* reserved */
          || (ch >= 0x1F888 && ch <= 0x1F88F) /* reserved */
          || (ch >= 0x1F8AE && ch <= 0x1F90F) /* reserved */
          || ch == 0x1F91F /* reserved */
          || ch == 0x1F93F /* reserved */
          || (ch >= 0x1F928 && ch <= 0x1F92F) /* reserved */
          || (ch >= 0x1F931 && ch <= 0x1F932) /* reserved */
          || (ch >= 0x1F94C && ch <= 0x1F94F) /* reserved */
          || (ch >= 0x1F95F && ch <= 0x1F97F) /* reserved */
          || (ch >= 0x1F992 && ch <= 0x1F9BF) /* reserved */
          || (ch >= 0x1F9C1 && ch <= 0x1FB92) /* reserved */
          || (ch >= 0x1FB94 && ch <= 0x1FBCA) /* reserved */
          || (ch >= 0x1FBF0 && ch <= 0x1FBF9) /* reserved */
          || (ch >= 0x1FC00 && ch <= 0x1FFFD) /* reserved */
          || (ch >= 0x20000 && ch <= 0x2A6FF) /* CJK Unified Ideographs Extension B */
          || (ch >= 0x2A700 && ch <= 0x2F7FF) /* CJK Unified Ideographs Extension C,
                                                 Supplementary Ideographic Plane (Plane 2) outside of blocks */
          || (ch >= 0x2F800 && ch <= 0x2FFFD) /* CJK Compatibility Ideographs Supplement,
                                                 Supplementary Ideographic Plane (Plane 2) outside of blocks */
          || (ch >= 0x30000 && ch <= 0x3FFFD) /* Tertiary Ideographic Plane (Plane 3) outside of blocks */)
        {
          if (is_property_extended_pictographic (ch))
            attr |= (int64_t) 1 << LBP_ID2;
          else
            attr |= (int64_t) 1 << LBP_ID1;
        }
    }

  if (attr == 0)
    /* unknown */
    attr |= (int64_t) 1 << LBP_XX;

  return attr;
}

/* Output the line breaking properties in a human readable format.  */
static void
debug_output_lbp (FILE *stream)
{
  unsigned int i;

  for (i = 0; i < 0x110000; i++)
    {
      int64_t attr = get_lbp (i);
      if (attr != (int64_t) 1 << LBP_XX)
        {
          fprintf (stream, "0x%04X", i);
#define PRINT_BIT(attr,bit) \
  if (attr & ((int64_t) 1 << bit)) fprintf (stream, " " #bit);
#define PRINT_BIT_ALT(attr,bit,name) \
  if (attr & ((int64_t) 1 << bit)) fprintf (stream, " " #name);
          PRINT_BIT(attr,LBP_BK);
          PRINT_BIT(attr,LBP_CR);
          PRINT_BIT(attr,LBP_LF);
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
          PRINT_BIT_ALT(attr,LBP_CP1,LBP_CP);
          PRINT_BIT_ALT(attr,LBP_CP2,LBP_CP);
          PRINT_BIT(attr,LBP_EX);
          PRINT_BIT(attr,LBP_IN);
          PRINT_BIT(attr,LBP_NS);
          PRINT_BIT_ALT(attr,LBP_OP1,LBP_OP);
          PRINT_BIT_ALT(attr,LBP_OP2,LBP_OP);
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
          PRINT_BIT(attr,LBP_HL);
          PRINT_BIT_ALT(attr,LBP_ID1,LBP_ID);
          PRINT_BIT_ALT(attr,LBP_ID2,LBP_ID);
          PRINT_BIT(attr,LBP_JL);
          PRINT_BIT(attr,LBP_JV);
          PRINT_BIT(attr,LBP_JT);
          PRINT_BIT(attr,LBP_RI);
          PRINT_BIT(attr,LBP_SA);
          PRINT_BIT(attr,LBP_ZWJ);
          PRINT_BIT(attr,LBP_EB);
          PRINT_BIT(attr,LBP_EM);
          PRINT_BIT(attr,LBP_XX);
#undef PRINT_BIT_ALT
#undef PRINT_BIT
          fprintf (stream, "\n");
        }
    }
}

static void
debug_output_lbrk_tables (const char *filename)
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
      TRY(LBP_CR)
      TRY(LBP_LF)
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
      TRY(LBP_CP)
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
      TRY(LBP_HL)
      TRY(LBP_ID)
      TRY(LBP_JL)
      TRY(LBP_JV)
      TRY(LBP_JT)
      TRY(LBP_RI)
      TRY(LBP_SA)
      TRY(LBP_ZWJ)
      TRY(LBP_EB)
      TRY(LBP_EM)
      TRY(LBP_XX)
#undef TRY
      else if (strcmp (field1, "NL") == 0) value = LBP_BK;
      else if (strcmp (field1, "SG") == 0) value = LBP_XX;
      else if (strcmp (field1, "CJ") == 0) value = LBP_NS;
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
          PRINT_BIT(attr,LBP_CR);
          PRINT_BIT(attr,LBP_LF);
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
          PRINT_BIT(attr,LBP_CP);
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
          PRINT_BIT(attr,LBP_HL);
          PRINT_BIT(attr,LBP_ID);
          PRINT_BIT(attr,LBP_JL);
          PRINT_BIT(attr,LBP_JV);
          PRINT_BIT(attr,LBP_JT);
          PRINT_BIT(attr,LBP_RI);
          PRINT_BIT(attr,LBP_SA);
          PRINT_BIT(attr,LBP_ZWJ);
          PRINT_BIT(attr,LBP_EB);
          PRINT_BIT(attr,LBP_EM);
          PRINT_BIT(attr,LBP_XX);
#undef PRINT_BIT
          fprintf (stream, "\n");
        }
    }
}

static void
debug_output_org_lbrk_tables (const char *filename)
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

/* Given an enum value LBP_..., returns its name "LBP_..." as a string.  */
static const char *
lbp_value_to_string (unsigned int value)
{
  const char *value_string;
  switch (value)
    {
#define CASE(x) case x: value_string = #x; break;
      CASE(LBP_BK);
      CASE(LBP_CR);
      CASE(LBP_LF);
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
      CASE(LBP_CP1);
      CASE(LBP_CP2);
      CASE(LBP_EX);
      CASE(LBP_IN);
      CASE(LBP_NS);
      CASE(LBP_OP1);
      CASE(LBP_OP2);
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
      CASE(LBP_HL);
      CASE(LBP_ID1);
      CASE(LBP_ID2);
      CASE(LBP_JL);
      CASE(LBP_JV);
      CASE(LBP_JT);
      CASE(LBP_RI);
      CASE(LBP_SA);
      CASE(LBP_ZWJ);
      CASE(LBP_EB);
      CASE(LBP_EM);
      CASE(LBP_XX);
#undef CASE
      default:
        abort ();
    }
  return value_string;
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
      int64_t attr = get_lbp (i);

      /* Now attr should contain exactly one bit.  */
      assert (attr != 0 && (attr & (attr - 1)) == 0);

      if (attr != (int64_t) 1 << LBP_XX)
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
  fprintf (stream1, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream1, "    int level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream1, "    unsigned char level3[%zu << %d];\n", t.level3_size, t.p);
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
      if (offset == 0)
        fprintf (stream2, " %5d", -1);
      else
        fprintf (stream2, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream2, ",");
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
      if (offset == 0)
        fprintf (stream2, " %5d", -1);
      else
        fprintf (stream2, " %5zu",
                 (offset - level3_offset) / sizeof (unsigned char));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream2, ",");
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
      if (i > 0 && (i % 8) == 0)
        fprintf (stream2, "\n   ");
      fprintf (stream2, " %s%s", lbp_value_to_string (value),
               (i+1 < t.level3_size << t.p ? "," : ""));
    }
  if (t.level3_size << t.p > 8)
    fprintf (stream2, "\n ");
  fprintf (stream2, " }\n");
  fprintf (stream2, "};\n");
}

static void
output_lbrk_tables (const char *filename1, const char *filename2, const char *version)
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
      fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
               version);
      fprintf (stream, "\n");

      fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
      fprintf (stream, "\n");
      output_library_license (stream, false);
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

static void
output_lbrk_rules_as_tables (const char *filename, const char *version)
{
  FILE *stream;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Table that encodes several line breaking rules.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2001-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, false);
  fprintf (stream, "\n");

  fprintf (stream, "#include <config.h>\n");
  fprintf (stream, "\n");
  fprintf (stream, "/* Specification.  */\n");
  fprintf (stream, "#include \"unilbrk/lbrktables.h\"\n");
  fprintf (stream, "\n");
  fprintf (stream, "/* Define unilbrkprop, table of line breaking properties.  */\n");
  fprintf (stream, "#include \"unilbrk/lbrkprop2.h\"\n");
  fprintf (stream, "\n");

  /* LBP_* table indices are in the range 0 .. NLBP-1.  */
  const unsigned int NLBP = 33;

  unsigned int before;
  unsigned int after;
  /* Describe the table cell (before, after).  */
  struct table_cell
  {
    /* Break prohibited when no spaces, i.e. in    before ÷ after  */
    bool prohibited_no_sp;
    /* Break prohibited with spaces, i.e. in   before SP+ ÷ after  */
    bool prohibited_with_sp;
  };
  struct table_cell table[NLBP][NLBP];
  /* Sets table[before][after].field to value.  */
  #define set_table_cell(field,value)  \
    (before == LBP_CP ? (set_table_cell_1 (LBP_CP1, field, value), set_table_cell_1 (LBP_CP2, field, value)) : \
     before == LBP_OP ? (set_table_cell_1 (LBP_OP1, field, value), set_table_cell_1 (LBP_OP2, field, value)) : \
     before == LBP_ID ? (set_table_cell_1 (LBP_ID1, field, value), set_table_cell_1 (LBP_ID2, field, value)) : \
     set_table_cell_1 (before, field, value))
  #define set_table_cell_1(row,field,value) \
    (after == LBP_CP ? (set_table_cell_2 (row, LBP_CP1, field, value), set_table_cell_2 (row, LBP_CP2, field, value)) : \
     after == LBP_OP ? (set_table_cell_2 (row, LBP_OP1, field, value), set_table_cell_2 (row, LBP_OP2, field, value)) : \
     after == LBP_ID ? (set_table_cell_2 (row, LBP_ID1, field, value), set_table_cell_2 (row, LBP_ID2, field, value)) : \
     set_table_cell_2 (row, after, field, value))
  #define set_table_cell_2(row,column,field,value) \
    (table[row][column].field = (value))

  /* Fill the table.
     If we were to apply the rules in top-down order (high precedence rules
     first), the table_cell fields have to support values false/true/unknown.
     If we apply the rules in the opposite order (high precedence order last),
     the table_cell fields need to support only the values false/true.
     So, that's what we do here.  */

  /* (LB31) Break everywhere.  */
  for (before = 0; before < NLBP; before++)
    for (after = 0; after < NLBP; after++)
      set_table_cell (prohibited_no_sp, false);

  /* (LB30b) Do not break between an emoji base (or potential emoji) and an
     emoji modifier.  */
  before = LBP_EB; after = LBP_EM; set_table_cell (prohibited_no_sp, true);
  before = LBP_ID2; after = LBP_EM; set_table_cell (prohibited_no_sp, true);

  /* (LB30) Do not break between letters, numbers, or ordinary symbols and
     opening or closing parentheses (except for East Asian parentheses).  */
  before = LBP_AL; after = LBP_OP1; set_table_cell (prohibited_no_sp, true);
  before = LBP_HL; after = LBP_OP1; set_table_cell (prohibited_no_sp, true);
  before = LBP_NU; after = LBP_OP1; set_table_cell (prohibited_no_sp, true);
  before = LBP_CP1; after = LBP_AL; set_table_cell (prohibited_no_sp, true);
  before = LBP_CP1; after = LBP_HL; set_table_cell (prohibited_no_sp, true);
  before = LBP_CP1; after = LBP_NU; set_table_cell (prohibited_no_sp, true);

  /* (LB29) Do not break between numeric punctuation and alphabetics
     ("e.g.").  */
  /* We don't implement this rule, because we find it desirable to break before
     the HTML tag "</P>" in strings like "<P>Some sentence.</P>".  */
#if 0
  before = LBP_IS; after = LBP_AL; set_table_cell (prohibited_no_sp, true);
  before = LBP_IS; after = LBP_HL; set_table_cell (prohibited_no_sp, true);
#endif

  /* (LB28) Do not break between alphabetics ("at").  */
  before = LBP_AL; after = LBP_AL; set_table_cell (prohibited_no_sp, true);
  before = LBP_AL; after = LBP_HL; set_table_cell (prohibited_no_sp, true);
  before = LBP_HL; after = LBP_AL; set_table_cell (prohibited_no_sp, true);
  before = LBP_HL; after = LBP_HL; set_table_cell (prohibited_no_sp, true);

  /* (LB27) Korean Syllable Block.  */
  before = LBP_JL; after = LBP_PO; set_table_cell (prohibited_no_sp, true);
  before = LBP_JV; after = LBP_PO; set_table_cell (prohibited_no_sp, true);
  before = LBP_JT; after = LBP_PO; set_table_cell (prohibited_no_sp, true);
  before = LBP_H2; after = LBP_PO; set_table_cell (prohibited_no_sp, true);
  before = LBP_H3; after = LBP_PO; set_table_cell (prohibited_no_sp, true);
  before = LBP_PR; after = LBP_JL; set_table_cell (prohibited_no_sp, true);
  before = LBP_PR; after = LBP_JV; set_table_cell (prohibited_no_sp, true);
  before = LBP_PR; after = LBP_JT; set_table_cell (prohibited_no_sp, true);
  before = LBP_PR; after = LBP_H2; set_table_cell (prohibited_no_sp, true);
  before = LBP_PR; after = LBP_H3; set_table_cell (prohibited_no_sp, true);

  /* (LB26) Do not break a Korean syllable.  */
  before = LBP_JL; after = LBP_JL; set_table_cell (prohibited_no_sp, true);
  before = LBP_JL; after = LBP_JV; set_table_cell (prohibited_no_sp, true);
  before = LBP_JL; after = LBP_H2; set_table_cell (prohibited_no_sp, true);
  before = LBP_JL; after = LBP_H3; set_table_cell (prohibited_no_sp, true);
  before = LBP_JV; after = LBP_JV; set_table_cell (prohibited_no_sp, true);
  before = LBP_JV; after = LBP_JT; set_table_cell (prohibited_no_sp, true);
  before = LBP_H2; after = LBP_JV; set_table_cell (prohibited_no_sp, true);
  before = LBP_H2; after = LBP_JT; set_table_cell (prohibited_no_sp, true);
  before = LBP_JT; after = LBP_JT; set_table_cell (prohibited_no_sp, true);
  before = LBP_H3; after = LBP_JT; set_table_cell (prohibited_no_sp, true);

  /* (LB25) Do not break between the following pairs of classes relevant to
     numbers.  */
  before = LBP_CL; after = LBP_PO; set_table_cell (prohibited_no_sp, true);
  before = LBP_CP; after = LBP_PO; set_table_cell (prohibited_no_sp, true);
  before = LBP_CL; after = LBP_PR; set_table_cell (prohibited_no_sp, true);
  before = LBP_CP; after = LBP_PR; set_table_cell (prohibited_no_sp, true);
  before = LBP_NU; after = LBP_PO; set_table_cell (prohibited_no_sp, true);
  before = LBP_NU; after = LBP_PR; set_table_cell (prohibited_no_sp, true);
  before = LBP_PO; after = LBP_OP; set_table_cell (prohibited_no_sp, true);
  before = LBP_PO; after = LBP_NU; set_table_cell (prohibited_no_sp, true);
  before = LBP_PR; after = LBP_OP; set_table_cell (prohibited_no_sp, true);
  before = LBP_PR; after = LBP_NU; set_table_cell (prohibited_no_sp, true);
  before = LBP_HY; after = LBP_NU; set_table_cell (prohibited_no_sp, true);
  before = LBP_IS; after = LBP_NU; set_table_cell (prohibited_no_sp, true);
  before = LBP_NU; after = LBP_NU; set_table_cell (prohibited_no_sp, true);
  before = LBP_SY; after = LBP_NU; set_table_cell (prohibited_no_sp, true);

  /* (LB24) Do not break between numeric prefix/postfix and letters, or between
     letters and prefix/postfix.  */
  before = LBP_PR; after = LBP_AL; set_table_cell (prohibited_no_sp, true);
  before = LBP_PR; after = LBP_HL; set_table_cell (prohibited_no_sp, true);
  before = LBP_PO; after = LBP_AL; set_table_cell (prohibited_no_sp, true);
  before = LBP_PO; after = LBP_HL; set_table_cell (prohibited_no_sp, true);
  before = LBP_AL; after = LBP_PR; set_table_cell (prohibited_no_sp, true);
  before = LBP_AL; after = LBP_PO; set_table_cell (prohibited_no_sp, true);
  before = LBP_HL; after = LBP_PR; set_table_cell (prohibited_no_sp, true);
  before = LBP_HL; after = LBP_PO; set_table_cell (prohibited_no_sp, true);

  /* (LB23a) Do not break between numeric prefixes and ideographs, or between
     ideographs and numeric postfixes.  */
  before = LBP_PR; after = LBP_ID; set_table_cell (prohibited_no_sp, true);
  before = LBP_PR; after = LBP_EB; set_table_cell (prohibited_no_sp, true);
  before = LBP_PR; after = LBP_EM; set_table_cell (prohibited_no_sp, true);
  before = LBP_ID; after = LBP_PO; set_table_cell (prohibited_no_sp, true);
  before = LBP_EB; after = LBP_PO; set_table_cell (prohibited_no_sp, true);
  before = LBP_EM; after = LBP_PO; set_table_cell (prohibited_no_sp, true);

  /* (LB23) Do not break between digits and letters.  */
  before = LBP_AL; after = LBP_NU; set_table_cell (prohibited_no_sp, true);
  before = LBP_HL; after = LBP_NU; set_table_cell (prohibited_no_sp, true);
  before = LBP_NU; after = LBP_AL; set_table_cell (prohibited_no_sp, true);
  before = LBP_NU; after = LBP_HL; set_table_cell (prohibited_no_sp, true);

  /* (LB22) Do not break before ellipses.  */
  for (before = 0; before < NLBP; before++)
    {
      after = LBP_IN; set_table_cell (prohibited_no_sp, true);
    }

  /* (LB21b) Don’t break between Solidus and Hebrew letters.  */
  before = LBP_SY; after = LBP_HL; set_table_cell (prohibited_no_sp, true);

  /* (LB21) Do not break before hyphen-minus, other hyphens, fixed-width spaces,
     small kana, and other non-starters, or after acute accents.  */
  for (before = 0; before < NLBP; before++)
    {
      after = LBP_BA; set_table_cell (prohibited_no_sp, true);
      after = LBP_HY; set_table_cell (prohibited_no_sp, true);
      after = LBP_NS; set_table_cell (prohibited_no_sp, true);
    }
  for (after = 0; after < NLBP; after++)
    {
      before = LBP_BB; set_table_cell (prohibited_no_sp, true);
    }

  /* (LB19) Do not break before or after quotation marks, such as '”'.  */
  for (before = 0; before < NLBP; before++)
    {
      after = LBP_QU; set_table_cell (prohibited_no_sp, true);
    }
  for (after = 0; after < NLBP; after++)
    {
      before = LBP_QU; set_table_cell (prohibited_no_sp, true);
    }

  /* (LB18) Break after spaces.  */
  for (before = 0; before < NLBP; before++)
    for (after = 0; after < NLBP; after++)
      set_table_cell (prohibited_with_sp, false);

  /* (LB17) Do not break within '——', even with intervening spaces.  */
  before = LBP_B2; after = LBP_B2; set_table_cell (prohibited_no_sp, true);
                                   set_table_cell (prohibited_with_sp, true);

  /* (LB16) Do not break between closing punctuation and a nonstarter (lb=NS),
     even with intervening spaces.  */
  before = LBP_CL; after = LBP_NS; set_table_cell (prohibited_no_sp, true);
                                   set_table_cell (prohibited_with_sp, true);
  before = LBP_CL; after = LBP_CP; set_table_cell (prohibited_no_sp, true);
                                   set_table_cell (prohibited_with_sp, true);

  /* (LB15) Do not break within '”[', even with intervening spaces.  */
  before = LBP_QU; after = LBP_OP; set_table_cell (prohibited_no_sp, true);
                                   set_table_cell (prohibited_with_sp, true);

  /* (LB14) Do not break after '[', even after spaces.  */
  for (after = 0; after < NLBP; after++)
    {
      before = LBP_OP; set_table_cell (prohibited_no_sp, true);
                       set_table_cell (prohibited_with_sp, true);
    }

  /* (LB13) Do not break before ‘]’ or ‘!’ or ‘;’ or ‘/’, even after spaces.  */
  for (before = 0; before < NLBP; before++)
    {
      after = LBP_CL; set_table_cell (prohibited_no_sp, true);
                      set_table_cell (prohibited_with_sp, true);
      after = LBP_CP; set_table_cell (prohibited_no_sp, true);
                      set_table_cell (prohibited_with_sp, true);
      after = LBP_EX; set_table_cell (prohibited_no_sp, true);
                      set_table_cell (prohibited_with_sp, true);
      after = LBP_IS; set_table_cell (prohibited_no_sp, true);
                      set_table_cell (prohibited_with_sp, true);
      after = LBP_SY; set_table_cell (prohibited_no_sp, true);
                      set_table_cell (prohibited_with_sp, true);
    }

  /* (LB12a) Do not break before NBSP and related characters, except after
     spaces and hyphens.  */
  for (before = 0; before < NLBP; before++)
    if (before != LBP_BA && before != LBP_HY)
      {
        after = LBP_GL; set_table_cell (prohibited_no_sp, true);
      }

  /* (LB12) Do not break after NBSP and related characters.  */
  for (after = 0; after < NLBP; after++)
    {
      before = LBP_GL; set_table_cell (prohibited_no_sp, true);
    }

  /* (LB11) Do not break before or after Word joiner and related characters.  */
  for (before = 0; before < NLBP; before++)
    {
      after = LBP_WJ; set_table_cell (prohibited_no_sp, true);
                      set_table_cell (prohibited_with_sp, true);
    }
  for (after = 0; after < NLBP; after++)
    {
      before = LBP_WJ; set_table_cell (prohibited_no_sp, true);
    }

  /* (LB10) Treat any remaining combining mark or ZWJ as AL.  */
  /* We resolve LBP_CM at runtime, before accessing the table.  */
  for (before = 0; before < NLBP; before++)
    table[before][LBP_ZWJ] = table[before][LBP_AL];
  for (after = 0; after < NLBP; after++)
    table[LBP_ZWJ][after] = table[LBP_AL][after];
  table[LBP_ZWJ][LBP_ZWJ] = table[LBP_AL][LBP_AL];

  /* (LB8a) Do not break between a zero width joiner and an ideograph, emoji
     base or emoji modifier.  */
  before = LBP_ZWJ; after = LBP_ID; set_table_cell (prohibited_no_sp, true);
  before = LBP_ZWJ; after = LBP_EB; set_table_cell (prohibited_no_sp, true);
  before = LBP_ZWJ; after = LBP_EM; set_table_cell (prohibited_no_sp, true);

  /* Not reflected in the table:
  (LB30a) Break between two regional indicator symbols if and only if there are
          an even number of regional indicators preceding the position of the
          break.
  (LB21a) Don't break after Hebrew + Hyphen.
  (LB20) Break before and after unresolved CB.
         We resolve LBP_CB at runtime, before accessing the table.
  (LB9) Do not break a combining character sequence; treat it as if it has the
        line breaking class of the base character in all of the following rules.
        Treat ZWJ as if it were CM.
  (LB8) Break before any character following a zero-width space, even if one
        or more spaces intervene.
        We handle LBP_ZW at runtime, before accessing the table.
  (LB7) Do not break before spaces or zero width space.
        We handle LBP_ZW at runtime, before accessing the table.
  (LB6) Do not break before hard line breaks.
        We handle LBP_BK at runtime, before accessing the table.
  (LB5) Treat CR followed by LF, as well as CR, LF, and NL as hard line breaks.
  (LB4) Always break after hard line breaks.
  (LB3) Always break at the end of text.
  (LB2) Never break at the start of text.
  */

  fprintf (stream, "const unsigned char unilbrk_table[%u][%u] =\n", NLBP, NLBP);
  fprintf (stream, "{\n");
  fprintf (stream, "                                /* after */\n");

  fprintf (stream, "        /* ");
  for (after = 0; after < NLBP; after++)
    fprintf (stream, " %-3s", lbp_value_to_string (after) + 4);
  fprintf (stream, " */\n");

  for (before = 0; before < NLBP; before++)
    {
      fprintf (stream, "/* %3s */ {", lbp_value_to_string (before) + 4);
      for (after = 0; after < NLBP; after++)
        {
          if (table[before][after].prohibited_no_sp)
            {
              if (table[before][after].prohibited_with_sp)
                /* Prohibited break.  */
                fprintf (stream, "  P,");
              else
                /* Indirect break.  */
                fprintf (stream, "  I,");
            }
          else
            {
              if (table[before][after].prohibited_with_sp)
                abort ();
              else
                /* Direct break.  */
                fprintf (stream, "  D,");
            }
        }
      fprintf (stream, " },\n");
    }
  fprintf (stream, "/* \"\" */\n");
  fprintf (stream, "/* before */\n");
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Word break property.
   Updated for Unicode TR #29 revision 17.  */

/* Possible values of the Word_Break property.  */
enum
{
  WBP_OTHER        = 0,
  WBP_CR           = 11,
  WBP_LF           = 12,
  WBP_NEWLINE      = 10,
  WBP_EXTEND       = 8,
  WBP_FORMAT       = 9,
  WBP_KATAKANA     = 1,
  WBP_ALETTER      = 2,
  WBP_MIDNUMLET    = 3,
  WBP_MIDLETTER    = 4,
  WBP_MIDNUM       = 5,
  WBP_NUMERIC      = 6,
  WBP_EXTENDNUMLET = 7,
  WBP_RI           = 13,
  WBP_DQ           = 14,
  WBP_SQ           = 15,
  WBP_HL           = 16,
  WBP_ZWJ          = 17,
  WBP_WSS          = 22
};

/* Returns the word breaking property for ch, as a bit mask.  */
static int
get_wbp (unsigned int ch)
{
  int attr = 0;

  if (unicode_attributes[ch].name != NULL)
    {
      if (ch == 0x000D)
        attr |= 1 << WBP_CR;

      if (ch == 0x000A)
        attr |= 1 << WBP_LF;

      if (ch == 0x000B || ch == 0x000C
          || ch == 0x0085
          || ch == 0x2028 || ch == 0x2029)
        attr |= 1 << WBP_NEWLINE;

      if (((unicode_properties[ch] >> PROP_GRAPHEME_EXTEND) & 1) != 0
          || ((unicode_properties[ch] >> PROP_OTHER_GRAPHEME_EXTEND) & 1) != 0
          || (unicode_attributes[ch].category != NULL
              && strcmp (unicode_attributes[ch].category, "Mc") == 0)
          || ((unicode_properties[ch] >> PROP_EMOJI_MODIFIER) & 1) != 0 /* Emoji modifier */)
        attr |= 1 << WBP_EXTEND;

      if (unicode_attributes[ch].category != NULL
          && strcmp (unicode_attributes[ch].category, "Cf") == 0
          && ch != 0x200B && ch != 0x200C && ch != 0x200D
          && !(ch >= 0xe0020 && ch <= 0xe007f))
        attr |= 1 << WBP_FORMAT;

      if ((unicode_scripts[ch] < numscripts
           && strcmp (scripts[unicode_scripts[ch]], "Katakana") == 0)
          || (ch >= 0x3031 && ch <= 0x3035)
          || ch == 0x309B || ch == 0x309C || ch == 0x30A0 || ch == 0x30FC
          || ch == 0xFF70)
        attr |= 1 << WBP_KATAKANA;

      if ((unicode_scripts[ch] < numscripts
           && strcmp (scripts[unicode_scripts[ch]], "Hebrew") == 0)
          && strcmp (unicode_attributes[ch].category, "Lo") == 0)
        attr |= 1 << WBP_HL;

      if ((((unicode_properties[ch] >> PROP_ALPHABETIC) & 1) != 0
           || (ch >= 0x02C2 && ch <= 0x02C5)
           || (ch >= 0x02D2 && ch <= 0x02D7)
           || (ch >= 0x02DE && ch <= 0x02DF)
           || (ch >= 0x02E5 && ch <= 0x02EB)
           || ch == 0x02ED
           || (ch >= 0x02EF && ch <= 0x02FF)
           || (ch >= 0x055A && ch <= 0x055C)
           || ch == 0x055E
           || ch == 0x058A
           || ch == 0x05F3
           || (ch >= 0xA708 && ch <= 0xA716)
           || (ch >= 0xA720 && ch <= 0xA721)
           || (ch >= 0xA789 && ch <= 0xA78A)
           || ch == 0xAB5B)
          && ((unicode_properties[ch] >> PROP_IDEOGRAPHIC) & 1) == 0
          && (attr & (1 << WBP_KATAKANA)) == 0
          && ((get_lbp (ch) >> LBP_SA) & 1) == 0
          && !(unicode_scripts[ch] < numscripts
               && strcmp (scripts[unicode_scripts[ch]], "Hiragana") == 0)
          && (attr & (1 << WBP_EXTEND)) == 0
          && (attr & (1 << WBP_HL)) == 0)
        attr |= 1 << WBP_ALETTER;

      if (is_WBP_MIDNUMLET (ch))
        attr |= 1 << WBP_MIDNUMLET;

      if (is_WBP_MIDLETTER (ch) && ch != 0x02D7)
        attr |= 1 << WBP_MIDLETTER;

      if ((((get_lbp (ch) >> LBP_IS) & 1) != 0
           || ch == 0x066C || ch == 0xFE50 || ch == 0xFE54 || ch == 0xFF0C
           || ch == 0xFF1B)
          && ch != 0x003A && ch != 0xFE13 && ch != 0x002E)
        attr |= 1 << WBP_MIDNUM;

      if ((((get_lbp (ch) >> LBP_NU) & 1) != 0
            || (ch >= 0xFF10 && ch <= 0xFF19))
          && ch != 0x066C)
        attr |= 1 << WBP_NUMERIC;

      if ((unicode_attributes[ch].category != NULL
           && strcmp (unicode_attributes[ch].category, "Pc") == 0)
          || ch == 0x202F /* NARROW NO-BREAK SPACE */)
        attr |= 1 << WBP_EXTENDNUMLET;

      if (is_property_regional_indicator (ch))
        attr |= 1 << WBP_RI;

      if (ch == 0x0022)
        attr |= 1 << WBP_DQ;

      if (ch == 0x0027)
        attr |= 1 << WBP_SQ;

      if (ch == 0x200D)
        attr |= 1 << WBP_ZWJ;

      if (is_category_Zs (ch) && ((get_lbp (ch) >> LBP_GL) & 1) == 0)
        attr |= 1 << WBP_WSS;
    }

  if (attr == 0)
    /* other */
    attr |= 1 << WBP_OTHER;

  return attr;
}

/* Output the word break property in a human readable format.  */
static void
debug_output_wbp (FILE *stream)
{
  unsigned int i;

  for (i = 0; i < 0x110000; i++)
    {
      int attr = get_wbp (i);
      if (attr != 1 << WBP_OTHER)
        {
          fprintf (stream, "0x%04X", i);
          if (attr & (1 << WBP_CR))
            fprintf (stream, " CR");
          if (attr & (1 << WBP_LF))
            fprintf (stream, " LF");
          if (attr & (1 << WBP_NEWLINE))
            fprintf (stream, " Newline");
          if (attr & (1 << WBP_EXTEND))
            fprintf (stream, " Extend");
          if (attr & (1 << WBP_FORMAT))
            fprintf (stream, " Format");
          if (attr & (1 << WBP_KATAKANA))
            fprintf (stream, " Katakana");
          if (attr & (1 << WBP_ALETTER))
            fprintf (stream, " ALetter");
          if (attr & (1 << WBP_MIDNUMLET))
            fprintf (stream, " MidNumLet");
          if (attr & (1 << WBP_MIDLETTER))
            fprintf (stream, " MidLetter");
          if (attr & (1 << WBP_MIDNUM))
            fprintf (stream, " MidNum");
          if (attr & (1 << WBP_NUMERIC))
            fprintf (stream, " Numeric");
          if (attr & (1 << WBP_EXTENDNUMLET))
            fprintf (stream, " ExtendNumLet");
          if (attr & (1 << WBP_RI))
            fprintf (stream, " Regional_Indicator");
          if (attr & (1 << WBP_DQ))
            fprintf (stream, " Double_Quote");
          if (attr & (1 << WBP_SQ))
            fprintf (stream, " Single_Quote");
          if (attr & (1 << WBP_HL))
            fprintf (stream, " Hebrew_Letter");
          if (attr & (1 << WBP_ZWJ))
            fprintf (stream, " ZWJ");
          if (attr & (1 << WBP_WSS))
            fprintf (stream, " WSegSpace");
          fprintf (stream, "\n");
        }
    }
}

static void
debug_output_wbrk_tables (const char *filename)
{
  FILE *stream;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  debug_output_wbp (stream);

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* The word break property from the WordBreakProperty.txt file.  */
int unicode_org_wbp[0x110000];

/* Stores in unicode_org_wbp[] the word break property from the
   WordBreakProperty.txt file.  */
static void
fill_org_wbp (const char *wordbreakproperty_filename)
{
  unsigned int i;
  FILE *stream;

  for (i = 0; i < 0x110000; i++)
    unicode_org_wbp[i] = WBP_OTHER;

  stream = fopen (wordbreakproperty_filename, "r");
  if (stream == NULL)
    {
      fprintf (stderr, "error during fopen of '%s'\n", wordbreakproperty_filename);
      exit (1);
    }

  for (;;)
    {
      char buf[200+1];
      unsigned int i1, i2;
      char padding[200+1];
      char propname[200+1];
      int propvalue;

      if (fscanf (stream, "%200[^\n]\n", buf) < 1)
        break;

      if (buf[0] == '\0' || buf[0] == '#')
        continue;

      if (sscanf (buf, "%X..%X%[ ;]%[^ ]", &i1, &i2, padding, propname) != 4)
        {
          if (sscanf (buf, "%X%[ ;]%[^ ]", &i1, padding, propname) != 3)
            {
              fprintf (stderr, "parse error in '%s'\n",
                       wordbreakproperty_filename);
              exit (1);
            }
          i2 = i1;
        }
#define PROP(name,value) \
      if (strcmp (propname, name) == 0) propvalue = value; else
      PROP ("CR", WBP_CR)
      PROP ("LF", WBP_LF)
      PROP ("Newline", WBP_NEWLINE)
      PROP ("Extend", WBP_EXTEND)
      PROP ("Format", WBP_FORMAT)
      PROP ("Katakana", WBP_KATAKANA)
      PROP ("ALetter", WBP_ALETTER)
      PROP ("MidNumLet", WBP_MIDNUMLET)
      PROP ("MidLetter", WBP_MIDLETTER)
      PROP ("MidNum", WBP_MIDNUM)
      PROP ("Numeric", WBP_NUMERIC)
      PROP ("ExtendNumLet", WBP_EXTENDNUMLET)
      PROP ("Regional_Indicator", WBP_RI)
      PROP ("Double_Quote", WBP_DQ)
      PROP ("Single_Quote", WBP_SQ)
      PROP ("Hebrew_Letter", WBP_HL)
      PROP ("ZWJ", WBP_ZWJ)
      PROP ("WSegSpace", WBP_WSS)
#undef PROP
        {
          fprintf (stderr, "unknown property value '%s' in '%s'\n", propname,
                   wordbreakproperty_filename);
          exit (1);
        }
      assert (i1 <= i2 && i2 < 0x110000);

      for (i = i1; i <= i2; i++)
        unicode_org_wbp[i] = propvalue;
    }

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error reading from '%s'\n", wordbreakproperty_filename);
      exit (1);
    }
}

/* Output the word break property in a human readable format.  */
static void
debug_output_org_wbp (FILE *stream)
{
  unsigned int i;

  for (i = 0; i < 0x110000; i++)
    {
      int propvalue = unicode_org_wbp[i];
      if (propvalue != WBP_OTHER)
        {
          fprintf (stream, "0x%04X", i);
#define PROP(name,value) \
          if (propvalue == value) fprintf (stream, " " name); else
          PROP ("CR", WBP_CR)
          PROP ("LF", WBP_LF)
          PROP ("Newline", WBP_NEWLINE)
          PROP ("Extend", WBP_EXTEND)
          PROP ("Format", WBP_FORMAT)
          PROP ("Katakana", WBP_KATAKANA)
          PROP ("ALetter", WBP_ALETTER)
          PROP ("MidNumLet", WBP_MIDNUMLET)
          PROP ("MidLetter", WBP_MIDLETTER)
          PROP ("MidNum", WBP_MIDNUM)
          PROP ("Numeric", WBP_NUMERIC)
          PROP ("ExtendNumLet", WBP_EXTENDNUMLET)
          PROP ("Regional_Indicator", WBP_RI)
          PROP ("Double_Quote", WBP_DQ)
          PROP ("Single_Quote", WBP_SQ)
          PROP ("Hebrew_Letter", WBP_HL)
          PROP ("ZWJ", WBP_ZWJ)
          PROP ("WSegSpace", WBP_WSS)
#undef PROP
          fprintf (stream, " ??");
          fprintf (stream, "\n");
        }
    }
}

static void
debug_output_org_wbrk_tables (const char *filename)
{
  FILE *stream;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  debug_output_org_wbp (stream);

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Construction of sparse 3-level tables.  */
#define TABLE wbp_table
#define ELEMENT unsigned char
#define DEFAULT WBP_OTHER
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

static void
output_wbp (FILE *stream)
{
  unsigned int i;
  struct wbp_table t;
  unsigned int level1_offset, level2_offset, level3_offset;

  t.p = 7;
  t.q = 9;
  wbp_table_init (&t);

  for (i = 0; i < 0x110000; i++)
    {
      int attr = get_wbp (i);

      /* Now attr should contain exactly one bit.  */
      assert (attr != 0 && (attr & (attr - 1)) == 0);

      if (attr != 1 << WBP_OTHER)
        {
          unsigned int log2_attr;
          for (log2_attr = 0; attr > 1; attr >>= 1, log2_attr++);

          wbp_table_add (&t, i, log2_attr);
        }
    }

  wbp_table_finalize (&t);

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
    fprintf (stream, "#define wbrkprop_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream, "\n");
  fprintf (stream, "typedef struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    int level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned char level3[%zu << %d];\n", t.level3_size, t.p);
  fprintf (stream, "  }\n");
  fprintf (stream, "wbrkprop_t;\n");
  fprintf (stream, "static const wbrkprop_t uniwbrkprop =\n");
  fprintf (stream, "{\n");
  fprintf (stream, "  {");
  if (t.level1_size > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level3_offset) / sizeof (unsigned char));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level3_size << t.p > 4)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level3_size << t.p; i++)
    {
      unsigned char value = ((unsigned char *) (t.result + level3_offset))[i];
      const char *value_string;
      switch (value)
        {
#define CASE(x) case x: value_string = #x; break;
          CASE(WBP_OTHER);
          CASE(WBP_CR);
          CASE(WBP_LF);
          CASE(WBP_NEWLINE);
          CASE(WBP_EXTEND);
          CASE(WBP_FORMAT);
          CASE(WBP_KATAKANA);
          CASE(WBP_ALETTER);
          CASE(WBP_MIDNUMLET);
          CASE(WBP_MIDLETTER);
          CASE(WBP_MIDNUM);
          CASE(WBP_NUMERIC);
          CASE(WBP_EXTENDNUMLET);
          CASE(WBP_RI);
          CASE(WBP_DQ);
          CASE(WBP_SQ);
          CASE(WBP_HL);
          CASE(WBP_ZWJ);
          CASE(WBP_WSS);
#undef CASE
          default:
            abort ();
        }
      if (i > 0 && (i % 4) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " %s%s", value_string,
               (i+1 < t.level3_size << t.p ? "," : ""));
    }
  if (t.level3_size << t.p > 4)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  fprintf (stream, "};\n");
}

static void
output_wbrk_tables (const char *filename, const char *version)
{
  FILE *stream;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Word breaking properties of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, false);
  fprintf (stream, "\n");

  output_wbp (stream);

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Grapheme break property.
   Updated for Unicode TR #29 revision 29.  */

/* Possible values of the Grapheme_Cluster_Break property.  */
enum
{
  GBP_OTHER        = 0,
  GBP_CR           = 1,
  GBP_LF           = 2,
  GBP_CONTROL      = 3,
  GBP_EXTEND       = 4,
  GBP_PREPEND      = 5,
  GBP_SPACINGMARK  = 6,
  GBP_L            = 7,
  GBP_V            = 8,
  GBP_T            = 9,
  GBP_LV           = 10,
  GBP_LVT          = 11,
  GBP_RI           = 12,
  GBP_ZWJ          = 13,
  GBP_EB           = 14,
  GBP_EM           = 15,
  GBP_GAZ          = 16,
  GBP_EBG          = 17
};

/* Construction of sparse 3-level tables.  */
#define TABLE gbp_table
#define ELEMENT unsigned char
#define DEFAULT GBP_OTHER
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

/* The grapheme break property from the GraphemeBreakProperty.txt file.  */
int unicode_org_gbp[0x110000];

/* Output the unit test data for the grapheme break property.  */
static void
output_gbp_test (const char *filename)
{
  FILE *stream;
  bool need_comma;
  unsigned int ch;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Test the Unicode grapheme break property functions.\n");
  fprintf (stream, "   Copyright (C) 2010-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_tests_license (stream);
  fprintf (stream, "\n");

  need_comma = false;
  for (ch = 0; ch < 0x110000; ch++)
    {
      int gbp = unicode_org_gbp[ch];
      const char *gbp_string;

      while (ch + 1 < 0x110000 && unicode_org_gbp[ch + 1] == gbp)
        ch++;

      switch (gbp)
        {
#define CASE(x) case x: gbp_string = #x; break;
      CASE (GBP_OTHER)
      CASE (GBP_CR)
      CASE (GBP_LF)
      CASE (GBP_CONTROL)
      CASE (GBP_EXTEND)
      CASE (GBP_PREPEND)
      CASE (GBP_SPACINGMARK)
      CASE (GBP_L)
      CASE (GBP_V)
      CASE (GBP_T)
      CASE (GBP_LV)
      CASE (GBP_LVT)
      CASE (GBP_RI)
      CASE (GBP_ZWJ)
      CASE (GBP_EB)
      CASE (GBP_EM)
      CASE (GBP_GAZ)
      CASE (GBP_EBG)
#undef CASE
        default:
          abort ();
        }

      if (need_comma)
        fprintf (stream, ",\n");
      fprintf (stream, "{ 0x%04X, %s }", ch + 1, gbp_string);

      need_comma = true;
    }
  fprintf (stream, "\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Output the per-character grapheme break property table.  */
static void
output_gbp_table (const char *filename, const char *version)
{
  FILE *stream;
  unsigned int ch, i;
  struct gbp_table t;
  unsigned int level1_offset, level2_offset, level3_offset;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Grapheme break property of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, false);
  fprintf (stream, "\n");

  t.p = 7;
  t.q = 9;
  gbp_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    gbp_table_add (&t, ch, unicode_org_gbp[ch]);

  gbp_table_finalize (&t);

  /* Offsets in t.result, in memory of this process.  */
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
    fprintf (stream, "#define gbrkprop_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream, "static const\n");
  fprintf (stream, "struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    short level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned char level3[%zu << %d];\n",
           t.level3_size, t.p);
  fprintf (stream, "  }\n");
  fprintf (stream, "unigbrkprop =\n");
  fprintf (stream, "{\n");
  fprintf (stream, "  {");
  if (t.level1_size > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level3_offset) / sizeof (uint8_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level3_size << t.p > 4)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level3_size << t.p; i++)
    {
      unsigned char value = ((unsigned char *) (t.result + level3_offset))[i];
      const char *value_string;
      switch (value)
        {
#define CASE(x) case x: value_string = #x; break;
      CASE (GBP_OTHER)
      CASE (GBP_CR)
      CASE (GBP_LF)
      CASE (GBP_CONTROL)
      CASE (GBP_EXTEND)
      CASE (GBP_PREPEND)
      CASE (GBP_SPACINGMARK)
      CASE (GBP_L)
      CASE (GBP_V)
      CASE (GBP_T)
      CASE (GBP_LV)
      CASE (GBP_LVT)
      CASE (GBP_RI)
      CASE (GBP_ZWJ)
      CASE (GBP_EB)
      CASE (GBP_EM)
      CASE (GBP_GAZ)
      CASE (GBP_EBG)
#undef CASE
          default:
            abort ();
        }
      if (i > 0 && (i % 4) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " %s%s", value_string,
               (i+1 < t.level3_size << t.p ? "," : ""));
    }
  if (t.level3_size << t.p > 4)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Stores in unicode_org_gbp[] the grapheme breaking property from the
   GraphemeBreakProperty.txt file.  */
static void
fill_org_gbp (const char *graphemebreakproperty_filename)
{
  unsigned int i;
  FILE *stream;
  int lineno = 0;

  for (i = 0; i < 0x110000; i++)
    unicode_org_gbp[i] = GBP_OTHER;

  stream = fopen (graphemebreakproperty_filename, "r");
  if (stream == NULL)
    {
      fprintf (stderr, "error during fopen of '%s'\n",
               graphemebreakproperty_filename);
      exit (1);
    }

  for (;;)
    {
      char buf[200+1];
      unsigned int i1, i2;
      char padding[200+1];
      char propname[200+1];
      int propvalue;

      lineno++;
      if (fscanf (stream, "%200[^\n]\n", buf) < 1)
        break;

      if (buf[0] == '\0' || buf[0] == '#')
        continue;

      if (sscanf (buf, "%X..%X%[ ;]%[^ ]", &i1, &i2, padding, propname) != 4)
        {
          if (sscanf (buf, "%X%[ ;]%[^ ]", &i1, padding, propname) != 3)
            {
              fprintf (stderr, "parse error in '%s'\n",
                       graphemebreakproperty_filename);
              exit (1);
            }
          i2 = i1;
        }
#define PROP(name,value) \
      if (strcmp (propname, name) == 0) propvalue = value; else
      PROP ("CR", GBP_CR)
      PROP ("LF", GBP_LF)
      PROP ("Control", GBP_CONTROL)
      PROP ("Extend", GBP_EXTEND)
      PROP ("Prepend", GBP_PREPEND)
      PROP ("SpacingMark", GBP_SPACINGMARK)
      PROP ("L", GBP_L)
      PROP ("V", GBP_V)
      PROP ("T", GBP_T)
      PROP ("LV", GBP_LV)
      PROP ("LVT", GBP_LVT)
      PROP ("Regional_Indicator", GBP_RI)
      PROP ("ZWJ", GBP_ZWJ)
      PROP ("E_Base", GBP_EB)
      PROP ("E_Modifier", GBP_EM)
      PROP ("Glue_After_Zwj", GBP_GAZ)
      PROP ("E_Base_GAZ", GBP_EBG)
#undef PROP
        {
          fprintf (stderr, "unknown property value '%s' in %s:%d\n", propname,
                   graphemebreakproperty_filename, lineno);
          exit (1);
        }
      assert (i1 <= i2 && i2 < 0x110000);

      for (i = i1; i <= i2; i++)
        unicode_org_gbp[i] = propvalue;
    }

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error reading from '%s'\n", graphemebreakproperty_filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Composition and decomposition.
   Updated for Unicode TR #15 revision 33.  */

/* Maximum number of characters into which a single Unicode character can be
   decomposed.  */
#define MAX_DECOMP_LENGTH 18

enum
{
  UC_DECOMP_CANONICAL,/*            Canonical decomposition.                  */
  UC_DECOMP_FONT,    /*   <font>    A font variant (e.g. a blackletter form). */
  UC_DECOMP_NOBREAK, /* <noBreak>   A no-break version of a space or hyphen.  */
  UC_DECOMP_INITIAL, /* <initial>   An initial presentation form (Arabic).    */
  UC_DECOMP_MEDIAL,  /*  <medial>   A medial presentation form (Arabic).      */
  UC_DECOMP_FINAL,   /*  <final>    A final presentation form (Arabic).       */
  UC_DECOMP_ISOLATED,/* <isolated>  An isolated presentation form (Arabic).   */
  UC_DECOMP_CIRCLE,  /*  <circle>   An encircled form.                        */
  UC_DECOMP_SUPER,   /*  <super>    A superscript form.                       */
  UC_DECOMP_SUB,     /*   <sub>     A subscript form.                         */
  UC_DECOMP_VERTICAL,/* <vertical>  A vertical layout presentation form.      */
  UC_DECOMP_WIDE,    /*   <wide>    A wide (or zenkaku) compatibility character. */
  UC_DECOMP_NARROW,  /*  <narrow>   A narrow (or hankaku) compatibility character. */
  UC_DECOMP_SMALL,   /*  <small>    A small variant form (CNS compatibility). */
  UC_DECOMP_SQUARE,  /*  <square>   A CJK squared font variant.               */
  UC_DECOMP_FRACTION,/* <fraction>  A vulgar fraction form.                   */
  UC_DECOMP_COMPAT   /*  <compat>   Otherwise unspecified compatibility character. */
};

/* Return the decomposition for a Unicode character (ignoring Hangul Jamo
   decompositions).  Return the type, or -1 for none.  */
static int
get_decomposition (unsigned int ch,
                   unsigned int *lengthp, unsigned int decomposed[MAX_DECOMP_LENGTH])
{
  const char *decomposition = unicode_attributes[ch].decomposition;

  if (decomposition != NULL && decomposition[0] != '\0')
    {
      int type = UC_DECOMP_CANONICAL;
      unsigned int length;
      char *endptr;

      if (decomposition[0] == '<')
        {
          const char *rangle;
          size_t typelen;

          rangle = strchr (decomposition + 1, '>');
          assert (rangle != NULL);
          typelen = rangle + 1 - decomposition;
#define TYPE(t1,t2) \
          if (typelen == (sizeof (t1) - 1) && memcmp (decomposition, t1, typelen) == 0) \
            type = t2; \
          else
          TYPE ("<font>", UC_DECOMP_FONT)
          TYPE ("<noBreak>", UC_DECOMP_NOBREAK)
          TYPE ("<initial>", UC_DECOMP_INITIAL)
          TYPE ("<medial>", UC_DECOMP_MEDIAL)
          TYPE ("<final>", UC_DECOMP_FINAL)
          TYPE ("<isolated>", UC_DECOMP_ISOLATED)
          TYPE ("<circle>", UC_DECOMP_CIRCLE)
          TYPE ("<super>", UC_DECOMP_SUPER)
          TYPE ("<sub>", UC_DECOMP_SUB)
          TYPE ("<vertical>", UC_DECOMP_VERTICAL)
          TYPE ("<wide>", UC_DECOMP_WIDE)
          TYPE ("<narrow>", UC_DECOMP_NARROW)
          TYPE ("<small>", UC_DECOMP_SMALL)
          TYPE ("<square>", UC_DECOMP_SQUARE)
          TYPE ("<fraction>", UC_DECOMP_FRACTION)
          TYPE ("<compat>", UC_DECOMP_COMPAT)
            {
              fprintf (stderr, "unknown decomposition type %*s\n", (int)typelen, decomposition);
              exit (1);
            }
#undef TYPE
          decomposition = rangle + 1;
          if (decomposition[0] == ' ')
            decomposition++;
        }
      for (length = 0; length < MAX_DECOMP_LENGTH; length++)
        {
          decomposed[length] = strtoul (decomposition, &endptr, 16);
          if (endptr == decomposition)
            break;
          decomposition = endptr;
          if (decomposition[0] == ' ')
            decomposition++;
        }
      /* Make sure that *DECOMPOSITION is not NULL-terminated.
         Otherwise MAX_DECOMP_LENGTH is too small.  */
      assert (*decomposition == '\0');

      *lengthp = length;
      return type;
    }
  else
    return -1;
}

/* Construction of sparse 3-level tables.  */
#define TABLE decomp_table
#define ELEMENT uint16_t
#define DEFAULT (uint16_t)(-1)
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

static void
output_decomposition (FILE *stream1, FILE *stream2)
{
  struct decomp_table t;
  unsigned int level1_offset, level2_offset, level3_offset;
  unsigned int offset;
  unsigned int ch;
  unsigned int i;

  t.p = 5;
  t.q = 5;
  decomp_table_init (&t);

  fprintf (stream1, "extern const unsigned char gl_uninorm_decomp_chars_table[];\n");
  fprintf (stream1, "\n");
  fprintf (stream2, "const unsigned char gl_uninorm_decomp_chars_table[] =\n{");
  offset = 0;

  for (ch = 0; ch < 0x110000; ch++)
    {
      unsigned int length;
      unsigned int decomposed[MAX_DECOMP_LENGTH];
      int type = get_decomposition (ch, &length, decomposed);

      if (type >= 0)
        {
          assert (offset < (1 << 15));
          decomp_table_add (&t, ch, ((type == UC_DECOMP_CANONICAL ? 0 : 1) << 15) | offset);

          /* Produce length 3-bytes entries.  */
          /* We would need a special representation of zero-length entries.  */
          assert (length != 0);
          for (i = 0; i < length; i++)
            {
              if (offset > 0)
                fprintf (stream2, ",");
              if ((offset % 4) == 0)
                fprintf (stream2, "\n ");
              assert (decomposed[i] < (1 << 18));
              fprintf (stream2, " 0x%02X, 0x%02X, 0x%02X",
                       (((i+1 < length ? (1 << 23) : 0)
                         | (i == 0 ? (type << 18) : 0)
                         | decomposed[i]) >> 16) & 0xff,
                       (decomposed[i] >> 8) & 0xff,
                       decomposed[i] & 0xff);
              offset++;
            }
        }
    }

  fprintf (stream2, "\n};\n");
  fprintf (stream2, "\n");

  decomp_table_finalize (&t);

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
    fprintf (stream1, "#define decomp_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream1, "\n");
  fprintf (stream1, "typedef struct\n");
  fprintf (stream1, "  {\n");
  fprintf (stream1, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream1, "    int level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream1, "    unsigned short level3[%zu << %d];\n", t.level3_size, t.p);
  fprintf (stream1, "  }\n");
  fprintf (stream1, "decomp_index_table_t;\n");
  fprintf (stream1, "extern const decomp_index_table_t gl_uninorm_decomp_index_table;\n");
  fprintf (stream2, "const decomp_index_table_t gl_uninorm_decomp_index_table =\n");
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
      if (offset == 0)
        fprintf (stream2, " %5d", -1);
      else
        fprintf (stream2, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream2, ",");
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
      if (offset == 0)
        fprintf (stream2, " %5d", -1);
      else
        fprintf (stream2, " %5zu",
                 (offset - level3_offset) / sizeof (uint16_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream2, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream2, "\n ");
  fprintf (stream2, " },\n");
  fprintf (stream2, "  {");
  if (t.level3_size << t.p > 8)
    fprintf (stream2, "\n   ");
  for (i = 0; i < t.level3_size << t.p; i++)
    {
      uint16_t value = ((uint16_t *) (t.result + level3_offset))[i];
      if (i > 0 && (i % 8) == 0)
        fprintf (stream2, "\n   ");
      fprintf (stream2, " %5d", value == (uint16_t)(-1) ? -1 : value);
      if (i+1 < t.level3_size << t.p)
        fprintf (stream2, ",");
    }
  if (t.level3_size << t.p > 8)
    fprintf (stream2, "\n ");
  fprintf (stream2, " }\n");
  fprintf (stream2, "};\n");
}

static void
output_decomposition_tables (const char *filename1, const char *filename2, const char *version)
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
      fprintf (stream, "/* Decomposition of Unicode characters.  */\n");
      fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
               version);
      fprintf (stream, "\n");

      fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
      fprintf (stream, "\n");
      output_library_license (stream, true);
      fprintf (stream, "\n");
    }

  output_decomposition (streams[0], streams[1]);

  for (i = 0; i < 2; i++)
    {
      if (ferror (streams[i]) || fclose (streams[i]))
        {
          fprintf (stderr, "error writing to '%s'\n", filenames[i]);
          exit (1);
        }
    }
}

/* The "excluded from composition" property from the CompositionExclusions.txt file.  */
char unicode_composition_exclusions[0x110000];

static void
fill_composition_exclusions (const char *compositionexclusions_filename)
{
  FILE *stream;
  unsigned int i;

  stream = fopen (compositionexclusions_filename, "r");
  if (stream == NULL)
    {
      fprintf (stderr, "error during fopen of '%s'\n", compositionexclusions_filename);
      exit (1);
    }

  for (i = 0; i < 0x110000; i++)
    unicode_composition_exclusions[i] = 0;

  for (;;)
    {
      char buf[200+1];
      unsigned int i;

      if (fscanf (stream, "%200[^\n]\n", buf) < 1)
        break;

      if (buf[0] == '\0' || buf[0] == '#')
        continue;

      if (sscanf (buf, "%X", &i) != 1)
        {
          fprintf (stderr, "parse error in '%s'\n", compositionexclusions_filename);
          exit (1);
        }
      assert (i < 0x110000);

      unicode_composition_exclusions[i] = 1;
    }

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error reading from '%s'\n", compositionexclusions_filename);
      exit (1);
    }
}

static void
debug_output_composition_tables (const char *filename)
{
  FILE *stream;
  unsigned int ch;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  for (ch = 0; ch < 0x110000; ch++)
    {
      unsigned int length;
      unsigned int decomposed[MAX_DECOMP_LENGTH];
      int type = get_decomposition (ch, &length, decomposed);

      if (type == UC_DECOMP_CANONICAL
          /* Consider only binary decompositions.
             Exclude singleton decompositions.  */
          && length == 2)
        {
          unsigned int code1 = decomposed[0];
          unsigned int code2 = decomposed[1];
          unsigned int combined = ch;

          /* Exclude decompositions where the first part is not a starter,
             i.e. is not of canonical combining class 0.  */
          if (strcmp (unicode_attributes[code1].combining, "0") == 0
              /* Exclude characters listed in CompositionExclusions.txt.  */
              && !unicode_composition_exclusions[combined])
            {
              /* The combined character must now also be a starter.
                 Verify this.  */
              assert (strcmp (unicode_attributes[combined].combining, "0") == 0);

              fprintf (stream, "0x%04X\t0x%04X\t0x%04X\t%s\n",
                       code1,
                       code2,
                       combined,
                       unicode_attributes[code2].combining);
            }
        }
    }

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

static void
output_composition_tables (const char *filename, const char *version)
{
  FILE *stream;
  unsigned int ch;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Canonical composition of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2009-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, true);
  fprintf (stream, "\n");

  /* The composition table is a set of mappings (code1, code2) -> combined,
     with 928 entries,
     367 values for code1 (from 0x003C to 0x30FD),
      54 values for code2 (from 0x0300 to 0x309A).
     For a fixed code1, there are from 1 to 19 possible values for code2.
     For a fixed code2, there are from 1 to 117 possible values for code1.
     This is a very sparse matrix.

     We want an O(1) hash lookup.

     We could implement the hash lookup by mapping (code1, code2) to a linear
     combination  mul1*code1 + mul2*code2, which is then used as an index into
     a 3-level table.  But this leads to a table of size 37 KB.

     We use gperf to implement the hash lookup, giving it the 928 sets of
     4 bytes (code1, code2) as input.  gperf generates a hash table of size
     1527, which is quite good (60% filled).  It requires an auxiliary table
     lookup in a table of size 0.5 KB.  The total tables size is 11 KB.  */

  fprintf (stream, "struct composition_rule { char codes[6]; };\n");
  fprintf (stream, "%%struct-type\n");
  fprintf (stream, "%%language=ANSI-C\n");
  fprintf (stream, "%%define slot-name codes\n");
  fprintf (stream, "%%define hash-function-name gl_uninorm_compose_hash\n");
  fprintf (stream, "%%define lookup-function-name gl_uninorm_compose_lookup\n");
  fprintf (stream, "%%compare-lengths\n");
  fprintf (stream, "%%compare-strncmp\n");
  fprintf (stream, "%%readonly-tables\n");
  fprintf (stream, "%%omit-struct-type\n");
  fprintf (stream, "%%%%\n");

  for (ch = 0; ch < 0x110000; ch++)
    {
      unsigned int length;
      unsigned int decomposed[MAX_DECOMP_LENGTH];
      int type = get_decomposition (ch, &length, decomposed);

      if (type == UC_DECOMP_CANONICAL
          /* Consider only binary decompositions.
             Exclude singleton decompositions.  */
          && length == 2)
        {
          unsigned int code1 = decomposed[0];
          unsigned int code2 = decomposed[1];
          unsigned int combined = ch;

          /* Exclude decompositions where the first part is not a starter,
             i.e. is not of canonical combining class 0.  */
          if (strcmp (unicode_attributes[code1].combining, "0") == 0
              /* Exclude characters listed in CompositionExclusions.txt.  */
              && !unicode_composition_exclusions[combined])
            {
              /* The combined character must now also be a starter.
                 Verify this.  */
              assert (strcmp (unicode_attributes[combined].combining, "0") == 0);

              fprintf (stream, "\"\\x%02x\\x%02x\\x%02x\\x%02x\\x%02x\\x%02x\", 0x%04x\n",
                       (code1 >> 16) & 0xff, (code1 >> 8) & 0xff, code1 & 0xff,
                       (code2 >> 16) & 0xff, (code2 >> 8) & 0xff, code2 & 0xff,
                       combined);
            }
        }
    }

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Output the test for a simple character mapping table to the given file.  */

static void
output_simple_mapping_test (const char *filename,
                            const char *function_name,
                            unsigned int (*func) (unsigned int),
                            const char *version)
{
  FILE *stream;
  bool need_comma;
  unsigned int ch;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Test the Unicode character mapping functions.\n");
  fprintf (stream, "   Copyright (C) 2009-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_tests_license (stream);
  fprintf (stream, "\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");
  fprintf (stream, "#include \"test-mapping-part1.h\"\n");
  fprintf (stream, "\n");

  need_comma = false;
  for (ch = 0; ch < 0x110000; ch++)
    {
      unsigned int value = func (ch);

      if (value != ch)
        {
          if (need_comma)
            fprintf (stream, ",\n");
          fprintf (stream, "    { 0x%04X, 0x%04X }", ch, value);
          need_comma = true;
        }
    }
  if (need_comma)
    fprintf (stream, "\n");

  fprintf (stream, "\n");
  fprintf (stream, "#define MAP(c) %s (c)\n", function_name);
  fprintf (stream, "#include \"test-mapping-part2.h\"\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* Construction of sparse 3-level tables.  */
#define TABLE mapping_table
#define ELEMENT int32_t
#define DEFAULT 0
#define xmalloc malloc
#define xrealloc realloc
#include "3level.h"

/* Output a simple character mapping table to the given file.  */

static void
output_simple_mapping (const char *filename,
                       unsigned int (*func) (unsigned int),
                       const char *version)
{
  FILE *stream;
  unsigned int ch, i;
  struct mapping_table t;
  unsigned int level1_offset, level2_offset, level3_offset;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Simple character mapping of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, false);
  fprintf (stream, "\n");

  t.p = 7;
  t.q = 9;
  mapping_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      int value = (int) func (ch) - (int) ch;

      mapping_table_add (&t, ch, value);
    }

  mapping_table_finalize (&t);

  /* Offsets in t.result, in memory of this process.  */
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
    fprintf (stream, "#define mapping_header_%d %d\n", i,
             ((uint32_t *) t.result)[i]);
  fprintf (stream, "static const\n");
  fprintf (stream, "struct\n");
  fprintf (stream, "  {\n");
  fprintf (stream, "    int level1[%zu];\n", t.level1_size);
  fprintf (stream, "    short level2[%zu << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    int level3[%zu << %d];\n", t.level3_size, t.p);
  fprintf (stream, "  }\n");
  fprintf (stream, "u_mapping =\n");
  fprintf (stream, "{\n");
  fprintf (stream, "  {");
  if (t.level1_size > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level1_size; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level1_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level2_offset) / sizeof (uint32_t));
      if (i+1 < t.level1_size)
        fprintf (stream, ",");
    }
  if (t.level1_size > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level2_size << t.q; i++)
    {
      uint32_t offset;
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      offset = ((uint32_t *) (t.result + level2_offset))[i];
      if (offset == 0)
        fprintf (stream, " %5d", -1);
      else
        fprintf (stream, " %5zu",
                 (offset - level3_offset) / sizeof (int32_t));
      if (i+1 < t.level2_size << t.q)
        fprintf (stream, ",");
    }
  if (t.level2_size << t.q > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " },\n");
  fprintf (stream, "  {");
  if (t.level3_size << t.p > 8)
    fprintf (stream, "\n   ");
  for (i = 0; i < t.level3_size << t.p; i++)
    {
      if (i > 0 && (i % 8) == 0)
        fprintf (stream, "\n   ");
      fprintf (stream, " %5d", ((int32_t *) (t.result + level3_offset))[i]);
      if (i+1 < t.level3_size << t.p)
        fprintf (stream, ",");
    }
  if (t.level3_size << t.p > 8)
    fprintf (stream, "\n ");
  fprintf (stream, " }\n");
  fprintf (stream, "};\n");

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* A special casing context.
   A context is negated through x -> -x.  */
enum
{
  SCC_ALWAYS             = 0,
  SCC_FINAL_SIGMA,
  SCC_AFTER_SOFT_DOTTED,
  SCC_MORE_ABOVE,
  SCC_BEFORE_DOT,
  SCC_AFTER_I
};

/* A special casing rule.  */
struct special_casing_rule
{
  unsigned int code;
  unsigned int lower_mapping[3];
  unsigned int title_mapping[3];
  unsigned int upper_mapping[3];
  unsigned int casefold_mapping[3];
  const char *language;
  int context;
};

/* The special casing rules.  */
struct special_casing_rule **casing_rules;
unsigned int num_casing_rules;
unsigned int allocated_casing_rules;

static void
add_casing_rule (struct special_casing_rule *new_rule)
{
  if (num_casing_rules == allocated_casing_rules)
    {
      allocated_casing_rules = 2 * allocated_casing_rules;
      if (allocated_casing_rules < 16)
        allocated_casing_rules = 16;
      casing_rules =
        (struct special_casing_rule **)
        realloc (casing_rules, allocated_casing_rules * sizeof (struct special_casing_rule *));
    }
  casing_rules[num_casing_rules++] = new_rule;
}

/* Stores in casing_rules the special casing rules found in
   specialcasing_filename.  */
static void
fill_casing_rules (const char *specialcasing_filename)
{
  FILE *stream;

  stream = fopen (specialcasing_filename, "r");
  if (stream == NULL)
    {
      fprintf (stderr, "error during fopen of '%s'\n", specialcasing_filename);
      exit (1);
    }

  casing_rules = NULL;
  num_casing_rules = 0;
  allocated_casing_rules = 0;

  for (;;)
    {
      char buf[200+1];
      char *scanptr;
      char *endptr;
      int i;

      unsigned int code;
      unsigned int lower_mapping[3];
      unsigned int title_mapping[3];
      unsigned int upper_mapping[3];
      char *language;
      int context;

      if (fscanf (stream, "%200[^\n]\n", buf) < 1)
        break;

      if (buf[0] == '\0' || buf[0] == '#')
        continue;

      /* Scan code.  */
      scanptr = buf;
      code = strtoul (scanptr, &endptr, 16);
      if (endptr == scanptr)
        {
          fprintf (stderr, "parse error in '%s'\n", specialcasing_filename);
          exit (1);
        }
      scanptr = endptr;
      if (*scanptr != ';')
        {
          fprintf (stderr, "parse error in '%s'\n", specialcasing_filename);
          exit (1);
        }
      scanptr++;

      /* Scan lower mapping.  */
      for (i = 0; i < 3; i++)
        lower_mapping[i] = 0;
      for (i = 0; i < 3; i++)
        {
          while (*scanptr == ' ')
            scanptr++;
          if (*scanptr == ';')
            break;
          lower_mapping[i] = strtoul (scanptr, &endptr, 16);
          if (endptr == scanptr)
            {
              fprintf (stderr, "parse error in '%s'\n", specialcasing_filename);
              exit (1);
            }
          scanptr = endptr;
        }
      if (*scanptr != ';')
        {
          fprintf (stderr, "parse error in '%s'\n", specialcasing_filename);
          exit (1);
        }
      scanptr++;

      /* Scan title mapping.  */
      for (i = 0; i < 3; i++)
        title_mapping[i] = 0;
      for (i = 0; i < 3; i++)
        {
          while (*scanptr == ' ')
            scanptr++;
          if (*scanptr == ';')
            break;
          title_mapping[i] = strtoul (scanptr, &endptr, 16);
          if (endptr == scanptr)
            {
              fprintf (stderr, "parse error in '%s'\n", specialcasing_filename);
              exit (1);
            }
          scanptr = endptr;
        }
      if (*scanptr != ';')
        {
          fprintf (stderr, "parse error in '%s'\n", specialcasing_filename);
          exit (1);
        }
      scanptr++;

      /* Scan upper mapping.  */
      for (i = 0; i < 3; i++)
        upper_mapping[i] = 0;
      for (i = 0; i < 3; i++)
        {
          while (*scanptr == ' ')
            scanptr++;
          if (*scanptr == ';')
            break;
          upper_mapping[i] = strtoul (scanptr, &endptr, 16);
          if (endptr == scanptr)
            {
              fprintf (stderr, "parse error in '%s'\n", specialcasing_filename);
              exit (1);
            }
          scanptr = endptr;
        }
      if (*scanptr != ';')
        {
          fprintf (stderr, "parse error in '%s'\n", specialcasing_filename);
          exit (1);
        }
      scanptr++;

      /* Scan language and context.  */
      language = NULL;
      context = SCC_ALWAYS;
      while (*scanptr == ' ')
        scanptr++;
      if (*scanptr != '\0' && *scanptr != '#')
        {
          const char *word_begin = scanptr;
          const char *word_end;

          while (*scanptr != '\0' && *scanptr != '#' && *scanptr != ';' && *scanptr != ' ')
            scanptr++;
          word_end = scanptr;

          while (*scanptr == ' ')
            scanptr++;

          if (word_end - word_begin == 2)
            {
              language = (char *) malloc ((word_end - word_begin) + 1);
              memcpy (language, word_begin, 2);
              language[word_end - word_begin] = '\0';
              word_begin = word_end = NULL;

              if (*scanptr != '\0' && *scanptr != '#' &&  *scanptr != ';')
                {
                  word_begin = scanptr;
                  while (*scanptr != '\0' && *scanptr != '#' && *scanptr != ';' && *scanptr != ' ')
                    scanptr++;
                  word_end = scanptr;
                }
            }

          if (word_end > word_begin)
            {
              bool negate = false;

              if (word_end - word_begin >= 4 && memcmp (word_begin, "Not_", 4) == 0)
                {
                  word_begin += 4;
                  negate = true;
                }
              if (word_end - word_begin == 11 && memcmp (word_begin, "Final_Sigma", 11) == 0)
                context = SCC_FINAL_SIGMA;
              else if (word_end - word_begin == 17 && memcmp (word_begin, "After_Soft_Dotted", 17) == 0)
                context = SCC_AFTER_SOFT_DOTTED;
              else if (word_end - word_begin == 10 && memcmp (word_begin, "More_Above", 10) == 0)
                context = SCC_MORE_ABOVE;
              else if (word_end - word_begin == 10 && memcmp (word_begin, "Before_Dot", 10) == 0)
                context = SCC_BEFORE_DOT;
              else if (word_end - word_begin == 7 && memcmp (word_begin, "After_I", 7) == 0)
                context = SCC_AFTER_I;
              else
                {
                  fprintf (stderr, "unknown context type in '%s'\n", specialcasing_filename);
                  exit (1);
                }
              if (negate)
                context = - context;
            }

          if (*scanptr != '\0' && *scanptr != '#' &&  *scanptr != ';')
            {
              fprintf (stderr, "parse error in '%s'\n", specialcasing_filename);
              exit (1);
            }
        }

      /* Store the rule.  */
      {
        struct special_casing_rule *new_rule =
          (struct special_casing_rule *) malloc (sizeof (struct special_casing_rule));
        new_rule->code = code;
        new_rule->language = language;
        new_rule->context = context;
        memcpy (new_rule->lower_mapping, lower_mapping, sizeof (new_rule->lower_mapping));
        memcpy (new_rule->title_mapping, title_mapping, sizeof (new_rule->title_mapping));
        memcpy (new_rule->upper_mapping, upper_mapping, sizeof (new_rule->upper_mapping));

        add_casing_rule (new_rule);
      }
    }

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error reading from '%s'\n", specialcasing_filename);
      exit (1);
    }
}

/* A casefolding rule.  */
struct casefold_rule
{
  unsigned int code;
  unsigned int mapping[3];
  const char *language;
};

/* The casefolding rules.  */
struct casefold_rule **casefolding_rules;
unsigned int num_casefolding_rules;
unsigned int allocated_casefolding_rules;

/* Stores in casefolding_rules the case folding rules found in
   casefolding_filename.  */
static void
fill_casefolding_rules (const char *casefolding_filename)
{
  FILE *stream;

  stream = fopen (casefolding_filename, "r");
  if (stream == NULL)
    {
      fprintf (stderr, "error during fopen of '%s'\n", casefolding_filename);
      exit (1);
    }

  casefolding_rules = NULL;
  num_casefolding_rules = 0;
  allocated_casefolding_rules = 0;

  for (;;)
    {
      char buf[200+1];
      char *scanptr;
      char *endptr;
      int i;

      unsigned int code;
      char type;
      unsigned int mapping[3];

      if (fscanf (stream, "%200[^\n]\n", buf) < 1)
        break;

      if (buf[0] == '\0' || buf[0] == '#')
        continue;

      /* Scan code.  */
      scanptr = buf;
      code = strtoul (scanptr, &endptr, 16);
      if (endptr == scanptr)
        {
          fprintf (stderr, "parse error in '%s'\n", casefolding_filename);
          exit (1);
        }
      scanptr = endptr;
      if (*scanptr != ';')
        {
          fprintf (stderr, "parse error in '%s'\n", casefolding_filename);
          exit (1);
        }
      scanptr++;

      /* Scan type.  */
      while (*scanptr == ' ')
        scanptr++;

      switch (*scanptr)
        {
        case 'C': case 'F': case 'S': case 'T':
          type = *scanptr;
          break;
        default:
          fprintf (stderr, "parse error in '%s'\n", casefolding_filename);
          exit (1);
        }
      scanptr++;
      if (*scanptr != ';')
        {
          fprintf (stderr, "parse error in '%s'\n", casefolding_filename);
          exit (1);
        }
      scanptr++;

      /* Scan casefold mapping.  */
      for (i = 0; i < 3; i++)
        mapping[i] = 0;
      for (i = 0; i < 3; i++)
        {
          while (*scanptr == ' ')
            scanptr++;
          if (*scanptr == ';')
            break;
          mapping[i] = strtoul (scanptr, &endptr, 16);
          if (endptr == scanptr)
            {
              fprintf (stderr, "parse error in '%s'\n", casefolding_filename);
              exit (1);
            }
          scanptr = endptr;
        }
      if (*scanptr != ';')
        {
          fprintf (stderr, "parse error in '%s'\n", casefolding_filename);
          exit (1);
        }
      scanptr++;

      /* Ignore rules of type 'S'; we use the rules of type 'F' instead.  */
      if (type != 'S')
        {
          const char * const *languages;
          unsigned int languages_count;

          /* Type 'T' indicates that the rule is applicable to Turkish
             languages only.  */
          if (type == 'T')
            {
              static const char * const turkish_languages[] = { "tr", "az" };
              languages = turkish_languages;
              languages_count = 2;
            }
          else
            {
              static const char * const all_languages[] = { NULL };
              languages = all_languages;
              languages_count = 1;
            }

          for (i = 0; i < languages_count; i++)
            {
              /* Store a new rule.  */
              struct casefold_rule *new_rule =
                (struct casefold_rule *) malloc (sizeof (struct casefold_rule));
              new_rule->code = code;
              memcpy (new_rule->mapping, mapping, sizeof (new_rule->mapping));
              new_rule->language = languages[i];

              if (num_casefolding_rules == allocated_casefolding_rules)
                {
                  allocated_casefolding_rules = 2 * allocated_casefolding_rules;
                  if (allocated_casefolding_rules < 16)
                    allocated_casefolding_rules = 16;
                  casefolding_rules =
                    (struct casefold_rule **)
                    realloc (casefolding_rules,
                             allocated_casefolding_rules * sizeof (struct casefold_rule *));
                }
              casefolding_rules[num_casefolding_rules++] = new_rule;
            }
        }
    }

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error reading from '%s'\n", casefolding_filename);
      exit (1);
    }
}

/* Casefold mapping, when it maps to a single character.  */
unsigned int unicode_casefold[0x110000];

static unsigned int
to_casefold (unsigned int ch)
{
  return unicode_casefold[ch];
}

/* Redistribute the casefolding_rules:
   - Rules that map to a single character, language independently, are stored
     in unicode_casefold.
   - Other rules are merged into casing_rules.  */
static void
redistribute_casefolding_rules (void)
{
  unsigned int ch, i, j;

  /* Fill unicode_casefold[].  */
  for (ch = 0; ch < 0x110000; ch++)
    unicode_casefold[ch] = ch;
  for (i = 0; i < num_casefolding_rules; i++)
    {
      struct casefold_rule *cfrule = casefolding_rules[i];

      if (cfrule->language == NULL && cfrule->mapping[1] == 0)
        {
          ch = cfrule->code;
          assert (ch < 0x110000);
          unicode_casefold[ch] = cfrule->mapping[0];
        }
    }

  /* Extend the special casing rules by filling in their casefold_mapping[]
     field.  */
  for (j = 0; j < num_casing_rules; j++)
    {
      struct special_casing_rule *rule = casing_rules[j];
      unsigned int k;

      rule->casefold_mapping[0] = to_casefold (rule->code);
      for (k = 1; k < 3; k++)
        rule->casefold_mapping[k] = 0;
    }

  /* Now merge the other casefolding rules into casing_rules.  */
  for (i = 0; i < num_casefolding_rules; i++)
    {
      struct casefold_rule *cfrule = casefolding_rules[i];

      if (!(cfrule->language == NULL && cfrule->mapping[1] == 0))
        {
          /* Find a rule that applies to the same code, same language, and it
             has context SCC_ALWAYS.  At the same time, update all rules that
             have the same code and same or more specific language.  */
          struct special_casing_rule *found_rule = NULL;

          for (j = 0; j < num_casing_rules; j++)
            {
              struct special_casing_rule *rule = casing_rules[j];

              if (rule->code == cfrule->code
                  && (cfrule->language == NULL
                      || (rule->language != NULL
                          && strcmp (rule->language, cfrule->language) == 0)))
                {
                  memcpy (rule->casefold_mapping, cfrule->mapping,
                          sizeof (rule->casefold_mapping));

                  if ((cfrule->language == NULL
                       ? rule->language == NULL
                       : rule->language != NULL
                         && strcmp (rule->language, cfrule->language) == 0)
                      && rule->context == SCC_ALWAYS)
                    {
                      /* Found it.  */
                      found_rule = rule;
                    }
                }
            }

          if (found_rule == NULL)
            {
              /* Create a new rule.  */
              struct special_casing_rule *new_rule =
                (struct special_casing_rule *) malloc (sizeof (struct special_casing_rule));

              /* Try to find a rule that applies to the same code, no language
                 restriction, and with context SCC_ALWAYS.  */
              for (j = 0; j < num_casing_rules; j++)
                {
                  struct special_casing_rule *rule = casing_rules[j];

                  if (rule->code == cfrule->code
                      && rule->context == SCC_ALWAYS
                      && rule->language == NULL)
                    {
                      /* Found it.  */
                      found_rule = rule;
                      break;
                    }
                }

              new_rule->code = cfrule->code;
              new_rule->language = cfrule->language;
              new_rule->context = SCC_ALWAYS;
              if (found_rule != NULL)
                {
                  memcpy (new_rule->lower_mapping, found_rule->lower_mapping,
                          sizeof (new_rule->lower_mapping));
                  memcpy (new_rule->title_mapping, found_rule->title_mapping,
                          sizeof (new_rule->title_mapping));
                  memcpy (new_rule->upper_mapping, found_rule->upper_mapping,
                          sizeof (new_rule->upper_mapping));
                }
              else
                {
                  unsigned int k;

                  new_rule->lower_mapping[0] = to_lower (cfrule->code);
                  for (k = 1; k < 3; k++)
                    new_rule->lower_mapping[k] = 0;
                  new_rule->title_mapping[0] = to_title (cfrule->code);
                  for (k = 1; k < 3; k++)
                    new_rule->title_mapping[k] = 0;
                  new_rule->upper_mapping[0] = to_upper (cfrule->code);
                  for (k = 1; k < 3; k++)
                    new_rule->upper_mapping[k] = 0;
                }
              memcpy (new_rule->casefold_mapping, cfrule->mapping,
                      sizeof (new_rule->casefold_mapping));

              add_casing_rule (new_rule);
            }
        }
    }
}

static int
compare_casing_rules (const void *a, const void *b)
{
  struct special_casing_rule *a_rule = *(struct special_casing_rule **) a;
  struct special_casing_rule *b_rule = *(struct special_casing_rule **) b;
  unsigned int a_code = a_rule->code;
  unsigned int b_code = b_rule->code;

  if (a_code < b_code)
    return -1;
  if (a_code > b_code)
    return 1;

  /* Sort the more specific rules before the more general ones.  */
  return (- ((a_rule->language != NULL ? 1 : 0) + (a_rule->context != SCC_ALWAYS ? 1 : 0))
          + ((b_rule->language != NULL ? 1 : 0) + (b_rule->context != SCC_ALWAYS ? 1 : 0)));
}

static void
sort_casing_rules (void)
{
  /* Sort the rules 1. by code, 2. by specificity.  */
  if (num_casing_rules > 1)
    qsort (casing_rules, num_casing_rules, sizeof (struct special_casing_rule *),
           compare_casing_rules);
}

/* Output the special casing rules.  */
static void
output_casing_rules (const char *filename, const char *version)
{
  FILE *stream;
  unsigned int i, j;
  unsigned int minor;

  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* Special casing rules of Unicode characters.  */\n");
  fprintf (stream, "/* Generated automatically by gen-uni-tables.c for Unicode %s.  */\n",
           version);
  fprintf (stream, "\n");

  fprintf (stream, "/* Copyright (C) 2000-2022 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  output_library_license (stream, false);
  fprintf (stream, "\n");

  fprintf (stream, "struct special_casing_rule { char code[3]; };\n");
  fprintf (stream, "%%struct-type\n");
  fprintf (stream, "%%language=ANSI-C\n");
  fprintf (stream, "%%define slot-name code\n");
  fprintf (stream, "%%define hash-function-name gl_unicase_special_hash\n");
  fprintf (stream, "%%define lookup-function-name gl_unicase_special_lookup\n");
  fprintf (stream, "%%compare-lengths\n");
  fprintf (stream, "%%compare-strncmp\n");
  fprintf (stream, "%%readonly-tables\n");
  fprintf (stream, "%%omit-struct-type\n");
  fprintf (stream, "%%%%\n");

  minor = 0;
  for (i = 0; i < num_casing_rules; i++)
    {
      struct special_casing_rule *rule = casing_rules[i];
      int context;

      if (i > 0 && rule->code == casing_rules[i - 1]->code)
        minor += 1;
      else
        minor = 0;

      if (!(rule->code < 0x10000))
        {
          fprintf (stderr, "special rule #%u: code %u out of range\n", i, rule->code);
          exit (1);
        }

      fprintf (stream, "\"\\x%02x\\x%02x\\x%02x\", ",
               (rule->code >> 8) & 0xff, rule->code & 0xff, minor);

      fprintf (stream, "%d, ",
               i + 1 < num_casing_rules && casing_rules[i + 1]->code == rule->code ? 1 : 0);

      context = rule->context;
      if (context < 0)
        {
          fprintf (stream, "-");
          context = - context;
        }
      else
        fprintf (stream, " ");
      switch (context)
        {
        case SCC_ALWAYS:
          fprintf (stream, "SCC_ALWAYS           ");
          break;
        case SCC_FINAL_SIGMA:
          fprintf (stream, "SCC_FINAL_SIGMA      ");
          break;
        case SCC_AFTER_SOFT_DOTTED:
          fprintf (stream, "SCC_AFTER_SOFT_DOTTED");
          break;
        case SCC_MORE_ABOVE:
          fprintf (stream, "SCC_MORE_ABOVE       ");
          break;
        case SCC_BEFORE_DOT:
          fprintf (stream, "SCC_BEFORE_DOT       ");
          break;
        case SCC_AFTER_I:
          fprintf (stream, "SCC_AFTER_I          ");
          break;
        default:
          abort ();
        }
      fprintf (stream, ", ");

      if (rule->language != NULL)
        {
          assert (strlen (rule->language) == 2);
          fprintf (stream, "{  '%c',  '%c' }, ", rule->language[0], rule->language[1]);
        }
      else
        fprintf (stream, "{ '\\0', '\\0' }, ");

      fprintf (stream, "{ ");
      for (j = 0; j < 3; j++)
        {
          if (j > 0)
            fprintf (stream, ", ");
          if (!(rule->upper_mapping[j] < 0x10000))
            {
              fprintf (stderr, "special rule #%u: upper mapping of code %u out of range\n", i, rule->code);
              exit (1);
            }
          if (rule->upper_mapping[j] != 0)
            fprintf (stream, "0x%04X", rule->upper_mapping[j]);
          else
            fprintf (stream, "     0");
        }
      fprintf (stream, " }, { ");
      for (j = 0; j < 3; j++)
        {
          if (j > 0)
            fprintf (stream, ", ");
          if (!(rule->lower_mapping[j] < 0x10000))
            {
              fprintf (stderr, "special rule #%u: lower mapping of code %u out of range\n", i, rule->code);
              exit (1);
            }
          if (rule->lower_mapping[j] != 0)
            fprintf (stream, "0x%04X", rule->lower_mapping[j]);
          else
            fprintf (stream, "     0");
        }
      fprintf (stream, " }, { ");
      for (j = 0; j < 3; j++)
        {
          if (j > 0)
            fprintf (stream, ", ");
          if (!(rule->title_mapping[j] < 0x10000))
            {
              fprintf (stderr, "special rule #%u: title mapping of code %u out of range\n", i, rule->code);
              exit (1);
            }
          if (rule->title_mapping[j] != 0)
            fprintf (stream, "0x%04X", rule->title_mapping[j]);
          else
            fprintf (stream, "     0");
        }
      fprintf (stream, " }, { ");
      for (j = 0; j < 3; j++)
        {
          if (j > 0)
            fprintf (stream, ", ");
          if (!(rule->casefold_mapping[j] < 0x10000))
            {
              fprintf (stderr, "special rule #%u: casefold mapping of code %u out of range\n", i, rule->code);
              exit (1);
            }
          if (rule->casefold_mapping[j] != 0)
            fprintf (stream, "0x%04X", rule->casefold_mapping[j]);
          else
            fprintf (stream, "     0");
        }
      fprintf (stream, " }\n");
    }

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }
}

/* ========================================================================= */

/* Quoting the Unicode standard:
     Definition: A character is defined to be "cased" if it has the Lowercase
     or Uppercase property or has a General_Category value of
     Titlecase_Letter.  */
static bool
is_cased (unsigned int ch)
{
  return (is_property_lowercase (ch)
          || is_property_uppercase (ch)
          || is_category_Lt (ch));
}

/* Quoting the Unicode standard:
     Definition: A character is defined to be "case-ignorable" if it has the
     value MidLetter {or the value MidNumLet} for the Word_Break property or
     its General_Category is one of Nonspacing_Mark (Mn), Enclosing_Mark (Me),
     Format (Cf), Modifier_Letter (Lm), or Modifier_Symbol (Sk).
   The text marked in braces was added in Unicode 5.1.0, see
   <https://www.unicode.org/versions/Unicode5.1.0/> section "Update of
   Definition of case-ignorable".   */
/* Since this predicate is only used for the "Before C" and "After C"
   conditions of FINAL_SIGMA, we exclude the "cased" characters here.
   This simplifies the evaluation of the regular expressions
     \p{cased} (\p{case-ignorable})* C
   and
     C (\p{case-ignorable})* \p{cased}
 */
static bool
is_case_ignorable (unsigned int ch)
{
  return (unicode_org_wbp[ch] == WBP_MIDLETTER
          || unicode_org_wbp[ch] == WBP_MIDNUMLET
          || is_category_Mn (ch)
          || is_category_Me (ch)
          || is_category_Cf (ch)
          || is_category_Lm (ch)
          || is_category_Sk (ch))
         && !is_cased (ch);
}

/* ------------------------------------------------------------------------- */

/* Output all case related properties.  */
static void
output_casing_properties (const char *version)
{
#define PROPERTY(FN,P) \
  debug_output_predicate ("unicase/" #FN ".txt", is_ ## P); \
  output_predicate_test ("../tests/unicase/test-" #FN ".c", is_ ## P, "uc_is_" #P " (c)"); \
  output_predicate ("unicase/" #FN ".h", is_ ## P, "u_casing_property_" #P, "Casing Properties", version);
  PROPERTY(cased, cased)
  PROPERTY(ignorable, case_ignorable)
#undef PROPERTY
}

/* ========================================================================= */

int
main (int argc, char * argv[])
{
  const char *unicodedata_filename;
  const char *proplist_filename;
  const char *derivedproplist_filename;
  const char *emojidata_filename;
  const char *arabicshaping_filename;
  const char *scripts_filename;
  const char *blocks_filename;
  const char *proplist30_filename;
  const char *eastasianwidth_filename;
  const char *linebreak_filename;
  const char *wordbreakproperty_filename;
  const char *graphemebreakproperty_filename;
  const char *compositionexclusions_filename;
  const char *specialcasing_filename;
  const char *casefolding_filename;
  const char *version;

  if (argc != 17)
    {
      fprintf (stderr, "Usage: %s UnicodeData.txt PropList.txt DerivedCoreProperties.txt emoji-data.txt ArabicShaping.txt Scripts.txt Blocks.txt PropList-3.0.1.txt EastAsianWidth.txt LineBreak.txt WordBreakProperty.txt GraphemeBreakProperty.txt CompositionExclusions.txt SpecialCasing.txt CaseFolding.txt version\n",
               argv[0]);
      exit (1);
    }

  unicodedata_filename = argv[1];
  proplist_filename = argv[2];
  derivedproplist_filename = argv[3];
  emojidata_filename = argv[4];
  arabicshaping_filename = argv[5];
  scripts_filename = argv[6];
  blocks_filename = argv[7];
  proplist30_filename = argv[8];
  eastasianwidth_filename = argv[9];
  linebreak_filename = argv[10];
  wordbreakproperty_filename = argv[11];
  graphemebreakproperty_filename = argv[12];
  compositionexclusions_filename = argv[13];
  specialcasing_filename = argv[14];
  casefolding_filename = argv[15];
  version = argv[16];

  fill_attributes (unicodedata_filename);
  clear_properties ();
  fill_properties (proplist_filename);
  fill_properties (derivedproplist_filename);
  fill_properties (emojidata_filename);
  fill_properties30 (proplist30_filename);
  fill_arabicshaping (arabicshaping_filename);
  fill_scripts (scripts_filename);
  fill_blocks (blocks_filename);
  fill_width (eastasianwidth_filename);
  fill_org_lbp (linebreak_filename);
  fill_org_wbp (wordbreakproperty_filename);
  fill_org_gbp (graphemebreakproperty_filename);
  fill_composition_exclusions (compositionexclusions_filename);
  fill_casing_rules (specialcasing_filename);
  fill_casefolding_rules (casefolding_filename);
  redistribute_casefolding_rules ();
  sort_casing_rules ();

  output_categories (version);
  output_category ("unictype/categ_of.h", version);
  output_combclass ("unictype/combiningclass.h", version);
  output_bidi_category ("unictype/bidi_of.h", version);
  output_decimal_digit_test ("../tests/unictype/test-decdigit.h", version);
  output_decimal_digit ("unictype/decdigit.h", version);
  output_digit_test ("../tests/unictype/test-digit.h", version);
  output_digit ("unictype/digit.h", version);
  output_numeric_test ("../tests/unictype/test-numeric.h", version);
  output_numeric ("unictype/numeric.h", version);
  output_mirror ("unictype/mirror.h", version);
  output_properties (version);
  output_joining_type_test ("../tests/unictype/test-joiningtype_of.h", version);
  output_joining_type ("unictype/joiningtype_of.h", version);
  output_joining_group_test ("../tests/unictype/test-joininggroup_of.h", version);
  output_joining_group ("unictype/joininggroup_of.h", version);

  output_scripts (version);
  output_scripts_byname (version);
  output_blocks (version);
  output_ident_properties (version);
  output_nonspacing_property ("uniwidth/width0.h", version);
  output_width2_property ("uniwidth/width2.h", version);
  output_width_property_test ("../tests/uniwidth/test-uc_width2.sh.part");
  output_old_ctype (version);

  debug_output_lbrk_tables ("unilbrk/lbrkprop.txt");
  debug_output_org_lbrk_tables ("unilbrk/lbrkprop_org.txt");
  output_lbrk_tables ("unilbrk/lbrkprop1.h", "unilbrk/lbrkprop2.h", version);
  output_lbrk_rules_as_tables ("unilbrk/lbrktables.c", version);

  debug_output_wbrk_tables ("uniwbrk/wbrkprop.txt");
  debug_output_org_wbrk_tables ("uniwbrk/wbrkprop_org.txt");
  output_wbrk_tables ("uniwbrk/wbrkprop.h", version);

  output_gbp_test ("../tests/unigbrk/test-uc-gbrk-prop.h");
  output_gbp_table ("unigbrk/gbrkprop.h", version);

  output_decomposition_tables ("uninorm/decomposition-table1.h", "uninorm/decomposition-table2.h", version);
  debug_output_composition_tables ("uninorm/composition.txt");
  output_composition_tables ("uninorm/composition-table.gperf", version);

  output_simple_mapping_test ("../tests/unicase/test-uc_toupper.c", "uc_toupper", to_upper, version);
  output_simple_mapping_test ("../tests/unicase/test-uc_tolower.c", "uc_tolower", to_lower, version);
  output_simple_mapping_test ("../tests/unicase/test-uc_totitle.c", "uc_totitle", to_title, version);
  output_simple_mapping ("unicase/toupper.h", to_upper, version);
  output_simple_mapping ("unicase/tolower.h", to_lower, version);
  output_simple_mapping ("unicase/totitle.h", to_title, version);
  output_simple_mapping ("unicase/tocasefold.h", to_casefold, version);
  output_casing_rules ("unicase/special-casing-table.gperf", version);
  output_casing_properties (version);

  return 0;
}

/*
 * Local Variables:
 * coding: utf-8
 * compile-command: "\
 *   gcc -O -Wall gen-uni-tables.c -Iunictype -o gen-uni-tables &&      \\
 *   ./gen-uni-tables                                                   \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/UnicodeData.txt \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/PropList.txt \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/DerivedCoreProperties.txt \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/emoji/emoji-data.txt \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/ArabicShaping.txt \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/Scripts.txt \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/Blocks.txt \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/3.0.1/PropList-3.0.1.txt \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/EastAsianWidth.txt \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/LineBreak.txt \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/auxiliary/WordBreakProperty.txt \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/auxiliary/GraphemeBreakProperty.txt \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/CompositionExclusions.txt \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/SpecialCasing.txt \\
 *        /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/CaseFolding.txt \\
 *        14.0.0                                                         \\
 *   && diff unilbrk/lbrkprop_org.txt unilbrk/lbrkprop.txt              \\
 *   && diff uniwbrk/wbrkprop_org.txt uniwbrk/wbrkprop.txt              \\
 *   && clisp -C uniname/gen-uninames.lisp                              \\
 *            /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/UnicodeData.txt \\
 *            uniname/uninames.h                                        \\
 *            /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/NameAliases.txt \\
 *   && cp /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/NameAliases.txt ../tests/uniname/NameAliases.txt \\
 *   && cp /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/UnicodeData.txt ../tests/uniname/UnicodeData.txt \\
 *   && cp /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/NormalizationTest.txt ../tests/uninorm/NormalizationTest.txt \\
 *   && cp /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/auxiliary/GraphemeBreakTest.txt ../tests/unigbrk/GraphemeBreakTest.txt \\
 *   && cp /media/nas/bruno/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/14.0.0/ucd/auxiliary/WordBreakTest.txt ../tests/uniwbrk/WordBreakTest.txt"
 * End:
 */

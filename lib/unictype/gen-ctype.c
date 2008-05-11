/* Generate Unicode conforming character classification tables from a
   UnicodeData file.
   Copyright (C) 2000-2002, 2007-2008 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2000-2002.

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

/* Usage example:
     $ gen-ctype /usr/local/share/Unidata/UnicodeData.txt \
                 /usr/local/share/Unidata/PropList.txt \
                 /usr/local/share/Unidata/DerivedCoreProperties.txt \
                 /usr/local/share/Unidata/Scripts.txt \
                 /usr/local/share/Unidata/Blocks.txt \
                 /usr/local/share/Unidata/PropList-3.0.1.txt \
                 5.0.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    /* Surrogates are UTF-16 artefacts, not real characters. Ignore them.  */
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
	  && strcmp (field1 + strlen(field1) - 8, ", First>") == 0)
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
  fprintf (stream, "   Copyright (C) 2007 Free Software Foundation, Inc.\n");
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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);

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
  fprintf (stream, "    int level1[%d];\n", t.level1_size);
  fprintf (stream, "    short level2[%d << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    /*unsigned*/ int level3[%d << %d];\n", t.level3_size, t.p);
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
	fprintf (stream, " %5d * sizeof (int) / sizeof (short) + %5d",
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
	fprintf (stream, " %5d + %5d * sizeof (short) / sizeof (int) + %5d",
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
      fprintf (stream, " 0x%08X",
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
  debug_output_predicate ("categ_" #C ".txt", is_category_ ## C); \
  output_predicate_test ("test-categ_" #C ".c", is_category_ ## C, "uc_is_general_category (c, UC_CATEGORY_" #C ")"); \
  output_predicate ("categ_" #C ".h", is_category_ ## C, "u_categ_" #C, "Categories", version);
  CATEGORY (L)
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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);

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
      if (value == 0 || ((value & (value - 1)) != 0))
	abort ();

      for (log2_value = 0; value > 1; value >>= 1, log2_value++);

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
  fprintf (stream, "    int level1[%d];\n", t.level1_size);
  fprintf (stream, "    short level2[%d << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned short level3[%d * %d + 1];\n", t.level3_size,
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
	fprintf (stream, " %5d",
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
	fprintf (stream, " %5d",
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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);

  t.p = 7;
  t.q = 9;
  combclass_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    if (unicode_attributes[ch].name != NULL)
      {
	int value = atoi (unicode_attributes[ch].combining);
        if (!(value >= 0 && value <= 255))
	  abort ();
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
  fprintf (stream, "    int level1[%d];\n", t.level1_size);
  fprintf (stream, "    short level2[%d << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned char level3[%d << %d];\n", t.level3_size, t.p);
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
	fprintf (stream, " %5d",
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
	fprintf (stream, " %5d",
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
  UC_BIDI_ON   /* Other Neutral */
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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);

  t.p = 7;
  t.q = 9;
  bidi_category_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      int value = get_bidi_category (ch);

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
  fprintf (stream, "    int level1[%d];\n", t.level1_size);
  fprintf (stream, "    short level2[%d << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned short level3[%d * %d + 1];\n", t.level3_size,
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
	fprintf (stream, " %5d",
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
	fprintf (stream, " %5d",
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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);

  need_comma = false;
  for (ch = 0; ch < 0x110000; ch++)
    {
      int value = get_decdigit_value (ch);

      if (!(value >= -1 && value < 10))
	abort ();

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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);

  t.p = 7;
  t.q = 9;
  decdigit_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      int value = 1 + get_decdigit_value (ch);

      if (!(value >= 0 && value <= 10))
	abort ();

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
  fprintf (stream, "    int level1[%d];\n", t.level1_size);
  fprintf (stream, "    short level2[%d << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned char level3[%d << %d];\n", t.level3_size,
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
	fprintf (stream, " %5d",
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
	fprintf (stream, " %5d",
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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);

  need_comma = false;
  for (ch = 0; ch < 0x110000; ch++)
    {
      int value = get_digit_value (ch);

      if (!(value >= -1 && value < 10))
	abort ();

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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);

  t.p = 7;
  t.q = 9;
  decdigit_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      int value = 1 + get_digit_value (ch);

      if (!(value >= 0 && value <= 10))
	abort ();

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
  fprintf (stream, "    int level1[%d];\n", t.level1_size);
  fprintf (stream, "    short level2[%d << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned char level3[%d << %d];\n", t.level3_size,
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
	fprintf (stream, " %5d",
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
	fprintf (stream, " %5d",
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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);

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
  uc_fraction_t fractions[128];
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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);

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
	  if (nfractions == 128)
	    abort ();
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
      if (i == nfractions)
	abort ();

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
  fprintf (stream, "    int level1[%d];\n", t.level1_size);
  fprintf (stream, "    short level2[%d << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned short level3[%d * %d + 1];\n", t.level3_size,
	   (1 << t.p) * 7 / 16);
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
	fprintf (stream, " %5d",
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
	fprintf (stream, " %5d",
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
      if (mirror_char != 0xfffd)
	abort ();
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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);

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
  fprintf (stream, "    int level1[%d];\n", t.level1_size);
  fprintf (stream, "    short level2[%d << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    int level3[%d << %d];\n", t.level3_size, t.p);
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
	fprintf (stream, " %5d",
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
	fprintf (stream, " %5d",
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
  /* DerivedCoreProperties.txt */
  PROP_MATH,
  PROP_ALPHABETIC,
  PROP_LOWERCASE,
  PROP_UPPERCASE,
  PROP_ID_START,
  PROP_ID_CONTINUE,
  PROP_XID_START,
  PROP_XID_CONTINUE,
  PROP_DEFAULT_IGNORABLE_CODE_POINT,
  PROP_GRAPHEME_EXTEND,
  PROP_GRAPHEME_BASE,
  PROP_GRAPHEME_LINK
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

      if (sscanf (buf, "%X..%X%[ ;]%[^ ]", &i1, &i2, padding, propname) != 4)
	{
	  if (sscanf (buf, "%X%[ ;]%[^ ]", &i1, padding, propname) != 3)
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
      PROP ("STerm", PROP_STERM)
      PROP ("Variation_Selector", PROP_VARIATION_SELECTOR)
      PROP ("Pattern_White_Space", PROP_PATTERN_WHITE_SPACE)
      PROP ("Pattern_Syntax", PROP_PATTERN_SYNTAX)
      /* DerivedCoreProperties.txt */
      PROP ("Math", PROP_MATH)
      PROP ("Alphabetic", PROP_ALPHABETIC)
      PROP ("Lowercase", PROP_LOWERCASE)
      PROP ("Uppercase", PROP_UPPERCASE)
      PROP ("ID_Start", PROP_ID_START)
      PROP ("ID_Continue", PROP_ID_CONTINUE)
      PROP ("XID_Start", PROP_XID_START)
      PROP ("XID_Continue", PROP_XID_CONTINUE)
      PROP ("Default_Ignorable_Code_Point", PROP_DEFAULT_IGNORABLE_CODE_POINT)
      PROP ("Grapheme_Extend", PROP_GRAPHEME_EXTEND)
      PROP ("Grapheme_Base", PROP_GRAPHEME_BASE)
      PROP ("Grapheme_Link", PROP_GRAPHEME_LINK)
#undef PROP
	{
	  fprintf (stderr, "unknown property named '%s' in '%s'\n", propname,
		   proplist_filename);
	  exit (1);
	}
      if (!(i1 <= i2 && i2 < 0x110000))
	abort ();

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
      if (!(i1 <= i2 && i2 < 0x110000))
	abort ();
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
    || (ch >= 0x24D0 && ch <= 0x24E9) /* CIRCLED LATIN SMALL LETTER */
    || (ch == 0x3007) /* IDEOGRAPHIC NUMBER ZERO */
    || (ch >= 0x3021 && ch <= 0x3029) /* HANGZHOU NUMERAL */
    || (ch >= 0x3038 && ch <= 0x303A) /* HANGZHOU NUMERAL */
    || (ch >= 0x10140 && ch <= 0x10174) /* GREEK ACROPHONICS */
    || (ch == 0x10341) /* GOTHIC LETTER NINETY */
    || (ch == 0x1034A) /* GOTHIC LETTER NINE HUNDRED */
    || (ch >= 0x103D1 && ch <= 0x103D5) /* OLD PERSIAN NUMBERS */
    || (ch >= 0x12400 && ch <= 0x12462); /* CUNEIFORM NUMERIC SIGNS */
  bool result2 =
    ((unicode_properties[ch] & (1ULL << PROP_ALPHABETIC)) != 0);

  if (result1 != result2)
    abort ();
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
     && !(ch >= 0xFFF9 && ch <= 0xFFFB)) /* Annotations */
    || ((is_category_Cc (ch) || is_category_Cs (ch))
	&& !is_property_white_space (ch))
    || ((unicode_properties[ch] & (1ULL << PROP_OTHER_DEFAULT_IGNORABLE_CODE_POINT)) != 0)
    || ((unicode_properties[ch] & (1ULL << PROP_VARIATION_SELECTOR)) != 0)
    || is_property_not_a_character (ch);
  bool result2 =
    ((unicode_properties[ch] & (1ULL << PROP_DEFAULT_IGNORABLE_CODE_POINT)) != 0);

  if (result1 != result2)
    abort ();
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

  if (result1 != result2)
    abort ();
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

  if (result1 != result2)
    abort ();
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

  if (result1 != result2)
    abort ();
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

  if (result1 != result2)
    abort ();
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

/* ------------------------------------------------------------------------- */

/* Output all properties.  */
static void
output_properties (const char *version)
{
#define PROPERTY(P) \
  debug_output_predicate ("pr_" #P ".txt", is_property_ ## P); \
  output_predicate_test ("test-pr_" #P ".c", is_property_ ## P, "uc_is_property_" #P " (c)"); \
  output_predicate ("pr_" #P ".h", is_property_ ## P, "u_property_" #P, "Properties", version);
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
#undef PROPERTY
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
      if (i2 < i1)
	abort ();
      if (i2 >= 0x110000)
	abort ();

      for (script = numscripts - 1; script >= 0; script--)
	if (strcmp (scripts[script], scriptname) == 0)
	  break;
      if (script < 0)
	{
	  scripts[numscripts] = strdup (scriptname);
	  script = numscripts;
	  numscripts++;
	  if (numscripts == 256)
	    abort ();
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
  const char *filename = "scripts.h";
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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);

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
  fprintf (stream, "    int level1[%d];\n", t.level1_size);
  fprintf (stream, "    short level2[%d << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned char level3[%d << %d];\n", t.level3_size, t.p);
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
	fprintf (stream, " %5d",
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
	fprintf (stream, " %5d",
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
  const char *filename = "scripts_byname.gperf";
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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);
  fprintf (stream, "struct named_script { const char *name; unsigned int index; };\n");
  fprintf (stream, "%%struct-type\n");
  fprintf (stream, "%%language=ANSI-C\n");
  fprintf (stream, "%%define hash-function-name scripts_hash\n");
  fprintf (stream, "%%define lookup-function-name uc_script_lookup\n");
  fprintf (stream, "%%readonly-tables\n");
  fprintf (stream, "%%global-table\n");
  fprintf (stream, "%%define word-array-name script_names\n");
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
static block_t blocks[256];
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
      if (numblocks > 0 && !(blocks[numblocks-1].end < blocks[numblocks].start))
	abort ();
      numblocks++;
      if (numblocks == 256)
	abort ();
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
  const char *filename = "blocks.h";
  const unsigned int shift = 8; /* bits to shift away for array access */
  const unsigned int threshold = 0x30000; /* cut-off table here to save space */
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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);

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
  fprintf (stream, "static const uint8_t blocks_level1[%d * 2] =\n",
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
   http://java.sun.com/docs/books/jls/third_edition/html/lexical.html#95710  */
static bool
is_java_whitespace (unsigned int ch)
{
  return (ch == ' ' || ch == '\t' || ch == '\f'
	  || ch == '\n' || ch == '\r');
}

/* The Java Language Specification, 3rd edition, §3.8.
   http://java.sun.com/docs/books/jls/third_edition/html/lexical.html#40625
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
  fprintf (stream, "/* Generated automatically by gen-ctype.c for Unicode %s.  */\n",
	   version);

  t.p = 7; /* or 8 */
  t.q = 5; /* or 4 */
  identsyntax_table_init (&t);

  for (ch = 0; ch < 0x110000; ch++)
    {
      int syntaxcode = predicate (ch);
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
  fprintf (stream, "    int level1[%d];\n", t.level1_size);
  fprintf (stream, "    short level2[%d << %d];\n", t.level2_size, t.q);
  fprintf (stream, "    unsigned short level3[%d * %d];\n", t.level3_size,
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
	fprintf (stream, " %5d",
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
	fprintf (stream, " %5d",
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
  debug_output_predicate ("sy_" #P ".txt", is_ ## P); \
  output_predicate_test ("test-sy_" #P ".c", is_ ## P, "uc_is_" #P " (c)"); \
  output_predicate ("sy_" #P ".h", is_ ## P, "u_" #P, "Language syntax properties", version);
  PROPERTY(c_whitespace)
  PROPERTY(java_whitespace)
#undef PROPERTY

  output_ident_category ("sy_c_ident.h", c_ident_category, "u_c_ident", version);
  output_ident_category ("sy_java_ident.h", java_ident_category, "u_java_ident", version);
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
is_outdigit (unsigned int ch)
{
  return (ch >= 0x0030 && ch <= 0x0039);
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
  debug_output_predicate ("ctype_" #P ".txt", is_ ## P); \
  output_predicate_test ("test-ctype_" #P ".c", is_ ## P, "uc_is_" #P " (c)"); \
  output_predicate ("ctype_" #P ".h", is_ ## P, "u_is_" #P, "ISO C <ctype.h> like properties", version);
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
  fprintf (stream, "%% Generated automatically by gen-unicode-ctype for Unicode %s.\n",
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

  /* Verifications. */
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

int
main (int argc, char * argv[])
{
  const char *unicodedata_filename;
  const char *proplist_filename;
  const char *derivedproplist_filename;
  const char *scripts_filename;
  const char *blocks_filename;
  const char *proplist30_filename;
  const char *version;

  if (argc != 8)
    {
      fprintf (stderr, "Usage: %s UnicodeData.txt PropList.txt DerivedCoreProperties.txt Scripts.txt Blocks.txt PropList-3.0.1.txt version\n",
	       argv[0]);
      exit (1);
    }

  unicodedata_filename = argv[1];
  proplist_filename = argv[2];
  derivedproplist_filename = argv[3];
  scripts_filename = argv[4];
  blocks_filename = argv[5];
  proplist30_filename = argv[6];
  version = argv[7];

  fill_attributes (unicodedata_filename);
  clear_properties ();
  fill_properties (proplist_filename);
  fill_properties (derivedproplist_filename);
  fill_properties30 (proplist30_filename);
  fill_scripts (scripts_filename);
  fill_blocks (blocks_filename);

  output_categories (version);
  output_category ("categ_of.h", version);
  output_combclass ("combining.h", version);
  output_bidi_category ("bidi_of.h", version);
  output_decimal_digit_test ("test-decdigit.h", version);
  output_decimal_digit ("decdigit.h", version);
  output_digit_test ("test-digit.h", version);
  output_digit ("digit.h", version);
  output_numeric_test ("test-numeric.h", version);
  output_numeric ("numeric.h", version);
  output_mirror ("mirror.h", version);
  output_properties (version);
  output_scripts (version);
  output_scripts_byname (version);
  output_blocks (version);
  output_ident_properties (version);
  output_old_ctype (version);

  return 0;
}

/*
 * For Emacs M-x compile
 * Local Variables:
 * compile-command: "
   gcc -O -Wall gen-ctype.c -o gen-ctype && \
   ./gen-ctype \
        /gfs/petix/Volumes/ExtData/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/5.0.0/ucd/UnicodeData.txt \
        /gfs/petix/Volumes/ExtData/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/5.0.0/ucd/PropList.txt \
        /gfs/petix/Volumes/ExtData/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/5.0.0/ucd/DerivedCoreProperties.txt \
        /gfs/petix/Volumes/ExtData/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/5.0.0/ucd/Scripts.txt \
        /gfs/petix/Volumes/ExtData/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/5.0.0/ucd/Blocks.txt \
        /gfs/petix/Volumes/ExtData/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions/3.0.1/PropList-3.0.1.txt \
        5.0.0
   "
 * End:
 */

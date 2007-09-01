/* linebreak.c - line breaking of Unicode strings
   Copyright (C) 2001-2003, 2006-2007 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2001.

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

#include <config.h>

/* Specification.  */
#include "linebreak.h"

#include <stdlib.h>
#include <string.h>
#include "c-ctype.h"
#include "xsize.h"
#include "unistr.h"
#include "uniwidth.h"
#include "uniwidth/cjk.h"
#include "streq.h"


static int
is_utf8_encoding (const char *encoding)
{
  if (STREQ (encoding, "UTF-8", 'U', 'T', 'F', '-', '8', 0, 0, 0 ,0))
    return 1;
  return 0;
}


/* Determine the line break points in S, and store the result at p[0..n-1].  */
/* We don't support line breaking of complex-context dependent characters
   (Thai, Lao, Myanmar, Khmer) yet, because it requires dictionary lookup. */

/* Line breaking classification.  */

enum
{
  /* Values >= 20 are resolved at run time. */
  LBP_BK =  0, /* mandatory break */
/*LBP_CR,         carriage return - not used here because it's a DOSism */
/*LBP_LF,         line feed - not used here because it's a DOSism */
  LBP_CM = 20, /* attached characters and combining marks */
/*LBP_SG,         surrogates - not used here because they are not characters */
  LBP_ZW =  1, /* zero width space */
  LBP_IN =  2, /* inseparable */
  LBP_GL =  3, /* non-breaking (glue) */
  LBP_CB = 22, /* contingent break opportunity */
  LBP_SP = 21, /* space */
  LBP_BA =  4, /* break opportunity after */
  LBP_BB =  5, /* break opportunity before */
  LBP_B2 =  6, /* break opportunity before and after */
  LBP_HY =  7, /* hyphen */
  LBP_NS =  8, /* non starter */
  LBP_OP =  9, /* opening punctuation */
  LBP_CL = 10, /* closing punctuation */
  LBP_QU = 11, /* ambiguous quotation */
  LBP_EX = 12, /* exclamation/interrogation */
  LBP_ID = 13, /* ideographic */
  LBP_NU = 14, /* numeric */
  LBP_IS = 15, /* infix separator (numeric) */
  LBP_SY = 16, /* symbols allowing breaks */
  LBP_AL = 17, /* ordinary alphabetic and symbol characters */
  LBP_PR = 18, /* prefix (numeric) */
  LBP_PO = 19, /* postfix (numeric) */
  LBP_SA = 23, /* complex context (South East Asian) */
  LBP_AI = 24, /* ambiguous (alphabetic or ideograph) */
  LBP_XX = 25  /* unknown */
};

#include "lbrkprop.h"

static inline unsigned char
lbrkprop_lookup (unsigned int uc)
{
  unsigned int index1 = uc >> lbrkprop_header_0;
  if (index1 < lbrkprop_header_1)
    {
      int lookup1 = lbrkprop.level1[index1];
      if (lookup1 >= 0)
        {
          unsigned int index2 = (uc >> lbrkprop_header_2) & lbrkprop_header_3;
          int lookup2 = lbrkprop.level2[lookup1 + index2];
          if (lookup2 >= 0)
            {
              unsigned int index3 = uc & lbrkprop_header_4;
              return lbrkprop.level3[lookup2 + index3];
            }
        }
    }
  return LBP_XX;
}

/* Table indexed by two line breaking classifications.  */
#define D 1  /* direct break opportunity, empty in table 7.3 of UTR #14 */
#define I 2  /* indirect break opportunity, '%' in table 7.3 of UTR #14 */
#define P 3  /* prohibited break,           '^' in table 7.3 of UTR #14 */
static const unsigned char lbrk_table[19][19] = {
                                /* after */
        /* ZW IN GL BA BB B2 HY NS OP CL QU EX ID NU IS SY AL PR PO */
/* ZW */ { P, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, },
/* IN */ { P, I, I, I, D, D, I, I, D, P, I, P, D, D, P, P, D, D, D, },
/* GL */ { P, I, I, I, I, I, I, I, I, P, I, P, I, I, P, P, I, I, I, },
/* BA */ { P, D, I, I, D, D, I, I, D, P, I, P, D, D, P, P, D, D, D, },
/* BB */ { P, I, I, I, I, I, I, I, I, P, I, P, I, I, P, P, I, I, I, },
/* B2 */ { P, D, I, I, D, P, I, I, D, P, I, P, D, D, P, P, D, D, D, },
/* HY */ { P, D, I, I, D, D, I, I, D, P, I, P, D, D, P, P, D, D, D, },
/* NS */ { P, D, I, I, D, D, I, I, D, P, I, P, D, D, P, P, D, D, D, },
/* OP */ { P, P, P, P, P, P, P, P, P, P, P, P, P, P, P, P, P, P, P, },
/* CL */ { P, D, I, I, D, D, I, P, D, P, I, P, D, D, P, P, D, D, I, },
/* QU */ { P, I, I, I, I, I, I, I, P, P, I, P, I, I, P, P, I, I, I, },
/* EX */ { P, D, I, I, D, D, I, I, D, P, I, P, D, D, P, P, D, D, D, },
/* ID */ { P, I, I, I, D, D, I, I, D, P, I, P, D, D, P, P, D, D, I, },
/* NU */ { P, I, I, I, D, D, I, I, D, P, I, P, D, I, P, P, I, D, I, },
/* IS */ { P, D, I, I, D, D, I, I, D, P, I, P, D, I, P, P, D, D, D, },
/* SY */ { P, D, I, I, D, D, I, I, D, P, I, P, D, I, P, P, D, D, D, },
/* AL */ { P, I, I, I, D, D, I, I, D, P, I, P, D, I, P, P, I, D, D, },
/* PR */ { P, D, I, I, D, D, I, I, I, P, I, P, I, I, P, P, I, D, D, },
/* PO */ { P, D, I, I, D, D, I, I, D, P, I, P, D, D, P, P, D, D, D, },
/* "" */
/* before */
};
/* Note: The (B2,B2) entry should probably be D instead of P.  */
/* Note: The (PR,ID) entry should probably be D instead of I.  */

void
u8_possible_linebreaks (const unsigned char *s, size_t n, const char *encoding, char *p)
{
  int LBP_AI_REPLACEMENT = (is_cjk_encoding (encoding) ? LBP_ID : LBP_AL);
  const unsigned char *s_end = s + n;
  int last_prop = LBP_BK; /* line break property of last non-space character */
  char *seen_space = NULL; /* Was a space seen after the last non-space character? */
  char *seen_space2 = NULL; /* At least two spaces after the last non-space? */

  /* Don't break inside multibyte characters.  */
  memset (p, UC_BREAK_PROHIBITED, n);

  while (s < s_end)
    {
      unsigned int uc;
      int count = u8_mbtouc_unsafe (&uc, s, s_end - s);
      int prop = lbrkprop_lookup (uc);

      if (prop == LBP_BK)
        {
          /* Mandatory break.  */
          *p = UC_BREAK_MANDATORY;
          last_prop = LBP_BK;
          seen_space = NULL;
          seen_space2 = NULL;
        }
      else
        {
          char *q;

          /* Resolve property values whose behaviour is not fixed.  */
          switch (prop)
            {
              case LBP_AI:
                /* Resolve ambiguous.  */
                prop = LBP_AI_REPLACEMENT;
                break;
              case LBP_CB:
                /* This is arbitrary.  */
                prop = LBP_ID;
                break;
              case LBP_SA:
                /* We don't handle complex scripts yet.
                   Treat LBP_SA like LBP_XX.  */
              case LBP_XX:
                /* This is arbitrary.  */
                prop = LBP_AL;
                break;
            }

          /* Deal with combining characters.  */
          q = p;
          if (prop == LBP_CM)
            {
              /* Don't break just before a combining character.  */
              *p = UC_BREAK_PROHIBITED;
              /* A combining character turns a preceding space into LBP_AL.  */
              if (seen_space != NULL)
                {
                  q = seen_space;
                  seen_space = seen_space2;
                  prop = LBP_AL;
                  goto lookup_via_table;
                }
            }
          else if (prop == LBP_SP)
            {
              /* Don't break just before a space.  */
              *p = UC_BREAK_PROHIBITED;
              seen_space2 = seen_space;
              seen_space = p;
            }
          else
            {
             lookup_via_table:
              /* prop must be usable as an index for table 7.3 of UTR #14.  */
              if (!(prop >= 1 && prop <= sizeof(lbrk_table) / sizeof(lbrk_table[0])))
                abort ();

              if (last_prop == LBP_BK)
                {
                  /* Don't break at the beginning of a line.  */
                  *q = UC_BREAK_PROHIBITED;
                }
              else
                {
                  switch (lbrk_table [last_prop-1] [prop-1])
                    {
                      case D:
                        *q = UC_BREAK_POSSIBLE;
                        break;
                      case I:
                        *q = (seen_space != NULL ? UC_BREAK_POSSIBLE : UC_BREAK_PROHIBITED);
                        break;
                      case P:
                        *q = UC_BREAK_PROHIBITED;
                        break;
                      default:
                        abort ();
                    }
                }
              last_prop = prop;
              seen_space = NULL;
              seen_space2 = NULL;
            }
        }

      s += count;
      p += count;
    }
}

void
u16_possible_linebreaks (const unsigned short *s, size_t n, const char *encoding, char *p)
{
  int LBP_AI_REPLACEMENT = (is_cjk_encoding (encoding) ? LBP_ID : LBP_AL);
  const unsigned short *s_end = s + n;
  int last_prop = LBP_BK; /* line break property of last non-space character */
  char *seen_space = NULL; /* Was a space seen after the last non-space character? */
  char *seen_space2 = NULL; /* At least two spaces after the last non-space? */

  /* Don't break inside multibyte characters.  */
  memset (p, UC_BREAK_PROHIBITED, n);

  while (s < s_end)
    {
      unsigned int uc;
      int count = u16_mbtouc_unsafe (&uc, s, s_end - s);
      int prop = lbrkprop_lookup (uc);

      if (prop == LBP_BK)
        {
          /* Mandatory break.  */
          *p = UC_BREAK_MANDATORY;
          last_prop = LBP_BK;
          seen_space = NULL;
          seen_space2 = NULL;
        }
      else
        {
          char *q;

          /* Resolve property values whose behaviour is not fixed.  */
          switch (prop)
            {
              case LBP_AI:
                /* Resolve ambiguous.  */
                prop = LBP_AI_REPLACEMENT;
                break;
              case LBP_CB:
                /* This is arbitrary.  */
                prop = LBP_ID;
                break;
              case LBP_SA:
                /* We don't handle complex scripts yet.
                   Treat LBP_SA like LBP_XX.  */
              case LBP_XX:
                /* This is arbitrary.  */
                prop = LBP_AL;
                break;
            }

          /* Deal with combining characters.  */
          q = p;
          if (prop == LBP_CM)
            {
              /* Don't break just before a combining character.  */
              *p = UC_BREAK_PROHIBITED;
              /* A combining character turns a preceding space into LBP_AL.  */
              if (seen_space != NULL)
                {
                  q = seen_space;
                  seen_space = seen_space2;
                  prop = LBP_AL;
                  goto lookup_via_table;
                }
            }
          else if (prop == LBP_SP)
            {
              /* Don't break just before a space.  */
              *p = UC_BREAK_PROHIBITED;
              seen_space2 = seen_space;
              seen_space = p;
            }
          else
            {
             lookup_via_table:
              /* prop must be usable as an index for table 7.3 of UTR #14.  */
              if (!(prop >= 1 && prop <= sizeof(lbrk_table) / sizeof(lbrk_table[0])))
                abort ();

              if (last_prop == LBP_BK)
                {
                  /* Don't break at the beginning of a line.  */
                  *q = UC_BREAK_PROHIBITED;
                }
              else
                {
                  switch (lbrk_table [last_prop-1] [prop-1])
                    {
                      case D:
                        *q = UC_BREAK_POSSIBLE;
                        break;
                      case I:
                        *q = (seen_space != NULL ? UC_BREAK_POSSIBLE : UC_BREAK_PROHIBITED);
                        break;
                      case P:
                        *q = UC_BREAK_PROHIBITED;
                        break;
                      default:
                        abort ();
                    }
                }
              last_prop = prop;
              seen_space = NULL;
              seen_space2 = NULL;
            }
        }

      s += count;
      p += count;
    }
}

void
u32_possible_linebreaks (const unsigned int *s, size_t n, const char *encoding, char *p)
{
  int LBP_AI_REPLACEMENT = (is_cjk_encoding (encoding) ? LBP_ID : LBP_AL);
  const unsigned int *s_end = s + n;
  int last_prop = LBP_BK; /* line break property of last non-space character */
  char *seen_space = NULL; /* Was a space seen after the last non-space character? */
  char *seen_space2 = NULL; /* At least two spaces after the last non-space? */

  while (s < s_end)
    {
      unsigned int uc = *s;
      int prop = lbrkprop_lookup (uc);

      if (prop == LBP_BK)
        {
          /* Mandatory break.  */
          *p = UC_BREAK_MANDATORY;
          last_prop = LBP_BK;
          seen_space = NULL;
          seen_space2 = NULL;
        }
      else
        {
          char *q;

          /* Resolve property values whose behaviour is not fixed.  */
          switch (prop)
            {
              case LBP_AI:
                /* Resolve ambiguous.  */
                prop = LBP_AI_REPLACEMENT;
                break;
              case LBP_CB:
                /* This is arbitrary.  */
                prop = LBP_ID;
                break;
              case LBP_SA:
                /* We don't handle complex scripts yet.
                   Treat LBP_SA like LBP_XX.  */
              case LBP_XX:
                /* This is arbitrary.  */
                prop = LBP_AL;
                break;
            }

          /* Deal with combining characters.  */
          q = p;
          if (prop == LBP_CM)
            {
              /* Don't break just before a combining character.  */
              *p = UC_BREAK_PROHIBITED;
              /* A combining character turns a preceding space into LBP_AL.  */
              if (seen_space != NULL)
                {
                  q = seen_space;
                  seen_space = seen_space2;
                  prop = LBP_AL;
                  goto lookup_via_table;
                }
            }
          else if (prop == LBP_SP)
            {
              /* Don't break just before a space.  */
              *p = UC_BREAK_PROHIBITED;
              seen_space2 = seen_space;
              seen_space = p;
            }
          else
            {
             lookup_via_table:
              /* prop must be usable as an index for table 7.3 of UTR #14.  */
              if (!(prop >= 1 && prop <= sizeof(lbrk_table) / sizeof(lbrk_table[0])))
                abort ();

              if (last_prop == LBP_BK)
                {
                  /* Don't break at the beginning of a line.  */
                  *q = UC_BREAK_PROHIBITED;
                }
              else
                {
                  switch (lbrk_table [last_prop-1] [prop-1])
                    {
                      case D:
                        *q = UC_BREAK_POSSIBLE;
                        break;
                      case I:
                        *q = (seen_space != NULL ? UC_BREAK_POSSIBLE : UC_BREAK_PROHIBITED);
                        break;
                      case P:
                        *q = UC_BREAK_PROHIBITED;
                        break;
                      default:
                        abort ();
                    }
                }
              last_prop = prop;
              seen_space = NULL;
              seen_space2 = NULL;
            }
        }

      s++;
      p++;
    }
}


/* Choose the best line breaks, assuming the uc_width function.
   Return the column after the end of the string.  */

int
u8_width_linebreaks (const unsigned char *s, size_t n,
                     int width, int start_column, int at_end_columns,
                     const char *o, const char *encoding,
                     char *p)
{
  const unsigned char *s_end;
  char *last_p;
  int last_column;
  int piece_width;

  u8_possible_linebreaks (s, n, encoding, p);

  s_end = s + n;
  last_p = NULL;
  last_column = start_column;
  piece_width = 0;
  while (s < s_end)
    {
      unsigned int uc;
      int count = u8_mbtouc_unsafe (&uc, s, s_end - s);

      /* Respect the override.  */
      if (o != NULL && *o != UC_BREAK_UNDEFINED)
        *p = *o;

      if (*p == UC_BREAK_POSSIBLE || *p == UC_BREAK_MANDATORY)
        {
          /* An atomic piece of text ends here.  */
          if (last_p != NULL && last_column + piece_width > width)
            {
              /* Insert a line break.  */
              *last_p = UC_BREAK_POSSIBLE;
              last_column = 0;
            }
        }

      if (*p == UC_BREAK_MANDATORY)
        {
          /* uc is a line break character.  */
          /* Start a new piece at column 0.  */
          last_p = NULL;
          last_column = 0;
          piece_width = 0;
        }
      else
        {
          /* uc is not a line break character.  */
          int w;

          if (*p == UC_BREAK_POSSIBLE)
            {
              /* Start a new piece.  */
              last_p = p;
              last_column += piece_width;
              piece_width = 0;
              /* No line break for the moment, may be turned into
                 UC_BREAK_POSSIBLE later, via last_p. */
            }

          *p = UC_BREAK_PROHIBITED;

          w = uc_width (uc, encoding);
          if (w >= 0) /* ignore control characters in the string */
            piece_width += w;
         }

      s += count;
      p += count;
      if (o != NULL)
        o += count;
    }

  /* The last atomic piece of text ends here.  */
  if (last_p != NULL && last_column + piece_width + at_end_columns > width)
    {
      /* Insert a line break.  */
      *last_p = UC_BREAK_POSSIBLE;
      last_column = 0;
    }

  return last_column + piece_width;
}

int
u16_width_linebreaks (const unsigned short *s, size_t n,
                      int width, int start_column, int at_end_columns,
                      const char *o, const char *encoding,
                      char *p)
{
  const unsigned short *s_end;
  char *last_p;
  int last_column;
  int piece_width;

  u16_possible_linebreaks (s, n, encoding, p);

  s_end = s + n;
  last_p = NULL;
  last_column = start_column;
  piece_width = 0;
  while (s < s_end)
    {
      unsigned int uc;
      int count = u16_mbtouc_unsafe (&uc, s, s_end - s);

      /* Respect the override.  */
      if (o != NULL && *o != UC_BREAK_UNDEFINED)
        *p = *o;

      if (*p == UC_BREAK_POSSIBLE || *p == UC_BREAK_MANDATORY)
        {
          /* An atomic piece of text ends here.  */
          if (last_p != NULL && last_column + piece_width > width)
            {
              /* Insert a line break.  */
              *last_p = UC_BREAK_POSSIBLE;
              last_column = 0;
            }
        }

      if (*p == UC_BREAK_MANDATORY)
        {
          /* uc is a line break character.  */
          /* Start a new piece at column 0.  */
          last_p = NULL;
          last_column = 0;
          piece_width = 0;
        }
      else
        {
          /* uc is not a line break character.  */
          int w;

          if (*p == UC_BREAK_POSSIBLE)
            {
              /* Start a new piece.  */
              last_p = p;
              last_column += piece_width;
              piece_width = 0;
              /* No line break for the moment, may be turned into
                 UC_BREAK_POSSIBLE later, via last_p. */
            }

          *p = UC_BREAK_PROHIBITED;

          w = uc_width (uc, encoding);
          if (w >= 0) /* ignore control characters in the string */
            piece_width += w;
         }

      s += count;
      p += count;
      if (o != NULL)
        o += count;
    }

  /* The last atomic piece of text ends here.  */
  if (last_p != NULL && last_column + piece_width + at_end_columns > width)
    {
      /* Insert a line break.  */
      *last_p = UC_BREAK_POSSIBLE;
      last_column = 0;
    }

  return last_column + piece_width;
}

int
u32_width_linebreaks (const unsigned int *s, size_t n,
                      int width, int start_column, int at_end_columns,
                      const char *o, const char *encoding,
                      char *p)
{
  const unsigned int *s_end;
  char *last_p;
  int last_column;
  int piece_width;

  u32_possible_linebreaks (s, n, encoding, p);

  s_end = s + n;
  last_p = NULL;
  last_column = start_column;
  piece_width = 0;
  while (s < s_end)
    {
      unsigned int uc = *s;

      /* Respect the override.  */
      if (o != NULL && *o != UC_BREAK_UNDEFINED)
        *p = *o;

      if (*p == UC_BREAK_POSSIBLE || *p == UC_BREAK_MANDATORY)
        {
          /* An atomic piece of text ends here.  */
          if (last_p != NULL && last_column + piece_width > width)
            {
              /* Insert a line break.  */
              *last_p = UC_BREAK_POSSIBLE;
              last_column = 0;
            }
        }

      if (*p == UC_BREAK_MANDATORY)
        {
          /* uc is a line break character.  */
          /* Start a new piece at column 0.  */
          last_p = NULL;
          last_column = 0;
          piece_width = 0;
        }
      else
        {
          /* uc is not a line break character.  */
          int w;

          if (*p == UC_BREAK_POSSIBLE)
            {
              /* Start a new piece.  */
              last_p = p;
              last_column += piece_width;
              piece_width = 0;
              /* No line break for the moment, may be turned into
                 UC_BREAK_POSSIBLE later, via last_p. */
            }

          *p = UC_BREAK_PROHIBITED;

          w = uc_width (uc, encoding);
          if (w >= 0) /* ignore control characters in the string */
            piece_width += w;
         }

      s++;
      p++;
      if (o != NULL)
        o++;
    }

  /* The last atomic piece of text ends here.  */
  if (last_p != NULL && last_column + piece_width + at_end_columns > width)
    {
      /* Insert a line break.  */
      *last_p = UC_BREAK_POSSIBLE;
      last_column = 0;
    }

  return last_column + piece_width;
}


#ifdef TEST1

#include <stdio.h>

/* Read the contents of an input stream, and return it, terminated with a NUL
   byte. */
char *
read_file (FILE *stream)
{
#define BUFSIZE 4096
  char *buf = NULL;
  int alloc = 0;
  int size = 0;
  int count;

  while (! feof (stream))
    {
      if (size + BUFSIZE > alloc)
        {
          alloc = alloc + alloc / 2;
          if (alloc < size + BUFSIZE)
            alloc = size + BUFSIZE;
          buf = realloc (buf, alloc);
          if (buf == NULL)
            {
              fprintf (stderr, "out of memory\n");
              exit (1);
            }
        }
      count = fread (buf + size, 1, BUFSIZE, stream);
      if (count == 0)
        {
          if (ferror (stream))
            {
              perror ("fread");
              exit (1);
            }
        }
      else
        size += count;
    }
  buf = realloc (buf, size + 1);
  if (buf == NULL)
    {
      fprintf (stderr, "out of memory\n");
      exit (1);
    }
  buf[size] = '\0';
  return buf;
#undef BUFSIZE
}

int
main (int argc, char * argv[])
{
  if (argc == 1)
    {
      /* Display all the break opportunities in the input string.  */
      char *input = read_file (stdin);
      int length = strlen (input);
      char *breaks = malloc (length);
      int i;

      u8_possible_linebreaks ((unsigned char *) input, length, "UTF-8", breaks);

      for (i = 0; i < length; i++)
        {
          switch (breaks[i])
            {
              case UC_BREAK_POSSIBLE:
                /* U+2027 in UTF-8 encoding */
                putc (0xe2, stdout); putc (0x80, stdout); putc (0xa7, stdout);
                break;
              case UC_BREAK_MANDATORY:
                /* U+21B2 (or U+21B5) in UTF-8 encoding */
                putc (0xe2, stdout); putc (0x86, stdout); putc (0xb2, stdout);
                break;
              case UC_BREAK_PROHIBITED:
                break;
              default:
                abort ();
            }
          putc (input[i], stdout);
        }

      free (breaks);

      return 0;
    }
  else if (argc == 2)
    {
      /* Insert line breaks for a given width.  */
      int width = atoi (argv[1]);
      char *input = read_file (stdin);
      int length = strlen (input);
      char *breaks = malloc (length);
      int i;

      u8_width_linebreaks ((unsigned char *) input, length, width, 0, 0, NULL, "UTF-8", breaks);

      for (i = 0; i < length; i++)
        {
          switch (breaks[i])
            {
              case UC_BREAK_POSSIBLE:
                putc ('\n', stdout);
                break;
              case UC_BREAK_MANDATORY:
                break;
              case UC_BREAK_PROHIBITED:
                break;
              default:
                abort ();
            }
          putc (input[i], stdout);
        }

      free (breaks);

      return 0;
    }
  else
    return 1;
}

#endif /* TEST1 */


/* Now the same thing with an arbitrary encoding.

   We convert the input string to Unicode.

   The standardized Unicode encodings are UTF-8, UCS-2, UCS-4, UTF-16,
   UTF-16BE, UTF-16LE, UTF-7.  UCS-2 supports only characters up to
   \U0000FFFF.  UTF-16 and variants support only characters up to
   \U0010FFFF.  UTF-7 is way too complex and not supported by glibc-2.1.
   UCS-4 specification leaves doubts about endianness and byte order mark.
   glibc currently interprets it as big endian without byte order mark,
   but this is not backed by an RFC.  So we use UTF-8. It supports
   characters up to \U7FFFFFFF and is unambiguously defined.  */

#if HAVE_ICONV

#include <iconv.h>
#include <errno.h>

/* Luckily, the encoding's name is platform independent.  */
#define UTF8_NAME "UTF-8"

/* Return the length of a string after conversion through an iconv_t.  */
static size_t
iconv_string_length (iconv_t cd, const char *s, size_t n)
{
#define TMPBUFSIZE 4096
  size_t count = 0;
  char tmpbuf[TMPBUFSIZE];
  const char *inptr = s;
  size_t insize = n;
  while (insize > 0)
    {
      char *outptr = tmpbuf;
      size_t outsize = TMPBUFSIZE;
      size_t res = iconv (cd, (ICONV_CONST char **) &inptr, &insize, &outptr, &outsize);
      if (res == (size_t)(-1) && errno != E2BIG)
        return (size_t)(-1);
      count += outptr - tmpbuf;
    }
  /* Avoid glibc-2.1 bug and Solaris 7 through 9 bug.  */
#if defined _LIBICONV_VERSION \
    || !((__GLIBC__ - 0 == 2 && __GLIBC_MINOR__ - 0 <= 1) || defined __sun)
  {
    char *outptr = tmpbuf;
    size_t outsize = TMPBUFSIZE;
    size_t res = iconv (cd, NULL, NULL, &outptr, &outsize);
    if (res == (size_t)(-1))
      return (size_t)(-1);
    count += outptr - tmpbuf;
  }
  /* Return to the initial state.  */
  iconv (cd, NULL, NULL, NULL, NULL);
#endif
  return count;
#undef TMPBUFSIZE
}

static void
iconv_string_keeping_offsets (iconv_t cd, const char *s, size_t n,
                              size_t *offtable, char *t, size_t m)
{
  size_t i;
  const char *s_end;
  const char *inptr;
  char *outptr;
  size_t outsize;
  /* Avoid glibc-2.1 bug.  */
#if !defined _LIBICONV_VERSION && (__GLIBC__ - 0 == 2 && __GLIBC_MINOR__ - 0 <= 1)
  const size_t extra = 1;
#else
  const size_t extra = 0;
#endif

  for (i = 0; i < n; i++)
    offtable[i] = (size_t)(-1);

  s_end = s + n;
  inptr = s;
  outptr = t;
  outsize = m + extra;
  while (inptr < s_end)
    {
      const char *saved_inptr;
      size_t insize;
      size_t res;

      offtable[inptr - s] = outptr - t;

      saved_inptr = inptr;
      res = (size_t)(-1);
      for (insize = 1; inptr + insize <= s_end; insize++)
        {
          res = iconv (cd, (ICONV_CONST char **) &inptr, &insize, &outptr, &outsize);
          if (!(res == (size_t)(-1) && errno == EINVAL))
            break;
          /* We expect that no input bytes have been consumed so far.  */
          if (inptr != saved_inptr)
            abort ();
        }
      /* After we verified the convertibility and computed the translation's
         size m, there shouldn't be any conversion error here. */
      if (res == (size_t)(-1))
        abort ();
    }
  /* Avoid glibc-2.1 bug and Solaris 7 bug.  */
#if defined _LIBICONV_VERSION \
    || !((__GLIBC__ - 0 == 2 && __GLIBC_MINOR__ - 0 <= 1) || defined __sun)
  if (iconv (cd, NULL, NULL, &outptr, &outsize) == (size_t)(-1))
    abort ();
#endif
  /* We should have produced exactly m output bytes.  */
  if (outsize != extra)
    abort ();
}

#endif /* HAVE_ICONV */

#if C_CTYPE_ASCII

/* Tests whether a string is entirely ASCII.  Returns 1 if yes.
   Returns 0 if the string is in an 8-bit encoding or an ISO-2022 encoding.  */
static int
is_all_ascii (const char *s, size_t n)
{
  for (; n > 0; s++, n--)
    {
      unsigned char c = (unsigned char) *s;

      if (!(c_isprint (c) || c_isspace (c)))
	return 0;
    }
  return 1;
}

#endif /* C_CTYPE_ASCII */

void
mbs_possible_linebreaks (const char *s, size_t n, const char *encoding,
                         char *p)
{
  if (n == 0)
    return;
  if (is_utf8_encoding (encoding))
    u8_possible_linebreaks ((const unsigned char *) s, n, encoding, p);
  else
    {
#if HAVE_ICONV
      iconv_t to_utf8;
      /* Avoid glibc-2.1 bug with EUC-KR.  */
# if (__GLIBC__ - 0 == 2 && __GLIBC_MINOR__ - 0 <= 1) && !defined _LIBICONV_VERSION
      if (STREQ (encoding, "EUC-KR", 'E', 'U', 'C', '-', 'K', 'R', 0, 0, 0))
	to_utf8 = (iconv_t)(-1);
      else
# endif
      /* Avoid Solaris 9 bug with GB2312, EUC-TW, BIG5, BIG5-HKSCS, GBK,
         GB18030.  */
# if defined __sun && !defined _LIBICONV_VERSION
      if (   STREQ (encoding, "GB2312", 'G', 'B', '2', '3', '1', '2', 0, 0, 0)
          || STREQ (encoding, "EUC-TW", 'E', 'U', 'C', '-', 'T', 'W', 0, 0, 0)
          || STREQ (encoding, "BIG5", 'B', 'I', 'G', '5', 0, 0, 0, 0, 0)
          || STREQ (encoding, "BIG5-HKSCS", 'B', 'I', 'G', '5', '-', 'H', 'K', 'S', 'C')
          || STREQ (encoding, "GBK", 'G', 'B', 'K', 0, 0, 0, 0, 0, 0)
          || STREQ (encoding, "GB18030", 'G', 'B', '1', '8', '0', '3', '0', 0, 0))
        to_utf8 = (iconv_t)(-1);
      else
# endif
      to_utf8 = iconv_open (UTF8_NAME, encoding);
      if (to_utf8 != (iconv_t)(-1))
        {
          /* Determine the length of the resulting UTF-8 string.  */
          size_t m = iconv_string_length (to_utf8, s, n);
          if (m != (size_t)(-1))
            {
              /* Convert the string to UTF-8 and build a translation table
                 from offsets into s to offsets into the translated string.  */
	      size_t memory_size = xsum3 (xtimes (n, sizeof (size_t)), m, m);
              char *memory =
		(size_in_bounds_p (memory_size) ? malloc (memory_size) : NULL);
              if (memory != NULL)
                {
                  size_t *offtable = (size_t *) memory;
                  char *t = (char *) (offtable + n);
                  char *q = (char *) (t + m);
                  size_t i;

                  iconv_string_keeping_offsets (to_utf8, s, n, offtable, t, m);

                  /* Determine the possible line breaks of the UTF-8 string.  */
                  u8_possible_linebreaks ((const unsigned char *) t, m, encoding, q);

                  /* Translate the result back to the original string.  */
                  memset (p, UC_BREAK_PROHIBITED, n);
                  for (i = 0; i < n; i++)
                    if (offtable[i] != (size_t)(-1))
                      p[i] = q[offtable[i]];

                  free (memory);
                  iconv_close (to_utf8);
                  return;
                }
            }
          iconv_close (to_utf8);
        }
#endif
      /* Impossible to convert.  */
#if C_CTYPE_ASCII
      if (is_all_ascii (s, n))
	{
	  /* ASCII is a subset of UTF-8.  */
	  u8_possible_linebreaks ((const unsigned char *) s, n, encoding, p);
	  return;
	}
#endif
      /* We have a non-ASCII string and cannot convert it.
	 Don't produce line breaks except those already present in the
	 input string.  All we assume here is that the encoding is
	 minimally ASCII compatible.  */
      {
        const char *s_end = s + n;
        while (s < s_end)
          {
            *p = (*s == '\n' ? UC_BREAK_MANDATORY : UC_BREAK_PROHIBITED);
            s++;
            p++;
          }
      }
    }
}

int
mbs_width_linebreaks (const char *s, size_t n,
                      int width, int start_column, int at_end_columns,
                      const char *o, const char *encoding,
                      char *p)
{
  if (n == 0)
    return start_column;
  if (is_utf8_encoding (encoding))
    return u8_width_linebreaks ((const unsigned char *) s, n, width, start_column, at_end_columns, o, encoding, p);
  else
    {
#if HAVE_ICONV
      iconv_t to_utf8;
      /* Avoid glibc-2.1 bug with EUC-KR.  */
# if (__GLIBC__ - 0 == 2 && __GLIBC_MINOR__ - 0 <= 1) && !defined _LIBICONV_VERSION
      if (STREQ (encoding, "EUC-KR", 'E', 'U', 'C', '-', 'K', 'R', 0, 0, 0))
	to_utf8 = (iconv_t)(-1);
      else
# endif
      /* Avoid Solaris 9 bug with GB2312, EUC-TW, BIG5, BIG5-HKSCS, GBK,
         GB18030.  */
# if defined __sun && !defined _LIBICONV_VERSION
      if (   STREQ (encoding, "GB2312", 'G', 'B', '2', '3', '1', '2', 0, 0, 0)
          || STREQ (encoding, "EUC-TW", 'E', 'U', 'C', '-', 'T', 'W', 0, 0, 0)
          || STREQ (encoding, "BIG5", 'B', 'I', 'G', '5', 0, 0, 0, 0, 0)
          || STREQ (encoding, "BIG5-HKSCS", 'B', 'I', 'G', '5', '-', 'H', 'K', 'S', 'C')
          || STREQ (encoding, "GBK", 'G', 'B', 'K', 0, 0, 0, 0, 0, 0)
          || STREQ (encoding, "GB18030", 'G', 'B', '1', '8', '0', '3', '0', 0, 0))
        to_utf8 = (iconv_t)(-1);
      else
# endif
      to_utf8 = iconv_open (UTF8_NAME, encoding);
      if (to_utf8 != (iconv_t)(-1))
        {
          /* Determine the length of the resulting UTF-8 string.  */
          size_t m = iconv_string_length (to_utf8, s, n);
          if (m != (size_t)(-1))
            {
              /* Convert the string to UTF-8 and build a translation table
                 from offsets into s to offsets into the translated string.  */
	      size_t memory_size =
		xsum4 (xtimes (n, sizeof (size_t)), m, m,
		       (o != NULL ? m : 0));
	      char *memory =
		(char *)
		(size_in_bounds_p (memory_size) ? malloc (memory_size) : NULL);
              if (memory != NULL)
                {
                  size_t *offtable = (size_t *) memory;
                  char *t = (char *) (offtable + n);
                  char *q = (char *) (t + m);
                  char *o8 = (o != NULL ? (char *) (q + m) : NULL);
                  int res_column;
                  size_t i;

                  iconv_string_keeping_offsets (to_utf8, s, n, offtable, t, m);

                  /* Translate the overrides to the UTF-8 string.  */
                  if (o != NULL)
                    {
                      memset (o8, UC_BREAK_UNDEFINED, m);
                      for (i = 0; i < n; i++)
                        if (offtable[i] != (size_t)(-1))
                          o8[offtable[i]] = o[i];
                    }

                  /* Determine the line breaks of the UTF-8 string.  */
                  res_column =
                    u8_width_linebreaks ((const unsigned char *) t, m, width, start_column, at_end_columns, o8, encoding, q);

                  /* Translate the result back to the original string.  */
                  memset (p, UC_BREAK_PROHIBITED, n);
                  for (i = 0; i < n; i++)
                    if (offtable[i] != (size_t)(-1))
                      p[i] = q[offtable[i]];

                  free (memory);
                  iconv_close (to_utf8);
                  return res_column;
                }
            }
          iconv_close (to_utf8);
        }
#endif
      /* Impossible to convert.  */
#if C_CTYPE_ASCII
      if (is_all_ascii (s, n))
	{
	  /* ASCII is a subset of UTF-8.  */
	  return u8_width_linebreaks ((const unsigned char *) s, n, width, start_column, at_end_columns, o, encoding, p);
	}
#endif
      /* We have a non-ASCII string and cannot convert it.
	 Don't produce line breaks except those already present in the
	 input string.  All we assume here is that the encoding is
	 minimally ASCII compatible.  */
      {
        const char *s_end = s + n;
        while (s < s_end)
          {
            *p = ((o != NULL && *o == UC_BREAK_MANDATORY) || *s == '\n'
                  ? UC_BREAK_MANDATORY
                  : UC_BREAK_PROHIBITED);
            s++;
            p++;
            if (o != NULL)
              o++;
          }
        /* We cannot compute widths in this case.  */
        return start_column;
      }
    }
}


#ifdef TEST2

#include <stdio.h>
#include <locale.h>

/* Read the contents of an input stream, and return it, terminated with a NUL
   byte. */
char *
read_file (FILE *stream)
{
#define BUFSIZE 4096
  char *buf = NULL;
  int alloc = 0;
  int size = 0;
  int count;

  while (! feof (stream))
    {
      if (size + BUFSIZE > alloc)
        {
          alloc = alloc + alloc / 2;
          if (alloc < size + BUFSIZE)
            alloc = size + BUFSIZE;
          buf = realloc (buf, alloc);
          if (buf == NULL)
            {
              fprintf (stderr, "out of memory\n");
              exit (1);
            }
        }
      count = fread (buf + size, 1, BUFSIZE, stream);
      if (count == 0)
        {
          if (ferror (stream))
            {
              perror ("fread");
              exit (1);
            }
        }
      else
        size += count;
    }
  buf = realloc (buf, size + 1);
  if (buf == NULL)
    {
      fprintf (stderr, "out of memory\n");
      exit (1);
    }
  buf[size] = '\0';
  return buf;
#undef BUFSIZE
}

int
main (int argc, char * argv[])
{
  setlocale (LC_CTYPE, "");
  if (argc == 1)
    {
      /* Display all the break opportunities in the input string.  */
      char *input = read_file (stdin);
      int length = strlen (input);
      char *breaks = malloc (length);
      int i;

      mbs_possible_linebreaks (input, length, locale_charset (), breaks);

      for (i = 0; i < length; i++)
        {
          switch (breaks[i])
            {
              case UC_BREAK_POSSIBLE:
                putc ('|', stdout);
                break;
              case UC_BREAK_MANDATORY:
                break;
              case UC_BREAK_PROHIBITED:
                break;
              default:
                abort ();
            }
          putc (input[i], stdout);
        }

      free (breaks);

      return 0;
    }
  else if (argc == 2)
    {
      /* Insert line breaks for a given width.  */
      int width = atoi (argv[1]);
      char *input = read_file (stdin);
      int length = strlen (input);
      char *breaks = malloc (length);
      int i;

      mbs_width_linebreaks (input, length, width, 0, 0, NULL, locale_charset (), breaks);

      for (i = 0; i < length; i++)
        {
          switch (breaks[i])
            {
              case UC_BREAK_POSSIBLE:
                putc ('\n', stdout);
                break;
              case UC_BREAK_MANDATORY:
                break;
              case UC_BREAK_PROHIBITED:
                break;
              default:
                abort ();
            }
          putc (input[i], stdout);
        }

      free (breaks);

      return 0;
    }
  else
    return 1;
}

#endif /* TEST2 */

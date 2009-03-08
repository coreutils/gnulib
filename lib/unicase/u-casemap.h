/* Case mapping for UTF-8/UTF-16/UTF-32 strings (locale dependent).
   Copyright (C) 2009 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Quoting the Unicode standard:
     Definition: A character is defined to be "cased" if it has the Lowercase or
     Uppercase property or has a General_Category value of Titlecase_Letter.  */
static inline bool
is_cased (ucs4_t uc)
{
  return (uc_is_property_lowercase (uc)
	  || uc_is_property_uppercase (uc)
	  || uc_is_general_category (uc, UC_TITLECASE_LETTER));
}

/* Quoting the Unicode standard:
     Definition: A character is defined to be "case-ignorable" if it has the
     value MidLetter {or the value MidNumLet} for the Word_Break property or
     its General_Category is one of Nonspacing_Mark (Mn), Enclosing_Mark (Me),
     Format (Cf), Modifier_Letter (Lm), or Modifier_Symbol (Sk).
   The text marked in braces was added in Unicode 5.1.0, see
   <http://www.unicode.org/versions/Unicode5.1.0/> section "Update of
   Definition of case-ignorable".   */
static inline bool
is_case_ignorable (ucs4_t uc)
{
  int wbp = uc_wordbreak_property (uc);

  return (wbp == WBP_MIDLETTER || wbp == WBP_MIDNUMLET
	  || uc_is_general_category_withtable (uc, UC_CATEGORY_MASK_Mn
						   | UC_CATEGORY_MASK_Me
						   | UC_CATEGORY_MASK_Cf
						   | UC_CATEGORY_MASK_Lm
						   | UC_CATEGORY_MASK_Sk));
}

UNIT *
FUNC (const UNIT *s, size_t n, const char *iso639_language,
      ucs4_t (*single_character_map) (ucs4_t),
      size_t offset_in_rule, /* offset in 'struct special_casing_rule' */
      uninorm_t nf,
      UNIT *resultbuf, size_t *lengthp)
{
  /* The result being accumulated.  */
  UNIT *result;
  size_t length;
  size_t allocated;

  /* Initialize the accumulator.  */
  if (nf != NULL || resultbuf == NULL)
    {
      result = NULL;
      allocated = 0;
    }
  else
    {
      result = resultbuf;
      allocated = *lengthp;
    }
  length = 0;

  {
    const UNIT *s_end = s + n;

    /* Helper for evaluating the FINAL_SIGMA condition:
       Last character that was not case-ignorable.  */
    ucs4_t last_char_except_ignorable = 0xFFFD;

    /* Helper for evaluating the AFTER_SOFT_DOTTED and AFTER_I conditions:
       Last character that was of combining class 230 ("Above") or 0.  */
    ucs4_t last_char_normal_or_above = 0xFFFD;

    while (s < s_end)
      {
	ucs4_t uc;
	int count = U_MBTOUC_UNSAFE (&uc, s, s_end - s);

	ucs4_t mapped_uc[3];
	unsigned int mapped_count;

	if (uc < 0x10000)
	  {
	    /* Look first in the special-casing table.  */
	    char code[3];

	    code[0] = (uc >> 8) & 0xff;
	    code[1] = uc & 0xff;

	    for (code[2] = 0; ; code[2]++)
	      {
		const struct special_casing_rule *rule =
		  gl_unicase_special_lookup (code, 3);

		if (rule == NULL)
		  break;

		/* Test if the condition applies.  */
		/* Does the language apply?  */
		if (rule->language[0] == '\0'
		    || (iso639_language != NULL
			&& iso639_language[0] == rule->language[0]
			&& iso639_language[1] == rule->language[1]))
		  {
		    /* Does the context apply?  */
		    int context = rule->context;
		    bool applies;

		    if (context < 0)
		      context = - context;
		    switch (context)
		      {
		      case SCC_ALWAYS:
			applies = true;
			break;

		      case SCC_FINAL_SIGMA:
			/* "Before" condition: preceded by a sequence
			   consisting of a cased letter and a case-ignorable
			   sequence.
			   "After" condition: not followed by a sequence
			   consisting of a case-ignorable sequence and then a
			   cased letter.  */
			/* Test the "before" condition.  */
			applies = is_cased (last_char_except_ignorable);
			/* Test the "after" condition.  */
			if (applies)
			  {
			    const UNIT *s2 = s + count;
			    while (s2 < s_end)
			      {
				ucs4_t uc2;
				int count2 = U_MBTOUC_UNSAFE (&uc2, s2, s_end - s2);
				if (is_cased (uc2))
				  {
				    applies = false;
				    break;
				  }
				if (!is_case_ignorable (uc2))
				  break;
				s2 += count2;
			      }
			  }
			break;

		      case SCC_AFTER_SOFT_DOTTED:
			/* "Before" condition: There is a Soft_Dotted character
			   before it, with no intervening character of
			   combining class 0 or 230 (Above).  */
			/* Test the "before" condition.  */
			applies = uc_is_property_soft_dotted (last_char_normal_or_above);
			break;

		      case SCC_MORE_ABOVE:
			/* "After" condition: followed by a character of
			   combining class 230 (Above) with no intervening
			   character of combining class 0 or 230 (Above).  */
			/* Test the "after" condition.  */
			{
			  const UNIT *s2 = s + count;
			  applies = false;
			  while (s2 < s_end)
			    {
			      ucs4_t uc2;
			      int count2 = U_MBTOUC_UNSAFE (&uc2, s2, s_end - s2);
			      int ccc = uc_combining_class (uc2);
			      if (ccc == UC_CCC_A)
				{
				  applies = true;
				  break;
				}
			      if (ccc == UC_CCC_NR)
				break;
			      s2 += count2;
			    }
			}
			break;

		      case SCC_BEFORE_DOT:
			/* "After" condition: followed by COMBINING DOT ABOVE
			   (U+0307). Any sequence of characters with a
			   combining class that is neither 0 nor 230 may
			   intervene between the current character and the
			   combining dot above.  */
			/* Test the "after" condition.  */
			{
			  const UNIT *s2 = s + count;
			  applies = false;
			  while (s2 < s_end)
			    {
			      ucs4_t uc2;
			      int count2 = U_MBTOUC_UNSAFE (&uc2, s2, s_end - s2);
			      if (uc2 == 0x0307) /* COMBINING DOT ABOVE */
				{
				  applies = true;
				  break;
				}
			      {
				int ccc = uc_combining_class (uc2);
				if (ccc == UC_CCC_A || ccc == UC_CCC_NR)
				  break;
			      }
			      s2 += count2;
			    }
			}
			break;

		      case SCC_AFTER_I:
			/* "Before" condition: There is an uppercase I before
			   it, and there is no intervening character of
			   combining class 0 or 230 (Above).  */
			/* Test the "before" condition.  */
			applies = (last_char_normal_or_above == 'I');
			break;

		      default:
			abort ();
		      }
		    if (rule->context < 0)
		      applies = !applies;

		    if (applies)
		      {
			/* The rule applies.
			   Look up the mapping (0 to 3 characters).  */
			const unsigned short *mapped_in_rule =
			  (const unsigned short *)((const char *)rule + offset_in_rule);

			if (mapped_in_rule[0] == 0)
			  mapped_count = 0;
			else
			  {
			    mapped_uc[0] = mapped_in_rule[0];
			    if (mapped_in_rule[1] == 0)
			      mapped_count = 1;
			    else
			      {
				mapped_uc[1] = mapped_in_rule[1];
				if (mapped_in_rule[2] == 0)
				  mapped_count = 2;
				else
				  {
				    mapped_uc[2] = mapped_in_rule[2];
				    mapped_count = 3;
				  }
			      }
			  }
			goto found_mapping;
		      }
		  }

		/* Optimization: Save a hash table lookup in the next round.  */
		if (!rule->has_next)
		  break;
	      }
	  }

	/* No special-cased mapping.  So use the locale and context independent
	   mapping.  */
	mapped_uc[0] = single_character_map (uc);
	mapped_count = 1;

       found_mapping:
	/* Found the mapping: uc maps to mapped_uc[0..mapped_count-1].  */
	{
	  unsigned int i;

	  for (i = 0; i < mapped_count; i++)
	    {
	      ucs4_t muc = mapped_uc[i];

	      /* Append muc to the result accumulator.  */
	      if (length < allocated)
		{
		  int ret = U_UCTOMB (result + length, muc, allocated - length);
		  if (ret == -1)
		    {
		      errno = EINVAL;
		      goto fail;
		    }
		  if (ret >= 0)
		    {
		      length += ret;
		      goto done_appending;
		    }
		}
	      {
		size_t old_allocated = allocated;
		size_t new_allocated = 2 * old_allocated;
		if (new_allocated < 64)
		  new_allocated = 64;
		if (new_allocated < old_allocated) /* integer overflow? */
		  abort ();
		{
		  UNIT *larger_result;
		  if (result == NULL)
		    {
		      larger_result = (UNIT *) malloc (new_allocated * sizeof (UNIT));
		      if (larger_result == NULL)
			{
			  errno = ENOMEM;
			  goto fail;
			}
		    }
		  else if (result == resultbuf)
		    {
		      larger_result = (UNIT *) malloc (new_allocated * sizeof (UNIT));
		      if (larger_result == NULL)
			{
			  errno = ENOMEM;
			  goto fail;
			}
		      U_CPY (larger_result, resultbuf, length);
		    }
		  else
		    {
		      larger_result =
			(UNIT *) realloc (result, new_allocated * sizeof (UNIT));
		      if (larger_result == NULL)
			{
			  errno = ENOMEM;
			  goto fail;
			}
		    }
		  result = larger_result;
		  allocated = new_allocated;
		  {
		    int ret = U_UCTOMB (result + length, muc, allocated - length);
		    if (ret == -1)
		      {
			errno = EINVAL;
			goto fail;
		      }
		    if (ret < 0)
		      abort ();
		    length += ret;
		    goto done_appending;
		  }
		}
	      }
	     done_appending: ;
	    }
	}

	if (!is_case_ignorable (uc))
	  last_char_except_ignorable = uc;

	{
	  int ccc = uc_combining_class (uc);
	  if (ccc == UC_CCC_A || ccc == UC_CCC_NR)
	    last_char_normal_or_above = uc;
	}

	s += count;
      }
  }

  if (nf != NULL)
    {
      /* Finally, normalize the result.  */
      UNIT *normalized_result;

      normalized_result = U_NORMALIZE (nf, result, length, resultbuf, lengthp);
      if (normalized_result == NULL)
	goto fail;

      free (result);
      return normalized_result;
    }

  if (length == 0)
    {
      if (result == NULL)
	{
	  /* Return a non-NULL value.  NULL means error.  */
	  result = (UNIT *) malloc (1);
	  if (result == NULL)
	    {
	      errno = ENOMEM;
	      goto fail;
	    }
	}
    }
  else if (result != resultbuf && length < allocated)
    {
      /* Shrink the allocated memory if possible.  */
      UNIT *memory;

      memory = (UNIT *) realloc (result, length * sizeof (UNIT));
      if (memory != NULL)
	result = memory;
    }

  *lengthp = length;
  return result;

 fail:
  if (result != resultbuf)
    {
      int saved_errno = errno;
      free (result);
      errno = saved_errno;
    }
  return NULL;
}

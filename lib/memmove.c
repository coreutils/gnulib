/* memmove.c -- copy memory.
   This file is in the public domain.  */

/* Written by David MacKenzie <djm@gnu.ai.mit.edu>.  */

#include <config.h>

#include <stddef.h>

/* Copy LENGTH bytes from SOURCE to DEST.  Does not null-terminate.  */

void *
memmove (void *dest0, void const *source0, size_t length)
{
  if (length > 0)
    {
      char *dest = dest0;
      char const *source = source0;
      if (source < dest)
        {
          /* Moving from low mem to hi mem; start at end.  */
          source += length;
          dest += length;
          do
            *--dest = *--source;
          while (--length > 0);
        }
      else if (source != dest)
        {
          /* Moving from hi mem to low mem; start at beginning.  */
          do
            *dest++ = *source++;
          while (--length > 0);
        }
    }
  return dest0;
}

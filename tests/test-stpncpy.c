/* Test the system defined function stpncpy().  */

#include <string.h>
#include <stdio.h>

int
main ()
{
  char from[10];
  char to[10];
  int i, j, k, h;
  char *ret;

  for (i = 0; i < 10; i++)
    for (j = 0; j < 10; j++)
      for (k = 0; k < 10; k++)
        {
          memset (from, 'X', sizeof from);
          memcpy (from, "SourceString", i);
          if (i < 10) from[i] = '\0';
          memset (to, 'Y', sizeof to);
          memcpy (to, "Destination", k);
          if (k < 10) to[k] = '\0';
          ret = stpncpy (to, from, j);
          printf ("i = %2d, j = %2d, k = %2d:  from = ", i, j, k);
          for (h = 0; h < 10; h++)
            if (from[h] >= 0x20 && from[h] < 0x7f)
              putchar (from[h]);
            else
              printf ("\\x%02x", from[h]);
          printf ("  to = ");
          for (h = 0; h < 10; h++)
            if (to[h] >= 0x20 && to[h] < 0x7f)
              putchar (to[h]);
            else
              printf ("\\x%02x", to[h]);
          printf ("  ret = to + %d\n", ret - to);
        }

  return 0;
}

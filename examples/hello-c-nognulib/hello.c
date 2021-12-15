/* This file is in the public domain.

   Source code of the C program.  */


/* Get printf() declaration.  */
#include <stdio.h>

/* Get getpid() declaration.  */
#if HAVE_UNISTD_H
# include <unistd.h>
#endif

int
main ()
{
  printf ("%s\n", "Hello, world!");
  printf ("This program is running as process number %d.", getpid ());
  putchar ('\n');

  return 0;
}

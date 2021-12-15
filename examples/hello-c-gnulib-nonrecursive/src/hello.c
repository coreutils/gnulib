/* This file is in the public domain.

   Source code of the C program.  */


/* Every source file that uses Gnulib needs this.  */
#include <config.h>

/* Get printf() declaration.  */
#include <stdio.h>

/* Get getpid() declaration.  */
#include <unistd.h>

/* Include Gnulib header files.  */
#include "get_ppid_of.h"

int
main ()
{
  printf ("%s\n", "Hello, world!");
  printf ("This program is running as process number %d.\n", getpid ());
  printf ("The parent process number is %d.\n", get_ppid_of (getpid ()));

  return 0;
}

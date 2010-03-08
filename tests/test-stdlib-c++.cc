/* Test of <stdlib.h> substitute in C++ mode.
   Copyright (C) 2010 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2010.  */

#define GNULIB_NAMESPACE gnulib
#include <config.h>

#include <stdlib.h>

#include "signature.h"


//SIGNATURE_CHECK (GNULIB_NAMESPACE::atexit, int, (void (*) (void)));

#if GNULIB_ATOLL
SIGNATURE_CHECK (GNULIB_NAMESPACE::atoll, long long, (const char *));
#endif

#if GNULIB_CALLOC_POSIX
SIGNATURE_CHECK (GNULIB_NAMESPACE::calloc, void *, (size_t, size_t));
#endif

#if GNULIB_CANONICALIZE_FILE_NAME
SIGNATURE_CHECK (GNULIB_NAMESPACE::canonicalize_file_name, char *,
                 (const char *));
#endif

#if GNULIB_GETLOADAVG
SIGNATURE_CHECK (GNULIB_NAMESPACE::getloadavg, int, (double[], int));
#endif

#if GNULIB_GETSUBOPT
SIGNATURE_CHECK (GNULIB_NAMESPACE::getsubopt, int,
                 (char **, char *const *, char **));
#endif

#if GNULIB_MALLOC_POSIX
SIGNATURE_CHECK (GNULIB_NAMESPACE::malloc, void *, (size_t));
#endif

#if GNULIB_MKDTEMP
SIGNATURE_CHECK (GNULIB_NAMESPACE::mkdtemp, char *, (char *));
#endif

#if GNULIB_MKOSTEMP
SIGNATURE_CHECK (GNULIB_NAMESPACE::mkostemp, int, (char *, int));
#endif

#if GNULIB_MKOSTEMPS
SIGNATURE_CHECK (GNULIB_NAMESPACE::mkostemps, int, (char *, int, int));
#endif

#if GNULIB_MKSTEMP
SIGNATURE_CHECK (GNULIB_NAMESPACE::mkstemp, int, (char *));
#endif

#if GNULIB_MKSTEMPS
SIGNATURE_CHECK (GNULIB_NAMESPACE::mkstemps, int, (char *, int));
#endif

#if GNULIB_PUTENV
SIGNATURE_CHECK (GNULIB_NAMESPACE::putenv, int, (char *));
#endif

#if GNULIB_RANDOM_R
SIGNATURE_CHECK (GNULIB_NAMESPACE::random_r, int,
                 (struct random_data *, int32_t *));
#endif

#if GNULIB_RANDOM_R
SIGNATURE_CHECK (GNULIB_NAMESPACE::srandom_r, int,
                 (unsigned int, struct random_data *));
#endif

#if GNULIB_RANDOM_R
SIGNATURE_CHECK (GNULIB_NAMESPACE::initstate_r, int,
                 (unsigned int, char *, size_t, struct random_data *));
#endif

#if GNULIB_RANDOM_R
SIGNATURE_CHECK (GNULIB_NAMESPACE::setstate_r, int,
                 (char *, struct random_data *));
#endif

#if GNULIB_REALLOC_POSIX
SIGNATURE_CHECK (GNULIB_NAMESPACE::realloc, void *, (void *, size_t));
#endif

#if GNULIB_REALPATH
SIGNATURE_CHECK (GNULIB_NAMESPACE::realpath, char *, (const char *, char *));
#endif

#if GNULIB_RPMATCH
SIGNATURE_CHECK (GNULIB_NAMESPACE::rpmatch, int, (const char *));
#endif

#if GNULIB_SETENV
SIGNATURE_CHECK (GNULIB_NAMESPACE::setenv, int,
                 (const char *, const char *, int));
#endif

#if GNULIB_STRTOD
SIGNATURE_CHECK (GNULIB_NAMESPACE::strtod, double, (const char *, char **));
#endif

#if GNULIB_STRTOLL
SIGNATURE_CHECK (GNULIB_NAMESPACE::strtoll, long long,
                 (const char *, char **, int));
#endif

#if GNULIB_STRTOULL
SIGNATURE_CHECK (GNULIB_NAMESPACE::strtoull, unsigned long long,
                 (const char *, char **, int));
#endif

#if GNULIB_UNSETENV
SIGNATURE_CHECK (GNULIB_NAMESPACE::unsetenv, int, (const char *));
#endif


int
main ()
{
}

/* Query the name of the current global locale.
   Copyright (C) 2019 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2019.  */

#include <config.h>

/* Specification.  */
#include <locale.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#if defined _WIN32 && !defined __CYGWIN__
# include <wchar.h>
#endif

#if !(SETLOCALE_NULL_ALL_MTSAFE && SETLOCALE_NULL_ONE_MTSAFE)
# if defined _WIN32 && !defined __CYGWIN__

#  define WIN32_LEAN_AND_MEAN  /* avoid including junk */
#  include <windows.h>

# elif HAVE_PTHREAD_API

#  include <pthread.h>
#  if HAVE_THREADS_H && HAVE_WEAK_SYMBOLS
#   include <threads.h>
#   pragma weak thrd_exit
#   define c11_threads_in_use() (thrd_exit != NULL)
#  else
#   define c11_threads_in_use() 0
#  endif

# elif HAVE_THREADS_H

#  include <threads.h>

# endif
#endif

/* Use the system's setlocale() function, not the gnulib override, here.  */
#undef setlocale

static int
setlocale_null_unlocked (int category, char *buf, size_t bufsize)
{
#if defined _WIN32 && !defined __CYGWIN__ && defined _MSC_VER
  /* On native Windows, nowadays, the setlocale() implementation is based
     on _wsetlocale() and uses malloc() for the result.  We are better off
     using _wsetlocale() directly.  */
  const wchar_t *result = _wsetlocale (category, NULL);
  size_t length = wcslen (result);
  if (length < bufsize)
    {
      size_t i;

      /* Convert wchar_t[] -> char[], assuming plain ASCII.  */
      for (i = 0; i <= length; i++)
        buf[i] = result[i];

      return 0;
    }
  else
    {
      if (bufsize > 0)
        {
          /* Return a truncated result in BUF.
             This is a convenience for callers that don't want to write
             explicit code for handling ERANGE.  */
          size_t i;

          /* Convert wchar_t[] -> char[], assuming plain ASCII.  */
          for (i = 0; i < bufsize; i++)
            buf[i] = result[i];
          buf[bufsize - 1] = '\0';
        }
      return ERANGE;
    }
#else
  const char *result = setlocale (category, NULL);
  size_t length = strlen (result);
  if (length < bufsize)
    {
      memcpy (buf, result, length + 1);
      return 0;
    }
  else
    {
      if (bufsize > 0)
        {
          /* Return a truncated result in BUF.
             This is a convenience for callers that don't want to write
             explicit code for handling ERANGE.  */
          memcpy (buf, result, bufsize - 1);
          buf[bufsize - 1] = '\0';
        }
      return ERANGE;
    }
#endif
}

#if !(SETLOCALE_NULL_ALL_MTSAFE && SETLOCALE_NULL_ONE_MTSAFE) /* musl libc, macOS, FreeBSD, NetBSD, OpenBSD, AIX, Haiku, Cygwin */

/* Use a lock, so that no two threads can invoke setlocale_null_unlocked
   at the same time.  */

/* Prohibit renaming this symbol.  */
# undef gl_get_setlocale_null_lock

# if defined _WIN32 && !defined __CYGWIN__

extern __declspec(dllimport) CRITICAL_SECTION *gl_get_setlocale_null_lock (void);

static int
setlocale_null_with_lock (int category, char *buf, size_t bufsize)
{
  CRITICAL_SECTION *lock = gl_get_setlocale_null_lock ();
  int ret;

  EnterCriticalSection (lock);
  ret = setlocale_null_unlocked (category, buf, bufsize);
  LeaveCriticalSection (lock);

  return ret;
}

# elif HAVE_PTHREAD_API /* musl libc, macOS, FreeBSD, NetBSD, OpenBSD, AIX, Haiku, Cygwin */

extern
#  if defined _WIN32 || defined __CYGWIN__
  __declspec(dllimport)
#  endif
  pthread_mutex_t *gl_get_setlocale_null_lock (void);

#  if HAVE_WEAK_SYMBOLS /* musl libc, FreeBSD, NetBSD, OpenBSD, Haiku */

    /* Avoid the need to link with '-lpthread'.  */
#   pragma weak pthread_mutex_lock
#   pragma weak pthread_mutex_unlock

    /* Determine whether libpthread is in use.  */
#   pragma weak pthread_mutexattr_gettype
    /* See the comments in lock.h.  */
#   define pthread_in_use() \
      (pthread_mutexattr_gettype != NULL || c11_threads_in_use ())

#  else
#   define pthread_in_use() 1
#  endif

static int
setlocale_null_with_lock (int category, char *buf, size_t bufsize)
{
  if (pthread_in_use())
    {
      pthread_mutex_t *lock = gl_get_setlocale_null_lock ();
      int ret;

      if (pthread_mutex_lock (lock))
        abort ();
      ret = setlocale_null_unlocked (category, buf, bufsize);
      if (pthread_mutex_unlock (lock))
        abort ();

      return ret;
    }
  else
    return setlocale_null_unlocked (category, buf, bufsize);
}

# elif HAVE_THREADS_H

extern mtx_t *gl_get_setlocale_null_lock (void);

static int
setlocale_null_with_lock (int category, char *buf, size_t bufsize)
{
  mtx_t *lock = gl_get_setlocale_null_lock ();
  int ret;

  if (mtx_lock (lock) != thrd_success)
    abort ();
  ret = setlocale_null_unlocked (category, buf, bufsize);
  if (mtx_unlock (lock) != thrd_success)
    abort ();

  return ret;
}

# endif

#endif

int
setlocale_null (int category, char *buf, size_t bufsize)
{
#if SETLOCALE_NULL_ALL_MTSAFE
# if SETLOCALE_NULL_ONE_MTSAFE

  return setlocale_null_unlocked (category, buf, bufsize);

# else

  if (category == LC_ALL)
    return setlocale_null_unlocked (category, buf, bufsize);
  else
    return setlocale_null_with_lock (category, buf, bufsize);

# endif
#else
# if SETLOCALE_NULL_ONE_MTSAFE

  if (category == LC_ALL)
    return setlocale_null_with_lock (category, buf, bufsize);
  else
    return setlocale_null_unlocked (category, buf, bufsize);

# else

  return setlocale_null_with_lock (category, buf, bufsize);

# endif
#endif
}

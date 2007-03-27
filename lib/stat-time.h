/* stat-related time functions.

   Copyright (C) 2005, 2007 Free Software Foundation, Inc.

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

/* Written by Paul Eggert.  */

#ifndef STAT_TIME_H
#define STAT_TIME_H 1

#include <sys/stat.h>
#include <time.h>

/* STAT_TIMESPEC (ST, ST_XTIM) is the ST_XTIM member for *ST of type
   struct timespec, if available.  If not, then STAT_TIMESPEC_NS (ST,
   ST_XTIM) is the nanosecond component of the ST_XTIM member for *ST,
   if available.  ST_XTIM can be st_atim, st_ctim, or st_mtim for
   access, status change, or data modification time, respectively.

   These macros are private to stat-time.h.  */
#if defined HAVE_STRUCT_STAT_ST_ATIM_TV_NSEC
# ifdef TYPEOF_STRUCT_STAT_ST_ATIM_IS_STRUCT_TIMESPEC
#  define STAT_TIMESPEC(st, st_xtim) ((st)->st_xtim)
# else
#  define STAT_TIMESPEC_NS(st, st_xtim) ((st)->st_xtim.tv_nsec)
# endif
#elif defined HAVE_STRUCT_STAT_ST_ATIMESPEC_TV_NSEC
# define STAT_TIMESPEC(st, st_xtim) ((st)->st_xtim##espec)
#elif defined HAVE_STRUCT_STAT_ST_ATIMENSEC
# define STAT_TIMESPEC_NS(st, st_xtim) ((st)->st_xtim##ensec)
#elif defined HAVE_STRUCT_STAT_ST_ATIM_ST__TIM_TV_NSEC
# define STAT_TIMESPEC_NS(st, st_xtim) ((st)->st_xtim.st__tim.tv_nsec)
#endif

#if defined HAVE_STRUCT_STAT_ST_BIRTHTIME || defined HAVE_STRUCT_STAT_ST_BIRTHTIMENSEC || defined HAVE_STRUCT_STAT_ST_BIRTHTIMESPEC_TV_NSEC || defined HAVE_STRUCT_STAT_ST_SPARE4
# define USE_BIRTHTIME 1
#else
# undef USE_BIRTHTIME
#endif


/* Return the nanosecond component of *ST's access time.  */
static inline long int
get_stat_atime_ns (struct stat const *st)
{
# if defined STAT_TIMESPEC
  return STAT_TIMESPEC (st, st_atim).tv_nsec;
# elif defined STAT_TIMESPEC_NS
  return STAT_TIMESPEC_NS (st, st_atim);
# elif defined HAVE_STRUCT_STAT_ST_SPARE1
  return st->st_spare1 * 1000;
# else
  return 0;
# endif
}

/* Return the nanosecond component of *ST's status change time.  */
static inline long int
get_stat_ctime_ns (struct stat const *st)
{
# if defined STAT_TIMESPEC
  return STAT_TIMESPEC (st, st_ctim).tv_nsec;
# elif defined STAT_TIMESPEC_NS
  return STAT_TIMESPEC_NS (st, st_ctim);
# elif defined HAVE_STRUCT_STAT_ST_SPARE1
  return st->st_spare3 * 1000;
# else
  return 0;
# endif
}

/* Return the nanosecond component of *ST's data modification time.  */
static inline long int
get_stat_mtime_ns (struct stat const *st)
{
# if defined STAT_TIMESPEC
  return STAT_TIMESPEC (st, st_mtim).tv_nsec;
# elif defined STAT_TIMESPEC_NS
  return STAT_TIMESPEC_NS (st, st_mtim);
# elif defined HAVE_STRUCT_STAT_ST_SPARE1
  return st->st_spare2 * 1000;
# else
  return 0;
# endif
}

/* Return the nanosecond component of *ST's birth time.  */
static inline long int
get_stat_birthtime_ns (struct stat const *st)
{
# if defined USE_BIRTHTIME
#  if defined STAT_TIMESPEC && defined HAVE_STRUCT_STAT_ST_BIRTHTIMESPEC_TV_NSEC
  return STAT_TIMESPEC (st, st_birthtim).tv_nsec;
#  elif defined STAT_TIMESPEC_NS && defined HAVE_STRUCT_STAT_ST_BIRTHTIMESPEC_TV_SEC
  return STAT_TIMESPEC_NS (st, st_birthtim);
#  elif defined HAVE_STRUCT_STAT_ST_SPARE4
  /* Cygwin, without __CYGWIN_USE_BIG_TYPES__ */
  return st->st_spare4[1] * 1000L;
#  else
  /* Birthtime is available, but not at nanosecond resolution.  */
  return 0;
#  endif
# else
  /* Birthtime is not available, so indicate this in the returned value.  */
  return 0;
# endif 
}

/* Return *ST's access time.  */
static inline struct timespec
get_stat_atime (struct stat const *st)
{
#ifdef STAT_TIMESPEC
  return STAT_TIMESPEC (st, st_atim);
#else
  struct timespec t;
  t.tv_sec = st->st_atime;
  t.tv_nsec = get_stat_atime_ns (st);
  return t;
#endif
}

/* Return *ST's status change time.  */
static inline struct timespec
get_stat_ctime (struct stat const *st)
{
#ifdef STAT_TIMESPEC
  return STAT_TIMESPEC (st, st_ctim);
#else
  struct timespec t;
  t.tv_sec = st->st_ctime;
  t.tv_nsec = get_stat_ctime_ns (st);
  return t;
#endif
}

/* Return *ST's data modification time.  */
static inline struct timespec
get_stat_mtime (struct stat const *st)
{
#ifdef STAT_TIMESPEC
  return STAT_TIMESPEC (st, st_mtim);
#else
  struct timespec t;
  t.tv_sec = st->st_mtime;
  t.tv_nsec = get_stat_mtime_ns (st);
  return t;
#endif
}

/* Return *ST's birth time, if available, in *PTS.  A nonzero value is
 * returned if the stat structure appears to indicate that the
 * timestamp is available.
 *
 * The return value of this function does not reliably indicate that the 
 * returned data is valid; see the comments within the body of the 
 * function for an explanation.
 */
static inline int
get_stat_birthtime (struct stat const *st,
		    struct timespec *pts)
{
#if defined USE_BIRTHTIME
# ifdef STAT_TIMESPEC
  *pts = STAT_TIMESPEC (st, st_birthtim);
# else
  struct timespec t;
  pts->tv_sec = st->st_birthtime;
  pts->tv_nsec = get_stat_birthtime_ns (st);
# endif

  /* NetBSD sometimes signals the absence of knowledge of the file's
   * birth time by using zero.  We indicate we don't know, by
   * returning 0 from this function when that happens.  This is
   * slightly problematic since (time_t)0 is otherwise a valid, albeit
   * unlikely, timestamp.  
   *
   * NetBSD sometimes returns 0 for unknown values (for example on
   * ffs) and sometimes begative values for tv_nsec (for example on
   * NFS).  For some filesystems (e.g. msdos) NetBSD also appears to
   * fail to update the st_birthtime member at all, and just leaves in
   * there whatever junk existed int he uninitialised stat structure
   * the caller provided.  Therefore, callers are advised to initialise
   * the tv_nsec number to a negative value before they call stat in
   * order to detect this problem.
   */
  if (pts->tv_sec == (time_t)0)
    {
      return 0;			/* result probably invalid, see above. */
    }
  else
    {
      /* Sometimes NetBSD returns junk in the birth time fields, so 
       * do a simple range check on the data, and return 0 to indicate
       * that the data is invalid if it just looks wrong. 
       */
      return (pts->tv_nsec >= 0) && (pts->tv_nsec <= 1000000000);
    }
#elif (defined _WIN32 || defined __WIN32__) && !defined __CYGWIN__
  /* Woe32 native platforms (mingw, msvc, but not Cygwin) put the
   * "file creation time" in st_ctime (!).  See for example the
   * article
   * <http://msdn2.microsoft.com/de-de/library/14h5k7ff(VS.80).aspx>
   */
  pts->tv_sec = st->st_ctime;
  pts->tv_nsec = 0;
  return 1;			/* result is valid */
#else
  /* Birth time not supported.  */
  pts->tv_sec = 0;
  pts->tv_nsec = 0;
  return 0;			/* result is not valid */
#endif
}

#endif

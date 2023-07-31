/* GNU's read utmp module.

   Copyright (C) 1992-2001, 2003-2006, 2009-2023 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by jla; revised by djm */

#include <config.h>

#include "readutmp.h"

#include <errno.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "xalloc.h"

/* Each of the FILE streams in this file is only used in a single thread.  */
#include "unlocked-io.h"

#if 8 <= __GNUC__
# pragma GCC diagnostic ignored "-Wsizeof-pointer-memaccess"
#endif

/* Copy UT_USER (UT) into storage obtained from malloc.  Then remove any
   trailing spaces from the copy, NUL terminate it, and return the copy.  */

char *
extract_trimmed_name (const STRUCT_UTMP *ut)
{
  char *p, *trimmed_name;

  trimmed_name = xmalloc (sizeof (UT_USER (ut)) + 1);
  strncpy (trimmed_name, UT_USER (ut), sizeof (UT_USER (ut)));
  /* Append a trailing NUL.  Some systems pad names shorter than the
     maximum with spaces, others pad with NULs.  Remove any trailing
     spaces.  */
  trimmed_name[sizeof (UT_USER (ut))] = '\0';
  for (p = trimmed_name + strlen (trimmed_name);
       trimmed_name < p && p[-1] == ' ';
       *--p = '\0')
    continue;
  return trimmed_name;
}

/* Is the utmp entry U desired by the user who asked for OPTIONS?  */

static bool
desirable_utmp_entry (STRUCT_UTMP const *u, int options)
{
  bool user_proc = IS_USER_PROCESS (u);
  if ((options & READ_UTMP_USER_PROCESS) && !user_proc)
    return false;
  if ((options & READ_UTMP_CHECK_PIDS)
      && user_proc
      && 0 < UT_PID (u)
      && (kill (UT_PID (u), 0) < 0 && errno == ESRCH))
    return false;
# if defined __OpenBSD__ && !HAVE_UTMPX_H
  /* Eliminate entirely empty entries.  */
  if (UT_TIME_MEMBER (ut) == 0 && UT_USER (ut)[0] == '\0'
      && ut->ut_line[0] == '\0' && ut->ut_host[0] == '\0')
    return false;
# endif
  return true;
}

#ifdef UTMP_NAME_FUNCTION

static void
copy_utmp_entry (STRUCT_UTMP *dst, STRUCT_UTMP *src)
{
#if __GLIBC__ && _TIME_BITS == 64
  /* Convert from external form in SRC to internal form in DST.
     It is OK to convert now, rather than earlier, before
     desirable_utmp_entry was invoked, because desirable_utmp_entry
     inspects only the leading prefix of the entry, which is the
     same in both external and internal forms.  */

  /* This is a near-copy of glibc's struct utmpx, which stops working
     after the year 2038.  Unlike the glibc version, struct utmpx32
     describes the file format even if time_t is 64 bits.  */
  struct utmpx32
  {
    short int ut_type;			/* Type of login.  */
    pid_t ut_pid;			/* Process ID of login process.  */
    char ut_line[sizeof src->ut_line];	/* Devicename.  */
    char ut_id[sizeof src->ut_id];	/* Inittab ID.  */
    char ut_user[sizeof src->ut_user];  /* Username.  */
    char ut_host[sizeof src->ut_host];	/* Hostname for remote login.  */
    struct __exit_status ut_exit;	/* Exit status of a process marked
                                           as DEAD_PROCESS.  */
    /* The fields ut_session and ut_tv must be the same size when compiled
       32- and 64-bit.  This allows files and shared memory to be shared
       between 32- and 64-bit applications.  */
    int ut_session;			/* Session ID, used for windowing.  */
    struct
    {
      int tv_sec;			/* Seconds.  */
      int tv_usec;			/* Microseconds.  */
    } ut_tv;				/* Time entry was made.  */
    int ut_addr_v6[4];			/* Internet address of remote host.  */
    char ut_reserved[20];		/* Reserved for future use.  */
  } *s = (struct utmpx32 *) src;
  memcpy (dst, s, offsetof (struct utmpx32, ut_session));
  dst->ut_session = s->ut_session;
  dst->ut_tv.tv_sec = s->ut_tv.tv_sec;
  dst->ut_tv.tv_usec = s->ut_tv.tv_usec;
  memcpy (&dst->ut_addr_v6, s->ut_addr_v6, sizeof dst->ut_addr_v6);
#else
  *dst = *src;
#endif
}

int
read_utmp (char const *file, size_t *n_entries, STRUCT_UTMP **utmp_buf,
           int options)
{
  idx_t n_read = 0;
  idx_t n_alloc = 0;
  STRUCT_UTMP *utmp = NULL;
  STRUCT_UTMP *u;

  /* Ignore the return value for now.
     Solaris' utmpname returns 1 upon success -- which is contrary
     to what the GNU libc version does.  In addition, older GNU libc
     versions are actually void.   */
  UTMP_NAME_FUNCTION ((char *) file);

  SET_UTMP_ENT ();

  while ((u = GET_UTMP_ENT ()) != NULL)
    if (desirable_utmp_entry (u, options))
      {
        if (n_read == n_alloc)
          utmp = xpalloc (utmp, &n_alloc, 1, -1, sizeof *utmp);

        copy_utmp_entry (&utmp[n_read++], u);
      }

  END_UTMP_ENT ();

  *n_entries = n_read;
  *utmp_buf = utmp;

  return 0;
}

#else

int
read_utmp (char const *file, size_t *n_entries, STRUCT_UTMP **utmp_buf,
           int options)
{
  idx_t n_read = 0;
  idx_t n_alloc = 0;
  STRUCT_UTMP *utmp = NULL;
  int saved_errno;
  FILE *f = fopen (file, "re");

  if (! f)
    return -1;

  for (;;)
    {
      if (n_read == n_alloc)
        utmp = xpalloc (utmp, &n_alloc, 1, -1, sizeof *utmp);
      if (fread (&utmp[n_read], sizeof utmp[n_read], 1, f) == 0)
        break;
      n_read += desirable_utmp_entry (&utmp[n_read], options);
    }

  saved_errno = ferror (f) ? errno : 0;
  if (fclose (f) != 0)
    saved_errno = errno;
  if (saved_errno != 0)
    {
      free (utmp);
      errno = saved_errno;
      return -1;
    }

  *n_entries = n_read;
  *utmp_buf = utmp;

  return 0;
}

#endif

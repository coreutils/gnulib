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

#if READUTMP_USE_SYSTEMD
# include <dirent.h>
# include <sys/sysinfo.h>
# include <systemd/sd-login.h>
# include <time.h>
#endif

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

#if READUTMP_USE_SYSTEMD
  trimmed_name = xstrdup (UT_USER (ut));
#else
  trimmed_name = xmalloc (UT_USER_SIZE + 1);
  strncpy (trimmed_name, UT_USER (ut), UT_USER_SIZE);
  /* Append a trailing NUL.  Some systems pad names shorter than the
     maximum with spaces, others pad with NULs.  */
  trimmed_name[UT_USER_SIZE] = '\0';
#endif
  /* Remove any trailing spaces.  */
  for (p = trimmed_name + strlen (trimmed_name);
       trimmed_name < p && p[-1] == ' ';
       *--p = '\0')
    ;
  return trimmed_name;
}

#if READ_UTMP_SUPPORTED

/* Is the utmp entry UT desired by the user who asked for OPTIONS?  */

static bool
desirable_utmp_entry (STRUCT_UTMP const *ut, int options)
{
  bool user_proc = IS_USER_PROCESS (ut);
  if ((options & READ_UTMP_USER_PROCESS) && !user_proc)
    return false;
  if ((options & READ_UTMP_CHECK_PIDS)
      && user_proc
      && 0 < UT_PID (ut)
      && (kill (UT_PID (ut), 0) < 0 && errno == ESRCH))
    return false;
# if defined __OpenBSD__ && !HAVE_UTMPX_H
  /* Eliminate entirely empty entries.  */
  if (UT_TIME_MEMBER (ut) == 0 && UT_USER (ut)[0] == '\0'
      && ut->ut_line[0] == '\0' && ut->ut_host[0] == '\0')
    return false;
# endif
  return true;
}

# if READUTMP_USE_SYSTEMD
/* Use systemd and Linux /proc and kernel APIs.  */

static struct timeval
get_boot_time_uncached ()
{
  /* /proc/uptime contains the uptime with a resolution of 0.01 sec.  */
  FILE *fp = fopen ("/proc/uptime", "re");
  if (fp != NULL)
    {
      char buf[32 + 1];
      size_t n = fread (buf, 1, sizeof (buf) - 1, fp);
      fclose (fp);
      if (n > 0)
        {
          buf[n] = '\0';
          /* buf now contains two values: the uptime and the idle time.  */
          char *endptr;
          double uptime = strtod (buf, &endptr);
          if (endptr > buf)
            {
              struct timeval result;
              if (gettimeofday (&result, NULL) >= 0)
                {
                  long uptime_sec = (long) uptime;
                  int uptime_usec =
                    (int) ((uptime - (double) uptime_sec) * 1000000.0 + 0.5);
                  if (result.tv_usec < uptime_usec)
                    {
                      result.tv_usec += 1000000;
                      result.tv_sec -= 1;
                    }
                  result.tv_sec -= uptime_sec;
                  result.tv_usec -= uptime_usec;
                  return result;
                }
            }
        }
    }

  /* The sysinfo call returns the uptime with a resolution of 1 sec only.  */
  struct sysinfo info;
  if (sysinfo (&info) >= 0)
    {
      struct timeval result;
      if (gettimeofday (&result, NULL) >= 0)
        {
          result.tv_sec -= info.uptime;
          return result;
        }
    }

  /* We shouldn't get here.  */
  return (struct timeval) { .tv_sec = 0, .tv_usec = 0 };
}

static struct timeval
get_boot_time ()
{
  static int cached;
  static struct timeval boot_time;

  if (!cached)
    {
      boot_time = get_boot_time_uncached ();
      cached = 1;
    }
  return boot_time;
}

/* Guess the pty name that was opened for the given user right after
   the given time AT.  */
static char *
guess_pty_name (uid_t uid, const struct timespec at)
{
  /* Traverse the entries of the /dev/pts/ directory, looking for devices
     which are owned by UID and whose ctime is shortly after AT.  */
  DIR *dirp = opendir ("/dev/pts");
  if (dirp != NULL)
    {
      /* Buffer containing /dev/pts/N.  */
      char name_buf[9 + 10 + 1];
      memcpy (name_buf, "/dev/pts/", 9);

      char best_name[9 + 10 + 1];
      struct timespec best_time = { .tv_sec = 0, .tv_nsec = 0 };

      for (;;)
        {
          struct dirent *dp = readdir (dirp);
          if (dp == NULL)
            break;
          if (dp->d_name[0] != '.' && strlen (dp->d_name) <= 10)
            {
              /* Compose the absolute file name /dev/pts/N.  */
              strcpy (name_buf + 9, dp->d_name);

              /* Find its owner and ctime.  */
              struct stat st;
              if (stat (name_buf, &st) >= 0
                  && st.st_uid == uid
                  && (st.st_ctim.tv_sec > at.tv_sec
                      || (st.st_ctim.tv_sec == at.tv_sec
                          && st.st_ctim.tv_nsec >= at.tv_nsec)))
                {
                  /* This entry has the owner UID and a ctime >= AT.  */
                  /* Is this entry the best one so far?  */
                  if ((best_time.tv_sec == 0 && best_time.tv_nsec == 0)
                      || (st.st_ctim.tv_sec < best_time.tv_sec
                          || (st.st_ctim.tv_sec == best_time.tv_sec
                              && st.st_ctim.tv_nsec < best_time.tv_nsec)))
                    {
                      strcpy (best_name, name_buf);
                      best_time = st.st_ctim;
                    }
                }
            }
        }

      closedir (dirp);

      /* Did we find an entry owned by ID, and is it at most 5 seconds
         after AT?  */
      if (!(best_time.tv_sec == 0 && best_time.tv_nsec == 0)
          && (best_time.tv_sec < at.tv_sec + 5
              || (best_time.tv_sec == at.tv_sec + 5
                  && best_time.tv_nsec <= at.tv_nsec)))
        return xstrdup (best_name + 5);
    }

  return NULL;
}

int
read_utmp (char const *file, size_t *n_entries, STRUCT_UTMP **utmp_buf,
           int options)
{
  /* Fill entries, simulating what an utmp file would contain.  */
  idx_t n_filled = 0;
  idx_t n_alloc = 0;
  STRUCT_UTMP *utmp = NULL;

  /* Synthesize a BOOT_TIME entry.  */
  if (!(options & READ_UTMP_USER_PROCESS))
    {
      if (n_filled == n_alloc)
        utmp = xpalloc (utmp, &n_alloc, 1, -1, sizeof (STRUCT_UTMP));
      STRUCT_UTMP *ut = &utmp[n_filled];
      ut->ut_user = xstrdup ("reboot");
      ut->ut_id = xstrdup ("");
      ut->ut_line = xstrdup ("~");
      ut->ut_pid = 0;
      ut->ut_type = BOOT_TIME;
      ut->ut_tv = get_boot_time ();
      ut->ut_host = xstrdup ("");
      ut->ut_session = 0;
      if (desirable_utmp_entry (ut, options))
        n_filled++;
      else
        free_utmp (1, ut);
    }

  /* Synthesize USER_PROCESS entries.  */
  char **sessions;
  int num_sessions = sd_get_sessions (&sessions);
  if (num_sessions >= 0)
    {
      char **session_ptr;
      for (session_ptr = sessions; *session_ptr != NULL; session_ptr++)
        {
          char *session = *session_ptr;

          uint64_t start_usec;
          if (sd_session_get_start_time (session, &start_usec) < 0)
            start_usec = 0;
          struct timeval start_tv;
          start_tv.tv_sec = start_usec / 1000000;
          start_tv.tv_usec = start_usec % 1000000;

          char *seat;
          if (sd_session_get_seat (session, &seat) < 0)
            seat = NULL;

          char *tty;
          if (sd_session_get_tty (session, &tty) < 0)
            {
              tty = NULL;
              /* Try harder to get a sensible value for the tty.  */
              char *type;
              if (sd_session_get_type (session, &type) >= 0)
                {
                  if (strcmp (type, "tty") == 0)
                    {
                      char *service;
                      if (sd_session_get_service (session, &service) < 0)
                        service = NULL;

                      char *pty;
                      uid_t uid;
                      if (sd_session_get_uid (session, &uid) >= 0)
                        {
                          struct timespec start_ts =
                            {
                              .tv_sec = start_tv.tv_sec,
                              .tv_nsec = start_tv.tv_usec * 1000
                            };
                          pty = guess_pty_name (uid, start_ts);
                        }
                      else
                        pty = NULL;

                      if (service != NULL && pty != NULL)
                        {
                          tty = xmalloc (strlen (service) + 1 + strlen (pty) + 1);
                          stpcpy (stpcpy (stpcpy (tty, service), " "), pty);
                        }
                      else if (service != NULL)
                        tty = xstrdup (service);
                      else if (pty != NULL)
                        tty = xstrdup (pty);

                      free (pty);
                      free (service);
                    }
                  free (type);
                }
            }

          /* Create up to two USER_PROCESS entries: one for the seat,
             one for the tty.  */
          if (seat != NULL || tty != NULL)
            {
              char *user;
              if (sd_session_get_username (session, &user) < 0)
                user = xstrdup ("");

              pid_t leader_pid;
              if (sd_session_get_leader (session, &leader_pid) < 0)
                leader_pid = 0;

              char *remote_host;
              if (sd_session_get_remote_host (session, &remote_host) < 0)
                remote_host = NULL;
              char *remote_user;
              if (sd_session_get_remote_user (session, &remote_user) < 0)
                remote_user = NULL;
              char *host;
              if (remote_host != NULL)
                {
                  if (remote_user != NULL)
                    {
                      host = xmalloc (strlen (remote_user) + 1 + strlen (remote_host) + 1);
                      stpcpy (stpcpy (stpcpy (host, remote_user), "@"), remote_host);
                    }
                  else
                    host = xstrdup (remote_host);
                }
              else
                host = xstrdup ("");

              size_t n_filled_after = n_filled + (seat != NULL) + (tty != NULL);
              if (n_filled_after > n_alloc)
                utmp = xpalloc (utmp, &n_alloc, n_filled_after - n_alloc, -1,
                                sizeof (STRUCT_UTMP));
              if (seat != NULL)
                {
                  STRUCT_UTMP *ut = &utmp[n_filled];
                  ut->ut_user = xstrdup (user);
                  ut->ut_id = xstrdup (session);
                  ut->ut_line = xstrdup (seat);
                  ut->ut_pid = leader_pid; /* this is the best we have */
                  ut->ut_type = USER_PROCESS;
                  ut->ut_tv = start_tv;
                  ut->ut_host = xstrdup (host);
                  ut->ut_session = leader_pid;
                  if (desirable_utmp_entry (ut, options))
                    n_filled++;
                  else
                    free_utmp (1, ut);
                }
              if (tty != NULL)
                {
                  STRUCT_UTMP *ut = &utmp[n_filled];
                  ut->ut_user = xstrdup (user);
                  ut->ut_id = xstrdup (session);
                  ut->ut_line = xstrdup (tty);
                  ut->ut_pid = leader_pid; /* this is the best we have */
                  ut->ut_type = USER_PROCESS;
                  ut->ut_tv = start_tv;
                  ut->ut_host = xstrdup (host);
                  ut->ut_session = leader_pid;
                  if (desirable_utmp_entry (ut, options))
                    n_filled++;
                  else
                    free_utmp (1, ut);
                }

              free (host);
              free (remote_user);
              free (remote_host);
              free (user);
            }
          free (tty);
          free (seat);
          free (session);
        }
      free (sessions);
    }

  *n_entries = n_filled;
  *utmp_buf = utmp;

  return 0;
}

# elif defined UTMP_NAME_FUNCTION

static void
copy_utmp_entry (STRUCT_UTMP *dst, STRUCT_UTMP *src)
{
#  if __GLIBC__ && _TIME_BITS == 64
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
#  else
  *dst = *src;
#  endif
}

int
read_utmp (char const *file, size_t *n_entries, STRUCT_UTMP **utmp_buf,
           int options)
{
  idx_t n_read = 0;
  idx_t n_alloc = 0;
  STRUCT_UTMP *utmp = NULL;
  STRUCT_UTMP *ut;

  /* Ignore the return value for now.
     Solaris' utmpname returns 1 upon success -- which is contrary
     to what the GNU libc version does.  In addition, older GNU libc
     versions are actually void.   */
  UTMP_NAME_FUNCTION ((char *) file);

  SET_UTMP_ENT ();

  while ((ut = GET_UTMP_ENT ()) != NULL)
    if (desirable_utmp_entry (ut, options))
      {
        if (n_read == n_alloc)
          utmp = xpalloc (utmp, &n_alloc, 1, -1, sizeof *utmp);

        copy_utmp_entry (&utmp[n_read++], ut);
      }

  END_UTMP_ENT ();

  *n_entries = n_read;
  *utmp_buf = utmp;

  return 0;
}

# else

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

# endif

#else /* dummy fallback */

int
read_utmp (char const *file, size_t *n_entries, STRUCT_UTMP **utmp_buf,
           int options)
{
  errno = ENOSYS;
  return -1;
}

#endif

void
free_utmp (size_t n_entries, STRUCT_UTMP *entries)
{
#if READUTMP_USE_SYSTEMD
  size_t i;
  for (i = 0; i < n_entries; i++)
    {
      STRUCT_UTMP *ut = &entries[i];
      free (ut->ut_user);
      free (ut->ut_id);
      free (ut->ut_line);
      free (ut->ut_host);
    }
#endif
}

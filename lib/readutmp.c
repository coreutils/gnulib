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

#include "stat-time.h"
#include "xalloc.h"

/* Each of the FILE streams in this file is only used in a single thread.  */
#include "unlocked-io.h"

/* The following macros describe the 'struct UTMP_STRUCT_NAME',
   *not* 'struct gl_utmp'.  */
#undef UT_USER
#undef UT_TIME_MEMBER
#undef UT_PID
#undef UT_TYPE_EQ
#undef UT_TYPE_NOT_DEFINED
#undef IS_USER_PROCESS
#undef UT_EXIT_E_TERMINATION
#undef UT_EXIT_E_EXIT

/* Accessor macro for the member named ut_user or ut_name.  */
#if (HAVE_UTMPX_H ? HAVE_STRUCT_UTMPX_UT_NAME \
     : HAVE_UTMP_H && HAVE_STRUCT_UTMP_UT_NAME)
# define UT_USER(UT) ((UT)->ut_name)
#else
# define UT_USER(UT) ((UT)->ut_user)
#endif

/* Accessor macro for the member of type time_t (or 'unsigned int').  */
#if HAVE_UTMPX_H || (HAVE_UTMP_H && HAVE_STRUCT_UTMP_UT_TV)
# define UT_TIME_MEMBER(UT) ((UT)->ut_tv.tv_sec)
#else
# define UT_TIME_MEMBER(UT) ((UT)->ut_time)
#endif

/* Accessor macro for the member named ut_pid.  */
#if (HAVE_UTMPX_H ? HAVE_STRUCT_UTMPX_UT_PID : HAVE_STRUCT_UTMP_UT_PID)
# define UT_PID(UT) ((UT)->ut_pid)
#else
# define UT_PID(UT) 0
#endif

/* Accessor macros for the member named ut_type.  */
#if (HAVE_UTMPX_H ? HAVE_STRUCT_UTMP_UT_TYPE : HAVE_STRUCT_UTMPX_UT_TYPE)
# define UT_TYPE_EQ(UT, V) ((UT)->ut_type == (V))
# define UT_TYPE_NOT_DEFINED 0
#else
# define UT_TYPE_EQ(UT, V) 0
# define UT_TYPE_NOT_DEFINED 1
#endif

/* Determines whether an entry *UT corresponds to a user process.  */
#define IS_USER_PROCESS(UT)                                    \
  ((UT)->ut_user[0]                                            \
   && (UT_TYPE_USER_PROCESS (UT)                               \
       || (UT_TYPE_NOT_DEFINED && (UT)->ut_ts.tv_sec != 0)))

#if HAVE_UTMPX_H
# if HAVE_STRUCT_UTMPX_UT_EXIT_E_TERMINATION
#  define UT_EXIT_E_TERMINATION(UT) ((UT)->ut_exit.e_termination)
# elif HAVE_STRUCT_UTMPX_UT_EXIT_UT_TERMINATION /* OSF/1 */
#  define UT_EXIT_E_TERMINATION(UT) ((UT)->ut_exit.ut_termination)
# else
#  define UT_EXIT_E_TERMINATION(UT) 0
# endif
#elif HAVE_UTMP_H
# if HAVE_STRUCT_UTMP_UT_EXIT_E_TERMINATION
#  define UT_EXIT_E_TERMINATION(UT) ((UT)->ut_exit.e_termination)
# else
#  define UT_EXIT_E_TERMINATION(UT) 0
# endif
#endif

#if HAVE_UTMPX_H
# if HAVE_STRUCT_UTMPX_UT_EXIT_E_EXIT
#  define UT_EXIT_E_EXIT(UT) ((UT)->ut_exit.e_exit)
# elif HAVE_STRUCT_UTMPX_UT_EXIT_UT_EXIT /* OSF/1 */
#  define UT_EXIT_E_EXIT(UT) ((UT)->ut_exit.ut_exit)
# else
#  define UT_EXIT_E_EXIT(UT) 0
# endif
#elif HAVE_UTMP_H
# if HAVE_STRUCT_UTMP_UT_EXIT_E_EXIT
#  define UT_EXIT_E_EXIT(UT) ((UT)->ut_exit.e_exit)
# else
#  define UT_EXIT_E_EXIT(UT) 0
# endif
#endif

/* Size of the UT_USER (ut) member.  */
#define UT_USER_SIZE  sizeof UT_USER ((struct UTMP_STRUCT_NAME *) 0)
/* Size of the ut->ut_id member.  */
#define UT_ID_SIZE    sizeof (((struct UTMP_STRUCT_NAME *) 0)->ut_id)
/* Size of the ut->ut_line member.  */
#define UT_LINE_SIZE  sizeof (((struct UTMP_STRUCT_NAME *) 0)->ut_line)
/* Size of the ut->ut_host member.  */
#define UT_HOST_SIZE  sizeof (((struct UTMP_STRUCT_NAME *) 0)->ut_host)

#define SIZEOF(a) (sizeof(a)/sizeof(a[0]))

#if 8 <= __GNUC__
# pragma GCC diagnostic ignored "-Wsizeof-pointer-memaccess"
#endif

/* Copy UT->ut_user into storage obtained from malloc.  Then remove any
   trailing spaces from the copy, NUL terminate it, and return the copy.  */

char *
extract_trimmed_name (const STRUCT_UTMP *ut)
{
  char const *name = ut->ut_user;
  idx_t len = strlen (name);
  char const *p;
  for (p = name + len; name < p && p[-1] == ' '; p--)
    continue;
  return ximemdup0 (name, p - name);
}

#if READ_UTMP_SUPPORTED

/* Is the utmp entry UT desired by the user who asked for OPTIONS?  */

static bool
desirable_utmp_entry (STRUCT_UTMP const *ut, int options)
{
# if defined __OpenBSD__ && !HAVE_UTMPX_H
  /* Eliminate entirely empty entries.  */
  if (ut->ut_ts.tv_sec == 0 && ut->ut_user[0] == '\0'
      && ut->ut_line[0] == '\0' && ut->ut_host[0] == '\0')
    return false;
# endif

  bool boot_time = UT_TYPE_BOOT_TIME (ut);
  if ((options & READ_UTMP_BOOT_TIME) && !boot_time)
    return false;
  if ((options & READ_UTMP_NO_BOOT_TIME) && boot_time)
    return false;

  bool user_proc = IS_USER_PROCESS (ut);
  if ((options & READ_UTMP_USER_PROCESS) && !user_proc)
    return false;
  if ((options & READ_UTMP_CHECK_PIDS)
      && user_proc
      && 0 < UT_PID (ut)
      && (kill (UT_PID (ut), 0) < 0 && errno == ESRCH))
    return false;

  return true;
}

/* A memory allocation for an in-progress read_utmp.  */

struct utmp_alloc
{
  /* A pointer to a possibly-empty array of utmp entries,
     followed by a possibly-empty sequence of unused bytes,
     followed by a possibly-empty sequence of string bytes.
     UTMP is either null or allocated by malloc.  */
  struct gl_utmp *utmp;

  /* The number of utmp entries.  */
  idx_t filled;

  /* The string byte sequence length.  Strings are null-terminated.  */
  idx_t string_bytes;

  /* The total number of bytes allocated.  This equals
     FILLED * sizeof *UTMP + [size of free area] + STRING_BYTES.  */
  idx_t alloc_bytes;
};

/* Use the memory allocation A, and if the read_utmp options OPTIONS
   permit it, add a new entry with the given USER, etc.  Grow A as
   needed, reporting an error and exit on memory allocation failure.
   Return the resulting memory allocation.  */

static struct utmp_alloc
add_utmp (struct utmp_alloc a, int options,
          char const *user, idx_t user_len,
          char const *id, idx_t id_len,
          char const *line, idx_t line_len,
          char const *host, idx_t host_len,
          pid_t pid, short type, struct timespec ts, long session,
          int termination, int exit)
{
  int entry_bytes = sizeof (struct gl_utmp);
  idx_t avail = a.alloc_bytes - (entry_bytes * a.filled + a.string_bytes);
  idx_t needed_string_bytes =
    (user_len + 1) + (id_len + 1) + (line_len + 1) + (host_len + 1);
  idx_t needed = entry_bytes + needed_string_bytes;
  if (avail < needed)
    {
      idx_t old_string_offset = a.alloc_bytes - a.string_bytes;
      void *new = xpalloc (a.utmp, &a.alloc_bytes, needed - avail, -1, 1);
      idx_t new_string_offset = a.alloc_bytes - a.string_bytes;
      a.utmp = new;
      char *q = new;
      memmove (q + new_string_offset, q + old_string_offset, a.string_bytes);
    }
  struct gl_utmp *ut = &a.utmp[a.filled];
  char *stringlim = (char *) a.utmp + a.alloc_bytes;
  char *p = stringlim - a.string_bytes;
  *--p = '\0'; /* NUL-terminate ut->ut_user */
  ut->ut_user = p = memcpy (p - user_len, user, user_len);
  *--p = '\0'; /* NUL-terminate ut->ut_id */
  ut->ut_id   = p = memcpy (p -   id_len,   id,   id_len);
  *--p = '\0'; /* NUL-terminate ut->ut_line */
  ut->ut_line = p = memcpy (p - line_len, line, line_len);
  *--p = '\0'; /* NUL-terminate ut->ut_host */
  ut->ut_host =     memcpy (p - host_len, host, host_len);
  ut->ut_ts = ts;
  ut->ut_pid = pid;
  ut->ut_session = session;
  ut->ut_type = type;
  ut->ut_exit.e_termination = termination;
  ut->ut_exit.e_exit = exit;
  if (desirable_utmp_entry (ut, options))
    {
      /* Now that UT has been checked, relocate its string slots to be
         relative to the end of the allocated storage, so that these
         slots survive realloc.  The slots will be relocated back just
         before read_utmp returns.  */
      ut->ut_user = (char *) (intptr_t) (ut->ut_user - stringlim);
      ut->ut_id   = (char *) (intptr_t) (ut->ut_id   - stringlim);
      ut->ut_line = (char *) (intptr_t) (ut->ut_line - stringlim);
      ut->ut_host = (char *) (intptr_t) (ut->ut_host - stringlim);
      a.filled++;
      a.string_bytes += needed_string_bytes;
    }
  return a;
}

/* Relocate the string pointers in A back to their natural position.  */
static struct utmp_alloc
finish_utmp (struct utmp_alloc a)
{
  char *stringlim = (char *) a.utmp + a.alloc_bytes;

  for (idx_t i = 0; i < a.filled; i++)
    {
      a.utmp[i].ut_user = (intptr_t) a.utmp[i].ut_user + stringlim;
      a.utmp[i].ut_id   = (intptr_t) a.utmp[i].ut_id   + stringlim;
      a.utmp[i].ut_line = (intptr_t) a.utmp[i].ut_line + stringlim;
      a.utmp[i].ut_host = (intptr_t) a.utmp[i].ut_host + stringlim;
    }

  return a;
}

# if !HAVE_UTMPX_H && HAVE_UTMP_H && defined UTMP_NAME_FUNCTION && !HAVE_DECL_GETUTENT
struct utmp *getutent (void);
# endif

static int
read_utmp_from_file (char const *file, idx_t *n_entries, STRUCT_UTMP **utmp_buf,
                     int options)
{
  if ((options & READ_UTMP_BOOT_TIME) != 0
      && (options & (READ_UTMP_USER_PROCESS | READ_UTMP_NO_BOOT_TIME)) != 0)
    {
      /* No entries can match the given options.  */
      *n_entries = 0;
      *utmp_buf = NULL;
      return 0;
    }

  struct utmp_alloc a = {0};

# if defined UTMP_NAME_FUNCTION /* glibc, musl, macOS, FreeBSD, NetBSD, Minix, AIX, IRIX, Solaris, Cygwin, Android */

  /* Ignore the return value for now.
     Solaris' utmpname returns 1 upon success -- which is contrary
     to what the GNU libc version does.  In addition, older GNU libc
     versions are actually void.   */
  UTMP_NAME_FUNCTION ((char *) file);

  SET_UTMP_ENT ();

  void const *entry;

  while ((entry = GET_UTMP_ENT ()) != NULL)
    {
#  if __GLIBC__ && _TIME_BITS == 64
      /* This is a near-copy of glibc's struct utmpx, which stops working
         after the year 2038.  Unlike the glibc version, struct utmpx32
         describes the file format even if time_t is 64 bits.  */
      struct utmpx32
      {
        short int ut_type;               /* Type of login.  */
        pid_t ut_pid;                    /* Process ID of login process.  */
        char ut_line[UT_LINE_SIZE];      /* Devicename.  */
        char ut_id[UT_ID_SIZE];          /* Inittab ID.  */
        char ut_user[UT_USER_SIZE];      /* Username.  */
        char ut_host[UT_HOST_SIZE];      /* Hostname for remote login. */
        struct __exit_status ut_exit;    /* Exit status of a process marked
                                            as DEAD_PROCESS.  */
        /* The fields ut_session and ut_tv must be the same size when compiled
           32- and 64-bit.  This allows files and shared memory to be shared
           between 32- and 64-bit applications.  */
        int ut_session;                  /* Session ID, used for windowing.  */
        struct
        {
          /* Seconds.  Unsigned not signed, as glibc did not exist before 1970,
             and if the format is still in use after 2038 its timestamps
             will surely have the sign bit on.  This hack stops working
             at 2106-02-07 06:28:16 UTC.  */
          unsigned int tv_sec;
          int tv_usec;                   /* Microseconds.  */
        } ut_tv;                         /* Time entry was made.  */
        int ut_addr_v6[4];               /* Internet address of remote host.  */
        char ut_reserved[20];            /* Reserved for future use.  */
      };
      struct utmpx32 const *ut = (struct utmpx32 const *) entry;
#  else
      struct UTMP_STRUCT_NAME const *ut = (struct UTMP_STRUCT_NAME const *) entry;
#  endif

      a = add_utmp (a, options,
                    UT_USER (ut), strnlen (UT_USER (ut), UT_USER_SIZE),
                    #if (HAVE_UTMPX_H ? HAVE_STRUCT_UTMPX_UT_ID : HAVE_STRUCT_UTMP_UT_ID)
                    ut->ut_id, strnlen (ut->ut_id, UT_ID_SIZE),
                    #else
                    "", 0,
                    #endif
                    ut->ut_line, strnlen (ut->ut_line, UT_LINE_SIZE),
                    #if (HAVE_UTMPX_H ? HAVE_STRUCT_UTMPX_UT_HOST : HAVE_STRUCT_UTMP_UT_HOST)
                    ut->ut_host, strnlen (ut->ut_host, UT_HOST_SIZE),
                    #else
                    "", 0,
                    #endif
                    #if (HAVE_UTMPX_H ? HAVE_STRUCT_UTMPX_UT_PID : HAVE_STRUCT_UTMP_UT_PID)
                    ut->ut_pid,
                    #else
                    0,
                    #endif
                    #if (HAVE_UTMPX_H ? HAVE_STRUCT_UTMPX_UT_TYPE : HAVE_STRUCT_UTMP_UT_TYPE)
                    ut->ut_type,
                    #else
                    0,
                    #endif
                    #if (HAVE_UTMPX_H ? 1 : HAVE_STRUCT_UTMP_UT_TV)
                    (struct timespec) { .tv_sec = ut->ut_tv.tv_sec, .tv_nsec = ut->ut_tv.tv_usec * 1000 },
                    #else
                    (struct timespec) { .tv_sec = ut->ut_time, .tv_nsec = 0 },
                    #endif
                    #if (HAVE_UTMPX_H ? HAVE_STRUCT_UTMPX_UT_SESSION : HAVE_STRUCT_UTMP_UT_SESSION)
                    ut->ut_session,
                    #else
                    0,
                    #endif
                    UT_EXIT_E_TERMINATION (ut), UT_EXIT_E_EXIT (ut)
                   );
    }

  END_UTMP_ENT ();

#  if defined __linux__
  /* On Alpine Linux, UTMP_FILE is not filled.  It is always empty.
     So, fake a BOOT_TIME entry, by getting the time stamp of a file that
     gets touched only during the boot process.  */
  if ((options & (READ_UTMP_USER_PROCESS | READ_UTMP_NO_BOOT_TIME)) == 0
      && strcmp (file, UTMP_FILE) == 0)
    {
      bool have_boot_time = false;
      for (idx_t i = 0; i < a.filled; i++)
        {
          struct gl_utmp *ut = &a.utmp[i];
          if (UT_TYPE_BOOT_TIME (ut))
            {
              have_boot_time = true;
              break;
            }
        }
      if (!have_boot_time)
        {
          const char * const boot_touched_files[] =
            {
              "/var/lib/systemd/random-seed", /* seen on distros with systemd */
              "/var/run/utmp",                /* seen on distros with OpenRC */
              "/var/lib/random-seed"          /* seen on older distros */
            };
          for (idx_t i = 0; i < SIZEOF (boot_touched_files); i++)
            {
              const char *filename = boot_touched_files[i];
              struct stat statbuf;
              if (stat (filename, &statbuf) >= 0)
                {
                  struct timespec boot_time = get_stat_mtime (&statbuf);
                  a = add_utmp (a, options,
                                "reboot", strlen ("reboot"),
                                "", 0,
                                "~", strlen ("~"),
                                "", 0,
                                0, BOOT_TIME, boot_time, 0, 0, 0);
                  break;
                }
            }
        }
    }
#  endif

# else /* old FreeBSD, OpenBSD, HP-UX */

  FILE *f = fopen (file, "re");

  if (! f)
    return -1;

  for (;;)
    {
      struct UTMP_STRUCT_NAME ut;

      if (fread (&ut, sizeof ut, 1, f) == 0)
        break;
      a = add_utmp (a, options,
                    UT_USER (&ut), strnlen (UT_USER (&ut), UT_USER_SIZE),
                    #if (HAVE_UTMPX_H ? HAVE_STRUCT_UTMPX_UT_ID : HAVE_STRUCT_UTMP_UT_ID)
                    ut.ut_id, strnlen (ut.ut_id, UT_ID_SIZE),
                    #else
                    "", 0,
                    #endif
                    ut.ut_line, strnlen (ut.ut_line, UT_LINE_SIZE),
                    #if (HAVE_UTMPX_H ? HAVE_STRUCT_UTMPX_UT_HOST : HAVE_STRUCT_UTMP_UT_HOST)
                    ut.ut_host, strnlen (ut.ut_host, UT_HOST_SIZE),
                    #else
                    "", 0,
                    #endif
                    #if (HAVE_UTMPX_H ? HAVE_STRUCT_UTMPX_UT_PID : HAVE_STRUCT_UTMP_UT_PID)
                    ut.ut_pid,
                    #else
                    0,
                    #endif
                    #if (HAVE_UTMPX_H ? HAVE_STRUCT_UTMPX_UT_TYPE : HAVE_STRUCT_UTMP_UT_TYPE)
                    ut.ut_type,
                    #else
                    0,
                    #endif
                    #if (HAVE_UTMPX_H ? 1 : HAVE_STRUCT_UTMP_UT_TV)
                    (struct timespec) { .tv_sec = ut.ut_tv.tv_sec, .tv_nsec = ut.ut_tv.tv_usec * 1000 },
                    #else
                    (struct timespec) { .tv_sec = ut.ut_time, .tv_nsec = 0 },
                    #endif
                    #if (HAVE_UTMPX_H ? HAVE_STRUCT_UTMPX_UT_SESSION : HAVE_STRUCT_UTMP_UT_SESSION)
                    ut.ut_session,
                    #else
                    0,
                    #endif
                    UT_EXIT_E_TERMINATION (&ut), UT_EXIT_E_EXIT (&ut)
                   );
    }

  int saved_errno = ferror (f) ? errno : 0;
  if (fclose (f) != 0)
    saved_errno = errno;
  if (saved_errno != 0)
    {
      free (a.utmp);
      errno = saved_errno;
      return -1;
    }

# endif

  a = finish_utmp (a);

  *n_entries = a.filled;
  *utmp_buf = a.utmp;

  return 0;
}

# if READUTMP_USE_SYSTEMD
/* Use systemd and Linux /proc and kernel APIs.  */

static struct timespec
get_boot_time_uncached (void)
{
  /* Try to find the boot time in the /var/run/utmp file.  */
  {
    idx_t n_entries = 0;
    STRUCT_UTMP *utmp = NULL;
    read_utmp_from_file (UTMP_FILE, &n_entries, &utmp, READ_UTMP_BOOT_TIME);
    if (n_entries > 0)
      {
        struct timespec result = utmp[0].ut_ts;
        free (utmp);
        return result;
      }
    free (utmp);
  }

  /* The following approaches are only usable as fallbacks, because they are
     all of the form
       boot_time = (time now) - (kernel's ktime_get_boottime[_ts64] ())
     and therefore produce wrong values after the date has been bumped in the
     running system, which happens frequently if the system is running in a
     virtual machine and this VM has been put into "saved" or "sleep" state
     and then resumed.  */

  /* The clock_gettime facility returns the uptime with a resolution of 1 µsec.
     It is available with glibc >= 2.14.  In glibc < 2.17 it required linking
     with librt.  */
#  if __GLIBC__ + (__GLIBC_MINOR__ >= 17) > 2
  struct timespec up;
  if (clock_gettime (CLOCK_BOOTTIME, &up) >= 0)
    {
      struct timespec result;
      /* equivalent to:
      if (clock_gettime (CLOCK_REALTIME, &result) >= 0)
      */
      if (timespec_get (&result, TIME_UTC) >= 0)
        {
          if (result.tv_nsec < up.tv_nsec)
            {
              result.tv_nsec += 1000000000;
              result.tv_sec -= 1;
            }
          result.tv_sec -= up.tv_sec;
          result.tv_nsec -= up.tv_nsec;
          return result;
        }
    }
#  endif

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
              struct timespec result;
              if (0 <= timespec_get (&result, TIME_UTC))
                {
                  time_t uptime_sec = uptime;
                  struct timespec up =
                    {
                      .tv_sec = uptime_sec,
                      .tv_nsec = (uptime - uptime_sec) * 1e9 + 0.5
                    };
                  if (result.tv_nsec < up.tv_nsec)
                    {
                      result.tv_nsec += 1000000000;
                      result.tv_sec -= 1;
                    }
                  result.tv_sec -= up.tv_sec;
                  result.tv_nsec -= up.tv_nsec;
                  return result;
                }
            }
        }
    }

  /* The sysinfo call returns the uptime with a resolution of 1 sec only.  */
  struct sysinfo info;
  if (sysinfo (&info) >= 0)
    {
      struct timespec result;
      if (0 <= timespec_get (&result, TIME_UTC))
        {
          result.tv_sec -= info.uptime;
          return result;
        }
    }

  /* We shouldn't get here.  */
  return (struct timespec) {0};
}

static struct timespec
get_boot_time (void)
{
  static bool volatile cached;
  static struct timespec volatile boot_time;

  if (!cached)
    {
      boot_time = get_boot_time_uncached ();
      cached = true;
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

static int
read_utmp_from_systemd (idx_t *n_entries, STRUCT_UTMP **utmp_buf, int options)
{
  /* Fill entries, simulating what a utmp file would contain.  */
  struct utmp_alloc a = {0};

  /* Synthesize a BOOT_TIME entry.  */
  if (!(options & (READ_UTMP_USER_PROCESS | READ_UTMP_NO_BOOT_TIME)))
    a = add_utmp (a, options,
                  "reboot", strlen ("reboot"),
                  "", 0,
                  "~", strlen ("~"),
                  "", 0,
                  0, BOOT_TIME, get_boot_time (), 0, 0, 0);

  /* Synthesize USER_PROCESS entries.  */
  if (!(options & READ_UTMP_BOOT_TIME))
    {
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
              struct timespec start_ts;
              start_ts.tv_sec = start_usec / 1000000;
              start_ts.tv_nsec = start_usec % 1000000 * 1000;

              char *seat;
              if (sd_session_get_seat (session, &seat) < 0)
                seat = NULL;

              char missing[] = "";

              char *type = NULL;
              char *tty;
              if (sd_session_get_tty (session, &tty) < 0)
                {
                  tty = NULL;
                  /* Try harder to get a sensible value for the tty.  */
                  if (sd_session_get_type (session, &type) < 0)
                    type = missing;
                  if (strcmp (type, "tty") == 0)
                    {
                      char *service;
                      if (sd_session_get_service (session, &service) < 0)
                        service = NULL;

                      uid_t uid;
                      char *pty = (sd_session_get_uid (session, &uid) < 0 ? NULL
                                   : guess_pty_name (uid, start_ts));

                      if (service != NULL && pty != NULL)
                        {
                          tty = xmalloc (strlen (service) + 1 + strlen (pty) + 1);
                          stpcpy (stpcpy (stpcpy (tty, service), " "), pty);
                          free (pty);
                          free (service);
                        }
                      else if (service != NULL)
                        tty = service;
                      else if (pty != NULL)
                        tty = pty;
                    }
                }

              /* Create up to two USER_PROCESS entries: one for the seat,
                 one for the tty.  */
              if (seat != NULL || tty != NULL)
                {
                  char *user;
                  if (sd_session_get_username (session, &user) < 0)
                    user = missing;

                  pid_t leader_pid;
                  if (sd_session_get_leader (session, &leader_pid) < 0)
                    leader_pid = 0;

                  char *host;
                  char *remote_host;
                  if (sd_session_get_remote_host (session, &remote_host) < 0)
                    {
                      host = missing;
                      /* For backward compatibility, put the X11 display into the
                         host field.  */
                      if (!type && sd_session_get_type (session, &type) < 0)
                        type = missing;
                      if (strcmp (type, "x11") == 0)
                        {
                          char *display;
                          if (sd_session_get_display (session, &display) < 0)
                            display = NULL;
                          host = display;
                        }
                    }
                  else
                    {
                      char *remote_user;
                      if (sd_session_get_remote_user (session, &remote_user) < 0)
                        host = remote_host;
                      else
                        {
                          host = xmalloc (strlen (remote_user) + 1
                                          + strlen (remote_host) + 1);
                          stpcpy (stpcpy (stpcpy (host, remote_user), "@"),
                                  remote_host);
                          free (remote_user);
                          free (remote_host);
                        }
                    }

                  if (seat != NULL)
                    a = add_utmp (a, options,
                                  user, strlen (user),
                                  session, strlen (session),
                                  seat, strlen (seat),
                                  host, strlen (host),
                                  leader_pid /* the best we have */,
                                  USER_PROCESS, start_ts, leader_pid, 0, 0);
                  if (tty != NULL)
                    a = add_utmp (a, options,
                                  user, strlen (user),
                                  session, strlen (session),
                                  tty, strlen (tty),
                                  host, strlen (host),
                                  leader_pid /* the best we have */,
                                  USER_PROCESS, start_ts, leader_pid, 0, 0);

                  if (host != missing)
                    free (host);
                  if (user != missing)
                    free (user);
                }

              if (type != missing)
                free (type);
              free (tty);
              free (seat);
              free (session);
            }
          free (sessions);
        }
    }

  a = finish_utmp (a);

  *n_entries = a.filled;
  *utmp_buf = a.utmp;

  return 0;
}

# endif

int
read_utmp (char const *file, idx_t *n_entries, STRUCT_UTMP **utmp_buf,
           int options)
{
# if READUTMP_USE_SYSTEMD
  if (strcmp (file, UTMP_FILE) == 0)
    /* Imitate reading UTMP_FILE, using systemd and Linux APIs.  */
    return read_utmp_from_systemd (n_entries, utmp_buf, options);
# endif

  return read_utmp_from_file (file, n_entries, utmp_buf, options);
}

#else /* dummy fallback */

int
read_utmp (char const *file, idx_t *n_entries, STRUCT_UTMP **utmp_buf,
           int options)
{
  errno = ENOSYS;
  return -1;
}

#endif

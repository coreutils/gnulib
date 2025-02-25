/* Return the version-control based modification time of a file.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

#include <config.h>

/* Specification.  */
#include "vc-mtime.h"

#include <stdlib.h>
#include <unistd.h>

#include <error.h>
#include "spawn-pipe.h"
#include "wait-process.h"
#include "execute.h"
#include "safe-read.h"
#include "xstrtol.h"
#include "stat-time.h"
#include "gettext.h"

#define _(msgid) dgettext ("gnulib", msgid)


/* Determines whether the specified file is under version control.  */
static bool
git_vc_controlled (const char *filename)
{
  /* Run "git ls-files FILENAME" and return true if the exit code is 0
     and the output is non-empty.  */
  const char *argv[4];
  pid_t child;
  int fd[1];

  argv[0] = "git";
  argv[1] = "ls-files";
  argv[2] = filename;
  argv[3] = NULL;
  child = create_pipe_in ("git", "git", argv, NULL, NULL,
                          DEV_NULL, true, true, false, fd);
  if (child == -1)
    return false;

  /* Read the subprocess output, and test whether it is non-empty.  */
  size_t count = 0;
  char c;

  while (safe_read (fd[0], &c, 1) > 0)
    count++;

  close (fd[0]);

  /* Remove zombie process from process list, and retrieve exit status.  */
  int exitstatus =
    wait_subprocess (child, "git", false, true, true, false, NULL);
  return (exitstatus == 0 && count > 0);
}

/* Determines whether the specified file is unmodified, compared to the
   last version in version control.  */
static bool
git_unmodified (const char *filename)
{
  /* Run "git diff --quiet -- HEAD FILENAME"
     (or "git diff --quiet HEAD FILENAME")
     and return true if the exit code is 0.
     The '--' option is for the case that the specified file was removed.  */
  const char *argv[7];
  int exitstatus;

  argv[0] = "git";
  argv[1] = "diff";
  argv[2] = "--quiet";
  argv[3] = "--";
  argv[4] = "HEAD";
  argv[5] = filename;
  argv[6] = NULL;
  exitstatus = execute ("git", "git", argv, NULL, NULL,
                        false, false, true, true,
                        true, false, NULL);
  return (exitstatus == 0);
}

/* Stores in *MTIME the time of last modification in version control of the
   specified file, and returns 0.
   Upon failure, it returns -1.  */
static int
git_mtime (struct timespec *mtime, const char *filename)
{
  /* Run "git log -1 --format=%ct -- FILENAME".  It prints the time of last
     modification (the 'CommitDate', not the 'AuthorDate' which merely
     represents the time at which the author locally committed the first version
     of the change), as the number of seconds since the Epoch.
     The '--' option is for the case that the specified file was removed.  */
  const char *argv[7];
  pid_t child;
  int fd[1];

  argv[0] = "git";
  argv[1] = "log";
  argv[2] = "-1";
  argv[3] = "--format=%ct";
  argv[4] = "--";
  argv[5] = filename;
  argv[6] = NULL;
  child = create_pipe_in ("git", "git", argv, NULL, NULL,
                          DEV_NULL, true, true, false, fd);
  if (child == -1)
    return -1;

  /* Retrieve its result.  */
  FILE *fp;
  char *line;
  size_t linesize;
  size_t linelen;

  fp = fdopen (fd[0], "r");
  if (fp == NULL)
    error (EXIT_FAILURE, errno, _("fdopen() failed"));

  line = NULL; linesize = 0;
  linelen = getline (&line, &linesize, fp);
  if (linelen == (size_t)(-1))
    {
      error (0, 0, _("%s subprocess I/O error"), "git");
      fclose (fp);
      wait_subprocess (child, "git", true, false, true, false, NULL);
    }
  else
    {
      int exitstatus;

      if (linelen > 0 && line[linelen - 1] == '\n')
        line[linelen - 1] = '\0';

      fclose (fp);

      /* Remove zombie process from process list, and retrieve exit status.  */
      exitstatus =
        wait_subprocess (child, "git", true, false, true, false, NULL);
      if (exitstatus == 0)
        {
          char *endptr;
          unsigned long git_log_time;
          if (xstrtoul (line, &endptr, 10, &git_log_time, NULL) == LONGINT_OK
              && endptr == line + strlen (line))
            {
              mtime->tv_sec = git_log_time;
              mtime->tv_nsec = 0;
              free (line);
              return 0;
            }
        }
    }
  free (line);
  return -1;
}

int
vc_mtime (struct timespec *mtime, const char *filename)
{
  static bool git_tested;
  static bool git_present;

  if (!git_tested)
    {
      /* Test for presence of git:
         "git --version >/dev/null 2>/dev/null"  */
      const char *argv[3];
      int exitstatus;

      argv[0] = "git";
      argv[1] = "--version";
      argv[2] = NULL;
      exitstatus = execute ("git", "git", argv, NULL, NULL,
                            false, false, true, true,
                            true, false, NULL);
      git_present = (exitstatus == 0);
      git_tested = true;
    }

  if (git_present
      && git_vc_controlled (filename)
      && git_unmodified (filename))
    {
      if (git_mtime (mtime, filename) == 0)
        return 0;
    }
  struct stat statbuf;
  if (stat (filename, &statbuf) == 0)
    {
      *mtime = get_stat_mtime (&statbuf);
      return 0;
    }
  return -1;
}

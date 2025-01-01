/* Test of the wait queue handling of read-write locks.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

#include <config.h>

/* Specification.  */
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "macros.h"

/* This program shows the wait queue handling of POSIX read-write locks.

   The program has a single pthread_rwlock_t.

   The function do_test takes as argument a string consisting of Rs and Ws,
   for example RWRRW. It launches a corresponding number of threads:
   For each R, a thread that attempts to lock the lock for reading;
   for each W, a thread that attempts to lock the lock for writing.
   The threads do this lock attempt one after the other.
   The first thread keeps the lock until after all threads have issued their
   requests, then releases it. The interesting part is, then, in which order
   these lock attempts are granted.

   The function find_wait_queue_handling performs a series of do_test
   invocations and produces a textual description of the wait queue handling.

   The main() function invokes find_wait_queue_handling for all available
   flavours of read-write locks.  */

/* Some platforms need a longer STEP_INTERVAL, otherwise some of the assertions
   RRR, RRRR, RRRRR fail.
   Note: The probability of failing these assertions is higher when the machine
   is under high load.  It can be worked around by increasing the STEP_INTERVAL.
   However, increasing the STEP_INTERVAL means to increase the total duration
   of this test:
     STEP_INTERVAL  Duration (on glibc/Linux)
         10 ms           29 sec
         20 ms           57 sec
         50 ms          2.4 min
        100 ms          4.8 min
        200 ms          9.6 min
   There is no way to have this test be reasonably fast and 100% reliable at the
   same time.  Therefore the compromise we have chosen is
     - to pick STEP_INTERVAL so that the test succeeds on developer machines
       with little load and on continuous integration machines,
     - to exclude the test from packaging, unless the gnulib-tool option
       '--with-longrunning-tests' is specified.  */
#if (defined __APPLE__ && defined __MACH__)
/* macOS */
# define STEP_INTERVAL 200000000 /* nanoseconds */
#elif defined _WIN32 || defined __CYGWIN__
/* Windows */
# define STEP_INTERVAL 50000000 /* nanoseconds */
#elif (defined __FreeBSD__ || defined __DragonFly__) || (defined __linux__ && defined __hppa)
/* FreeBSD, Linux/hppa */
# define STEP_INTERVAL 20000000 /* nanoseconds */
#else
# define STEP_INTERVAL 10000000 /* nanoseconds */
#endif

static pthread_rwlock_t lock;

/* A lock that serializes the sprintf() calls.  */
static pthread_rwlock_t sprintf_lock;

struct locals
{
  const char *name;
  unsigned int wait_before;
  unsigned int wait_after;
  char *result;
};

static void *
reader_func (void *arg)
{
  struct locals *l = arg;
  int err;

  if (l->wait_before > 0)
    {
      struct timespec duration;
      duration.tv_sec  = l->wait_before / 1000000000;
      duration.tv_nsec = l->wait_before % 1000000000;
      nanosleep (&duration, NULL);
    }
  err = pthread_rwlock_rdlock (&lock);
  if (err)
    {
      fprintf (stderr, "pthread_rwlock_rdlock failed, error = %d\n", err);
      abort ();
    }
  ASSERT (pthread_rwlock_wrlock (&sprintf_lock) == 0);
  sprintf (l->result + strlen (l->result), " %s", l->name);
  ASSERT (pthread_rwlock_unlock (&sprintf_lock) == 0);
  if (l->wait_after > 0)
    {
      struct timespec duration;
      duration.tv_sec  = l->wait_after / 1000000000;
      duration.tv_nsec = l->wait_after % 1000000000;
      nanosleep (&duration, NULL);
    }
  err = pthread_rwlock_unlock (&lock);
  if (err)
    {
      fprintf (stderr, "pthread_rwlock_unlock failed, error = %d\n", err);
      abort ();
    }

  return NULL;
}

static void *
writer_func (void *arg)
{
  struct locals *l = arg;
  int err;

  if (l->wait_before > 0)
    {
      struct timespec duration;
      duration.tv_sec  = l->wait_before / 1000000000;
      duration.tv_nsec = l->wait_before % 1000000000;
      nanosleep (&duration, NULL);
    }
  err = pthread_rwlock_wrlock (&lock);
  if (err)
    {
      fprintf (stderr, "pthread_rwlock_rdlock failed, error = %d\n", err);
      abort ();
    }
  ASSERT (pthread_rwlock_wrlock (&sprintf_lock) == 0);
  sprintf (l->result + strlen (l->result), " %s", l->name);
  ASSERT (pthread_rwlock_unlock (&sprintf_lock) == 0);
  if (l->wait_after > 0)
    {
      struct timespec duration;
      duration.tv_sec  = l->wait_after / 1000000000;
      duration.tv_nsec = l->wait_after % 1000000000;
      nanosleep (&duration, NULL);
    }
  err = pthread_rwlock_unlock (&lock);
  if (err)
    {
      fprintf (stderr, "pthread_rwlock_unlock failed, error = %d\n", err);
      abort ();
    }

  return NULL;
}

static const char *
do_test (const char *rw_string)
{
  size_t n = strlen (rw_string);
  int err;
  char resultbuf[100];

  char **names = (char **) malloc (n * sizeof (char *));
  for (size_t i = 0; i < n; i++)
    {
      char name[12];
      sprintf (name, "%c%u", rw_string[i], (unsigned int) (i+1));
      names[i] = strdup (name);
    }

  resultbuf[0] = '\0';

  /* Create the threads.  */
  struct locals *locals = (struct locals *) malloc (n * sizeof (struct locals));
  pthread_t *threads = (pthread_t *) malloc (n * sizeof (pthread_t));
  for (size_t i = 0; i < n; i++)
    {
      locals[i].name = names[i];
      locals[i].wait_before = i * STEP_INTERVAL;
      locals[i].wait_after  = (i == 0 ? n * STEP_INTERVAL : 0);
      locals[i].result = resultbuf;
      err = pthread_create (&threads[i], NULL,
                            rw_string[i] == 'R' ? reader_func :
                            rw_string[i] == 'W' ? writer_func :
                            (abort (), (void * (*) (void *)) NULL),
                            &locals[i]);
      if (err)
        {
          fprintf (stderr, "pthread_create failed to create thread %u, error = %d\n",
                   (unsigned int) (i+1), err);
          abort ();
        }
    }

  /* Wait until the threads are done.  */
  for (size_t i = 0; i < n; i++)
    {
      void *retcode;
      err = pthread_join (threads[i], &retcode);
      if (err)
        {
          fprintf (stderr, "pthread_join failed to wait for thread %u, error = %d\n",
                   (unsigned int) (i+1), err);
          abort ();
        }
    }

  /* Clean up.  */
  free (threads);
  free (locals);
  for (size_t i = 0; i < n; i++)
    free (names[i]);
  free (names);

  return strdup (resultbuf);
}

static bool
startswith (const char *str, const char *prefix)
{
  return strncmp (str, prefix, strlen (prefix)) == 0;
}

static int
find_wait_queue_handling (void)
{
  /* These are mandatory behaviour of rwlocks:
     R1 R2 ... R(i-1) ... => R1 R2 ... R(i-1) ...
     because the R2 ... R(i-1) succeed while R1 is still being held.  */
  ASSERT (startswith (do_test ("R"), " R1"));
  ASSERT (startswith (do_test ("RR"), " R1 R2"));
  ASSERT (startswith (do_test ("RRR"), " R1 R2 R3"));
  ASSERT (startswith (do_test ("RRRR"), " R1 R2 R3 R4"));
  ASSERT (startswith (do_test ("RRRRR"), " R1 R2 R3 R4 R5"));

  bool final_r_prefers_first = true;
  bool final_r_prefers_readers = true;
  bool final_r_prefers_readers_first_reader = true;
  bool final_r_prefers_readers_last_reader = true;
  bool final_r_prefers_readers_first_writer = true;
  bool final_r_prefers_readers_last_writer = true;
  bool final_r_prefers_writers = true;
  bool final_r_prefers_writers_first_writer = true;
  bool final_r_prefers_writers_last_writer = true;
  bool final_w_prefers_first_a = true;
  bool final_w_prefers_first_b = true;
  bool final_w_prefers_first = true;
  bool final_w_prefers_readers = true;
  bool final_w_prefers_readers_first_reader = true;
  bool final_w_prefers_readers_last_reader = true;
  bool final_w_prefers_readers_first_writer = true;
  bool final_w_prefers_readers_last_writer = true;
  bool final_w_prefers_writers = true;
  bool final_w_prefers_writers_first_writer = true;
  bool final_w_prefers_writers_last_writer = true;
  bool final_w_prefers_writers_some_reader = true;
  bool final_w_prefers_writers_first_reader = true;
  bool final_w_prefers_writers_last_reader = true;

  /* Perform the test a few times, so that in case of a non-deterministic
     behaviour that happens to look like deterministic in one round, we get
     a higher probability of finding that it is non-deterministic.  */
  for (int repeat = 3; repeat > 0; repeat--)
    {
      bool r_prefers_first = false;
      bool r_prefers_readers = false;
      bool r_prefers_readers_first_reader = false;
      bool r_prefers_readers_last_reader = false;
      bool r_prefers_readers_first_writer = false;
      bool r_prefers_readers_last_writer = false;
      bool r_prefers_writers = false;
      bool r_prefers_writers_first_writer = false;
      bool r_prefers_writers_last_writer = false;
      bool w_prefers_first_a = false;
      bool w_prefers_first_b = false;
      bool w_prefers_first = false;
      bool w_prefers_readers = false;
      bool w_prefers_readers_first_reader = false;
      bool w_prefers_readers_last_reader = false;
      bool w_prefers_readers_first_writer = false;
      bool w_prefers_readers_last_writer = false;
      bool w_prefers_writers = false;
      bool w_prefers_writers_first_writer = false;
      bool w_prefers_writers_last_writer = false;
      bool w_prefers_writers_some_reader = false;
      bool w_prefers_writers_first_reader = false;
      bool w_prefers_writers_last_reader = false;

      {
        const char * RWR = do_test ("RWR");
        const char * RWW = do_test ("RWW");
        const char * RWRR = do_test ("RWRR");
        const char * RWRW = do_test ("RWRW");
        const char * RWWR = do_test ("RWWR");
        const char * RWWW = do_test ("RWWW");
        const char * RWRRR = do_test ("RWRRR");
        const char * RWRRW = do_test ("RWRRW");
        const char * RWRWR = do_test ("RWRWR");
        const char * RWRWW = do_test ("RWRWW");
        const char * RWWRR = do_test ("RWWRR");
        const char * RWWRW = do_test ("RWWRW");
        const char * RWWWR = do_test ("RWWWR");
        const char * RWWWW = do_test ("RWWWW");

        if (   startswith (RWR, " R1 W2")
            && startswith (RWW, " R1 W2")
            && startswith (RWRR, " R1 W2")
            && startswith (RWRW, " R1 W2")
            && startswith (RWWR, " R1 W2")
            && startswith (RWWW, " R1 W2")
            && startswith (RWRRR, " R1 W2")
            && startswith (RWRRW, " R1 W2")
            && startswith (RWRWR, " R1 W2")
            && startswith (RWRWW, " R1 W2")
            && startswith (RWWRR, " R1 W2")
            && startswith (RWWRW, " R1 W2")
            && startswith (RWWWR, " R1 W2")
            && startswith (RWWWW, " R1 W2"))
          r_prefers_first = true;
        if (   startswith (RWR, " R1 R")
            && startswith (RWRR, " R1 R")
            && startswith (RWRW, " R1 R")
            && startswith (RWWR, " R1 R")
            && startswith (RWRRR, " R1 R")
            && startswith (RWRRW, " R1 R")
            && startswith (RWRWR, " R1 R")
            && startswith (RWRWW, " R1 R")
            && startswith (RWWRR, " R1 R")
            && startswith (RWWRW, " R1 R")
            && startswith (RWWWR, " R1 R"))
          {
            r_prefers_readers = true;
            if (   startswith (RWR, " R1 R3")
                && startswith (RWRR, " R1 R3")
                && startswith (RWRW, " R1 R3")
                && startswith (RWWR, " R1 R4")
                && startswith (RWRRR, " R1 R3")
                && startswith (RWRRW, " R1 R3")
                && startswith (RWRWR, " R1 R3")
                && startswith (RWRWW, " R1 R3")
                && startswith (RWWRR, " R1 R4")
                && startswith (RWWRW, " R1 R4")
                && startswith (RWWWR, " R1 R5"))
              r_prefers_readers_first_reader = true;
            if (   startswith (RWR, " R1 R3")
                && startswith (RWRR, " R1 R4")
                && startswith (RWRW, " R1 R3")
                && startswith (RWWR, " R1 R4")
                && startswith (RWRRR, " R1 R5")
                && startswith (RWRRW, " R1 R4")
                && startswith (RWRWR, " R1 R5")
                && startswith (RWRWW, " R1 R3")
                && startswith (RWWRR, " R1 R5")
                && startswith (RWWRW, " R1 R4")
                && startswith (RWWWR, " R1 R5"))
              r_prefers_readers_last_reader = true;
            if (   startswith (RWW, " R1 W2")
                && startswith (RWWW, " R1 W2")
                && startswith (RWWWW, " R1 W2"))
              r_prefers_readers_first_writer = true;
            if (   startswith (RWW, " R1 W3")
                && startswith (RWWW, " R1 W4")
                && startswith (RWWWW, " R1 W5"))
              r_prefers_readers_last_writer = true;
            if (!(r_prefers_readers_first_writer || r_prefers_readers_last_writer))
              {
                printf ("    RWW =>%s\n", RWW);
                printf ("    RWWW =>%s\n", RWWW);
                printf ("    RWWWW =>%s\n", RWWWW);
              }
          }
        if (   startswith (RWR, " R1 W")
            && startswith (RWW, " R1 W")
            && startswith (RWRR, " R1 W")
            && startswith (RWRW, " R1 W")
            && startswith (RWWR, " R1 W")
            && startswith (RWWW, " R1 W")
            && startswith (RWRRR, " R1 W")
            && startswith (RWRRW, " R1 W")
            && startswith (RWRWR, " R1 W")
            && startswith (RWRWW, " R1 W")
            && startswith (RWWRR, " R1 W")
            && startswith (RWWRW, " R1 W")
            && startswith (RWWWR, " R1 W")
            && startswith (RWWWW, " R1 W"))
          {
            r_prefers_writers = true;
            if (   startswith (RWR, " R1 W2")
                && startswith (RWW, " R1 W2")
                && startswith (RWRR, " R1 W2")
                && startswith (RWRW, " R1 W2")
                && startswith (RWWR, " R1 W2")
                && startswith (RWWW, " R1 W2")
                && startswith (RWRRR, " R1 W2")
                && startswith (RWRRW, " R1 W2")
                && startswith (RWRWR, " R1 W2")
                && startswith (RWRWW, " R1 W2")
                && startswith (RWWRR, " R1 W2")
                && startswith (RWWRW, " R1 W2")
                && startswith (RWWWR, " R1 W2")
                && startswith (RWWWW, " R1 W2"))
              r_prefers_writers_first_writer = true;
            if (   startswith (RWR, " R1 W2")
                && startswith (RWW, " R1 W3")
                && startswith (RWRR, " R1 W2")
                && startswith (RWRW, " R1 W4")
                && startswith (RWWR, " R1 W3")
                && startswith (RWWW, " R1 W4")
                && startswith (RWRRR, " R1 W2")
                && startswith (RWRRW, " R1 W5")
                && startswith (RWRWR, " R1 W4")
                && startswith (RWRWW, " R1 W5")
                && startswith (RWWRR, " R1 W3")
                && startswith (RWWRW, " R1 W5")
                && startswith (RWWWR, " R1 W4")
                && startswith (RWWWW, " R1 W5"))
              r_prefers_writers_last_writer = true;
          }
        if (!(r_prefers_first || r_prefers_readers || r_prefers_writers))
          {
            printf ("    RWR =>%s\n", RWR);
            printf ("    RWW =>%s\n", RWW);
            printf ("    RWRR =>%s\n", RWRR);
            printf ("    RWRW =>%s\n", RWRW);
            printf ("    RWWR =>%s\n", RWWR);
            printf ("    RWWW =>%s\n", RWWW);
            printf ("    RWRRR =>%s\n", RWRRR);
            printf ("    RWRRW =>%s\n", RWRRW);
            printf ("    RWRWR =>%s\n", RWRWR);
            printf ("    RWRWW =>%s\n", RWRWW);
            printf ("    RWWRR =>%s\n", RWWRR);
            printf ("    RWWRW =>%s\n", RWWRW);
            printf ("    RWWWR =>%s\n", RWWWR);
            printf ("    RWWWW =>%s\n", RWWWW);
          }
      }

      {
        const char * WRR = do_test ("WRR");
        const char * WRW = do_test ("WRW");
        const char * WWR = do_test ("WWR");
        const char * WWW = do_test ("WWW");
        const char * WRRR = do_test ("WRRR");
        const char * WRRW = do_test ("WRRW");
        const char * WRWR = do_test ("WRWR");
        const char * WRWW = do_test ("WRWW");
        const char * WWRR = do_test ("WWRR");
        const char * WWRW = do_test ("WWRW");
        const char * WWWR = do_test ("WWWR");
        const char * WWWW = do_test ("WWWW");
        const char * WRRRR = do_test ("WRRRR");
        const char * WRRRW = do_test ("WRRRW");
        const char * WRRWR = do_test ("WRRWR");
        const char * WRRWW = do_test ("WRRWW");
        const char * WRWRR = do_test ("WRWRR");
        const char * WRWRW = do_test ("WRWRW");
        const char * WRWWR = do_test ("WRWWR");
        const char * WRWWW = do_test ("WRWWW");
        const char * WWRRR = do_test ("WWRRR");
        const char * WWRRW = do_test ("WWRRW");
        const char * WWRWR = do_test ("WWRWR");
        const char * WWRWW = do_test ("WWRWW");
        const char * WWWRR = do_test ("WWWRR");
        const char * WWWRW = do_test ("WWWRW");
        const char * WWWWR = do_test ("WWWWR");
        const char * WWWWW = do_test ("WWWWW");

        if (   startswith (WRR, " W1 R")
            && startswith (WRW, " W1 R")
            && startswith (WWR, " W1 W")
            && startswith (WWW, " W1 W")
            && startswith (WRRR, " W1 R")
            && startswith (WRRW, " W1 R")
            && startswith (WRWR, " W1 R")
            && startswith (WRWW, " W1 R")
            && startswith (WWRR, " W1 W")
            && startswith (WWRW, " W1 W")
            && startswith (WWWR, " W1 W")
            && startswith (WWWW, " W1 W")
            && startswith (WRRRR, " W1 R")
            && startswith (WRRRW, " W1 R")
            && startswith (WRRWR, " W1 R")
            && startswith (WRRWW, " W1 R")
            && startswith (WRWRR, " W1 R")
            && startswith (WRWRW, " W1 R")
            && startswith (WRWWR, " W1 R")
            && startswith (WRWWW, " W1 R")
            && startswith (WWRRR, " W1 W")
            && startswith (WWRRW, " W1 W")
            && startswith (WWRWR, " W1 W")
            && startswith (WWRWW, " W1 W")
            && startswith (WWWRR, " W1 W")
            && startswith (WWWRW, " W1 W")
            && startswith (WWWWR, " W1 W")
            && startswith (WWWWW, " W1 W"))
          {
            w_prefers_first_a = true;
            if (   startswith (WRR, " W1 R")
                && startswith (WRW, " W1 R")
                && startswith (WWR, " W1 W2")
                && startswith (WWW, " W1 W2")
                && startswith (WRRR, " W1 R")
                && startswith (WRRW, " W1 R")
                && startswith (WRWR, " W1 R")
                && startswith (WRWW, " W1 R")
                && startswith (WWRR, " W1 W2")
                && startswith (WWRW, " W1 W2")
                && startswith (WWWR, " W1 W2")
                && startswith (WWWW, " W1 W2")
                && startswith (WRRRR, " W1 R")
                && startswith (WRRRW, " W1 R")
                && startswith (WRRWR, " W1 R")
                && startswith (WRRWW, " W1 R")
                && startswith (WRWRR, " W1 R")
                && startswith (WRWRW, " W1 R")
                && startswith (WRWWR, " W1 R")
                && startswith (WRWWW, " W1 R")
                && startswith (WWRRR, " W1 W2")
                && startswith (WWRRW, " W1 W2")
                && startswith (WWRWR, " W1 W2")
                && startswith (WWRWW, " W1 W2")
                && startswith (WWWRR, " W1 W2")
                && startswith (WWWRW, " W1 W2")
                && startswith (WWWWR, " W1 W2")
                && startswith (WWWWW, " W1 W2"))
              {
                w_prefers_first_b = true;
                if (   startswith (WRR, " W1 R2")
                    && startswith (WRW, " W1 R2")
                    && startswith (WWR, " W1 W2")
                    && startswith (WWW, " W1 W2")
                    && startswith (WRRR, " W1 R2")
                    && startswith (WRRW, " W1 R2")
                    && startswith (WRWR, " W1 R2")
                    && startswith (WRWW, " W1 R2")
                    && startswith (WWRR, " W1 W2")
                    && startswith (WWRW, " W1 W2")
                    && startswith (WWWR, " W1 W2")
                    && startswith (WWWW, " W1 W2")
                    && startswith (WRRRR, " W1 R2")
                    && startswith (WRRRW, " W1 R2")
                    && startswith (WRRWR, " W1 R2")
                    && startswith (WRRWW, " W1 R2")
                    && startswith (WRWRR, " W1 R2")
                    && startswith (WRWRW, " W1 R2")
                    && startswith (WRWWR, " W1 R2")
                    && startswith (WRWWW, " W1 R2")
                    && startswith (WWRRR, " W1 W2")
                    && startswith (WWRRW, " W1 W2")
                    && startswith (WWRWR, " W1 W2")
                    && startswith (WWRWW, " W1 W2")
                    && startswith (WWWRR, " W1 W2")
                    && startswith (WWWRW, " W1 W2")
                    && startswith (WWWWR, " W1 W2")
                    && startswith (WWWWW, " W1 W2"))
                  w_prefers_first = true;
              }
          }
        if (   startswith (WRR, " W1 R")
            && startswith (WRW, " W1 R")
            && startswith (WWR, " W1 R")
            && startswith (WRRR, " W1 R")
            && startswith (WRRW, " W1 R")
            && startswith (WRWR, " W1 R")
            && startswith (WRWW, " W1 R")
            && startswith (WWRR, " W1 R")
            && startswith (WWRW, " W1 R")
            && startswith (WWWR, " W1 R")
            && startswith (WRRRR, " W1 R")
            && startswith (WRRRW, " W1 R")
            && startswith (WRRWR, " W1 R")
            && startswith (WRRWW, " W1 R")
            && startswith (WRWRR, " W1 R")
            && startswith (WRWRW, " W1 R")
            && startswith (WRWWR, " W1 R")
            && startswith (WRWWW, " W1 R")
            && startswith (WWRRR, " W1 R")
            && startswith (WWRRW, " W1 R")
            && startswith (WWRWR, " W1 R")
            && startswith (WWRWW, " W1 R")
            && startswith (WWWRR, " W1 R")
            && startswith (WWWRW, " W1 R")
            && startswith (WWWWR, " W1 R"))
          {
            w_prefers_readers = true;
            if (   startswith (WRR, " W1 R2")
                && startswith (WRW, " W1 R2")
                && startswith (WWR, " W1 R3")
                && startswith (WRRR, " W1 R2")
                && startswith (WRRW, " W1 R2")
                && startswith (WRWR, " W1 R2")
                && startswith (WRWW, " W1 R2")
                && startswith (WWRR, " W1 R3")
                && startswith (WWRW, " W1 R3")
                && startswith (WWWR, " W1 R4")
                && startswith (WRRRR, " W1 R2")
                && startswith (WRRRW, " W1 R2")
                && startswith (WRRWR, " W1 R2")
                && startswith (WRRWW, " W1 R2")
                && startswith (WRWRR, " W1 R2")
                && startswith (WRWRW, " W1 R2")
                && startswith (WRWWR, " W1 R2")
                && startswith (WRWWW, " W1 R2")
                && startswith (WWRRR, " W1 R3")
                && startswith (WWRRW, " W1 R3")
                && startswith (WWRWR, " W1 R3")
                && startswith (WWRWW, " W1 R3")
                && startswith (WWWRR, " W1 R4")
                && startswith (WWWRW, " W1 R4")
                && startswith (WWWWR, " W1 R5"))
              w_prefers_readers_first_reader = true;
            if (   startswith (WRR, " W1 R3")
                && startswith (WRW, " W1 R2")
                && startswith (WWR, " W1 R3")
                && startswith (WRRR, " W1 R4")
                && startswith (WRRW, " W1 R3")
                && startswith (WRWR, " W1 R4")
                && startswith (WRWW, " W1 R2")
                && startswith (WWRR, " W1 R4")
                && startswith (WWRW, " W1 R3")
                && startswith (WWWR, " W1 R4")
                && startswith (WRRRR, " W1 R5")
                && startswith (WRRRW, " W1 R4")
                && startswith (WRRWR, " W1 R5")
                && startswith (WRRWW, " W1 R3")
                && startswith (WRWRR, " W1 R5")
                && startswith (WRWRW, " W1 R4")
                && startswith (WRWWR, " W1 R5")
                && startswith (WRWWW, " W1 R2")
                && startswith (WWRRR, " W1 R5")
                && startswith (WWRRW, " W1 R4")
                && startswith (WWRWR, " W1 R5")
                && startswith (WWRWW, " W1 R3")
                && startswith (WWWRR, " W1 R5")
                && startswith (WWWRW, " W1 R4")
                && startswith (WWWWR, " W1 R5"))
              w_prefers_readers_last_reader = true;
            if (   startswith (WWW, " W1 W2 W3")
                && startswith (WWWW, " W1 W2 W3 W4")
                && startswith (WWWWW, " W1 W2 W3 W4 W5"))
              w_prefers_readers_first_writer = true;
            if (   startswith (WWW, " W1 W3")
                && startswith (WWWW, " W1 W4")
                && startswith (WWWWW, " W1 W5"))
              w_prefers_readers_last_writer = true;
            if (!(w_prefers_readers_first_writer || w_prefers_readers_last_writer))
              {
                printf ("    WWW =>%s\n", WWW);
                printf ("    WWWW =>%s\n", WWWW);
                printf ("    WWWWW =>%s\n", WWWWW);
              }
          }
        if (   startswith (WRW, " W1 W")
            && startswith (WWR, " W1 W")
            && startswith (WWW, " W1 W")
            && startswith (WRRW, " W1 W")
            && startswith (WRWR, " W1 W")
            && startswith (WRWW, " W1 W")
            && startswith (WWRR, " W1 W")
            && startswith (WWRW, " W1 W")
            && startswith (WWWR, " W1 W")
            && startswith (WWWW, " W1 W")
            && startswith (WRRRW, " W1 W")
            && startswith (WRRWR, " W1 W")
            && startswith (WRRWW, " W1 W")
            && startswith (WRWRR, " W1 W")
            && startswith (WRWRW, " W1 W")
            && startswith (WRWWR, " W1 W")
            && startswith (WRWWW, " W1 W")
            && startswith (WWRRR, " W1 W")
            && startswith (WWRRW, " W1 W")
            && startswith (WWRWR, " W1 W")
            && startswith (WWRWW, " W1 W")
            && startswith (WWWRR, " W1 W")
            && startswith (WWWRW, " W1 W")
            && startswith (WWWWR, " W1 W")
            && startswith (WWWWW, " W1 W"))
          {
            w_prefers_writers = true;
            if (   startswith (WRW, " W1 W3")
                && startswith (WWR, " W1 W2")
                && startswith (WWW, " W1 W2")
                && startswith (WRRW, " W1 W4")
                && startswith (WRWR, " W1 W3")
                && startswith (WRWW, " W1 W3")
                && startswith (WWRR, " W1 W2")
                && startswith (WWRW, " W1 W2")
                && startswith (WWWR, " W1 W2")
                && startswith (WWWW, " W1 W2")
                && startswith (WRRRW, " W1 W5")
                && startswith (WRRWR, " W1 W4")
                && startswith (WRRWW, " W1 W4")
                && startswith (WRWRR, " W1 W3")
                && startswith (WRWRW, " W1 W3")
                && startswith (WRWWR, " W1 W3")
                && startswith (WRWWW, " W1 W3")
                && startswith (WWRRR, " W1 W2")
                && startswith (WWRRW, " W1 W2")
                && startswith (WWRWR, " W1 W2")
                && startswith (WWRWW, " W1 W2")
                && startswith (WWWRR, " W1 W2")
                && startswith (WWWRW, " W1 W2")
                && startswith (WWWWR, " W1 W2")
                && startswith (WWWWW, " W1 W2"))
              w_prefers_writers_first_writer = true;
            if (   startswith (WRW, " W1 W3")
                && startswith (WWR, " W1 W2")
                && startswith (WWW, " W1 W3")
                && startswith (WRRW, " W1 W4")
                && startswith (WRWR, " W1 W3")
                && startswith (WRWW, " W1 W4")
                && startswith (WWRR, " W1 W2")
                && startswith (WWRW, " W1 W4")
                && startswith (WWWR, " W1 W3")
                && startswith (WWWW, " W1 W4")
                && startswith (WRRRW, " W1 W5")
                && startswith (WRRWR, " W1 W4")
                && startswith (WRRWW, " W1 W5")
                && startswith (WRWRR, " W1 W3")
                && startswith (WRWRW, " W1 W5")
                && startswith (WRWWR, " W1 W4")
                && startswith (WRWWW, " W1 W5")
                && startswith (WWRRR, " W1 W2")
                && startswith (WWRRW, " W1 W5")
                && startswith (WWRWR, " W1 W4")
                && startswith (WWRWW, " W1 W5")
                && startswith (WWWRR, " W1 W3")
                && startswith (WWWRW, " W1 W5")
                && startswith (WWWWR, " W1 W4")
                && startswith (WWWWW, " W1 W5"))
              w_prefers_writers_last_writer = true;
            if (   startswith (WRR, " W1 R")
                && startswith (WRRR, " W1 R")
                && startswith (WRRRR, " W1 R"))
              {
                w_prefers_writers_some_reader = true;
                if (   startswith (WRR, " W1 R2 R3")
                    && startswith (WRRR, " W1 R2 R3 R4")
                    && startswith (WRRRR, " W1 R2 R3 R4 R5"))
                  w_prefers_writers_first_reader = true;
                if (   startswith (WRR, " W1 R3")
                    && startswith (WRRR, " W1 R4")
                    && startswith (WRRRR, " W1 R5"))
                  w_prefers_writers_last_reader = true;
              }
            if (!w_prefers_writers_some_reader)
              {
                printf ("    WRR =>%s\n", WRR);
                printf ("    WRRR =>%s\n", WRRR);
                printf ("    WRRRR =>%s\n", WRRRR);
              }
          }
        if (!(w_prefers_first_a || w_prefers_readers || w_prefers_writers))
          {
            printf ("    WRR =>%s\n", WRR);
            printf ("    WRW =>%s\n", WRW);
            printf ("    WWR =>%s\n", WWR);
            printf ("    WWW =>%s\n", WWW);
            printf ("    WRRR =>%s\n", WRRR);
            printf ("    WRRW =>%s\n", WRRW);
            printf ("    WRWR =>%s\n", WRWR);
            printf ("    WRWW =>%s\n", WRWW);
            printf ("    WWRR =>%s\n", WWRR);
            printf ("    WWRW =>%s\n", WWRW);
            printf ("    WWWR =>%s\n", WWWR);
            printf ("    WWWW =>%s\n", WWWW);
            printf ("    WRRRR =>%s\n", WRRRR);
            printf ("    WRRRW =>%s\n", WRRRW);
            printf ("    WRRWR =>%s\n", WRRWR);
            printf ("    WRRWW =>%s\n", WRRWW);
            printf ("    WRWRR =>%s\n", WRWRR);
            printf ("    WRWRW =>%s\n", WRWRW);
            printf ("    WRWWR =>%s\n", WRWWR);
            printf ("    WRWWW =>%s\n", WRWWW);
            printf ("    WWRRR =>%s\n", WWRRR);
            printf ("    WWRRW =>%s\n", WWRRW);
            printf ("    WWRWR =>%s\n", WWRWR);
            printf ("    WWRWW =>%s\n", WWRWW);
            printf ("    WWWRR =>%s\n", WWWRR);
            printf ("    WWWRW =>%s\n", WWWRW);
            printf ("    WWWWR =>%s\n", WWWWR);
            printf ("    WWWWW =>%s\n", WWWWW);
          }
      }

      final_r_prefers_first &= r_prefers_first;
      final_r_prefers_readers &= r_prefers_readers;
      final_r_prefers_readers_first_reader &= r_prefers_readers_first_reader;
      final_r_prefers_readers_last_reader &= r_prefers_readers_last_reader;
      final_r_prefers_readers_first_writer &= r_prefers_readers_first_writer;
      final_r_prefers_readers_last_writer &= r_prefers_readers_last_writer;
      final_r_prefers_writers &= r_prefers_writers;
      final_r_prefers_writers_first_writer &= r_prefers_writers_first_writer;
      final_r_prefers_writers_last_writer &= r_prefers_writers_last_writer;
      final_w_prefers_first_a &= w_prefers_first_a;
      final_w_prefers_first_b &= w_prefers_first_b;
      final_w_prefers_first &= w_prefers_first;
      final_w_prefers_readers &= w_prefers_readers;
      final_w_prefers_readers_first_reader &= w_prefers_readers_first_reader;
      final_w_prefers_readers_last_reader &= w_prefers_readers_last_reader;
      final_w_prefers_readers_first_writer &= w_prefers_readers_first_writer;
      final_w_prefers_readers_last_writer &= w_prefers_readers_last_writer;
      final_w_prefers_writers &= w_prefers_writers;
      final_w_prefers_writers_first_writer &= w_prefers_writers_first_writer;
      final_w_prefers_writers_last_writer &= w_prefers_writers_last_writer;
      final_w_prefers_writers_some_reader &= w_prefers_writers_some_reader;
      final_w_prefers_writers_first_reader &= w_prefers_writers_first_reader;
      final_w_prefers_writers_last_reader &= w_prefers_writers_last_reader;
    }

  bool deterministic = true;

  printf ("  When releasing the last reader lock:\n");

  if (final_r_prefers_first)
    printf ("    The first of the enqueued lock attempts is granted.\n");
  else if (final_r_prefers_readers)
    {
      if (final_r_prefers_readers_first_reader)
        printf ("    If at least one of the enqueued lock attempts is for reading, the\n"
                "    first one of them is granted.\n");
      else if (final_r_prefers_readers_last_reader)
        printf ("    If at least one of the enqueued lock attempts is for reading, the\n"
                "    latest (LIFO!) one of them is granted.\n");
      else
        {
          printf ("    If at least one of the enqueued lock attempts is for reading, one\n"
                  "    of them is granted.\n");
          deterministic = false;
        }
      if (final_r_prefers_readers_first_writer)
        printf ("    Otherwise, the first of the waiting write attempts is granted.\n");
      else if (final_r_prefers_readers_last_writer)
        printf ("    Otherwise, the latest (LIFO!) waiting write attempt is granted.\n");
      else
        {
          printf ("    Otherwise ???\n");
          deterministic = false;
        }
    }
  else if (final_r_prefers_writers)
    {
      if (final_r_prefers_writers_first_writer)
        printf ("    If at least one of the enqueued lock attempts is for writing, the\n"
                "    first one of them is granted.\n");
      else if (final_r_prefers_writers_last_writer)
        printf ("    If at least one of the enqueued lock attempts is for writing, the\n"
                "    latest (LIFO!) one of them is granted.\n");
      else
        {
          printf ("    If at least one of the enqueued lock attempts is for writing, one\n"
                  "    of them is granted.\n");
          deterministic = false;
        }
    }
  else
    {
      printf ("    ???\n");
      deterministic = false;
    }

  printf ("  When releasing a writer lock:\n");

  if (final_w_prefers_first_a)
    {
      if (final_w_prefers_first_b)
        {
          if (final_w_prefers_first)
            printf ("    The first of the enqueued lock attempts is granted.\n");
          else
            {
              printf ("    If at least one of the enqueued lock attempts is for writing, the\n"
                      "    first of them is granted.\n"
                      "    Otherwise, one of the waiting read attempts is granted.\n");
              deterministic = false;
            }
        }
      else
        {
          printf ("    If at least one of the enqueued lock attempts is for writing, one of\n"
                  "    the waiting write attempts is granted.\n"
                  "    Otherwise, one of the waiting read attempts is granted.\n");
          deterministic = false;
        }
    }
  else if (final_w_prefers_readers)
    {
      if (final_w_prefers_readers_first_reader)
        printf ("    If at least one of the enqueued lock attempts is for reading, the\n"
                "    first of them is granted.\n");
      else if (final_w_prefers_readers_last_reader)
        printf ("    If at least one of the enqueued lock attempts is for reading, the\n"
                "    latest (LIFO!) one of them is granted.\n");
      else
        {
          printf ("    If at least one of the enqueued lock attempts is for reading, one of\n"
                  "    them is granted.\n");
          deterministic = false;
        }
      if (final_w_prefers_readers_first_writer)
        printf ("    Otherwise, the first of the waiting write attempts is granted.\n");
      else if (final_w_prefers_readers_last_writer)
        printf ("    Otherwise, the latest (LIFO!) of the waiting write attempts is granted.\n");
      else
        {
          printf ("    Otherwise ???\n");
          deterministic = false;
        }
    }
  else if (final_w_prefers_writers)
    {
      if (final_w_prefers_writers_first_writer)
        printf ("    If at least one of the enqueued lock attempts is for writing, the\n"
                "    first one of them is granted.\n");
      else if (final_w_prefers_writers_last_writer)
        printf ("    If at least one of the enqueued lock attempts is for writing, the\n"
                "    latest (LIFO!) one of them is granted.\n");
      else
        {
          printf ("    If at least one of the enqueued lock attempts is for writing, one of\n"
                  "    the waiting write attempts is granted (not necessarily the first one).\n");
          deterministic = false;
        }
      if (final_w_prefers_writers_some_reader)
        {
          if (final_w_prefers_writers_first_reader)
            printf ("    Otherwise, the first of the waiting read attempts is granted.\n");
          else if (final_w_prefers_writers_last_reader)
            printf ("    Otherwise, the latest (LIFO!) of the waiting read attempts is granted.\n");
          else
            {
              printf ("    Otherwise, one of the waiting read attempts is granted.\n");
              deterministic = false;
            }
        }
      else
        {
          printf ("    Otherwise, ???\n");
          deterministic = false;
        }
    }
  else
    {
      printf ("    ???\n");
      deterministic = false;
    }

  if (final_r_prefers_readers)
    {
      if (final_w_prefers_readers)
        printf ("  This implementation always prefers readers.\n");
      else
        printf ("  This implementation does not globally prefer readers, only when releasing\n"
                "  a reader lock.\n");
    }
  else
    {
      if (final_w_prefers_readers)
        printf ("  This implementation does not globally prefer readers, only when releasing\n"
                "  a writer lock.\n");
      else
        printf ("  This implementation does not prefer readers.\n");
    }

  if (final_r_prefers_writers)
    {
      if (final_w_prefers_writers)
        printf ("  This implementation always prefers writers.\n");
      else
        printf ("  This implementation does not globally prefer writers, only when releasing\n"
                "  a reader lock.\n");
    }
  else
    {
      if (final_w_prefers_writers)
        printf ("  This implementation does not globally prefer writers, only when releasing\n"
                "  a writer lock.\n");
      else
        printf ("  This implementation does not prefer writers.\n");
    }

  if (deterministic)
    printf ("  This implementation is deterministic.\n");

  /* The wait queue handling is unsuitable if it always prefers readers,
     because it leads to writer starvation: On machines with 8 or more CPUs,
     test-pthread-rwlock may never terminate.  See
     <https://lists.gnu.org/archive/html/bug-gnulib/2024-06/msg00291.html>
     <https://lists.gnu.org/archive/html/bug-gnulib/2024-07/msg00081.html>
     for details.  */
  return final_r_prefers_readers && final_w_prefers_readers;
}

int
main ()
{
  /* Initialize the sprintf_lock.  */
  ASSERT (pthread_rwlock_init (&sprintf_lock, NULL) == 0);

#if __GLIBC__ >= 2 && defined __linux__

  /* Find the wait queue handling of a PREFER_READER lock.  */
  printf ("glibc/Linux PREFER_READER\n");
  {
    pthread_rwlockattr_t attr;
    ASSERT (pthread_rwlockattr_init (&attr) == 0);
    ASSERT (pthread_rwlockattr_setkind_np (&attr, PTHREAD_RWLOCK_PREFER_READER_NP) == 0);
    ASSERT (pthread_rwlock_init (&lock, &attr) == 0);
    ASSERT (pthread_rwlockattr_destroy (&attr) == 0);
    find_wait_queue_handling ();
    ASSERT (pthread_rwlock_destroy (&lock) == 0);
  }

  /* Find the wait queue handling of an PREFER_WRITER lock.  */
  printf ("glibc/Linux PREFER_WRITER\n");
  {
    pthread_rwlockattr_t attr;
    ASSERT (pthread_rwlockattr_init (&attr) == 0);
    ASSERT (pthread_rwlockattr_setkind_np (&attr, PTHREAD_RWLOCK_PREFER_WRITER_NP) == 0);
    ASSERT (pthread_rwlock_init (&lock, &attr) == 0);
    ASSERT (pthread_rwlockattr_destroy (&attr) == 0);
    find_wait_queue_handling ();
    ASSERT (pthread_rwlock_destroy (&lock) == 0);
  }

  /* Find the wait queue handling of a PREFER_WRITER_NONRECURSIVE lock.  */
  printf ("glibc/Linux PREFER_WRITER_NONRECURSIVE\n");
  {
    pthread_rwlockattr_t attr;
    ASSERT (pthread_rwlockattr_init (&attr) == 0);
    ASSERT (pthread_rwlockattr_setkind_np (&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP) == 0);
    ASSERT (pthread_rwlock_init (&lock, &attr) == 0);
    ASSERT (pthread_rwlockattr_destroy (&attr) == 0);
    find_wait_queue_handling ();
    ASSERT (pthread_rwlock_destroy (&lock) == 0);
  }

  /* Find the wait queue handling of a DEFAULT lock.  */
  printf ("glibc/Linux DEFAULT\n");
  {
    pthread_rwlockattr_t attr;
    ASSERT (pthread_rwlockattr_init (&attr) == 0);
    ASSERT (pthread_rwlockattr_setkind_np (&attr, PTHREAD_RWLOCK_DEFAULT_NP) == 0);
    ASSERT (pthread_rwlock_init (&lock, &attr) == 0);
    ASSERT (pthread_rwlockattr_destroy (&attr) == 0);
    find_wait_queue_handling ();
    ASSERT (pthread_rwlock_destroy (&lock) == 0);
  }

#endif

  /* Find the wait queue handling of a default-initialized lock.  */
  printf ("Default\n");
  ASSERT (pthread_rwlock_init (&lock, NULL) == 0);
  {
    int fail = find_wait_queue_handling ();
    ASSERT (pthread_rwlock_destroy (&lock) == 0);
    ASSERT (fail == 0);
  }

  return test_exit_status;
}

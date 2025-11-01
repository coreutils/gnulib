/* Test openat2.
   Copyright 2025 Free Software Foundation, Inc.

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

/* Written by Paul Eggert and Adhemerval Zanella.  */

#include <config.h>

#include <fcntl.h>

#include "signature.h"
SIGNATURE_CHECK (openat2, int, (int, char const *,
                                struct open_how const *, size_t));

#include <errno.h>
#include <stdarg.h>
#include <stdcountof.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#if HAVE_DECL_ALARM
# include <signal.h>
#endif

#include "ignore-value.h"
#include "macros.h"

#define BASE "test-openat2.t"

#include "test-open.h"

static int dfd = AT_FDCWD;
static uint_least64_t resolve;

/* Wrapper around openat2 to test open behavior.  */
static int
do_open (char const *name, int flags, ...)
{
  mode_t mode = 0;
  if (flags & O_CREAT)
    {
      va_list arg;
      va_start (arg, flags);

      /* We have to use PROMOTED_MODE_T instead of mode_t, otherwise GCC 4
         creates crashing code when 'mode_t' is smaller than 'int'.  */
      mode = va_arg (arg, PROMOTED_MODE_T);

      va_end (arg);
    }

  struct open_how how = { .flags = flags, .mode = mode, .resolve = resolve };
  return openat2 (dfd, name, &how, sizeof how);
}

#define temp_dir BASE "temp_dir"

static void
do_prepare ()
{
  /* Remove any leftovers from a previous partial run.  */
  ignore_value (system ("rm -rf " BASE "*"));

  ASSERT (mkdirat (AT_FDCWD, temp_dir, 0700) == 0);
  dfd = openat2 (AT_FDCWD, temp_dir,
                 (&(struct open_how) { .flags = O_RDONLY | O_DIRECTORY }),
                 sizeof (struct open_how));
  ASSERT (0 <= dfd);
}

static void
do_prepare_symlinks ()
{
  /*
     Construct a test directory with the following structure:

     temp_dir/
        |- escaping_link -> /
        |- escaping_link_2 -> escaping_link
        |- some_file
        |- invalid_link -> some_file/invalid
        |- valid_link -> some_file
	|- subdir/
	   |- some_file
  */

  ASSERT (symlinkat ("/", dfd, "escaping_link") == 0);
  ASSERT (symlinkat ("escaping_link", dfd, "escaping_link_2") == 0);
  ASSERT (symlinkat ("some_file/invalid", dfd, "invalid_link") == 0);
  ASSERT (symlinkat ("some_file", dfd, "valid_link") == 0);
  ASSERT (mkdirat (dfd, "subdir", 0700) == 0);
  ASSERT (close (openat2 (dfd, "some_file",
                          (&(struct open_how) { .flags = O_CREAT,
                                                .mode = 0600 }),
                          sizeof (struct open_how)))
          == 0);
  ASSERT (close (openat2 (dfd, "subdir/some_file",
                          (&(struct open_how) { .flags = O_CREAT,
                                                .mode = 0600 }),
                          sizeof (struct open_how)))
          == 0);
}

static void
do_test_struct ()
{
  static struct struct_test
  {
    struct open_how_ext
    {
      struct open_how inner;
      int extra1;
      int extra2;
      int extra3;
    } arg;
    size_t size;
    int err;
  } const tests[] =
  {
    {
      /* Zero size.  */
      .arg.inner.flags = O_RDONLY,
      .size = 0,
      .err = EINVAL,
    },
    {
      /* Normal struct.  */
      .arg.inner.flags = O_RDONLY,
      .size = sizeof (struct open_how),
    },
    {
      /* Larger struct, zeroed out the unused values.  */
      .arg.inner.flags = O_RDONLY,
      .size = sizeof (struct open_how_ext),
    },
    {
      /* Larger struct, non-zeroed out the unused values.  */
      .arg.inner.flags = O_RDONLY,
      .arg.extra1 = 0xdeadbeef,
      .size = sizeof (struct open_how_ext),
      .err = E2BIG,
    },
    {
      /* Larger struct, non-zeroed out the unused values.  */
      .arg.inner.flags = O_RDONLY,
      .arg.extra2 = 0xdeadbeef,
      .size = sizeof (struct open_how_ext),
      .err = E2BIG,
    },
  };

  for (struct struct_test const *t = tests; t < tests + countof (tests); t++)
    {
      int fd = openat2 (AT_FDCWD, ".", (struct open_how *) &t->arg, t->size);
      if (!t->err)
        ASSERT (close (fd) == 0);
      else
	{
          ASSERT (errno == t->err);
          ASSERT (fd == -1);
	}
    }
}

static void
do_test_flags (void)
{
  static struct flag_test
  {
    char const *filename;
    struct open_how how;
    int err;
  } const tests[] =
  {
#ifdef O_PATH
# ifdef O_TMPFILE
    /* O_TMPFILE is incompatible with O_PATH and O_CREAT.  */
    {
      .how.flags = O_TMPFILE | O_PATH | O_RDWR,
      .err = EINVAL },
    {
      .how.flags = O_TMPFILE | O_CREAT | O_RDWR,
      .err = EINVAL },
# endif

    /* O_PATH only permits certain other flags to be set ...  */
    {
      .how.flags = O_PATH | O_CLOEXEC
    },
    {
      .how.flags = O_PATH | O_DIRECTORY
    },
    {
      .how.flags = O_PATH | O_NOFOLLOW
    },
    /* ... and others are absolutely not permitted. */
    {
      .how.flags = O_PATH | O_RDWR,
      .err = EINVAL },
    {
      .how.flags = O_PATH | O_CREAT,
      .err = EINVAL },
    {
      .how.flags = O_PATH | O_EXCL,
      .err = EINVAL },
    {
      .how.flags = O_PATH | O_NOCTTY,
      .err = EINVAL },
    {
      .how.flags = O_PATH | O_DIRECT,
      .err = EINVAL },
#endif

    /* ->mode must only be set with O_{CREAT,TMPFILE}. */
    {
      .how.flags = O_RDONLY,
      .how.mode = 0600,
      .err = EINVAL },
#ifdef O_PATH
    {
      .how.flags = O_PATH,
      .how.mode = 0600,
      .err = EINVAL },
#endif
    {
      .how.flags = O_CREAT,
      .how.mode = 0600 },
#ifdef O_TMPFILE
    {
      .how.flags = O_TMPFILE | O_RDWR,
      .how.mode = 0600 },
#endif
    /* ->mode must only contain 07777 bits. */
    {
      .how.flags = O_CREAT, .how.mode = 0xFFFF, .err = EINVAL },
    {
      .how.flags = O_CREAT, .how.mode = 0xc000000000000000,
      .err = EINVAL },
#ifdef O_TMPFILE
    {
      .how.flags = O_TMPFILE | O_RDWR, .how.mode = 0x10000,
      .err = EINVAL },
    {
      .how.flags = O_TMPFILE | O_RDWR,
      .how.mode = 0xa00000000000,
      .err = EINVAL
    },
#endif

    /* ->resolve flags must not conflict. */
    {
      .how.flags = O_RDONLY,
      .how.resolve = RESOLVE_BENEATH | RESOLVE_IN_ROOT,
      .err = EINVAL
    },

    /* ->resolve must only contain RESOLVE_* flags.  */
    {
      .how.flags = O_RDONLY,
      .how.resolve = 0x1337,
      .err = EINVAL
    },
    {
      .how.flags = O_CREAT,
      .how.resolve = 0x1337,
      .err = EINVAL
    },
#ifdef O_TMPFILE
    {
      .how.flags = O_TMPFILE | O_RDWR,
      .how.resolve = 0x1337,
      .err = EINVAL
    },
#endif
#ifdef O_PATH
    {
      .how.flags = O_PATH,
      .how.resolve = 0x1337,
      .err = EINVAL
    },
#endif

    /* currently unknown upper 32 bit rejected.  */
    {
      .how.flags = O_RDONLY | (1ull << 63),
      .how.resolve = 0,
      .err = EINVAL
    },
  };

  for (struct flag_test const *t = tests; t < tests + countof (tests); t++)
    {
      char const *filename = t->how.flags & O_CREAT ? "created" : ".";
      if (t->how.flags & O_CREAT)
        unlinkat (dfd, filename, 0);

      int fd = openat2 (dfd, filename, &t->how, sizeof (struct open_how));
      if (fd < 0 && errno == EOPNOTSUPP)
	{
	  /* Skip the testcase if FS does not support the operation (e.g.
	     valid O_TMPFILE on NFS).  */
	  continue;
	}

      if (!t->err)
        ASSERT (close (fd) == 0);
      else
        {
          ASSERT (errno == t->err);
          ASSERT (fd == -1);
	}
    }
}

static bool
is_nofollow_error (int err)
{
#ifdef EFTYPE /* NetBSD openat+O_NOFOLLOW on symlink */
  if (err == EFTYPE)
    return true;
#endif
#ifdef EMLINK /* FreeBSD openat+O_NOFOLLOW on symlink */
  if (err == EMLINK)
    return true;
#endif
  return err == ELOOP;
}

static void
do_test_resolve (void)
{
  int fd;

  /* ESCAPING_LINK links to /tmp, which escapes the temporary test
     directory.  */
  fd = openat2 (dfd,
		"escaping_link",
		(&(struct open_how)
                 {
                   .resolve = RESOLVE_BENEATH | RESOLVE_NO_SYMLINKS,
                 }),
		sizeof (struct open_how));
  ASSERT ((errno == EXDEV) | is_nofollow_error (errno));
  ASSERT (fd == -1);

  /* Same as before, ESCAPING_LINK_2 links to ESCAPING_LINK.  */
  fd = openat2 (dfd,
		"escaping_link_2",
		(&(struct open_how)
                 {
                   .resolve = RESOLVE_BENEATH | RESOLVE_NO_SYMLINKS,
                 }),
		sizeof (struct open_how));
  ASSERT ((errno == EXDEV) | is_nofollow_error (errno));
  ASSERT (fd == -1);

  /* ESCAPING_LINK links to the temporary directory itself (dfd).  */
  fd = openat2 (dfd,
		"escaping_link",
		(&(struct open_how)
                 {
                   .resolve = RESOLVE_BENEATH | RESOLVE_NO_SYMLINKS,
                 }),
		sizeof (struct open_how));
  ASSERT ((errno == EXDEV) | is_nofollow_error (errno));
  ASSERT (fd == -1);

  /* Although it points to a valid file in same path, the link refers to
     an absolute path.  */
  fd = openat2 (dfd,
		"invalid_link",
		(&(struct open_how)
                 {
                   .resolve = RESOLVE_BENEATH | RESOLVE_NO_SYMLINKS,
                 }),
		sizeof (struct open_how));
  ASSERT ((errno == EXDEV) | is_nofollow_error (errno));
  ASSERT (fd == -1);

  fd = openat2 (dfd,
		"valid_link",
		(&(struct open_how)
                 {
                   .resolve = RESOLVE_BENEATH | RESOLVE_NO_SYMLINKS,
                 }),
		sizeof (struct open_how));
  ASSERT (is_nofollow_error (errno));
  ASSERT (fd == -1);

  fd = openat2 (dfd,
	       "should-not-work",
                (&(struct open_how)
                 {
                   .resolve = RESOLVE_IN_ROOT | RESOLVE_NO_SYMLINKS,
                 }),
	       sizeof (struct open_how));
  ASSERT ((errno == ENOENT) | is_nofollow_error (errno));
  ASSERT (fd == -1);

  {
    int subdfd = openat2 (dfd,
                          "subdir",
                          (&(struct open_how)
                           {
                             .flags = O_RDONLY | O_DIRECTORY,
                             .resolve = RESOLVE_IN_ROOT | RESOLVE_NO_SYMLINKS,
                           }),
                          sizeof (struct open_how));
    ASSERT (0 <= subdfd);

    /* Open the file within the subdir with both tst-openat2
       and tst-openat2/subdir file descriptors.  */
    fd = openat2 (subdfd,
                  "some_file",
		  (&(struct open_how)
                   {
                     .resolve = RESOLVE_IN_ROOT,
                   }),
		  sizeof (struct open_how));
    ASSERT (close (fd) == 0);

    fd = openat2 (dfd,
                  "subdir/some_file",
		  (&(struct open_how)
                   {
                     .resolve = RESOLVE_IN_ROOT,
                   }),
		  sizeof (struct open_how));
    ASSERT (close (fd) == 0);
  }
}

static void
do_test_basic ()
{
  int fd;

  fd = openat2 (dfd,
		"some-file",
		(&(struct open_how)
                 {
                   .flags = O_CREAT|O_RDWR|O_EXCL,
                   .mode = 0666,
                 }),
		sizeof (struct open_how));
  ASSERT (0 <= fd);
  ASSERT (write (fd, "hello", 5) == 5);

  /* Before closing the file, try using this file descriptor to open
     another file.  This must fail.  */
  {
    int fd2 = openat2 (fd,
		       "should-not-work",
		       (&(struct open_how)
                        {
                          .flags = O_CREAT|O_RDWR|O_EXCL,
                          .mode = 0666,
                        }),
		       sizeof (struct open_how));
    ASSERT (errno == ENOTDIR);
    ASSERT (fd2 == -1);
  }

  ASSERT (unlinkat (dfd, "some-file", 0) == 0);

  ASSERT (unlinkat (dfd, "escaping_link", 0) == 0);
  ASSERT (unlinkat (dfd, "escaping_link_2", 0) == 0);
  ASSERT (unlinkat (dfd, "invalid_link", 0) == 0);
  ASSERT (unlinkat (dfd, "some_file", 0) == 0);
  ASSERT (unlinkat (dfd, "subdir/some_file", 0) == 0);
  ASSERT (unlinkat (dfd, "subdir", AT_REMOVEDIR) == 0);
  ASSERT (unlinkat (dfd, "valid_link", 0) == 0);

  ASSERT (close (dfd) == 0);

  fd = openat2 (dfd,
		"some-file",
		(&(struct open_how)
                 {
                   .flags = O_CREAT|O_RDWR|O_EXCL,
                   .mode = 0666,
                 }),
		sizeof (struct open_how));
  ASSERT (errno == EBADF);
  ASSERT (fd == -1);

  ASSERT (unlinkat (AT_FDCWD, temp_dir, AT_REMOVEDIR) == 0);
}

int
main ()
{
  int result;
  struct open_how ro = { .flags = O_RDONLY };

  /* Test behavior for invalid file descriptors.  */
  {
    errno = 0;
    ASSERT (openat2 (AT_FDCWD == -1 ? -2 : -1, "foo", &ro, sizeof ro) == -1);
    ASSERT (errno == EBADF);
  }
  {
    close (99);
    errno = 0;
    ASSERT (openat2 (99, "foo", &ro, sizeof ro) == -1);
    ASSERT (errno == EBADF);
  }

  /* Basic checks.  */
  result = test_open (do_open, false);
  dfd = open (".", O_RDONLY);
  ASSERT (0 <= dfd);
  ASSERT (test_open (do_open, false) == result);
  ASSERT (close (dfd) == 0);

  do_test_struct ();

  do_prepare ();
  do_test_flags ();
#if !(defined _WIN32 && !defined __CYGWIN__)
  do_prepare_symlinks ();
  do_test_resolve ();
  do_test_basic ();
#endif

  /* Check that even when *-safer modules are in use, plain openat2 can
     land in fd 0.  Do this test last, since it is destructive to
     stdin.  */
  ASSERT (close (STDIN_FILENO) == 0);
  ASSERT (openat2 (AT_FDCWD, ".", &ro, sizeof ro) == STDIN_FILENO);
  {
    dfd = open (".", O_RDONLY);
    ASSERT (STDIN_FILENO < dfd);
    ASSERT (chdir ("..") == 0);
    ASSERT (close (STDIN_FILENO) == 0);
    ASSERT (openat2 (dfd, ".", &ro, sizeof ro) == STDIN_FILENO);
    ASSERT (close (dfd) == 0);
  }
  return result ? result : test_exit_status;
}

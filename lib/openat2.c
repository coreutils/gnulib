/* Open a file, with more flags than openat
   Copyright 2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* written by Paul Eggert  */

#include <config.h>

#include <fcntl.h>

#include "eloop-threshold.h"
#include "filename.h"
#include "ialloc.h"
#include "idx.h"
#include "verify.h"

#include <errno.h>
#include <limits.h>
#include <stdckdint.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#if defined __linux__ || defined __ANDROID__
# include <sys/syscall.h>
# include <linux/version.h>
# if HAVE_SYS_VFS_H && HAVE_FSTATFS && HAVE_STRUCT_STATFS_F_TYPE
#  include <sys/vfs.h>
/* Linux-specific constant from coreutils src/fs.h.  */
#  define S_MAGIC_PROC 0x9FA0
# endif
#endif

/* FSTAT_O_PATH_BUG is true if fstat fails on O_PATH file descriptors.
   Although it can be dicey to use static checks for Linux kernel versions,
   due to the dubious practice of building on newer kernels for older ones,
   do it here anyway as the buggy kernels are rare (all EOLed by 2016)
   and builders for them are unlikely to use the dubious practice.
   Circa 2030 we should remove the old-kernel workarounds entirely.  */
#ifdef LINUX_VERSION_CODE
# define FSTAT_O_PATH_BUG (KERNEL_VERSION (2, 6, 39) <= LINUX_VERSION_CODE \
                           && LINUX_VERSION_CODE < KERNEL_VERSION (3, 6, 0))
#else
# define FSTAT_O_PATH_BUG false
#endif

#ifndef E2BIG
# define E2BIG EINVAL
#endif

#ifndef PATH_MAX
# define PATH_MAX IDX_MAX
#endif

#ifndef O_ASYNC
# define O_ASYNC 0
#endif
#ifndef O_CLOFORK
# define O_CLOFORK 0
#endif
#ifndef O_LARGEFILE
# define O_LARGEFILE 0
#endif
#ifndef O_NOCLOBBER
# define O_NOCLOBBER 0
#endif
#ifndef O_PATH
# define O_PATH 0
#endif
#ifndef O_RESOLVE_BENEATH /* A FreeBSD flag.  */
# define O_RESOLVE_BENEATH 0
#endif
#ifndef O_TMPFILE
# define O_TMPFILE 0
#endif
#if O_PATH
enum { O_PATHSEARCH = O_PATH };
#else
enum { O_PATHSEARCH = O_SEARCH };
#endif

/* Return true if the memory region at S of size N contains only zeros.  */
static bool
memiszero (void const *s, idx_t n)
{
  /* Keep it simple, as N is typically zero.  */
  char const *p = s;
  for (idx_t i = 0; i < n; i++)
    if (p[i])
      return false;
  return true;
}

/* Return the negative of errno, helping the compiler about its sign.  */
static int
negative_errno (void)
{
  int err = -errno;
  assume (err < 0);
  return err;
}

/* Make *BUF, which is of size BUFSIZE and which is heap-allocated
   if not equal to STACKBUF, large enough to hold an object of NGROW + NTAIL.
   Keep the last NTAIL bytes of *BUF; the rest of *BUF becomes uninitialized.
   NTAIL must not exceed BUFSIZE.
   Return the resulting buffer size, or a negative errno value
   if the buffer could not be grown.  */
static ptrdiff_t
maybe_grow (char **buf, idx_t bufsize, char *stackbuf,
            idx_t ngrow, idx_t ntail)
{
  if (ngrow <= bufsize - ntail)
    return bufsize;

  idx_t needed;
  if (ckd_add (&needed, ngrow, ntail))
    return -ENOMEM;
  idx_t s = ckd_add (&s, needed, needed >> 1) ? needed : s;
  char *newbuf = imalloc (s);
  if (!newbuf)
    return negative_errno ();
  char *oldbuf = *buf;
  memcpy (newbuf + s - ntail, oldbuf + bufsize - ntail, ntail);
  if (oldbuf != stackbuf)
    free (oldbuf);
  *buf = newbuf;
  return s;
}

/* Store DIRFD's file status into *ST.
   DIRFD is either AT_FDCWD or a nonnegative file descriptor.
   Return 0 on success, -1 (setting errno) on failure.  */
static int
dirstat (int dirfd, struct stat *st)
{
  /* Use fstatat only if fstat is buggy.  fstatat is a bit slower,
     and using it only on buggy hosts means openat2 need not depend on
     Gnulib's fstatat module, as all systems with the fstat bug have
     an fstatat that works well enough. */
#if FSTAT_O_PATH_BUG
  return fstatat (dirfd, ".", st);
#else
  return dirfd < 0 ? stat (".", st) : fstat (dirfd, st);
#endif
}

/* Like openat2 (*FD, FILENAME, h, sizeof h) where h is
   (struct open_how) { .flags = FLAGS, .resolve = RESOLVE, .mode = MODE },
   except trust h's contents, advance *FD as we go,
   use and update *BUF (originally pointing to a buffer of size BUFSIZE,
   though it may be changed to point to a freshly allocated heap buffer),
   and return the negative of the errno value on failure.
   *FD and *BUF can be updated even on failure.
   BUFSIZE must be at least 2.  */
static int
do_openat2 (int *fd, char const *filename,
            int flags, char resolve, mode_t mode,
            char **buf, idx_t bufsize)
{
  int dfd = *fd;

  /* RESOLVED_CACHED cannot be implemented properly in user space,
     so pretend nothing is cached.  */
  if (resolve & RESOLVE_CACHED)
    return -EAGAIN;

  /* Put the file name being processed (including trailing NUL) at buffer end,
     to simplify symlink resolution.  */
  idx_t filenamelen = strlen (filename);
  if (!filenamelen)
    return -ENOENT;
  idx_t filenamesize = filenamelen + 1;
  if (PATH_MAX < filenamesize)
    return -ENAMETOOLONG;
  char *stackbuf = *buf;
  bufsize = maybe_grow (buf, bufsize, stackbuf, filenamesize, 0);
  if (bufsize < 0)
    return bufsize;

  /* Pointer to buffer end.  A common access is E[-I] where I is a
     negative index relative to buffer end.  */
  char *e = *buf + bufsize;
  memcpy (&e[-filenamesize], filename, filenamesize);

  /* Directory depth below DFD.  This is -1 if ".." ascended above
     DFD at any point in the past, which can happen only if
     neither RESOLVE_BENEATH nor RESOLVE_IN_ROOT is in effect.  */
  ptrdiff_t depth = 0;

  /* DFD's device.  UNKNOWN_DDEV if not acquired yet.  If the actual
     device number equals UNKNOWN_DDEV the code still works,
     albeit more slowly.  */
  dev_t const UNKNOWN_DEV = -1;
  dev_t ddev = UNKNOWN_DEV;

  long int maxlinks = resolve & RESOLVE_NO_SYMLINKS ? 0 : __eloop_threshold ();

  /* Iterates through file name components, possibly expanded by
     symlink contents.  */
  while (true)
    {
      /* Make progress in interpreting &E[-FILENAMESIZE] as a file name.
         If relative, it is relative to *FD.
         FILENAMESIZE is positive.

         Start by computing sizes of strings at the file name's end.
         Use negations of sizes to index into E.
         Here is an example file name and sizes of the trailing strings:

           ///usr//bin/.////cat
           F  G  H             1

         As the '1' indicates, all sizes are positive
         and include the trailing NUL at E[-1].

         If there are file name components (the typical case),
         -F <= -G < -H <= -1 and the first component
         starts at E[-G] and ends just before E[-H].
         Otherwise if the file name is nonempty,
         -F < -G = -H = -1 and &E[-F] is a file system root.
         Otherwise it is Solaris and we resolved an empty final symlink, and
         -F = -G = -H = -1 and the empty file name is equivalent to ".".

         F = G means the file name is relative to *FD;
         otherwise the file name is not relative.

         F (i.e., FILENAMESIZE) is the size of the file name.  &E[-F] is what
         is typically passed next to openat (with E[-H] set to NUL).

         G is the size of the file name's suffix that starts with the name's
         first component; &E[-G] addresses the first component.

         H is the size of the suffix after the first component, i.e.,
         E[-H] is the slash or NUL after the first component.

         If there is no component, G and H are both 1.  */
      idx_t f = filenamesize;
      idx_t g = f - FILE_SYSTEM_PREFIX_LEN (&e[-f]);
      while (ISSLASH (e[-g]))
        g--;
      if (f != g)
        {
          /* The file name is not relative.  */
          if (resolve & RESOLVE_BENEATH)
            return -EXDEV;
          if (resolve & RESOLVE_IN_ROOT)
            f = g;
          if (*fd != dfd)
            {
              /* A non-relative symlink had been resolved at positive depth.
                 Forget its parent directory.  */
              close (*fd);
              *fd = dfd;
            }
        }
      idx_t h = g;
      while (h > 1)
        {
          h--;
          if (ISSLASH (e[-h]))
            break;
        }

      /* Properties of the file name through the first component's end,
         or to file name end if there is no component.  */
      bool leading_dot = e[-f] == '.';
      bool dot_or_empty = f - h == leading_dot;
      bool dotdot = leading_dot & (f - h == 2) & (e[-h - 1] == '.');
      bool dotdot_as_dot = dotdot & !depth & !!(resolve & RESOLVE_IN_ROOT);
      bool dotlike = dot_or_empty | dotdot_as_dot;

      if (dotdot & !depth & !!(resolve & RESOLVE_BENEATH))
        return -EXDEV;

      /* NEXTF is the value of FILENAMESIZE next time through the loop,
         unless a symlink intervenes.  */
      idx_t nextf = h;
      while (ISSLASH (e[-nextf]))
        nextf--;

      /* FINAL means this is the last time through the loop,
         unless a symlink intervenes.  */
      bool final = nextf == 1;

      if (!final & dotlike)
        {
          /* A non-final component that acts like "."; skip it.  */
          filenamesize = nextf;
        }
      else if (!final & dotdot & (depth == 1))
        {
          /* A non-final ".." in a name like "x/../y/z" when "x" is an
             existing non-symlink directory.  As an optimization,
             resolve it like "y/z".  */
          close (*fd);
          *fd = dfd;
          depth = 0;
          filenamesize = nextf;
        }
      else
        {
          if (dotlike)
            {
              /* This is empty or the last component, and acts like ".".
                 Use "." regardless of whether it was "" or "." or "..".  */
              f = sizeof ".";
              e[-f] = '.';
            }

          /* Open the current component, as either an internal directory or
             the final open.  Do not follow symlinks.  */
          int subflags = ((!final
                           ? O_PATHSEARCH | O_CLOEXEC | O_CLOFORK
                           : flags)
                          | O_NOFOLLOW | (e[-h] ? O_DIRECTORY : 0));
          e[-h] = '\0';
          int subfd = openat (*fd, &e[-f], subflags, mode);

          if (subfd < 0)
            {
              if (maxlinks <= 0 || errno != ELOOP)
                return negative_errno ();
              maxlinks--;

              /* A symlink and the symlink loop count is not exhausted.
                 Fail now if magic and if RESOLVE_NO_MAGIC_LINKS.  */
#ifdef S_MAGIC_PROC
              if (resolve & RESOLVE_NO_MAGICLINKS)
                {
                  bool relative = IS_RELATIVE_FILE_NAME (&e[-f]);
                  struct statfs st;
                  int r;
                  if (relative)
                    r = *fd < 0 ? statfs (".", &st) : fstatfs (*fd, &st);
                  else
                    {
                      char eg = e[-g];
                      e[-g] = '\0';
                      r = statfs (&e[-f], &st);
                      e[-g] = eg;
                    }
                  if (r < 0)
                    return negative_errno ();
                  if (st.f_type == S_MAGIC_PROC)
                    return -ELOOP;
                }
#endif

              /* Read symlink contents into buffer start.
                 But if the root prefix might be needed,
                 leave room for it at buffer start.  */
              idx_t rootlen = f - g;
              char *slink;
              ssize_t slinklen;
              for (idx_t more = rootlen + 1; ; more = bufsize - f + 1)
                {
                  bufsize = maybe_grow (buf, bufsize, stackbuf, more, f);
                  if (bufsize < 0)
                    return bufsize;
                  e = *buf + bufsize;
                  slink = *buf + rootlen;
                  idx_t slinksize = bufsize - f - rootlen;
                  slinklen = readlinkat (*fd, &e[-f], slink, slinksize);
                  if (slinklen < 0)
                    return negative_errno ();
                  if (slinklen < slinksize)
                    break;
                }

              /* Compute KEPT, the number of trailing bytes in the file
                 name that will be appended to the symlink contents.  */
              idx_t kept;
              if (slinklen == 0)
                {
                  /* On Solaris empty symlinks act like ".".
                     On other platforms that allow them,
                     they fail with ENOENT.  */
#ifdef __sun
                  slink[slinklen] = '\0';  /* For IS_RELATIVE_FILE_NAME.  */
                  kept = nextf;
#else
                  return -ENOENT;
#endif
                }
              else if (ISSLASH (slink[slinklen - 1]))
                {
                  /* Skip any leading slashes in the kept bytes.
                     This can matter if the symlink contains only slashes,
                     because "//" and "/" can be distinct directories.  */
                  kept = nextf;
                }
              else
                kept = h;

              if (ISSLASH ('\\'))
                slink[slinklen] = '\0';  /* For IS_RELATIVE_FILE_NAME.  */

              /* Compute the new file name by concatenating:
                  - Any old root prefix if the symlink contents are relative.
                  - The symlink contents.
                  - The last KEPT bytes of the old file name.
                 The KEPT part is already in place.  */
              char const *prefix;  /* [old root prefix +] symlink contents */
              idx_t prefixlen;
              if (IS_RELATIVE_FILE_NAME (slink))
                {
                  prefix = memmove (*buf, &e[-f], rootlen);
                  prefixlen = rootlen + slinklen;
                }
              else
                {
                  if (*fd != dfd)
                    {
                      close (*fd);
                      *fd = dfd;
                    }
                  prefix = slink;
                  prefixlen = slinklen;
                }
              filenamesize = prefixlen + kept;
              if (PATH_MAX < filenamesize)
                return -ENAMETOOLONG;
              memmove (&e[-filenamesize], prefix, prefixlen);
            }
          else
            {
              if (*fd != dfd)
                close (*fd);
              *fd = subfd;

              /* SUBFD is open to the file named by the current component.
                 If requested, require it to be in the same file system.  */
              if (resolve & RESOLVE_NO_XDEV)
                {
                  struct stat st;
                  if (ddev == UNKNOWN_DEV)
                    {
                      if (dirstat (dfd, &st) < 0)
                        return negative_errno ();
                      ddev = st.st_dev;
                    }
                  if (dirstat (subfd, &st) < 0)
                    return negative_errno ();
                  if (st.st_dev != ddev)
                    return -EXDEV;
                }

              if (final)
                {
                  *fd = dfd;
                  return subfd;
                }

              /* The component cannot be dotlike here, so if the depth is
                 nonnegative adjust it by +1 or -1.  */
              if (0 <= depth)
                depth += dotdot ? -1 : 1;

              filenamesize = nextf;
            }
        }
    }
}

/* Like openat (DFD, FILENAME, HOW->flags, HOW->mode),
   but with extra flags in *HOW, which is of size HOWSIZE.  */
int
openat2 (int dfd, char const *filename,
         struct open_how const *how, size_t howsize)
{
  int r;

#ifdef SYS_openat2
  r = syscall (SYS_openat2, dfd, filename, how, howsize);
  if (! (r < 0 && errno == ENOSYS))
    return r;

  /* Keep going, to support the dubious practice of compiling for an
     older kernel.  The openat2 syscall was introduced in Linux 5.6.
     Linux 5.4 LTS is EOLed at the end of 2025, so perhaps after that
     we can simply return the syscall result instead of continuing.  */
#endif

  /* Check for invalid arguments.  Once the size test has succeeded,
     *HOW's members are safe to access, so use & and | as there is
     little point to using && or || when invalid arguments are rare.
     (Other parts of this file also use & and | for similar reasons.)
     These checks mimic those of the Linux kernel: when the Linux
     kernel is overly generous, these checks are too.  */
  if (howsize < sizeof *how)
    r = -EINVAL;
  else if (!memiszero (how + 1, howsize - sizeof *how))
    r = -E2BIG;
  else if ((how->flags
            & ~ (O_CLOFORK | O_CLOEXEC | O_DIRECTORY
                 | O_NOFOLLOW | O_PATH
                 | (how->flags & O_PATH
                    ? 0
                    : (O_ACCMODE | O_APPEND | O_ASYNC | O_BINARY
                       | O_CREAT | O_DIRECT | O_DSYNC | O_EXCL
                       | O_IGNORE_CTTY | O_LARGEFILE | O_NDELAY
                       | O_NOATIME | O_NOCLOBBER | O_NOCTTY
                       | O_NOLINK | O_NOLINKS | O_NONBLOCK | O_NOTRANS
                       | O_RSYNC | O_SYNC
                       | O_TEXT | O_TMPFILE | O_TRUNC | O_TTY_INIT))))
           | (!!(how->flags & O_CREAT)
              & !!(how->flags & (O_DIRECTORY | O_TMPFILE)))
           | (!!(how->flags & O_TMPFILE & ~O_DIRECTORY)
              & ((how->flags & (O_ACCMODE | O_PATH)) != O_WRONLY)
              & ((how->flags & (O_ACCMODE | O_PATH)) != O_RDWR))
           | (how->mode
              & ~ (how->flags & (O_CREAT | (O_TMPFILE & ~O_DIRECTORY))
                   ? (S_ISUID | S_ISGID | S_ISVTX
                      | S_IRWXU | S_IRWXG | S_IRWXO)
                   : 0))
           | (how->resolve
              & ~ (RESOLVE_BENEATH | RESOLVE_CACHED | RESOLVE_IN_ROOT
                   | RESOLVE_NO_MAGICLINKS | RESOLVE_NO_SYMLINKS
                   | RESOLVE_NO_XDEV))
           | ((how->resolve & (RESOLVE_BENEATH | RESOLVE_IN_ROOT))
              == (RESOLVE_BENEATH | RESOLVE_IN_ROOT)))
    r = -EINVAL;
  else
    {
      /* Args are valid so it is safe to use narrower types.  */
      int flags = how->flags;
      char resolve = how->resolve;
      mode_t mode = how->mode;

      /* For speed use openat if it suffices, though it is unlikely a
         caller would use openat2 when openat's simpler API would do.  */
      if (O_RESOLVE_BENEATH ? !(resolve & ~RESOLVE_BENEATH) : !resolve)
        {
          if (resolve & RESOLVE_BENEATH)
            flags |= O_RESOLVE_BENEATH;
          return openat (dfd, filename, flags, mode);
        }

      int fd = dfd;
      char stackbuf[256];
      char *buf = stackbuf;

      r = do_openat2 (&fd, filename, flags, resolve, mode,
                      &buf, sizeof stackbuf);

      if (fd != dfd)
        close (fd);
      if (buf != stackbuf)
        free (buf);
    }

  if (r < 0)
    {
      errno = -r;
      return -1;
    }
  return r;
}

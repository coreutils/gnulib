/* Dropping uid/gid privileges of the current process permanently.
   Copyright (C) 2009-2022 Free Software Foundation, Inc.

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

#include <config.h>

#include "idpriv.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int
idpriv_drop (void)
{
#if HAVE_GETUID
  int uid = getuid ();
#endif
#if HAVE_GETGID
  int gid = getgid ();
#endif

  /* Drop the gid privilege first, because in some cases the gid privilege
     cannot be dropped after the uid privilege has been dropped.  */

  /* This is for executables that have the setgid bit set.  */
#if HAVE_SETRESGID /* glibc, FreeBSD, OpenBSD, HP-UX */
  /* This code is needed: In particular, on HP-UX 11.11, setregid (gid, gid)
     may leave the saved gid as 0.  See also the comment below regarding
     setresuid.  */
  if (setresgid (gid, gid, gid) < 0)
    return -1;
#elif HAVE_SETREGID /* Mac OS X, NetBSD, AIX, IRIX, Solaris, OSF/1, Cygwin */
  if (setregid (gid, gid) < 0)
    return -1;
#elif HAVE_SETEGID /* Solaris 2.4 */
  if (setegid (gid) < 0)
    return -1;
#endif

  /* This is for executables that have the setuid bit set.  */
#if HAVE_SETRESUID /* glibc, FreeBSD, OpenBSD, HP-UX */
  /* On systems which have setresuid(), we use it instead of setreuid(),
     because
       Hao Chen, David Wagner, Drew Dean: Setuid Demystified
       <https://www.usenix.org/legacy/publications/library/proceedings/sec02/full_papers/chen/chen.pdf>
     says about setreuid(): "The rule by which the saved uid id is modified
     is complicated." Similarly, <https://unixpapa.com/incnote/setuid.html>
     says about setreuid(): "What exactly happens to the saved UID when this
     is used seems to vary a lot."  */
  if (setresuid (uid, uid, uid) < 0)
    return -1;
#elif HAVE_SETREUID /* Mac OS X, NetBSD, AIX, IRIX, Solaris, OSF/1, Cygwin */
  if (setreuid (uid, uid) < 0)
    return -1;
#elif HAVE_SETEUID /* Solaris 2.4 */
  if (seteuid (uid) < 0)
    return -1;
#endif

  /* Verify that the privileges have really been dropped.
     This verification is here for security reasons.  Doesn't matter if it
     takes a couple of system calls.
     On Solaris (which has saved uids and gids but no getresuid, getresgid
     functions), we could read /proc/<pid>/cred and verify the saved uid and
     gid found there. But it's not clear to me when to interpret the file as a
     'prcred_t' and when as a 'prcred32_t'.
     Hao Chen, David Wagner, Drew Dean: Setuid Demystified
     <https://www.usenix.org/legacy/publications/library/proceedings/sec02/full_papers/chen/chen.pdf>
     section 8.1.3 also recommends to use a setreuid call as a probe, but
     this call would unexpectedly succeed (and the verification thus fail)
     on Linux if the process has the CAP_SETUID capability.
     When the verification fails, it indicates that we need to use different
     API in the code above. Therefore 'abort ()', not 'return -1'.  */
#if HAVE_GETRESUID /* glibc, FreeBSD, OpenBSD, HP-UX */
  {
    uid_t real;
    uid_t effective;
    uid_t saved;
    if (getresuid (&real, &effective, &saved) < 0
        || real != uid
        || effective != uid
        || saved != uid)
      abort ();
  }
#else
# if HAVE_GETEUID
  if (geteuid () != uid)
    abort ();
# endif
# if HAVE_GETUID
  if (getuid () != uid)
    abort ();
# endif
#endif
#if HAVE_GETRESGID /* glibc, FreeBSD, OpenBSD, HP-UX */
  {
    gid_t real;
    gid_t effective;
    gid_t saved;
    if (getresgid (&real, &effective, &saved) < 0
        || real != gid
        || effective != gid
        || saved != gid)
      abort ();
  }
#else
# if HAVE_GETEGID
  if (getegid () != gid)
    abort ();
# endif
# if HAVE_GETGID
  if (getgid () != gid)
    abort ();
# endif
#endif

  return 0;
}

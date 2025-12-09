/* Determination whether a file is local or remote.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include "file-remote.h"

#include <string.h>

#if defined __linux__ || defined __ANDROID__                /* Linux */
# include <sys/statfs.h>
#elif defined __gnu_hurd__                                  /* GNU/Hurd */
# include <sys/statfs.h>
# include <hurd/hurd_types.h>
#elif defined __APPLE__ && defined __MACH__                 /* macOS */
# include <sys/mount.h>
#elif defined __FreeBSD__ || defined __DragonFly__ || defined __FreeBSD_kernel__
                                                     /* FreeBSD, GNU/kFreeBSD */
# include <sys/types.h>
# include <sys/param.h>
# include <sys/mount.h>
#elif defined __NetBSD__                                    /* NetBSD */
# include <sys/statvfs.h>
#elif defined __OpenBSD__                                   /* OpenBSD */
# include <sys/types.h>
# include <sys/mount.h>
#elif defined _AIX                                          /* AIX */
# include <sys/statvfs.h>
#elif defined __sun                                         /* Solaris */
# include <sys/statvfs.h>
#elif defined __CYGWIN__                                    /* Cygwin */
# include "cygpath.h"
#elif defined __HAIKU__                                     /* Haiku */
# include <sys/stat.h>
# include <fs_info.h>
# include <errno.h>
#else                                                       /* Unknown OS */
# if defined SLOW_AND_OVERKILL
#  include <sys/stat.h>
#  include "mountlist.h"
#  include <errno.h>
# endif
#endif

#if defined _WIN32 || defined __CYGWIN__                    /* Windows */
# define WIN32_LEAN_AND_MEAN  /* avoid including junk */
# include <windows.h>
# include <errno.h>
/* Don't assume that UNICODE is not defined.  */
# undef GetFullPathName
# define GetFullPathName GetFullPathNameA
# undef GetDriveType
# define GetDriveType GetDriveTypeA

/* Here, FILE is a file or directory name in native Windows syntax.  */
static int
windows_file_is_remote (const char *file)
{
  char buf[MAX_PATH];
  /* Documentation:
     <https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfullpathnamea>  */
  if (! GetFullPathName (file, sizeof (buf), buf, NULL))
    {
      switch (GetLastError ())
        {
        case ERROR_FILE_NOT_FOUND: /* The last component of file does not exist.  */
        case ERROR_PATH_NOT_FOUND: /* Some directory component in file does not exist.  */
        case ERROR_BAD_PATHNAME:   /* file is such as '\\server'.  */
        case ERROR_BAD_NET_NAME:   /* file is such as '\\server\nonexistentshare'.  */
        case ERROR_INVALID_NAME:   /* file contains wildcards, misplaced colon, etc.  */
          errno = ENOENT;
          break;
        case ERROR_ACCESS_DENIED:  /* file is such as 'C:\System Volume Information\foo'.  */
        case ERROR_SHARING_VIOLATION: /* file is such as 'C:\pagefile.sys'.  */
                                      /* XXX map to EACCES or EPERM? */
          errno = EACCES;
          break;
        default:
          errno = EINVAL;
          break;
        }
      return -1;
    }
  const char *root = buf;
  if (root[0] == '\\' && root[1] == '\\')
    {
      if (root[2] == '?' && root[3] == '\\')
        {
          if (strncmp (root + 4, "Volume", 6) == 0)
            /* '\\?\Volume{GUID}\' designates a local volume.  */
            return 0;
          if (strncmp (root + 4, "UNC\\", 4) == 0)
            /* '\\?\UNC\server\share\' designates a remote mount.  */
            return 1;
          root += 4;
        }
      else
        /* '\\server\share\' designates a remote mount.  */
        return 1;
    }
  if (root[0] != '\0' && root[1] == ':')
    {
      /* Documentation:
         <https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getdrivetypea>  */
      UINT type = GetDriveType (root);
      return type == DRIVE_REMOTE;
    }
  /* ROOT does not look like a remote mount.  */
  return 0;
}

#endif

int
file_is_remote (const char *file)
{
#if defined __linux__ || defined __ANDROID__                /* Linux */
  struct statfs fs;
  if (statfs (file, &fs) < 0)
    return -1;
  /* See coreutils/src/fs.h and coreutils/src/fs-is-local.h.  */
  switch (fs.f_type)
    {
    case 0x61636673 /* S_MAGIC_ACFS */:
    case 0x5346414F /* S_MAGIC_AFS */:
    case 0x00C36400 /* S_MAGIC_CEPH */:
    case 0xFF534D42 /* S_MAGIC_CIFS */:
    case 0x73757245 /* S_MAGIC_CODA */:
    case 0x19830326 /* S_MAGIC_FHGFS */:
    case 0x65735546 /* S_MAGIC_FUSE */:
    case 0x65735543 /* S_MAGIC_FUSECTL */:
    case 0x01161970 /* S_MAGIC_GFS */:
    case 0x47504653 /* S_MAGIC_GPFS */:
    case 0x013111A8 /* S_MAGIC_IBRIX */:
    case 0x6B414653 /* S_MAGIC_KAFS */:
    case 0x0BD00BD0 /* S_MAGIC_LUSTRE */:
    case 0x0000564C /* S_MAGIC_NCP */:
    case 0x00006969 /* S_MAGIC_NFS */:
    case 0x6E667364 /* S_MAGIC_NFSD */:
    case 0x7461636F /* S_MAGIC_OCFS2 */:
    case 0x794C7630 /* S_MAGIC_OVERLAYFS */:
    case 0xAAD7AAEA /* S_MAGIC_PANFS */:
    case 0x7C7C6673 /* S_MAGIC_PRL_FS */:
    case 0x0000517B /* S_MAGIC_SMB */:
    case 0xFE534D42 /* S_MAGIC_SMB2 */:
    case 0xBEEFDEAD /* S_MAGIC_SNFS */:
    case 0x786F4256 /* S_MAGIC_VBOXSF */:
    case 0xBACBACBC /* S_MAGIC_VMHGFS */:
    case 0xA501FCF5 /* S_MAGIC_VXFS */:
      return 1;
    default:
      return 0;
    }
#elif defined __gnu_hurd__                                  /* GNU/Hurd */
  /* On this platform, we could equally use 'statvfs', as
     it's identical to 'statfs'.  */
  struct statfs fs;
  if (statfs (file, &fs) < 0)
    return -1;
  /* See <hurd/hurd_types.h>.
     Note: The only ones of these that are currently implemented in hurd.git
     are nfs and ftpfs.  */
  switch (fs.f_type)
    {
    case FSTYPE_NFS:    /* Network File System ala Sun */
    case FSTYPE_FTP:    /* Transparent FTP */
    case FSTYPE_GRFS:   /* GNU Remote File System */
    case FSTYPE_AFS:    /* Andrew File System 3.xx */
    case FSTYPE_DFS:    /* Distributed File Sys (OSF) == AFS 4.xx */
    case FSTYPE_SOCKET: /* io_t that isn't a file but a socket */
    case FSTYPE_HTTP:   /* Transparent HTTP */
      return 1;
    default:
      return 0;
    }
#elif defined __APPLE__ && defined __MACH__                 /* macOS */
  struct statfs fs;
  if (statfs (file, &fs) < 0)
    return -1;
  return (fs.f_flags & MNT_LOCAL) == 0
         && !(strcmp (fs.f_fstypename, "fdesc") == 0);
#elif defined __FreeBSD__ || defined __DragonFly__ || defined __FreeBSD_kernel__
                                                     /* FreeBSD, GNU/kFreeBSD */
  struct statfs fs;
  if (statfs (file, &fs) < 0)
    return -1;
  return (fs.f_flags & MNT_LOCAL) == 0
         && !(strcmp (fs.f_fstypename, "devfs") == 0
              || strcmp (fs.f_fstypename, "fdescfs") == 0);
#elif defined __NetBSD__                                    /* NetBSD */
  struct statvfs fs;
  if (statvfs1 (file, &fs, ST_NOWAIT) < 0)
    return -1;
  return (fs.f_flag & ST_LOCAL) == 0;
#elif defined __OpenBSD__                                   /* OpenBSD */
  struct statfs fs;
  if (statfs (file, &fs) < 0)
    return -1;
  return (fs.f_flags & MNT_LOCAL) == 0;
#elif defined _AIX                                          /* AIX */
  struct statvfs fs;
  if (statvfs (file, &fs) < 0)
    return -1;
  /* See /etc/vfs.  */
  return (strcmp (fs.f_basetype, "nfs") == 0
          || strcmp (fs.f_basetype, "nfs3") == 0
          || strcmp (fs.f_basetype, "nfs4") == 0
          || strcmp (fs.f_basetype, "stnfs") == 0
          || strcmp (fs.f_basetype, "cachefs") == 0);
#elif defined __sun                                         /* Solaris */
  struct statvfs fs;
  if (statvfs (file, &fs) < 0)
    return -1;
  /* See /etc/dfs/fstypes.  */
  return (strcmp (fs.f_basetype, "nfs") == 0
          || strcmp (fs.f_basetype, "smb") == 0
          || strcmp (fs.f_basetype, "smbfs") == 0
          || strcmp (fs.f_basetype, "autofs") == 0);
#elif defined __CYGWIN__                                    /* Cygwin */
  /* The 'struct statfs' member 'f_type' does not have the
     necessary information.  We need to use the native Windows API.  */
  char *windows_file = cygpath_w (file);
  int result = windows_file_is_remote (windows_file);
  free (windows_file);
  return result;
#elif defined _WIN32 && !defined __CYGWIN__                 /* Native Windows */
  return windows_file_is_remote (file);
#elif defined __HAIKU__                                     /* Haiku */
  struct stat statbuf;
  if (stat (file, &statbuf) < 0)
    return -1;
  dev_t device = statbuf.st_dev;
  /* Documentation:
     https://www.haiku-os.org/legacy-docs/bebook/TheStorageKit_Functions.html#fs_stat_dev
     https://www.haiku-os.org/legacy-docs/bebook/TheStorageKit_DefinedTypes.html#fs_info
     This function actually sets errno when it fails.  */
  struct fs_info fs;
  if (fs_stat_dev (device, &fs) != B_OK)
    return -1;
  return (strcmp (fs.fsh_name, "userlandfs") == 0
          || strcmp (fs.fsh_name, "nfs") == 0
          || strcmp (fs.fsh_name, "netfs") == 0
          || strcmp (fs.fsh_name, "websearchfs") == 0);
#else                                                       /* Unknown OS */
# if defined SLOW_AND_OVERKILL
  /* This makes many system calls, is therefore slow, and
     is also not multithread-safe.  */
  struct stat statbuf;
  if (stat (file, &statbuf) < 0)
    return -1;
  dev_t device = statbuf.st_dev;
  struct mount_entry *me_list = read_file_system_list (true);
  if (me_list == NULL)
    {
      errno = EIO;
      return -1;
    }
  int result = 0;
  for (struct mount_entry *me = me_list; me != NULL; )
    {
      if (me->me_dev == device)
        {
          result = me->me_remote;
          break;
        }
      me = me->me_next;
    }
  for (struct mount_entry *me = me_list; me != NULL; )
    {
      struct mount_entry *next = me->me_next;
      free_mount_entry (me);
      me = next;
    }
  return result;
# else
  /* Assume all file systems are local.  */
  return 0;
# endif
#endif
}


#if TEST

#include <errno.h>
#include <stdio.h>
#include <string.h>

/* Test program that prints the result for a file name given on the command
   line.  */
int
main (int argc, char *argv[])
{
  for (int i = 1; i < argc; i++)
    {
      const char *file = argv[i];
      int ret = file_is_remote (file);
      if (ret == 0)
        printf ("%s => local\n", file);
      else if (ret > 0)
        printf ("%s => remote\n", file);
      else
        printf ("%s => error: %s\n", file, strerror (errno));
    }

  return 0;
}

#endif /* TEST */

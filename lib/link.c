/* Emulate link on platforms that lack it, namely native Windows platforms.

   Copyright (C) 2009 Free Software Foundation, Inc.

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

#include <config.h>

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__

#define WIN32_LEAN_AND_MEAN
#include <unistd.h>
#include <windows.h>

#include <errno.h>

/* CreateHardLink was introduced only in Windows 2000.  */
typedef BOOL (WINAPI * CreateHardLinkFuncType) (LPCTSTR lpFileName,
						LPCTSTR lpExistingFileName,
						LPSECURITY_ATTRIBUTES lpSecurityAttributes);
static CreateHardLinkFuncType CreateHardLinkFunc = NULL;
static BOOL initialized = FALSE;

static void
initialize (void)
{
  HMODULE kernel32 = LoadLibrary ("kernel32.dll");
  if (kernel32 != NULL)
    {
      CreateHardLinkFunc =
	(CreateHardLinkFuncType) GetProcAddress (kernel32, "CreateHardLinkA");
    }
  initialized = TRUE;
}

int
link (const char *path1, const char *path2)
{
  if (!initialized)
    initialize ();
  if (CreateHardLinkFunc == NULL)
    {
      /* System does not support hard links.  */
      errno = EPERM;
      return -1;
    }
  if (CreateHardLinkFunc (path2, path1, NULL) == 0)
    {
      /* It is not documented which errors CreateHardLink() can produce.
       * The following conversions are based on tests on a Windows XP SP2
       * system. */
      DWORD err = GetLastError ();
      switch (err)
	{
	case ERROR_ACCESS_DENIED:
	  errno = EACCES;
	  break;

	case ERROR_INVALID_FUNCTION:	/* fs does not support hard links */
	  errno = EPERM;
	  break;

	case ERROR_NOT_SAME_DEVICE:
	  errno = EXDEV;
	  break;

	case ERROR_PATH_NOT_FOUND:
	case ERROR_FILE_NOT_FOUND:
	  errno = ENOENT;
	  break;

	case ERROR_INVALID_PARAMETER:
	  errno = ENAMETOOLONG;
	  break;

	case ERROR_TOO_MANY_LINKS:
	  errno = EMLINK;
	  break;

	case ERROR_ALREADY_EXISTS:
	  errno = EEXIST;
	  break;

	default:
	  errno = EIO;
	}
      return -1;
    }

  return 0;
}

#else /* !Windows */

#error "This platform lacks a link function, and Gnulib doesn't provide a replacement. This is a bug in Gnulib."

#endif /* !Windows */

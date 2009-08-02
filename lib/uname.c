/* uname replacement.
   Copyright (C) 2009 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <sys/utsname.h>

/* This file provides an implementation only for the native Windows API.  */
#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__

#include <stdio.h>
#include <unistd.h>
#include <windows.h>

/* Mingw headers don't have latest processor codes.  */
#ifndef PROCESSOR_AMD_X8664
# define PROCESSOR_AMD_X8664 8664
#endif

int
uname (struct utsname *buf)
{
  OSVERSIONINFO version;
  BOOL have_version;
  const char *super_version;

  /* Fill in nodename.  */
  if (gethostname (buf->nodename, sizeof (buf->nodename)) < 0)
    strcpy (buf->nodename, "localhost");

  /* Determine major-major Windows version.  */
  version.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
  have_version = GetVersionEx (&version);
  if (have_version)
    {
      if (version.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
	  /* Windows NT or newer.  */
	  super_version = "NT";
	}
      else if (version.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
	  /* Windows 95/98/ME.  */
	  switch (version.dwMinorVersion)
	    {
	    case 0:
	      super_version = "95";
	      break;
	    case 10:
	      super_version = "98";
	      break;
	    case 90:
	      super_version = "ME";
	      break;
	    default:
	      super_version = "";
	      break;
	    }
	}
      else
	super_version = "";
    }
  else
    super_version = "";

  /* Fill in sysname.  */
#ifdef __MINGW32__
  /* Returns a string compatible with the MSYS uname.exe program,
     so that no further changes are needed to GNU config.guess.
     For example,
       $ ./uname.exe -s      => MINGW32_NT-5.1
   */
  if (have_version)
    sprintf (buf->sysname, "MINGW32_%s-%u.%u", super_version,
	     (unsigned int) version.dwMajorVersion,
	     (unsigned int) version.dwMinorVersion);
  else
    strcpy (buf->sysname, "MINGW32");
#else
  if (have_version)
    sprintf (buf->sysname, "Windows%s", super_version);
  else
    strcpy (buf->sysname, "Windows");
#endif

  /* Fill in release, version.  */
  /* The MSYS uname.exe programs uses strings from a modified Cygwin runtime:
       $ ./uname.exe -r      => 1.0.11(0.46/3/2)
       $ ./uname.exe -v      => 2008-08-25 23:40
     There is no point in imitating this behaviour.  */
  if (have_version)
    {
      if (version.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
	  /* Windows NT or newer.  */
	  if (version.dwMajorVersion <= 4)
	    sprintf (buf->release, "Windows NT %u.%u",
		     (unsigned int) version.dwMajorVersion,
		     (unsigned int) version.dwMinorVersion);
	  else if (version.dwMajorVersion == 5)
	    switch (version.dwMinorVersion)
	      {
	      case 0:
		strcpy (buf->release, "Windows 2000");
		break;
	      case 1:
		strcpy (buf->release, "Windows XP");
		break;
	      case 2:
		strcpy (buf->release, "Windows Server 2003");
		break;
	      default:
		strcpy (buf->release, "Windows");
		break;
	      }
	  else if (version.dwMajorVersion == 6)
	    {
	      OSVERSIONINFOEX versionex;

	      versionex.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);
	      if (GetVersionEx ((OSVERSIONINFO *) &versionex)
		  && versionex.wProductType != VER_NT_WORKSTATION)
		strcpy (buf->release, "Windows Server 2008");
	      else
		switch (version.dwMinorVersion)
		  {
		  case 0:
		    strcpy (buf->release, "Windows Vista");
		    break;
		  case 1:
		  default: /* versions not yet known */
		    strcpy (buf->release, "Windows 7");
		    break;
		  }
	    }
	  else
	    strcpy (buf->release, "Windows");
	}
      else
	{
	  /* Windows 95/98/ME.  */
	  sprintf (buf->release, "Windows %s", super_version);
	}
      strcpy (buf->version, version.szCSDVersion);
    }
  else
    {
      strcpy (buf->release, "Windows");
      strcpy (buf->version, "");
    }

  /* Fill in machine.  */
  {
    SYSTEM_INFO info;

    GetSystemInfo (&info);
    /* Check for Windows NT, since the info.wProcessorLevel is
       garbage on Windows 95. */
    if (have_version && version.dwPlatformId == VER_PLATFORM_WIN32_NT)
      {
	/* Windows NT or newer.  */
	switch (info.wProcessorArchitecture)
	  {
	  case PROCESSOR_ARCHITECTURE_AMD64:
	    strcpy (buf->machine, "x86_64");
	    break;
	  case PROCESSOR_ARCHITECTURE_IA64:
	    strcpy (buf->machine, "ia64");
	    break;
	  case PROCESSOR_ARCHITECTURE_INTEL:
	    strcpy (buf->machine, "i386");
	    if (info.wProcessorLevel >= 3)
	      buf->machine[1] =
		'0' + (info.wProcessorLevel <= 6 ? info.wProcessorLevel : 6);
	    break;
	  default:
	    strcpy (buf->machine, "unknown");
	    break;
	  }
      }
    else
      {
	/* Windows 95/98/ME.  */
	switch (info.dwProcessorType)
	  {
	  case PROCESSOR_AMD_X8664:
	    strcpy (buf->machine, "x86_64");
	    break;
	  case PROCESSOR_INTEL_IA64:
	    strcpy (buf->machine, "ia64");
	    break;
	  default:
	    if (info.dwProcessorType % 100 == 86)
	      sprintf (buf->machine, "i%u",
		       (unsigned int) info.dwProcessorType);
	    else
	      strcpy (buf->machine, "unknown");
	    break;
	  }
      }
  }

  return 0;
}

#endif

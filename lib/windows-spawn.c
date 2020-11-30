/* Auxiliary functions for the creation of subprocesses.  Native Windows API.
   Copyright (C) 2001, 2003-2020 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2003.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "windows-spawn.h"

/* Get declarations of the native Windows API functions.  */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Get _get_osfhandle().  */
#if GNULIB_MSVC_NOTHROW
# include "msvc-nothrow.h"
#else
# include <io.h>
#endif
#include <process.h>

#include "findprog.h"
#include "xalloc.h"

/* Don't assume that UNICODE is not defined.  */
#undef STARTUPINFO
#define STARTUPINFO STARTUPINFOA
#undef CreateProcess
#define CreateProcess CreateProcessA

#define SHELL_SPECIAL_CHARS "\"\\ \001\002\003\004\005\006\007\010\011\012\013\014\015\016\017\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037*?"
#define SHELL_SPACE_CHARS " \001\002\003\004\005\006\007\010\011\012\013\014\015\016\017\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037"
char **
prepare_spawn (char **argv)
{
  size_t argc;
  char **new_argv;
  size_t i;

  /* Count number of arguments.  */
  for (argc = 0; argv[argc] != NULL; argc++)
    ;

  /* Allocate new argument vector.  */
  new_argv = XNMALLOC (1 + argc + 1, char *);

  /* Add an element upfront that can be used when argv[0] turns out to be a
     script, not a program.
     On Unix, this would be "/bin/sh". On native Windows, "sh" is actually
     "sh.exe".  We have to omit the directory part and rely on the search in
     PATH, because the mingw "mount points" are not visible inside Windows
     CreateProcess().  */
  *new_argv++ = "sh.exe";

  /* Put quoted arguments into the new argument vector.  */
  for (i = 0; i < argc; i++)
    {
      const char *string = argv[i];

      if (string[0] == '\0')
        new_argv[i] = xstrdup ("\"\"");
      else if (strpbrk (string, SHELL_SPECIAL_CHARS) != NULL)
        {
          bool quote_around = (strpbrk (string, SHELL_SPACE_CHARS) != NULL);
          size_t length;
          unsigned int backslashes;
          const char *s;
          char *quoted_string;
          char *p;

          length = 0;
          backslashes = 0;
          if (quote_around)
            length++;
          for (s = string; *s != '\0'; s++)
            {
              char c = *s;
              if (c == '"')
                length += backslashes + 1;
              length++;
              if (c == '\\')
                backslashes++;
              else
                backslashes = 0;
            }
          if (quote_around)
            length += backslashes + 1;

          quoted_string = (char *) xmalloc (length + 1);

          p = quoted_string;
          backslashes = 0;
          if (quote_around)
            *p++ = '"';
          for (s = string; *s != '\0'; s++)
            {
              char c = *s;
              if (c == '"')
                {
                  unsigned int j;
                  for (j = backslashes + 1; j > 0; j--)
                    *p++ = '\\';
                }
              *p++ = c;
              if (c == '\\')
                backslashes++;
              else
                backslashes = 0;
            }
          if (quote_around)
            {
              unsigned int j;
              for (j = backslashes; j > 0; j--)
                *p++ = '\\';
              *p++ = '"';
            }
          *p = '\0';

          new_argv[i] = quoted_string;
        }
      else
        new_argv[i] = (char *) string;
    }
  new_argv[argc] = NULL;

  return new_argv;
}

intptr_t
spawnpvech (int mode,
            const char *progname, const char * const *argv,
            const char * const *envp,
            const char *currdir,
            HANDLE stdin_handle, HANDLE stdout_handle, HANDLE stderr_handle)
{
  /* Validate the arguments.  */
  if (!(mode == P_WAIT
        || mode == P_NOWAIT
        || mode == P_DETACH
        || mode == P_OVERLAY)
      || progname == NULL || argv == NULL)
    {
      errno = EINVAL;
      return -1;
    }

  /* Implement the 'p' letter: search for PROGNAME in getenv ("PATH").  */
  const char *resolved_progname =
    find_in_given_path (progname, getenv ("PATH"), false);
  if (resolved_progname == NULL)
    return -1;

  /* Compose the command.
     Just concatenate the argv[] strings, separated by spaces.  */
  char *command;
  {
    /* Determine the size of the needed block of memory.  */
    size_t total_size = 0;
    const char * const *ap;
    const char *p;
    for (ap = argv; (p = *ap) != NULL; ap++)
      total_size += strlen (p) + 1;
    size_t command_size = (total_size > 0 ? total_size : 1);
    command = (char *) malloc (command_size);
    if (command == NULL)
      goto out_of_memory_1;
    if (total_size > 0)
      {
        char *cp = command;
        for (ap = argv; (p = *ap) != NULL; ap++)
          {
            size_t size = strlen (p) + 1;
            memcpy (cp, p, size - 1);
            cp += size;
            cp[-1] = ' ';
          }
        cp[-1] = '\0';
      }
    else
      *command = '\0';
  }

  /* Copy *ENVP into a contiguous block of memory.  */
  char *envblock;
  if (envp == NULL)
    envblock = NULL;
  else
   retry:
    {
      /* Guess the size of the needed block of memory.
         The guess will be exact if other threads don't make modifications.  */
      size_t total_size = 0;
      const char * const *ep;
      const char *p;
      for (ep = envp; (p = *ep) != NULL; ep++)
        total_size += strlen (p) + 1;
      size_t envblock_size = total_size;
      envblock = (char *) malloc (envblock_size + 1);
      if (envblock == NULL)
        goto out_of_memory_2;
      size_t envblock_used = 0;
      for (ep = envp; (p = *ep) != NULL; ep++)
        {
          size_t size = strlen (p) + 1;
          if (envblock_used + size > envblock_size)
            {
              /* Other threads did modifications.  Need more memory.  */
              envblock_size += envblock_size / 2;
              if (envblock_used + size > envblock_size)
                envblock_size = envblock_used + size;

              char *new_envblock = (char *) realloc (envblock, envblock_size + 1);
              if (new_envblock == NULL)
                goto out_of_memory_3;
              envblock = new_envblock;
            }
          memcpy (envblock + envblock_used, p, size);
          envblock_used += size;
          if (envblock[envblock_used - 1] != '\0')
            {
              /* Other threads did modifications.  Restart.  */
              free (envblock);
              goto retry;
            }
        }
      envblock[envblock_used] = '\0';
    }

  /* CreateProcess
     <https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa>  */
  /* Regarding handle inheritance, see
     <https://docs.microsoft.com/en-us/windows/win32/sysinfo/handle-inheritance>  */
  /* <https://docs.microsoft.com/en-us/windows/win32/procthread/process-creation-flags>  */
  DWORD flags = (mode == P_DETACH ? DETACHED_PROCESS : 0);
  /* STARTUPINFO
     <https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-startupinfoa>  */
  STARTUPINFO sinfo;
  sinfo.cb = sizeof (STARTUPINFO);
  sinfo.lpReserved = NULL;
  sinfo.lpDesktop = NULL;
  sinfo.lpTitle = NULL;
  sinfo.dwFlags = STARTF_USESTDHANDLES;
  sinfo.hStdInput = stdin_handle;
  sinfo.hStdOutput = stdout_handle;
  sinfo.hStdError = stderr_handle;

  char *hblock = NULL;
#if 0
  sinfo.cbReserved2 = 0;
  sinfo.lpReserved2 = NULL;
#else
  /* On newer versions of Windows, more file descriptors / handles than the
     first three can be passed.
     The format is as follows: Let N be an exclusive upper bound for the file
     descriptors to be passed. Two arrays are constructed in memory:
       - flags[0..N-1], of element type 'unsigned char',
       - handles[0..N-1], of element type 'HANDLE' or 'intptr_t'.
     For used entries, handles[i] is the handle, and flags[i] is a set of flags,
     a combination of:
        1 for open file descriptors,
       64 for handles of type FILE_TYPE_CHAR,
        8 for handles of type FILE_TYPE_PIPE.
     For unused entries - this includes the first three, since they are already
     passed above -, handles[i] is INVALID_HANDLE_VALUE and flags[i] is zero.
     lpReserved2 now is a pointer to the concatenation (without padding) of:
       - an 'unsigned int' whose value is N,
       - the contents of the flags[0..N-1] array,
       - the contents of the handles[0..N-1] array.
     cbReserved2 is the size (in bytes) of the object at lpReserved2.  */
  {
    /* _getmaxstdio
       <https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/getmaxstdio>
       Default value is 512.  */
    unsigned int fdmax;
    for (fdmax = _getmaxstdio (); fdmax > 0; fdmax--)
      {
        unsigned int fd = fdmax - 1;
        /* _get_osfhandle
           <https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/get-osfhandle>  */
        HANDLE handle = (HANDLE) _get_osfhandle (fd);
        if (handle != INVALID_HANDLE_VALUE)
          {
            DWORD hflags;
            /* GetHandleInformation
               <https://docs.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-gethandleinformation>  */
            if (GetHandleInformation (handle, &hflags))
              {
                if ((hflags & HANDLE_FLAG_INHERIT) != 0)
                  /* fd denotes an inheritable descriptor.  */
                  break;
              }
          }
      }
    if (fdmax > 0)
      {
        sinfo.cbReserved2 =
          sizeof (unsigned int)
          + fdmax * sizeof (unsigned char)
          + fdmax * sizeof (HANDLE);
        /* Add some padding, so that we can work with a properly HANDLE array.  */
        hblock = (char *) malloc (sinfo.cbReserved2 + (sizeof (HANDLE) - 1));
        if (hblock == NULL)
          goto out_of_memory_3;
        * (unsigned int *) hblock = fdmax;
        unsigned char *flags = (unsigned char *) (hblock + sizeof (unsigned int));
        char *handles = (char *) (flags + fdmax);
        HANDLE *handles_aligned =
          (HANDLE *) (((uintptr_t) handles + (sizeof (HANDLE) - 1))
                      & - (uintptr_t) sizeof (HANDLE));

        unsigned int fd;
        for (fd = 0; fd < fdmax; fd++)
          {
            flags[fd] = 0;
            handles_aligned[fd] = INVALID_HANDLE_VALUE;
            /* The first three are already passed above.  */
            if (fd >= 3)
              {
                /* _get_osfhandle
                   <https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/get-osfhandle>  */
                HANDLE handle = (HANDLE) _get_osfhandle (fd);
                if (handle != INVALID_HANDLE_VALUE)
                  {
                    DWORD hflags;
                    /* GetHandleInformation
                       <https://docs.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-gethandleinformation>  */
                    if (GetHandleInformation (handle, &hflags))
                      {
                        if ((hflags & HANDLE_FLAG_INHERIT) != 0)
                          {
                            /* fd denotes an inheritable descriptor.  */
                            /* On Microsoft Windows, it would be sufficient to
                               set flags[fd] = 1.  But on ReactOS or Wine,
                               adding the bit that indicates the handle type
                               may be necessary.  So, just do it everywhere.  */
                            switch (GetFileType (handle))
                              {
                              case FILE_TYPE_CHAR:
                                flags[fd] = 64 | 1;
                                break;
                              case FILE_TYPE_PIPE:
                                flags[fd] = 8 | 1;
                                break;
                              default:
                                flags[fd] = 1;
                                break;
                              }
                            handles_aligned[fd] = handle;
                          }
                      }
                  }
              }
          }

        if (handles != (char *) handles_aligned)
          memmove (handles, (char *) handles_aligned, fdmax * sizeof (HANDLE));
        sinfo.lpReserved2 = (BYTE *) hblock;
      }
    else
      {
        sinfo.cbReserved2 = 0;
        sinfo.lpReserved2 = NULL;
      }
  }
#endif

  PROCESS_INFORMATION pinfo;
  if (!CreateProcess (resolved_progname, command, NULL, NULL, TRUE,
                      flags, envblock, currdir, &sinfo, &pinfo))
    {
      DWORD error = GetLastError ();

      if (hblock != NULL)
        free (hblock);
      if (envblock != NULL)
        free (envblock);
      free (command);
      if (resolved_progname != progname)
        free ((char *) resolved_progname);

      /* Some of these errors probably cannot happen.  But who knows...  */
      switch (error)
        {
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
        case ERROR_BAD_PATHNAME:
        case ERROR_BAD_NET_NAME:
        case ERROR_INVALID_NAME:
        case ERROR_DIRECTORY:
          errno = ENOENT;
          break;

        case ERROR_ACCESS_DENIED:
        case ERROR_SHARING_VIOLATION:
          errno = EACCES;
          break;

        case ERROR_OUTOFMEMORY:
          errno = ENOMEM;
          break;

        case ERROR_BUFFER_OVERFLOW:
        case ERROR_FILENAME_EXCED_RANGE:
          errno = ENAMETOOLONG;
          break;

        default:
          errno = EINVAL;
          break;
        }

      return -1;
    }

  if (pinfo.hThread)
    CloseHandle (pinfo.hThread);
  if (hblock != NULL)
    free (hblock);
  if (envblock != NULL)
    free (envblock);
  free (command);
  if (resolved_progname != progname)
    free ((char *) resolved_progname);

  switch (mode)
    {
    case P_WAIT:
      {
        /* Wait until it terminates.  Then get its exit status code.  */
        switch (WaitForSingleObject (pinfo.hProcess, INFINITE))
          {
          case WAIT_OBJECT_0:
            break;
          case WAIT_FAILED:
            errno = ECHILD;
            return -1;
          default:
            abort ();
          }

        DWORD exit_code;
        if (!GetExitCodeProcess (pinfo.hProcess, &exit_code))
          {
            errno = ECHILD;
            return -1;
          }
        CloseHandle (pinfo.hProcess);
        return exit_code;
      }

    case P_NOWAIT:
      /* Return pinfo.hProcess, not pinfo.dwProcessId.  */
      return (intptr_t) pinfo.hProcess;

    case P_DETACH:
    case P_OVERLAY:
      CloseHandle (pinfo.hProcess);
      return 0;

    default:
      /* Already checked above.  */
      abort ();
    }

  /*NOTREACHED*/
#if 0
 out_of_memory_4:
  if (hblock != NULL)
    free (hblock);
#endif
 out_of_memory_3:
  if (envblock != NULL)
    free (envblock);
 out_of_memory_2:
  free (command);
 out_of_memory_1:
  if (resolved_progname != progname)
    free ((char *) resolved_progname);
  errno = ENOMEM;
  return -1;
}

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

#ifndef _WINDOWS_SPAWN_H
#define _WINDOWS_SPAWN_H

#include <stdint.h>

/* Get declarations of the native Windows API functions.  */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* Prepares an argument vector before calling spawn().
   Note that spawn() does not by itself call the command interpreter
     (getenv ("COMSPEC") != NULL ? getenv ("COMSPEC") :
      ({ OSVERSIONINFO v; v.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
         GetVersionEx(&v);
         v.dwPlatformId == VER_PLATFORM_WIN32_NT;
      }) ? "cmd.exe" : "command.com").
   Instead it simply concatenates the arguments, separated by ' ', and calls
   CreateProcess().  We must quote the arguments since Windows CreateProcess()
   interprets characters like ' ', '\t', '\\', '"' (but not '<' and '>') in a
   special way:
   - Space and tab are interpreted as delimiters. They are not treated as
     delimiters if they are surrounded by double quotes: "...".
   - Unescaped double quotes are removed from the input. Their only effect is
     that within double quotes, space and tab are treated like normal
     characters.
   - Backslashes not followed by double quotes are not special.
   - But 2*n+1 backslashes followed by a double quote become
     n backslashes followed by a double quote (n >= 0):
       \" -> "
       \\\" -> \"
       \\\\\" -> \\"
   - '*', '?' characters may get expanded through wildcard expansion in the
     callee: By default, in the callee, the initialization code before main()
     takes the result of GetCommandLine(), wildcard-expands it, and passes it
     to main(). The exceptions to this rule are:
       - programs that inspect GetCommandLine() and ignore argv,
       - mingw programs that have a global variable 'int _CRT_glob = 0;',
       - Cygwin programs, when invoked from a Cygwin program.
 */
extern char ** prepare_spawn (char **argv);

/* Creates a subprocess.
   MODE is either P_WAIT or P_NOWAIT.
   PROGNAME is the program to invoke.
   ARGV is the NULL-terminated array of arguments, ARGV[0] being PROGNAME by
   convention.
   ENVP is the NULL-terminated set of environment variable assignments, or NULL
   to inherit the initial environ variable assignments from the caller and
   ignore all calls to putenv(), setenv(), unsetenv() done in the caller.
   CURRDIR is the directory in which to start the program, or NULL to inherit
   the working directory from the caller.
   STDIN_HANDLE, STDOUT_HANDLE, STDERR_HANDLE are the handles to use for the
   first three file descriptors in the callee process.
   Returns
     - 0 for success (if MODE is P_WAIT), or
     - a handle that be passed to _cwait (on Windows) or waitpid (on OS/2), or
     - -1 upon error, with errno set.
 */
extern intptr_t spawnpvech (int mode,
                            const char *progname, const char * const *argv,
                            const char * const *envp,
                            const char *currdir,
                            HANDLE stdin_handle, HANDLE stdout_handle,
                            HANDLE stderr_handle);

#endif /* _WINDOWS_SPAWN_H */

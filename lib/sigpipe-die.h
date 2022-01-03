/* Report a SIGPIPE signal and exit.
   Copyright (C) 2008-2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2008.  */

/* SIGPIPE is the signal sent to a process calling write() on a pipe with no
   readers.  Together with the signal, the write() call is terminated with
   return value -1, errno = EPIPE.  Note that SIGPIPE is a *synchronous*
   signal: it occurs only during write(), without delay (unless blocked).

   The default reaction on SIGPIPE, namely terminating the process without
   an error message, is suitable for programs which only produce output to
   standard output and don't have side effects.

   When a program has side effects, other than writing to standard output, the
   suitable behaviour is either
     (a) to exit with an error message
   or - in certain cases, for example when writing to subprocesses -
     (b) to continue the operation without writing to the pipe/socket with
         no readers.

   This file provides support for (a).
   For (b), the program needs to know which of the output file descriptors
   has no readers.  This is usually implemented by blocking the SIGPIPE signal
   and handling an EPIPE error indicator in all affected library calls
   (write(), send(), fwrite(), fflush(), fclose(), etc.).  */

#ifndef _SIGPIPE_DIE_H
#define _SIGPIPE_DIE_H

#ifdef __cplusplus
extern "C" {
#endif


/* Emit an error message indicating a SIGPIPE signal, and terminate the
   process with an error code.  */
/*extern*/ _Noreturn void sigpipe_die (void);

/* Install a SIGPIPE handler that invokes PREPARE_DIE and then emits an
   error message and exits.  PREPARE_DIE may be NULL, meaning a no-op.  */
extern void install_sigpipe_die_handler (void (*prepare_die) (void));


#ifdef __cplusplus
}
#endif


#endif /* _SIGPIPE_DIE_H */

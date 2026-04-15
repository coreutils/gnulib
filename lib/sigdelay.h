/* Delaying the delivery of signals to the current process.
   Copyright (C) 2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible.  */

#ifndef _SIGDELAY_H
#define _SIGDELAY_H

#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

/* While pthread_sigmask() can block the delivery of a signal to a particular
   thread and thus — in the case of a process with a single thread — actually
   delay the signal, POSIX lacks a facility for blocking or delaying a signal
   process-wide, i.e. for all threads of the process at once.

   The sigdelay() function is such a facility.
   It assumes that the signal handler for the affected signals does not change
   between matching sigdelay (SIG_BLOCK, ...) and sigdelay (SIG_UNBLOCK, ...)
   invocations.  */

/* With HOW = SIG_BLOCK, this function adds the signals from SET to the set
   of delayed signals.
   With HOW = SIG_UNBLOCK, this function removes the signals from SET from the
   set of delayed signals.
   This function works cumulatively, that is, when a signal was added N times,
   it also needs to be removed N times in order to restore the initial state.
   If OLD_SET is non-NULL, this function also stores the previous set of
   delayed signals in *OLD_SET.
   Returns 0 if successful, or -1 with errno set in case of failure.  */
extern int sigdelay (int how, const sigset_t *restrict set,
                     sigset_t *restrict old_set);

#ifdef __cplusplus
}
#endif

#endif /* _SIGDELAY_H */

/* Waiting for a subprocess to finish.
   Copyright (C) 2001-2003 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2001.

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
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _WAIT_PROCESS_H
#define _WAIT_PROCESS_H

/* Get pid_t.  */
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>

#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


/* Wait for a subprocess to finish.  Return its exit code.
   If it didn't terminate correctly, exit if exit_on_error is true, otherwise
   return 127.  */
extern int wait_subprocess (pid_t child, const char *progname,
			    bool null_stderr,
			    bool slave_process, bool exit_on_error);

/* Register a subprocess as being a slave process.  This means that the
   subprocess will be terminated when its creator receives a catchable fatal
   signal or exits normally.  Registration ends when wait_subprocess()
   notices that the subprocess has exited.  */
extern void register_slave_subprocess (pid_t child);


#ifdef __cplusplus
}
#endif


#endif /* _WAIT_PROCESS_H */

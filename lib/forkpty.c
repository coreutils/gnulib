/* Fork a child process attached to the slave of a pseudo-terminal.
   Copyright (C) 2010-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <pty.h>

#if HAVE_FORKPTY

/* Provide a wrapper with the prototype of glibc-2.8 and newer.  */
# undef forkpty
int
rpl_forkpty (int *amaster, char *name, struct termios const *termp,
         struct winsize const *winp)
{
  /* Cast away const, for implementations with weaker prototypes.  */
  return forkpty (amaster, name, (struct termios *) termp,
                  (struct winsize *) winp);
}

#else /* AIX 5.1, HP-UX 11, IRIX 6.5, Solaris 10, mingw */

# include <pty.h>
# include <unistd.h>

extern int login_tty (int slave_fd);

int
forkpty (int *amaster, char *name,
         const struct termios *termp, const struct winsize *winp)
{
  int master, slave, pid;

  if (openpty (&master, &slave, name, termp, winp) == -1)
    return -1;

  switch (pid = fork ())
    {
    case -1:
      close (master);
      close (slave);
      return -1;

    case 0:
      /* Child.  */
      close (master);
      if (login_tty (slave))
        _exit (1);
      return 0;

    default:
      /* Parent.  */
      *amaster = master;
      close (slave);
      return pid;
    }
}

#endif

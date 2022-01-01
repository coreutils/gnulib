/* Open a pseudo-terminal.
   Copyright (C) 2010-2022 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include <pty.h>

#if HAVE_OPENPTY

/* Provide a wrapper with the prototype of glibc-2.8 and newer.  */
# undef openpty
int
rpl_openpty (int *amaster, int *aslave, char *name,
             struct termios const *termp, struct winsize const *winp)
{
  /* Cast away const, for implementations with weaker prototypes.  */
  return openpty (amaster, aslave, name, (struct termios *) termp,
                  (struct winsize *) winp);
}

#elif defined _WIN32 && !defined __CYGWIN__ /* mingw */

# include <errno.h>

int
openpty (_GL_UNUSED int *amaster, _GL_UNUSED int *aslave,
         _GL_UNUSED char *name,
         _GL_UNUSED struct termios const *termp,
         _GL_UNUSED struct winsize const *winp)
{
  /* Mingw lacks pseudo-terminals altogether.  */
  errno = ENOSYS;
  return -1;
}

#else /* AIX 5.1, HP-UX 11, IRIX 6.5, Solaris 10 */

# include <fcntl.h>
# include <stdlib.h>
# include <string.h>
# include <sys/ioctl.h>
# include <termios.h>
# include <unistd.h>
# if defined __sun || defined __hpux /* Solaris, HP-UX */
#  include <stropts.h>
# endif

int
openpty (int *amaster, int *aslave, char *name,
         struct termios const *termp, struct winsize const *winp)
{
  int master;
  char *slave_name;
  int slave;

# if HAVE__GETPTY /* IRIX */

  slave_name = _getpty (&master, O_RDWR, 0622, 0);
  if (slave_name == NULL)
    return -1;

# else /* AIX 5.1, HP-UX 11, Solaris 10, mingw */

  /* This call uses the 'posix_openpt' module.  */
  master = posix_openpt (O_RDWR | O_NOCTTY);
  if (master < 0)
    return -1;

# endif

  /* This call does not require a dependency to the 'grantpt' module,
     because AIX, HP-UX, IRIX, Solaris all have the grantpt() function.  */
  if (grantpt (master))
    goto fail;

  /* This call does not require a dependency to the 'unlockpt' module,
     because AIX, HP-UX, IRIX, Solaris all have the unlockpt() function.  */
  if (unlockpt (master))
    goto fail;

# if !HAVE__GETPTY /* !IRIX */
  slave_name = ptsname (master);
  if (slave_name == NULL)
    goto fail;
# endif

  slave = open (slave_name, O_RDWR | O_NOCTTY);
  if (slave == -1)
    goto fail;

# if defined __sun || defined __hpux /* Solaris, HP-UX */
  if (ioctl (slave, I_PUSH, "ptem") < 0
      || ioctl (slave, I_PUSH, "ldterm") < 0
#  if defined __sun
      || ioctl (slave, I_PUSH, "ttcompat") < 0
#  endif
     )
    {
      close (slave);
      goto fail;
    }
# endif

  /* XXX Should we ignore errors here?  */
  if (termp)
    tcsetattr (slave, TCSAFLUSH, termp);
  if (winp)
    ioctl (slave, TIOCSWINSZ, winp);

  *amaster = master;
  *aslave = slave;
  if (name != NULL)
    strcpy (name, slave_name);

  return 0;

 fail:
  close (master);
  return -1;
}

#endif

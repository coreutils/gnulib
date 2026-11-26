/* Abstract interface of directory information per file descriptor.
   Copyright (C) 2006-2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible.  */

#include <config.h>

/* Specification.  */
#include "dirinfo-abstract.h"

#include <errno.h>
#include <stddef.h>
#include <unistd.h>

static const struct dirinfo_vtable_t *impl_vtable /* = NULL */;

void
_gl_set_dirinfo_impl (const struct dirinfo_vtable_t *vtable)
{
  impl_vtable = vtable;
}

void
_gl_unregister_fd (int fd)
{
  if (impl_vtable != NULL)
    impl_vtable->unregister_fd (fd);
}

int
_gl_register_dup (int oldfd, int newfd)
{
  if (impl_vtable != NULL)
    return impl_vtable->register_dup (oldfd, newfd);
  else
    return newfd;
}

const char *
_gl_directory_name (int fd)
{
  if (impl_vtable != NULL)
    return impl_vtable->directory_name (fd);
  else
    {
      if (0 <= fd)
        {
          if (dup2 (fd, fd) == fd)
            errno = ENOTDIR;
        }
      else
        errno = EBADF;
      return NULL;
    }
}

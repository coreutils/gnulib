/* Extension of the global locale with LC_MESSAGES.
   Copyright (C) 2025 Free Software Foundation, Inc.

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
#include "setlocale-messages.h"

#include <stddef.h>
#include <string.h>

/* The system does not store an LC_MESSAGES locale category.  Do it here.  */
static char lc_messages_name[64] = "C";

const char *
setlocale_messages (const char *name)
{
  if (name != NULL)
    {
      lc_messages_name[sizeof (lc_messages_name) - 1] = '\0';
      strncpy (lc_messages_name, name, sizeof (lc_messages_name) - 1);
    }
  return lc_messages_name;
}

const char *
setlocale_messages_null (void)
{
  /* This implementation is multithread-safe, assuming no other thread changes
     the LC_MESSAGES locale category.  */
  return lc_messages_name;
}

/* Invalid parameter handler for MSVC runtime libraries.
   Copyright (C) 2011 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <config.h>

/* Specification.  */
#include "msvc-inval.h"

#if HAVE_MSVC_INVALID_PARAMETER_HANDLER

/* Get declarations of the Win32 API functions.  */
# define WIN32_LEAN_AND_MEAN
# include <windows.h>

# if defined _MSC_VER

static void cdecl
gl_msvc_invalid_parameter_handler (const wchar_t *expression,
                                   const wchar_t *function,
                                   const wchar_t *file,
                                   unsigned int line,
                                   uintptr_t dummy)
{
  RaiseException (STATUS_GNULIB_INVALID_PARAMETER, 0, 0, NULL);
}

# else

jmp_buf gl_msvc_inval_restart;
int gl_msvc_inval_restart_valid;

static void cdecl
gl_msvc_invalid_parameter_handler (const wchar_t *expression,
                                   const wchar_t *function,
                                   const wchar_t *file,
                                   unsigned int line,
                                   uintptr_t dummy)
{
  if (gl_msvc_inval_restart_valid)
    longjmp (gl_msvc_inval_restart, 1);
  else
    /* An invalid parameter notification from outside the gnulib code.
       Give the caller a chance to intervene.  */
    RaiseException (STATUS_GNULIB_INVALID_PARAMETER, 0, 0, NULL);
}

# endif

static int gl_msvc_inval_initialized /* = 0 */;

void
gl_msvc_inval_ensure_handler (void)
{
  if (gl_msvc_inval_initialized == 0)
    {
      _set_invalid_parameter_handler (gl_msvc_invalid_parameter_handler);
      gl_msvc_inval_initialized = 1;
    }
}

#endif

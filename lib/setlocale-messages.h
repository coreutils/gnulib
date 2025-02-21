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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

#ifndef _SETLOCALE_MESSAGES_H
#define _SETLOCALE_MESSAGES_H

/* This file is only relevant on platforms that lack LC_MESSAGES, namely
   native Windows.  */

#ifdef __cplusplus
extern "C" {
#endif

/* setlocale_messages (NAME) is like setlocale (LC_MESSAGES, NAME).  */
extern const char *setlocale_messages (const char *name);

/* setlocale_messages_null () is like setlocale (LC_MESSAGES, NULL), except that
   it is guaranteed to be multithread-safe.  */
extern const char *setlocale_messages_null (void);

#ifdef __cplusplus
}
#endif

#endif /* _SETLOCALE_MESSAGES_H */

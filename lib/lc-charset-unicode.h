/* Conversion between the current locale's character encoding and Unicode.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

/* This facility is only needed on specific platforms.  */
#if GL_CHAR32_T_IS_UNICODE && GL_CHAR32_T_VS_WCHAR_T_NEEDS_CONVERSION

# include <wchar.h>
# include <uchar.h>

/* Returns WC (must be != 0) as a Unicode character,
   or 0 in case it cannot be converted.  */
extern char32_t locale_encoding_to_unicode (wchar_t wc);

/* Returns UC (must be != 0) as a wide character,
   or 0 in case it cannot be converted.  */
extern wchar_t unicode_to_locale_encoding (char32_t uc);

#endif

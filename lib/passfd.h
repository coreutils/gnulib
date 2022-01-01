/* passfd.h -- definitions and prototypes for passfd.c

   Copyright (C) 2011-2022 Free Software Foundation, Inc.

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

#ifndef PASSFD_H_
#define PASSFD_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

extern int sendfd (int sock, int fd);
extern int recvfd (int sock, int flags);

#ifdef __cplusplus
}
#endif

#endif

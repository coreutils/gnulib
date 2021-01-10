/* Immutable data.

   Copyright (C) 2021 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2021.  */

#include <config.h>

/* Specification.  */
#include "immutable.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#if IMMUTABLE_EFFECTIVE
/* Real implementation.  */

/* Get CHAR_BIT.  */
# include <limits.h>

/* Get intptr_t, uintptr_t.  */
# include <stdint.h>

# include <stdio.h>

/* Declare getpagesize().  */
# include <unistd.h>
/* On HP-UX, getpagesize exists, but it is not declared in <unistd.h> even if
   the compiler options -D_HPUX_SOURCE -D_XOPEN_SOURCE=600 are used.  */
# ifdef __hpux
extern
#  ifdef __cplusplus
       "C"
#  endif
       int getpagesize (void);
# endif

/* Declare mmap(), mprotect().  */
# include <sys/types.h>
# include <sys/mman.h>

/* Declare open().  */
# include <unistd.h>
# include <fcntl.h>

# include "glthread/lock.h"


/* ================= Back end of the malloc implementation ================= */

/* The memory page size.
   Once it is initialized, a power of 2.  Typically 4096 or 8192.  */
static uintptr_t pagesize;

/* Initializes pagesize.  */
static void
init_pagesize (void)
{
  /* Simultaneous execution of this initialization in multiple threads is OK. */
  pagesize = getpagesize ();
}


/* Variables needed for obtaining memory pages via mmap().  */
static int file_fd;
static long file_length;

/* Initialization of these variables. */
static void
do_init_mmap_file (void)
{
  char filename[100];
  sprintf (filename, "%s/glimmdata-%d-%ld", "/tmp", getpid (), random ());
  file_fd = open (filename, O_CREAT | O_TRUNC | O_RDWR | O_CLOEXEC, 0700);
  if (file_fd < 0)
    {
      fprintf (stderr, "glimm: Cannot open %s!\n", filename);
      abort ();
    }
  /* Remove the file from the file system as soon as possible, to make
     sure there is no leftover after this process terminates or crashes.  */
  unlink (filename);

  file_length = 0;
}

/* Once-only initializer for these variables.  */
gl_once_define (static, for_mmap_once)

static inline void
init_mmap_file (void)
{
  /* Use a once-only initializer here, since simultaneous execution of
     do_init_mmap_file() in multiple threads must be avoided.  */
  gl_once (for_mmap_once, do_init_mmap_file);
}


/* Size of the (page-aligned) header that links the writable mapping
   and the read-only mapping together.  */
# define SHARED_LINK_HEADER_SIZE \
    (INTPTR_WIDTH / CHAR_BIT)   /* = sizeof (void *) */

/* Allocates a contiguous set of pages of memory.
   size > 0, must be a multiple of pagesize.
   Returns a multiple of PAGESIZE, or 0 upon failure.  */
static uintptr_t
alloc_pages (size_t size)
{
  /* Extend the file by size/pagesize pages.  */
  long new_file_length = file_length + size;
  if (ftruncate (file_fd, new_file_length) < 0)
    {
      fprintf (stderr, "glimm: Cannot extend backing file!\n");
      return 0;
    }
  /* Create separate writable mapping and read-only mapping.  */
  char *mem_w = (char *) mmap (NULL, size, PROT_READ | PROT_WRITE,
                               MAP_SHARED, file_fd, file_length);
  char *mem_r = (char *) mmap (NULL, size, PROT_READ,
                               MAP_SHARED, file_fd, file_length);
  if (mem_w == (char *)(-1) || mem_r == (char *)(-1))
    {
      if (mem_w != (char *)(-1))
        munmap (mem_w, size);
      if (mem_r != (char *)(-1))
        munmap (mem_r, size);
      return 0;
    }
  file_length = new_file_length;

  /* Link the two memory areas together.  */
  ((intptr_t *) mem_w)[0] = mem_r - mem_w;
  return (uintptr_t) mem_w;
}

/* Frees a contiguous set of pages of memory, returned by alloc_pages.
   size > 0, must be a multiple of pagesize.  */
static void
free_pages (uintptr_t pages, size_t size)
{
  pages -= SHARED_LINK_HEADER_SIZE;
  if ((pages & (pagesize - 1)) != 0)
    abort ();
  char *mem_w = (char *) pages;
  char *mem_r = mem_w + ((intptr_t *) mem_w)[0];
  if (munmap (mem_w, size) < 0)
    abort ();
  if (munmap (mem_r, size) < 0)
    abort ();
}

/* Cygwin defines PAGESIZE in <limits.h>.  */
# undef PAGESIZE

/* ======================= Instantiate the front end ======================= */

# define PAGESIZE pagesize
/* On Cygwin and Linux/PowerPC, PAGESIZE is 65536.  On macOS 11, it is 16384.
   On all other platforms, it is either 4096 or 8192.  */
# if defined __CYGWIN__ || (defined __linux__ && defined __powerpc__)
#  define PAGESIZE_MAX 65536
# else
#  define PAGESIZE_MAX 16384
# endif

# define ALLOC_PAGES alloc_pages
# define FREE_PAGES free_pages
# define ALIGNMENT sizeof (void *)
# define PAGE_RESERVED_HEADER_SIZE SHARED_LINK_HEADER_SIZE

# include "ssfmalloc.h"


void *
immmalloc (size_t size)
{
  /* Initializations.  */
  if (!pagesize)
    {
      init_mmap_file ();
      init_pagesize ();
    }

  void *writable_pointer = (void *) allocate_block (size);
  if (writable_pointer == NULL)
    errno = ENOMEM;
  return writable_pointer;
}

const void *
immfreeze (void *writable_pointer)
{
  uintptr_t mem_w = (uintptr_t) writable_pointer & -(intptr_t)pagesize;
  return (void *) ((uintptr_t) writable_pointer + ((intptr_t *) mem_w)[0]);
}

void
immfree (const void *readonly_pointer)
{
  uintptr_t mem_r = (uintptr_t) readonly_pointer & -(intptr_t)pagesize;
  free_block ((uintptr_t) readonly_pointer - ((intptr_t *) mem_r)[0]);
}

#else
/* Dummy implementation.  */

void *
immmalloc (size_t size)
{
  void *p = malloc (size);
  if (p == NULL)
    errno = ENOMEM;
  return p;
}

const void *
immfreeze (void *writable_pointer)
{
  return writable_pointer;
}

void
immfree (const void *readonly_pointer)
{
  void *writable_pointer = (void *) readonly_pointer;
  free (writable_pointer);
}

#endif


const char *
immstrdup (const char *string)
{
  size_t size = strlen (string) + 1;
  void *wp = immmalloc (size);
  memcpy (wp, string, size);
  return (const char *) immfreeze (wp);
}

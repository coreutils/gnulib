/* Stack overflow handling.

   Copyright (C) 2002, 2004, 2006, 2008-2020 Free Software Foundation, Inc.

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

/* Written by Paul Eggert.  */

/* NOTES:

   A program that uses alloca, dynamic arrays, or large local
   variables may extend the stack by more than a page at a time.  If
   so, when the stack overflows the operating system may not detect
   the overflow until the program uses the array, and this module may
   incorrectly report a program error instead of a stack overflow.

   To avoid this problem, allocate only small objects on the stack; a
   program should be OK if it limits single allocations to a page or
   less.  Allocate larger arrays in static storage, or on the heap
   (e.g., with malloc).  Yes, this is a pain, but we don't know of any
   better solution that is portable.

   No attempt has been made to deal with multithreaded applications.  */

#include <config.h>

#include "c-stack.h"

#include "gettext.h"
#define _(msgid) gettext (msgid)

#include <errno.h>
#include <inttypes.h>

#include <signal.h>
#if ! HAVE_STACK_T && ! defined stack_t
typedef struct sigaltstack stack_t;
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* Pre-2008 POSIX declared ucontext_t in ucontext.h instead of signal.h.  */
#if HAVE_UCONTEXT_H
# include <ucontext.h>
#endif

#include <unistd.h>

#if DEBUG
# include <stdio.h>
#endif

/* Use libsigsegv only if needed; kernels like Solaris can detect
   stack overflow without the overhead of an external library.  */
#define USE_LIBSIGSEGV (HAVE_XSI_STACK_OVERFLOW_HEURISTIC && HAVE_LIBSIGSEGV)

#if USE_LIBSIGSEGV
# include <sigsegv.h>
/* libsigsegv 2.6 through 2.8 have a bug where some architectures use
   more than the Linux default of an 8k alternate stack when deciding
   if a fault was caused by stack overflow.  */
# if LIBSIGSEGV_VERSION <= 0x0208 && SIGSTKSZ < 16384
#  undef SIGSTKSZ
# endif
#endif
#ifndef SIGSTKSZ
# define SIGSTKSZ 16384
#endif

#include "exitfail.h"
#include "ignore-value.h"
#include "intprops.h"
#include "getprogname.h"

#if defined SA_ONSTACK && defined SA_SIGINFO
# define SIGINFO_WORKS 1
#else
# define SIGINFO_WORKS 0
# ifndef SA_ONSTACK
#  define SA_ONSTACK 0
# endif
#endif

/* The user-specified action to take when a SEGV-related program error
   or stack overflow occurs.  */
static _GL_ASYNC_SAFE void (* volatile segv_action) (int);

/* Translated messages for program errors and stack overflow.  Do not
   translate them in the signal handler, since gettext is not
   async-signal-safe.  */
static char const * volatile program_error_message;
static char const * volatile stack_overflow_message;

#if (USE_LIBSIGSEGV                                           \
     || (HAVE_SIGALTSTACK && HAVE_DECL_SIGALTSTACK            \
         && HAVE_STACK_OVERFLOW_HANDLING))

/* Output an error message, then exit with status EXIT_FAILURE if it
   appears to have been a stack overflow, or with a core dump
   otherwise.  This function is async-signal-safe.  */

static char const * volatile progname;

static _GL_ASYNC_SAFE _Noreturn void
die (int signo)
{
# if !SIGINFO_WORKS && !USE_LIBSIGSEGV
  /* We can't easily determine whether it is a stack overflow; so
     assume that the rest of our program is perfect (!) and that
     this segmentation violation is a stack overflow.  */
  signo = 0;
# endif
  segv_action (signo);
  char const *message = signo ? program_error_message : stack_overflow_message;

  /* If the message is short, write it all at once to avoid
     interleaving with other messages.  Avoid writev as it is not
     documented to be async-signal-safe.  */
  size_t prognamelen = strlen (progname);
  size_t messagelen = strlen (message);
  static char const separator[] = {':', ' '};
  char buf[SIGSTKSZ / 16 + sizeof separator];
  ptrdiff_t buflen;
  if (prognamelen + messagelen < sizeof buf - sizeof separator)
    {
      char *p = mempcpy (buf, progname, prognamelen);
      p = mempcpy (p, separator, sizeof separator);
      p = mempcpy (p, message, messagelen);
      *p++ = '\n';
      buflen = p - buf;
    }
  else
    {
      ignore_value (write (STDERR_FILENO, progname, prognamelen));
      ignore_value (write (STDERR_FILENO, separator, sizeof separator));
      ignore_value (write (STDERR_FILENO, message, messagelen));
      buf[0] = '\n';
      buflen = 1;
    }
  ignore_value (write (STDERR_FILENO, buf, buflen));

  if (! signo)
    _exit (exit_failure);
  raise (signo);
  abort ();
}

/* Storage for the alternate signal stack.  */
static union
{
  char buffer[SIGSTKSZ];
  max_align_t align;
} alternate_signal_stack;

static _GL_ASYNC_SAFE void
null_action (int signo _GL_UNUSED)
{
}

#endif /* SIGALTSTACK || LIBSIGSEGV */

#if USE_LIBSIGSEGV

/* Nonzero if general segv handler could not be installed.  */
static volatile int segv_handler_missing;

/* Handle a segmentation violation and exit if it cannot be stack
   overflow.  This function is async-signal-safe.  */

static _GL_ASYNC_SAFE int
segv_handler (void *address _GL_UNUSED, int serious)
{
# if DEBUG
  {
    char buf[1024];
    int saved_errno = errno;
    ignore_value (write (STDERR_FILENO, buf,
                         sprintf (buf, "segv_handler serious=%d\n", serious)));
    errno = saved_errno;
  }
# endif

  /* If this fault is not serious, return 0 to let the stack overflow
     handler take a shot at it.  */
  if (!serious)
    return 0;
  die (SIGSEGV);
}

/* Handle a segmentation violation that is likely to be a stack
   overflow and exit.  This function is async-signal-safe.  */

static _GL_ASYNC_SAFE _Noreturn void
overflow_handler (int emergency, stackoverflow_context_t context _GL_UNUSED)
{
# if DEBUG
  {
    char buf[1024];
    ignore_value (write (STDERR_FILENO, buf,
                         sprintf (buf, ("overflow_handler emergency=%d"
                                        " segv_handler_missing=%d\n"),
                                  emergency, segv_handler_missing)));
  }
# endif

  die ((!emergency || segv_handler_missing) ? 0 : SIGSEGV);
}

int
c_stack_action (_GL_ASYNC_SAFE void (*action) (int))
{
  segv_action = action ? action : null_action;
  program_error_message = _("program error");
  stack_overflow_message = _("stack overflow");
  progname = getprogname ();

  /* Always install the overflow handler.  */
  if (stackoverflow_install_handler (overflow_handler,
                                     alternate_signal_stack.buffer,
                                     sizeof alternate_signal_stack.buffer))
    {
      errno = ENOTSUP;
      return -1;
    }
  /* Try installing a general handler; if it fails, then treat all
     segv as stack overflow.  */
  segv_handler_missing = sigsegv_install_handler (segv_handler);
  return 0;
}

#elif HAVE_SIGALTSTACK && HAVE_DECL_SIGALTSTACK && HAVE_STACK_OVERFLOW_HANDLING

# if SIGINFO_WORKS

static size_t volatile page_size;

/* Handle a segmentation violation and exit.  This function is
   async-signal-safe.  */

static _GL_ASYNC_SAFE _Noreturn void
segv_handler (int signo, siginfo_t *info, void *context _GL_UNUSED)
{
  /* Clear SIGNO if it seems to have been a stack overflow.  */

  /* If si_code is nonpositive, something like raise (SIGSEGV) occurred
     so it cannot be a stack overflow.  */
  bool cannot_be_stack_overflow = info->si_code <= 0;

  /* An unaligned address cannot be a stack overflow.  */
#  if FAULT_YIELDS_SIGBUS
  cannot_be_stack_overflow |= signo == SIGBUS && info->si_code == BUS_ADRALN;
#  endif

  /* If we can't easily determine that it is not a stack overflow,
     assume that the rest of our program is perfect (!) and that
     this segmentation violation is a stack overflow.

     Note that although both Linux and Solaris provide
     sigaltstack, SA_ONSTACK, and SA_SIGINFO, currently only
     Solaris satisfies the XSI heuristic.  This is because
     Solaris populates uc_stack with the details of the
     interrupted stack, while Linux populates it with the details
     of the current stack.  */
  if (!cannot_be_stack_overflow)
    {
      /* If the faulting address is within the stack, or within one
         page of the stack, assume that it is a stack overflow.  */
      uintptr_t faulting_address = (uintptr_t) info->si_addr;

      /* On all platforms we know of, the first page is not in the
         stack to catch null pointer dereferening.  However, all other
         pages might be in the stack.  */
      void *stack_base = (void *) (uintptr_t) page_size;
      uintptr_t stack_size = 0; stack_size -= page_size;
#  if HAVE_XSI_STACK_OVERFLOW_HEURISTIC
      /* Tighten the stack bounds via the XSI heuristic.  */
      ucontext_t const *user_context = context;
      stack_base = user_context->uc_stack.ss_sp;
      stack_size = user_context->uc_stack.ss_size;
#  endif
      uintptr_t base = (uintptr_t) stack_base,
        lo = (INT_SUBTRACT_WRAPV (base, page_size, &lo) || lo < page_size
              ? page_size : lo),
        hi = ((INT_ADD_WRAPV (base, stack_size, &hi)
               || INT_ADD_WRAPV (hi, page_size - 1, &hi))
              ? UINTPTR_MAX : hi);
      if (lo <= faulting_address && faulting_address <= hi)
        signo = 0;

#   if DEBUG
      {
        char buf[1024];
        ignore_value (write (STDERR_FILENO, buf,
                             sprintf (buf,
                                      ("segv_handler code=%d fault=%p base=%p"
                                       " size=0x%zx page=0x%zx signo=%d\n"),
                                      info->si_code, info->si_addr, stack_base,
                                      stack_size, page_size, signo)));
      }
#  endif
    }

  die (signo);
}
# endif

int
c_stack_action (_GL_ASYNC_SAFE void (*action) (int))
{
  stack_t st;
  st.ss_flags = 0;
# if SIGALTSTACK_SS_REVERSED
  /* Irix mistakenly treats ss_sp as the upper bound, rather than
     lower bound, of the alternate stack.  */
  st.ss_sp = alternate_signal_stack.buffer + SIGSTKSZ - sizeof (void *);
  st.ss_size = sizeof alternate_signal_stack.buffer - sizeof (void *);
# else
  st.ss_sp = alternate_signal_stack.buffer;
  st.ss_size = sizeof alternate_signal_stack.buffer;
# endif
  int r = sigaltstack (&st, NULL);
  if (r != 0)
    return r;

  segv_action = action ? action : null_action;
  program_error_message = _("program error");
  stack_overflow_message = _("stack overflow");
  progname = getprogname ();

# if SIGINFO_WORKS
  page_size = sysconf (_SC_PAGESIZE);
# endif

  struct sigaction act;
  sigemptyset (&act.sa_mask);

# if SIGINFO_WORKS
  /* POSIX 1003.1-2001 says SA_RESETHAND implies SA_NODEFER, but
     this is not true on Solaris 8 at least.  It doesn't hurt to use
     SA_NODEFER here, so leave it in.  */
  act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND | SA_SIGINFO;
  act.sa_sigaction = segv_handler;
# else
  act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
  act.sa_handler = die;
# endif

# if FAULT_YIELDS_SIGBUS
  if (sigaction (SIGBUS, &act, NULL) < 0)
    return -1;
# endif
  return sigaction (SIGSEGV, &act, NULL);
}

#else /* ! (USE_LIBSIGSEGV
            || (HAVE_SIGALTSTACK && HAVE_DECL_SIGALTSTACK
                && HAVE_STACK_OVERFLOW_HANDLING)) */

int
c_stack_action (_GL_ASYNC_SAFE void (*action) (int)  _GL_UNUSED)
{
  errno = ENOTSUP;
  return -1;
}

#endif

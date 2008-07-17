# Check prerequisites for compiling lib/c-stack.c.

# Copyright (C) 2002, 2003, 2004, 2008 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Paul Eggert.

# serial 3

AC_DEFUN([AC_SYS_XSI_STACK_OVERFLOW_HEURISTIC],
  [# for STACK_DIRECTION
   AC_REQUIRE([AC_FUNC_ALLOCA])
   AC_CHECK_FUNCS_ONCE([setrlimit])
   AC_CHECK_HEADERS_ONCE([ucontext.h])

   AC_CACHE_CHECK([for working C stack overflow detection],
     ac_cv_sys_xsi_stack_overflow_heuristic,
     [AC_TRY_RUN(
	[
	 #include <unistd.h>
	 #include <signal.h>
	 #if HAVE_UCONTEXT_H
	 # include <ucontext.h>
	 #endif
	 #if HAVE_SETRLIMIT
	 # include <sys/types.h>
	 # include <sys/time.h>
	 # include <sys/resource.h>
	 #endif

	 static union
	 {
	   char buffer[SIGSTKSZ];
	   long double ld;
	   long u;
	   void *p;
	 } alternate_signal_stack;

	 #if STACK_DIRECTION
	 # define find_stack_direction(ptr) STACK_DIRECTION
	 #else
	 static int
	 find_stack_direction (char const *addr)
	 {
	   char dummy;
	   return (! addr ? find_stack_direction (&dummy)
		   : addr < &dummy ? 1 : -1);
	 }
	 #endif

	 static void
	 segv_handler (int signo, siginfo_t *info, void *context)
	 {
	   if (0 < info->si_code)
	     {
               /* For XSI heuristics to work, we need uc_stack to describe
	          the interrupted stack (as on Solaris), and not the
		  currently executing stack (as on Linux).  */
	       ucontext_t const *user_context = context;
	       char const *stack_min = user_context->uc_stack.ss_sp;
	       size_t stack_size = user_context->uc_stack.ss_size;
	       char const *faulting_address = info->si_addr;
	       size_t s = faulting_address - stack_min;
	       size_t page_size = sysconf (_SC_PAGESIZE);
	       if (find_stack_direction (0) < 0)
		 s += page_size;
	       if (s < stack_size + page_size)
		 _exit (0);
	     }

	   _exit (1);
	 }

	 static int
	 c_stack_action ()
	 {
	   stack_t st;
	   struct sigaction act;
	   int r;

	   st.ss_flags = 0;
	   st.ss_sp = alternate_signal_stack.buffer;
	   st.ss_size = sizeof alternate_signal_stack.buffer;
	   r = sigaltstack (&st, 0);
	   if (r != 0)
	     return r;

	   sigemptyset (&act.sa_mask);
	   act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND | SA_SIGINFO;
	   act.sa_sigaction = segv_handler;
	   return sigaction (SIGSEGV, &act, 0);
	 }

	 static int
	 recurse (char *p)
	 {
	   char array[500];
	   array[0] = 1;
	   return *p + recurse (array);
	 }

	 int
	 main ()
	 {
	   #if HAVE_SETRLIMIT && defined RLIMIT_STACK
	   /* Before starting the endless recursion, try to be friendly
	      to the user's machine.  On some Linux 2.2.x systems, there
	      is no stack limit for user processes at all.  We don't want
	      to kill such systems.  */
	   struct rlimit rl;
	   rl.rlim_cur = rl.rlim_max = 0x100000; /* 1 MB */
	   setrlimit (RLIMIT_STACK, &rl);
	   #endif

	   c_stack_action ();
	   return recurse ("\1");
	 }
	],
	[ac_cv_sys_xsi_stack_overflow_heuristic=yes],
	[ac_cv_sys_xsi_stack_overflow_heuristic=no],
	[ac_cv_sys_xsi_stack_overflow_heuristic=cross-compiling])])

   if test $ac_cv_sys_xsi_stack_overflow_heuristic = yes; then
     AC_DEFINE(HAVE_XSI_STACK_OVERFLOW_HEURISTIC, 1,
       [Define to 1 if extending the stack slightly past the limit causes
	a SIGSEGV, and an alternate stack can be established with sigaltstack,
	and the signal handler is passed a context that specifies the
	run time stack.  This behavior is defined by POSIX 1003.1-2001
	with the X/Open System Interface (XSI) option
	and is a standardized way to implement a SEGV-based stack
	overflow detection heuristic.])
   fi])


AC_DEFUN([gl_PREREQ_C_STACK],
  [AC_REQUIRE([AC_SYS_XSI_STACK_OVERFLOW_HEURISTIC])
   AC_REQUIRE([gl_LIBSIGSEGV])

   # for STACK_DIRECTION
   AC_REQUIRE([AC_FUNC_ALLOCA])

   AC_CHECK_FUNCS_ONCE([sigaltstack])
   AC_CHECK_DECLS([sigaltstack], , , [#include <signal.h>])

   AC_CHECK_HEADERS_ONCE([unistd.h ucontext.h])

   AC_CHECK_MEMBERS([struct sigaction.sa_sigaction], , , [#include <signal.h>])

   AC_CHECK_TYPES([stack_t], , , [#include <signal.h>])

   dnl c-stack does not need -lsigsegv if the system has XSI heuristics.
   if test "$gl_cv_lib_sigsegv" = yes \
       && test $"ac_cv_sys_xsi_stack_overflow_heuristic" != yes ; then
     AC_SUBST([LIBCSTACK], [$LIBSIGSEGV])
     AC_SUBST([LTLIBCSTACK], [$LTLIBSIGSEGV])
   fi
])

AC_DEFUN([gl_C_STACK],
[
  dnl Prerequisites of lib/c-stack.c.
  gl_PREREQ_C_STACK
])

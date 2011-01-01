# host-cpu-c-abi.m4 serial 1
dnl Copyright (C) 2002-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Bruno Haible and Sam Steingold.

dnl Sets the HOST_CPU_C_ABI variable to the canonical name of the CPU with its
dnl C language ABI (application binary interface).
dnl Also defines __${HOST_CPU_C_ABI}__ as a C macro in config.h.
dnl
dnl This canonical name can be used to select a particular assembly language
dnl source file that will interoperate with C code on the given host.
dnl
dnl For example:
dnl * 'i386' and 'sparc' are different canonical names, because code for i386
dnl   will not run on SPARC CPUs and vice versa. They have different
dnl   instruction sets.
dnl * 'sparc' and 'sparc64' are different canonical names, because code for
dnl   'sparc' and code for 'sparc64' cannot be linked together: 'sparc' code
dnl   contains 32-bit instructions, whereas 'sparc64' code contains 64-bit
dnl   instructions. A process on a SPARC CPU can be in 32-bit mode or in 64-bit
dnl   mode, but not both.
dnl * 'mips' and 'mipsn32' are different canonical names, because they use
dnl   different argument passing and return conventions for C functions, and
dnl   although the instruction set of 'mips' is a large subset of the
dnl   instruction set of 'mipsn32'.
dnl * 'mipsn32' and 'mips64' are different canonical names, because they use
dnl   different sizes for the C types like 'int' and 'void *', and although
dnl   the instruction sets of 'mipsn32' and 'mips64' are the same.
dnl * 'arm' and 'armel' are different canonical names, because they use
dnl   different memory ordering for the C types like 'int', and although
dnl   the instruction sets of 'arm' and 'armel' are the same.
dnl * The same name 'i386' is used for CPUs of type i386, i486, i586
dnl   (Pentium), AMD K7, Pentium II, Pentium IV, etc., because
dnl   - Instructions that do not exist on all of these CPUs (cmpxchg,
dnl     MMX, SSE, SSE2, 3DNow! etc.) are not frequently used. If your
dnl     assembly language source files use such instructions, you will
dnl     need to make the distinction.
dnl   - Speed of execution of the common instruction set is reasonable across
dnl     the entire family of CPUs. If you have assembly language source files
dnl     that are optimized for particular CPU types (like GNU gmp has), you
dnl     will need to make the distinction.
dnl   See <http://en.wikipedia.org/wiki/X86_instruction_listings>.
AC_DEFUN([gl_HOST_CPU_C_ABI],
[
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_CACHE_CHECK([host CPU and C ABI], [gl_cv_host_cpu_c_abi],
    [case "$host_cpu" in

changequote(,)dnl
       i[4567]86 )
changequote([,])dnl
         gl_cv_host_cpu_c_abi=i386
         ;;

       x86_64 )
         # On x86_64 systems, the C compiler may still be generating
         # 32-bit code.
         AC_EGREP_CPP([yes],
           [#if defined __LP64__ || defined __x86_64__ || defined __amd64__
            yes
            #endif],
           [gl_cv_host_cpu_c_abi=x86_64],
           [gl_cv_host_cpu_c_abi=i386])
         ;;

changequote(,)dnl
       alphaev[4-8] | alphaev56 | alphapca5[67] | alphaev6[78] )
changequote([,])dnl
         gl_cv_host_cpu_c_abi=alpha
         ;;

       arm* )
         AC_EGREP_CPP([yes],
           [#if defined __ARMEL__
            yes
            #endif],
           [gl_cv_host_cpu_c_abi=armel],
           [gl_cv_host_cpu_c_abi=arm])
         ;;

       hppa1.0 | hppa1.1 | hppa2.0* | hppa64 )
         # TODO: Distinguish hppa and hppa64 correctly.
         gl_cv_host_cpu_c_abi=hppa
         ;;

       mips* )
         # We should also check for (_MIPS_SZPTR == 64), but gcc keeps this
         # at 32.
         AC_EGREP_CPP([yes],
           [#if defined _MIPS_SZLONG && (_MIPS_SZLONG == 64)
            yes
            #endif],
           [gl_cv_host_cpu_c_abi=mips64],
           [# Strictly speaking, the MIPS ABI (-32 or -n32) is independent
            # from the CPU identification (-mips[12] or -mips[34]). But -n32
            # is commonly used together with -mips3, and it's easier to test
            # the CPU identification.
            AC_EGREP_CPP([yes],
              [#if __mips >= 3
               yes
               #endif],
              [gl_cv_host_cpu_c_abi=mipsn32],
              [gl_cv_host_cpu_c_abi=mips])])
         ;;

       powerpc64 )
         # On powerpc64 systems, the C compiler may still be generating
         # 32-bit code.
         AC_EGREP_CPP([yes],
           [#if defined __powerpc64__ || defined _ARCH_PPC64
            yes
            #endif],
           [gl_cv_host_cpu_c_abi=powerpc64],
           [gl_cv_host_cpu_c_abi=powerpc])
         ;;

       rs6000 )
         gl_cv_host_cpu_c_abi=powerpc
         ;;

       # TODO: Distinguish s390 and s390x correctly.

       sparc | sparc64 )
         # UltraSPARCs running Linux have `uname -m` = "sparc64", but the
         # C compiler still generates 32-bit code.
         AC_EGREP_CPP([yes],
           [#if defined __sparcv9 || defined __arch64__
            yes
            #endif],
           [gl_cv_host_cpu_c_abi=sparc64],
           [gl_cv_host_cpu_c_abi=sparc])
         ;;

       *)
         gl_cv_host_cpu_c_abi="$host_cpu"
         ;;
     esac
    ])

  HOST_CPU_C_ABI="$gl_cv_host_cpu_c_abi"
  AC_SUBST([HOST_CPU_C_ABI])

  # This was AC_DEFINE_UNQUOTED([__${gl_cv_host_cpu_c_abi}__]) earlier,
  # but KAI C++ 3.2d doesn't like this.
  cat >> confdefs.h <<EOF
#ifndef __${gl_cv_host_cpu_c_abi}__
#define __${gl_cv_host_cpu_c_abi}__ 1
#endif
EOF
  AH_TOP([/* CPU and C ABI indicator */
#ifndef __i386__
#undef __i386__
#endif
#ifndef __x86_64__
#undef __x86_64__
#endif
#ifndef __alpha__
#undef __alpha__
#endif
#ifndef __arm__
#undef __arm__
#endif
#ifndef __armel__
#undef __armel__
#endif
#ifndef __hppa__
#undef __hppa__
#endif
#ifndef __hppa64__
#undef __hppa64__
#endif
#ifndef __ia64__
#undef __ia64__
#endif
#ifndef __m68k__
#undef __m68k__
#endif
#ifndef __mips__
#undef __mips__
#endif
#ifndef __mipsn32__
#undef __mipsn32__
#endif
#ifndef __mips64__
#undef __mips64__
#endif
#ifndef __powerpc__
#undef __powerpc__
#endif
#ifndef __powerpc64__
#undef __powerpc64__
#endif
#ifndef __s390__
#undef __s390__
#endif
#ifndef __s390x__
#undef __s390x__
#endif
#ifndef __sh__
#undef __sh__
#endif
#ifndef __sparc__
#undef __sparc__
#endif
#ifndef __sparc64__
#undef __sparc64__
#endif
])

])

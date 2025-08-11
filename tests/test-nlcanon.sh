#!/bin/sh

if test $# = 0 || { test $# = 1 && test "x$1" = "x--no-reexec"; }; then
  # Unit test.
  . "${srcdir=.}/init.sh"; path_prepend_ .

  $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stdout "${srcdir}/test-nlcanon.sh" 42
  # Test the exit code.
  test $? = 42 || Exit 11
  # Test standard output.
  $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stdout "${srcdir}/test-nlcanon.sh" 42 | grep stdout
  test $? = 0 || Exit 12
  $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stdout "${srcdir}/test-nlcanon.sh" 42 | grep stderr
  test $? != 0 || Exit 13
  $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stdout "${srcdir}/test-nlcanon.sh" 42 | tr '\r' r | grep contentsr
  test $? != 0 || Exit 14
  # Test standard error.
  { $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stdout "${srcdir}/test-nlcanon.sh" 42; } 2>&1 >/dev/null | grep stdout
  test $? != 0 || Exit 15
  { $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stdout "${srcdir}/test-nlcanon.sh" 42; } 2>&1 >/dev/null | grep stderr
  test $? = 0 || Exit 16
  { $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stdout "${srcdir}/test-nlcanon.sh" 42; } 2>&1 >/dev/null | tr '\r' r | grep contentsr
  test $? = 0 || Exit 17

  $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stderr "${srcdir}/test-nlcanon.sh" 42
  # Test the exit code.
  test $? = 42 || Exit 21
  # Test standard output.
  $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stderr "${srcdir}/test-nlcanon.sh" 42 | grep stdout
  test $? = 0 || Exit 22
  $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stderr "${srcdir}/test-nlcanon.sh" 42 | grep stderr
  test $? != 0 || Exit 23
  $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stderr "${srcdir}/test-nlcanon.sh" 42 | tr '\r' r | grep contentsr
  test $? = 0 || Exit 24
  # Test standard error.
  { $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stderr "${srcdir}/test-nlcanon.sh" 42; } 2>&1 >/dev/null | grep stdout
  test $? != 0 || Exit 25
  { $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stderr "${srcdir}/test-nlcanon.sh" 42; } 2>&1 >/dev/null | grep stderr
  test $? = 0 || Exit 26
  { $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stderr "${srcdir}/test-nlcanon.sh" 42; } 2>&1 >/dev/null | tr '\r' r | grep contentsr
  test $? != 0 || Exit 27

  $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stdout,stderr "${srcdir}/test-nlcanon.sh" 42
  # Test the exit code.
  test $? = 42 || Exit 31
  # Test standard output.
  $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stdout,stderr "${srcdir}/test-nlcanon.sh" 42 | grep stdout
  test $? = 0 || Exit 32
  $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stdout,stderr "${srcdir}/test-nlcanon.sh" 42 | grep stderr
  test $? != 0 || Exit 33
  $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stdout,stderr "${srcdir}/test-nlcanon.sh" 42 | tr '\r' r | grep contentsr
  test $? != 0 || Exit 34
  # Test standard error.
  { $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stdout,stderr "${srcdir}/test-nlcanon.sh" 42; } 2>&1 >/dev/null | grep stdout
  test $? != 0 || Exit 35
  { $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stdout,stderr "${srcdir}/test-nlcanon.sh" 42; } 2>&1 >/dev/null | grep stderr
  test $? = 0 || Exit 36
  { $BOURNE_SHELL ${top_builddir}/nlcanon.sh all stdout,stderr "${srcdir}/test-nlcanon.sh" 42; } 2>&1 >/dev/null | tr '\r' r | grep contentsr
  test $? != 0 || Exit 37

  Exit 0
else
  # Callee.
  printf 'stdout-contents\r\n'
  printf 'stderr-contents\r\n' 1>&2
  exit $1
fi

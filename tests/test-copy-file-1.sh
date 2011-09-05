#!/bin/sh

# Test copy-file on the file system of /var/tmp, which usually is a local
# file system.

if test -d /var/tmp; then
  TMPDIR=/var/tmp
else
  TMPDIR=/tmp
fi
export TMPDIR

exec "${srcdir}/test-copy-file.sh"

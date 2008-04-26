#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' 1 2 3 15

tmpfiles="t-xstrtoimax.tmp t-xstrtoimax.xo"
: > t-xstrtoimax.tmp
too_big=99999999999999999999999999999999999999999999999999999999999999999999
result=0

# test xstrtoimax
./test-xstrtoimax${EXEEXT} 1 >> t-xstrtoimax.tmp 2>&1 || result=1
./test-xstrtoimax${EXEEXT} -1 >> t-xstrtoimax.tmp 2>&1 || result=1
./test-xstrtoimax${EXEEXT} 1k >> t-xstrtoimax.tmp 2>&1 || result=1
./test-xstrtoimax${EXEEXT} ${too_big}h >> t-xstrtoimax.tmp 2>&1 && result=1
./test-xstrtoimax${EXEEXT} $too_big >> t-xstrtoimax.tmp 2>&1 && result=1
./test-xstrtoimax${EXEEXT} x >> t-xstrtoimax.tmp 2>&1 && result=1
./test-xstrtoimax${EXEEXT} 9x >> t-xstrtoimax.tmp 2>&1 && result=1
./test-xstrtoimax${EXEEXT} 010 >> t-xstrtoimax.tmp 2>&1 || result=1
./test-xstrtoimax${EXEEXT} MiB >> t-xstrtoimax.tmp 2>&1 || result=1

# Find out how to remove carriage returns from output. Solaris /usr/ucb/tr
# does not understand '\r'.
if echo solaris | tr -d '\r' | grep solais > /dev/null; then
  cr='\015'
else
  cr='\r'
fi

# normalize output
LC_ALL=C tr -d "$cr" < t-xstrtoimax.tmp > t-xstrtoimax.xo
mv t-xstrtoimax.xo t-xstrtoimax.tmp

# compare expected output
cat > t-xstrtoimax.xo <<EOF
1->1 ()
-1->-1 ()
1k->1024 ()
invalid suffix in X argument \`${too_big}h'
X argument \`$too_big' too large
invalid X argument \`x'
invalid suffix in X argument \`9x'
010->8 ()
MiB->1048576 ()
EOF

diff t-xstrtoimax.xo t-xstrtoimax.tmp || result=1

rm -fr $tmpfiles

exit $result

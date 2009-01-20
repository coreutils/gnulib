#!/bin/sh

tmpfiles="test-link-a.txt test-link-b.txt test-link-c.txt"
trap 'rm -fr $tmpfiles' 1 2 3 15

# Create a file.
echo "hello" > test-link-a.txt || exit 1

# Use link() to create a new name for it.
./test-link${EXEEXT} test-link-a.txt test-link-b.txt || exit 1
cmp test-link-a.txt test-link-b.txt || exit 1

# Modify the contents of the first file.
echo "world" >> test-link-a.txt || exit 1
cmp test-link-a.txt test-link-b.txt || exit 1

# Modify the contents of the second file.
echo "some text" >> test-link-b.txt || exit 1
cmp test-link-a.txt test-link-b.txt || exit 1

# Delete the first file, then verity the second file still has the same
# contents.
cp test-link-a.txt test-link-c.txt || exit 1
rm test-link-a.txt || exit 1
cmp test-link-b.txt test-link-c.txt || exit 1

rm -fr $tmpfiles
exit 0

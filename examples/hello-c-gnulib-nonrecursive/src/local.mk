
# The list of programs that are built.
bin_PROGRAMS = src/hello

# The source files of the 'hello' program.
src_hello_SOURCES = src/hello.c
src_hello_LDADD = lib/libgnu.a

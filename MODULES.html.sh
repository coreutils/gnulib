#!/bin/sh
#
# Copyright (C) 2002, 2003 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#

# Usage: MODULES.html.sh > MODULES.html

# Extend the PATH so that gnulib-tool is found.
PATH=`dirname "$0"`:$PATH; export PATH

POSIX2001_URL='http://www.opengroup.org/onlinepubs/007904975/'
case $USER in
  bruno )
    POSIX2001_URL='file:/packages/www/www.opengroup.org/susv3/' ;;
esac

sed_lt='s,<,\&lt;,g'
sed_gt='s,>,\&gt;,g'
sed_escape_dot='s,\.,\\.,g'
sed_escape_slash='s,/,\\/,g'
trnl='\012'
sed_alt1='s,$,\\|,'
sed_alt2='s,^\\|,\\(,'
sed_alt3='s,\\|\\|$,\\),'
posix_headers=`echo '
aio
arpa/inet
assert
complex
cpio
ctype
dirent
dlfcn
errno
fcntl
fenv
float
fmtmsg
fnmatch
ftw
glob
grp
iconv
inttypes
iso646
langinfo
libgen
limits
locale
math
monetary
mqueue
ndbm
net/if
netdb
netinet/in
netinet/tcp
nl_types
poll
pthread
pwd
regex
sched
search
semaphore
setjmp
signal
spawn
stdarg
stdbool
stddef
stdint
stdio
stdlib
string
strings
stropts
sys/ipc
sys/mman
sys/msg
sys/resource
sys/select
sys/sem
sys/shm
sys/socket
sys/stat
sys/statvfs
sys/time
sys/timeb
sys/times
sys/types
sys/uio
sys/un
sys/utsname
sys/wait
syslog
tar
termios
tgmath
time
trace
ucontext
ulimit
unistd
utime
utmpx
wchar
wctype
wordexp
' | sed -e "$sed_alt1" | tr -d "$trnl" | sed -e "$sed_alt2" -e "$sed_alt3"`
posix_functions=`echo '
FD_CLR
FD_ISSET
FD_SET
FD_ZERO
_Exit
_exit
_longjmp
_setjmp
_tolower
_toupper
a64l
abort
abs
accept
access
acos
acosf
acosh
acoshf
acoshl
acosl
aio_cancel
aio_error
aio_fsync
aio_read
aio_return
aio_suspend
aio_write
alarm
asctime
asctime_r
asin
asinf
asinh
asinhf
asinhl
asinl
assert
atan
atan2
atan2f
atan2l
atanf
atanh
atanhf
atanhl
atanl
atexit
atof
atoi
atol
atoll
basename
bcmp
bcopy
bind
bsd_signal
bsearch
btowc
bzero
cabs
cabsf
cabsl
cacos
cacosf
cacosh
cacoshf
cacoshl
cacosl
calloc
carg
cargf
cargl
casin
casinf
casinh
casinhf
casinhl
casinl
catan
catanf
catanh
catanhf
catanhl
catanl
catclose
catgets
catopen
cbrt
cbrtf
cbrtl
ccos
ccosf
ccosh
ccoshf
ccoshl
ccosl
ceil
ceilf
ceill
cexp
cexpf
cexpl
cfgetispeed
cfgetospeed
cfsetispeed
cfsetospeed
chdir
chmod
chown
cimag
cimagf
cimagl
clearerr
clock
clock_getcpuclockid
clock_getres
clock_gettime
clock_nanosleep
clock_settime
clog
clogf
clogl
close
closedir
closelog
confstr
conj
conjf
conjl
connect
contents
copysign
copysignf
copysignl
cos
cosf
cosh
coshf
coshl
cosl
cpow
cpowf
cpowl
cproj
cprojf
cprojl
creal
crealf
creall
creat
crypt
csin
csinf
csinh
csinhf
csinhl
csinl
csqrt
csqrtf
csqrtl
ctan
ctanf
ctanh
ctanhf
ctanhl
ctanl
ctermid
ctime
ctime_r
daylight
dbm_clearerr
dbm_close
dbm_delete
dbm_error
dbm_fetch
dbm_firstkey
dbm_nextkey
dbm_open
dbm_store
difftime
dirname
div
dlclose
dlerror
dlopen
dlsym
drand48
dup
dup2
ecvt
encrypt
endgrent
endhostent
endnetent
endprotoent
endpwent
endservent
endutxent
environ
erand48
erf
erfc
erfcf
erfcl
erff
erfl
errno
exec
execl
execle
execlp
execv
execve
execvp
exit
exp
exp2
exp2f
exp2l
expf
expl
expm1
expm1f
expm1l
fabs
fabsf
fabsl
fattach
fchdir
fchmod
fchown
fclose
fcntl
fcvt
fdatasync
fdetach
fdim
fdimf
fdiml
fdopen
feclearexcept
fegetenv
fegetexceptflag
fegetround
feholdexcept
feof
feraiseexcept
ferror
fesetenv
fesetexceptflag
fesetround
fetestexcept
feupdateenv
fflush
ffs
fgetc
fgetpos
fgets
fgetwc
fgetws
fileno
flockfile
floor
floorf
floorl
fma
fmaf
fmal
fmax
fmaxf
fmaxl
fmin
fminf
fminl
fmod
fmodf
fmodl
fmtmsg
fnmatch
fopen
fork
fpathconf
fpclassify
fprintf
fputc
fputs
fputwc
fputws
fread
free
freeaddrinfo
freopen
frexp
frexpf
frexpl
fscanf
fseek
fseeko
fsetpos
fstat
fstatvfs
fsync
ftell
ftello
ftime
ftok
ftruncate
ftrylockfile
ftw
funlockfile
fwide
fwprintf
fwrite
fwscanf
gai_strerror
gcvt
getaddrinfo
getc
getc_unlocked
getchar
getchar_unlocked
getcontext
getcwd
getdate
getegid
getenv
geteuid
getgid
getgrent
getgrgid
getgrgid_r
getgrnam
getgrnam_r
getgroups
gethostbyaddr
gethostbyname
gethostent
gethostid
gethostname
getitimer
getlogin
getlogin_r
getmsg
getnameinfo
getnetbyaddr
getnetbyname
getnetent
getopt
getpeername
getpgid
getpgrp
getpid
getpmsg
getppid
getpriority
getprotobyname
getprotobynumber
getprotoent
getpwent
getpwnam
getpwnam_r
getpwuid
getpwuid_r
getrlimit
getrusage
gets
getservbyname
getservbyport
getservent
getsid
getsockname
getsockopt
getsubopt
gettimeofday
getuid
getutxent
getutxid
getutxline
getwc
getwchar
getwd
glob
globfree
gmtime
gmtime_r
grantpt
h_errno
hcreate
hdestroy
hsearch
htonl
htons
hypot
hypotf
hypotl
iconv
iconv_close
iconv_open
if_freenameindex
if_indextoname
if_nameindex
if_nametoindex
ilogb
ilogbf
ilogbl
imaxabs
imaxdiv
index
inet_addr
inet_ntoa
inet_ntop
inet_pton
initstate
insque
ioctl
isalnum
isalpha
isascii
isastream
isatty
isblank
iscntrl
isdigit
isfinite
isgraph
isgreater
isgreaterequal
isinf
isless
islessequal
islessgreater
islower
isnan
isnormal
isprint
ispunct
isspace
isunordered
isupper
iswalnum
iswalpha
iswblank
iswcntrl
iswctype
iswdigit
iswgraph
iswlower
iswprint
iswpunct
iswspace
iswupper
iswxdigit
isxdigit
j0
j1
jn
jrand48
kill
killpg
l64a
labs
lchown
lcong48
ldexp
ldexpf
ldexpl
ldiv
lfind
lgamma
lgammaf
lgammal
link
lio_listio
listen
llabs
lldiv
llrint
llrintf
llrintl
llround
llroundf
llroundl
localeconv
localtime
localtime_r
lockf
log
log10
log10f
log10l
log1p
log1pf
log1pl
log2
log2f
log2l
logb
logbf
logbl
logf
logl
longjmp
lrand48
lrint
lrintf
lrintl
lround
lroundf
lroundl
lsearch
lseek
lstat
makecontext
malloc
mblen
mbrlen
mbrtowc
mbsinit
mbsrtowcs
mbstowcs
mbtowc
memccpy
memchr
memcmp
memcpy
memmove
memset
mkdir
mkfifo
mknod
mkstemp
mktemp
mktime
mlock
mlockall
mmap
modf
modff
modfl
mprotect
mq_close
mq_getattr
mq_notify
mq_open
mq_receive
mq_send
mq_setattr
mq_timedreceive
mq_timedsend
mq_unlink
mrand48
msgctl
msgget
msgrcv
msgsnd
msync
munlock
munlockall
munmap
nan
nanf
nanl
nanosleep
nearbyint
nearbyintf
nearbyintl
nextafter
nextafterf
nextafterl
nexttoward
nexttowardf
nexttowardl
nftw
nice
nl_langinfo
nrand48
ntohl
ntohs
open
opendir
openlog
optarg
pathconf
pause
pclose
perror
pipe
poll
popen
posix_fadvise
posix_fallocate
posix_madvise
posix_mem_offset
posix_memalign
posix_openpt
posix_spawn
posix_spawn_file_actions_addclose
posix_spawn_file_actions_adddup2
posix_spawn_file_actions_addopen
posix_spawn_file_actions_destroy
posix_spawn_file_actions_init
posix_spawnattr_destroy
posix_spawnattr_getflags
posix_spawnattr_getpgroup
posix_spawnattr_getschedparam
posix_spawnattr_getschedpolicy
posix_spawnattr_getsigdefault
posix_spawnattr_getsigmask
posix_spawnattr_init
posix_spawnattr_setflags
posix_spawnattr_setpgroup
posix_spawnattr_setschedparam
posix_spawnattr_setschedpolicy
posix_spawnattr_setsigdefault
posix_spawnattr_setsigmask
posix_spawnp
posix_trace_attr_destroy
posix_trace_attr_getclockres
posix_trace_attr_getcreatetime
posix_trace_attr_getgenversion
posix_trace_attr_getinherited
posix_trace_attr_getlogfullpolicy
posix_trace_attr_getlogsize
posix_trace_attr_getmaxdatasize
posix_trace_attr_getmaxsystemeventsize
posix_trace_attr_getmaxusereventsize
posix_trace_attr_getname
posix_trace_attr_getstreamfullpolicy
posix_trace_attr_getstreamsize
posix_trace_attr_init
posix_trace_attr_setinherited
posix_trace_attr_setlogfullpolicy
posix_trace_attr_setlogsize
posix_trace_attr_setmaxdatasize
posix_trace_attr_setname
posix_trace_attr_setstreamfullpolicy
posix_trace_attr_setstreamsize
posix_trace_clear
posix_trace_close
posix_trace_create
posix_trace_create_withlog
posix_trace_event
posix_trace_eventid_equal
posix_trace_eventid_get_name
posix_trace_eventid_open
posix_trace_eventset_add
posix_trace_eventset_del
posix_trace_eventset_empty
posix_trace_eventset_fill
posix_trace_eventset_ismember
posix_trace_eventtypelist_getnext_id
posix_trace_eventtypelist_rewind
posix_trace_flush
posix_trace_get_attr
posix_trace_get_filter
posix_trace_get_status
posix_trace_getnext_event
posix_trace_open
posix_trace_rewind
posix_trace_set_filter
posix_trace_shutdown
posix_trace_start
posix_trace_stop
posix_trace_timedgetnext_event
posix_trace_trid_eventid_open
posix_trace_trygetnext_event
posix_typed_mem_get_info
posix_typed_mem_open
pow
powf
powl
pread
printf
pselect
pthread_atfork
pthread_attr_destroy
pthread_attr_getdetachstate
pthread_attr_getguardsize
pthread_attr_getinheritsched
pthread_attr_getschedparam
pthread_attr_getschedpolicy
pthread_attr_getscope
pthread_attr_getstack
pthread_attr_getstackaddr
pthread_attr_getstacksize
pthread_attr_init
pthread_attr_setdetachstate
pthread_attr_setguardsize
pthread_attr_setinheritsched
pthread_attr_setschedparam
pthread_attr_setschedpolicy
pthread_attr_setscope
pthread_attr_setstack
pthread_attr_setstackaddr
pthread_attr_setstacksize
pthread_barrier_destroy
pthread_barrier_init
pthread_barrier_wait
pthread_barrierattr_destroy
pthread_barrierattr_getpshared
pthread_barrierattr_init
pthread_barrierattr_setpshared
pthread_cancel
pthread_cleanup_pop
pthread_cleanup_push
pthread_cond_broadcast
pthread_cond_destroy
pthread_cond_init
pthread_cond_signal
pthread_cond_timedwait
pthread_cond_wait
pthread_condattr_destroy
pthread_condattr_getclock
pthread_condattr_getpshared
pthread_condattr_init
pthread_condattr_setclock
pthread_condattr_setpshared
pthread_create
pthread_detach
pthread_equal
pthread_exit
pthread_getconcurrency
pthread_getcpuclockid
pthread_getschedparam
pthread_getspecific
pthread_join
pthread_key_create
pthread_key_delete
pthread_kill
pthread_mutex_destroy
pthread_mutex_getprioceiling
pthread_mutex_init
pthread_mutex_lock
pthread_mutex_setprioceiling
pthread_mutex_timedlock
pthread_mutex_trylock
pthread_mutex_unlock
pthread_mutexattr_destroy
pthread_mutexattr_getprioceiling
pthread_mutexattr_getprotocol
pthread_mutexattr_getpshared
pthread_mutexattr_gettype
pthread_mutexattr_init
pthread_mutexattr_setprioceiling
pthread_mutexattr_setprotocol
pthread_mutexattr_setpshared
pthread_mutexattr_settype
pthread_once
pthread_rwlock_destroy
pthread_rwlock_init
pthread_rwlock_rdlock
pthread_rwlock_timedrdlock
pthread_rwlock_timedwrlock
pthread_rwlock_tryrdlock
pthread_rwlock_trywrlock
pthread_rwlock_unlock
pthread_rwlock_wrlock
pthread_rwlockattr_destroy
pthread_rwlockattr_getpshared
pthread_rwlockattr_init
pthread_rwlockattr_setpshared
pthread_self
pthread_setcancelstate
pthread_setcanceltype
pthread_setconcurrency
pthread_setschedparam
pthread_setschedprio
pthread_setspecific
pthread_sigmask
pthread_spin_destroy
pthread_spin_init
pthread_spin_lock
pthread_spin_trylock
pthread_spin_unlock
pthread_testcancel
ptsname
putc
putc_unlocked
putchar
putchar_unlocked
putenv
putmsg
putpmsg
puts
pututxline
putwc
putwchar
pwrite
qsort
raise
rand
rand_r
random
read
readdir
readdir_r
readlink
readv
realloc
realpath
recv
recvfrom
recvmsg
regcomp
regerror
regexec
regfree
remainder
remainderf
remainderl
remove
remque
remquo
remquof
remquol
rename
rewind
rewinddir
rindex
rint
rintf
rintl
rmdir
round
roundf
roundl
scalb
scalbln
scalblnf
scalblnl
scalbn
scalbnf
scalbnl
scanf
sched_get_priority_max
sched_getparam
sched_getscheduler
sched_rr_get_interval
sched_setparam
sched_setscheduler
sched_yield
seed48
seekdir
select
sem_close
sem_destroy
sem_getvalue
sem_init
sem_open
sem_post
sem_timedwait
sem_trywait
sem_unlink
sem_wait
semctl
semget
semop
send
sendmsg
sendto
setbuf
setcontext
setegid
setenv
seteuid
setgid
setgrent
sethostent
setitimer
setjmp
setkey
setlocale
setlogmask
setnetent
setpgid
setpgrp
setpriority
setprotoent
setpwent
setregid
setreuid
setrlimit
setservent
setsid
setsockopt
setstate
setuid
setutxent
setvbuf
shm_open
shm_unlink
shmat
shmctl
shmdt
shmget
shutdown
sigaction
sigaddset
sigaltstack
sigdelset
sigemptyset
sigfillset
sighold
sigignore
siginterrupt
sigismember
siglongjmp
signal
signbit
sigpause
sigpending
sigprocmask
sigqueue
sigrelse
sigset
sigsetjmp
sigsuspend
sigtimedwait
sigwait
sigwaitinfo
sin
sinf
sinh
sinhf
sinhl
sinl
sleep
snprintf
sockatmark
socket
socketpair
sprintf
sqrt
sqrtf
sqrtl
srand
srand48
srandom
sscanf
stat
statvfs
stdin
strcasecmp
strcat
strchr
strcmp
strcoll
strcpy
strcspn
strdup
strerror
strerror_r
strfmon
strftime
strlen
strncasecmp
strncat
strncmp
strncpy
strpbrk
strptime
strrchr
strspn
strstr
strtod
strtof
strtoimax
strtok
strtok_r
strtol
strtold
strtoll
strtoul
strtoull
strtoumax
strxfrm
swab
swapcontext
swprintf
swscanf
symlink
sync
sysconf
syslog
system
tan
tanf
tanh
tanhf
tanhl
tanl
tcdrain
tcflow
tcflush
tcgetattr
tcgetpgrp
tcgetsid
tcsendbreak
tcsetattr
tcsetpgrp
tdelete
telldir
tempnam
tfind
tgamma
tgammaf
tgammal
time
timer_create
timer_delete
timer_getoverrun
timer_settime
times
timezone
tmpfile
tmpnam
toascii
toc
tolower
toupper
towctrans
towlower
towupper
trunc
truncate
truncf
truncl
tsearch
ttyname
ttyname_r
twalk
tzname
tzset
ualarm
ulimit
umask
uname
ungetc
ungetwc
unlink
unlockpt
unsetenv
usleep
utime
utimes
va_arg
va_copy
va_end
va_start
vfork
vfprintf
vfscanf
vfwprintf
vfwscanf
vprintf
vscanf
vsnprintf
vsprintf
vsscanf
vswprintf
vswscanf
vwprintf
vwscanf
wait
waitid
waitpid
wcrtomb
wcscat
wcschr
wcscmp
wcscoll
wcscpy
wcscspn
wcsftime
wcslen
wcsncat
wcsncmp
wcsncpy
wcspbrk
wcsrchr
wcsrtombs
wcsspn
wcsstr
wcstod
wcstof
wcstoimax
wcstok
wcstol
wcstold
wcstoll
wcstombs
wcstoul
wcstoull
wcstoumax
wcswcs
wcswidth
wcsxfrm
wctob
wctomb
wctrans
wctype
wcwidth
wmemchr
wmemcmp
wmemcpy
wmemmove
wmemset
wordexp
wordfree
wprintf
write
writev
wscanf
y0
y1
yn
' | sed -e "$sed_alt1" | tr -d "$trnl" | sed -e "$sed_alt2" -e "$sed_alt3"`

indent=""
missed_modules=`gnulib-tool --list`
missed_files=`ls -d lib/* m4/* | sed -e /CVS/d -e /README/d -e /ChangeLog/d -e /Makefile/d -e /TODO/d -e '/~$/d'`

# func_echo line
# outputs line with indentation.
func_echo ()
{
  echo "${indent}$*"
}

# func_indent
# increases the indentation.
func_indent ()
{
  indent="$indent  "
}

# func_unindent
# decreases the indentation.
func_unindent ()
{
  indent=`echo "$indent" | sed -e 's/  $//'`
}

# func_begin tag [attribute...]
# opens a HTML tag.
func_begin ()
{
  func_echo "<$*>"
  func_indent
}

# func_end tag
# closes a HTML tag.
func_end ()
{
  func_unindent
  func_echo "</$1>"
}

# func_wrap tag [attribute...]
# encloses $element in a HTML tag, without line breaks.
func_wrap ()
{
  element="<$*>${element}</$1>"
}

# func_section_wrap sectiontag
# encloses $element in a HTML tag referring to the given tag.
func_section_wrap ()
{
  if test -n "$in_toc"; then
    func_wrap A "HREF=\"#$1\""
  else
    func_wrap A "NAME=\"$1\""
  fi
}

# func_begin_table
func_begin_table ()
{
  func_begin TABLE
  if test -z "$in_toc"; then
    func_begin TR
    func_echo "<TH ALIGN=LEFT>modules/"
    func_echo "<TH ALIGN=LEFT>lib/"
    func_echo "<TH ALIGN=LEFT>lib/"
    func_echo "<TH ALIGN=LEFT>m4/"
    func_echo "<TH ALIGN=LEFT>&nbsp;"
    func_end TR
    func_begin TR
    func_echo "<TH ALIGN=LEFT>Module"
    func_echo "<TH ALIGN=LEFT>Header"
    func_echo "<TH ALIGN=LEFT>Implementation"
    func_echo "<TH ALIGN=LEFT>Autoconf macro"
    func_echo "<TH ALIGN=LEFT>Depends on"
    func_end TR
  fi
}

# func_module module
func_module ()
{
    sed_remove_trailing_empty_line='${
/^$/d
}'

  if test -n "$in_toc"; then

    func_begin TR "WIDTH=\"100%\""

    element='<A HREF="#module='$1'">'$1'</A>'
    func_echo "<TD ALIGN=LEFT VALIGN=TOP WIDTH=\"20%\">$element"

    element=`gnulib-tool --extract-description $1 | sed -e "$sed_lt" -e "$sed_gt" -e "$sed_remove_trailing_empty_line" | sed -e 's,^, ,' | sed -e 's,\([^a-zA-Z]\)'"${posix_functions}"'(),\1<A HREF="'"$POSIX2001_URL"'functions/\2.html">\2</A>(),g' | sed -e 's,^ ,,'`
    func_echo "<TD ALIGN=LEFT VALIGN=TOP WIDTH=\"80%\">$element"

    func_end TR

  else

    func_begin TR

    element='<A NAME="module='$1'"></A><A HREF="modules/'$1'">'$1'</A>'
    func_echo "<TD ALIGN=LEFT VALIGN=TOP>$element"

    element=`gnulib-tool --extract-include-directive $1 | sed -e "$sed_lt" -e "$sed_gt" -e "$sed_remove_trailing_empty_line" | sed -e 's,^#include "\(.*\)"$,#include "<A HREF="lib/\1">\1</A>",' -e 's,^#include &lt;'"${posix_headers}"'\.h&gt;$,#include \&lt;<A HREF="'"$POSIX2001_URL"'basedefs/\1.h.html">\1.h</A>\&gt;,' | sed -e 's/$/<BR>/' | tr -d "$trnl" | sed -e 's/<BR>$//'`
    test -n "$element" || element='---'
    func_echo "<TD ALIGN=LEFT VALIGN=TOP>$element"

    sed_choose_unconditional_nonstandard_include='s,^#include "\(.*\)"$,\1,p'
    includefile=`gnulib-tool --extract-include-directive $1 | sed -n -e "$sed_choose_unconditional_nonstandard_include" | sed -e "$sed_escape_dot" | tr -d "$trnl"`
    sed_choose_lib_files='s,^lib/\(.*\)$,\1,p'
    element=`gnulib-tool --extract-filelist $1 | sed -e '/^$/d' | sed -n -e "$sed_choose_lib_files" | sed -e '/^'"${includefile}"'$/d' | sed -e 's,^\(.*\)$,<A HREF="lib/\1">\1</A>,' | sed -e 's/$/<BR>/' | tr -d "$trnl" | sed -e 's/<BR>$//'`
    test -n "$element" || element='---'
    func_echo "<TD ALIGN=LEFT VALIGN=TOP>$element"

    sed_choose_m4_files='s,^m4/\(.*\)$,\1,p'
    element=`(gnulib-tool --extract-filelist $1 | sed -e "$sed_remove_trailing_empty_line" | sed -n -e "$sed_choose_m4_files" | sed -e '/^onceonly/d' | sed -e 's,^\(.*\)$,<A HREF="m4/\1">\1</A>,'; gnulib-tool --extract-autoconf-snippet $1 | sed -e "$sed_remove_trailing_empty_line") | sed -e 's/$/<BR>/' | tr -d "$trnl" | sed -e 's/<BR>$//'`
    test -n "$element" || element='---'
    func_echo "<TD ALIGN=LEFT VALIGN=TOP>$element"

    element=`gnulib-tool --extract-dependencies $1 | sed -e "$sed_remove_trailing_empty_line" | sed -e 's/$/<BR>/' | tr -d "$trnl" | sed -e 's/<BR>$//'`
    test -n "$element" || element='---'
    func_echo "<TD ALIGN=LEFT VALIGN=TOP>$element"

    func_end TR

    missed_modules=`echo "$missed_modules" | sed -e '/^'"$1"'$/d'`

    files=`gnulib-tool --extract-filelist $1`
    sed_removal_prefix='s,^,/^,'
    sed_removal_suffix='s,$,\$/d,'
    sed_remove_files=`echo '{'; for file in $files; do echo $file | sed -e "$sed_escape_dot" -e "$sed_escape_slash" | sed -e "$sed_removal_prefix" -e "$sed_removal_suffix"; done; echo '}'`
    missed_files=`echo "$missed_files" | sed -e "$sed_remove_files"`

  fi
}

# func_end_table
func_end_table ()
{
  func_end TABLE
}

# func_all_modules
func_all_modules ()
{
  element="Support for systems lacking ANSI C 89"
  func_section_wrap ansic_sup
  func_wrap H2
  func_echo "$element"

  func_begin_table
  func_module c-bs-a
  func_module assert
  func_module exit
  func_module atexit
  func_module strtod
  func_module strtol
  func_module strtoul
  func_module memchr
  func_module memcmp
  func_module memcpy
  func_module memmove
  func_module memset
  func_module strcspn
  func_module strpbrk
  func_module strstr
  func_module strerror
  func_module mktime
  func_end_table

  func_echo 'These modules are not listed among dependencies below, for simplicity.'

  element="Enhancements for ANSI C 89 functions"
  func_section_wrap ansic_enh
  func_wrap H2
  func_echo "$element"

  element="Memory management functions <stdlib.h>"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap ansic_enh_stdlib_memory
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module malloc
  func_module realloc
  func_end_table

  element="Date and time <time.h>"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap ansic_enh_time_datetime
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module strftime
  func_end_table

  element="Extra functions based on ANSI C 89"
  func_section_wrap ansic_ext
  func_wrap H2
  func_echo "$element"

  element="Memory management functions <stdlib.h>"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap ansic_ext_stdlib_memory
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module xalloc
  func_module alloca
  func_module bumpalloc
  func_end_table

  element="Integer arithmetic functions <stdlib.h>"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap ansic_ext_stdlib_arith
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module gcd
  func_module minmax
  func_end_table

  element="Environment variables <stdlib.h>"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap ansic_ext_stdlib_env
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module putenv
  func_module setenv
  func_module xsetenv
  func_end_table

  element="Character handling <ctype.h>"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap ansic_ext_ctype
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module c-ctype
  func_end_table

  element="String handling <string.h>"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap ansic_ext_string
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module bcopy
  func_module memrchr
  func_module stpcpy
  func_module stpncpy
  func_module strcase
  func_module strdup
  func_module strnlen
  func_module strndup
  #func_module fstrcmp
  func_end_table

  element="Numeric conversion functions <stdlib.h>"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap ansic_ext_stdlib_conv
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module xstrtod
  func_module xstrtol
  func_end_table

  element="Date and time <time.h>"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap ansic_ext_time_datetime
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module getdate
  func_end_table

  element="Input/Output <stdio.h>"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap ansic_ext_stdio
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module unlocked-io
  func_module vasnprintf
  func_module vasprintf
  func_end_table

  element="Command-line arguments"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap ansic_ext_argv
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module argmatch
  func_module version-etc
  func_module long-options
  func_end_table

  element="Cryptographic computations"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap ansic_ext_crypto
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module md5
  func_module sha
  func_end_table

  element="Misc"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap ansic_ext_misc
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module diacrit
  func_module getline
  func_module linebuffer
  func_module obstack
  func_module hash-pjw
  func_module hash
  func_module readtokens
  func_module strverscmp
  func_end_table

  element="Support for systems lacking ISO C 99"
  func_section_wrap isoc_sup
  func_wrap H2
  func_echo "$element"

  element="Boolean type and values <stdbool.h>"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap isoc_sup_stdbool
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module stdbool
  func_end_table

  element="Numeric conversion functions <stdlib.h>"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap isoc_sup_stdlib_conv
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module strtoll
  func_module strtoull
  func_end_table

  element="Functions for greatest-width integer types <inttypes.h>"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap isoc_sup_inttypes
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module strtoimax
  func_module strtoumax
  func_end_table

  element="Enhancements for ISO C 99 functions"
  func_section_wrap isoc_enh
  func_wrap H2
  func_echo "$element"

  func_begin_table
  func_end_table

  element="Extra functions based on ISO C 99"
  func_section_wrap isoc_ext
  func_wrap H2
  func_echo "$element"

  element="Numeric conversion functions <stdlib.h>"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap isoc_ext_stdlib_conv
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module xstrtoimax
  func_module xstrtoumax
  func_end_table

  element="Support for systems lacking POSIX:2001"
  func_section_wrap posix_sup
  func_wrap H2
  func_echo "$element"

  func_begin_table
  func_module chown
  func_module dup2
  func_module ftruncate
  func_module getgroups
  func_module gettimeofday
  func_module mkdir
  func_module mkstemp
  #func_module mkdtemp
  func_module stat
  func_module lstat
  func_module timespec
  func_module nanosleep
  func_module regex
  func_module rename
  func_module rmdir
  func_module utime
  func_end_table

  element="Enhancements for POSIX:2001 functions"
  func_section_wrap posix_enh
  func_wrap H2
  func_echo "$element"

  func_begin_table
  func_module dirname
  func_module getopt
  func_module unistd-safer
  func_module fnmatch
  func_module fnmatch-posix
  func_module fnmatch-gnu
  func_module exclude
  func_end_table

  element="Extra functions based on POSIX:2001"
  func_section_wrap posix_ext
  func_wrap H2
  func_echo "$element"

  element="Numeric conversion functions"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap posix_ext_conv
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module human
  func_end_table

  element="File system functions"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap posix_ext_filesys
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module acl
  func_module backupfile
  #func_module copy-file
  func_module fsusage
  func_module dirfd
  func_module euidaccess
  func_module file-type
  func_module fileblocks
  func_module filemode
  func_module isdir
  func_module lchown
  func_module makepath
  func_module modechange
  func_module mountlist
  func_module path-concat
  #func_module pathname
  func_module pathmax
  func_module same
  func_module save-cwd
  func_module savedir
  func_module xgetcwd
  func_module xreadlink
  func_end_table

  element="File descriptor based Input/Output"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap posix_ext_filedesc
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module safe-read
  func_module safe-write
  func_module full-read
  func_module full-write
  #func_module binary-io
  func_end_table

  element="File stream based Input/Output"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap posix_ext_filestream
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module fpending
  func_module closeout
  func_module stdio-safer
  func_module getpass
  func_end_table

  element="Users and groups"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap posix_ext_uidgid
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module getugroups
  func_module group-member
  func_module idcache
  func_module userspec
  func_end_table

  element="Date and time"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap posix_ext_datetime
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module gettime
  func_module settime
  func_module posixtm
  func_end_table

  element="Networking functions"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap posix_ext_net
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module gethostname
  func_module xgethostname
  func_module canon-host
  func_end_table

  element="Internationalization functions"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap posix_ext_i18n
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module gettext
  func_module iconv
  func_module localcharset
  func_module hard-locale
  func_module mbswidth
  func_module memcasecmp
  func_module memcoll
  func_module xmemcoll
  func_module unicodeio
  func_module rpmatch
  func_module yesno
  #func_module ucs4-utf8
  #func_module ucs4-utf16
  #func_module utf8-ucs4
  #func_module utf16-ucs4
  #func_module linebreak
  func_end_table

  element="Executing programs"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap posix_ext_exec
  func_wrap H3
  func_echo "$element"

  #func_begin_table
  #func_module findprog
  #func_module wait-process
  #func_module execute
  #func_module pipe
  #func_module sh-quote
  #func_end_table

  element="Java"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap posix_ext_java
  func_wrap H3
  func_echo "$element"

  #func_begin_table
  #func_module classpath
  #func_module javacomp
  #func_module javaexec
  #func_module gcj
  #func_end_table

  element="Misc"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_section_wrap posix_ext_misc
  func_wrap H3
  func_echo "$element"

  func_begin_table
  func_module exitfail
  func_module c-stack
  func_module error
  func_module fatal
  func_module getloadavg
  func_module getpagesize
  func_module getusershell
  func_module physmem
  func_module posixver
  func_module quotearg
  func_module quote
  func_module readutmp
  func_module sig2str
  func_end_table
}


echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">'
func_begin HTML

func_begin HEAD

element="Gnulib Module List"
func_wrap TITLE
func_echo "$element"

modules=`gnulib-tool --list`
modules=`for m in $modules; do printf "%s, " $m; done | sed -e 's/, $//'`
func_echo '<META NAME="keywords" CONTENT="'"${modules}"'">'

func_end HEAD

func_begin BODY

func_end BODY

element="Gnulib Module List"
func_wrap H1
func_echo "$element"

func_echo 'This is a list of the modules which make up gnulib, with dependencies.'

in_toc=yes
func_all_modules

in_toc=
func_all_modules

if test -n "$missed_modules"; then

  element="Unclassified modules - please update MODULES.html.sh"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_wrap H2
  func_echo "$element"

  func_begin_table
  for module in $missed_modules; do
    func_module $module
  done
  func_end_table

fi

if test -n "$missed_files"; then

  element="Lone files - please create new modules containing them"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_wrap H2
  func_echo "$element"

  func_echo '<PRE>'
  echo "$missed_files" | sed -e 's,^\(.*\)$,<A HREF="\1">\1</A>,'
  echo '</PRE>'

fi

element="Future developments"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H2
func_echo "$element"

func_echo 'Ideally a module could consist of:'
func_begin UL
func_echo '<LI>A header file: lib/<VAR>module</VAR>.h'
func_echo '<LI>One or more implementation files: lib/<VAR>module</VAR>.c et al.'
func_echo '<LI>One or more autoconf macro files: m4/<VAR>module</VAR>.m4 et al.'
func_echo '<LI>A configure.ac fragment, Makefile.am fragment, dependency list: modules/<VAR>module</VAR>'
func_echo '<LI>Some documentation'
func_echo '<LI>A POT file and some PO files'
func_echo '<LI>A testsuite'
func_end UL

func_echo '<HR>'
func_echo 'Generated from <CODE>MODULES.html.sh</CODE> on '`LC_ALL=C date +"%e %B %Y"`.

func_end HTML

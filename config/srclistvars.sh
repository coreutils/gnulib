# $Id: srclistvars.sh,v 1.9 2003-07-14 22:44:04 eggert Exp $
# Variables for srclist-update and srclist.txt.
# Will change for each user.

case $LOGNAME in
eggert)
  : ${AUTOCONF=../autoconf}
  : ${AUTOMAKE=../automake}
  : ${GETTEXT=../gettext}
  : ${GNUCONFIG=../config}
  : ${GNUORG=../gnuorg}
  : ${LIBCSRC=../libc}
  : ${TEXINFOSRC=../texinfo}
esac

: ${ACLOCAL=/usr/local/gnu/share/aclocal}
: ${AUTOCONF=/usr/local/gnu/share/autoconf}
: ${AUTOMAKE=$HOME/gnu/src/automake}
: ${EMACSSRC=$HOME/gnu/src/emacs}
: ${GETTEXT=/usr/local/gnu/share/gettext}
: ${GNUBIN=/usr/local/gnu/bin}
: ${GNUCONFIG=$HOME/gnu/src/ftp.gnu.org/pub/gnu/config}
: ${GNULIBSRC=$HOME/gnu/src/gnulib}
: ${GNUORG=$HOME/gnu/gnuorg}
: ${LIBCSRC=$HOME/gnu/src/libc}
: ${TEXINFOSRC=/u/texinfo/src}
: ${TEXMFROOT=/usr/local/texmf/texmf}

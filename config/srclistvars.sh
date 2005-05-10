# $Id: srclistvars.sh,v 1.23 2005-05-10 20:44:26 karl Exp $
# Variables for srclist-update and srclist.txt.
# Will change for each user.

case $LOGNAME in
jas | eggert)
  : ${AUTOCONF=../autoconf}
  : ${AUTOMAKE=../automake}
  : ${GETTEXT=../gettext}
  : ${GNUCONFIG=../config}
  : ${GNUORG=../gnuorg}
  : ${GNUWWWLICENSES=$GNUORG}
  : ${GNUSTANDARDS=../gnustandards}
  : ${LIBTOOL=../libtool}
  : ${LIBCSRC=../libc}
  : ${TEXINFOSRC=../texinfo}
  ;;

karl)
  : ${ACLOCAL=/usr/local/gnu/share/aclocal}
  : ${AUTOCONF=$HOME/gnu/src/autoconf}
  : ${AUTOMAKE=$HOME/gnu/src/automake}
  : ${EMACSSRC=$HOME/gnu/src/emacs}
  : ${GETTEXT=$HOME/gnu/src/gettext}
  : ${GNUBIN=/usr/local/gnu/bin}
  : ${GNUCONFIG=$HOME/gnu/src/config}
  : ${GNULIBSRC=$HOME/gnu/src/gnulib}
  : ${GNUSTANDARDS=$HOME/gnu/src/gnustandards}
  : ${GNUWWWLICENSES=$HOME/gnu/www/www/licenses}
  : ${LIBCSRC=$HOME/gnu/src/libc}
  : ${LIBTOOL=$HOME/gnu/src/libtool}
  : ${TEXINFOSRC=/u/texinfo/src}
  : ${TEXMFROOT=/usr/local/texmf/texmf-dist}
  ;;

esac

# $Id: srclistvars.sh,v 1.26 2006-08-08 16:15:45 karl Exp $
# Variables for srclist-update and srclist.txt.
# Will change for each user.

case $LOGNAME in
jas | eggert)
  : ${AUTOCONF=../autoconf}
  : ${AUTOMAKE=../automake}
  : ${COREUTILS=../coreutils}
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
  : ${GNUBIN=/usr/local/gnu/bin}
  : ${GNUWWWLICENSES=$HOME/gnu/www/www/licenses}
  : ${TEXMFROOT=/usr/local/texmf/texmf-dist}
  : ${AUTOCONF=../autoconf}
  : ${AUTOMAKE=../automake}
  : ${COREUTILS=../coreutils}
  : ${EMACSSRC=../emacs}
  : ${GETTEXT=../gettext}
  : ${GNUCONFIG=../config}
  : ${GNUORG=$HOME/gnu/gnuorg}
  : ${GNULIBSRC=../gnulib}
  : ${GNUSTANDARDS=../gnustandards}
  : ${LIBCSRC=../libc}
  : ${LIBTOOL=../libtool}
  : ${TEXINFOSRC=../texinfo}
  ;;

esac

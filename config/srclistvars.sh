# $Id: srclistvars.sh,v 1.12 2004-01-18 14:54:16 karl Exp $
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
  ;;

karl)
  : ${ACLOCAL=/usr/local/gnu/share/aclocal}
  : ${AUTOCONF=/usr/local/gnu/share/autoconf}
  : ${AUTOMAKE=$HOME/gnu/src/automake}
  : ${EMACSSRC=$HOME/gnu/src/emacs}
  : ${GETTEXT=$HOME/gnu/src/gettext}
  : ${GNUBIN=/usr/local/gnu/bin}
  : ${GNUCONFIG=$HOME/gnu/src/config}
  : ${GNULIBSRC=$HOME/gnu/src/gnulib}
  : ${GNUWWWLICENSES=$HOME/gnu/www/licenses}
  : ${LIBCSRC=$HOME/gnu/src/libc}
  : ${TEXINFOSRC=/u/texinfo/src}
  : ${TEXMFROOT=/usr/local/texmf/texmf}
  ;;

esac

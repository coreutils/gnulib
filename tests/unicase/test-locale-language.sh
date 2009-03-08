#!/bin/sh

LC_ALL=C                       ./test-locale-language${EXEEXT} '' || exit 1

: ${LOCALE_FR=fr_FR}
if test $LOCALE_FR_UTF8 != none; then
  LC_ALL=$LOCALE_FR_UTF8       ./test-locale-language${EXEEXT} fr || exit 1
fi

: ${LOCALE_FR_UTF8=fr_FR.UTF-8}
if test $LOCALE_FR_UTF8 != none; then
  LC_ALL=$LOCALE_FR_UTF8       ./test-locale-language${EXEEXT} fr || exit 1
fi

: ${LOCALE_JA=ja_JP}
if test $LOCALE_JA != none; then
  LC_ALL=$LOCALE_JA            ./test-locale-language${EXEEXT} ja || exit 1
fi

: ${LOCALE_TR_UTF8=tr_TR.UTF-8}
if test $LOCALE_TR_UTF8 != none; then
  LC_ALL=$LOCALE_TR_UTF8       ./test-locale-language${EXEEXT} tr || exit 1
fi

: ${LOCALE_ZH_CN=zh_CN.GB18030}
if test $LOCALE_ZH_CN != none; then
  LC_ALL=$LOCALE_ZH_CN         ./test-locale-language${EXEEXT} zh || exit 1
fi

exit 0

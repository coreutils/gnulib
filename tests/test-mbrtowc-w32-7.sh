#!/bin/sh

# This test is not enabled by default, because MSVCRT does not support and
# will likely never support locales with MB_CUR_MAX > 2.

# Test some UTF-8 locales.
./test-mbrtowc-w32${EXEEXT} French_France Japanese_Japan Chinese_Taiwan Chinese_China 65001

#!/bin/sh

# Test a CP950 locale.
${CHECKER} ./test-mbrtoc16-w32${EXEEXT} Chinese_Taiwan 950

#!/bin/sh

# Test a CP936 locale.
${CHECKER} ./test-mbrtoc16-w32${EXEEXT} Chinese_China 936

#!/bin/sh

# Test a GB18030 locale.
${CHECKER} ./test-mbrtoc16-w32${EXEEXT} Chinese_China 54936

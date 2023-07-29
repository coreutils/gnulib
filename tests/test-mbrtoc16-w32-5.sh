#!/bin/sh

# Test a CP932 locale.
${CHECKER} ./test-mbrtoc16-w32${EXEEXT} Japanese_Japan 932

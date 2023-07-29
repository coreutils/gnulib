#!/bin/sh

# Test a CP1256 locale.
${CHECKER} ./test-mbrtoc16-w32${EXEEXT} "Arabic_Saudi Arabia" 1256

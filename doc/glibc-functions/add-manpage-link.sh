#!/bin/sh
# Adds the reference to the Linux manual page to the file foo.texi.
# Usage: add-manpage-link.sh foo.texi

case "$1" in
  *.texi)
    f="$1"
    g="${f%.texi}"
    if wget https://www.kernel.org/doc/man-pages/online/pages/man3/${g}.3.html >/dev/null 2>&1; then
      if sed -i -e "s|^Gnulib module|Documentation:@* @uref{https://www.kernel.org/doc/man-pages/online/pages/man3/${g}.3.html,,man ${g}}\n\
\n\
Gnulib module|" ${f}; then
        echo $f updated
      else
        echo $f update failed
      fi
    else
      if wget https://www.kernel.org/doc/man-pages/online/pages/man2/${g}.2.html >/dev/null 2>&1; then
        if sed -i -e "s|^Gnulib module|Documentation:@* @uref{https://www.kernel.org/doc/man-pages/online/pages/man2/${g}.2.html,,man ${g}}\n\
\n\
Gnulib module|" ${f}; then
          echo $f updated
        else
          echo $f update failed
        fi
      else
        echo $f not documented among the man pages
      fi
    fi
    ;;
  *) echo "invalid argument: $1" 1>&2 ;;
esac

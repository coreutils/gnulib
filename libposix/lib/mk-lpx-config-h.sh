#! /bin/sh
# -*- Mode: Shell-script -*-

# Copyright (C) 2002-2010 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

infile=$1
outfile=$2

{
    echo '/*'
    sed '1,/^$/d;s/^#/ */;/http:\/\/www\.gnu\.org/q' $0

    g='_LPX_CONFIG_H_GUARD'
    cat <<EOF
 */
#ifndef $g
#define $g 1
/* derived from the libposix config.h by $0 */
EOF

    # 1. strip comments for eyeball verification of name changes
    # 2. Change the #define names
    # 3. Change the #ifdef names
    # 4. Change the "defined XXX" names in "#if" directives
    # 5. Clean up whatever we figure out we've missed.
    #
    sedcmd='
	/^\/\*.*\*\/$/d
	/^\/\*/,/\*\/$/d
	/^$/d
	/#define GNULIB_TEST/d
	s/^\(# *define *\)\([A-Za-z]\)/\1LPX_\2/
	/^#if/s/\(defined *\)\([A-Za-z]\)/\1LPX_\2/g
	s/^\(# *ifn*def *\)\([A-Za-z]\)/\1LPX_\2/
	s/>DIR_FD_MEMBER_NAME/>LPX_DIR_FD_MEMBER_NAME/
	s/_GL_UNUSED/_LPX_UNUSED/
	'

    sed "${sedcmd}" $infile

    echo "#endif /* $g */"
} > $outfile

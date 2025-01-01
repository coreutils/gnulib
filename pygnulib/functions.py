# Copyright (C) 2002-2025 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

from __future__ import annotations

import os.path
from .constants import substart
from .GLConfig import GLConfig


def rewrite_file_name(file_name: str, config: GLConfig, also_tests_lib: bool = False) -> str:
    '''Replace auxdir, docbase, sourcebase, m4base and testsbase from their
    default to the ones specified in a GLConfig object.
    - file_name, the file name to transform,
    - config, the GLConfig storing directory names,
    - also_tests_lib, True if 'tests=lib/' should be replaced.'''
    if type(file_name) is not str:
        raise TypeError(f'file_name must be a str type, not {type(file_name).__name__}')
    auxdir = config['auxdir']
    docbase = config['docbase']
    sourcebase = config['sourcebase']
    m4base = config['m4base']
    testsbase = config['testsbase']
    if file_name.startswith('build-aux/'):
        result = substart('build-aux/', '%s/' % auxdir, file_name)
    elif file_name.startswith('doc/'):
        result = substart('doc/', '%s/' % docbase, file_name)
    elif file_name.startswith('lib/'):
        result = substart('lib/', '%s/' % sourcebase, file_name)
    elif file_name.startswith('m4/'):
        result = substart('m4/', '%s/' % m4base, file_name)
    elif file_name.startswith('tests/'):
        result = substart('tests/', '%s/' % testsbase, file_name)
    elif also_tests_lib and file_name.startswith('tests=lib/'):
        result = substart('tests=lib/', '%s/' % testsbase, file_name)
    elif file_name.startswith('top/'):
        result = substart('top/', '', file_name)
    else:  # file is not a special file
        result = file_name
    return os.path.normpath(result)

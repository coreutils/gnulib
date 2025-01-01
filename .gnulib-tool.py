# Copyright (C) 2024-2025 Free Software Foundation, Inc.
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
#

# This file exists to satisfy the standard layout of a Python program:
#   - a single .py file in the parent directory,
#   - any number of .py files in a subdirectory.
# Explanation: https://stackoverflow.com/questions/16981921/relative-imports-in-python-3
# Without this standard layout, we got
#   - errors at runtime:
#     from . import constants          =>   ImportError: attempted relative import with no known parent package
#     from pygnulib import constants   =>   ModuleNotFoundError: No module named 'pygnulib'
#   - error markers in Eclipse + PyDev.

from pygnulib import main

if __name__ == '__main__':
    main.main_with_exception_handling()

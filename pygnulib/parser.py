#!/usr/bin/python
# encoding: UTF-8
"""gnulib command-line parsing API"""



import argparse as _argparse
import os as _os

from .error import CommandLineError as _CommandLineError

from .config import LGPLv2_LICENSE as _LGPLv2_LICENSE
from .config import LGPLv3_LICENSE as _LGPLv3_LICENSE
from .config import GPLv2_LICENSE as _GPLv2_LICENSE
from .config import LGPL_LICENSE as _LGPL_LICENSE



class CommandLine:
    """gnulib-tool command line configuration"""
    _LIST = (1 << 0)
    _FIND = (1 << 1)
    _IMPORT = (1 << 2)
    _ADD_IMPORT = (1 << 3)
    _REMOVE_IMPORT = (1 << 4)
    _UPDATE = (1 << 5)
    _TEST_DIRECTORY = (1 << 6)
    _MEGA_TEST_DIRECTORY = (1 << 7)
    _TEST = (1 << 8)
    _MEGA_TEST = (1 << 9)
    _COPY_FILE = (1 << 10)
    _EXTRACT_DESCRIPTION = (1 << 11)
    _EXTRACT_COMMENT = (1 << 12)
    _EXTRACT_STATUS = (1 << 13)
    _EXTRACT_NOTICE = (1 << 14)
    _EXTRACT_APPLICABILITY = (1 << 15)
    _EXTRACT_FILELIST = (1 << 16)
    _EXTRACT_DEPENDENCIES = (1 << 17)
    _EXTRACT_AUTOCONF_SNIPPET = (1 << 18)
    _EXTRACT_AUTOMAKE_SNIPPET = (1 << 19)
    _EXTRACT_INCLUDE_DIRECTIVE = (1 << 20)
    _EXTRACT_LINK_DIRECTIVE = (1 << 21)
    _EXTRACT_LICENSE = (1 << 22)
    _EXTRACT_MAINTAINER = (1 << 23)
    _EXTRACT_TESTS_MODULE = (1 << 24)
    _HELP = (1 << 25)
    _ANY_IMPORT = _IMPORT | _ADD_IMPORT | _REMOVE_IMPORT | _UPDATE
    _ANY_TEST = _TEST | _MEGA_TEST | _TEST_DIRECTORY | _MEGA_TEST_DIRECTORY
    _ANY_EXTRACT = _EXTRACT_DESCRIPTION | _EXTRACT_COMMENT | _EXTRACT_STATUS | \
                   _EXTRACT_NOTICE | _EXTRACT_APPLICABILITY | _EXTRACT_FILELIST | \
                   _EXTRACT_DEPENDENCIES | _EXTRACT_AUTOCONF_SNIPPET | \
                   _EXTRACT_AUTOMAKE_SNIPPET | _EXTRACT_INCLUDE_DIRECTIVE | \
                   _EXTRACT_LINK_DIRECTIVE | _EXTRACT_LICENSE | _EXTRACT_MAINTAINER | \
                   _EXTRACT_TESTS_MODULE
    _ALL = _LIST | _FIND | _ANY_IMPORT | _ANY_TEST
    _MODES = (
        (_LIST, "list", ""),
        (_FIND, "find", "filename"),
        (_IMPORT, "import", "[module1 ... moduleN]"),
        (_ADD_IMPORT, "add-import", "[module1 ... moduleN]"),
        (_REMOVE_IMPORT, "remove-import", "[module1 ... moduleN]"),
        (_UPDATE, "update", ""),
        (_TEST_DIRECTORY, "testdir", "--dir=directory [module1 ... moduleN]"),
        (_MEGA_TEST_DIRECTORY, "megatestdir", "--dir=directory [module1 ... moduleN]"),
        (_TEST, "test", "--dir=directory [module1 ... moduleN]"),
        (_MEGA_TEST, "megatest", "--dir=directory [module1 ... moduleN]"),
        (_EXTRACT_DESCRIPTION, "extract-description", "module"),
        (_EXTRACT_COMMENT, "extract-comment", "module"),
        (_EXTRACT_STATUS, "extract-status", "module"),
        (_EXTRACT_NOTICE, "extract-notice", "module"),
        (_EXTRACT_APPLICABILITY, "extract-applicability", "module"),
        (_EXTRACT_FILELIST, "extract-filelist", "module"),
        (_EXTRACT_DEPENDENCIES, "extract-dependencies", "module"),
        (_EXTRACT_AUTOCONF_SNIPPET, "extract-autoconf-snippet", "module"),
        (_EXTRACT_AUTOMAKE_SNIPPET, "extract-automake-snippet", "module"),
        (_EXTRACT_INCLUDE_DIRECTIVE, "extract-include-directive", "module"),
        (_EXTRACT_LINK_DIRECTIVE, "extract-link-directive", "module"),
        (_EXTRACT_LICENSE, "extract-license", "module"),
        (_EXTRACT_MAINTAINER, "extract-maintainer", "module"),
        (_EXTRACT_TESTS_MODULE, "extract-tests-module", "module"),
        (_COPY_FILE, "copy", "file [destination]"),
        (_HELP, "help", ""),
    )
    _LINK_SYMBOLIC = (1 << 0)
    _LINK_HARD = (1 << 1)
    _LINK_LOCAL = (1 << 2)
    _LINK_NOTICE = (1 << 3)


    class _Option(_argparse.Action):
        def __init__(self, *args, **kwargs):
            self.__flags = kwargs.pop("flags")
            self.__options = kwargs["option_strings"]
            super().__init__(default=_argparse.SUPPRESS, *args, **kwargs)

        def __call__(self, parser, namespace, value, option=None):
            if hasattr(namespace, "mode"):
                mode = getattr(namespace, "mode")
                mode = mode if option != "--help" else CommandLine._HELP
                if mode and not (self.__flags & mode) and (mode != CommandLine._HELP):
                    mode = "--" + {k:v for (k, v, _) in CommandLine._MODES}[mode]
                    fmt = "argument {0}: not allowed with {1}"
                    parser.error(fmt.format(mode, option))
            setattr(namespace, self.dest, value)


    class _ConstOption(_Option):
        def __init__(self, *args, **kwargs):
            self.__const = kwargs.pop("const")
            super().__init__(nargs=0, *args, **kwargs)

        def __call__(self, parser, namespace, value, option=None):
            args = (parser, namespace, self.__const, option)
            setattr(parser, self.dest, self.__const)
            super().__call__(*args)


    class _TrueOption(_ConstOption):
        def __init__(self, *args, **kwargs):
            super().__init__(const=True, *args, **kwargs)


    class _FalseOption(_ConstOption):
        def __init__(self, *args, **kwargs):
            super().__init__(const=False, *args, **kwargs)


    class _ModeOption(_Option):
        def __init__(self, *args, **kwargs):
            mode = kwargs["const"]
            kwargs["dest"] = "mode"
            kwargs["nargs"] = 0
            if mode & CommandLine._UPDATE:
                kwargs["nargs"] = 0
            elif mode & CommandLine._ANY_IMPORT \
            or mode & CommandLine._ANY_TEST \
            or mode & CommandLine._ANY_EXTRACT:
                kwargs["nargs"] = "*"
                kwargs["metavar"] = "module0 ... moduleN"
            elif mode & CommandLine._FIND:
                kwargs.pop("nargs")
                kwargs["metavar"] = "FILE"
            elif mode & CommandLine._COPY_FILE:
                kwargs["nargs"] = "+"
                kwargs["metavar"] = "SRC [DST]"
            super().__init__(*args, **kwargs)

        def __call__(self, parser, namespace, value, option=None):
            mode_option = {}
            option_mode = {}
            for (mode, name, _) in CommandLine._MODES:
                name = "--{}".format(name)
                mode_option[mode] = name
                option_mode[name] = mode
            modules = value
            new_option = option
            new_mode = value = option_mode[option]
            old_mode = getattr(namespace, self.dest, 0)
            old_option = mode_option[old_mode] if old_mode != 0 else ""
            if (new_mode & (CommandLine._UPDATE | CommandLine._HELP)) == 0:
                setattr(namespace, "modules", set(modules))
            value = CommandLine._HELP if CommandLine._HELP in {old_mode, new_mode} else new_mode
            super().__call__(parser, namespace, value, option)


    class _AvoidOption(_Option):
        def __call__(self, parser, namespace, value, option=None):
            if not hasattr(namespace, self.dest):
                setattr(namespace, self.dest, list())
            value = set(getattr(namespace, self.dest) | {value})
            super().__call__(parser, namespace, value, option)


    class _OverrideOption(_Option):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, **kwargs)

        def __call__(self, parser, namespace, value, option=None):
            if not hasattr(namespace, self.dest):
                setattr(namespace, self.dest, list())
            overrides = getattr(namespace, self.dest)
            overrides.append(value)


    class _VerbosityOption(_Option):
        def __init__(self, *args, **kwargs):
            super().__init__(nargs=0, *args, **kwargs)

        def __call__(self, parser, namespace, value, option=None):
            if not hasattr(namespace, self.dest):
                setattr(namespace, self.dest, 0)
            value = getattr(namespace, self.dest)
            verbose = option in ("-v", "--verbose")
            value += +1 if verbose else -1
            super().__call__(parser, namespace, value, option)


    class _LGPLOption(_Option):
        def __call__(self, parser, namespace, value, option=None):
            if value not in {"2", "3", "yes", "3orGPLv2"}:
                parser.error("argument --lgpl: 2, 3, yes or 3orGPLv2")
            value = {
                "2": _LGPLv2_LICENSE,
                "3": _LGPLv3_LICENSE,
                "yes": _LGPL_LICENSE,
                "3orGPLv2": (_GPLv2_LICENSE | _LGPLv3_LICENSE),
            }[value]
            super().__call__(parser, namespace, value, option)


    class _CopyModeOption(_Option):
        def __init__(self, *args, **kwargs):
            super().__init__(nargs=0, *args, **kwargs)

        def __call__(self, parser, namespace, value, option=None):
            if not hasattr(namespace, self.dest):
                setattr(namespace, self.dest, None)
            if option.endswith("symlink"):
                value = "symlink"
            elif option.endswith("hardlink"):
                value = "hardlink"
            else:
                fmt = "{}: None, 'symlink' or 'hardlink'"
                raise ValueError(fmt.format(self.dest))
            super().__call__(parser, namespace, value, option)


    class _CopyrightsCopyModeOption(_CopyModeOption):
        def __call__(self, parser, namespace, value, option=None):
            if not hasattr(namespace, self.dest):
                setattr(namespace, "copyrights", False)
            if option.endswith("symlink"):
                value = "symlink"
            elif option.endswith("hardlink"):
                value = "hardlink"
            else:
                fmt = "{}: None, 'symlink' or 'hardlink'"
                raise ValueError(fmt.format(self.dest))
            setattr(namespace, "copyrights", True)
            super().__call__(parser, namespace, value, option)


    # section0: (section0_modes, (([section0_option0, section0_optionN], **section0_kwargs)))
    # sectionN: (sectionN_modes, (([sectionN_option0, sectionN_optionN], **sectionN_kwargs)))
    #
    # for (name, flags, arguments) in sections:
    #     for argument in arguments:
    #         (options, kwargs) = argument
    _SECTIONS_ = (
        (
            "Operation modes",
            0,
            (
                (["-l", "--list"], {
                    "help": (
                        "print the available module names",
                    ),
                    "action": _ModeOption,
                    "const": _LIST,
                }),
                (["-f", "--find"], {
                    "help": (
                        "find the modules which contain the specified file",
                    ),
                    "action": _ModeOption,
                    "const": _FIND,
                }),
                (["-i", "--import"], {
                    "help": (
                        "import the given modules into the current package",
                    ),
                    "action": _ModeOption,
                    "const": _IMPORT,
                }),
                (["-a", "--add-import"], {
                    "help": (
                        "augment the list of imports from gnulib into the",
                        "current package, by adding the given modules;",
                        "if no modules are specified, update the current",
                        "package from the current gnulib",
                    ),
                    "action": _ModeOption,
                    "const": _ADD_IMPORT,
                }),
                (["-r", "--remove-import"], {
                    "help": (
                        "reduce the list of imports from gnulib into the",
                        "current package, by removing the given modules",
                    ),
                    "action": _ModeOption,
                    "const": _REMOVE_IMPORT,
                }),
                (["-u", "--update"], {
                    "help": (
                        "update the current package, restore files omitted",
                        "from version control",
                    ),
                    "action": _ModeOption,
                    "const": _UPDATE,
                }),

                (["--extract-description"], {
                    "help": (
                        "extract the description",
                    ),
                    "action": _ModeOption,
                    "const": _EXTRACT_DESCRIPTION,
                }),
                (["--extract-comment"], {
                    "help": (
                        "extract the comment",
                    ),
                    "action": _ModeOption,
                    "const": _EXTRACT_COMMENT,
                }),
                (["--extract-status"], {
                    "help": (
                        "extract the status (obsolete etc.)",
                    ),
                    "action": _ModeOption,
                    "const": _EXTRACT_STATUS,
                }),
                (["--extract-notice"], {
                    "help": (
                        "extract the notice or banner",
                    ),
                    "action": _ModeOption,
                    "const": _EXTRACT_NOTICE,
                }),
                (["--extract-applicability"], {
                    "help": (
                        "extract the applicability",
                    ),
                    "action": _ModeOption,
                    "const": _EXTRACT_APPLICABILITY,
                }),
                (["--extract-filelist"], {
                    "help": (
                        "extract the list of files",
                    ),
                    "action": _ModeOption,
                    "const": _EXTRACT_FILELIST,
                }),
                (["--extract-dependencies"], {
                    "help": (
                        "extract the dependencies",
                    ),
                    "action": _ModeOption,
                    "const": _EXTRACT_DEPENDENCIES,
                }),
                (["--extract-autoconf-snippet"], {
                    "help": (
                        "extract the snippet for configure.ac",
                    ),
                    "action": _ModeOption,
                    "const": _EXTRACT_AUTOCONF_SNIPPET,
                }),
                (["--extract-automake-snippet"], {
                    "help": (
                        "extract the snippet for library makefile",
                    ),
                    "action": _ModeOption,
                    "const": _EXTRACT_AUTOMAKE_SNIPPET,
                }),
                (["--extract-include-directive"], {
                    "help": (
                        "extract the #include directive",
                    ),
                    "action": _ModeOption,
                    "const": _EXTRACT_INCLUDE_DIRECTIVE,
                }),
                (["--extract-link-directive"], {
                    "help": (
                        "extract the linker directive",
                    ),
                    "action": _ModeOption,
                    "const": _EXTRACT_LINK_DIRECTIVE,
                }),
                (["--extract-license"], {
                    "help": (
                        "report the license terms of the source files",
                        "under lib/",
                    ),
                    "action": _ModeOption,
                    "const": _EXTRACT_LICENSE,
                }),
                (["--extract-maintainer", "--extract-maintainers"], {
                    "help": (
                        "report the maintainer(s) inside gnulib",
                    ),
                    "action": _ModeOption,
                    "const": _EXTRACT_MAINTAINER,
                }),
                (["--extract-tests-module"], {
                    "help": (
                        "report the unit test module, if it exists",
                    ),
                    "action": _ModeOption,
                    "const": _EXTRACT_TESTS_MODULE,
                }),

                (["--copy-file"], {
                    "help": (
                        "copy a file that is not part of any module",
                    ),
                    "action": _ModeOption,
                    "const": _COPY_FILE,
                }),

                (["--help"], {
                    "help": (
                        "show this help text",
                    ),
                    "action": _ModeOption,
                    "const": _HELP,
                }),
            ),
        ),


        (
            "General options",
            _ALL,
            (
                (["--dir"], {
                    "help": (
                        "specify the target directory; on --import,",
                        "this specifies where your configure.ac",
                        "can be found; defaults to current directory",
                    ),
                    "action": _Option,
                    "dest": "root",
                    "metavar": "DIRECTORY",
                }),
                (["--local-dir"], {
                    "help": (
                        "pecify a local override directory where to look",
                        "up files before looking in gnulib's directory",
                    ),
                    "action": _OverrideOption,
                    "dest": "overrides",
                    "metavar": "DIRECTORY",
                }),
                (["-v", "--verbose"], {
                    "help": (
                        "increase verbosity; may be repeated",
                    ),
                    "action": _VerbosityOption,
                    "dest": "verbosity",
                }),
                (["-q", "--quiet"], {
                    "help": (
                        "decrease verbosity; may be repeated",
                    ),
                    "action": _VerbosityOption,
                    "dest": "verbosity",
                }),
            ),
        ),


        (
            "Options for --import, --add/remove-import, --update",
            _ANY_IMPORT,
            (
                (["--dry-run"], {
                    "help": (
                        "only print what would have been done",
                    ),
                    "action": _TrueOption,
                    "dest": "dry_run",
                }),
            ),
        ),


        (
            "Options for --import, --add/remove-import",
            (_IMPORT | _ADD_IMPORT | _REMOVE_IMPORT),
            (
                (["--with-tests"], {
                    "help": (
                        "include unit tests for the included modules",
                    ),
                    "action": _TrueOption,
                    "dest": "tests",
                }),
                (["--single-configure"], {
                    "help": (
                        "generate a single configure file, not a separate",
                        "configure file for the tests directory",
                    ),
                    "action": _TrueOption,
                    "dest": "single_configure",
                }),
            ),
        ),


        (
            "Options for --create-[mega]testdir, --[mega]test",
            _ANY_TEST,
            (
                (["--without-tests"], {
                    "help": (
                        "don't include unit tests for the included modules",
                    ),
                    "action": _FalseOption,
                    "dest": "tests",
                }),
            ),
        ),


        (
            "Options for --import, --add/remove-import,"
            "\n"
            "            --create-[mega]testdir, --[mega]test",
            (_ANY_IMPORT & ~_UPDATE) | _ANY_TEST,
            (
                (["--with-obsolete"], {
                    "help": (
                        "include obsolete modules when they occur among the",
                        "dependencies; by default, dependencies to obsolete",
                        "modules are ignored",
                    ),
                    "action": _TrueOption,
                    "dest": "obsolete",
                }),

                (["--with-c++-tests"], {
                    "help": (
                        "include even unit tests for C++ interoperability",
                    ),
                    "action": _TrueOption,
                    "dest": "cxx_tests",
                }),
                (["--without-c++-tests"], {
                    "help": (
                        "exclude unit tests for C++ interoperability",
                    ),
                    "action": _FalseOption,
                    "dest": "cxx_tests",
                }),

                (["--with-longrunning-tests"], {
                    "help": (
                        "include even unit tests that are long-runners",
                    ),
                    "action": _TrueOption,
                    "dest": "longrunning_tests",
                }),
                (["--without-longrunning-tests"], {
                    "help": (
                        "exclude unit tests that are long-runners",
                    ),
                    "action": _FalseOption,
                    "dest": "longrunning_tests",
                }),

                (["--with-privileged-tests"], {
                    "help": (
                        "include even unit tests that require root privileges",
                    ),
                    "action": _TrueOption,
                    "dest": "privileged_tests",
                }),
                (["--without-privileged-tests"], {
                    "help": (
                        "exclude unit tests that require root privileges",
                    ),
                    "action": _FalseOption,
                    "dest": "privileged_tests",
                }),

                (["--with-unportable-tests"], {
                    "help": (
                        "include even unit tests that fail on some platforms",
                    ),
                    "action": _TrueOption,
                    "dest": "unportable_tests",
                }),
                (["--without-unportable-tests"], {
                    "help": (
                        "exclude unit tests that fail on some platforms",
                    ),
                    "action": _FalseOption,
                    "dest": "unportable_tests",
                }),

                (["--with-all-tests"], {
                    "help": (
                        "include all kinds of problematic unit tests",
                    ),
                    "action": _TrueOption,
                    "dest": "all_tests",
                }),
                (["--avoid"], {
                    "help": (
                        "avoid including the given MODULE; useful if you",
                        "have code that provides equivalent functionality;",
                        "this option can be repeated",
                    ),
                    "action": _AvoidOption,
                    "nargs": 1,
                    "metavar": "MODULE",
                }),
                (["--conditional-dependencies"], {
                    "help": (
                        "support conditional dependencies (may save configure",
                        "time and object code)",
                    ),
                    "action": _TrueOption,
                    "dest": "conditionals",
                }),
                (["--no-conditional-dependencies"], {
                    "help": (
                        "don't use conditional dependencies",
                    ),
                    "action": _FalseOption,
                    "dest": "conditionals",
                }),
                (["--libtool"], {
                    "help": (
                        "use libtool rules",
                    ),
                    "action": _TrueOption,
                    "dest": "libtool",
                }),
                (["--no-libtool"], {
                    "help": (
                        "don't use libtool rules",
                    ),
                    "action": _FalseOption,
                    "dest": "libtool",
                }),
            ),
        ),


        (
            "Options for --import, --add/remove-import",
            (_ANY_IMPORT & ~_UPDATE),
            (
                (["--lib"], {
                    "help": (
                        "specify the library name; defaults to 'libgnu'",
                    ),
                    "action": _Option,
                    "dest": "libname",
                    "metavar": "LIBRARY",
                }),
                (["--source-base"], {
                    "help": (
                        "directory relative to --dir where source code is",
                        "placed (default \"lib\")",
                    ),
                    "action": _Option,
                    "metavar": "DIRECTORY",
                }),
                (["--m4-base"], {
                    "help": (
                        "directory relative to --dir where *.m4 macros are",
                        "placed (default \"m4\")",
                    ),
                    "action": _Option,
                    "metavar": "DIRECTORY",
                }),
                (["--po-base"], {
                    "help": (
                        "directory relative to --dir where *.po files are",
                        "placed (default \"po\")",
                    ),
                    "action": _Option,
                    "metavar": "DIRECTORY",
                }),
                (["--doc-base"], {
                    "help": (
                        "directory relative to --dir where doc files are",
                        "placed (default \"doc\")",
                    ),
                    "action": _Option,
                    "metavar": "DIRECTORY",
                }),
                (["--tests-base"], {
                    "help": (
                        "directory relative to --dir where unit tests are",
                        "placed (default \"tests\")",
                    ),
                    "action": _Option,
                    "metavar": "DIRECTORY",
                }),
                (["--aux-dir"], {
                    "help": (
                        "directory relative to --dir where auxiliary build",
                        "tools are placed (default comes from configure.ac);",
                    ),
                    "action": _Option,
                    "dest": "auxdir",
                    "metavar": "DIRECTORY",
                }),
                (["--gnu-make"], {
                    "help": (
                        "output for GNU Make instead of for the default",
                        "Automake",
                    ),
                    "action": _TrueOption,
                    "dest": "gnumake",
                }),
                (["--lgpl"], {
                    "help": (
                        "abort if modules aren't available under the LGPL;",
                        "also modify license template from GPL to LGPL;",
                        "the version number of the LGPL can be specified;",
                        "the default is currently LGPLv3.",
                    ),
                    "action": _LGPLOption,
                    "dest": "licenses",
                    "metavar": "[=2|=3orGPLv2|=3]",
                }),
                (["--makefile-name"], {
                    "help": (
                        "name of makefile in automake syntax in the",
                        "source-base and tests-base directories",
                        "(default \"Makefile.am\")",
                    ),
                    "action": _Option,
                    "metavar": "NAME",
                }),
                (["--macro-prefix"], {
                    "help": (
                        "specify the prefix of the macros 'gl_EARLY' and",
                        "'gl_INIT'; default is 'gl'",
                    ),
                    "action": _Option,
                    "metavar": "PREFIX",
                }),
                (["--po-domain"], {
                    "help": (
                        "specify the prefix of the i18n domain; usually use",
                        "the package name; a suffix '-gnulib' is appended",
                    ),
                    "action": _Option,
                    "metavar": "NAME",
                }),
                (["--witness-c-macro"], {
                    "help": (
                        "specify the C macro that is defined when the",
                        "sources in this directory are compiled or used",
                    ),
                    "action": _Option,
                    "metavar": "NAME",
                }),
                (["--vc-files"], {
                    "help": (
                        "update version control related files",
                        "(.gitignore and/or .cvsignore)",
                    ),
                    "action": _TrueOption,
                    "dest": "vc_files",
                }),
                (["--no-vc-files"], {
                    "help": (
                        "don't update version control related files",
                        "(.gitignore and/or .cvsignore)",
                    ),
                    "action": _FalseOption,
                    "dest": "libtool",
                }),
                (["--no-changelog"], {
                    "help": (
                        "don't update or create ChangeLog files;",
                        "this option is currently deprecated",
                    ),
                    "action": _ConstOption,
                    "const": None,
                }),
            ),
        ),


        (
            "Options for --import, --add/remove-import, --update"
            "\n"
            "            --create-[mega]testdir, --[mega]test",
            _ANY_IMPORT | _ANY_TEST,
            (
                (["-s", "--symlink"], {
                    "help": (
                        "make symbolic links instead of copying files",
                    ),
                    "action": _CopyModeOption,
                    "dest": "copymode",
                }),
                (["--local-symlink"], {
                    "help": (
                        "make symbolic links instead of copying files, only",
                        "for files from the local override directory"
                    ),
                    "action": _CopyModeOption,
                    "dest": "local_copymode",
                }),
                (["-h", "--hardlink"], {
                    "help": (
                        "make hard links instead of copying files",
                    ),
                    "action": _CopyModeOption,
                    "dest": "copymode",
                }),
                (["--local-hardlink"], {
                    "help": (
                        "make hard links instead of copying files, only",
                        "for files from the local override directory"
                    ),
                    "action": _CopyModeOption,
                    "dest": "local_copymode",
                }),
            ),
        ),


        (
            "Options for --import, --add/remove-import, --update",
            _ANY_IMPORT,
            (
                (["-S", "--more-symlink"], {
                    "help": (
                        "make symbolic links instead of copying files and",
                        "don't replace copyright notices",
                    ),
                    "action": _CopyrightsCopyModeOption,
                    "dest": "copymode",
                }),
                (["-H", "--more-hardlink"], {
                    "help": (
                        "make symbolic links instead of copying files and",
                        "don't replace copyright notices",
                    ),
                    "action": _CopyrightsCopyModeOption,
                    "dest": "copymode",
                }),
            ),
        ),
    )


    def __error(self, message):
        raise _CommandLineError(self.__program, message)


    def __format_help(self):
        lines = [""]
        for (name, _, args) in CommandLine._SECTIONS_:
            offset = -1
            lines += ["", "%s:" % name, ""]
            for arg in args:
                (options, kwargs) = arg
                options = ", ".join(options)
                if "metavar" in kwargs:
                    options += (" " + kwargs["metavar"])
                length = len(options)
                if length > offset:
                    offset = length
            fmt1 = "      %-{0}s    %s".format(offset)
            fmt2 = "      " + " " * offset + "    %s"
            for arg in args:
                (options, kwargs) = arg
                options = ", ".join(options)
                if "metavar" in kwargs:
                    sep = "" if "choices" in kwargs else "="
                    options += ("%s%s" % (sep, kwargs["metavar"]))
                description = iter(kwargs["help"])
                line = next(description)
                lines += [fmt1 % (options, line)]
                for line in description:
                    lines += [fmt2 % line]
            lines += [""]
        return self.__format_usage()[:-1] + "\n".join(lines)


    def __format_usage(self):
        iterable = iter(CommandLine._MODES)
        (_, cmd, args) = next(iterable)
        fmt = (" --{cmd}" + (" {args}" if args else ""))
        lines = ["usage: {program}" + fmt.format(cmd=cmd, args=args)]
        for (_, cmd, args) in iterable:
            fmt = (" --{cmd}" + (" {args}" if args else ""))
            lines += ["       {program}" + fmt.format(cmd=cmd, args=args)]
        lines += ["", ""]
        return "\n".join(lines).format(program=self.__program)


    def __init__(self, program):
        self.__parser = _argparse.ArgumentParser(prog=program, add_help=False)
        for (options, kwargs) in CommandLine._SECTIONS_[0][2]:
            flags = kwargs.get("const")
            self.__parser.add_argument(*options, **kwargs, flags=flags)
        for (_, flags, args) in CommandLine._SECTIONS_[1:]:
            for (options, kwargs) in args:
                self.__parser.add_argument(*options, **kwargs, flags=flags)
        self.__program = _os.path.basename(program)
        self.__parser.error = self.__error
        self.__parser.format_help = self.__format_help
        self.__parser.format_usage = self.__format_usage


    @property
    def usage(self):
        """usage message"""
        return self.__format_usage()[:-1]


    @property
    def help(self):
        """help message"""
        return self.__format_help()


    def parse(self, arguments):
        """
        self.parse(arguments) -> (namespace, mode, verbosity, options)

        Parse the command-line arguments and return a new configuration.
        The default configuration holds default parameter values.
        The parameter is overriden if and only if it was set explicitly.

        namespace: a dictionary to use upon pygnulib.config.Base construction
        mode: a string representing the mode ("import", "add-import", etc.)
        verbosity: an integer representing the desired verbosity level
        options: a combination of CommandLine.Option flags
        """
        (namespace, arguments) = self.__parser.parse_known_args(arguments)
        (namespace, options) = (vars(namespace), 0)
        mode = namespace.pop("mode", None)
        if mode is None:
            self.__parser.error("no operating mode selected")
        if arguments and mode != CommandLine._HELP:
            fmt = "unrecognized arguments: {0}"
            arguments = " ".join(arguments)
            self.__parser.error(fmt.format(arguments))
        mode = {k:v for (k, v, _) in CommandLine._MODES}[mode]
        if mode in {"import", "add-import", "remove-import"} and not namespace["modules"]:
            fmt = "--{0}: expected at least one argument"
            self.__parser.error(fmt.format(mode))
        verbosity = namespace.pop("verbosity", 0)
        namespace.pop("no_changelog", None)

        options = dict(namespace)
        namespace.setdefault("root", ".")
        options.setdefault("dry_run", False)
        namespace["overrides"] = list(reversed(namespace.get("overrides", [])))
        return (namespace, mode, verbosity, options)

#!/usr/bin/python
# encoding: UTF-8



import argparse as _argparse_
import enum as _enum_
import os as _os_

from .config import Base as _BaseConfig_
from .error import CommandLineError as _CommandLineError_



class CommandLine:
    """gnulib-tool command line configuration"""
    _LIST_ = (1 << 0)
    _FIND_ = (1 << 1)
    _IMPORT_ = (1 << 2)
    _ADD_IMPORT_ = (1 << 3)
    _REMOVE_IMPORT_ = (1 << 4)
    _UPDATE_ = (1 << 5)
    _TEST_DIRECTORY_ = (1 << 6)
    _MEGA_TEST_DIRECTORY_ = (1 << 7)
    _TEST_ = (1 << 8)
    _MEGA_TEST_ = (1 << 9)
    _COPY_FILE_ = (1 << 10)
    _EXTRACT_DESCRIPTION_ = (1 << 11)
    _EXTRACT_COMMENT_ = (1 << 12)
    _EXTRACT_STATUS_ = (1 << 13)
    _EXTRACT_NOTICE_ = (1 << 14)
    _EXTRACT_APPLICABILITY_ = (1 << 15)
    _EXTRACT_FILELIST_ = (1 << 16)
    _EXTRACT_DEPENDENCIES_ = (1 << 17)
    _EXTRACT_AUTOCONF_SNIPPET_ = (1 << 18)
    _EXTRACT_AUTOMAKE_SNIPPET_ = (1 << 19)
    _EXTRACT_INCLUDE_DIRECTIVE_ = (1 << 20)
    _EXTRACT_LINK_DIRECTIVE_ = (1 << 21)
    _EXTRACT_LICENSE_ = (1 << 22)
    _EXTRACT_MAINTAINER_ = (1 << 23)
    _EXTRACT_TESTS_MODULE_ = (1 << 24)
    _HELP_ = (1 << 25)
    _ANY_IMPORT_ = _IMPORT_ | _ADD_IMPORT_ | _REMOVE_IMPORT_ | _UPDATE_
    _ANY_TEST_ = _TEST_ | _MEGA_TEST_ | _TEST_DIRECTORY_ | _MEGA_TEST_DIRECTORY_
    _ALL_ = _LIST_ | _FIND_ | _ANY_IMPORT_ | _ANY_TEST_
    _MODES_ = (
        (_LIST_, "list", ""),
        (_FIND_, "find", "filename"),
        (_IMPORT_, "import", "[module1 ... moduleN]"),
        (_ADD_IMPORT_, "add-import", "[module1 ... moduleN]"),
        (_REMOVE_IMPORT_, "remove-import", "[module1 ... moduleN]"),
        (_UPDATE_, "update", ""),
        (_TEST_DIRECTORY_, "testdir", "--dir=directory [module1 ... moduleN]"),
        (_MEGA_TEST_DIRECTORY_, "megatestdir", "--dir=directory [module1 ... moduleN]"),
        (_TEST_, "test", "--dir=directory [module1 ... moduleN]"),
        (_MEGA_TEST_, "megatest", "--dir=directory [module1 ... moduleN]"),
        (_EXTRACT_DESCRIPTION_, "extract-description", "module"),
        (_EXTRACT_COMMENT_, "extract-comment", "module"),
        (_EXTRACT_STATUS_, "extract-status", "module"),
        (_EXTRACT_NOTICE_, "extract-notice", "module"),
        (_EXTRACT_APPLICABILITY_, "extract-applicability", "module"),
        (_EXTRACT_FILELIST_, "extract-filelist", "module"),
        (_EXTRACT_DEPENDENCIES_, "extract-dependencies", "module"),
        (_EXTRACT_AUTOCONF_SNIPPET_, "extract-autoconf-snippet", "module"),
        (_EXTRACT_AUTOMAKE_SNIPPET_, "extract-automake-snippet", "module"),
        (_EXTRACT_INCLUDE_DIRECTIVE_, "extract-include-directive", "module"),
        (_EXTRACT_LINK_DIRECTIVE_, "extract-link-directive", "module"),
        (_EXTRACT_LICENSE_, "extract-license", "module"),
        (_EXTRACT_MAINTAINER_, "extract-maintainer", "module"),
        (_EXTRACT_TESTS_MODULE_, "extract-tests-module", "module"),
        (_COPY_FILE_, "copy", "file [destination]"),
        (_HELP_, "help", ""),
    )
    _LINK_SYMBOLIC_ = (1 << 0)
    _LINK_HARD_ = (1 << 1)
    _LINK_LOCAL_ = (1 << 2)
    _LINK_NOTICE_ = (1 << 3)


    class _ModeAction_(_argparse_.Action):
        def __init__(self, *args, **kwargs):
            mode = kwargs["const"]
            kwargs["dest"] = "mode"
            kwargs["nargs"] = 0
            if mode & CommandLine._UPDATE_:
                kwargs["nargs"] = 0
            elif mode & CommandLine._ANY_IMPORT_ or mode & CommandLine._ANY_TEST_:
                kwargs["nargs"] = "+"
                kwargs["metavar"] = "module0 ... moduleN"
            elif mode & CommandLine._FIND_:
                kwargs.pop("nargs")
                kwargs["metavar"] = "FILE"
            elif mode & CommandLine._COPY_FILE_:
                kwargs["nargs"] = "+"
                kwargs["metavar"] = "SRC [DST]"
            super().__init__(*args, **kwargs)


        def __call__(self, parser, namespace, value, option=None):
            old_option = ""
            new_option = option
            new_mode = None
            old_mode = getattr(namespace, self.dest)
            for (mode, name, _) in CommandLine._MODES_:
                option = ("--" + name)
                if mode == old_mode:
                    old_option = option
                if option == new_option:
                    new_mode = mode
                if old_option and new_mode:
                    break
            if old_mode != new_mode:
                if not old_mode is None:
                    fmt = "argument {0}: not allowed with {1}"
                    parser.error(fmt.format(new_option, old_option))
                setattr(namespace, "modules", list(value))
                setattr(namespace, self.dest, new_mode)


    class _AvoidAction_(_argparse_.Action):
        def __call__(self, parser, namespace, value, option=None):
            values = getattr(namespace, self.dest)
            values += value


    class _VerboseAction_(_argparse_.Action):
        def __call__(self, parser, namespace, value, option=None):
            value = getattr(namespace, self.dest)
            verbose = option in ("-v", "--verbose")
            value += +1 if verbose else -1
            setattr(namespace, self.dest, value)


    class _LinkAction_(_argparse_.Action):
        def __call__(self, parser, namespace, value, option=None):
            flags = getattr(namespace, self.dest)
            symlink = ("-s", "--symlink", "--local-symlink", "-S", "--more-symlink")
            hardlink = ("-h", "--hardlink", "--local-hardlink", "-H", "--more-hardlink")
            local = ("--local-symlink", "--local-hardlink")
            disable_notice = ("-S", "--more-symlink", "-H", "--more-hardlink")
            if option in symlink:
                if flags & CommandLine._LINK_HARD_:
                    parser.error("conflicting --symlink and --hardlink options")
                flags |= CommandLine._LINK_SYMBOLIC_
            if option in hardlink:
                if flags & CommandLine._LINK_SYMBOLIC_:
                    parser.error("conflicting --symlink and --hardlink options")
                flags |= CommandLine._LINK_HARD_
            if option in local:
                flags |= CommandLine._LINK_LOCAL_
            if option in disable_notice:
                flags &= ~CommandLine._LINK_NOTICE_
            setattr(namespace, self.dest, flags)


    # section0: (section0_modes, (([section0_option0, section0_optionN], **section0_kwargs)))
    # sectionN: (sectionN_modes, (([sectionN_option0, sectionN_optionN], **sectionN_kwargs)))
    #
    # for (name, flags, arguments) in sections:
    #     for argument in arguments:
    #         (options, kwargs) = argument
    _SECTIONS_ = (
        (
            "Operation modes",
            None,
            (
                (["-l", "--list"], {
                    "help": (
                        "print the available module names",
                    ),
                    "action": _ModeAction_,
                    "const": _LIST_,
                }),
                (["-f", "--find"], {
                    "help": (
                        "find the modules which contain the specified file",
                    ),
                    "action": _ModeAction_,
                    "const": _FIND_,
                }),
                (["-i", "--import"], {
                    "help": (
                        "import the given modules into the current package",
                    ),
                    "action": _ModeAction_,
                    "const": _IMPORT_,
                }),
                (["-a", "--add-import"], {
                    "help": (
                        "augment the list of imports from gnulib into the",
                        "current package, by adding the given modules;",
                        "if no modules are specified, update the current",
                        "package from the current gnulib",
                    ),
                    "action": _ModeAction_,
                    "const": _ADD_IMPORT_,
                }),
                (["-r", "--remove-import"], {
                    "help": (
                        "reduce the list of imports from gnulib into the",
                        "current package, by removing the given modules",
                    ),
                    "action": _ModeAction_,
                    "const": _REMOVE_IMPORT_,
                }),
                (["-u", "--update"], {
                    "help": (
                        "update the current package, restore files omitted",
                        "from version control",
                    ),
                    "action": _ModeAction_,
                    "const": _UPDATE_,
                }),

                (["--extract-description"], {
                    "help": (
                        "extract the description",
                    ),
                    "action": _ModeAction_,
                    "const": _EXTRACT_DESCRIPTION_,
                }),
                (["--extract-comment"], {
                    "help": (
                        "extract the comment",
                    ),
                    "action": _ModeAction_,
                    "const": _EXTRACT_COMMENT_,
                }),
                (["--extract-status"], {
                    "help": (
                        "extract the status (obsolete etc.)",
                    ),
                    "action": _ModeAction_,
                    "const": _EXTRACT_STATUS_,
                }),
                (["--extract-notice"], {
                    "help": (
                        "extract the notice or banner",
                    ),
                    "action": _ModeAction_,
                    "const": _EXTRACT_NOTICE_,
                }),
                (["--extract-applicability"], {
                    "help": (
                        "extract the applicability",
                    ),
                    "action": _ModeAction_,
                    "const": _EXTRACT_APPLICABILITY_,
                }),
                (["--extract-filelist"], {
                    "help": (
                        "extract the list of files",
                    ),
                    "action": _ModeAction_,
                    "const": _EXTRACT_FILELIST_,
                }),
                (["--extract-dependencies"], {
                    "help": (
                        "extract the dependencies",
                    ),
                    "action": _ModeAction_,
                    "const": _EXTRACT_DEPENDENCIES_,
                }),
                (["--extract-autoconf-snippet"], {
                    "help": (
                        "extract the snippet for configure.ac",
                    ),
                    "action": _ModeAction_,
                    "const": _EXTRACT_AUTOCONF_SNIPPET_,
                }),
                (["--extract-automake-snippet"], {
                    "help": (
                        "extract the snippet for library makefile",
                    ),
                    "action": _ModeAction_,
                    "const": _EXTRACT_AUTOMAKE_SNIPPET_,
                }),
                (["--extract-include-directive"], {
                    "help": (
                        "extract the #include directive",
                    ),
                    "action": _ModeAction_,
                    "const": _EXTRACT_INCLUDE_DIRECTIVE_,
                }),
                (["--extract-link-directive"], {
                    "help": (
                        "extract the linker directive",
                    ),
                    "action": _ModeAction_,
                    "const": _EXTRACT_LINK_DIRECTIVE_,
                }),
                (["--extract-license"], {
                    "help": (
                        "report the license terms of the source files",
                        "under lib/",
                    ),
                    "action": _ModeAction_,
                    "const": _EXTRACT_LICENSE_,
                }),
                (["--extract-maintainer"], {
                    "help": (
                        "report the maintainer(s) inside gnulib",
                    ),
                    "action": _ModeAction_,
                    "const": _EXTRACT_MAINTAINER_,
                }),
                (["--extract-tests-module"], {
                    "help": (
                        "report the unit test module, if it exists",
                    ),
                    "action": _ModeAction_,
                    "const": _EXTRACT_TESTS_MODULE_,
                }),

                (["--copy-file"], {
                    "help": (
                        "copy a file that is not part of any module",
                    ),
                    "action": _ModeAction_,
                    "const": _COPY_FILE_,
                }),

                (["--help"], {
                    "help": (
                        "show this help text",
                    ),
                    "action": _ModeAction_,
                    "const": _HELP_,
                }),
            ),
        ),


        (
            "General options",
            _ALL_,
            (
                (["--dir"], {
                    "help": (
                        "specify the target directory; on --import,",
                        "this specifies where your configure.ac",
                        "can be found; defaults to current directory",
                    ),
                    "dest": "root",
                    "default": _argparse_.SUPPRESS,
                    "metavar": "DIRECTORY",
                }),
                (["--local-dir"], {
                    "help": (
                        "pecify a local override directory where to look",
                        "up files before looking in gnulib's directory",
                    ),
                    "dest": "local",
                    "default": _argparse_.SUPPRESS,
                    "nargs": 1,
                    "metavar": "DIRECTORY",
                }),
                (["-v", "--verbose"], {
                    "help": (
                        "increase verbosity; may be repeated",
                    ),
                    "action": _VerboseAction_,
                    "dest": "verbosity",
                    "default": 0,
                    "nargs": 0,
                }),
                (["-q", "--quiet"], {
                    "help": (
                        "decrease verbosity; may be repeated",
                    ),
                    "action": _VerboseAction_,
                    "dest": "verbosity",
                    "default": 0,
                    "nargs": 0,
                }),
            ),
        ),


        (
            "Options for --import, --add/remove-import, --update",
            _ANY_IMPORT_,
            (
                (["--dry-run"], {
                    "help": (
                        "only print what would have been done",
                    ),
                    "dest": "dry_run",
                    "action": "store_true",
                    "default": False,
                }),
            ),
        ),


        (
            "Options for --import, --add/remove-import",
            (_IMPORT_ | _ADD_IMPORT_ | _REMOVE_IMPORT_),
            (
                (["--with-tests"], {
                    "help": (
                        "include unit tests for the included modules",
                    ),
                    "dest": "tests",
                    "action": "store_true",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--single-configure"], {
                    "help": (
                        "generate a single configure file, not a separate",
                        "configure file for the tests directory",
                    ),
                    "dest": "single_configure",
                    "action": "store_true",
                    "default": False,
                }),
            ),
        ),


        (
            "Options for --create-[mega]testdir, --[mega]test",
            _ANY_TEST_,
            (
                (["--without-tests"], {
                    "help": (
                        "don't include unit tests for the included modules",
                    ),
                    "dest": "tests",
                    "action": "store_false",
                    "default": _argparse_.SUPPRESS,
                }),
            ),
        ),


        (
            "Options for --import, --add/remove-import,"
            "\n"
            "            --create-[mega]testdir, --[mega]test",
            (_ANY_IMPORT_ & ~_UPDATE_) | _ANY_TEST_,
            (
                (["--with-obsolete"], {
                    "help": (
                        "include obsolete modules when they occur among the",
                        "dependencies; by default, dependencies to obsolete",
                        "modules are ignored",
                    ),
                    "dest": "obsolete",
                    "action": "store_true",
                    "default": _argparse_.SUPPRESS,
                }),

                (["--with-c++-tests"], {
                    "help": (
                        "include even unit tests for C++ interoperability",
                    ),
                    "dest": "cxx_tests",
                    "action": "store_true",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--without-c++-tests"], {
                    "help": (
                        "exclude unit tests for C++ interoperability",
                    ),
                    "dest": "cxx_tests",
                    "action": "store_false",
                    "default": _argparse_.SUPPRESS,
                }),

                (["--with-longrunning-tests"], {
                    "help": (
                        "include even unit tests that are long-runners",
                    ),
                    "dest": "longrunning_tests",
                    "action": "store_true",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--without-longrunning-tests"], {
                    "help": (
                        "exclude unit tests that are long-runners",
                    ),
                    "dest": "longrunning_tests",
                    "action": "store_false",
                    "default": _argparse_.SUPPRESS,
                }),

                (["--with-privileged-tests"], {
                    "help": (
                        "include even unit tests that require root privileges",
                    ),
                    "dest": "privileged_tests",
                    "action": "store_true",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--without-privileged-tests"], {
                    "help": (
                        "exclude unit tests that require root privileges",
                    ),
                    "dest": "privileged_tests",
                    "action": "store_false",
                    "default": _argparse_.SUPPRESS,
                }),

                (["--with-unportable-tests"], {
                    "help": (
                        "include even unit tests that fail on some platforms",
                    ),
                    "dest": "unportable_tests",
                    "action": "store_true",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--without-unportable-tests"], {
                    "help": (
                        "exclude unit tests that fail on some platforms",
                    ),
                    "dest": "unportable_tests",
                    "action": "store_false",
                    "default": _argparse_.SUPPRESS,
                }),

                (["--with-all-tests"], {
                    "help": (
                        "include all kinds of problematic unit tests",
                    ),
                    "dest": "all_tests",
                    "action": "store_true",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--avoid"], {
                    "help": (
                        "avoid including the given MODULE; useful if you",
                        "have code that provides equivalent functionality;",
                        "this option can be repeated",
                    ),
                    "action": _AvoidAction_,
                    "nargs": 1,
                    "default": [],
                    "metavar": "MODULE",
                }),
                (["--conditional-dependencies"], {
                    "help": (
                        "support conditional dependencies (may save configure",
                        "time and object code)",
                    ),
                    "dest": "conddeps",
                    "action": "store_true",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--no-conditional-dependencies"], {
                    "help": (
                        "don't use conditional dependencies",
                    ),
                    "dest": "conddeps",
                    "action": "store_false",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--libtool"], {
                    "help": (
                        "use libtool rules",
                    ),
                    "dest": "libtool",
                    "action": "store_true",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--no-libtool"], {
                    "help": (
                        "don't use libtool rules",
                    ),
                    "dest": "libtool",
                    "action": "store_false",
                    "default": _argparse_.SUPPRESS,
                }),
            ),
        ),


        (
            "Options for --import, --add/remove-import",
            (_ANY_IMPORT_ & ~_UPDATE_),
            (
                (["--lib"], {
                    "help": (
                        "specify the library name; defaults to 'libgnu'",
                    ),
                    "metavar": "LIBRARY",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--source-base"], {
                    "help": (
                        "directory relative to --dir where source code is",
                        "placed (default \"lib\")",
                    ),
                    "metavar": "DIRECTORY",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--m4-base"], {
                    "help": (
                        "directory relative to --dir where *.m4 macros are",
                        "placed (default \"m4\")",
                    ),
                    "metavar": "DIRECTORY",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--po-base"], {
                    "help": (
                        "directory relative to --dir where *.po files are",
                        "placed (default \"po\")",
                    ),
                    "metavar": "DIRECTORY",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--doc-base"], {
                    "help": (
                        "directory relative to --dir where doc files are",
                        "placed (default \"doc\")",
                    ),
                    "metavar": "DIRECTORY",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--tests-base"], {
                    "help": (
                        "directory relative to --dir where unit tests are",
                        "placed (default \"tests\")",
                    ),
                    "metavar": "DIRECTORY",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--aux-dir"], {
                    "help": (
                        "directory relative to --dir where auxiliary build",
                        "tools are placed (default comes from configure.ac);",
                    ),
                    "dest": "auxdir",
                    "metavar": "DIRECTORY",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--lgpl"], {
                    "help": (
                        "abort if modules aren't available under the LGPL;",
                        "also modify license template from GPL to LGPL;",
                        "the version number of the LGPL can be specified;",
                        "the default is currently LGPLv3.",
                    ),
                    "choices": (2, 3),
                    "type": int,
                    "metavar": "[=2|=3]",
                }),
                (["--makefile-name"], {
                    "help": (
                        "name of makefile in automake syntax in the",
                        "source-base and tests-base directories",
                        "(default \"Makefile.am\")",
                    ),
                    "metavar": "NAME",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--macro-prefix"], {
                    "help": (
                        "specify the prefix of the macros 'gl_EARLY' and",
                        "'gl_INIT'; default is 'gl'",
                    ),
                    "metavar": "PREFIX",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--po-domain"], {
                    "help": (
                        "specify the prefix of the i18n domain; usually use",
                        "the package name; a suffix '-gnulib' is appended",
                    ),
                    "metavar": "NAME",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--witness-c-macro"], {
                    "help": (
                        "specify the C macro that is defined when the",
                        "sources in this directory are compiled or used",
                    ),
                    "metavar": "NAME",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--vc-files"], {
                    "help": (
                        "update version control related files",
                        "(.gitignore and/or .cvsignore)",
                    ),
                    "dest": "vc_files",
                    "action": "store_true",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--no-vc-files"], {
                    "help": (
                        "don't update version control related files",
                        "(.gitignore and/or .cvsignore)",
                    ),
                    "dest": "libtool",
                    "action": "store_false",
                    "default": _argparse_.SUPPRESS,
                }),
                (["--no-changelog"], {
                    "help": (
                        "don't update or create ChangeLog files;",
                        "this option is currently deprecated",
                    ),
                    "action": "store_const",
                    "const": None,
                    "default": _argparse_.SUPPRESS,
                }),
            ),
        ),


        (
            "Options for --import, --add/remove-import, --update"
            "\n"
            "            --create-[mega]testdir, --[mega]test",
            _ANY_IMPORT_ | _ANY_TEST_,
            (
                (["-s", "--symlink"], {
                    "help": (
                        "make symbolic links instead of copying files",
                    ),
                    "dest": "link",
                    "action": _LinkAction_,
                    "nargs": 0,
                    "default": _LINK_NOTICE_,
                }),
                (["--local-symlink"], {
                    "help": (
                        "make symbolic links instead of copying files, only",
                        "for files from the local override directory"
                    ),
                    "dest": "link",
                    "action": _LinkAction_,
                    "nargs": 0,
                    "default": _LINK_NOTICE_,
                }),
                (["-h", "--hardlink"], {
                    "help": (
                        "make hard links instead of copying files",
                    ),
                    "dest": "link",
                    "action": _LinkAction_,
                    "nargs": 0,
                    "default": _LINK_NOTICE_,
                }),
                (["--local-hardlink"], {
                    "help": (
                        "make hard links instead of copying files, only",
                        "for files from the local override directory"
                    ),
                    "dest": "link",
                    "action": _LinkAction_,
                    "nargs": 0,
                    "default": _LINK_NOTICE_,
                }),
            ),
        ),


        (
            "Options for --import, --add/remove-import, --update",
            _ANY_IMPORT_,
            (
                (["-S", "--more-symlink"], {
                    "help": (
                        "make symbolic links instead of copying files and",
                        "don't replace copyright notices",
                    ),
                    "dest": "link",
                    "action": _LinkAction_,
                    "nargs": 0,
                    "default": _LINK_NOTICE_,
                }),
                (["-H", "--more-hardlink"], {
                    "help": (
                        "make symbolic links instead of copying files and",
                        "don't replace copyright notices",
                    ),
                    "dest": "link",
                    "action": _LinkAction_,
                    "nargs": 0,
                    "default": _LINK_NOTICE_,
                }),
            ),
        ),
    )


    def __error(self, message):
        raise _CommandLineError_(self.__program, message)


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
        iterable = iter(CommandLine._MODES_)
        (_, cmd, args) = next(iterable)
        fmt = (" --{cmd}" + (" {args}" if args else ""))
        lines = ["usage: {program}" + fmt.format(cmd=cmd, args=args)]
        for (_, cmd, args) in iterable:
            fmt = (" --{cmd}" + (" {args}" if args else ""))
            lines += ["       {program}" + fmt.format(cmd=cmd, args=args)]
        lines += ["", ""]
        return "\n".join(lines).format(program=self.__program)


    class Option(_enum_.Flag):
        DryRun = (1 << 0)
        Symlink = (1 << 1)
        Hardlink = (1 << 2)
        OnlyLocalLinks = (1 << 3)
        UpdateCopyrights = (1 << 4)
        SingleConfigure = (1 << 5)


    def __init__(self, program):
        self.__parser = _argparse_.ArgumentParser(prog=program, add_help=False)
        for (_, _, args) in CommandLine._SECTIONS_:
            for arg in args:
                (options, kwargs) = arg
                self.__parser.add_argument(*options, **kwargs)
        self.__program = _os_.path.basename(program)
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


    def parse(self, *arguments):
        """
        self.parse(*arguments) -> (config, explicit, mode, verbosity, options)

        Parse the command-line arguments and return a new configuration.
        The default configuration holds default parameter values.
        The parameter is overriden if and only if it was set explicitly.

        config: a new configuration instance (pygnulib.Config.Base)
        explicit: an iterable representing explicit configuration keys
        mode: a string representing the mode ("import", "add-import", etc.)
        verbosity: an integer representing the desired verbosity level
        options: a combination of CommandLine.Option flags
        """
        namespace = vars(self.__parser.parse_args(arguments))
        options = ~(CommandLine.Option.DryRun |
                    CommandLine.Option.Symlink |
                    CommandLine.Option.Hardlink |
                    CommandLine.Option.OnlyLocalLinks |
                    CommandLine.Option.UpdateCopyrights |
                    CommandLine.Option.SingleConfigure)
        mode = namespace.pop("mode", None)
        if mode is None:
            self.__parser.error("no operating mode selected")
        mode = dict((_[0], _[1]) for _ in CommandLine._MODES_)[mode]
        verbosity = namespace.pop("verbosity", 0)
        if namespace.pop("dry_run", False):
            options |= CommandLine.Option.DryRun
        link = namespace.pop("link", None)
        if link is not None:
            if link & CommandLine._LINK_SYMBOLIC_:
                options |= CommandLine.Option.Symlink
            if link & CommandLine._LINK_HARD_:
                options |= CommandLine.Option.Hardlink
            if link & CommandLine._LINK_LOCAL_:
                options |= CommandLine.Option.OnlyLocalLinks
            if link & CommandLine._LINK_NOTICE_:
                options |= CommandLine.Option.UpdateCopyrights
        if namespace.pop("single_configure", False):
            options |= CommandLine.Option.SingleConfigure
        namespace.pop("no_changelog", None)

        config = _BaseConfig_(**namespace)
        explicit = namespace.keys()
        return (config, explicit, mode, verbosity, options)

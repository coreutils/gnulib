#!/usr/bin/python
# encoding: UTF-8


import argparse
import codecs
import os
import re
import sys

from .error import AutoconfVersionError


class Config:
    """gnulib generic configuration"""
    _TABLE_ = {
        "root"              : "",
        "local_dir"         : "",
        "source_base"       : "lib",
        "m4_base"           : "m4",
        "po_base"           : "po",
        "doc_base"          : "doc",
        "tests_base"        : "tests",
        "auxdir"            : "",
        "lib"               : "libgnu",
        "makefile_name"     : "Makefile.am",
        "macro_prefix"      : "gl",
        "po_domain"         : "",
        "witness_c_macro"   : "",
        "lgpl"              : 0,
        "tests"             : False,
        "obsolete"          : False,
        "cxx_tests"         : False,
        "longrunning_tests" : False,
        "privileged_tests"  : False,
        "unportable_tests"  : False,
        "all_tests"         : False,
        "libtool"           : False,
        "conddeps"          : False,
        "vc_files"          : False,
        "autoconf"          : 2.59,
        "modules"           : [],
        "avoid"             : [],
        "files"             : [],
    }


    def __init__(self, **kwargs):
        self._table_ = dict()
        for key in Config._TABLE_:
            self._table_[key] = Config._TABLE_[key]
        for key, value in kwargs.items():
            self[key] = value


    def __repr__(self):
        return repr(self._table_)


    def __iter__(self):
        for key in Config._TABLE_:
            value = self[key]
            yield key, value


    @property
    def source_base(self):
        """directory relative to ROOT where source code is placed; defaults to 'lib'"""
        return self["source_base"]

    @source_base.setter
    def source_base(self, value):
        self["source_base"] = value


    @property
    def m4_base(self):
        """directory relative to ROOT where *.m4 macros are placed; defaults to 'm4'"""
        return self["m4_base"]

    @m4_base.setter
    def m4_base(self, value):
        self["m4_base"] = value


    @property
    def po_base(self):
        """directory relative to ROOT where *.po files are placed; defaults to 'po'"""
        return self["po_base"]

    @po_base.setter
    def po_base(self, value):
        self["po_base"] = value


    @property
    def doc_base(self):
        """directory relative to ROOT where doc files are placed; defaults to 'doc'"""
        return self["doc_base"]

    @doc_base.setter
    def doc_base(self, value):
        self["doc_base"] = value


    @property
    def tests_base(self):
        """directory relative to ROOT where unit tests are placed; defaults to 'tests'"""
        return self["tests_base"]

    @tests_base.setter
    def tests_base(self, value):
        self["tests_base"] = value


    @property
    def auxdir(self):
        """directory relative to ROOT where auxiliary build tools are placed"""
        return self["auxdir"]

    @auxdir.setter
    def auxdir(self, value):
        self["auxdir"] = value


    @property
    def lib(self):
        """library name; defaults to 'libgnu'"""
        return self["lib"]

    @lib.setter
    def lib(self, value):
        self["lib"] = value


    @property
    def makefile_name(self):
        """name of makefile in automake syntax in the source-base and tests-base directories"""
        return self["makefile_name"]

    @makefile_name.setter
    def makefile_name(self, value):
        self["makefile_name"] = value


    @property
    def macro_prefix(self):
        """
        the prefix of the macros 'gl_EARLY' and 'gl_INIT' (default is 'gl');
        the change of this parameter also affects include_guard_prefix parameter
        """
        return self["macro_prefix"]

    @macro_prefix.setter
    def macro_prefix(self, value):
        self["macro_prefix"] = value


    @property
    def po_domain(self):
        """the prefix of the i18n domain"""
        return self["po_domain"]

    @po_domain.setter
    def po_domain(self, value):
        self["po_domain"] = value


    @property
    def witness_c_macro(self):
        """the C macro that is defined when the sources are compiled or used"""
        return self["witness_c_macro"]

    @witness_c_macro.setter
    def witness_c_macro(self, value):
        self["witness_c_macro"] = value


    @property
    def lgpl(self):
        """abort if modules aren't available under the LGPL; also modify license template"""
        return self["lgpl"]

    @lgpl.setter
    def lgpl(self, value):
        if value is None:
            value = 0
        if value not in [0, 2, 3]:
            raise TypeError("lgpl option must be either None or integral version (2 or 3)")
        self["lgpl"] = value


    @property
    def tests(self):
        """include unit tests for the included modules"""
        return self["tests"]

    @tests.setter
    def tests(self, value):
        self["tests"] = value


    @property
    def obsolete(self):
        """include obsolete modules when they occur among the modules"""
        return self["obsolete"]

    @obsolete.setter
    def obsolete(self, value):
        self["obsolete"] = value


    @property
    def cxx_tests(self):
        """include even unit tests for C++ interoperability"""
        return self["cxx_tests"]

    @cxx_tests.setter
    def cxx_tests(self, value):
        self["cxx_tests"] = value


    @property
    def longrunning_tests(self):
        """include even unit tests that are long-runners"""
        return self["longrunning_tests"]

    @longrunning_tests.setter
    def longrunning_tests(self, value):
        self["longrunning_tests"] = value


    @property
    def privileged_tests(self):
        """include even unit tests that require root privileges"""
        return self["privileged_tests"]

    @privileged_tests.setter
    def privileged_tests(self, value):
        self["privileged_tests"] = value


    @property
    def unportable_tests(self):
        """include even unit tests that fail on some platforms"""
        return self["unportable_tests"]

    @unportable_tests.setter
    def unportable_tests(self, value):
        self["unportable_tests"] = value


    @property
    def all_tests(self):
        """include all kinds of problematic unit tests"""
        result = True
        result &= self.tests
        result &= self.cxx_tests
        result &= self.privileged_tests
        result &= self.unportable_tests
        result &= self.longrunning_tests
        return result

    @all_tests.setter
    def all_tests(self, value):
        self.tests = value
        self.cxx_tests = value
        self.privileged_tests = value
        self.unportable_tests = value
        self.longrunning_tests = value


    @property
    def libtool(self):
        """use libtool rules"""
        return self["libtool"]

    @libtool.setter
    def libtool(self, value):
        self["libtool"] = value


    @property
    def conddeps(self):
        """support conditional dependencies (may save configure time and object code)"""
        return self["conddeps"]

    @conddeps.setter
    def conddeps(self, value):
        self["conddeps"] = value


    @property
    def vc_files(self):
        """update version control related files"""
        return self["vc_files"]

    @vc_files.setter
    def vc_files(self, value):
        self["vc_files"] = value


    @property
    def autoconf(self):
        """autoconf version"""
        return self["autoconf"]

    @autoconf.setter
    def autoconf(self, value):
        self["autoconf"] = value


    @property
    def modules(self):
        """list of modules"""
        return self["modules"]

    @modules.setter
    def modules(self, value):
        self["modules"] = tuple(value)


    @property
    def avoid(self):
        """list of modules to avoid"""
        return self["avoid"]

    @avoid.setter
    def avoid(self, value):
        self["avoid"] = tuple(value)


    @property
    def files(self):
        """list of files to be processed"""
        return self["files"]

    @files.setter
    def files(self, value):
        self["files"] = list(value)


    @property
    def include_guard_prefix(self):
        """include guard prefix"""
        prefix = self["macro_prefix"].upper()
        default = Config._TABLE_["macro_prefix"]
        return "GL_%s" % prefix if prefix == default else "GL"


    def __getitem__(self, key):
        if key not in Config._TABLE_:
            key = key.replace("-", "_")
            if key not in Config._TABLE_:
                raise KeyError("unsupported option: %r" % key)
        return self._table_[key]


    def __setitem__(self, key, value):
        if key not in Config._TABLE_:
            key = key.replace("_", "-")
            if key not in Config._TABLE_:
                raise KeyError("unsupported option: %r" % key)
        key = key.replace("-", "_")
        typeid = type(Config._TABLE_[key])
        if key == "lgpl":
            if value not in [None, 2, 3]:
                raise TypeError("lgpl option must be either None or integral version (2 or 3)")
        elif key == "autoconf":
            if value < 2.59:
                raise AutoconfVersionError(2.59)
        elif not isinstance(value, typeid):
            raise TypeError("%r option must be of %r type" % (key, typeid))
        self._table_[key] = value


    def items(self):
        """a set-like object providing a view on configuration items"""
        return self._table_.items()


    def keys(self):
        """a set-like object providing a view on configuration keys"""
        return self._table_.keys()


    def values(self):
        """a set-like object providing a view on configuration values"""
        return self._table_.values()


class Cache(Config):
    """gnulib cached configuration"""
    _AUTOCONF_ = {
        "autoconf" : re.compile(".*AC_PREREQ\\(\\[(.*?)\\]\\)", re.S | re.M),
        "auxdir"   : re.compile("^AC_CONFIG_AUX_DIR\\(\\[(.*?)\\]\\)$", re.S | re.M),
        "libtool"  : re.compile("A[CM]_PROG_LIBTOOL", re.S | re.M)
    }
    _GNULIB_CACHE_ = {
        "libtool"           : (bool, "gl_LIBTOOL"),
        "conddeps"          : (bool, "gl_CONDITIONAL_DEPENDENCIES"),
        "vc_files"          : (bool, "gl_VC_FILES"),
        "tests"             : (bool, "gl_WITH_TESTS"),
        "obsolete"          : (bool, "gl_WITH_OBSOLETE"),
        "cxx_tests"         : (bool, "gl_WITH_CXX_TESTS"),
        "longrunning_tests" : (bool, "gl_WITH_LONGRUNNING_TESTS"),
        "privileged_tests"  : (bool, "gl_WITH_PRIVILEGED_TESTS"),
        "unportable_tests"  : (bool, "gl_WITH_UNPORTABLE_TESTS"),
        "all_tests"         : (bool, "gl_WITH_ALL_TESTS"),
        "local_dir"         : (str, "gl_LOCAL_DIR"),
        "source_base"       : (str, "gl_SOURCE_BASE"),
        "m4_base"           : (str, "gl_M4_BASE"),
        "po_base"           : (str, "gl_PO_BASE"),
        "doc_base"          : (str, "gl_DOC_BASE"),
        "tests_base"        : (str, "gl_TESTS_BASE"),
        "makefile_name"     : (str, "gl_MAKEFILE_NAME"),
        "macro_prefix"      : (str, "gl_MACRO_PREFIX"),
        "po_domain"         : (str, "gl_PO_DOMAIN"),
        "witness_c_macro"   : (str, "gl_WITNESS_C_MACRO"),
        "lib"               : (str, "gl_LIB"),
        "modules"           : (list, "gl_MODULES"),
        "avoid"             : (list, "gl_AVOID"),
        "lgpl"              : (str, "gl_LGPL"),
    }
    _GNULIB_CACHE_BOOL_ = []
    _GNULIB_CACHE_STR_ = []
    _GNULIB_CACHE_LIST_ = []
    for _key_, (_typeid_, _) in _GNULIB_CACHE_.items():
        if _typeid_ is bool:
            _GNULIB_CACHE_BOOL_ += [_key_]
        elif _typeid_ is str:
            _GNULIB_CACHE_STR_ += [_key_]
        else:
            _GNULIB_CACHE_LIST_ += [_key_]
    _GNULIB_CACHE_PATTERN_ = re.compile("^(gl_.*?)\\(\\[(.*?)\\]\\)$", re.S | re.M)


    def __init__(self, root, autoconf=None, **kwargs):
        if not isinstance(root, str):
            raise TypeError("root must be of 'str' type")
        super().__init__(**kwargs)
        self._autoconf_(root, autoconf)
        self._gnulib_cache_(root)
        self._gnulib_comp_(root)

    def _autoconf_(self, root, autoconf):
        if not autoconf:
            autoconf = os.path.join(root, "configure.ac")
            if not os.path.exists(autoconf):
                autoconf = os.path.join(root, "configure.in")
        if not os.path.isabs(autoconf):
            autoconf = os.path.join(root, autoconf)
        autoconf = os.path.normpath(autoconf)
        with codecs.open(autoconf, "rb", "UTF-8") as stream:
            data = stream.read()
        for key, pattern in Cache._AUTOCONF_.items():
            match = pattern.findall(data)
            if not match:
                continue
            if key == "autoconf":
                self["autoconf"] = sorted(set([float(_.strip()) for _ in match if _.strip()]))[-1]
            else:
                self[key] = match[-1]

    def _gnulib_cache_(self, root):
        m4base = self["m4-base"]
        gnulib_cache = os.path.join(root, m4base, "gnulib-cache.m4")
        if os.path.exists(gnulib_cache):
            with codecs.open(gnulib_cache, "rb", "UTF-8") as stream:
                data = stream.read()
            for key in Cache._GNULIB_CACHE_BOOL_:
                (_, macro) = Cache._GNULIB_CACHE_[key]
                if key in data:
                    self[key] = True
            match = dict(Cache._GNULIB_CACHE_PATTERN_.findall(data))
            for key in Cache._GNULIB_CACHE_STR_:
                (_, macro) = Cache._GNULIB_CACHE_[key]
                if macro in match:
                    self[key] = match[macro].strip()
            for key in Cache._GNULIB_CACHE_LIST_:
                (_, macro) = Cache._GNULIB_CACHE_[key]
                if macro in match:
                    self[key] = [_.strip() for _ in match[macro].split("\n") if _.strip()]

    def _gnulib_comp_(self, root):
        m4base = self["m4-base"]
        gnulib_comp = os.path.join(root, m4base, "gnulib-comp.m4")
        if os.path.exists(gnulib_comp):
            with codecs.open(gnulib_comp, "rb", "UTF-8") as stream:
                data = stream.read()
            regex = "AC_DEFUN\\(\\[%s_FILE_LIST\\], \\[(.*?)\\]\\)" % self["macro-prefix"]
            pattern = re.compile(regex, re.S | re.M)
            match = pattern.findall(data)
            if match:
                self.files = [_.strip() for _ in match[-1].split("\n") if _.strip()]



class CommandLine(Config):
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
    )
    _LINK_SYMBOLIC_ = (1 << 0)
    _LINK_HARD_ = (1 << 1)
    _LINK_LOCAL_ = (1 << 2)
    _LINK_NOTICE_ = (1 << 3)


    class _ModeAction_(argparse.Action):
        def __init__(self, *args, **kwargs):
            mode = kwargs["const"]
            kwargs["dest"] = "mode"
            kwargs["nargs"] = 0
            if mode & CommandLine._ANY_IMPORT_ or mode & CommandLine._ANY_TEST_:
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


    class _AvoidAction_(argparse.Action):
        def __call__(self, parser, namespace, value, option=None):
            values = getattr(namespace, self.dest)
            values += value


    class _VerboseAction_(argparse.Action):
        def __call__(self, parser, namespace, value, option=None):
            value = getattr(namespace, self.dest)
            verbose = option in ("-v", "--verbose")
            value += +1 if verbose else -1
            setattr(namespace, self.dest, value)


    class _LinkAction_(argparse.Action):
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
                    "default": ".",
                    "metavar": "DIRECTORY",
                }),
                (["--local-dir"], {
                    "help": (
                        "pecify a local override directory where to look",
                        "up files before looking in gnulib's directory",
                    ),
                    "dest": "local_dir",
                    "default": "",
                    "nargs": 1,
                    "metavar": "DIRECTORY",
                }),
                (["-v", "--verbose"], {
                    "help": (
                        "increase verbosity; may be repeated",
                    ),
                    "dest": "verbosity",
                    "action": _VerboseAction_,
                    "default": 0,
                    "nargs": 0,
                }),
                (["-q", "--quiet"], {
                    "help": (
                        "decrease verbosity; may be repeated",
                    ),
                    "dest": "verbosity",
                    "action": _VerboseAction_,
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
                    "default": False,
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
                    "default": False,
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
                    "default": False,
                }),

                (["--with-c++-tests"], {
                    "help": (
                        "include even unit tests for C++ interoperability",
                    ),
                    "dest": "cxx_tests",
                    "action": "store_true",
                    "default": False,
                }),
                (["--without-c++-tests"], {
                    "help": (
                        "exclude unit tests for C++ interoperability",
                    ),
                    "dest": "cxx_tests",
                    "action": "store_false",
                    "default": False,
                }),

                (["--with-longrunning-tests"], {
                    "help": (
                        "include even unit tests that are long-runners",
                    ),
                    "dest": "longrunning_tests",
                    "action": "store_true",
                    "default": False,
                }),
                (["--without-longrunning-tests"], {
                    "help": (
                        "exclude unit tests that are long-runners",
                    ),
                    "dest": "longrunning_tests",
                    "action": "store_false",
                    "default": False,
                }),

                (["--with-privileged-tests"], {
                    "help": (
                        "include even unit tests that require root privileges",
                    ),
                    "dest": "privileged_tests",
                    "action": "store_true",
                    "default": False,
                }),
                (["--without-privileged-tests"], {
                    "help": (
                        "exclude unit tests that require root privileges",
                    ),
                    "dest": "privileged_tests",
                    "action": "store_false",
                    "default": False,
                }),

                (["--with-unportable-tests"], {
                    "help": (
                        "include even unit tests that fail on some platforms",
                    ),
                    "dest": "unportable_tests",
                    "action": "store_true",
                    "default": False,
                }),
                (["--without-unportable-tests"], {
                    "help": (
                        "exclude unit tests that fail on some platforms",
                    ),
                    "dest": "unportable_tests",
                    "action": "store_false",
                    "default": False,
                }),

                (["--with-all-tests"], {
                    "help": (
                        "include all kinds of problematic unit tests",
                    ),
                    "dest": "all_tests",
                    "action": "store_true",
                    "default": False,
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
                    "action": "store_true",
                    "default": False,
                    "dest": "conddeps",
                }),
                (["--no-conditional-dependencies"], {
                    "help": (
                        "don't use conditional dependencies",
                    ),
                    "action": "store_false",
                    "default": False,
                    "dest": "conddeps",
                }),
                (["--libtool"], {
                    "help": (
                        "use libtool rules",
                    ),
                    "action": "store_true",
                    "default": False,
                    "dest": "libtool",
                }),
                (["--no-libtool"], {
                    "help": (
                        "don't use libtool rules",
                    ),
                    "action": "store_false",
                    "default": False,
                    "dest": "libtool",
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
                    "default": "libgnu",
                    "metavar": "LIBRARY",
                }),
                (["--source-base"], {
                    "help": (
                        "directory relative to --dir where source code is",
                        "placed (default \"lib\")",
                    ),
                    "default": "lib",
                    "metavar": "DIRECTORY",
                }),
                (["--m4-base"], {
                    "help": (
                        "directory relative to --dir where *.m4 macros are",
                        "placed (default \"m4\")",
                    ),
                    "default": "m4",
                    "metavar": "DIRECTORY",
                }),
                (["--po-base"], {
                    "help": (
                        "directory relative to --dir where *.po files are",
                        "placed (default \"po\")",
                    ),
                    "default": "po",
                    "metavar": "DIRECTORY",
                }),
                (["--doc-base"], {
                    "help": (
                        "directory relative to --dir where doc files are",
                        "placed (default \"doc\")",
                    ),
                    "default": "doc",
                    "metavar": "DIRECTORY",
                }),
                (["--tests-base"], {
                    "help": (
                        "directory relative to --dir where unit tests are",
                        "placed (default \"tests\")",
                    ),
                    "default": "tests",
                    "metavar": "DIRECTORY",
                }),
                (["--aux-dir"], {
                    "help": (
                        "directory relative to --dir where auxiliary build",
                        "tools are placed (default comes from configure.ac);",
                    ),
                    "dest": "auxdir",
                    "default": "",
                    "metavar": "DIRECTORY",
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
                    "default": "Makefile.am",
                    "metavar": "NAME",
                }),
                (["--macro-prefix"], {
                    "help": (
                        "specify the prefix of the macros 'gl_EARLY' and",
                        "'gl_INIT'; default is 'gl'",
                    ),
                    "default": "gl",
                    "metavar": "PREFIX",
                }),
                (["--po-domain"], {
                    "help": (
                        "specify the prefix of the i18n domain; usually use",
                        "the package name; a suffix '-gnulib' is appended",
                    ),
                    "default": "",
                    "metavar": "NAME",
                }),
                (["--witness-c-macro"], {
                    "help": (
                        "specify the C macro that is defined when the",
                        "sources in this directory are compiled or used",
                    ),
                    "default": "",
                    "metavar": "NAME",
                }),
                (["--vc-files"], {
                    "help": (
                        "update version control related files",
                        "(.gitignore and/or .cvsignore)",
                    ),
                    "action": "store_true",
                    "default": False,
                    "dest": "vc_files",
                }),
                (["--no-vc-files"], {
                    "help": (
                        "don't update version control related files",
                        "(.gitignore and/or .cvsignore)",
                    ),
                    "action": "store_false",
                    "default": False,
                    "dest": "libtool",
                }),
                (["--no-changelog"], {
                    "help": (
                        "don't update or create ChangeLog files;",
                        "this option is currently deprecated",
                    ),
                    "default": None,
                    "action": "store_const",
                    "const": None,
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


    def _usage_(self):
        iterable = iter(CommandLine._MODES_)
        (_, cmd, args) = next(iterable)
        fmt = (" --{cmd}" + (" {args}" if args else ""))
        lines = ["usage: {program}" + fmt.format(cmd=cmd, args=args)]
        for (_, cmd, args) in iterable:
            fmt = (" --{cmd}" + (" {args}" if args else ""))
            lines += ["       {program}" + fmt.format(cmd=cmd, args=args)]
        lines += ["", ""]
        return "\n".join(lines).format(program=self._program_)


    def _help_(self):
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
        return self._usage_()[:-1] + "\n".join(lines)


    def __init__(self, program, argv, **kwargs):
        super().__init__(**kwargs)
        parser = argparse.ArgumentParser(prog=program, add_help=False, allow_abbrev=False)
        for (_, _, args) in CommandLine._SECTIONS_:
            for arg in args:
                (options, kwargs) = arg
                parser.add_argument(*options, **kwargs)
        self._program_ = os.path.basename(program)
        parser.format_usage = self._usage_
        parser.format_help = self._help_
        if "--help" in argv:
            parser.print_help()
            sys.exit(0)

        namespace = parser.parse_args(argv)
        namespace = vars(namespace)
        self._mode_ = namespace.pop("mode")
        if self._mode_ is None:
            parser.error("no operating mode selected")
        self._dry_run_ = namespace.pop("dry_run")
        self._link_ = namespace.pop("link", None)
        self._single_configure_ = namespace.pop("single_configure")
        self._verbosity_ = namespace.pop("verbosity")
        namespace.pop("no_changelog", None)
        for (key, value) in namespace.items():
            self[key] = value


    @property
    def mode(self):
        """operating mode"""
        for (flag, cmd, _) in CommandLine._MODES_:
            if flag == self._mode_:
                return cmd
        return ""


    @property
    def dry_run(self):
        """running in dry-run mode?"""
        return self._dry_run_


    @property
    def verbosity(self):
        """verbosity level"""
        return self._verbosity_


    @property
    def single_configure(self):
        """generate a single configure file?"""
        return self._single_configure_


    @property
    def symlink(self):
        """make symbolic links instead of copying files?"""
        return bool(self._link_ & CommandLine._LINK_SYMBOLIC_)


    def hardlink(self):
        """make hard links instead of copying files?"""
        return bool(self._link_ & CommandLine._LINK_HARD_)


    @property
    def only_local_links(self):
        """make links only for files from the local override directory?"""
        return bool(self._link_ & CommandLine._LINK_LOCAL_)


    @property
    def allow_license_update(self):
        """allow to update license notice?"""
        return (self._link_ & CommandLine._LINK_NOTICE_)

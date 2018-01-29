#!/usr/bin/python
# encoding: UTF-8



import codecs
import os
import re
import stat
import sys
import tempfile
import traceback
import subprocess as sp


from pygnulib.error import CommandLineError
from pygnulib.error import UnknownModuleError

from pygnulib.config import BaseConfig
from pygnulib.config import CachedConfig

from pygnulib.generator import gnulib_cache
from pygnulib.generator import gnulib_comp
from pygnulib.generator import lib_makefile
from pygnulib.generator import po_make_vars
from pygnulib.generator import tests_makefile

from pygnulib.module import DummyModule
from pygnulib.module import Database

from pygnulib.parser import CommandLine as CommandLineParser

from pygnulib.tools import Executable

from pygnulib.vfs import BaseVFS
from pygnulib.vfs import GnulibGitVFS
from pygnulib.vfs import backup as vfs_backup
from pygnulib.vfs import compare as vfs_compare
from pygnulib.vfs import copy as vfs_copy
from pygnulib.vfs import exists as vfs_exists
from pygnulib.vfs import hardlink as vfs_hardlink
from pygnulib.vfs import lookup as vfs_lookup
from pygnulib.vfs import move as vfs_move
from pygnulib.vfs import iostream as vfs_iostream
from pygnulib.vfs import symlink as vfs_symlink
from pygnulib.vfs import unlink as vfs_unlink



class GnulibExecutable(Executable):
    def __init__(self, name, encoding=None, shell_name=None, shell_path=None):
        path = None
        if shell_name is None:
            shell_name = shell_name.upper()
        if shell_path is None:
            shell_path = "{}PATH".format(shell_name)
        environ = dict(ENVIRON)
        environ.update(os.environ)
        if shell_name in environ:
            path = shell_name
        elif shell_path in environ:
            path = "{}{}".format(shell_path)
        super().__init__(name, path)



AC_VERSION_PATTERN = re.compile(r"AC_PREREQ\(\[(.*?)\]\)", re.S | re.M)
IGNORED_LICENSES = {
    "GPLed build tool",
    "public domain",
    "unlimited",
    "unmodifiable license text",
}
TRANSFER_MODES = {
    None: vfs_copy,
    "hardlink": vfs_hardlink,
    "symlink": vfs_symlink,
}
SUBSTITUTION = {
    "build-aux": "auxdir",
    "doc": "doc_base",
    "lib": "source_base",
    "m4": "m4_base",
    "tests": "tests_base",
    "tests=lib": "tests_base",
    "po": "po_base",
    "top": "",
}
TP_URL = "http://translationproject.org/latest/"
TP_RSYNC_URI = "translationproject.org::tp/latest/"



def list_hook(gnulib, *args, **kwargs):
    (_, _) = (args, kwargs)
    for module in sorted(gnulib.modules()):
        print(module.name, file=sys.stdout)
    return os.EX_OK



def extract_hook(program, gnulib, mode, namespace, *args, **kwargs):
    (_, _) = (args, kwargs)
    mode = mode.replace("extract-", "")
    mode += "s" if mode == "maintainer" else ""
    for module in namespace["modules"]:
        try:
            module = gnulib.module(module)
            print(module[mode], file=sys.stdout)
        except UnknownModuleError as error:
            print("{0}:".format(program), "warning:", error, file=sys.stderr)
    return os.EX_OK



def import_hook(script, gnulib, namespace, explicit, verbosity, options, *args, **kwargs):
    (_, _) = (args, kwargs)
    config = BaseConfig(**namespace)
    try:
        cache = CachedConfig(root=config.root)
    except FileNotFoundError:
        cache = BaseConfig(**config)
        cache.files = set()
        cache.ac_version = "2.59"
    for key in {"ac_version", "files"}:
        if key not in namespace:
            config[key] = cache[key]
    database = Database(gnulib.module, config)

    # Print some information about modules.
    print("Module list with included dependencies (indented):", file=sys.stdout)
    BOLD_ON = BOLD_OFF = ""
    mode = os.fstat(sys.stdout.fileno()).st_mode
    if not (stat.S_ISFIFO(mode) or stat.S_ISREG(mode)):
        BOLD_ON = "\033[1m"
        BOLD_OFF = "\033[0m"
    for module in database.final_modules:
        manual = module in database.explicit_modules
        prefix = "  " if manual else "    "
        bold_on = BOLD_ON if manual else ""
        bold_off = BOLD_OFF if manual else ""
        print("{0}{1}{2}{3}".format(prefix, bold_on, module.name, bold_off), file=sys.stdout)
    if verbosity >= 1:
        print("Main module list:", file=sys.stdout)
        for module in database.main_modules:
            if module is not DummyModule():
                print("  {0}".format(module.name), file=sys.stdout)
        if database.main_modules:
            print("", file=sys.stdout)
        print("Tests-related module list:", file=sys.stdout)
        for module in database.test_modules:
            print("  {0}".format(module.name), file=sys.stdout)
        if database.test_modules:
            print("", file=sys.stdout)

    # Determine license incompatibilities, if any.
    incompatibilities = set()
    if config.licenses & {"LGPLv2", "LGPLv2+", "LGPLv3", "LGPLv3+"}:
        acceptable = IGNORED_LICENSES | config.licenses
        for (name, licenses) in ((module.name, module.licenses) for module in main):
            if not (acceptable & licenses):
                incompatibilities.add((name, licenses))
    if incompatibilities:
        print("{0}: *** incompatible license on modules:".format(script), file=sys.stderr)
        for (name, licenses) in sorted(incompatibilities):
            print(" " * 16, "{0:50}{1}".format(name, " ".join(sorted(licenses))), file=sys.stderr)
        print("{0}: *** Stop.".format(script), file=sys.stderr)
        return os.EX_DATAERR

    # Show banner notice of every module.
    if verbosity >= -1:
        for module in database.main_modules:
            notice = module.notice
            if notice.strip():
                print("Notice from module {0}:".format(module.name), file=sys.stdout)
                print("\n".join("  " + line for line in notice.splitlines()), file=sys.stdout)

    # Determine the final file list.
    files = (set(database.main_files) | set(database.test_files))
    if verbosity >= 0:
        print("File list:", file=sys.stdout)
        for file in sorted(files):
            if file.startswith("tests=lib/"):
                name = file[len("tests=lib/"):]
                src = "lib/" + name
                dst = "tests/" + name
                print("  ", src, " -> ", dst, file=sys.stdout, sep="")
            else:
                print("  ", file, file=sys.stdout, sep="")

    table = {}
    overrides = []
    project = BaseVFS(config.root, **table)
    overrides = {BaseVFS(override, **table) for override in config.overrides}
    for (name, key) in SUBSTITUTION.items():
        project[name] = config[key] if key else ""
    for override in overrides:
        for (name, key) in SUBSTITUTION.items():
            override[name] = config[key] if key else ""

    table = {}
    old_files = set(cache.files)
    if "m4/gnulib-tool.m4" in project:
        old_files |= set(["m4/gnulib-tool.m4"])
    for (tbl_key, cfg_key) in SUBSTITUTION.items():
        table[tbl_key] = cache[cfg_key] if cfg_key else ""
    new_files = frozenset(files | set(["m4/gnulib-tool.m4"]))

    dry_run = options["dry_run"]
    gnulib_copymode = config.copymode
    local_copymode = config.local_copymode

    def transfer_file(local, src_vfs, src_name, dst_vfs, dst_name):
        args = (src_vfs, src_name, dst_vfs, dst_name)
        if src_vfs is None:
            return vfs_copy(*args)
        action = TRANSFER_MODES[local_copymode if local else gnulib_copymode]
        try:
            return action(*args)
        except OSError as error:
            if error.errno == errno.EXDEV:
                return vfs_copy(*args)
            raise error

    def remove_file(project, file):
        action = ("Removing", "Remove")[dry_run]
        fmt = (action + " file {file} (backup in {file}~)")
        if not dry_run:
            try:
                vfs_unlink(project, file, backup=True)
            except FileNotFoundError:
                pass
        print(fmt.format(file=file), file=sys.stdout)

    def update_file(local, src_vfs, src_name, dst_vfs, dst_name, present):
        if not vfs_compare(src_vfs, src_name, dst_vfs, dst_name):
            action = (("Replacing", "Replace"), ("Updating", "Update"))[present][dry_run]
            message = ("(non-gnulib code backed up in {file}~) !!", "(backup in {file}~)")[present]
            fmt = (action + " file {file} " + message)
            if not dry_run:
                vfs_backup(dst_vfs, dst_name)
                transfer_file(local, src_vfs, src_name, dst_vfs, dst_name)
            print(fmt.format(file=dst_name), file=sys.stdout)

    def add_file(local, src_vfs, src_name, dst_vfs, dst_name, present):
        action = ("Copying", "Copy")[dry_run]
        fmt = (action + " file {file}")
        if not dry_run:
            transfer_file(local, src_vfs, src_name, dst_vfs, dst_name)
        print(fmt.format(file=dst_name), file=sys.stdout)

    # First the files that are in old_files, but not in new_files.
    # Then the files that are in new_files, but not in old_files.
    # Then the files that are in new_files and in old_files.
    removed_files = {file for file in old_files if file not in new_files}
    added_files = {file for file in new_files if file not in old_files}
    kept_files = (old_files & new_files)
    for file in sorted(removed_files):
        remove_file(project, file)
    for (present, files) in ((False, added_files), (True, kept_files)):
        for dst in sorted(files):
            match = tuple(override for override in overrides if dst in override)
            override = match[0] if match else gnulib
            (vfs, src) = vfs_lookup(dst, gnulib, override, patch="patch")
            action = update_file if vfs_exists(project, dst) else add_file
            action(bool(match), vfs, src, project, dst, present)

    mkedits = []
    if config.makefile_name == "Makefile.am":
        dirname = os.path.dirname(config.source_base)
        basename = os.path.basename(config.source_base)
        dirname = "" if dirname == "." else (dirname + os.path.sep)
        mkedits.append((dirname, "SUBDIRS", basename))
    if "po_base" in explicit:
        dirname = os.path.dirname(config.po_base)
        basename = os.path.basename(config.po_base)
        dirname = "" if dirname == "." else dirname
        mkedits.append((dirname, "SUBDIRS", basename))
    if config.tests and config.makefile_name == "Makefile.am":
        dirname = os.path.dirname(config.tests_base)
        basename = os.path.basename(config.tests_base)
        dirname = "" if dirname == "." else (dirname + os.path.sep)
        mkedits.append((dirname, "SUBDIRS", basename))
    mkedits.append(("", "ACLOCAL_AMFLAGS", "-I {}".format(config.m4_base)))

    # Find the first parent directory of m4_base that contains or will contain a Makefile.am.
    (dir1, dir2) = (config.m4_base, "")
    source_base_makefile = os.path.join(config.source_base, config.makefile_name)
    tests_base_makefile = os.path.join(config.tests_base, config.makefile_name)
    while dir1 != ".":
        path = os.path.join(dir1, "Makefile.am")
        if vfs_exists(project, os.path.join(config.root, dir1, "Makefile.am")) \
        or (config.tests and path in (source_base_makefile, tests_base_makefile)):
            break
        dir2 = os.path.join(os.path.basename(dir1), dir2)
        dir1 = os.path.dirname(dir1)
    mkedits.append((dir1, "EXTRA_DIST", os.path.join(dir2, "gnulib-cache.m4")))

    # Generate the contents of library makefile.
    path = os.path.join(config.source_base, config.makefile_name)
    with tempfile.NamedTemporaryFile("w", encoding="UTF-8", delete=False) as tmp:
        modules = database.main_modules
        for line in lib_makefile(path, config, explicit, database, modules, mkedits, False):
            print(line, file=tmp)
    (src, dst) = (tmp.name, path)
    present = vfs_exists(project, dst)
    if present:
        added_files.add(dst)
    action = update_file if present else add_file
    action(False, None, src, project, dst, present)
    os.unlink(tmp.name)

    # Generate po makefile and directories.
    url = (TP_RSYNC_URI + "gnulib/")
    if "po_base" in explicit:
        for file in ("Makefile.in.in", "remove-potcdate.sin"):
            path = os.path.join("build-aux", "po", file)
            match = tuple(override for override in overrides if file in override)
            override = match[0] if match else gnulib
            (vfs, src) = vfs_lookup(path, gnulib, override, patch="patch")
            dst = os.path.join("po", file)
            present = vfs_exists(project, dst)
            if present:
                added_files.add(dst)
            action = update_file if present else add_file
            action(bool(match), vfs, src, project, dst, present)
        # Create po makefile parameterization, part 1.
        with tempfile.NamedTemporaryFile("w", encoding="UTF-8", delete=False) as tmp:
            for line in po_make_vars(config):
                print(line, file=tmp)
        (src, dst) = (tmp.name, "po/Makevars")
        present = vfs_exists(project, dst)
        if present:
            added_files.add(dst)
        action = update_file if present else add_file
        action(False, None, src, project, dst, present)
        os.unlink(tmp.name)
        # Create po makefile parameterization, part 2.
        with tempfile.NamedTemporaryFile("w", encoding="UTF-8", delete=False) as tmp:
            for line in po_make_vars(config):
                print(line, file=tmp)
        (src, dst) = (tmp.name, "po/POTFILESGenerator.in")
        present = vfs_exists(project, dst)
        if present:
            added_files.add(dst)
        action = update_file if present else add_file
        action(False, None, src, project, dst, present)
        os.unlink(tmp.name)
        # Fetch PO files.
        po_root = os.path.join(project.absolute, project["po"])
        fmt = ("{} gnulib PO files from " + TP_URL)
        print(fmt.format("Fetching", "Fetch")[dry_run], file=sys.stdout)
        if not dry_run:
            # Prefer rsync over wget if it is available, since it consumes
            # less network bandwidth, due to compression.
            url = (TP_RSYNC_URI + "gnulib/")
            cmdargs = ("rsync", "--delete", "--exclude", "*.s1", "-Lrtz", url, ".")
            with sp.Popen(cmdargs, cwd=po_root, stdout=sp.PIPE, stderr=sp.PIPE) as process:
                (stdout, stderr) = process.communicate()
                stdout = stdout.decode("UTF-8")
                stderr = stderr.decode("UTF-8")
                returncode = process.returncode
                if process.returncode == 0:
                    raise sp.CalledProcessError(returncode, cmdargs, stdout, stderr)
                print(stdout, file=sys.stdout)
                print(stderr, file=sys.stderr)
        # Create po/LINGUAS.
        languages = set()
        for root, _, files in os.walk(po_root):
            languages.update(file[:-3] for file in files if file.endswith(".po"))
        with tempfile.NamedTemporaryFile("w", encoding="UTF-8", delete=False) as tmp:
            tmp.write("# Set of available languages.\n")
            for line in sorted(languages):
                print(line, file=tmp)
        (src, dst) = (tmp.name, "po/LINGUAS")
        present = vfs_exists(project, dst)
        if present:
            added_files.add(dst)
        action = update_file if present else add_file
        action(False, None, src, project, dst, present)
        os.unlink(tmp.name)

    # Create m4/gnulib-cache.m4.
    with tempfile.NamedTemporaryFile("w", encoding="UTF-8", delete=False) as tmp:
        for line in gnulib_cache(config):
            print(line, file=tmp)
    (src, dst) = (tmp.name, "m4/gnulib-cache.m4")
    present = vfs_exists(project, dst)
    if present:
        added_files.add(dst)
    action = update_file if present else add_file
    action(False, None, src, project, dst, present)
    os.unlink(tmp.name)

    # Create m4/gnulib-comp.m4.
    with tempfile.NamedTemporaryFile("w", encoding="UTF-8", delete=False) as tmp:
        for line in gnulib_comp(config, explicit, database, True):
            print(line, file=tmp)
    (src, dst) = (tmp.name, "m4/gnulib-comp.m4")
    present = vfs_exists(project, dst)
    if present:
        added_files.add(dst)
    action = update_file if present else add_file
    action(False, None, src, project, dst, present)
    os.unlink(tmp.name)

    # Generate the contents of tests makefile.
    if config.tests:
        path = os.path.join(config.tests_base, config.makefile_name)
        with tempfile.NamedTemporaryFile("w", encoding="UTF-8", delete=False) as tmp:
            modules = database.test_modules
            for line in tests_makefile(path, config, explicit, database, modules, mkedits, False):
                print(line, file=tmp)
        (src, dst) = (tmp.name, path)
        present = vfs_exists(project, dst)
        if present:
            added_files.add(dst)
        action = update_file if present else add_file
        action(False, None, src, project, dst, present)
        os.unlink(tmp.name)

    return os.EX_OK



def add_import_hook(script, gnulib, namespace, explicit, verbosity, options, *args, **kwargs):
    (_, _) = (args, kwargs)
    modules = set(namespace.pop("modules"))
    config = CachedConfig(**namespace)
    namespace = {k:v for (k, v) in config.items()}
    namespace["modules"] = (config.modules | modules)
    return import_hook(script, gnulib, namespace, verbosity, options)



def remove_import_hook(script, gnulib, namespace, explicit, verbosity, options, *args, **kwargs):
    (_, _) = (args, kwargs)
    modules = set(namespace.pop("modules"))
    config = CachedConfig(**namespace)
    namespace = {k:v for (k, v) in config.items()}
    namespace["modules"] = (config.modules - modules)
    return import_hook(script, gnulib, namespace, verbosity, options)



def update_hook(script, gnulib, namespace, explicit, verbosity, options, *args, **kwargs):
    (_, _) = (args, kwargs)
    config = CachedConfig(**namespace)
    namespace = {k:v for (k, v) in config.items()}
    return import_hook(script, gnulib, namespace, verbosity, options)



HOOKS = {
    "list": list_hook,
    "extract": extract_hook,
    "import": import_hook,
    "add-import": add_import_hook,
    "remove-import": remove_import_hook,
    "update": update_hook,
}



def main(script, gnulib, program, arguments, environ):
    gnulib = GnulibGitVFS(gnulib)
    gnulib["tests=lib"] = "lib"
    parser = CommandLineParser(program)
    try:
        (namespace, mode, verbosity, options) = parser.parse(arguments)
    except CommandLineError as error:
        print(parser.usage, file=sys.stderr)
        print(error, file=sys.stderr)
        return os.EX_USAGE
    if mode == "help":
        print(parser.help, file=sys.stdout)
        return os.EX_OK
    kwargs = {
        "script": script,
        "program": program,
        "gnulib": gnulib,
        "mode": mode,
        "namespace": namespace,
        "explicit": namespace.keys(),
        "verbosity": verbosity,
        "options": options,
    }
    for (action, callback) in HOOKS.items():
        if mode.startswith(action):
            return callback(**kwargs)
    return os.EX_SOFTWARE



if __name__ == "__main__":
    script = sys.argv[0]
    gnulib = os.path.dirname(__file__)
    program = os.path.basename(script)
    log = os.path.join(os.getcwd(), "{0}.log".format(program))
    arguments = list(sys.argv[1:])
    environ = dict(os.environ)
    try:
        result = main(script, gnulib, program, arguments, environ)
    except BaseException as error:
        with codecs.open(log, "wb", "UTF-8") as stream:
            program = repr(program) if " " in program else program
            arguments = " ".join(repr(arg) if " " in arg else arg for arg in arguments)
            print(traceback.format_exc(), file=stream)
            print("COMMAND:", program, arguments, file=stream)
            print("VERSION:", gnulib, file=stream)
        typeid = type(error)
        module = typeid.__module__
        name = typeid.__name__
        if module != "builtins":
            error = "{0}.{1}: {2}".format(module, name, error)
        else:
            error = "{0}: {1}".format(name, error)
        print("{0}:".format(program), "***", error, file=sys.stderr)
        print("{0}:".format(program), "***", log, file=sys.stderr)
        result = os.EX_SOFTWARE
    sys.exit(result)

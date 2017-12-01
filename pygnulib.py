#!/usr/bin/python
# encoding: UTF-8



import codecs
import os
import re
import stat
import sys
import traceback

from pygnulib.error import CommandLineError
from pygnulib.error import UnknownModuleError

from pygnulib.config import Base as BaseConfig
from pygnulib.config import Cache as CacheConfig

from pygnulib.module import filelist
from pygnulib.module import dummy_required
from pygnulib.module import libtests_required
from pygnulib.module import transitive_closure

from pygnulib.parser import CommandLine as CommandLineParser

from pygnulib.vfs import backup
from pygnulib.vfs import compare
from pygnulib.vfs import copy
from pygnulib.vfs import exists
from pygnulib.vfs import hardlink
from pygnulib.vfs import lookup
from pygnulib.vfs import move
from pygnulib.vfs import symlink
from pygnulib.vfs import unlink
from pygnulib.vfs import Base as BaseVFS
from pygnulib.vfs import GnulibGit as GnulibGitVFS



AC_VERSION_PATTERN = re.compile(r"AC_PREREQ\(\[(.*?)\]\)", re.S | re.M)
IGNORED_LICENSES = {
    "GPLed build tool",
    "public domain",
    "unlimited",
    "unmodifiable license text",
}
SUBSTITUTION_RULES = {
    "build-aux": "auxdir",
    "doc": "doc_base",
    "lib": "source_base",
    "m4": "m4_base",
    "tests": "tests_base",
    "tests=lib": "tests_base",
    "po": "po_base",
    "top": "",
}



def list_hook(gnulib, *args, **kwargs):
    (_, _) = (args, kwargs)
    for module in sorted(gnulib.modules(full=False)):
        print(module.name)
    return os.EX_OK



def extract_hook(program, gnulib, mode, namespace, *args, **kwargs):
    (_, _) = (args, kwargs)
    mode = mode.replace("extract-", "")
    mode += "s" if mode == "maintainer" else ""
    for module in namespace["modules"]:
        try:
            module = gnulib.module(module, full=True)
            print(module[mode], file=sys.stdout)
        except UnknownModuleError as error:
            print("{0}:".format(program), "warning:", error, file=sys.stderr)
    return os.EX_OK



def import_hook(script, gnulib, namespace, verbosity, options, *args, **kwargs):
    keywords = frozenset({
        "tests",
        "obsolete",
        "cxx_tests",
        "longrunning_tests",
        "privileged_tests",
        "unportable_tests",
    })
    (_, _) = (args, kwargs)
    config = BaseConfig(**namespace)
    cache = CacheConfig(configure=None)
    for key in {"ac_version", "files"}:
        if key not in namespace:
            config[key] = cache[key]
    test_options = {key:config[key] for key in keywords}
    (base, full, main, final, tests) = transitive_closure(gnulib.module, config.modules, **test_options)

    # Print some information about modules.
    print("Module list with included dependencies (indented):", file=sys.stdout)
    BOLD_ON = BOLD_OFF = ""
    mode = os.fstat(sys.stdout.fileno()).st_mode
    if not (stat.S_ISFIFO(mode) or stat.S_ISREG(mode)):
        BOLD_ON = "\033[1m"
        BOLD_OFF = "\033[0m"
    for module in sorted(final):
        manual = module.name in config.modules
        prefix = "  " if manual else "    "
        bold_on = BOLD_ON if manual else ""
        bold_off = BOLD_OFF if manual else ""
        print("{0}{1}{2}{3}".format(prefix, bold_on, module.name, bold_off), file=sys.stdout)
    if verbosity >= 1:
        print("Main module list:", file=sys.stdout)
        for module in sorted(main):
            print("  {0}".format(module.name), file=sys.stdout)
        print("" if main else "\n", end="")
        print("Tests-related module list:", file=sys.stdout)
        for module in sorted(tests):
            print("  {0}".format(module.name), file=sys.stdout)
        print("" if tests else "\n", end="")

    # Determine if dummy needs to be added to main or test sets.
    if "dummy" not in config.avoid:
        if dummy_required(main):
            main.add(gnulib.module("dummy"))
        if libtests_required(tests) and dummy_required(tests):
            tests.add(gnulib.module("dummy"))

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
        for module in sorted(main):
            name = module.name
            notice = module.notice
            if notice.strip():
                print("Notice from module {0}:".format(name), file=sys.stdout)
                print("\n".join("  " + line for line in notice.splitlines()), file=sys.stdout)

    # Determine the final file lists.
    main_files = filelist(main, config.ac_version)
    tests_files = filelist(tests, config.ac_version)
    for file in tests_files:
        if file.startswith("lib/"):
            tests_files.remove(file)
            file = "tests=lib/" + file[len("lib/"):]
            tests_files.add(file)
    files = (main_files | tests_files)
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


    overrides = []
    root = config.root
    table = {k:config[v] for (k, v) in SUBSTITUTION_RULES.items() if v}
    for override in config.overrides:
        vfs = BaseVFS(override, **table)
        overrides.append(vfs)
    project = BaseVFS(root, **table)

    old_files = set(cache.files)
    if "m4/gnulib-tool.m4" in project:
        old_files |= set(["m4/gnulib-tool.m4"])
    table = {k:cache[v] for (k, v) in SUBSTITUTION_RULES.items() if v}
    with BaseVFS(config.root, **table) as vfs:
        old_files = frozenset({vfs[file] for file in old_files})
    new_files = frozenset(files | set(["m4/gnulib-tool.m4"]))


    dry_run = options["dry_run"]
    def remove_file(project, file):
        action = ("Removing", "Remove")[dry_run]
        fmt = (action + " file {file} (backup in {file}~)")
        if not dry_run:
            backup(project, file)
            unlink(project, file)
        print(fmt.format(file=file))

    def update_file(src_vfs, src_name, dst_vfs, dst_name, present):
        if not compare(src_vfs, src_name, dst_vfs, dst_name):
            action = (("Replacing", "Replace"), ("Updating", "Update"))[present][dry_run]
            message = ("(non-gnulib code backed up in {file}~) !!", "(backup in {file}~)")[present]
            fmt = (action + " file {file} " + message)
            if not dry_run:
                backup(dst_vfs, dst_name)
                copy(src_vfs, src_name, dst_vfs, dst_name)
            print(fmt.format(file=dst_name))

    def add_file(src_vfs, src_name, dst_vfs, dst_name, present):
        action = ("Copying", "Copy")[dry_run]
        fmt = (action + " file {file}")
        if not dry_run:
            copy(src_vfs, src_name, dst_vfs, dst_name)
        print(fmt.format(file=dst_name))


    # First the files that are in old-files, but not in new-files.
    removed_files = {file for file in old_files if file not in new_files}
    for file in sorted(removed_files):
        remove_file(project, file)

    # Then the files that are in new-files, but not in old-files.
    added_files = {file for file in new_files if file not in old_files}
    for dst in sorted(added_files):
        match = [override for override in overrides if dst in override]
        override = match[0] if len(match) else gnulib
        (vfs, src) = lookup(dst, gnulib, override, patch="patch")
        action = update_file if exists(project, dst) else add_file
        action(vfs, src, project, dst, present=False)

    # Then the files that are in new-files and in old-files.
    kept_files = (old_files & new_files)
    for dst in sorted(kept_files):
        match = [override for override in overrides if dst in override]
        override = match[0] if len(match) else gnulib
        (vfs, src) = lookup(dst, gnulib, override, patch="patch")
        action = update_file if exists(project, dst) else add_file
        action(vfs, src, project, dst, present=True)

    return os.EX_OK



def add_import_hook(script, gnulib, namespace, verbosity, options, *args, **kwargs):
    (_, _) = (args, kwargs)
    modules = set(namespace.pop("modules"))
    config = CacheConfig(**namespace)
    namespace = {k:v for (k, v) in config.items()}
    namespace["modules"] = (config.modules | modules)
    return import_hook(script, gnulib, namespace, verbosity, options)



def remove_import_hook(script, gnulib, namespace, verbosity, options, *args, **kwargs):
    (_, _) = (args, kwargs)
    modules = set(namespace.pop("modules"))
    config = CacheConfig(**namespace)
    namespace = {k:v for (k, v) in config.items()}
    namespace["modules"] = (config.modules - modules)
    return import_hook(script, gnulib, namespace, verbosity, options)



def update_hook(script, gnulib, namespace, verbosity, options, *args, **kwargs):
    (_, _) = (args, kwargs)
    config = CacheConfig(**namespace)
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
        "verbosity": verbosity,
        "options": options,
    }
    for (action, callback) in HOOKS.items():
        if mode.startswith(action):
            return callback(**kwargs)
    return os.EX_SOFTWARE



if __name__ == "__main__":
    script = sys.argv[0]
    gnulib = os.path.dirname(os.path.realpath(__file__))
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

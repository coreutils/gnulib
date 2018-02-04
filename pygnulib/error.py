#!/usr/bin/python
# encoding: UTF-8
"""gnulib miscellaneous error classes"""



class AutoconfVersionError(Exception):
    """minimum supported autoconf version mismatch"""
    def __init__(self, version):
        fmt = "minimum supported autoconf version is {0}"
        super().__init__(fmt.format(version))



class CommandLineError(Exception):
    """command-line parsing error"""
    def __init__(self, program, message):
        super().__init__("{0}: {1}".format(program, message))



class EmptyFileListError(Exception):
    """cannot process empty file list"""
    def __init__(self):
        super().__init__("cannot process empty file list")



class ConditionalDependenciesUnavailableError(Exception):
    """conditional dependencies are not supported with tests"""
    def __init__(self):
        super().__init__("conditional dependencies are not supported with tests")



class IncompatibleLicenseError(Exception):
    """incompatible licenses on modules"""
    def __init__(self, modules):
        fmt = "incompatible licenses on modules: {0}"
        super().__init__(fmt.format(" ".join(modules)))



class M4BaseMismatchError(Exception):
    """unexpected gl_M4_BASE macro value"""
    def __init__(self, path, expected, actual):
        fmt = "{0} is expected to contain gl_M4_BASE([{1}]), not gl_M4_BASE([{2}])"
        super().__init__(fmt.format(path, expected, actual))



class ModuleLicenseError(Exception):
    """module lacks a license"""
    def __init__(self, module):
        fmt = "module lacks a license: {0}"
        super().__init__(fmt.format(module))



class UnknownLicenseError(Exception):
    """unknown license"""
    def __init__(self, variant):
        fmt = "unknown license: {0}"
        super().__init__(fmt.format(variant))



class UnknownModuleError(Exception):
    """unknown gnulib module"""
    def __init__(self, name):
        fmt = "unknown gnulib module: {0}"
        super().__init__(fmt.format(name))

#!/usr/bin/python
# encoding: UTF-8



def type_assert(key, value, types):
    typeset = []
    if isinstance(types, type):
        types = [types]
    types = tuple(types)
    if not isinstance(value, types):
        for typeid in types:
            module = typeid.__module__
            name = typeid.__name__
            if module == "builtins":
                typeset += [name]
            else:
                typeset += [module + "." + name]
        typeset = "{0}".format(", ".join(typeset))
        raise TypeError("{0}: {1} expected".format(key, typeset))



class AutoconfVersionError(Exception):
    """minimum supported autoconf version mismatch"""
    def __init__(self, version):
        fmt = "minimum supported autoconf version is %f"
        super().__init__(fmt.format(version))



class CommandLineParsingError(Exception):
    def __init__(self, program, message):
        super().__init__("{0}: {1}".format(program, message))



class M4BaseMismatchError(Exception):
    """<gnulib-comp.m4> is expected to contain gl_M4_BASE([m4base])"""
    def __init__(self, m4_base):
        fmt = "<gnulib-comp.m4> is expected to contain gl_M4_BASE([{0}])"
        super().__init__(fmt.format(m4_base))



class ConditionalDependenciesUnavailableError(Exception):
    """conditional dependencies are not supported with tests"""
    def __init__(self):
        super().__init__("conditional dependencies are not supported with tests")



class IncompatibleLicenseError(Exception):
    """incompatible licenses on modules"""
    def __init__(self, modules):
        fmt = "incompatible licenses on modules: {0}"
        super().__init__(fmt.format(" ".join(modules)))



class EmptyFileListError(Exception):
    """cannot process empty file list"""
    def __init__(self):
        super().__init__("cannot process empty file list")



class UnknownLicenseError(Exception):
    """module lacks a license"""
    def __init__(self, module):
        fmt = "module lacks a license: {0}"
        super().__init__(fmt.format(module))

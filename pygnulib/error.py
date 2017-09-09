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
        typeset = "{%s}" % ", ".join(set(typeset))
        raise TypeError("{0}: {1} expected".format(key, typeset))



class AutoconfVersionError(Exception):
    """minimum supported autoconf version mismatch"""
    def __init__(self, version):
        fmt = "minimum supported autoconf version is %f"
        message = fmt % version
        super().__init__(message)



class M4BaseMismatchError(Exception):
    """<gnulib-comp.m4> is expected to contain gl_M4_BASE([m4base])"""
    def __init__(self, m4_base):
        fmt = "<gnulib-comp.m4> is expected to contain gl_M4_BASE([%s])"
        message = fmt % m4_base
        super().__init__(message)



class ConditionalDependenciesUnavailableError(Exception):
    """conditional dependencies are not supported with tests"""
    def __init__(self):
        message = "conditional dependencies are not supported with tests"
        super().__init__(message)



class IncompatibleLicenseError(Exception):
    """incompatible licenses on modules"""
    def __init__(self, modules):
        fmt = "incompatible licenses on modules: %r"
        message = fmt % modules
        super().__init__(message)



class EmptyFileListError(Exception):
    """cannot process empty file list"""
    def __init__(self):
        message = "cannot process empty file list"
        super().__init__(message)



class UnknownLicenseError(Exception):
    """module lacks a license"""
    def __init__(self, module):
        fmt = "module lacks a license: %r"
        message = fmt % module
        super().__init__(message)

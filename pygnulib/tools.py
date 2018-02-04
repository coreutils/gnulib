#!/usr/bin/python
# encoding: UTF-8
"""gnulib command-line tools"""



import subprocess as _sp



class _PipeMeta(type):
    __INSTANCE = None
    def __call__(cls, *args, **kwargs):
        if _PipeMeta.__INSTANCE is None:
            _PipeMeta.__INSTANCE = super(_PipeMeta, cls).__call__(*args, **kwargs)
        return _PipeMeta.__INSTANCE


class Pipe(metaclass=_PipeMeta):
    """pipe handle singleton"""
    pass



class Executable:
    """command-line program or script"""
    def __init__(self, name, path=None, encoding=None):
        self.__name = name
        self.__path = path
        self.__encoding = encoding


    @property
    def name(self):
        """executable name"""
        return self.__name


    @property
    def path(self):
        """executable path"""
        return self.__path if self.__path else self.name


    def __call__(self, *args, **kwargs):
        """
        Invoke command-line tool with the given arguments.
        Upon execution subprocess.Popen instance is returned.
        """
        args = ([self.path] + list(args))
        for key in ("stdin", "stdout", "stderr"):
            if isinstance(kwargs.get(key, Pipe()), Pipe):
                kwargs[key] = _sp.PIPE
        kwargs.setdefault("encoding", self.__encoding)
        return _sp.Popen(args, **kwargs)

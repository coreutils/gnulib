#!/usr/bin/python
# encoding: UTF-8


import codecs
import os
import re



class Module:
    """gnulib generic module"""
    _PATTERN_ = re.compile("")
    _TABLE_ = {
        "description"   : (0x00, str, None, "Description"),
        "comment"       : (0x01, str, None, "Comment"),
        "status"        : (0x02, str, None, "Status"),
        "notice"        : (0x03, str, None, "Notice"),
        "applicability" : (0x04, str, None, "Applicability"),
        "files"         : (0x05, list, None, "Files"),
        "dependencies"  : (0x06, list, None, "Depends-on"),
        "preconfigure"  : (0x07, str, None, "configure.ac-early"),
        "configure"     : (0x08, str, None, "configure.ac"),
        "makefile"      : (0x09, str, None, "Makefile.am"),
        "include"       : (0x0A, list, None, "Include"),
        "link"          : (0x0B, list, None, "Link"),
        "license"       : (0x0C, str, None, "License"),
        "maintainers"   : (0x0D, list, None, "Maintainer"),
    }
    for _key_, (_uid_, _typeid_, _, _field_) in _TABLE_.items():
        _suffix_ = ("(?:" + ":|".join([_[3] for _ in _TABLE_.values()]) + ":)")
        _pattern_ = re.compile(("^%s:\\s*(.*?)%s" % (_field_, _suffix_)), re.S | re.M)
        _TABLE_[_key_] = (_uid_, _typeid_, _pattern_, _field_)


    def __init__(self, name, **kwargs):
        if not isinstance(name, str):
            raise TypeError("name must be of 'str' type")
        self.__dict__["_name_"] = name
        self.__dict__["_table_"] = dict()
        for key in Module._TABLE_:

            self.__dict__["_table_"][key] = ""
        self.__dict__["_table_"]["maintainers"] = ["all"]
        for key, value in kwargs.items():
            self[key] = value


    @property
    def name(self):
        return self.__dict__["_name_"]


    @property
    def dependencies(self):
        pattern = re.compile("^([A-Za-z0-9_\\-\\+/]+)(?:\\s+(.+))*$", re.S)
        for dep in self.__dict__["_table_"]["dependencies"]:
            yield pattern.findall(dep)[0]


    def __hash__(self):
        return hash(str(self))


    def __repr__(self):
        return self.__dict__["_name_"]


    def __str__(self):
        result = ""
        for key, (_, typeid, _, field) in sorted(Module._TABLE_.items(), key=lambda k: k[1][0]):
            field += ":\n"
            if typeid is list:
                value = "\n".join(self[key])
            else:
                value = self[key]
            if value:
                result += field
                result += value
                result += "\n\n" if value else "\n"
        return result.strip() + "\n"


    def __setattr__(self, key, value):
        self[key] = value
        self.__dict__[key] = value


    def __getattr__(self, key):
        return self[key]


    def __getitem__(self, key):
        if key not in Module._TABLE_:
            raise ValueError("unsupported key: %r" % key)
        return self.__dict__["_table_"][key]


    def __setitem__(self, key, value):
        if key not in Module._TABLE_:
            raise ValueError("unsupported key: %r" % key)
        typeid = Module._TABLE_[key][1]
        if not isinstance(value, typeid):
            typename = typeid.__name__
            raise TypeError("%r key must be of %r type" % (key, typename))
        self.__dict__["_table_"][key] = value


    def __lt__(self, value):
        if isinstance(value, Module):
            return (self != value) and (self.__dict__["_name_"] < value.name)
        return TypeError("cannot compare pygnulib.Module with %r type" % type(value))

    def __le__(self, value):
        return self.__lt__(value) or self.__eq__(value)

    def __eq__(self, value):
        if isinstance(value, Module):
            return (self.__dict__["_name_"] == value.__dict__["_name_"]) \
                and (self.__dict__["_table_"] == value.__dict__["_table_"])
        return TypeError("cannot compare pygnulib.Module with %r type" % type(value))

    def __ne__(self, value):
        return not self.__eq__(value)

    def __ge__(self, value):
        if isinstance(value, Module):
            return value.__le__(self)
        return TypeError("cannot compare pygnulib.Module with %r type" % type(value))

    def __gt__(self, value):
        if isinstance(value, Module):
            return value.__lt__(self)
        return TypeError("cannot compare pygnulib.Module with %r type" % type(value))



class FileModule(Module):
    """gnulib module text file"""

    def __init__(self, path, mode="r", name=None, **kwargs):
        if name is None:
            name = os.path.basename(path)
        if mode not in ("r", "w", "rw"):
            raise ValueError("illegal mode: %r" % mode)
        if mode == "r":
            super(FileModule, self).__init__(name)
            with codecs.open(path, "rb", "UTF-8") as stream:
                data = stream.read()
            for key, (_, typeid, pattern, _) in Module._TABLE_.items():
                pattern = re.compile(pattern)
                match = pattern.findall(data)
                self[key] = [_ for _ in "".join(match).split("\n") if _.strip()] \
                            if typeid is list else \
                            ("\n".join([_.strip() for _ in match]) if match else "")
            self["dependencies"] = [_ for _ in self["dependencies"] if not _.startswith("#")]
            self.__dict__["_stream_"] = None
        elif mode == "w":
            super(FileModule, self).__init__(name)
            self.__dict__["_stream_"] = codecs.open(path, "w+", "UTF-8")
        elif mode == "rw":
            self.__init__(path, "r")
            self.__dict__["_stream_"] = codecs.open(path, "w+", "UTF-8")
        else:
            raise ValueError("invalid mode: %r" % mode)
        for key, value in kwargs.items():
            self[key] = value


    def close(self):
        if self.__dict__["_stream_"]:
            self.__dict__["_stream_"].truncate(0)
            self.__dict__["_stream_"].write(str(self))
            self.__dict__["_stream_"].close()


    def __enter__(self):
        return self


    def __exit__(self, exctype, excval, exctrace):
        self.close()

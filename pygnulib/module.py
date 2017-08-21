#!/usr/bin/python
# encoding: UTF-8


import os
import re
import codecs


class Module:
    _TABLE_ = {
        "description"   : (0x00,  str, None, "Description"),
        "comment"       : (0x01,  str, None, "Comment"),
        "status"        : (0x02,  str, None, "Status"),
        "notice"        : (0x03,  str, None, "Notice"),
        "applicability" : (0x04,  str, None, "Applicability"),
        "files"         : (0x05, list, None, "Files"),
        "dependencies"  : (0x06, list, None, "Depends-on"),
        "preconfigure"  : (0x07,  str, None, "configure.ac-early"),
        "configure"     : (0x08,  str, None, "configure.ac"),
        "makefile"      : (0x09,  str, None, "Makefile.am"),
        "include"       : (0x0A, list, None, "Include"),
        "link"          : (0x0B, list, None, "Link"),
        "license"       : (0x0C,  str, None, "License"),
        "maintainers"   : (0x0D, list, None, "Maintainer"),
    }
    _EXCLUDE_ = {
        "CVS",
        "ChangeLog",
        "COPYING",
        "README",
        "TEMPLATE",
        "TEMPLATE-EXTENDED",
    }
    for key, (uid, typeid, _, field) in _TABLE_.items():
        suffix = ("(?:" + ":|".join([_[3] for _ in _TABLE_.values()]) + ":)")
        pattern = re.compile(("^%s:\\s*(.*?)%s" % (field, suffix)), re.S | re.M)
        _TABLE_[key] = (uid, typeid, pattern, field)


    def __init__(self):
        self._table_ = dict()


    def __getitem__(self, key):
        if key not in Module._TABLE_:
            raise ValueError("unsupported key: '%s'" % key)
        return self._table_[key]


    def __setitem__(self, key, value):
        if key not in Module._TABLE_:
            raise ValueError("unsupported key: '%s'" % key)
        typeid = Module._TABLE_[key][1]
        if not isinstance(value, typeid):
            raise TypeError("'%s' key expects '%s' type" % (key, typeid))
        self._table_[key] = value


    @classmethod
    def file(cls, path):
        module = Module()
        with codecs.open(path, "rb", "UTF-8") as stream:
            data = stream.read()
        for key, (_, typeid, pattern, field) in Module._TABLE_.items():
            match = pattern.findall(data)
            if typeid is list:
                module[key] = [_ for _ in "".join(match).split("\n") if _.strip()]
            else:
                module[key] = "\n".join([_.strip() for _ in match]) if match else ""
        return module


    @classmethod
    def list(cls, config):
        base = os.path.join(config["gnulib"], "modules")
        for root, _, files in os.walk(base):
            for file in [_ for _ in files if _ not in Module._EXCLUDE_]:
                yield file


    def package(self):
        result = ""
        for key, (_, typeid, _, field) in sorted(Module._TABLE_.items(), key=lambda k: k[1][0]):
            field += ":\n"
            if typeid is list:
                value = "\n".join(self[key])
            else:
                value = self[key]
            if key == "maintainers" and not value:
                value = "all"
            if value:
                result += field
                result += value
                result += "\n\n" if value else "\n"
        return result.strip() + "\n"

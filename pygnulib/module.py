#!/usr/bin/python
# encoding: UTF-8



import codecs
import collections
import hashlib
import os
import re



class Module:
    """gnulib generic module"""
    _PATTERN_ = re.compile("")
    _TABLE_ = {
        "description"     : (0x00, str, None, "Description"),
        "comment"         : (0x01, str, None, "Comment"),
        "status"          : (0x02, str, None, "Status"),
        "notice"          : (0x03, str, None, "Notice"),
        "applicability"   : (0x04, str, None, "Applicability"),
        "files"           : (0x05, list, None, "Files"),
        "dependencies"    : (0x06, list, None, "Depends-on"),
        "configure_early" : (0x07, str, None, "configure.ac-early"),
        "configure"       : (0x08, str, None, "configure.ac"),
        "makefile"        : (0x09, str, None, "Makefile.am"),
        "include"         : (0x0A, list, None, "Include"),
        "link"            : (0x0B, list, None, "Link"),
        "license"         : (0x0C, str, None, "License"),
        "maintainers"     : (0x0D, list, None, "Maintainer"),
    }
    for _key_, (_uid_, _typeid_, _, _field_) in _TABLE_.items():
        _suffix_ = ("(?:" + ":|".join([_[3] for _ in _TABLE_.values()]) + ":)")
        _pattern_ = re.compile(("^%s:\\s*(.*?)%s" % (_field_, _suffix_)), re.S | re.M)
        _TABLE_[_key_] = (_uid_, _typeid_, _pattern_, _field_)


    def __init__(self, name, **kwargs):
        if not isinstance(name, str):
            raise TypeError("name must be of 'str' type")
        self._name_ = name
        self._table_ = {"maintainers": ["all"]}
        for key in Module._TABLE_:
            self._table_[key] = ""
        for key, value in kwargs.items():
            self._table_[key] = value


    @property
    def name(self):
        """name"""
        return self._name_

    @name.setter
    def name(self, value):
        if not isinstance(value, str):
            raise TypeError("'str' type is expected")
        self._name_ = value


    @property
    def description(self):
        """description"""
        return self._table_["description"]

    @description.setter
    def description(self, value):
        if not isinstance(value, str):
            raise TypeError("'str' type is expected")
        self._table_["description"] = value


    @property
    def comment(self):
        """comment"""
        return self._table_["comment"]

    @comment.setter
    def comment(self, value):
        if not isinstance(value, str):
            raise TypeError("'str' type is expected")
        self._table_["comment"] = value


    @property
    def status(self):
        """status"""
        return self._table_["status"]

    @status.setter
    def status(self, value):
        if not isinstance(value, str):
            raise TypeError("'str' type is expected")
        self._table_["status"] = value


    @property
    def notice(self):
        """notice"""
        return self._table_["notice"]

    @notice.setter
    def notice(self, value):
        if not isinstance(value, str):
            raise TypeError("'str' type is expected")
        self._table_["notice"] = value


    @property
    def applicability(self):
        """applicability (usually "main" or "tests")"""
        default = "main" if self.name.endswith("-tests") else "tests"
        current = self._table_["applicability"]
        return current if current.strip() else default

    @applicability.setter
    def applicability(self, value):
        if not isinstance(value, str):
            raise TypeError("'str' type is expected")
        self._table_["applicability"] = value


    @property
    def files(self):
        """file dependencies iterator (set of strings)"""
        for file in self._table_["files"]:
            yield file

    @files.setter
    def files(self, iterable):
        if isinstance(iterable, (bytes, str)) \
        or not isinstance(iterable, collections.Iterable):
            raise TypeError("iterable of strings is expected")
        result = set()
        for item in iterable:
            if not isinstance(item, str):
                raise TypeError("iterable of strings is expected")
            result.update([item])
        self._table_["files"] = result


    @property
    def dependencies(self):
        """dependencies iterator (name, condition)"""
        pattern = re.compile("^([A-Za-z0-9_\\-\\+/]+)(?:\\s+(.+))*$", re.S)
        for entry in self._table_["dependencies"]:
            yield pattern.findall(entry)[0]

    @dependencies.setter
    def dependencies(self, iterable):
        error = TypeError("iterable of pairs (name, condition) is expected")
        if isinstance(iterable, (bytes, str)) \
        or not isinstance(iterable, collections.Iterable):
            raise error
        result = set()
        try:
            for pair in iterable:
                (name, condition) = pair
                if not isinstance(name, str) \
                or not isinstance(condition, str):
                    raise error
                result.update([(name, condition)])
        except ValueError:
            raise error
        self._table_["dependencies"] = result


    @property
    def configure_early(self):
        """early configure.ac snippet"""
        return self._table_["configure_early"]

    @configure_early.setter
    def configure_early(self, value):
        if not isinstance(value, str):
            raise TypeError("'str' type is expected")
        self._table_["configure_early"] = value


    @property
    def configure(self):
        """configure.ac snippet"""
        return self._table_["configure"]

    @configure.setter
    def configure(self, value):
        if not isinstance(value, str):
            raise TypeError("'str' type is expected")
        self._table_["configure"] = value


    @property
    def makefile(self):
        """makefile snippet"""
        return self._table_["makefile"]

    @makefile.setter
    def makefile(self, value):
        if not isinstance(value, str):
            raise TypeError("'str' type is expected")
        self._table_["makefile"] = value


    @property
    def include(self):
        """include files iterator (header, comment)"""
        pattern = re.compile("^#include\\s\\<([A-Za-z0-9/\\-_]\\.h)\\>(?:\\s+.*^)*$")
        for entry in self._table_["include"]:
            yield pattern.findall(entry)[0]

    @include.setter
    def include(self, iterable):
        error = TypeError("iterable of pairs (header, comment) is expected")
        if isinstance(iterable, (bytes, str)) \
        or not isinstance(iterable, collections.Iterable):
            raise error
        result = set()
        try:
            for pair in iterable:
                (header, comment) = pair
                if not isinstance(header, str) \
                or not isinstance(comment, str):
                    raise error
                result.update([(header, comment)])
        except ValueError:
            raise error
        self._table_["include"] = result


    @property
    def link(self):
        """linker directives iterator (directive, comment)"""
        pattern = re.compile("^\\<([A-Za-z0-9/\\-_])\\>(?:\\s+.*^)*$")
        for entry in self._table_["link"]:
            yield pattern.findall(entry)[0]

    @link.setter
    def link(self, iterable):
        error = TypeError("iterable of pairs (directive, comment) is expected")
        if isinstance(iterable, (bytes, str)) \
        or not isinstance(iterable, collections.Iterable):
            raise error
        result = set()
        try:
            for pair in iterable:
                (directive, comment) = pair
                if not isinstance(directive, str) \
                or not isinstance(comment, str):
                    raise error
                result.update([(directive, comment)])
        except ValueError:
            raise error
        self._table_["link"] = result


    @property
    def license(self):
        """license"""
        return self._table_["license"]

    @license.setter
    def license(self, value):
        if not isinstance(value, str):
            raise TypeError("'str' type is expected")
        self._table_["license"] = value


    @property
    def maintainers(self):
        """maintainers iterator (maintainer)"""
        for entry in self._table_["maintainers"]:
            yield entry

    @maintainers.setter
    def maintainers(self, iterable):
        if isinstance(iterable, (bytes, str)) \
        or not isinstance(iterable, collections.Iterable):
            raise TypeError("iterable of strings is expected")
        result = set()
        for item in iterable:
            if not isinstance(item, str):
                raise TypeError("iterable of strings is expected")
            result.update([item])
        self._table_["maintainers"] = result


    def shell_variable(self, macro_prefix="gl"):
        """Get the name of the shell variable set to true once m4 macros have been executed."""
        module = self.name
        if len(module) != len(module.encode()):
            module = (module + "\n").encode("UTF-8")
            module = hashlib.md5(module).hexdigest()
        return "%s_gnulib_enabled_%s" % (macro_prefix, module)


    def shell_function(self, macro_prefix="gl"):
        """Get the name of the shell function containing the m4 macros."""
        module = self.name
        if len(module) != len(module.encode()):
            module = (module + "\n").encode("UTF-8")
            module = hashlib.md5(module).hexdigest()
        return "func_%s_gnulib_m4code_%s" % (macro_prefix, module)


    def conditional_name(self, macro_prefix="gl"):
        """Get the automake conditional name."""
        module = self.name
        if len(module) != len(module.encode()):
            module = (module + "\n").encode("UTF-8")
            module = hashlib.md5(module).hexdigest()
        return "%s_GNULIB_ENABLED_%s" % (macro_prefix, module)


    def __hash__(self):
        return hash(str(self))


    def __repr__(self):
        return self._name_


    def __str__(self):
        result = ""
        for key, (_, typeid, _, field) in sorted(Module._TABLE_.items(), key=lambda k: k[1][0]):
            field += ":\n"
            if typeid is list:
                value = "\n".join(self._table_[key])
            else:
                value = self._table_[key]
            if value:
                result += field
                result += value
                result += "\n\n" if value else "\n"
        return result.strip() + "\n"


    def __lt__(self, value):
        return self.name < value.name

    def __le__(self, value):
        return self.__lt__(value) or self.__eq__(value)

    def __eq__(self, value):
        return self.name == value.name

    def __ne__(self, value):
        return not self.__eq__(value)

    def __ge__(self, value):
        return value.__le__(self)

    def __gt__(self, value):
        return value.__lt__(self)



class FileModule(Module):
    """gnulib module text file"""

    def __init__(self, path, mode="r", name=None, **kwargs):
        if name is None:
            name = os.path.basename(path)
        if mode not in ("r", "w", "rw"):
            raise ValueError("illegal mode: %r" % mode)
        if mode == "r":
            super().__init__(name)
            with codecs.open(path, "rb", "UTF-8") as stream:
                data = ""
                for line in stream:
                    if line.startswith("#"):
                        continue
                    data += line
            for key, (_, typeid, pattern, _) in Module._TABLE_.items():
                pattern = re.compile(pattern)
                match = pattern.findall(data)
                self._table_[key] = [_ for _ in "".join(match).split("\n") if _.strip()] \
                                    if typeid is list else \
                                    ("\n".join([_.strip() for _ in match]) if match else "")
            self._stream_ = None
        elif mode == "w":
            super().__init__(name)
            self._stream_ = codecs.open(path, "w+", "UTF-8")
        elif mode == "rw":
            self.__init__(path, "r")
            self._stream_ = codecs.open(path, "w+", "UTF-8")
        else:
            raise ValueError("invalid mode: %r" % mode)
        for key, value in kwargs.items():
            self._table_[key] = value


    def close(self):
        """Close the underlying stream and write data into the file."""
        if self._stream_:
            self._stream_.truncate(0)
            self._stream_.write(str(self))
            self._stream_.close()


    def __enter__(self):
        return self


    def __exit__(self, exctype, excval, exctrace):
        self.close()

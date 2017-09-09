#!/usr/bin/python
# encoding: UTF-8



import codecs
import collections
import hashlib
import os
import re



class Module:
    """gnulib generic module"""
    _TABLE_ = {
        "description"            : (0x00, str, "Description"),
        "comment"                : (0x01, str, "Comment"),
        "status"                 : (0x02, str, "Status"),
        "notice"                 : (0x03, str, "Notice"),
        "applicability"          : (0x04, str, "Applicability"),
        "files"                  : (0x05, list, "Files"),
        "dependencies"           : (0x06, list, "Depends-on"),
        "early_autoconf_snippet" : (0x07, str, "configure.ac-early"),
        "autoconf_snippet"       : (0x08, str, "configure.ac"),
        "automake_snippet"       : (0x09, str, "Makefile.am"),
        "include"                : (0x0A, list, "Include"),
        "link"                   : (0x0B, list, "Link"),
        "license"                : (0x0C, str, "License"),
        "maintainers"            : (0x0D, list, "Maintainer"),
    }
    _PATTERN_DEPENDENCIES_ = re.compile("^(\\S+)(?:\\s+(.+))*$")
    _PATTERN_INCLUDE_ = re.compile("^[\\<\"]([A-Za-z0-9/\\-_]+\\.h)[\\>\"](?:\\s+.*^)*$")


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
        for entry in self._table_["dependencies"]:
            yield Module._PATTERN_DEPENDENCIES_.findall(entry)[0]

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
    def early_autoconf_snippet(self):
        """early configure.ac snippet"""
        return self._table_["early_autoconf_snippet"]

    @early_autoconf_snippet.setter
    def early_autoconf_snippet(self, value):
        if not isinstance(value, str):
            raise TypeError("'str' type is expected")
        self._table_["early_autoconf_snippet"] = value


    @property
    def autoconf_snippet(self):
        """configure.ac snippet"""
        return self._table_["autoconf_snippet"]

    @autoconf_snippet.setter
    def autoconf_snippet(self, value):
        if not isinstance(value, str):
            raise TypeError("'str' type is expected")
        self._table_["autoconf_snippet"] = value


    @property
    def automake_snippet(self):
        """Makefile.am snippet"""
        return self._table_["automake_snippet"]

    @automake_snippet.setter
    def automake_snippet(self, value):
        if not isinstance(value, str):
            raise TypeError("'str' type is expected")
        self._table_["automake_snippet"] = value


    @property
    def include(self):
        """include files iterator (header, comment)"""
        for entry in self._table_["include"]:
            match = Module._PATTERN_INCLUDE_.findall(entry)
            yield match[0] if match else entry

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
        """linkage iterator (string)"""
        for entry in self._table_["link"]:
            yield entry

    @link.setter
    def link(self, iterable):
        if isinstance(iterable, (bytes, str)) \
        or not isinstance(iterable, collections.Iterable):
            raise TypeError("iterable of strings is expected")
        result = set()
        for item in iterable:
            if not isinstance(item, str):
                raise TypeError("iterable of strings is expected")
            result.update([item])
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
        for key, (_, typeid, field) in sorted(Module._TABLE_.items(), key=lambda k: k[1][0]):
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
    _TABLE_ = {
        "Description"        : (str, "description"),
        "Comment"            : (str, "comment"),
        "Status"             : (str, "status"),
        "Notice"             : (str, "notice"),
        "Applicability"      : (str, "applicability"),
        "Files"              : (list, "files"),
        "Depends-on"         : (list, "dependencies"),
        "configure.ac-early" : (str, "early_autoconf_snippet"),
        "configure.ac"       : (str, "autoconf_snippet"),
        "Makefile.am"        : (str, "automake_snippet"),
        "Include"            : (list, "include"),
        "Link"               : (list, "link"),
        "License"            : (str, "license"),
        "Maintainer"         : (list, "maintainers"),
    }
    _FIELDS_ = [field for (_, _, field) in Module._TABLE_.values()]
    _PATTERN_ = re.compile("(%s):" % "|".join(_FIELDS_))

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
                    line = line.strip("\n")
                    if line.startswith("#") \
                    or (line.startswith("/*") and line.endswith("*/")):
                        continue
                    data += (line + "\n")
            match = FileModule._PATTERN_.split(data)[1:]
            for (group, value) in zip(match[::2], match[1::2]):
                (typeid, key) = FileModule._TABLE_[group]
                if typeid is list:
                    self._table_[key] = [_ for _ in "".join(value).split("\n") if _.strip()]
                else:
                    self._table_[key] = value.strip()
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

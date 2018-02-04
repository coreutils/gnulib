#!/usr/bin/python
# encoding: UTF-8
"""miscellaneous tools and traits"""



import os as _os
from collections import OrderedDict as _OrderedDict



class Property(property):
    """generic property"""
    @staticmethod
    def __fget(*args, **kwargs):
        raise AttributeError("unreadable attribute")

    @staticmethod
    def __fset(*args, **kwargs):
        raise AttributeError("can't set attribute")


    def __init__(self, fget=None, fset=None, doc=None, check=None):
        """
        doc  : any arbitrary string
        fget : function to get the value
        fset : function to set the value

        def fget(self):
            return self.value

        def fset(self, value):
            self.value = value
        """
        fget = Property.__fget if fget is None else fget
        fset = Property.__fset if fset is None else fset
        # if not callable(fget):
        #     raise TypeError("fget: callable expected")
        # if not callable(fset):
        #     raise TypeError("fset: callable expected")
        super().__init__(fget=fget, fset=fset, doc=doc)
        self.__check = check
        self.__fget = fget
        self.__fset = fset
        self.__doc__ = doc


    def __set__(self, obj, val):
        if self.__check:
            if not self.__check(val):
                raise ValueError("value check failed")
        return super().__set__(obj, val)


class PathProperty(Property):
    """file system path property"""
    def __init__(self, fget=None, fset=None, doc=None):
        """
        doc   : any arbitrary string
        fget  : function to get the value
        fset  : function to set the value

        def fget(self):
            return self.path

        def fset(self, path):
            self.path = path
        """
        super().__init__(fget=fget, fset=fset, doc=doc)


    def __set__(self, obj, val):
        if not isinstance(val, str):
            raise TypeError("value: str expected")
        if not val:
            raise ValueError("value: empty path")
        val = _os.path.normpath(val)
        return super().__set__(obj, val)



class BitwiseProperty(Property):
    """bitwise flag property"""
    def __init__(self, mask, fget=None, fset=None, doc=None):
        """
        mask : int bitwise mask
        doc  : any arbitrary string
        fget : function to get the value
        fset : function to set the value

        def fget(self, mask):
            return bool(self.mask & mask)

        def fset(self, mask, state):
            if state:
                self.mask |= mask
            else:
                self.mask &= ~mask
        """
        if not isinstance(mask, int):
            raise TypeError("mask: int expected")

        def _fget(obj):
            return self.__fget(obj, self.__mask)

        def _fset(obj, state):
            return self.__fset(obj, self.__mask, state)

        super().__init__(doc=doc, fget=_fget, fset=_fset)
        self.__mask = mask
        self.__fget = fget
        self.__fset = fset


    @property
    def mask(self):
        return self.__mask


    def __set__(self, obj, val):
        if not isinstance(val, bool):
            raise TypeError("value: bool expected")
        return super().__set__(obj, val)



class StringListProperty(Property):
    """string sequence property"""
    __TYPES = (list, tuple, set, frozenset, type({}.keys()), type({}.values()))

    def __init__(self, sorted=False, unique=False, fget=None, fset=None, doc=None):
        if not isinstance(sorted, bool):
            raise TypeError("sorted: bool expected")
        if not isinstance(unique, bool):
            raise TypeError("unique: bool expected")
        super().__init__(fget=fget, fset=fset, doc=doc)
        self.__sorted = sorted
        self.__unique = unique


    def __set__(self, obj, val):
        if not isinstance(val, StringListProperty.__TYPES):
            raise TypeError("value: iterable expected")
        res = list()
        for item in val:
            if not isinstance(item, str):
                raise TypeError("item: str expected")
            res.append(item)
        res = tuple(_OrderedDict.fromkeys(res) if self.__unique else res)
        val = tuple(sorted(res) if self.__sorted else res)
        return super().__set__(obj, val)



class PathListProperty(StringListProperty):
    """path sequence property"""
    __TYPES = (list, tuple, set, frozenset, type({}.keys()), type({}.values()))


    def __init__(self, sorted=False, unique=False, fget=None, fset=None, doc=None):
        super().__init__(sorted=sorted, unique=unique, fget=fget, fset=fset, doc=doc)


    def __set__(self, obj, val):
        if not isinstance(val, PathListProperty.__TYPES):
            raise TypeError("value: iterable expected")
        res = list()
        for item in val:
            if not isinstance(item, str):
                raise TypeError("item: str expected")
            if not item:
                raise TypeError("item: empty path")
            item = _os.path.normpath(item)
            res.append(item)
        return super().__set__(obj, res)

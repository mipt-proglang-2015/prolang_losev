#!/usr/bin/env python
# -*- coding: utf-8 -*-
from distutils.core import setup, Extension

setup(
    name='losev-aho-corasick-trie',
    version='0.1',
    description='Aho-Corasick trie implementation',
    ext_modules=[Extension('losev.trie', sources=['py-modules/trie/trie.cpp'])],
    packages=['losev'],
    package_dir={
		'losev': 'py-modules',
		'losev.trie': 'py-modules/trie'
	}
)

#!/usr/bin/env python
# -*- coding: utf-8 -*-
from losev.trie import Trie
from collections import Counter
import string
import re
import random
import time

class ElapsedTime:
    def __init__(self, label):
        self.label = label

    def __call__(self, function):
        def wrapped(*args, **kwargs):
            start_time = time.time()
            value = function(*args, **kwargs)
            print '{}:\telapsed {:0.4f} seconds'.format(
                self.label,
                time.time() - start_time
            )
            return value
        return wrapped

class CompareTest():

    @ElapsedTime('sample init')
    def generate_test_sample(self, letters='abcde', count=1000, strlen=10000):
        strings = []
        for i in xrange(count):
            strings.append(''.join(random.choice(letters) for i in xrange(strlen)))
        return strings

    @ElapsedTime('regex init')
    def init_regex(self, strings):
        return re.compile('|'.join(strings))

    @ElapsedTime('trie init')
    def init_trie(self, strings):
        return Trie(strings)

    @ElapsedTime('regex test')
    def test_regex(self, regex, search_in):
        for s in search_in:
            regex.findall(s)

    @ElapsedTime('trie test')
    def test_trie(self, trie, search_in):
        for s in search_in:
            trie.find_in_string(s)

    @ElapsedTime('test_match')
    def test_match(self, trie, regex, search_in):
        for s in search_in:
            a = set(i[0] for i in trie.find_in_string(s))
            b = set(regex.findall(s))
            assert a == b, '{} {}'.format(a, b)

    def __call__(self):
        search_for = ['aaba', 'baba', 'caba', 'daba']
        search_in = self.generate_test_sample()
        trie = self.init_trie(search_for)
        regex = self.init_regex(search_for)
        self.test_match(trie, regex, search_in)
        self.test_trie(trie, search_in)
        self.test_regex(regex, search_in)

if __name__ == '__main__':
    CompareTest()()

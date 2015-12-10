#!/usr/bin/env python
# -*- coding: utf-8 -*-
from losev.trie import Trie
from collections import Counter
import string


def trie_test_empty_samples():
    """
    Nothig is to be returned if we are searching for nothing
    """

    empty_trie = Trie([])
    assert [] == empty_trie.find_in_string(string.letters)
    assert [] == empty_trie.find_in_string(string.digits)
    assert [] == empty_trie.find_in_string(''.join(chr(i) for i in xrange(1, 256)))
    assert [] == empty_trie.find_in_string('a' * 1000000)


def trie_test_nested_search():
    """
    All nested sequences are returned - not only the
    longest match. Nested sequences' return order is
    following: for any two matches the one with the
    smallest end position is returned first. If two
    matches have the same end position - the longest
    is returned first. 
    """

    nested_trie = Trie(['a' * i for i in xrange(1, 11)])
    matches = nested_trie.find_in_string('a' * 10)   
    expected_matches = []
    for end_position in xrange(1, 11):
        for seq_len in xrange(end_position, 0, -1):
            expected_matches.append(('a' * seq_len, end_position - seq_len) )
    assert expected_matches == matches 
    
    
    same_suffix_strings = ['b' + 'a' * i for i in xrange(1, 11)]
    same_suffix_trie = Trie(same_suffix_strings)
    assert zip(same_suffix_strings, [0] * 10) == same_suffix_trie.find_in_string('b' + 'a' * 10)   

    same_prefix_strings = ['a' * i + 'b' for i in xrange(1, 11)]
    same_prefix_trie = Trie(same_prefix_strings)
    assert zip(reversed(same_prefix_strings), range(10)) == same_prefix_trie.find_in_string('a' * 10 + 'b')


def trie_test_empty_string():
    """
    In case of searching for an empy substring returns
    all occurances of the empty string in a search_in
    including one preceding any character of search_in
    and one after all characters of search_in string.
    If search string is empty itself the latter two are
    the same.
    """
    trie = Trie([''])
    assert [('', 0)] == trie.find_in_string('')
    assert [('', i) for i in xrange(11)] == trie.find_in_string('a' * 10)

def trie_test_word_search():
    """
    Testing that match positions are really match positions
    """

    with open('three_men_in_a_boat_ch1.txt') as search_in_flo:
        search_in = ''.join(search_in_flo.readlines()).lower()

    search_for = ['three', 'men', 'in', 'a', 'boat']
    for match, position in Trie(search_for).find_in_string(search_in):
        assert match in ['three', 'men', 'in', 'a', 'boat']
        assert match == search_in[position: position + len(match)]

    
if __name__ == '__main__':
    trie_test_empty_samples()
    trie_test_nested_search()
    trie_test_empty_string()
    trie_test_word_search()

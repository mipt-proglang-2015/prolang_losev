#ifndef __LOSEV_TRIE_CPP__
#define __LOSEV_TRIE_CPP__

#include "trie.hpp" 

TrieVertex::TrieVertex()
{
    match = NULL;
    next_match = UNDEFINED;
    link = UNDEFINED;
    parent = UNDEFINED;
    matches_count = 0;
    came_by = 0;
    for (int i = 0; i < 256; ++i)
    {
       next[i] = go[i] = UNDEFINED;
    }
}

TrieVertex::~TrieVertex()
{
    if (match != NULL)
    {
        Py_DECREF(match);
    }
}

static PyObject* trie_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds)
{
    PyObject* object = subtype->tp_alloc(subtype, 1);
    Trie* trie = reinterpret_cast<Trie*>(object);
    new (&trie->_trie) std::deque<TrieVertex>();
    return object;
}

static void trie_dealloc(Trie *self)
{
    self->_trie.~deque(); 
    self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}

static int trie_init(Trie *self, PyObject *args, PyObject *kwargs)
{
    PyObject *strings;
    if (!PyArg_ParseTuple(args, "O", &strings))
    {
        return -1; 
    }
    
    self->_trie.clear();
    self->_trie.push_back(TrieVertex());
    self->_trie.push_back(TrieVertex());
        
    self->_trie[1].link = 0;
    for (int i = 0; i < 256; ++i)
    {
        self->_trie[0].next[i] = 1;
    }

    Py_ssize_t strings_count = PyList_Size(strings);
    for (Py_ssize_t i = 0; i < strings_count; ++i)
    {
        PyObject *py_str = PyList_GetItem(strings, i);
        if (!PyString_Check(py_str))
        {
            return -1;
        }
        _trie_add_string(self, py_str); 
    }
        
    for (int vertex_index = 1; vertex_index < self->_trie.size(); ++vertex_index)
    {
        for (int char_index = 0; char_index < 256; ++char_index)
        {
            _trie_go(self, vertex_index, char_index);
        }
    }

    return 0;
}

static void _trie_add_string(Trie *self, PyObject *py_str)
{
    int vertex_index = 1;
    unsigned char * byte_seq = reinterpret_cast<unsigned char*>(PyString_AsString(py_str));

    for (; *byte_seq != 0; ++byte_seq)
    {
        if (self->_trie[vertex_index].next[*byte_seq] == UNDEFINED)
        {
            self->_trie[vertex_index].next[*byte_seq] = self->_trie.size();
            self->_trie.push_back(TrieVertex());
            self->_trie.back().parent  = vertex_index;
            self->_trie.back().came_by = *byte_seq;
        }
        vertex_index = self->_trie[vertex_index].next[*byte_seq];
    }

    if (self->_trie[vertex_index].match == NULL)
    {
        Py_INCREF(py_str);
        self->_trie[vertex_index].match = py_str;
        self->_trie[vertex_index].matches_count = 1;
    }
}
    
static size_t _trie_go(Trie *self, size_t vertex_index, unsigned char byte)
{
    if (self->_trie[vertex_index].go[byte] == UNDEFINED)
    {
        if (self->_trie[vertex_index].next[byte] == UNDEFINED)
        {
            self->_trie[vertex_index].go[byte] = 
                _trie_go(self, _trie_get_link(self, vertex_index), byte);  
        }
        else
        {
            self->_trie[vertex_index].go[byte] = self->_trie[vertex_index].next[byte];
        }
    }
    return self->_trie[vertex_index].go[byte];
}

static size_t _trie_get_link(Trie *self, size_t vertex_index)
{
    TrieVertex& vertex = self->_trie[vertex_index];
    if (vertex.link == UNDEFINED)
    {
        vertex.link = _trie_go(self, _trie_get_link(self, vertex.parent), vertex.came_by);
        const TrieVertex& suffix_vertex = self->_trie[vertex.link];

        if (suffix_vertex.match != NULL)
        {
            vertex.next_match = vertex.link;
        }
        else if (suffix_vertex.next_match != UNDEFINED)
        {
            vertex.next_match = suffix_vertex.next_match;
        }

        vertex.matches_count += suffix_vertex.matches_count;
    }
    return vertex.link;
}   

static PyObject* trie_find_in_string(Trie *self, PyObject* args, PyObject *kwargs)
{
    const unsigned char *byte_seq;
    if (!PyArg_ParseTuple(args, "s", &byte_seq))
    {
        return NULL;
    }

    PyObject *matches = PyList_New(0);

    int vertex_index = 1;
    
    _trie_collect_matches(self, vertex_index, 0, matches);
    for (Py_ssize_t byte_index = 1; *byte_seq != 0; ++byte_seq, ++byte_index)
    {
        vertex_index = _trie_go(self, vertex_index, *byte_seq); 
        if (self->_trie[vertex_index].matches_count)
        {
            _trie_collect_matches(self, vertex_index, byte_index, matches);
        }
    }

    return matches;
}

static void _trie_collect_matches(Trie *self, int vertex_index, Py_ssize_t byte_index, PyObject* matches)
{
    for (
        int match_vertex_index = vertex_index;
        match_vertex_index != UNDEFINED;
        match_vertex_index = self->_trie[match_vertex_index].next_match
    )
    {
        if (self->_trie[match_vertex_index].match == NULL)
        {
            continue;
        }
        PyObject* py_str = self->_trie[match_vertex_index].match;
        PyObject* start_index = PyInt_FromSsize_t(byte_index - PyString_Size(py_str));
        PyList_Append(matches, PyTuple_Pack(2, py_str, start_index));
        Py_DECREF(start_index);
    }
}

extern "C" void inittrie(void)
{
    PyObject* m;
    if (PyType_Ready(&TrieType) < 0)
    {
        return;
    }

    m = Py_InitModule3("trie", module_functions, "Aho corasick's sample search trie");

    Py_INCREF(&TrieType);
    PyModule_AddObject(m, "Trie", (PyObject *)&TrieType);
}

#endif

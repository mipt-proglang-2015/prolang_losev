#ifndef __LOSEV_TRIE_HPP__
#define __LOSEV_TRIE_HPP__

#include <Python.h>
#include "structmember.h"
#include <deque>

int UNDEFINED = -1;

struct TrieVertex
{
    TrieVertex();
    unsigned char came_by;
    int parent;
    int link;
    int next_match;
    int next[256];
    int go[256];
    PyObject* match; 
    Py_ssize_t matches_count;
    virtual ~TrieVertex();
};

struct Trie
{
    PyObject_HEAD
    std::deque<TrieVertex> _trie;
};

static PyObject* trie_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static int trie_init(Trie *self, PyObject* args, PyObject* kwargs);
static void trie_dealloc(Trie *self);
static void _trie_add_string(Trie *self, PyObject *py_str);
static void _trie_collect_matches(Trie *self, int vertex_index, Py_ssize_t byte_index, PyObject* matches);
static size_t _trie_go(Trie *self, size_t vertex_index, unsigned char byte);
static size_t _trie_get_link(Trie *self, size_t vertex_index);
static PyObject* trie_find_in_string(Trie *self, PyObject* args, PyObject *kwargs);

static PyMemberDef trie_members[] = {{NULL}};
static PyMethodDef trie_methods[] = {
    {
        "find_in_string", (PyCFunction)trie_find_in_string, METH_VARARGS,
        "Finds all ocurances of sample strings in the text. "
        "Works in O(#matches + len(string))\n"
        ":param text: text to search in."
        ":return: [(match_sample, match_start_offset)]"
    },
    {NULL}
};

static PyTypeObject TrieType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "Trie",                    /*tp_name*/
    sizeof(Trie),              /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)trie_dealloc,  /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "A data structure for fast search of sample strings in text", /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    trie_methods,              /* tp_methods */
    trie_members,              /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)trie_init,       /* tp_init */
    0,                         /* tp_alloc */
    trie_new,                  /* tp_new */
};


static PyMethodDef module_functions [] = {{NULL, NULL, 0, NULL}};
extern "C" void inittrie(void);

#endif


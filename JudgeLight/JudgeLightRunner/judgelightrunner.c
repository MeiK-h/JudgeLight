#include "jl_convert.h"
#include "jl_runner.h"

#include <Python.h>
#include <stdlib.h>

PyObject *run(PyObject *self, PyObject *args) {
#define ERROR(msg)                                                             \
    {                                                                          \
        if (PyErr_Occurred() == NULL) PyErr_SetString(PyExc_SystemError, msg); \
        stats = (PyObject *)NULL;                                              \
        goto END;                                                              \
    }

    PyObject *stats;
    if ((stats = PyDict_New()) == NULL) {
        ERROR("PyDict_New failure!");
    }

    struct RunnerConfig rconfig;
    struct RunnerStats rstats;

    /** 解析参数 */
    if (ParsePythonArgs(args, &rconfig) != 0) {
        ERROR("ParsePythonArgs failure!");
    }

    /** 执行代码 */
    if (RunIt(&rconfig, &rstats) != 0) {
        ERROR("RunIt failure!")
    }

    /** 生成返回结果 */
    if (GenPythonObject(&rstats, stats) != 0) {
        ERROR("GenPythonObject failure!");
    }

END:
    return stats;
}

/**
 * Extending Python with C or C++
 * Docs: https://docs.python.org/3/extending/extending.html
 * */
static PyMethodDef JudgeLightRunnerMethods[] = {
    {"run", run, METH_VARARGS, "JudgeLightRunner"}, {NULL, NULL, 0, NULL}};

static struct PyModuleDef judgelightrunnermodule = {
    PyModuleDef_HEAD_INIT, "JudgeLightRunner", NULL, -1,
    JudgeLightRunnerMethods};

PyMODINIT_FUNC PyInit_JudgeLightRunner(void) {
    PyObject *m;
    m = PyModule_Create(&judgelightrunnermodule);
    if (m == NULL) {
        return NULL;
    }
    return m;
}
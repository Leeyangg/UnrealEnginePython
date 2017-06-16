#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

#include "UePyFbx.h"

static PyObject *py_ue_fbx_object_get_name(ue_PyFbxObject *self, PyObject *args) {
	return PyUnicode_FromString(self->fbx_object->GetName());
}

static PyObject *py_ue_fbx_object_get_class_name(ue_PyFbxObject *self, PyObject *args) {
	return PyUnicode_FromString(self->fbx_object->GetClassId().GetName());
}

static PyObject *py_ue_fbx_object_get_member_count(ue_PyFbxObject *self, PyObject *args) {
	FbxCollection *fbx_collection = FbxCast<FbxCollection>(self->fbx_object);
	if (!fbx_collection)
		return PyErr_Format(PyExc_Exception, "unable to cast to FbxCollection");
	return PyLong_FromLong(fbx_collection->GetMemberCount());
}

static PyObject *py_ue_fbx_object_get_member(ue_PyFbxObject *self, PyObject *args) {
	int index;
	if (!PyArg_ParseTuple(args, "i", &index)) {
		return nullptr;
	}
	FbxCollection *fbx_collection = FbxCast<FbxCollection>(self->fbx_object);
	if (!fbx_collection)
		return PyErr_Format(PyExc_Exception, "unable to cast to FbxCollection");
	FbxObject *fbx_object = fbx_collection->GetMember(index);
	if (!fbx_object)
		return PyErr_Format(PyExc_Exception, "unable to find FbxObject with index %d", index);
	return py_ue_new_fbx_object(fbx_collection->GetMember(index));
}

static PyMethodDef ue_PyFbxObject_methods[] = {
	{ "get_member_count", (PyCFunction)py_ue_fbx_object_get_member_count, METH_VARARGS, "" },
	{ "get_member", (PyCFunction)py_ue_fbx_object_get_member, METH_VARARGS, "" },
	{ "get_name", (PyCFunction)py_ue_fbx_object_get_name, METH_VARARGS, "" },
	{ "get_class_name", (PyCFunction)py_ue_fbx_object_get_class_name, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PyFbxObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FbxObject", /* tp_name */
	sizeof(ue_PyFbxObject),    /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,   /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FbxObject", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFbxObject_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};

static int py_ue_fbx_object_init(ue_PyFbxObject *self, PyObject * args) {
	PyObject *py_object;
	char *name;
	if (!PyArg_ParseTuple(args, "Os", &py_object, &name)) {
		return -1;
	}

	ue_PyFbxManager *py_fbx_manager = py_ue_is_fbx_manager(py_object);
	if (!py_fbx_manager) {
		PyErr_SetString(PyExc_Exception, "argument is not a FbxManager");
		return -1;
	}

	self->fbx_object = FbxObject::Create(py_fbx_manager->fbx_manager, name);
	return 0;
}

void ue_python_init_fbx_object(PyObject *ue_module) {
	ue_PyFbxObjectType.tp_new = PyType_GenericNew;;
	ue_PyFbxObjectType.tp_init = (initproc)py_ue_fbx_object_init;
	if (PyType_Ready(&ue_PyFbxObjectType) < 0)
		return;

	Py_INCREF(&ue_PyFbxObjectType);
	PyModule_AddObject(ue_module, "FbxObject", (PyObject *)&ue_PyFbxObjectType);
}

PyObject *py_ue_new_fbx_object(FbxObject *fbx_object) {
	ue_PyFbxObject *ret = (ue_PyFbxObject *)PyObject_New(ue_PyFbxObject, &ue_PyFbxObjectType);
	ret->fbx_object = fbx_object;
	return (PyObject *)ret;
}

#endif
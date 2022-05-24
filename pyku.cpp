#include "ku.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;

PYBIND11_MODULE(pyku, m)
{
    m.def("vec_sqrt", vec_sqrt, "vec"_a);
    m.def("print_vec", print_vec, "vec"_a);
}
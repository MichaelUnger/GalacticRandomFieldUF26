#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "UF26Field.h"

namespace py = pybind11;

class PyUF26 {
public:
  PyUF26(const std::string& model) :
    field(model) {}

  double
  get_field(double x, double y, double z)
    const
  {
    return field({x, y, z});
  }

  py::array_t<double>
  get_field_many(py::array_t<double, py::array::c_style | py::array::forcecast> positions)
    const
  {
    if (positions.ndim() != 2 || positions.shape(1) != 3)
      throw std::runtime_error("Input must be Nx3 array of positions.");

    const ssize_t N = positions.shape(0);
    py::array_t<double> result(N);
    auto pos = positions.unchecked<2>();
    auto out = result.mutable_unchecked<1>();

    for (ssize_t i = 0; i < N; ++i)
      out(i) = field({pos(i,0), pos(i,1), pos(i,2)});

    return result;
  }

private:
  UF26Field field;
};

PYBIND11_MODULE(uf26, m) {
    m.doc() = "UF26 random Galactic magnetic field Python bindings";

    py::class_<PyUF26>(m, "UF26")
      .def(py::init<const std::string&>(), py::arg("model"))
      .def("get_field", &PyUF26::get_field,
           "rms strength of the random field [microgauss] at a single point [kpc]")
      .def("get_field_many", &PyUF26::get_field_many,
           "rms strength of the random field [microgauss] at many points (Nx3 numpy array, kpc)");
}

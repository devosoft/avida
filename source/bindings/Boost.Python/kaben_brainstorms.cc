#ifndef PRIMITIVE_HH
#include "primitive.hh"
#endif
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>


using namespace std;

//void ShuntSetup(cEnvironment &environment, cPopulationInterface &test_interface){
//  char *argv[]={"kaben_brainstorms"};
//  SetupAvida(1, argv, environment, test_interface);
//}
//
//struct kFu { int m_fu; };
//class kBar {
//public:
//  kBar(){}
//  int fu(kFu &fu){ return fu.m_fu; }
//};
//
//BOOST_PYTHON_MODULE(kaben_brainstorms)
//{
//    using namespace boost::python;
//    class_<cEnvironment>("cEnvironment");
//    class_<cPopulationInterface>("cPopulationInterface");
//    //class_<cConfig>("cConfig")
//    //  .def(";
//    
//    // Also add invite() as a regular function to the module.
//    def("ShuntSetup", ShuntSetup);
//
//    class_<kFu>("kFu");
//}


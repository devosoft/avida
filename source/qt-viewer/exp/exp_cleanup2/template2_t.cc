#ifndef TEMPLATE2_T_HH
#include "template2_t.hh"
#endif

int main(int argc, char *argv[]){
  std::cout << std::endl 
  << "----------------------------------------" << std::endl
  << "Testing Template2." << std::endl;
  nDemoTestsuite2::cTestSuite t(argc, argv);
  t.run_test();
  std::cout << text_test_interpreter().interpretation(t.last_result());
  return(t.last_result().success_count() != t.last_result().total_test_count());
}

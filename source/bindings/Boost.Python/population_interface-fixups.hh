#ifndef POPULATION_INTERFACE_FIXUPS_HH
#define POPULATION_INTERFACE_FIXUPS_HH

#ifndef POPULATION_INTERFACE_HH
#include "population_interface.hh"
#endif
#ifndef CALLBACK_UTIL_HH
#include "callback_util.hh"
#endif

inline void BuildTestPopInterface(cPopulationInterface &test_interface){
  test_interface.SetFun_NewHardware(&cCallbackUtil::CB_NewHardware);
  test_interface.SetFun_Recycle(&cCallbackUtil::CB_RecycleHardware);
  test_interface.SetFun_Divide(&cCallbackUtil::CB_TestDivide);
  test_interface.SetFun_GetInput(&cCallbackUtil::CB_GetInput);
  test_interface.SetFun_GetInputAt(&cCallbackUtil::CB_GetInputAt);
  test_interface.SetFun_GetResources(&cCallbackUtil::CB_GetResources);
  test_interface.SetFun_UpdateResources(&cCallbackUtil::CB_UpdateResources);
  test_interface.SetFun_ReceiveValue(&cCallbackUtil::CB_ReceiveValue);
}

#endif

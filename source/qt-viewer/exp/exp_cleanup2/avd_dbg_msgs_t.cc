#ifndef AVD_DBG_MSGS_HH
#include "avd_dbg_msgs.hh"
#endif

#ifndef AVD_MESSAGE_CLASS_HH
#include "avd_message_class.hh"
#endif
#ifndef AVD_MESSAGE_CLOSURE_M_HH
#include "avd_message_closure_m.hh"
#endif
#ifndef AVD_MESSAGE_DISPLAY_HH
#include "avd_message_display.hh"
#endif
#ifndef AVD_MESSAGE_TYPE_HH
#include "avd_message_type.hh"
#endif

#ifndef TEST_CASE_H
#include "third-party/yaktest/test_case.h"
#endif
#ifndef TEXT_TEST_INTERPRETER_H
#include "third-party/yaktest/text_test_interpreter.h"
#endif
#ifndef TEST_RESULT_H
#include "third-party/yaktest/test_result.h"
#endif

#include <iostream>

using namespace std;

static avdMessageDisplay s_info_msg_dsp(&cerr);
static avdMessageDisplay s_debug_msg_dsp(&cerr);
static avdMessageDisplay s_error_msg_dsp(&cerr);
static avdMessageDisplay s_plain_msg_dsp(&cerr);
static avdMessageDisplayBase *s_info_msg_hdl(&s_info_msg_dsp);
static avdMessageDisplayBase *s_debug_msg_hdl(&s_debug_msg_dsp);
static avdMessageDisplayBase *s_error_msg_hdl(&s_error_msg_dsp);
static avdMessageDisplayBase *s_plain_msg_hdl(&s_plain_msg_dsp);
static avdMessageClass s_info_msg_class("Info:", &s_info_msg_hdl, false, false, false);
static avdMessageClass s_debug_msg_class("Debug:", &s_debug_msg_hdl, false, false, false);
static avdMessageClass s_error_msg_class("Error:", &s_error_msg_hdl, false, true, false);
static avdMessageClass s_plain_msg_class("Plain:", &s_plain_msg_hdl, false, true, true);
static avdMessageType                            Info_MOCK_avdMsg("Mock", s_info_msg_class);
static avdMessageType                           Debug_MOCK_avdMsg("Mock", s_debug_msg_class);
static avdMessageType                           Error_MOCK_avdMsg("Mock", s_error_msg_class);
static avdMessageType                                 MOCK_avdMsg("Mock", s_plain_msg_class);
#define mockInfo     AVD_INFO_MSG(mockMessageClosure, MOCK)
#define mockDebug   AVD_DEBUG_MSG(mockMessageClosure, MOCK)
#define mockError   AVD_ERROR_MSG(mockMessageClosure, MOCK)
#define mockMessage AVD_PLAIN_MSG(mockMessageClosure, MOCK)

class cInfoTest : public test_case {
public: virtual void test() {
  mockInfo << "mockInfo.\n";
  test_is_true(true);
} };

class cDebugTest : public test_case {
public: virtual void test() {
  mockDebug << "mockDebug.\n";
  test_is_true(true);
} };

class cErrorTest : public test_case {
public: virtual void test() {
  mockError << "mockError.\n";
  test_is_true(true);
} };

class cMessageTest : public test_case {
public: virtual void test() {
  mockMessage << "mockMessage.\n";
  test_is_true(true);
} };

class cTestSuite : public test_case {
public: cTestSuite() : test_case() {
  adopt_test_case(new  cInfoTest); 
  adopt_test_case(new cDebugTest); 
  adopt_test_case(new cErrorTest); 
  adopt_test_case(new cMessageTest); 
} };

int main(int argc, char *argv[]){
  cout << endl 
  << "----------------------------------------" << endl
  << "Testing avdInfo, avdDebug, avdError, and Message." << endl;
  cTestSuite t;
  t.run_test();
  cout << text_test_interpreter().interpretation(t.last_result());
  return(t.last_result().success_count() != t.last_result().total_test_count());
}

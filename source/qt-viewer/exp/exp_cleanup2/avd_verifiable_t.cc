#ifndef AVD_VERIFIABLE_M_HH
#include "avd_verifiable_m.hh"
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
#include <string.h>

using namespace std;

namespace avd_verifiable_t {
  class test_ConstructorWithoutParent : public test_case {
  public: virtual void test() {
    // Should record name and reference to stubVerifiable::s_null.  also
    // tests getName() and nullParent().
    const char name[] = "verifiable";
    stubVerifiable verifiable(name);
    test_is_true(0 == strncmp(name, verifiable.getName(), sizeof(name)));
    test_is_true(verifiable.access_nullParent());
  } };
  
  class test_Constructor : public test_case {
  public: virtual void test() {
    // Parent's child list should initially be empty.
    const char parent_name[] = "parent";
    const char name[] = "child";
    stubVerifiable parent(parent_name);
    test_is_true(0 == parent.access_getChildList().GetSize());

    // Should record name and reference to parent.  Also tests
    // getName(), operator==(), and getParent().
    stubVerifiable child(name, parent);
    test_is_true(0 == strncmp(name, child.getName(), sizeof(name)));
    test_is_true(&parent == &child.access_getParent());

    // Should add self to parent list.  Also tests getChildList().
    test_is_true(parent.access_getChildList().Find(&child));
  } };
  
  
  class test_addVerifiable : public test_case {
  public: virtual void test() {
    // Should add a verifiable child to child list.  Also tests getChildList().
    stubVerifiable v1("v1");
    stubVerifiable v2("v2");
    v1.access_addVerifiable(v2);
    test_is_true(v1.access_getChildList().Find(&v2));
  } };

  class test_nullParent : public test_case {
  public: virtual void test() {
    // Should be a static class member who can substitute for no-parent
    // condition.  Also tests getParent() and operator==().
    stubVerifiable verifiable("verifiable");
    test_is_true(&stubVerifiable::s_null == &verifiable.access_getParent());
  } };
  
  class test_verify : public test_case {
  public: virtual void test() {
    // Should recursively verify children.
    stubVerifiable parent("parent");
    stubVerifiable child1("child1", parent);
    stubVerifiable child2("child2", parent);
    stubVerifiable child3("child3", child2);
    for(int i=0; i<=1; i++)
      for(int j=0; j<=1; j++)
        for(int k=0; k<=1; k++)
          for(int l=0; l<=1; l++){
            parent.shouldFail(i);
            child1.shouldFail(j);
            child2.shouldFail(k);
            child3.shouldFail(l);
            // parent.verify() should be true if and only if all of parent and children don't fail,
            // i.e., when i==j==k==l==0.
            test_is_true(((i+j+k+l)==0)==parent.verify());
          }
  } };

  class cTestSuite : public test_case {
  public: cTestSuite() : test_case() {
    adopt_test_case(new test_ConstructorWithoutParent); 
    adopt_test_case(new test_Constructor); 
    adopt_test_case(new test_addVerifiable); 
    adopt_test_case(new test_nullParent); 
    adopt_test_case(new test_verify); 
  } };
}

int main(int argc, char *argv[]){
  cout << endl 
  << "----------------------------------------" << endl
  << "Testing avdVerifiable via stubVerifiable." << endl;
  avd_verifiable_t::cTestSuite t; t.run_test();
  cout << text_test_interpreter().interpretation(t.last_result());
  return(t.last_result().success_count() != t.last_result().total_test_count());
}

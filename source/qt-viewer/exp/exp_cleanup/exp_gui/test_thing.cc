#ifndef TEST_THING_HH
#include "test_thing.hh"
#endif
#ifndef AVD_EXP_GUI_DBG_HH
#include "avd_exp_gui_dbg.hh"
#endif

void cTestThing::doAllTests(){
  Debug << "in.";
  doScopeGuardTest();
  Debug << "out.";
}

/*
ScopeGuard test and demonstration.
*/
#ifndef SCOPEGUARD_H_
#include "memory_mgt/ScopeGuard.h"
#endif
/* XXX disables the small-object allocator; it crashes under OSX10.2.6. */
#ifndef MAX_SMALL_OBJECT_SIZE
#define MAX_SMALL_OBJECT_SIZE 0
#endif
/* XXX -- kaben */
#ifndef FUNCTOR_INC_
#include "third-party/Loki/Functor.h"
#endif
class cScopeGuardTestObj {
  bool m_initialized;
  bool m_deinitialized;
public:
  cScopeGuardTestObj():m_initialized(false), m_deinitialized(false){}
  ~cScopeGuardTestObj(){ Debug("init'd: ")(m_initialized); Debug("deinit'd: ")(m_deinitialized); }
  void init(){ m_initialized = true; }
  void deinit(cTestThing *){ Debug << "in."; m_deinitialized = true; Debug << "out."; }
  void deinittoo(cTestThing &){ Debug << "in."; m_deinitialized = true; Debug << "out."; }
  void testy(){ Debug << "in"; }
};
void testy(){ Debug << "..."; }
void cTestThing::doScopeGuardTest(){
  Debug << "in.";
  { ScopeGuard foo = MakeGuard(testy);
    //foo.Dismiss();
  }
  { cScopeGuardTestObj fu; fu.init();
    ScopeGuard foo = MakeObjGuard(fu, &cScopeGuardTestObj::testy);
    //foo.Dismiss();
  }
  { cScopeGuardTestObj fu; fu.init();
    ScopeGuard foo = MakeObjGuard(fu, &cScopeGuardTestObj::deinit, this);
    //foo.Dismiss();
  }
  { cScopeGuardTestObj fu; fu.init();
    Loki::Functor<void> funct(&fu, &cScopeGuardTestObj::testy);
    ScopeGuard foo = MakeGuard(funct);
    //foo.Dismiss();
  }
  { cScopeGuardTestObj fu; fu.init();
    Loki::Functor<void, TYPELIST_1(cTestThing &)> funct(&fu, &cScopeGuardTestObj::deinittoo);
    Loki::Functor<void> bind_funct(BindFirst(funct, *this));
    ScopeGuard foo = MakeGuard(bind_funct);
    //foo.Dismiss();
  }
  Debug << "out.";
}

// arch-tag: implementation file for devel-experimentation testing class

#ifndef TESTING_INTERFACE_HH
#define TESTING_INTERFACE_HH

class cTestThing;
class cTestInterface 
{ public:
  virtual void testFunction() = 0;
  virtual void setThing(cTestThing *thing) = 0;
  virtual cTestThing *getThing() = 0;
};

#endif

// arch-tag: header file for devel-experimentation testing interface class

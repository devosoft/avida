//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HELP_ENTRY_HH
#define HELP_ENTRY_HH

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef STRING_UTIL_HH
#include "string_util.hh"
#endif

class cHelpType;
class cString; // aggregate
class cStringUtil; // accessed

class cHelpEntry {
protected:
  cString name;
public:
  cHelpEntry(const cString & _name) : name(_name) { ; }
  virtual ~cHelpEntry() { ; }

  const cString & GetName() const { return name; }
  virtual const cString & GetKeyword() const = 0;
  virtual const cString & GetDesc() const = 0;
  virtual cHelpType * GetType() const = 0;

  virtual bool IsAlias() const = 0;

  cString GetHTMLFilename() const
    { return cStringUtil::Stringf("help.%s.html", GetKeyword()()); }
};

#endif

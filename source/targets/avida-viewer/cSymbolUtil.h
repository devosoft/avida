//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          // 
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cSymbolUtil_h
#define cSymbolUtil_h

class cPopulationCell;

class cSymbolUtil {
public:
  static char GetBasicSymbol(    const cPopulationCell & cell );
  static char GetSpeciesSymbol(  const cPopulationCell & cell );
  static char GetModifiedSymbol( const cPopulationCell & cell );
  static char GetResourceSymbol( const cPopulationCell & cell );
  static char GetAgeSymbol(      const cPopulationCell & cell );
  static char GetBreedSymbol(    const cPopulationCell & cell );
  static char GetParasiteSymbol( const cPopulationCell & cell );
  static char GetMutSymbol(      const cPopulationCell & cell );
  static char GetThreadSymbol(const cPopulationCell & cell);
  static char GetLineageSymbol(  const cPopulationCell & cell );


#ifdef ENABLE_UNIT_TESTS
public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
#endif  
};

#endif

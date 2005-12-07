/*
 *  cGenesis.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cGenesis_h
#define cGenesis_h

#ifndef cInitFile_h
#include "cInitFile.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif

/**
 * A specialized initialization file class that is used for setting up
 * the way Avida should run.
 */

class cGenesis : public cInitFile {
private:
    cGenesis(const cGenesis &);
public:
    /** 
     * The empty constructor creates a clean object.
     **/
  cGenesis() { ; }
  
    /**
     * This constructor opens the given initialization file, reads it in,
     * removes all comments, and closes it again.
     **/
    cGenesis(const cString & filename);

    /**
     * Opens an initialization file, reads it in, removes all comments, and 
     * closes it again.
     **/
    int Open(cString _filename, std::ios::openmode mode=(std::ios::in));

    /**
     * Convenience function. Adds an integer valued entry to the file in
     * memory. Uses @ref cInitFile::AddLine().
     **/
    void AddInput(const cString & in_name, int in_value);
  
    /**
     * Convenience function. Adds a string valued entry to the file in
     * memory. Uses @ref cInitFile::AddLine().
     **/
    void AddInput(const cString & in_name, const cString & in_value);
    
    /**
     * Reads an entry of type int. In case the entry does not exist,
     * the value of base is returned.
     *
     * @param name The name of the entry.
     * @param base The default value.
     * @param warn Warn user if not set?
     **/
    int ReadInt (const cString & name, int base=0, bool warn=true) const;
  
    /**
     * Reads an entry of type float. In case the entry does not exist,
     * the value of base is returned.
     *
     * @param name The name of the entry.
     * @param base The default value.
     * @param warn Warn user if not set?
     **/
     double ReadFloat (const cString & name, float base=0.0, bool warn=true) const;

  void Read(cString & _var, const cString & _name, const cString & _def="") {
    _var = ReadString(_name, _def);
  }

  void Read(int & _var, const cString & _name, const cString & _def="0") {
    _var = ReadInt(_name, _def.AsInt());
  }

  void Read(double & _var, const cString & _name, const cString & _def="0.0") {
    _var = ReadFloat(_name, _def.AsDouble());
  }

  void Read(bool & _var, const cString & _name, const cString & _def="0.0") {
    _var = ReadInt(_name, _def.AsInt()) != 0;
  }
};

#endif


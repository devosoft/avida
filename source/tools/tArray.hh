//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

/*
   tArray.hh
   Array Templates

   Constructor:
     tArray( int size )
     tArray( const tArray & rhs )

   Interface:
     tArray & operator= ( const tArray & rhs )

     bool Good () const
       array is defined and ready to take data

     unsigned int GetSize () const
       returns the size of the array

     const T& ElementAt ( const int index ) const
           T& ElementAt ( const int index )
     const T& operator[] ( const int index ) const
           T& operator[] ( const int index )
       return the element at index in the array
*/

#ifndef TARRAY_HH
#define TARRAY_HH

#include <assert.h>

#ifndef NULL
#define NULL 0
#endif

/**
 * This class provides an array template.
 **/ 

template <class T> class tArray {

protected:
  // Internal Variables
  T * data;  // Data Elements
  int size;  // Number of Elements

public:
  void ResizeClear(const int in_size){
    size = in_size;
    assert(size >= 0);  // Invalid size specified for array intialization
    if (data != NULL) delete [] data;  // remove old data if exists
    if (size > 0) {
      data = new T[size];   // Allocate block for data
      assert(data != NULL); // Memory allocation error: Out of Memory?
    }
    else data = NULL;
  }

public:
  // Constructor
  explicit tArray(const int _size=0) : data(NULL) {
    ResizeClear(_size);
  }

  // Assingment Operator
  tArray & operator= (const tArray & rhs) {
    if (size != rhs.GetSize())  ResizeClear(rhs.GetSize());
    for(int i = 0; i < size; i++) data[i] = rhs[i];
    return *this;
  }

  // Copy constructor
  explicit tArray(const tArray & rhs) : data(NULL), size(0) {
    this->operator=(rhs);
  }

  // Destructor
  virtual ~tArray() {
    if (data != NULL) delete [] data;
  }

  // Interface Methods ///////////////////////////////////////////////////////

  bool Good() const { return (data != NULL); }

//    bool OK() const {
//      assert(size >= 0);
//      return true;
//    }

  int GetSize() const { return size; }

  void Resize(int new_size) {
    assert(new_size >= 0);

   // If we're already at the size we want, don't bother doing anything.
    if (size == new_size) return;

    // If new size is 0, clean up and go!
    if (new_size == 0) {
      delete [] data;
      data = NULL;
      size = 0;
      return;
    }

    // If new size > 0
    T * new_data = new T[new_size];
    assert(new_data != NULL); // Memory Allocation Error: Out of Memory?

    // Copy over old data...
    for (int i = 0; i < size && i < new_size; i++) {
      new_data[i] = data[i];
    }
    if (data != NULL) delete [] data;  // remove old data if exists
    data = new_data;

    size = new_size;
  }


  void Resize(int new_size, const T & empty_value) {
    assert(new_size >= 0);
    int old_size = size;
    Resize(new_size);
    if( new_size > old_size ){
      for (int i = old_size; i < new_size; i++) {
	data[i] = empty_value;
      }
    }
  }


  T & ElementAt(const int index){
    // check range
    assert(index >= 0);    // Lower Bounds Error
    assert(index < size);  // Upper Bounds Error
    return data[index];    // in range, so return element
  }

  const T & ElementAt(const int index) const {
    // check range
    assert(index >= 0);    // Lower Bounds Error
    assert(index < size);  // Upper Bounds Error
    return data[index];    // in range, so return element
  }

        T & operator[](const int index)       { return ElementAt(index); }
  const T & operator[](const int index) const { return ElementAt(index); }


  void Push(const T & value) {
    Resize(size+1);
    data[size-1] = value;
  }

  void SetAll(const T & value){
    for( int i=0; i < size; ++i ){
      data[i] = value;
    }
  }

};

#endif // TARRAY_HH

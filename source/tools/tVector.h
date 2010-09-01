/*
 *  tVector.h
 *  Avida
 *
 *  Called "tVector.hh" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef tVector_h
#define tVector_h

/**
 * This class provides a vector template.
 **/ 

template <class T> class tVector {
private:
  T *		_data;
  int		_size;
  int		_capacity;
  int		_capIncrement;

public:
  tVector(void): _size(0), _capacity(1), _capIncrement(-1) {
    _data = new T[_capacity];
    #ifdef EXCEPTION_HANDLING
    if(_data == NULL) throw InsufficientMemory();
    #endif
  }

  tVector(int cap, int incr = -1): _size(0), _capacity(cap),
				  _capIncrement(incr){
    _data = new T[_capacity];
    #ifdef EXCEPTION_HANDLING
    if(_data == NULL) throw InsufficientMemory();
    #endif
  }


  tVector(const tVector &v): _size(v._size), _capacity(v._capacity),
			   _capIncrement(v._capIncrement)
  {
    _data = new T[_capacity];
    #ifdef EXCEPTION_HANDLING
    if(_data == NULL) throw InsufficientMemory();
    #endif
    for(int i = 0; i < _size; i++) {
      _data[i] = v._data[i];
    }
  }

  ~tVector() {
    if(_data) delete [] _data;
  }

public:
  int Size(void) const {
    return _size;
  }

  int Capacity(void) const {
    return _capacity;
  }

  void Add(T data) {
    if(Size() + 1 > Capacity()) { // if we have to allocate new space, do so
      T* newdata;
      if(_capIncrement == -1) {
	newdata = new T[Capacity() * 2];
	#ifdef EXCEPTION_HANDLING
	if(newdata == NULL) throw InsufficientMemory();
	#endif
	_capacity = Capacity() * 2;
      }
      else {
	newdata = new T[Capacity() + _capIncrement];
	#ifdef EXCEPTION_HANDLING
	if(newdata == NULL) throw InsufficientMemory();
	#endif
	_capacity = Capacity() + _capIncrement;
      }

      for(int i = 0; i < Size(); i++) {
	newdata[i] = _data[i];
      }
      newdata[_size++] = data;
      delete [] _data;
      _data = newdata;
      return;
    }
    else { // no need to allocate new data
      _data[_size++] = data;
      return;
    }
  }

  void Remove(T data) {
    int i, j;
    for (i = 0; _data[i] != data && i < Size(); i++) ;
    if (i < Size()) {
      T *newdata = new T[Capacity()];
      for(j = 0; j < i; j++) {
	newdata[j] = _data[j];
      }
      for(; j < Size() - 1; j++) {
	newdata[j] = _data[j + 1];
      }
      delete [] _data;
      _data = newdata;
      --_size;
      return;
    }
    return;
  }

  void RemoveAt(int idx) {
    int i;
    if(idx >= 0 && idx < Size()) {
      T *newdata = new T[Capacity()];
      for(i = 0; i < idx; i++) {
	newdata[i] = _data[i];
      }
      for(; i < Size() - 1; i++) {
	newdata[i] = _data[i + 1];
      }
      delete [] _data;
      _data = newdata;
      --_size;
      return;
    }
    return;
  }

  void Clear(void) {
    delete [] _data;
    _data = new T[Capacity()];
    _size = 0;
  }

public:
  T& operator[](int idx) {
    assert( idx >= 0);
    assert( idx < Size());
    if(idx >= 0 && idx < Size()) { // it is in range
      return _data[idx];
    }
    #ifdef EXCEPTION_HANDLING
    throw IndexOutofRangeError(idx);
    #endif
    return _data[0];
  }

  T operator[](int idx) const {
    assert( idx >= 0);
    assert( idx < Size());
    if(idx >= 0 && idx < Size()) {
      return _data[idx];
    }
    #ifdef EXCEPTION_HANDLING
    throw IndexOutofRangeError(idx);
    #endif
    return _data[0];
  }
};

#endif

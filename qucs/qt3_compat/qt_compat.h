/***************************************************************************
                                 container.h
                                -----------
    begin                : 2018
    copyright            : QUCS Developers
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// provide a compatibility layer between different qt releases for qucs.
//
// collect all QT_VERSION dependent code here.

#ifndef QUCS_CONTAINER_H
#define QUCS_CONTAINER_H

#include <QList>
#include <QDebug>

// TODO: trace.h
#include <iostream>
#define incomplete() ( \
    std::cerr << "@@#\n@@@\nincomplete:" \
              << __FILE__ << ":" << __LINE__ << ":" << __func__ << "\n" )
#ifdef DO_TRACE
#define untested() ( std::cerr <<  "@@#\n@@@:"<< __FILE__ << ":"<< __LINE__ \
          <<":" << __func__ << "\n" )
#else
#define untested()
#endif
#define itested()

// implement Q3Ptrlist for use in old code.
// just don't use it in new code.
template <class T>
class Q3PtrList {
public:
	typedef QList<T*> container_type;
	typedef typename container_type::const_iterator const_iterator;
	typedef typename container_type::iterator iterator;
public:
	Q3PtrList() : _autodelete(false) { };
	Q3PtrList(const T &val) : _autodelete(false) { untested();
		localList(val);
		cur = localList.end();
	};
	~Q3PtrList() {
		if(_autodelete){
			for(auto x : localList){
				delete x;
			}
		}else{
		}
	};
	T* at(unsigned int i) {
		if (i < (unsigned int)localList.count()) {
			cur = localList.begin()+i;
			return *cur;
		} else {
			cur = localList.end();
			return nullptr;
		}
	};
	int at() { untested();
		incomplete();
		return -1;
	};
	void setAutoDelete(bool b) {
		_autodelete = b;
	};
	T* first() {   // pointer to first element
		cur = localList.begin();
		if (localList.count() == 0)
			return nullptr;
		return *cur;
	};
	T* last() {    // pointer to last element
		if (localList.count()) { untested();
			cur = localList.end()-1; // yikes. no rbegin. use std::list?!
			return *cur;
		} else { untested();
			return nullptr;
		}
	};
	void append(T *t) {
		localList.append(t);
	};
	int contains(T *) { untested();
		incomplete();
		return -1;
	};
	int findNext(T *) { untested();
		incomplete();
		return -1;
	};
	int findNextRef(T *) { untested();
		incomplete();
		return -1;
	};
	bool isEmpty() { untested();
		incomplete();
		return false;
	};
	int findPrev() { untested();
		incomplete();
		return -1;
	};
	bool replace(int, T*) { untested();
		incomplete();
		return true;
	};
	int containsRef(T *t) { untested();
		int n = 0;
		for (int i = 0; i < localList.count(); i++){ untested();
			if (t == localList[i]) { untested();
				n++;
			}
		}
		return n;
	};
	int findRef(T *t) {
		auto i=0;
		for (cur=localList.begin(); cur!=localList.end(); ++cur){
			if (t == *cur){
				return i;
			}
			++i;
		}
		return -1;
	};
	bool removeRef(T *t) { untested();
		int nr = findRef(t);
		if (nr >= 0) { untested();
			remove(nr);
			return true;
		}
		return false;
	};
	void prepend(T *t) { untested();
		localList.insert(0, t);
	};
	T* take(uint index) { untested();
		return localList.takeAt(index);
	};
	T* take() { untested();
		if (cur != localList.end()){ untested();
			auto t=*cur;
			auto newcur=cur;
			++newcur;
			localList.erase(cur);
			cur=newcur;
			return t;
		} else { untested();
			return nullptr;
		}
	};
	T* getFirst() { itested();
		return localList.first();
	};
	T* getLast() { itested();
		return localList.last();
	};
	T* next() {    // get pointer to next element, correct the current
		if (cur == localList.end()){ untested();
			return nullptr;
		}else{
			cur++;
			if (cur==localList.end()){
				return nullptr;
			}else{
				return *cur;
			}
		}
	};
	T* prev() {    // get pointer to prev element, correct the current
		if (cur == localList.end()) { untested();
			return nullptr;
		}else if (cur == localList.begin()){ untested();
			cur=localList.end();
			return nullptr;
		}else{ untested();
			cur--;
			return *cur;
		}
	};
	T* current() { // get pointer to current element
		if (cur==localList.end()){ untested();
			return nullptr;
		}else{ untested();
			return *cur;
		}
	};
	unsigned int count() {
		return (unsigned int)localList.count();
	};
	void remove(int i) { //
		if (i >= 0 && i < localList.count()) { untested();
			localList.removeAt(i);
		}
	};
	void remove() { // remove the current element
		if (cur!=localList.end()){ untested();
			auto next=cur;
			++next;
			if(_autodelete){ untested();
				delete *cur;
			}
			localList.erase(cur);
		}else{ untested();
		}
	};
	void removeLast() { untested();
		int c = localList.count() -1;
		if (c >= 0) { untested();
			localList.removeAt(c);
		}
	};
	void removeFirst() { untested();
		int c = localList.count() -1;
		if (c >= 0) { untested();
			localList.removeAt(0);
		}
	};
	void insert(unsigned int i, T *t) { untested();
		localList.insert(i, t);
		cur=localList.begin()+i;
	};
	int find(T *t) { untested();
		for (int i = 0; i < localList.count(); i++){ untested();
			if (t == localList[i]) { untested();
				return i;
			}
		}
		return -1;
	};
	const_iterator begin() const{ untested();
		return localList.begin();
	}
	const_iterator end() const{ untested();
		return localList.end();
	}
	iterator begin(){
		return localList.begin();
	}
	iterator end(){
		return localList.end();
	}
	void clear(){
		return localList.clear();
	}

private:
	QList<T*> localList; // why not std::list!?
	bool _autodelete;
	typename QList<T*>::iterator cur;
};

#endif

//vim:ts=8:sw=4:et

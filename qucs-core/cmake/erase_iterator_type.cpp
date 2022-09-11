
/*
 * Check compiler implementation of std::vector::erase iterator type
 *   - type =  [const_iterator | iterator]
 *
 * Adapted from:
 * http://www.gnu.org/software/autoconf-archive/ax_cxx_erase_iterator_type.html
 *
 * Copyright (c) 2015 Bastien ROUCARIES
 *
 * Copying and distribution of this file, with or without modification, are
 * permitted in any medium without royalty provided the copyright notice
 * and this notice are preserved. This file is offered as-is, without any
 * warranty.
 *
 */

#include <vector>

typedef std::vector<int> intvector;

struct myvector {
  myvector() { v.push_back(1); }
  typedef intvector::const_iterator const_iterator;
  typedef intvector::iterator iterator;
  iterator erase (const_iterator position) { return v.erase(position); };
  iterator erase (const_iterator first, const_iterator last) { return v.erase(first,last); };
  intvector v;
};

int main()
{
   myvector v;
	 v.erase(v.v.begin(),v.v.end());
	 return 1;
}


/***************************************************************************
 *   Copyright (C) 2021 by Blinkov Yu. A.                                  *
 *   BlinkovUA@info.sgu.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef GINV_BASIS_H
#define GINV_BASIS_H

#include "util/timer.h"
#include "poly_int.h"

#include "config.h"

namespace GInv {


class Basis {
  Allocator            mAllocator;
  GCListWrapPolyInt    mQ;
  GCListWrapPolyInt    mT;
  JanetPolyInt         mJanet;

  Timer                mTimer;
  int                  mReduction;

public:
  Basis():
      mAllocator(),
      mQ(),
      mT(),
      mJanet(&mAllocator, -1),
      mTimer(),
      mReduction(0) {
  }
  ~Basis() {
    for(GCListWrapPolyInt::ConstIterator j(mQ.begin()); j; ++j)
      delete j.data();
    for(GCListWrapPolyInt::ConstIterator j(mT.begin()); j; ++j)
      delete j.data();
  }

  bool compare(const PolyInt& a) {
    return (!mQ or !mT) ||
      (mQ && mQ.head()->poly().compare(a)) ||
      (mT && mT.head()->poly().compare(a));
  }
  void push(const PolyInt& a) {
    assert(compare(a));
    mQ.push(new WrapPolyInt(a));
  }

  void build();

  GCListWrapPolyInt::ConstIterator begin() const {
    return mT.begin();
  }
  const JanetPolyInt& janet() const { return mJanet; }

  const Timer& timer() const { return mTimer; }
  int reduction() const { return mReduction; }
};

}

#endif // GINV_BASIS_H

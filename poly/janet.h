/***************************************************************************
 *   Copyright (C) 2017 by Blinkov Yu. A.                                  *
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

#ifndef GINV_POLY_JANET_H
#define GINV_POLY_JANET_H

#include "util/allocator.h"
#include "util/list.h"
#include "monom.h"

#include "config.h"

#ifdef GINV_POLY_GRAPHVIZ
  #include <sstream>
  #include <graphviz/gvc.h>
#endif // GINV_POLY_GRAPHVIZ

namespace GInv {

class Wrap {
  Allocator*  mAllocator;
  Monom       mLm;
  Monom       mAnsector;
  bool*       mNM;
  bool*       mBuild;

public:
  Wrap()=delete;
  Wrap(const Wrap& a)=delete;
  Wrap(const Monom& m, Allocator* allocator):
      mAllocator(allocator),
      mLm(m, allocator),
      mAnsector(m, allocator),
      mNM(new(allocator) bool[m.size()]),
      mBuild(new(allocator) bool[m.size()]) {
    for(int i=0; i < mLm.size(); i++) {
      mNM[i] = false;
      mBuild[i] = false;
    }
  }
  Wrap(const Wrap* ansector, Monom::Variable var, Allocator* allocator):
      mAllocator(allocator),
      mLm(ansector->mAnsector, var, allocator),
      mAnsector(ansector->mAnsector, allocator),
      mNM(new(allocator) bool[ansector->mLm.size()]),
      mBuild(new(allocator) bool[ansector->mLm.size()]) {
    assert(ansector->mNM[var] && !ansector->mBuild[var]);
    ansector->mBuild[var] = true;
    for(int i=0; i < mLm.size(); i++) {
      mNM[i] = false;
      mBuild[i] = false;
    }
  }
  ~Wrap() {
    mAllocator->dealloc(mNM, mLm.size());
    mAllocator->dealloc(mBuild, mLm.size());
  }

  const Monom& lm() const { return mLm; }
  const Monom& ansector() const { return mAnsector; }
  bool isGB() const { return mAnsector.degree() == mLm.degree(); }
  
  void setNM(int var) const {
    assert(0 <= var && var < mLm.size());
    mNM[var] = true;
  }
  bool NM(int var) const {
    assert(0 <= var && var < mLm.size());
    return mNM[var];
  }

  bool build(int var) const {
    assert(0 <= var && var < mLm.size());
    return mBuild[var];
  }
//   bool setBuild(int var) const {
//     assert(0 <= var && var < mLm.size());
//     mBuild[var] = true;
//   }
};


class Janet {
  friend class ConstIterator;
  friend class Iterator;

  struct Node {
    Monom::Variable mDeg;
    Wrap*           mWrap;
    Node*           mNextDeg;
    Node*           mNextVar;

    Node(Monom::Variable deg): mDeg(deg), mWrap(nullptr),
      mNextDeg(nullptr), mNextVar(nullptr) {}
    ~Node() {}
  };

  typedef Node* Link;

public:
  class ConstIterator {
    Link i;

  public:
    ConstIterator(Link n): i(n) {}

    void deg() { assert(i); i = i->mNextDeg; }
    void var() { assert(i); i = i->mNextVar; }
    operator bool() const { return (i != nullptr); }
    ConstIterator nextDeg() const { assert(i); return i->mNextDeg; }
    ConstIterator nextVar() const { assert(i); return i->mNextVar; }
    Wrap* wrap() const { assert(i); return i->mWrap; }
    Monom::Variable degree() const { assert(i); return i->mDeg; }

    void setNM(Monom::Variable var);

    bool assertValid();
  };

private:
  class Iterator {
    Link *i;

  public:
    Iterator(Link &n): i(&n) {}

    void deg() { assert(*i); i = &(*i)->mNextDeg; }
    void var() { assert(*i); i = &(*i)->mNextVar; }
    operator bool() const { return (*i != nullptr); }
    ConstIterator nextDeg() const {
      assert(*i); return (*i)->mNextDeg;
    }
    ConstIterator nextVar() const {
      assert(*i); return (*i)->mNextVar;
    }
    operator ConstIterator() const { return *i; }
    Wrap*& wrap() const { assert(*i); return (*i)->mWrap; }
    Monom::Variable degree() const { assert(i); return (*i)->mDeg; }
    void build(int d, Monom::Variable var, Wrap *wrap, Allocator* allocator);
    void del(Allocator* allocator) {
      Link tmp = *i;
      assert(tmp);
      *i = tmp->mNextDeg;
      allocator->destroy(tmp);
    }
    void clear(Allocator* allocator);
  };

  Allocator*  mAllocator;
  int         mPos;
  Link        mRoot;

#ifdef GINV_POLY_GRAPHVIZ
  static Agnode_t* draw(Agraph_t *g, Link j, Monom::Variable var);
#endif // GINV_POLY_GRAPHVIZ

public:
  explicit Janet(Allocator* allocator):
      mAllocator(allocator),
      mPos(0),
      mRoot(nullptr) {
  }
  ~Janet() {
    if (mRoot) {
      Janet::Iterator j(mRoot);
      j.clear(mAllocator);
    }
    assert(mRoot == nullptr);
  }

  Janet::ConstIterator begin() const { return mRoot; }
  
  Wrap* find(const Monom &m) const;
//   void insert(Wrap *wrap);
  void insert(Wrap *wrap);
//   void update(Wrap *wrap);

#ifdef GINV_POLY_GRAPHVIZ
  void draw(const char* format, const char* filename) const {
    GVC_t *gvc=gvContext();
    Agraph_t *g=agopen((char*)"Janet",  Agdirected, (Agdisc_t*)nullptr);
    if (mRoot)
      draw(g, mRoot, 0);
    gvLayout(gvc, g, (char*)"dot");
    gvRenderFilename(gvc, g, format, filename);
    gvFreeLayout(gvc, g);
    agclose(g);
  }
#endif // GINV_POLY_GRAPHVIZ
};

}

#endif // GINV_POLY_JANET_H

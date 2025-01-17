/***************************************************************************
 *   Copyright (C) 2019 by Blinkov Yu. A.                                  *
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

#include "wrap.h"

namespace GInv {

void Wrap::swap(Wrap& a) {
  assert(this != &a);

  auto tmp1=mAllocator;
  mAllocator = a.mAllocator;
  a.mAllocator = tmp1;

  mLm.swap(a.mLm);
  mAnsector.swap(a.mAnsector);

  auto tmp2=mNM;
  mNM = a.mNM;
  a.mNM = tmp2;

  auto tmp3 = mNMd;
  mNMd = a.mNMd;
  a.mNMd = tmp3;

  auto tmp4 = mBuild;
  mBuild = a.mBuild;
  a.mBuild = tmp4;
}

std::ostream& operator<<(std::ostream& out, const Wrap &a) {
  out << a.lm() << a.ansector() << '[';
  if (a.NM(0))
    out << '*';
  else
    out << ' ';
  for(int i=1; i < a.lm().size(); i++)
    if (a.NM(i))
      out << '*';
    else
      out << ' ';
  out << ']';
  return out;
}

}

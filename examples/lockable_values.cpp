/*
  MIT License
  Copyright (c) 2021-2025 Yafiyogi
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "yy_lockable_value.h"
#include <iostream>
#include <memory>
#include <mutex>

using namespace std;
using namespace yafiyogi;

using LockableInt = yy_util::lockable_value<int, std::mutex>;
using LockInt =
  yy_util::lock_type<LockableInt, std::unique_lock<LockableInt::mutex_type>>;

struct Thing
{
    Thing(int v) :
      value(v)
    {
    }

    int value;
};

using ThingPtr = std::shared_ptr<Thing>;

using LockableThingPtr = yy_util::lockable_value<ThingPtr, std::mutex>;
using LockThingPtr =
  yy_util::lock_type<LockableThingPtr,
                     std::unique_lock<LockableThingPtr::mutex_type>>;

int main()
{
  LockableInt val;

  LockInt::set(val, 14);

  LockInt::visit(val, [](auto && v) {
    cout << v << endl;
  });

  LockableThingPtr thing;

  LockThingPtr::visit(thing, [](auto && v) {
    cout << v.value << endl;
  });

  LockThingPtr::set(thing, std::make_shared<Thing>(12));

  LockThingPtr::visit(thing, [](auto && v) {
    cout << v.value << endl;
  });
  return 0;
}

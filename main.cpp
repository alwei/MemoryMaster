#include "MemoryMaster.h"
#include <boost/progress.hpp>

#include <list>

// #define new new(__FILE__, __LINE__)

class Base {
  int a, b;
  float c, d;
};

class Test : public Base {
public:
  Test() {}
  ~Test() {}
};

class TestAppendMemoryPool :
  public Base,
  public MemoryPool::AppendMemoryPool<Test, 100000>
{
public:
  TestAppendMemoryPool() {}
  ~TestAppendMemoryPool() {}
};

int main() {
  {
    boost::progress_timer t;
    Test* ptr;
    for (int j = 0; j < 1000; ++j) {
      for (int i = 0; i < 100000; ++i) {
        ptr = new Test;
        delete ptr;
      }
    }
  }
  {
    boost::progress_timer t;
    TestAppendMemoryPool::init();
    TestAppendMemoryPool* ptr;
    for (int j = 0; j < 1000; ++j) {
      for (int i = 0; i < 100000; ++i) {
        ptr = new TestAppendMemoryPool;
        delete ptr;
      }
    }
  }
  {
    boost::progress_timer t;
    MemoryPool::MakeMemoryPool<Test, 100000> mp;
    Test* ptr;
    for (int j = 0; j < 1000; ++j) {
      for (int i = 0; i < 100000; ++i) {
        ptr = mp.poolNew();
        mp.poolDelete(ptr);
      }
    }
  }
}


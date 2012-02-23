#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_

#include <memory>
#include <cstdlib>

namespace MemoryPool {

static const int DEFAULT_EXPAND_SIZE = 10000;

template <class T, std::size_t Size = DEFAULT_EXPAND_SIZE>
class MemoryPool {
public:
  MemoryPool() : next(0) {
    expandFreeList(Size);
  }

  ~MemoryPool() {
    MemoryPool<T, Size>* nextPtr = next;
    for (nextPtr = next; nextPtr != 0; nextPtr = next) {
      next = next->next;
      std::free(nextPtr);
    }
  }

  void* alloc(std::size_t) {
    if (!next) expandFreeList(Size);
    MemoryPool<T, Size>* head = next;
    next = head->next;
    return head;
  }

  void free(void* freePtr) {
    MemoryPool<T, Size>* head = static_cast<MemoryPool<T, Size>*>(freePtr);
    head->next = next;
    next = head;
  }

private:
  void expandFreeList(int howMany) {
    std::size_t size =
      (sizeof(T) > sizeof(MemoryPool<T, Size>*)) ? sizeof(T) : sizeof(MemoryPool<T, Size>*);

    MemoryPool<T, Size>* runner = reinterpret_cast<MemoryPool<T, Size>*>(std::malloc(size));
    next = runner;

    for (int i = 0; i < howMany; ++i) {
      runner->next = reinterpret_cast<MemoryPool<T, Size>*>(std::malloc(size));
      runner = runner->next;
    }
    runner->next = 0;
  }

private:
  MemoryPool<T, Size>* next;
};

template <class T, std::size_t Size = DEFAULT_EXPAND_SIZE>
class AppendMemoryPool {
public:
  void* operator new(std::size_t size) { return pool->alloc(size); }
  void operator delete(void* deletePtr, std::size_t) { pool->free(deletePtr); }
  void* operator new[](std::size_t size) { return pool->alloc(size); }
  void operator delete[](void* deletePtr, std::size_t) { pool->free(deletePtr); }

  static void init() { pool.reset(new MemoryPool<T, Size>); }

private:
  static std::auto_ptr<MemoryPool<T, Size> > pool;
};

template <class T, std::size_t Size = DEFAULT_EXPAND_SIZE>
class MakeMemoryPool {
public:
  MakeMemoryPool() : pool(new MemoryPool<T, Size>) {}
  ~MakeMemoryPool() {}

  T* poolNew() {
    T* allocPtr = static_cast<T*>(pool->alloc(sizeof(T)));
    return static_cast<T*>(new(allocPtr) T());
  }

  void poolDelete(T* deletePtr) {
    deletePtr->~T();
    pool->free(deletePtr);
  }

private:
  std::auto_ptr<MemoryPool<T, Size> > pool;
};

struct DataPool16 { char data[16]; };
struct DataPool32 { char data[32]; };
struct DataPool64 { char data[64]; };
struct DataPool128 { char data[128]; };
struct DataPool256 { char data[256]; };

class GeneralMemoryPool {
public:
  GeneralMemoryPool() {}
  ~GeneralMemoryPool() {}

  void* poolAlloc(std::size_t size) {
    void* allocPtr = NULL;
         if (size < 16) { allocPtr = mp16.alloc(size); }
    else if (size < 32) { allocPtr = mp32.alloc(size); }
    else if (size < 64) { allocPtr = mp64.alloc(size); }
    else if (size < 128) { allocPtr = mp128.alloc(size); }
    else if (size < 256) { allocPtr = mp256.alloc(size); }
    return allocPtr;
  }

  void poolFree(void* freePtr, std::size_t size) {
         if (size < 16) { mp16.free(freePtr); }
    else if (size < 32) { mp32.free(freePtr); }
    else if (size < 64) { mp64.free(freePtr); }
    else if (size < 128) { mp128.free(freePtr); }
    else if (size < 256) { mp256.free(freePtr); }
  }

private:
  MemoryPool<DataPool16> mp16;
  MemoryPool<DataPool32> mp32;
  MemoryPool<DataPool64> mp64;
  MemoryPool<DataPool128> mp128;
  MemoryPool<DataPool256> mp256;
};

template <class T, std::size_t Size>
std::auto_ptr<MemoryPool<T, Size> > AppendMemoryPool<T, Size>::pool;

} /* end of namespace MemoryPool */

#endif

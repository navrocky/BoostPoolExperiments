#include <QDebug>

#include <boost/pool/pool.hpp>
#include <memory>

using namespace std;

class Test
{
public:
    Test()
    {
        qDebug() << "Test";
    }

    ~Test()
    {
        qDebug() << "~Test";
    }

private:
//    int val_;
//    int val1_;
//    int val2_;
//    int val3_;
};

struct TestAllocator
{
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  static char * malloc(const size_type bytes)
  {
      qDebug() << "malloc(" << bytes << ")";
      return new (std::nothrow) char[bytes];
  }

  static void free(char * const block)
  {
      qDebug() << "delete(" << (void*)block << ")";
      delete [] block;
  }
};

class TestPool : public boost::pool<TestAllocator>
{
public:
    TestPool(std::size_t sz)
        : boost::pool<TestAllocator>(sz)
    {
        qDebug() << "Pool";
    }
    ~TestPool()
    {
        qDebug() << "~Pool";
    }
};

typedef std::shared_ptr<TestPool> TestPoolPtr;
typedef std::shared_ptr<Test> TestPtr;

template <typename T, typename PoolPtr>
class PoolAllocator
{
public:
    typedef T value_type;

    PoolAllocator(const PoolPtr& pool)
        : pool_(pool)
    {}

    template <class U>
    PoolAllocator(const PoolAllocator<U, PoolPtr>& src)
        : pool_(src.pool())
    {}

    T* allocate(std::size_t n)
    {
        qDebug() << typeid(T).name() <<  sizeof(T) << n;
        return static_cast<T*>(pool_->malloc());
    }

    void deallocate(T* p, std::size_t n)
    {
        qDebug() << typeid(T).name() <<  sizeof(T) << n;
        pool_->free(p);
    }

    const PoolPtr& pool() const { return pool_; }

private:
    PoolPtr pool_;
};

template <typename T, typename PoolPtr>
std::shared_ptr<T> createObjectFromPool(const PoolPtr& pool)
{
    PoolAllocator<void, PoolPtr> alloc(pool);
    return std::allocate_shared<T>(alloc);
}

template <typename T>
class CalcAllocator
{
public:
    typedef T value_type;

    CalcAllocator(int& size)
        : size_(size)
    {}

    template <class U>
    CalcAllocator(const CalcAllocator<U>& src)
        : size_(src.size_)
    {}

    T* allocate(std::size_t)
    {
        size_ = sizeof(T);
        return nullptr;
    }

    void deallocate(T*, std::size_t) {}

    int& size_;
};

template <typename T>
int calcHolderSize()
{
    int size;
    CalcAllocator<T> alloc(size);
    std::allocate_shared<T>(alloc);
    return size - sizeof(CalcAllocator<T>) + sizeof(PoolAllocator<T, std::shared_ptr<int> >);
}

int main(int , char **)
{
    TestPtr test;
    {
        TestPoolPtr pool = std::make_shared<TestPool>(calcHolderSize<Test>());

        test = createObjectFromPool<Test>(pool);
        std::list<TestPtr> list;
        for (int i = 0; i < 10; i++)
        {
            list.push_back(createObjectFromPool<Test>(pool));
        }
    }

    qDebug() << "1";

    return 0;
}

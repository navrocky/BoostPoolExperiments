#include <QDebug>

#include <boost/pool/pool.hpp>
#include <memory>

using namespace std;

class Test
{
public:
    Test()
    {
        str = "asdadasdas";
        qDebug() << "Test";
    }

    ~Test()
    {
        qDebug() << "~Test";
    }

private:
    char val_[71];
    std::string str;
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
        auto v = static_cast<T*>(pool_->malloc());
        return v;
    }

    void deallocate(T* p, std::size_t n)
    {
        qDebug() << typeid(T).name() <<  sizeof(T) << n;
        pool_->free(p);
    }

    void destroy(T* p)
    {
        p->~T();
    }

    template<typename U>
    struct rebind
    {
        typedef PoolAllocator<U, PoolPtr> other;
    };

    const PoolPtr& pool() const { return pool_; }

private:
    PoolPtr pool_;
};

template <typename T, typename PoolPtr>
std::shared_ptr<T> createObjectFromPool(const PoolPtr& pool)
{
    PoolAllocator<T, PoolPtr> alloc(pool);
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

    template<typename U>
    struct rebind
    {
        typedef CalcAllocator<U> other;
    };

    T* allocate(std::size_t)
    {
        size_ = sizeof(T);
        return nullptr;
    }

    void destroy(T*) {}
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
        int sz = calcHolderSize<Test>();

        qDebug() << sz << sizeof(Test);

        TestPoolPtr pool = std::make_shared<TestPool>(sz);

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

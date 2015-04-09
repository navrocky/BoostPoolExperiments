#include <QDebug>

#include <boost/pool/object_pool.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace std;

class Test : public boost::enable_shared_from_this<Test>
{
public:
    Test()
    {
//        qDebug() << "Test";
    }

    ~Test()
    {
//        qDebug() << "~Test";
    }

private:
    int val_;
};

struct TestAllocator
{
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  static char * malloc(const size_type bytes)
  {
//      qDebug() << "malloc(" << bytes << ")";
      return new (std::nothrow) char[bytes];
  }

  static void free(char * const block)
  {
//      qDebug() << "delete(" << (void*)block << ")";
      delete [] block;
  }
};

class TestPool : public boost::object_pool<Test, TestAllocator>
{
public:
    TestPool()
    {
//        qDebug() << "Pool";
    }
    ~TestPool()
    {
//        qDebug() << "~Pool";
    }
};

typedef boost::shared_ptr<TestPool> TestPoolPtr;
typedef boost::shared_ptr<Test> TestPtr;

template <typename PoolPtr>
struct Destructor
{
    Destructor(const PoolPtr& pool) : pool(pool) {}
    PoolPtr pool;
    void operator()(typename PoolPtr::element_type::element_type* t)
    {
        pool->destroy(t);
//        qDebug() << "destroy" << t;
    }
};

template <typename PoolPtr>
boost::shared_ptr<typename PoolPtr::element_type::element_type> createObjectFromPool(const PoolPtr& pool)
{
    return typename boost::shared_ptr<typename PoolPtr::element_type::element_type>(pool->construct(), Destructor<PoolPtr>(pool));
}

int main(int , char **)
{
    TestPtr test;
    {
        TestPoolPtr pool = boost::make_shared<TestPool>();

        test = createObjectFromPool(pool);
        std::list<TestPtr> list;
        for (int i = 0; i < 10; i++)
        {
            list.push_back(createObjectFromPool(pool));
        }
    }

//    qDebug() << "1";

    return 0;
}

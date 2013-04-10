#include <iostream>
#include "behavior.h"
#include <locale>

using namespace std;
using namespace dan_ai::bts;

class test_entity
{
public:
    test_entity(void) : _count(0) {}

    int     count(void) const { return _count; }
    int &   count(void) { return _count; }

private:
    int _count;
};

class range_condition : public condition < test_entity >
{
public:
    range_condition(int min, int max) : _min(min), _max(max)
    {
    }

    bool in_range(int value) const
    {
        return (value >= min() && value <= max());
    }

    bool evaluate(const test_entity & entity) const
    {
        return in_range(entity.count());
    }

public:
    int min(void) const { return _min; }
    int max(void) const { return _max; }

private:
    int _min;
    int _max;
};

class task : public behavior_base < test_entity >
{
public:
    task(void) : behavior_base()
    {
        wcout << name() << " construct" << endl;
    }
    
    task(const wstring & node_name) : behavior_base(node_name)
    {
        wcout << name() << " construct" << endl;
    }
    
    virtual ~task(void)
    {
        wcout << name() << " de-construct" << endl;
    }
    
    virtual	behavior_result update(test_entity & entity)
    {
        entity.count()++;

        wcout << name() << L" -> count: " << entity.count() << endl;

        if ( entity.count() % 17 == 0 )
        {
            return bh_success;
        }
        
        return bh_failure;
    }
};

class test_selector : public selector < test_entity >
{
public:
    test_selector(void) : selector < test_entity > (), _tick(0)
    {
        wcout << name() << " construct" << endl;
    }

    test_selector(const wstring & node_name) : selector < test_entity > (node_name), _tick(0)
    {
        wcout << name() << " construct" << endl;
    }

    virtual ~test_selector(void)
    {
        wcout << name() << " de-construct" << endl;
    }

    virtual behavior_result update(test_entity & entity)
    {
        wcout << name() << L" tick " << tick() << endl;
        behavior_result result = selector<test_entity>::update(entity);
        tick()++;
        return result;
    }
public:
    int     tick(void) const { return _tick; }
    int &   tick(void) { return _tick; }

private:
    int _tick;
};

int main(int argc, char * argv[])
{
    std::locale::global(std::locale("zh_CN.utf8"));

    test_selector root(L"root");
    root.add_child(new task(L"t1"));
    root.add_child(new task(L"t2"));
    root.add_child(new task(L"t3"));

    test_entity t;
    while(root.execute(t) != bh_success);
    return 0;
}

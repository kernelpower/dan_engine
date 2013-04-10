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
        wcout << L"range condtion (" << this->min() << ", " << this->max() << ") construct." << endl;
    }

    virtual ~range_condition(void)
    {
        wcout << L"range condtion (" << this->min() << ", " << this->max() << ") de-construct." << endl;
    }

    bool in_range(int value) const
    {
        return (value >= min() && value <= max());
    }

    bool evaluate(const test_entity & entity) const
    {
        wcout << L"range condition evaluate " << entity.count() << L" in (";
        wcout << this->min() << L", " << this->max() << L")?" << endl;
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
        wcout << name() << L" update, count = " << entity.count() << endl;
        return bh_success;
    }
};

class test_selector : public selector < test_entity >
{
public:
    test_selector(void) : selector < test_entity > ()
    {
        wcout << name() << " construct" << endl;
    }

    test_selector(const wstring & node_name) : selector < test_entity > (node_name)
    {
        wcout << name() << " construct" << endl;
    }

    virtual ~test_selector(void)
    {
        wcout << name() << " de-construct" << endl;
    }

    virtual behavior_result update(test_entity & entity)
    {
        behavior_result result = selector<test_entity>::update(entity);
        return result;
    }
};

int main(int argc, char * argv[])
{
    std::locale::global(std::locale("zh_CN.utf8"));

    test_selector root(L"root");
    typedef shared_ptr < behavior_base < test_entity > > safe_behavior_ptr;

    safe_behavior_ptr & t1 =
        root.add_child(new task(L"t1"));
    
    t1->set_condition(new range_condition(2, 7));

    root.add_child(new task(L"t2"))->set_condition(new range_condition(8, 11));
    root.add_child(new task(L"t3"))->set_condition(new range_condition(12, 35));

    test_entity t;
    wstring result;

    while(t.count() < 37)
    {
        bts_util::get_result_desc(result, root.execute(t));

        wcout << L"tick: " << t.count() << L", result = " << result << endl;
        t.count()++;
    }
    return 0;
}

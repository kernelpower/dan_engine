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

class range_task : public behavior_base < test_entity >
{
public:
    range_task(void) : behavior_base()
    {
        wcout << name() << " construct" << endl;
    }
    
    range_task(const wstring & node_name) : behavior_base(node_name)
    {
        wcout << name() << " construct" << endl;
    }
    
    virtual ~range_task(void)
    {
        wcout << name() << " de-construct" << endl;
    }
    
    void update(test_entity & entity)
    {
        wcout << name() << L" update, count = " << entity.count() << endl;
        notify_success();
    }
};

class mod_task : public behavior_base < test_entity >
{
public:
    mod_task(void) : behavior_base()
    {
        wcout << name() << " construct" << endl;
    }
    
    mod_task(const wstring & node_name) : behavior_base(node_name)
    {
        wcout << name() << " construct" << endl;
    }
    
    virtual ~mod_task(void)
    {
        wcout << name() << " de-construct" << endl;
    }
    
    void update(test_entity & entity)
    {
        wcout << "mod get " << entity.count() << endl;
        if (entity.count() % 5 == 0)
        {
            wcout << "mod success with" << entity.count() << endl;
            notify_success();
        }
        else if (entity.count() % 5 > 2)
        {
            notify_failure(L"too large!");
        }
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

    void update(test_entity & entity)
    {
        selector<test_entity>::update(entity);
    }
};

class test_sequence : public sequence < test_entity >
{
public:
    test_sequence(void) : sequence < test_entity > ()
    {
        wcout << name() << " construct" << endl;
    }

    test_sequence(const wstring & node_name) : sequence < test_entity > (node_name)
    {
        wcout << name() << " construct" << endl;
    }

    virtual ~test_sequence(void)
    {
        wcout << name() << " de-construct" << endl;
    }

    void update(test_entity & entity)
    {
        sequence<test_entity>::update(entity);
    }
};

int main(int argc, char * argv[])
{
    std::locale::global(std::locale("zh_CN.utf8"));

    test_selector root(L"root");

    root.add_child(new range_task(L"t1"))->set_condition(new range_condition(2, 7));
    root.add_child(new mod_task(L"mod"));
    root.add_child(new range_task(L"t2"))->set_condition(new range_condition(8, 12));
    root.add_child(new range_task(L"t3"))->set_condition(new range_condition(13, 15));

    test_entity t;
    wstring result;

    while(t.count() < 19)
    {
        bts_util::get_result_desc(result, root.execute(t));

        wcout << L"tick: " << t.count() << L", result = " << result;
        //wcout << L", desc = " << root.description();
        wcout << endl << endl;
        t.count()++;
    }
    return 0;
}

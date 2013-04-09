#include <iostream>
#include "behavior.h"
#include <locale>

using namespace std;
using namespace dan_engine::ai::bts;

class test_entity
{
public:
    test_entity(void) {}
};

class task : public behavior_base < test_entity >
{
public:
    task(void) : behavior_base()
    {
        wcout << L"task 构造: " << name() << endl;
    }
    
    task(const wstring & node_name) : behavior_base(node_name)
    {
        wcout << L"task 构造: " << name() << endl;
    }
    
    virtual ~task(void)
    {
        wcout << L"task 析构: " << name() << endl;
    }

    
    virtual	behavior_result update(test_entity & entity)
    {
       return bh_success;
    }
};

int main(int argc, char * argv[])
{
    std::locale::global(std::locale("zh_CN.utf8"));

    task t1(L"t1");
    cout << "hello linux!" << endl;
    return 0;
}

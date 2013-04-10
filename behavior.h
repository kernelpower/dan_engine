#pragma once
#include <vector>
#include <tr1/memory>
#include <string>

using namespace std;
using namespace std::tr1;

namespace dan_ai
{
    namespace bts
    {
        enum behavior_result
        {
            // 就绪
            bh_ready,

            // 行为符合条件，并成功执行。
            bh_success,

            // 行为不符合条件。
            bh_failure,

            // 行为正在运行中
            bh_running,
        };

        template < typename ENTITY_TYPE >
            class condition
            {
                public:
                    condition(void) {}
                    virtual ~condition(void) {}

                    virtual bool evaluate(const ENTITY_TYPE & entity) = 0;
            };

        // 基本行为（抽象类）
        template < typename ENTITY_TYPE >
            class behavior_base
            {
                public:
                    behavior_base(void)
                        : _name(L"")
                          , _precondition(static_cast< condition < ENTITY_TYPE > * > (0))
                {
                }

                    behavior_base(const wstring & node_name)
                        : _name(node_name)
                          , _precondition(static_cast< condition < ENTITY_TYPE > * > (0))
                {
                }

                    virtual ~behavior_base(void) {}

                    virtual	behavior_result execute(ENTITY_TYPE & entity)
                    {
                        if (!evaluate(entity))
                            return bh_failure;

                        return update(entity);
                    }

                protected:
                    virtual	behavior_result update(ENTITY_TYPE & entity) = 0;

                    virtual bool evaluate(ENTITY_TYPE & entity) const
                    {
                        if (precondition() == 0)
                            return true;

                        return (*precondition()).evaluate(entity);
                    }

                public:
                    typedef shared_ptr < condition < ENTITY_TYPE > > condition_ptr;
                    const condition_ptr & precondition(void) const { return _precondition; }
                    const wstring & name(void) const { return _name; }

                private:
                    condition_ptr           _precondition;
                    wstring					_name;
            };

        // 组合结点
        template < typename ENTITY_TYPE >
        class composite : public behavior_base < ENTITY_TYPE >
        {
        public:
            typedef shared_ptr < behavior_base < ENTITY_TYPE > > safe_behavior_ptr;
            typedef typename std::vector < safe_behavior_ptr > children_vector;

            composite(void) : behavior_base <ENTITY_TYPE> () {}
            composite(const wstring & node_name) : behavior_base < ENTITY_TYPE > (node_name) {}
            virtual ~composite(void) {}

            virtual behavior_result update(ENTITY_TYPE & entity) = 0;

            void add_child(behavior_base < ENTITY_TYPE > * child_node)
            {
                children().push_back(safe_behavior_ptr(child_node));
            }

        protected:
            children_vector & children(void) { return _children; }

        private:
            children_vector _children;
        };

        // 选择结点
        template < typename ENTITY_TYPE >
        class selector : public composite < ENTITY_TYPE >
        {
        public:
            selector(void) : composite < ENTITY_TYPE > () {}
            selector(const wstring & node_name) : composite < ENTITY_TYPE > (node_name) {}
            virtual ~selector(void) {}

            typedef shared_ptr < behavior_base < ENTITY_TYPE > > safe_behavior_ptr;
            typedef typename std::vector < safe_behavior_ptr > children_vector;

            virtual behavior_result update(ENTITY_TYPE & entity)
            {
                if ( !evaluate(entity) )
                    return bh_failure;

                children_vector v1;
                typedef typename children_vector::iterator children_iterator;

                for ( children_iterator iter = children().begin();
                    iter != children().end();
                    iter++)
                {
                    
                }
                

                return bh_failure;
            }
        };
    }
}

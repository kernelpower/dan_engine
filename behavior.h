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
        enum behavior_state
        {
            // 就绪
            bh_ready = 0,

            // 行为符合条件，并成功执行。
            bh_success = 1,

            // 行为不符合条件。
            bh_failure = -1,

            // 行为正在运行中
            bh_running = 2,
        };

        class bts_util
        {
        public:
            static void get_result_desc(wstring & desc_text, behavior_state result)
            {
                switch(result)
                {
                case bh_ready:
                    desc_text = L"ready";
                    break;
                
                case bh_success:
                    desc_text = L"success";
                    break;
                
                case bh_failure:
                    desc_text = L"failure";
                    break;
                
                case bh_running:
                    desc_text = L"running";
                    break;
                
                default:
                    desc_text = L"unknown";
                }
            }
        };

        template < typename ENTITY_TYPE >
            class condition
            {
                public:
                    condition(void) {}
                    virtual ~condition(void) {}

                    virtual bool evaluate(const ENTITY_TYPE & entity) const = 0;
            };

        // 基本行为（抽象类）
        template < typename ENTITY_TYPE >
        class behavior_base
        {
        public:
            behavior_base(void)
                : _name(L"")
                , _state(bh_ready)
                , _precondition(static_cast< condition < ENTITY_TYPE > * > (0))
            {
            }

            behavior_base(const wstring & node_name)
                : _name(node_name)
                , _state(bh_ready)
                , _precondition(static_cast< condition < ENTITY_TYPE > * > (0))
            {
            }

            virtual ~behavior_base(void) {}

            virtual	behavior_state execute(ENTITY_TYPE & entity)
            {
                if(state() == bh_success || state() == bh_failure)
                {
                    _description = L"";
                    _state = bh_ready;
                }

                if (!evaluate(entity))
                {
                    notify_failure(L"evaluate failed!");
                    return state();
                }

                if (state() == bh_ready)
                {
                    init(entity);
                    _state = bh_running;
                }

                update(entity);

                if(state() == bh_success || state() == bh_failure)
                {
                    done(entity);
                }

                return state();
            }

            void set_condition(condition < ENTITY_TYPE > * new_condition)
            {
                _precondition = condition_ptr(new_condition);
            }
        
        protected:
            virtual	void init(ENTITY_TYPE & entity) {}
            virtual void update(ENTITY_TYPE & entity) = 0;
            virtual	void done(ENTITY_TYPE & entity) {}

            virtual bool evaluate(ENTITY_TYPE & entity) const
            {
                if (precondition() == 0)
                    return true;

                return (*precondition()).evaluate(entity);
            }

            void notify_success(const wstring & message)
            {
                _description = message;
                _state = bh_success;
            }

            void notify_success(void)
            {
                notify_success(L"");
            }

            void notify_failure(const wstring & message)
            {
                _description = message;
                _state = bh_failure;
            }

        public:
            typedef shared_ptr < condition < ENTITY_TYPE > > condition_ptr;
            const condition_ptr & precondition(void) const { return _precondition; }
            const wstring & name(void) const { return _name; }
            behavior_state state(void) const { return _state; }
            const wstring & description(void) const { return _description; }

        protected:
            condition_ptr           _precondition;
            wstring					_name;
            behavior_state          _state;
            wstring                 _description;
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

            virtual void update(ENTITY_TYPE & entity) = 0;

            safe_behavior_ptr & add_child(behavior_base < ENTITY_TYPE > * child_node)
            {
                children().push_back(safe_behavior_ptr(child_node));
                return children().back();
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

            typedef typename composite<ENTITY_TYPE>::children_vector::iterator children_iterator;

            using composite<ENTITY_TYPE>::children;
            using composite<ENTITY_TYPE>::notify_failure;
            using composite<ENTITY_TYPE>::notify_success;

            virtual void update(ENTITY_TYPE & entity)
            {
                for ( children_iterator iter = children().begin();
                    iter != children().end();
                    iter++)
                {
                    behavior_state result = (*iter)->execute(entity);
                    if (result == bh_success)
                    {
                        notify_success((*iter)->description());
                        return;
                    }
                    else if (result == bh_running)
                    {
                        return;
                    }
                }

                notify_failure(L"all of children were failed!");
            }
       };

        // 序列结点
        template < typename ENTITY_TYPE >
        class sequence : public composite < ENTITY_TYPE >
        {
        public:
            sequence(void) : composite < ENTITY_TYPE > () {}
            sequence(const wstring & node_name) : composite < ENTITY_TYPE > (node_name) {}
            virtual ~sequence(void) {}

            typedef typename composite<ENTITY_TYPE>::safe_behavior_ptr children_iterator;
            using composite<ENTITY_TYPE>::children;
            using composite<ENTITY_TYPE>::notify_failure;
            using composite<ENTITY_TYPE>::notify_success;

            virtual	void init(ENTITY_TYPE & entity)
            {
                _current_index = 0;
            }

            virtual void update(ENTITY_TYPE & entity)
            {
                if (children().size() == 0)
                {
                    notify_failure(L"none of children!");
                    return;
                }
                
                children_iterator & child = children()[_current_index];
                behavior_state child_state = child->execute(entity);
                
                if (child_state != bh_running)
                    _current_index++;

                if (_current_index >= children().size())
                {
                    notify_success();
                }                
            }

        private:
            unsigned int _current_index;
       };
    }
}

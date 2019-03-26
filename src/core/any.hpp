// Ben Jones 2015

#pragma once

#include <typeinfo>
#include <string>
#include <memory>
#include <cstdint>
#include <utility>

namespace simple_any {

    class any
    {

      public:

        any()
        : _held(nullptr)
        {
        }

        template <typename T>
        any(T const & t)
        : _held(std::move(std::make_shared<typed_holder<T>>(t)))
        {
        }

        std::type_info const &type() const
        {
            return _held->type();
        }

        class holder
        {
          public:
            virtual std::type_info const &type() const = 0;
        };

        template <typename T>
        class typed_holder : public holder
        {
          public:
            typed_holder(T const & t) : _data(t){}
            T _data;
            std::type_info const &type() const { return typeid(T); }
            virtual ~typed_holder() = default;
        };

        // types for which there are no specializations
        std::shared_ptr<holder> _held;
    };

    template<typename T, typename V>
    T any_cast(V && a)
    {
        if(a._held != nullptr) {
            if (typeid(T) == a.type()) {
                return static_cast<any::typed_holder<T>*>(a._held.get())->_data;
            }
        }
        throw std::runtime_error("Bad any cast");
    }

}

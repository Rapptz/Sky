// The zlib/libpng License

// Copyright (c) 2014 Danny Y., Rapptz

// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.

// Permission is granted to anyone to use this software for any purpose, including
// commercial applications, and to alter it and redistribute it freely, subject to
// the following restrictions:

// 1. The origin of this software must not be misrepresented; you must not claim
// that you wrote the original software. If you use this software in a product,
// an acknowledgment in the product documentation would be appreciated but is
// not required.

// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.

// 3. This notice may not be removed or altered from any source distribution.

#ifndef SKY_ENTITY_HPP
#define SKY_ENTITY_HPP

#include "Base.hpp"
#include <map>
#include <typeindex>
#include <typeinfo>
#include <memory>
#include <stdexcept>

namespace sky {
struct Entity {
private:
    std::map<std::type_index, std::unique_ptr<Component>> components;

    template<typename T>
    bool has_impl() const {
        is_component<T>();
        return components.count(std::type_index(typeid(T)));
    }

    template<typename T, typename... Args>
    typename std::enable_if<(sizeof...(Args) >= 1), bool>::type has_impl() const {
        return has_impl<T>() && has_impl<Args...>();
    }

    template<typename T>
    void remove_impl() {
        is_component<T>();
        components.erase(std::type_index(typeid(T)));
    }

    template<typename T, typename... Args>
    typename std::enable_if<(sizeof...(Args) >= 1)>::type remove_impl() {
        remove_impl<T>();
        remove_impl<Args...>();
    }

public:
    Entity() = default;

    template<typename T>
    T* get() const {
        is_component<T>();
        auto it = components.find(std::type_index(typeid(T)));

        if(it == components.end()) {
            return nullptr;
        }

        return dynamic_cast<T*>(it->second.get());
    }

    template<typename T, typename... Args>
    void emplace(Args&&... args) {
        is_component<T>();
        std::unique_ptr<component> c{ new T(std::forward<Args>(args)...) };
        components.emplace(typeid(T), std::move(c));
    }

    template<typename... Args>
    bool has() const {
        return has_impl<Args...>();
    }

    template<typename... Args>
    void remove() {
        return remove_impl<Args...>();
    }
};
} // sky

#endif // SKY_ENTITY_HPP

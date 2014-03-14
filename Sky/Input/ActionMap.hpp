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

#ifndef SKY_ACTIONMAP_HPP
#define SKY_ACTIONMAP_HPP

#include "Action.hpp"
#include <functional>
#include <unordered_map>

namespace sky {
template<typename Key>
class ActionMap {
private:
    std::unordered_map<Key, Action> actions;
    std::unordered_map<Key, std::function<void(void)>> callbacks;
    EventBuffer buffer;
public:
    ActionMap() = default;
    ActionMap(std::initializer_list<std::pair<Key, Action>> l): actions(l.begin(), l.end()) {}

    void pushEvent(sf::Event event) {
        buffer.insert(std::move(event));
    }

    void clearEvents() {
        buffer.clear();
    }

    template<typename... Args>
    void emplace(Key&& key, Args&&... args) {
        actions.emplace(std::piecewise_construct,
                        std::forward_as_tuple(key),
                        std::forward_as_tuple(args...));
    }

    template<typename Callback>
    void connect(Key&& key, Callback&& callback) {
        if(actions.count(key)) {
            callbacks.emplace(key, std::forward<Callback>(callback));
        }
    }

    void disconnect(Key&& key) {
        callbacks.erase(std::forward<Key>(key));
    }

    template<typename Callback, typename... Args>
    void bind(Key&& key, Callback&& callback, Args&&... args) {
        actions.emplace(std::piecewise_construct,
                        std::make_tuple(key),
                        std::forward_as_tuple(args...));
        callbacks.emplace(key, std::forward<Callback>(callback));
    }

    template<typename Window>
    void update(Window& window) {
        buffer.clear();
        sf::Event event;

        while(window.pollEvent(event)) {
            buffer.insert(event);
        }

        invoke();
    }

    auto operator[](Key&& key) -> decltype(actions[key]) {
        return actions[key];
    }

    void invoke() {
        for(auto&& pair : callbacks) {
            if(isActive(pair.first)) {
                auto&& func = pair.second;

                if(func) {
                    func();
                }
            }
        }
    }

    void invoke(Key&& key) {
        auto it = callbacks.find(std::forward<Key>(key));

        if(it != callbacks.end()) {
            auto&& func = *it;

            if(func) {
                func();
            }
        }
    }

    void clear() {
        actions.clear();
        buffer.clear();
    }

    void erase(const Key& key) {
        actions.erase(key);
    }

    bool isActive(const Key& key) const {
        auto it = actions.find(key);

        if(it == actions.end()) {
            return false;
        }

        return it->second.isActive(buffer);
    }
};
} // sky

#endif // SKY_ACTIONMAP_HPP

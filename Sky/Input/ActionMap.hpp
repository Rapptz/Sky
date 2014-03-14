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
#include <unordered_map>

namespace sky {
template<typename Key>
class ActionMap {
private:
    std::unordered_map<Key, Action> actions;
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

    template<typename Window>
    void update(Window& window) {
        buffer.clear();
        sf::Event event;
        while(window.pollEvent(event)) {
            buffer.insert(event);
        }
    }

    auto operator[](Key&& key) -> decltype(actions[key]) {
        return actions[key];
    }

    void clear() {
        actions.clear();
        buffer.clear();
    }

    void erase(Key&& key) {
        actions.erase(std::forward<Key>(key));
    }

    bool isActive(Key&& key) {
        auto it = actions.find(std::forward<Key>(key));
        if(it == actions.end()) {
            return false;
        }
        return it->second.isActive(buffer);
    }
};
} // sky

#endif // SKY_ACTIONMAP_HPP

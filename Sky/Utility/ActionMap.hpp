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
#include <tuple>
#include <set>

namespace sky {
namespace action_map_detail {
struct EventCompare {
    constexpr bool operator()(const sf::Event::EventType& lhs, const sf::Event::EventType& rhs) const noexcept {
        return static_cast<int>(lhs) < static_cast<int>(rhs);
    }
};
} // action_map_detail

template<typename Key>
class ActionMap {
private:
    std::unordered_map<Key, Action> actions;
    std::set<sf::Event::EventType, action_map_detail::EventCompare> eventBuffer;

    struct Visitor {
        bool operator()(const Action& action, decltype((eventBuffer)) e) const noexcept {
            if(action.mouse != nullptr) {
                return sf::Mouse::isButtonPressed(*(action.mouse));
            }

            if(action.key != nullptr) {
                return sf::Keyboard::isKeyPressed(*(action.key));
            }

            if(action.event != nullptr) {
                return e.count(*(action.event));
            }

            return false;
        }
    };

public:
    ActionMap() = default;
    ActionMap(std::initializer_list<std::pair<Key, Action>> l) noexcept: actions(l.begin(), l.end()) {}

    bool isActive(Key&& key) noexcept {
        return actions.count(std::forward<Key>(key)) ? actions[std::forward<Key>(key)].active : false;
    }

    template<typename... Args>
    void bind(Key&& key, Args&&... args) {
        actions.emplace(std::piecewise_construct,
                        std::forward_as_tuple(key),
                        std::forward_as_tuple(args...));
    }

    template<typename Window>
    void update(Window& window) noexcept {
        sf::Event event;
        clearEvents();
        while(window.pollEvent(event)) {
            pushEvents(event);
        }
    }

    void pushEvents(const sf::Event& event) noexcept {
        eventBuffer.insert(event.type);

        Visitor visitor;

        for(auto&& action : actions) {
            if(action.second.isExpression()) {
                action.second.evaluate(&(action.second), [this, &visitor](const Action& a) {
                    return visitor(a, eventBuffer);
                });
            }
            else {
                action.second.updateStatus(visitor(action.second, eventBuffer));
            }
        }
    }

    void clearEvents() noexcept {
        eventBuffer.clear();
    }
};
} // sky

#endif // SKY_ACTIONMAP_HPP

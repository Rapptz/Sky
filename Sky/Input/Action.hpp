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

#ifndef SKY_ACTION_HPP
#define SKY_ACTION_HPP

#include "ActionNode.hpp"

namespace sky {
struct Action {
private:
    template<typename Key> friend class ActionMap;
    std::unique_ptr<ActionNode> node;
    Action(std::unique_ptr<ActionNode> node): node(std::move(node)) {}

    template<typename Buffer>
    bool isActive(const Buffer& buffer) const {
        return node->isActive(buffer);
    }
public:
    enum Type {
        Held,
        PressedOnce,
        Released
    };

    Action() = default;
    Action(Action&&) = default;
    Action& operator=(Action&&) = default;

    Action(const Action& other): node(other.node->clone()) {}
    Action& operator=(const Action& other) {
        node = other.node->clone();
        return *this;
    }

    Action(sf::Event::EventType event): node(new EventNode(event)) {}
    Action(sf::Keyboard::Key key, Type action = Held) {
        switch(action) {
        case Held:
            node.reset(new RealtimeKeyNode(key));
            break;
        case PressedOnce:
            node.reset(new KeyNode(key, sf::Event::KeyPressed));
            break;
        case Released:
            node.reset(new KeyNode(key, sf::Event::KeyReleased));
            break;
        }
    }

    Action(sf::Mouse::Button button, Type action = Held) {
        switch(action) {
        case Held:
            node.reset(new RealtimeMouseNode(button));
            break;
        case PressedOnce:
            node.reset(new MouseNode(button, sf::Event::MouseButtonPressed));
            break;
        case Released:
            node.reset(new MouseNode(button, sf::Event::MouseButtonReleased));
            break;
        }
    }

    friend Action operator&&(const Action& lhs, const Action& rhs) {
        return Action(std::unique_ptr<ActionNode>{new AndNode(lhs.node->clone(), rhs.node->clone())});
    }

    friend Action operator||(const Action& lhs, const Action& rhs) {
        return Action(std::unique_ptr<ActionNode>{new OrNode(lhs.node->clone(), rhs.node->clone())});
    }
};
} // sky

#endif // SKY_ACTION_HPP

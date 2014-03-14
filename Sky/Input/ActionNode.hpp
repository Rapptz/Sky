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

#ifndef SKY_ACTIONNODE_HPP
#define SKY_ACTIONNODE_HPP

#include "EventBuffer.hpp"
#include <memory>

namespace sky {
struct ActionNode {
    ActionNode() = default;
    virtual ~ActionNode() = default;
    virtual std::unique_ptr<ActionNode> clone() const = 0;
    virtual bool isActive(const EventBuffer& buffer) const = 0;
};

struct EventNode : public ActionNode {
private:
    sf::Event event;
public:
    explicit EventNode(sf::Event::EventType type) noexcept {
        event.type = type;
    }

    virtual std::unique_ptr<ActionNode> clone() const {
        return std::unique_ptr<ActionNode>{new EventNode(event.type)};
    }

    virtual bool isActive(const EventBuffer& buffer) const {
        return buffer.contains(event);
    }
};

struct KeyNode : public ActionNode {
private:
    sf::Event event;
public:
    explicit KeyNode(sf::Keyboard::Key key, sf::Event::EventType type) noexcept {
        event.key.code = key;
        event.type = type;
    }

    virtual std::unique_ptr<ActionNode> clone() const {
        return std::unique_ptr<ActionNode>{new KeyNode(event.key.code, event.type)};
    }

    virtual bool isActive(const EventBuffer& buffer) const {
        return buffer.contains(event, [this](const sf::Event& e){
            return e.type == event.type && e.key.code == event.key.code;
        });
    }
};

struct RealtimeKeyNode : public ActionNode {
private:
    sf::Keyboard::Key key;
public:
    explicit RealtimeKeyNode(sf::Keyboard::Key key) noexcept: key(key) {}

    virtual std::unique_ptr<ActionNode> clone() const {
        return std::unique_ptr<ActionNode>{new RealtimeKeyNode(key)};
    }

    virtual bool isActive(const EventBuffer& buffer) const {
        return buffer.isRealtime() && sf::Keyboard::isKeyPressed(key);
    }
};

struct MouseNode : public ActionNode {
private:
    sf::Event event;
public:
    explicit MouseNode(sf::Mouse::Button button, sf::Event::EventType type) noexcept {
        event.mouseButton.button = button;
        event.type = type;
    }

    virtual std::unique_ptr<ActionNode> clone() const {
        return std::unique_ptr<ActionNode>{new MouseNode(event.mouseButton.button, event.type)};
    }

    virtual bool isActive(const EventBuffer& buffer) const {
        return buffer.contains(event, [this](const sf::Event& e){
            return e.type == event.type && e.mouseButton.button == event.mouseButton.button;
        });
    }
};

struct RealtimeMouseNode : public ActionNode {
private:
    sf::Mouse::Button button;
public:
    explicit RealtimeMouseNode(sf::Mouse::Button button) noexcept: button(button) {}
    virtual std::unique_ptr<ActionNode> clone() const {
        return std::unique_ptr<ActionNode>{new RealtimeMouseNode(button)};
    }
    virtual bool isActive(const EventBuffer& buffer) const {
        return buffer.isRealtime() && sf::Mouse::isButtonPressed(button);
    }
};

struct AndNode : public ActionNode {
private:
    using pointer = std::unique_ptr<ActionNode>;
    pointer lhs;
    pointer rhs;
public:
    AndNode(pointer lhs, pointer rhs): lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    virtual std::unique_ptr<ActionNode> clone() const {
        return std::unique_ptr<ActionNode>{new AndNode(lhs->clone(), rhs->clone())};
    }

    virtual bool isActive(const EventBuffer& buffer) const {
        return lhs->isActive(buffer) && rhs->isActive(buffer);
    }
};

struct OrNode : public ActionNode {
private:
    using pointer = std::unique_ptr<ActionNode>;
    pointer lhs;
    pointer rhs;
public:
    OrNode(pointer lhs, pointer rhs): lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    virtual std::unique_ptr<ActionNode> clone() const {
        return std::unique_ptr<ActionNode>{new OrNode(lhs->clone(), rhs->clone())};
    }

    virtual bool isActive(const EventBuffer& buffer) const {
        return lhs->isActive(buffer) || rhs->isActive(buffer);
    }
};
} // sky

#endif // SKY_ACTIONNODE_HPP

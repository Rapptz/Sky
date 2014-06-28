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

#ifndef SKY_EVENTBUFFER_HPP
#define SKY_EVENTBUFFER_HPP

#include <SFML/Window/Event.hpp>
#include <set>
#include <utility>

namespace sky {
struct EventCompare {
    bool operator()(const sf::Event& lhs, const sf::Event& rhs) const noexcept {
        return lhs.type < rhs.type;
    }
};

struct EventBuffer {
private:
    std::multiset<sf::Event, EventCompare> events;
    bool realtime = true;
public:
    EventBuffer() = default;

    void insert(sf::Event event) {
        switch(event.type) {
        case sf::Event::GainedFocus:
            realtime = true;
            break;
        case sf::Event::LostFocus:
            realtime = false;
            break;
        default:
            break;
        }

        events.insert(std::move(event));
    }

    auto count(const sf::Event& event) const -> decltype(events.count(event)) {
        return events.count(event);
    }

    bool contains(const sf::Event& event) const {
        return events.count(event) >= 1;
    }

    template<typename Predicate>
    bool contains(const sf::Event& event, Predicate p) const {
        auto pair = events.equal_range(event);
        for(auto begin = pair.first; begin != pair.second; ++begin) {
            if(p(*begin)) {
                return true;
            }
        }
        return false;
    }

    auto find(const sf::Event& event) const -> decltype(events.find(event)) {
        return events.find(event);
    }

    void clear() {
        events.clear();
    }

    bool isRealtime() const {
        return realtime;
    }
};
} // sky

#endif // SKY_EVENTBUFFER_HPP

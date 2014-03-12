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

#ifndef SKY_ANIMATION_HPP
#define SKY_ANIMATION_HPP

#include <vector>
#include <utility>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sky {
class Animation {
private:
    const sf::Texture* texture = nullptr;
    std::vector<sf::IntRect> frames;
public:
    Animation() = default;

    Animation(const sf::Texture& texture): texture(&texture) {}

    void setTexture(const sf::Texture& texture) {
        this->texture = &texture;
    }

    void addFrame(sf::IntRect frame) {
        frames.emplace_back(std::move(frame));
    }

    void addFrame(int x, int y, int width, int height) {
        frames.emplace_back(x, y, width, height);
    }

    void addFrames(size_t count, int x, int y, int width, int height) {
        for(size_t i = 0; i < count; ++i) {
            frames.emplace_back(x, y, width, height);
            x += width;
        }
    }

    const sf::Texture* getTexture() const {
        return texture;
    }

    const sf::IntRect& getFrame(size_t index) const {
        return frames[index];
    }

    size_t getSize() const {
        return frames.size();
    }
};
} // sky

#endif // SKY_ANIMATION_HPP

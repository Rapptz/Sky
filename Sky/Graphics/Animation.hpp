// The MIT License (MIT)

// Copyright (c) 2014 Danny Y., Rapptz

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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

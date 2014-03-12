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

#ifndef SKY_SPRITESHEET_HPP
#define SKY_SPRITESHEET_HPP

#include <SFML/Graphics/Texture.hpp>
#include <utility>
#include <stdexcept>
#include <type_traits>

// Circumvent including "unnecessary" dependencies.
namespace sf {
class Sprite;
} // sf

namespace sky {
class Animation;

class Spritesheet {
private:
    const sf::Texture* data = nullptr;
    std::vector<sf::IntRect> sprites;
public:
    Spritesheet() noexcept = default;

    explicit Spritesheet(const sf::Texture& tex, unsigned width, unsigned height, unsigned spacing = 0): data(&tex) {
        setSpriteSize(width, height, spacing);
    }

    auto operator[](unsigned index) noexcept -> decltype(sprites[0]) {
        return sprites[index];
    }

    auto at(unsigned index) -> decltype(sprites[0]) {
        if(index >= sprites.size()) {
            throw std::out_of_range("Index exceeds number of sprites");
        }

        return sprites[index];
    }

    unsigned getNumberOfSprites() const noexcept {
        return sprites.size();
    }

    const sf::Texture* getTexture() const noexcept {
        return data;
    }

    void setTexture(const sf::Texture& texture) noexcept {
        data = &texture;
        sprites.clear();
    }

    void setSpriteSize(unsigned width, unsigned height, unsigned spacing = 0) {
        if(data == nullptr) {
            throw std::logic_error("No texture is set");
        }

        auto size = data->getSize();

        for(unsigned y = 0; y < size.y; y += height + spacing) {
            for(unsigned x = 0; x < size.x; x += width + spacing) {
                sprites.emplace_back(x, y, width, height);
            }
        }
    }

    template<class T = Animation, typename... Args>
    T createAnimation(Args && ... indices) {
        static_assert(sizeof(T), "Incomplete type. Did you forget to include <Sky/Graphics/Animation.hpp>?");

        if(data == nullptr || sprites.empty()) {
            return T();
        }

        T result(*data);
        typename std::common_type<Args...>::type frames[] = { std::forward<Args>(indices)... };

        for(auto && frame : frames) {
            result.addFrame(this->at(frame));
        }

        return result;
    }

    template<class T = sf::Sprite>
    T createSprite(unsigned index) {
        static_assert(sizeof(T), "Incomplete type. Did you forget to include <SFML/Graphics/Sprite.hpp>?");

        if(data == nullptr || sprites.empty()) {
            return T();
        }

        return T(*data, sprites[index]);
    }
};
} // namespace sky

#endif // SKY_SPRITESHEET_HPP
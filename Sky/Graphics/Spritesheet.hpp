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
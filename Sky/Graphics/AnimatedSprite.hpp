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

#ifndef SKY_ANIMATEDSPRITE_HPP
#define SKY_ANIMATEDSPRITE_HPP

#include "Animation.hpp"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

namespace sky {
class AnimatedSprite : public sf::Drawable, public sf::Transformable {
private:
    const Animation* animation = nullptr;
    const sf::Texture* texture = nullptr;
    bool looped = true;
    bool paused = false;
    sf::Time delay = sf::seconds(0.1f);
    sf::Time currentTime;
    size_t currentFrame = 0;
    sf::Vertex vertices[4];

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        if(animation != nullptr && texture != nullptr) {
            states.transform *= getTransform();
            states.texture = texture;
            target.draw(vertices, 4, sf::Quads, states);
        }
    }
public:
    AnimatedSprite() = default;
    explicit AnimatedSprite(const Animation& animation, sf::Time delay = sf::seconds(0.1f)):
    animation(&animation), texture(animation.getTexture()), delay(delay) {}

    void setFrame(size_t position, bool reset = true) {
        if(animation != nullptr) {
            auto&& rect = animation->getFrame(position);

            vertices[0].position = sf::Vector2f(0, 0);
            vertices[1].position = sf::Vector2f(0, rect.height);
            vertices[2].position = sf::Vector2f(rect.width, rect.height);
            vertices[3].position = sf::Vector2f(rect.width, 0);

            float left = static_cast<float>(rect.left) + 0.0001f;
            float right = left + rect.width;
            float top = static_cast<float>(rect.top);
            float bottom = top + rect.height;

            vertices[0].texCoords = sf::Vector2f(left, top);
            vertices[1].texCoords = sf::Vector2f(left, bottom);
            vertices[2].texCoords = sf::Vector2f(right, bottom);
            vertices[3].texCoords = sf::Vector2f(right, top);
        }

        if(reset) {
            currentTime = sf::Time::Zero;
        }
    }

    sf::IntRect getFrame(size_t position) const {
        return animation->getFrame(position);
    }

    void setAnimation(const Animation& animation) {
        this->animation = &animation;
        texture = this->animation->getTexture();
        currentFrame = 0;
        setFrame(currentFrame);
    }

    void play() {
        if(paused) {
            currentTime = sf::Time::Zero;
            paused = false;
        }
    }

    void pause() {
        paused = true;
    }

    void stop() {
        paused = true;
        currentFrame = 0;
    }

    void setLooped(bool looped) {
        this->looped = looped;
    }

    bool isLooped() const {
        return looped;
    }

    bool isPlaying() const {
        return !paused;
    }

    sf::Time getDelay() const {
        return delay;
    }

    void setDelay(const sf::Time& time) {
        delay = time;
    }

    sf::FloatRect getLocalBounds() const {
        auto&& rect = animation->getFrame(currentFrame);
        return sf::FloatRect(0.0f, 0.0f, rect.width, rect.height);
    }

    sf::FloatRect getGlobalBounds() const {
        return getTransform().transformRect(getLocalBounds());
    }

    void setColor(const sf::Color& color) {
        vertices[0].color = color;
        vertices[1].color = color;
        vertices[2].color = color;
        vertices[3].color = color;
    }

    virtual void update(sf::Time deltaTime) {
        if(!paused && (animation != nullptr)) {
            currentTime += deltaTime;

            if(currentTime >= delay) {
                currentTime = sf::Time::Zero;

                if(currentFrame + 1 < animation->getSize()) {
                    ++currentFrame;
                }
                else {
                    currentFrame = 0;
                    if(!looped) {
                        paused = true;
                    }
                }
            }
            setFrame(currentFrame, false);
        }
    }

    const sf::Texture* getTexture() const {
        return texture;
    }

    const Animation* getAnimation() const {
        return animation;
    }
};
} // sky

#endif // SKY_ANIMATEDSPRITE_HPP
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

#ifndef SKY_RESOURCECACHE_HPP
#define SKY_RESOURCECACHE_HPP

#include <memory>
#include <unordered_map>

namespace sky {
template<class Key, class Resource>
class ResourceCache {
private:
    std::unordered_map<Key, std::shared_ptr<Resource>> cache;
public:
    ResourceCache() = default;
    ResourceCache(ResourceCache&&) = default;            // Implicitly delete copy
    ResourceCache& operator=(ResourceCache&&) = default; // Implicitly delete copy

    template<typename... Args>
    auto insert(Key&& key, Args&&... args) -> decltype(*this) {
        if(!cache.count(std::forward<Key>(key))) {
            cache.emplace(std::forward<Key>(key), std::make_shared<Resource>(std::forward<Args>(args)...));
        }
        return *this;
    }

    auto insert(Key&& key, Resource* resource) -> decltype(*this) {
        if(!cache.count(std::forward<Key>(key))) {
            cache.emplace(std::forward<Key>(key), std::shared_ptr<Resource>(resource));
        }
        return *this;
    }

    auto release(Key&& key) -> decltype(*this) {
        auto it = cache.find(std::forward<Key>(key));
        if(it != cache.end()) {
            it->second.reset();
        }
        return *this;
    }

    template<class Callable>
    auto apply(Callable&& func) -> decltype(*this) {
        for(auto&& ptrpair : cache) {
            if(ptrpair.second) {
                func(ptrpair.second);
            }
        }

        return *this;
    }

    template<class Callable, typename... Keys>
    auto apply(Callable&& func, Keys&&... keys) -> decltype(*this) {
        Key indices[sizeof...(keys)] = { std::forward<Keys>(keys)... };
        for(auto&& index : indices) {
            auto ptr = this->get(index);
            if(ptr) {
                func(ptr);
            }
        }

        return *this;
    }

    std::shared_ptr<Resource> get(Key&& key) {
        auto it = cache.find(std::forward<Key>(key));
        if(it != cache.end()) {
            return it->second;
        }
        return nullptr;
    }
};
} // sky

#endif // SKY_RESOURCECACHE_HPP

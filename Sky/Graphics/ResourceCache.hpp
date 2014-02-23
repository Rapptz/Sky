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

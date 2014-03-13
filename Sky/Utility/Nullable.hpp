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

#ifndef SKY_NULLABLE_HPP
#define SKY_NULLABLE_HPP

#include <type_traits>
#include <utility>
#include <cassert>
#include <functional> // std::hash
#include <memory>

namespace sky {
template<typename T>
struct Nullable {
private:
    static_assert(!std::is_reference<T>{}, "Nullable<T> must not hold a reference type");
    template<typename... Args>
    void emplace(Args&&... args) {
        assert(!initialised);
        ::new(&storage) T(std::forward<Args>(args)...);
        initialised = true;
    }

    void destroy() {
        assert(initialised);
        get().~T();
        initialised = false;
    }

    bool initialised = false;
    typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;
public:
    Nullable(std::nullptr_t) {}
    Nullable() = default;

    template<typename... Args>
    Nullable(Args&&... args) {
        emplace(std::forward<Args>(args)...);
    }

    Nullable(const T& t) {
        emplace(t);
    }

    Nullable(T&& t) {
        emplace(std::move(t));
    }

    Nullable(const Nullable& other) {
        if(other.initialised) {
            emplace(other.get());
        }
        else {
            initialised = false;
        }
    }
    Nullable(Nullable&& other) {
        if(other.initialised) {
            emplace(std::move(other.get()));
        }
        else {
            initialised = false;
        }
    }

    Nullable& operator=(std::nullptr_t) {
        if(initialised) {
            destroy();
        }

        return *this;
    }

    Nullable& operator=(const Nullable& other) {
        if(initialised && other.initialised) {
            this->get() = other.get();
        }
        else if(initialised) {
            destroy();
        }
        else if(other.initialised) {
            emplace(other.get());
        }
        else {
            initialised = false;
        }

        return *this;
    }
    Nullable& operator=(Nullable&& other) {
        if(initialised && other.initialised) {
            this->get() = std::move(other.get());
        }
        else if(initialised) {
            destroy();
        }
        else if(other.initialised) {
            emplace(std::move(other.get()));
        }
        else {
            initialised = false;
        }

        return *this;
    }

    ~Nullable() {
        if(initialised) {
            destroy();
        }
    }

    T& get() {
        return *static_cast<T*>(static_cast<void*>(&storage));
    }

    const T& get() const {
        return *static_cast<const T*>(static_cast<const void*>(&storage));
    }

    T& operator*() {
        return get();
    }

    const T& operator*() const {
        return get();
    }

    T* operator->() {
        return &get();
    }
    const T* operator->() const {
        return &get();
    }

    explicit operator bool() const {
        return initialised;
    }
};

template<typename T>
constexpr bool operator==(const Nullable<T>& x, const Nullable<T>& y) {
    return bool(x) != bool(y) ? false : bool(x) == false ? true : *x == *y;
}

template<typename T>
constexpr bool operator!=(const Nullable<T>& x, const Nullable<T>& y) {
    return !(x == y);
}

template<typename T>
constexpr bool operator<(const Nullable<T>& x, const Nullable<T>& y) {
    return (!y) ? false : (!x) ? true : *x < *y;
}

template<typename T>
constexpr bool operator>(const Nullable<T>& x, const Nullable<T>& y) {
    return (y < x);
}

template<typename T>
constexpr bool operator<=(const Nullable<T>& x, const Nullable<T>& y) {
    return !(y < x);
}

template<typename T>
constexpr bool operator>=(const Nullable<T>& x, const Nullable<T>& y) {
    return !(x < y);
}

template<typename T>
constexpr bool operator==(const Nullable<T>& x, std::nullptr_t) noexcept {
    return (!x);
}

template<typename T>
constexpr bool operator==(std::nullptr_t, const Nullable<T>& x) noexcept {
    return (!x);
}

template<typename T>
constexpr bool operator!=(const Nullable<T>& x, std::nullptr_t) noexcept {
    return bool(x);
}

template<typename T>
constexpr bool operator!=(std::nullptr_t, const Nullable<T>& x) noexcept {
    return bool(x);
}

template<typename T>
constexpr bool operator<(const Nullable<T>&, std::nullptr_t) noexcept {
    return false;
}

template<typename T>
constexpr bool operator<(std::nullptr_t, const Nullable<T>& x) noexcept {
    return bool(x);
}

template<typename T>
constexpr bool operator<=(const Nullable<T>& x, std::nullptr_t) noexcept {
    return (!x);
}

template<typename T>
constexpr bool operator<=(std::nullptr_t, const Nullable<T>&) noexcept {
    return true;
}

template<typename T>
constexpr bool operator>(const Nullable<T>& x, std::nullptr_t) noexcept {
    return bool(x);
}

template<typename T>
constexpr bool operator>(std::nullptr_t, const Nullable<T>&) noexcept {
    return false;
}

template<typename T>
constexpr bool operator>=(const Nullable<T>&, std::nullptr_t) noexcept {
    return true;
}

template<typename T>
constexpr bool operator>=(std::nullptr_t, const Nullable<T>& x) noexcept {
    return (!x);
}


template<typename T>
constexpr bool operator==(const Nullable<T>& x, const T& v) {
    return bool(x) ? *x == v : false;
}

template<typename T>
constexpr bool operator==(const T& v, const Nullable<T>& x) {
    return bool(x) ? v == *x : false;
}

template<typename T>
constexpr bool operator!=(const Nullable<T>& x, const T& v) {
    return bool(x) ? *x != v : true;
}

template<typename T>
constexpr bool operator!=(const T& v, const Nullable<T>& x) {
    return bool(x) ? v != *x : true;
}

template<typename T>
constexpr bool operator<(const Nullable<T>& x, const T& v) {
    return bool(x) ? *x < v : true;
}

template<typename T>
constexpr bool operator>(const T& v, const Nullable<T>& x) {
    return bool(x) ? v > *x : true;
}

template<typename T>
constexpr bool operator>(const Nullable<T>& x, const T& v) {
    return bool(x) ? *x > v : false;
}

template<typename T>
constexpr bool operator<(const T& v, const Nullable<T>& x) {
    return bool(x) ? v < *x : false;
}

template<typename T>
constexpr bool operator>=(const Nullable<T>& x, const T& v) {
    return bool(x) ? *x >= v : false;
}

template<typename T>
constexpr bool operator<=(const T& v, const Nullable<T>& x) {
    return bool(x) ? v <= *x : false;
}

template<typename T>
constexpr bool operator<=(const Nullable<T>& x, const T& v) {
    return bool(x) ? *x <= v : true;
}

template<typename T>
constexpr bool operator>=(const T& v, const Nullable<T>& x) {
    return bool(x) ? v >= *x : true;
}
} // sky

namespace std {
template<typename T>
struct hash<sky::Nullable<T>> {
    using result_type = typename hash<T>::result_type;
    using argument_type = sky::Nullable<T>;

    constexpr result_type operator()(const argument_type& arg) const {
        return arg ? hash<T>{}(arg.get()) : result_type{};
    }
};
} // std

#endif // SKY_NULLABLE_HPP

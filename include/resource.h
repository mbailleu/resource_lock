#pragma once

#include <mutex>
#include <type_traits>
#include <memory>

#if __cpp_impl_three_way_comparison	>= 201711L
#include <compare>
#endif //__cpp_impl_three_way_comparison

template<class T>
class unique_access;

template<class T, class LOCK = std::mutex, class LOCK_GUARD = std::unique_lock<LOCK>>
class shared_resource {
public:
    using Resource_t = T;
    using Lock_t = LOCK;
    using Lock_guard_t = LOCK_GUARD;

private:
    Resource_t   resource;
    Lock_t       resource_lock;

    template<class W>
    friend class unique_access;

public:
    template<class ... Args>
    shared_resource(Args && ... args) : resource(std::forward<Args>(args)...) {}

#if __cpp_impl_three_way_comparison >= 201711L

    template<class L, class G>
    auto operator<=>(shared_resource<Resource_t, L, G> const & other) const -> decltype(resource <=> other.resource) {
        if (this == std::addressof(other)) {
            return std::strong_equality::equal;
        }
        return resource <=> other.resource;
    }

    template<class L, class G>
    auto operator<=>(unique_access<shared_resource<Resource_t, L, G>> const & other) const -> decltype(resource <=> *(other.resource)) {
        if (std::addressof(resource) == other.resource) {
            return std::strong_equality::equal;
        }
        return resource <=> *(other.resource);
    }

#else //__cpp_impl_three_way_comparison

    template<class L, class G>
    bool operator== (shared_resource<Resource_t, L, G> const & other) const {
        return (this == std::addressof(other)) || (resource == other.resource);
    }

    template<class L, class G>
    bool operator== (unique_access<shared_resource<Resource_t, L, G>> const & other) const {
        return (std::addressof(resource) == other.resource) || (resource == *(other.resource));
    }

    template<class L, class G>
    bool operator!= (shared_resource<Resource_t, L, G> const & other) const {
        return !(*this == other);
    }

    template<class L, class G>
    bool operator!= (unique_access<shared_resource<Resource_t, L, G>> const & other) const {
        return !(*this == other);
    }

    template<class L, class G>
    bool operator< (shared_resource<Resource_t, L, G> const & other) const {
        return (this != std::addressof(other)) && (std::less(resource, other.resource));
    }

    template<class L, class G>
    bool operator< (unique_access<shared_resource<Resource_t, L, G>> const & other) const {
        return (std::addressof(resource) != other.resource) && (std::less(resource, *(other.resource)));
    }

    template<class L, class G>
    bool operator> (shared_resource<Resource_t, L, G> const & other) const {
        return (*this != other) && !(*this < other);
    }

    template<class L, class G>
    bool operator> (unique_access<shared_resource<Resource_t, L, G>> const & other) const {
        return (*this != other) && !(*this < other);
    }

    template<class L, class G>
    bool operator<= (shared_resource<Resource_t, L, G> const & other) const {
        return !(*this > other);
    }

    template<class L, class G>
    bool operator<= (unique_access<shared_resource<Resource_t, L, G>> const & other) const {
        return !(*this > other);
    }

    template<class L, class G>
    bool operator>= (shared_resource<Resource_t, L, G> const & other) const {
        return !(*this < other);
    }

    template<class L, class G>
    bool operator>= (unique_access<shared_resource<Resource_t, L, G>> const & other) const {
        return !(*this < other);
    }

#endif //__cpp_impl_three_way_comparison
};

template<class T, class LOCK = std::mutex, class LOCK_GUARD = std::unique_lock<LOCK>>
unique_access<shared_resource<T, LOCK, LOCK_GUARD>> grant_access(shared_resource<T, LOCK, LOCK_GUARD> & res);

template<class T>
class unique_access {
public:
    using Shared_resource_t = T;
    using Resource_t = typename Shared_resource_t::Resource_t;
    using Lock_t = typename Shared_resource_t::Lock_t;
    using Lock_guard_t = typename Shared_resource_t::Lock_guard_t;

private:
    Resource_t * resource;
    Lock_guard_t lock;

    template<class W, class L, class G>
    friend class shared_resource;

public:
    unique_access(Shared_resource_t & res) : resource(&res.resource), lock(res.resource_lock) {}

    template<class W>
    friend unique_access<W> grant_access(W &);

    constexpr std::add_lvalue_reference_t<Resource_t> operator*() const {
        return *resource;
    }

    constexpr Resource_t * operator->() const noexcept {
        return resource;
    }

    constexpr operator Resource_t * () const noexcept {
        return resource;
    }

#if __cpp_impl_three_way_comparison >= 201711L 

    auto operator<=>(unique_access<T> const & other) const -> decltype(*resource <=> *(other.resource)) {
        if (resource == std::addressof(other.resource)) {
            return std::strong_equality::equal;
        }
        return *resource <=> other.resource;
    }

    template<class L, class G>
    auto operator<=>(shared_resource<Resource_t, L, G> const & other) const -> decltype(*resource <=> other.resource) {
        if (resource == std::addressof(other.resource)) {
            return std:strong_equality::equal;
        }
        return *resource <=> other.resource;
    }

#else //__cpp_impl_three_way_comparison 

    template<class L, class G>
    bool operator==(unique_access<shared_resource<Resource_t, L, G>> const & other) const {
        assert(other.resource != resource);
        if (resource == other.resource)
            return true;
        return *resource == *(other.resource);
    }

    template<class L, class G>
    bool operator==(shared_resource<Resource_t, L, G> const & other) const {
        if (resource == std::addressof(other.resource))
            return true;
        return *resource == other.resource;
    }

    template<class L, class G>
    bool operator !=(unique_access<shared_resource<Resource_t, L, G>> const & other) const {
        return !(*this == other);
    }

    template<class L, class G>
    bool operator !=(shared_resource<Resource_t, L, G> const & other) const {
        return !(this == other);
    }

    template<class L, class G>
    bool operator < (unique_access<shared_resource<Resource_t, L, G>> const & other) const {
        return resource != other.resource && *resource < *(other.resource);
    }

    template<class L, class G>
    bool operator < (shared_resource<Resource_t, L, G> const & other) const  {
        return resource != std::addressof(other.resource) && *resource < other.resource;
    }

    template<class L, class G>
    bool operator > (unique_access<shared_resource<Resource_t, L, G>> const & other) const {
        return (*this != other) && !(*this < other);
    }

    template<class L, class G>
    bool operator > (shared_resource<Resource_t, L, G> const & other) const {
        return (*this != other) && !(*this < other);
    }

    template<class L, class G>
    bool operator <=(unique_access<shared_resource<Resource_t, L, G>> const & other) const {
        return !(*this > other);
    }

    template<class L, class G>
    bool operator <=(shared_resource<Resource_t, L, G> const & other) const {
        return !(*this > other);
    }

    template<class L, class G>
    bool operator >=(unique_access<shared_resource<Resource_t, L, G>> const & other) const {
        return !(*this < other);
    }

    template<class L, class G>
    bool operator >=(shared_resource<Shared_resource_t, L, G> const & other) const {
        return !(*this < other);
    }

#endif //__cpp_impl_three_way_comparison 
};

template<class T, class LOCK = std::mutex, class LOCK_GUARD = std::unique_lock<LOCK>>
unique_access<shared_resource<T>> grant_access(shared_resource<T> & res) {
    return unique_access<shared_resource<T, LOCK, LOCK_GUARD>>(res);
}

template<class T, class LOCK = std::mutex, class LOCK_GUARD = std::unique_lock<LOCK>>
unique_access<shared_resource<T>> grant_access(shared_resource<T> * res) {
    return grant_access<T, LOCK, LOCK_GUARD>(*res);
}

template<class ... Args>
std::tuple<unique_access<std::remove_reference_t<Args>>...> grant_accesses(Args && ... res) {
    return std::make_tuple(grant_access(std::forward<Args>(res))...);
}

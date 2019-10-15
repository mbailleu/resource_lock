#pragma once

#include <mutex>

template<class T>
class unique_access;

template<class T, class LOCK = std::mutex, class LOCK_GUARD = std::lock_guard<LOCK>>
class shared_resource {
public:
    using Resource_t = T;
    using Lock_t = LOCK;
    using Lock_guard_t = LOCK_GUARD;

private:
    Resource_t   resource;
    Lock_t       resource_lock;

    friend unique_access<shared_resource<Resource_t, Lock_t, Lock_guard_t>>;

public:
    template<class ... Args>
    shared_resource(Args && ... args) : resource(std::forward<Args>(args)...) {}
};

template<class T, class LOCK = std::mutex, class LOCK_GUARD = std::lock_guard<LOCK>>
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

public:
    unique_access(Shared_resource_t & res) : resource(&res.resource), lock(res.resource_lock) {}
    friend unique_access grant_access<Resource_t, Lock_t, Lock_guard_t>(Shared_resource_t & res);

    constexpr std::add_lvalue_reference_t<Resource_t> operator*() const {
        return *resource;
    }

    constexpr Resource_t * operator->() const noexcept {
        return resource;
    }

    constexpr operator Resource_t * () const noexcept {
        return resource;
    }
};

template<class T, class LOCK = std::mutex, class LOCK_GUARD = std::lock_guard<LOCK>>
unique_access<shared_resource<T>> grant_access(shared_resource<T> & res) {
    return unique_access<shared_resource<T, LOCK, LOCK_GUARD>>(res);
}

template<class T, class LOCK = std::mutex, class LOCK_GUARD = std::lock_guard<LOCK>>
unique_access<shared_resource<T>> grant_access(shared_resource<T> * res) {
    return grant_access<T, LOCK, LOCK_GUARD>(*res);
}

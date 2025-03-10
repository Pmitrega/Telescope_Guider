//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/json
//

#ifndef BOOST_JSON_IMPL_OBJECT_IPP
#define BOOST_JSON_IMPL_OBJECT_IPP

#include <boost/container_hash/hash.hpp>
#include <boost/json/object.hpp>
#include <boost/json/detail/digest.hpp>
#include <boost/json/detail/except.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <new>
#include <stdexcept>
#include <type_traits>

namespace boost {
namespace json {
namespace detail {

template<class CharRange>
std::pair<key_value_pair*, std::size_t>
find_in_object(
    object const& obj,
    CharRange key) noexcept
{
    BOOST_ASSERT(obj.t_->capacity > 0);
    if(obj.t_->is_small())
    {
        auto it = &(*obj.t_)[0];
        auto const last =
            &(*obj.t_)[obj.t_->size];
        for(;it != last; ++it)
            if( key == it->key() )
                return { it, 0 };
        return { nullptr, 0 };
    }
    std::pair<
        key_value_pair*,
        std::size_t> result;
    BOOST_ASSERT(obj.t_->salt != 0);
    result.second = detail::digest(key.begin(), key.end(), obj.t_->salt);
    auto i = obj.t_->bucket(
        result.second);
    while(i != object::null_index_)
    {
        auto& v = (*obj.t_)[i];
        if( key == v.key() )
        {
            result.first = &v;
            return result;
        }
        i = access::next(v);
    }
    result.first = nullptr;
    return result;
}


template
std::pair<key_value_pair*, std::size_t>
find_in_object<string_view>(
    object const& obj,
    string_view key) noexcept;

} // namespace detail

//----------------------------------------------------------

constexpr object::table::table() = default;

// empty objects point here
BOOST_JSON_REQUIRE_CONST_INIT
object::table object::empty_;

std::size_t
object::table::
digest(string_view key) const noexcept
{
    BOOST_ASSERT(salt != 0);
    return detail::digest(
        key.begin(), key.end(), salt);
}

auto
object::table::
bucket(std::size_t hash) noexcept ->
    index_t&
{
    return reinterpret_cast<
        index_t*>(&(*this)[capacity])[
            hash % capacity];
}

auto
object::table::
bucket(string_view key) noexcept ->
    index_t&
{
    return bucket(digest(key));
}

void
object::table::
clear() noexcept
{
    BOOST_ASSERT(! is_small());
    // initialize buckets
    std::memset(
        reinterpret_cast<index_t*>(
            &(*this)[capacity]),
        0xff, // null_index_
        capacity * sizeof(index_t));
}

object::table*
object::table::
allocate(
    std::size_t capacity,
    std::uintptr_t salt,
    storage_ptr const& sp)
{
    BOOST_STATIC_ASSERT(
        alignof(key_value_pair) >=
        alignof(index_t));
    BOOST_ASSERT(capacity > 0);
    BOOST_ASSERT(capacity <= max_size());
    table* p;
    if(capacity <= detail::small_object_size_)
    {
        p = reinterpret_cast<
            table*>(sp->allocate(
                sizeof(table) + capacity *
                    sizeof(key_value_pair)));
        p->capacity = static_cast<
            std::uint32_t>(capacity);
    }
    else
    {
        p = reinterpret_cast<
            table*>(sp->allocate(
                sizeof(table) + capacity * (
                    sizeof(key_value_pair) +
                    sizeof(index_t))));
        p->capacity = static_cast<
            std::uint32_t>(capacity);
        p->clear();
    }
    if(salt)
    {
        p->salt = salt;
    }
    else
    {
        // VFALCO This would be better if it
        //        was random, but maybe this
        //        is good enough.
        p->salt = reinterpret_cast<
            std::uintptr_t>(p);
    }
    return p;
}

//----------------------------------------------------------

void
object::
revert_construct::
destroy() noexcept
{
    obj_->destroy();
}

//----------------------------------------------------------

void
object::
revert_insert::
destroy() noexcept
{
    obj_->destroy(
        &(*obj_->t_)[size_],
        obj_->end());
}

//----------------------------------------------------------
//
// Construction
//
//----------------------------------------------------------

object::
object(detail::unchecked_object&& uo)
    : sp_(uo.storage())
{
    if(uo.size() == 0)
    {
        t_ = &empty_;
        return;
    }
    // should already be checked
    BOOST_ASSERT(
        uo.size() <= max_size());
    t_ = table::allocate(
        uo.size(), 0, sp_);

    // insert all elements, keeping
    // the last of any duplicate keys.
    auto dest = begin();
    auto src = uo.release();
    auto const end = src + 2 * uo.size();
    if(t_->is_small())
    {
        t_->size = 0;
        while(src != end)
        {
            access::construct_key_value_pair(
                dest, pilfer(src[0]), pilfer(src[1]));
            src += 2;
            auto result = detail::find_in_object(*this, dest->key());
            if(! result.first)
            {
                ++dest;
                ++t_->size;
                continue;
            }
            // handle duplicate
            auto& v = *result.first;
            // don't bother to check if
            // storage deallocate is trivial
            v.~key_value_pair();
            // trivial relocate
            std::memcpy(
                static_cast<void*>(&v),
                    dest, sizeof(v));
        }
        return;
    }
    while(src != end)
    {
        access::construct_key_value_pair(
            dest, pilfer(src[0]), pilfer(src[1]));
        src += 2;
        auto& head = t_->bucket(dest->key());
        auto i = head;
        for(;;)
        {
            if(i == null_index_)
            {
                // end of bucket
                access::next(
                    *dest) = head;
                head = static_cast<index_t>(
                    dest - begin());
                ++dest;
                break;
            }
            auto& v = (*t_)[i];
            if(v.key() != dest->key())
            {
                i = access::next(v);
                continue;
            }

            // handle duplicate
            access::next(*dest) =
                access::next(v);
            // don't bother to check if
            // storage deallocate is trivial
            v.~key_value_pair();
            // trivial relocate
            std::memcpy(
                static_cast<void*>(&v),
                    dest, sizeof(v));
            break;
        }
    }
    t_->size = static_cast<
        index_t>(dest - begin());
}

object::
~object() noexcept
{
    if(sp_.is_not_shared_and_deallocate_is_trivial())
        return;
    if(t_->capacity == 0)
        return;
    destroy();
}

object::
object(
    std::size_t min_capacity,
    storage_ptr sp)
    : sp_(std::move(sp))
    , t_(&empty_)
{
    reserve(min_capacity);
}

object::
object(object&& other) noexcept
    : sp_(other.sp_)
    , t_(detail::exchange(
        other.t_, &empty_))
{
}

object::
object(
    object&& other,
    storage_ptr sp)
    : sp_(std::move(sp))
{
    if(*sp_ == *other.sp_)
    {
        t_ = detail::exchange(
            other.t_, &empty_);
        return;
    }

    t_ = &empty_;
    object(other, sp_).swap(*this);
}

object::
object(
    object const& other,
    storage_ptr sp)
    : sp_(std::move(sp))
    , t_(&empty_)
{
    reserve(other.size());
    revert_construct r(*this);
    if(t_->is_small())
    {
        for(auto const& v : other)
        {
            ::new(end())
                key_value_pair(v, sp_);
            ++t_->size;
        }
        r.commit();
        return;
    }
    for(auto const& v : other)
    {
        // skip duplicate checking
        auto& head =
            t_->bucket(v.key());
        auto pv = ::new(end())
            key_value_pair(v, sp_);
        access::next(*pv) = head;
        head = t_->size;
        ++t_->size;
    }
    r.commit();
}

object::
object(
    std::initializer_list<std::pair<
        string_view, value_ref>> init,
    std::size_t min_capacity,
    storage_ptr sp)
    : sp_(std::move(sp))
    , t_(&empty_)
{
    if( min_capacity < init.size())
        min_capacity = init.size();
    reserve(min_capacity);
    revert_construct r(*this);
    insert(init);
    r.commit();
}

//----------------------------------------------------------
//
// Assignment
//
//----------------------------------------------------------

object&
object::
operator=(object const& other)
{
    object tmp(other, sp_);
    this->~object();
    ::new(this) object(pilfer(tmp));
    return *this;
}

object&
object::
operator=(object&& other)
{
    object tmp(std::move(other), sp_);
    this->~object();
    ::new(this) object(pilfer(tmp));
    return *this;
}

object&
object::
operator=(
    std::initializer_list<std::pair<
        string_view, value_ref>> init)
{
    object tmp(init, sp_);
    this->~object();
    ::new(this) object(pilfer(tmp));
    return *this;
}

//----------------------------------------------------------
//
// Lookup
//
//----------------------------------------------------------

system::result<value&>
object::
try_at(string_view key) noexcept
{
    auto it = find(key);
    if( it != end() )
        return it->value();

    system::error_code ec;
    BOOST_JSON_FAIL(ec, error::out_of_range);
    return ec;
}

system::result<value const&>
object::
try_at(string_view key) const noexcept
{
    auto it = find(key);
    if( it != end() )
        return it->value();

    system::error_code ec;
    BOOST_JSON_FAIL(ec, error::out_of_range);
    return ec;
}

value const&
object::
at(string_view key, source_location const& loc) const&
{
    return try_at(key).value(loc);
}

//----------------------------------------------------------
//
// Modifiers
//
//----------------------------------------------------------

void
object::
clear() noexcept
{
    if(empty())
        return;
    if(! sp_.is_not_shared_and_deallocate_is_trivial())
        destroy(begin(), end());
    if(! t_->is_small())
        t_->clear();
    t_->size = 0;
}

void
object::
insert(
    std::initializer_list<std::pair<
        string_view, value_ref>> init)
{
    auto const n0 = size();
    if(init.size() > max_size() - n0)
    {
        BOOST_STATIC_CONSTEXPR source_location loc = BOOST_CURRENT_LOCATION;
        detail::throw_system_error( error::object_too_large, &loc );
    }
    revert_insert r( *this, n0 + init.size() );
    if(t_->is_small())
    {
        for(auto& iv : init)
        {
            auto result =
                detail::find_in_object(*this, iv.first);
            if(result.first)
            {
                // ignore duplicate
                continue;
            }
            ::new(end()) key_value_pair(
                iv.first,
                iv.second.make_value(sp_));
            ++t_->size;
        }
        r.commit();
        return;
    }
    for(auto& iv : init)
    {
        auto& head = t_->bucket(iv.first);
        auto i = head;
        for(;;)
        {
            if(i == null_index_)
            {
                // VFALCO value_ref should construct
                // a key_value_pair using placement
                auto& v = *::new(end())
                    key_value_pair(
                        iv.first,
                        iv.second.make_value(sp_));
                access::next(v) = head;
                head = static_cast<index_t>(
                    t_->size);
                ++t_->size;
                break;
            }
            auto& v = (*t_)[i];
            if(v.key() == iv.first)
            {
                // ignore duplicate
                break;
            }
            i = access::next(v);
        }
    }
    r.commit();
}

auto
object::
erase(const_iterator pos) noexcept ->
    iterator
{
    return do_erase(pos,
        [this](iterator p) {
            // the casts silence warnings
            std::memcpy(
                static_cast<void*>(p),
                static_cast<void const*>(end()),
                sizeof(*p));
        },
        [this](iterator p) {
            reindex_relocate(end(), p);
        });
}

auto
object::
erase(string_view key) noexcept ->
    std::size_t
{
    auto it = find(key);
    if(it == end())
        return 0;
    erase(it);
    return 1;
}

auto
object::
stable_erase(const_iterator pos) noexcept ->
    iterator
{
    return do_erase(pos,
        [this](iterator p) {
            // the casts silence warnings
            std::memmove(
                static_cast<void*>(p),
                static_cast<void const*>(p + 1),
                sizeof(*p) * (end() - p));
        },
        [this](iterator p) {
            for (; p != end(); ++p)
            {
                reindex_relocate(p + 1, p);
            }
        });
}

auto
object::
stable_erase(string_view key) noexcept ->
    std::size_t
{
    auto it = find(key);
    if(it == end())
        return 0;
    stable_erase(it);
    return 1;
}

void
object::
swap(object& other)
{
    if(*sp_ == *other.sp_)
    {
        t_ = detail::exchange(
            other.t_, t_);
        return;
    }
    object temp1(
        std::move(*this),
        other.storage());
    object temp2(
        std::move(other),
        this->storage());
    other.~object();
    ::new(&other) object(pilfer(temp1));
    this->~object();
    ::new(this) object(pilfer(temp2));
}

//----------------------------------------------------------
//
// Lookup
//
//----------------------------------------------------------

auto
object::
operator[](string_view key) ->
    value&
{
    auto const result =
        emplace(key, nullptr);
    return result.first->value();
}

auto
object::
count(string_view key) const noexcept ->
    std::size_t
{
    if(find(key) == end())
        return 0;
    return 1;
}

auto
object::
find(string_view key) noexcept ->
    iterator
{
    if(empty())
        return end();
    auto const p =
        detail::find_in_object(*this, key).first;
    if(p)
        return p;
    return end();
}

auto
object::
find(string_view key) const noexcept ->
    const_iterator
{
    if(empty())
        return end();
    auto const p =
        detail::find_in_object(*this, key).first;
    if(p)
        return p;
    return end();
}

bool
object::
contains(
    string_view key) const noexcept
{
    if(empty())
        return false;
    return detail::find_in_object(*this, key).first
        != nullptr;
}

value const*
object::
if_contains(
    string_view key) const noexcept
{
    auto const it = find(key);
    if(it != end())
        return &it->value();
    return nullptr;
}

value*
object::
if_contains(
    string_view key) noexcept
{
    auto const it = find(key);
    if(it != end())
        return &it->value();
    return nullptr;
}

//----------------------------------------------------------
//
// (private)
//
//----------------------------------------------------------

key_value_pair*
object::
insert_impl(
    pilfered<key_value_pair> p,
    std::size_t hash)
{
    BOOST_ASSERT(
        capacity() > size());
    if(t_->is_small())
    {
        auto const pv = ::new(end())
            key_value_pair(p);
        ++t_->size;
        return pv;
    }
    auto& head =
        t_->bucket(hash);
    auto const pv = ::new(end())
        key_value_pair(p);
    access::next(*pv) = head;
    head = t_->size;
    ++t_->size;
    return pv;
}

// allocate new table, copy elements there, and rehash them
object::table*
object::
reserve_impl(std::size_t new_capacity)
{
    BOOST_ASSERT(
        new_capacity > t_->capacity);
    auto t = table::allocate(
        growth(new_capacity),
            t_->salt, sp_);
    if(! empty())
        std::memcpy(
            static_cast<
                void*>(&(*t)[0]),
            begin(),
            size() * sizeof(
                key_value_pair));
    t->size = t_->size;
    std::swap(t_, t);

    if(! t_->is_small())
    {
        // rebuild hash table,
        // without dup checks
        auto p = end();
        index_t i = t_->size;
        while(i-- > 0)
        {
            --p;
            auto& head =
                t_->bucket(p->key());
            access::next(*p) = head;
            head = i;
        }
    }

    return t;
}

bool
object::
equal(object const& other) const noexcept
{
    if(size() != other.size())
        return false;
    auto const end_ = other.end();
    for(auto e : *this)
    {
        auto it = other.find(e.key());
        if(it == end_)
            return false;
        if(it->value() != e.value())
            return false;
    }
    return true;
}

std::size_t
object::
growth(
    std::size_t new_size) const
{
    if(new_size > max_size())
    {
        BOOST_STATIC_CONSTEXPR source_location loc = BOOST_CURRENT_LOCATION;
        detail::throw_system_error( error::object_too_large, &loc );
    }
    std::size_t const old = capacity();
    if(old > max_size() - old / 2)
        return new_size;
    std::size_t const g =
        old + old / 2; // 1.5x
    if(g < new_size)
        return new_size;
    return g;
}

void
object::
remove(
    index_t& head,
    key_value_pair& v) noexcept
{
    BOOST_ASSERT(! t_->is_small());
    auto const i = static_cast<
        index_t>(&v - begin());
    if(head == i)
    {
        head = access::next(v);
        return;
    }
    auto* pn =
        &access::next((*t_)[head]);
    while(*pn != i)
        pn = &access::next((*t_)[*pn]);
    *pn = access::next(v);
}

void
object::
destroy() noexcept
{
    BOOST_ASSERT(t_->capacity > 0);
    BOOST_ASSERT(! sp_.is_not_shared_and_deallocate_is_trivial());
    destroy(begin(), end());
    table::deallocate(t_, sp_);
}

void
object::
destroy(
    key_value_pair* first,
    key_value_pair* last) noexcept
{
    BOOST_ASSERT(! sp_.is_not_shared_and_deallocate_is_trivial());
    while(last != first)
        (--last)->~key_value_pair();
}

template<class FS, class FB>
auto
object::
do_erase(
    const_iterator pos,
    FS small_reloc,
    FB big_reloc) noexcept
    -> iterator
{
    auto p = begin() + (pos - begin());
    if(t_->is_small())
    {
        p->~value_type();
        --t_->size;
        if(p != end())
        {
            small_reloc(p);
        }
        return p;
    }
    remove(t_->bucket(p->key()), *p);
    p->~value_type();
    --t_->size;
    if(p != end())
    {
        big_reloc(p);
    }
    return p;
}

void
object::
reindex_relocate(
    key_value_pair* src,
    key_value_pair* dst) noexcept
{
    BOOST_ASSERT(! t_->is_small());
    auto& head = t_->bucket(src->key());
    remove(head, *src);
    // the casts silence warnings
    std::memcpy(
        static_cast<void*>(dst),
        static_cast<void const*>(src),
        sizeof(*dst));
    access::next(*dst) = head;
    head = static_cast<
        index_t>(dst - begin());
}

} // namespace json
} // namespace boost

//----------------------------------------------------------
//
// std::hash specialization
//
//----------------------------------------------------------

std::size_t
std::hash<::boost::json::object>::operator()(
    ::boost::json::object const& jo) const noexcept
{
    return ::boost::hash< ::boost::json::object >()( jo );
}

//----------------------------------------------------------


#endif

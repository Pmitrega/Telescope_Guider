//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2020 Krystian Stasiowski (sdkrystian@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/json
//

#ifndef BOOST_JSON_VALUE_HPP
#define BOOST_JSON_VALUE_HPP

#include <boost/json/detail/config.hpp>
#include <boost/json/array.hpp>
#include <boost/json/kind.hpp>
#include <boost/json/object.hpp>
#include <boost/json/pilfer.hpp>
#include <boost/json/set_pointer_options.hpp>
#include <boost/json/storage_ptr.hpp>
#include <boost/json/string.hpp>
#include <boost/json/string_view.hpp>
#include <boost/json/value_ref.hpp>
#include <boost/json/detail/except.hpp>
#include <boost/json/detail/value.hpp>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iosfwd>
#include <limits>
#include <new>
#include <type_traits>
#include <utility>

namespace boost {
namespace json {

//----------------------------------------------------------

/** The type used to represent any JSON value

    This is a
    <a href="https://en.cppreference.com/w/cpp/concepts/regular"><em>Regular</em></a>
    type which works like
    a variant of the basic JSON data types: array,
    object, string, number, boolean, and null.

    @par Thread Safety

    Distinct instances may be accessed concurrently.
    Non-const member functions of a shared instance
    may not be called concurrently with any other
    member functions of that instance.
*/
class value
{
#ifndef BOOST_JSON_DOCS
    using scalar = detail::scalar;

    union
    {
        storage_ptr sp_; // must come first
        array       arr_;
        object      obj_;
        string      str_;
        scalar      sca_;
    };
#endif

    struct init_iter;

#ifndef BOOST_JSON_DOCS
    // VFALCO doc toolchain incorrectly treats this as public
    friend struct detail::access;
#endif

    explicit
    value(
        detail::unchecked_array&& ua)
        : arr_(std::move(ua))
    {
    }

    explicit
    value(
        detail::unchecked_object&& uo)
        : obj_(std::move(uo))
    {
    }

    value(
        detail::key_t const&,
        string_view s,
        storage_ptr sp)
        : str_(detail::key_t{}, s, std::move(sp))
    {
    }

    value(
        detail::key_t const&,
        string_view s1,
        string_view s2,
        storage_ptr sp)
        : str_(detail::key_t{}, s1, s2, std::move(sp))
    {
    }

    inline bool is_scalar() const noexcept
    {
        return sca_.k < json::kind::string;
    }

public:
    /// Associated [Allocator](https://en.cppreference.com/w/cpp/named_req/Allocator)
    using allocator_type = container::pmr::polymorphic_allocator<value>;

    /** Destructor.

        The value and all of its contents are destroyed.
        Any dynamically allocated memory that was allocated
        internally is freed.

        @par Complexity
        Constant, or linear in size for array or object.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    ~value() noexcept;

    /** Default constructor.

        The constructed value is null,
        using the [default memory resource].

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        [default memory resource]: json/allocators/storage_ptr.html#json.allocators.storage_ptr.default_memory_resource
    */
    value() noexcept
        : sca_()
    {
    }

    /** Constructor.

        The constructed value is null, using the
        specified `boost::container::pmr::memory_resource`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    explicit
    value(storage_ptr sp) noexcept
        : sca_(std::move(sp))
    {
    }

    /** Pilfer constructor.

        The value is constructed by acquiring ownership
        of the contents of `other` using pilfer semantics.
        This is more efficient than move construction, when
        it is known that the moved-from object will be
        immediately destroyed afterwards.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param other The value to pilfer. After pilfer
        construction, `other` is not in a usable state
        and may only be destroyed.

        @see @ref pilfer,
            <a href="http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0308r0.html">
                Valueless Variants Considered Harmful</a>
    */
    value(pilfered<value> other) noexcept
    {
        relocate(this, other.get());
        ::new(&other.get().sca_) scalar();
    }

    /** Copy constructor.

        The value is constructed with a copy of the
        contents of `other`, using the same
        memory resource as `other`.

        @par Complexity
        Linear in the size of `other`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param other The value to copy.
    */
    value(value const& other)
        : value(other, other.storage())
    {
    }

    /** Copy constructor

        The value is constructed with a copy of the
        contents of `other`, using the
        specified memory resource.

        @par Complexity
        Linear in the size of `other`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param other The value to copy.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    BOOST_JSON_DECL
    value(
        value const& other,
        storage_ptr sp);

    /** Move constructor

        The value is constructed by acquiring ownership of
        the contents of `other` and shared ownership of
        `other`'s memory resource.

        @note

        After construction, the moved-from value becomes a
        null value with its current storage pointer.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param other The value to move.
    */
    BOOST_JSON_DECL
    value(value&& other) noexcept;

    /** Move constructor

        The value is constructed with the contents of
        `other` by move semantics, using the specified
        memory resource:

        @li If `*other.storage() == *sp`, ownership of
        the underlying memory is transferred in constant
        time, with no possibility of exceptions.
        After construction, the moved-from value becomes
        a null value with its current storage pointer.

        @li If `*other.storage() != *sp`, an
        element-wise copy is performed if
        `other.is_structured() == true`, which may throw.
        In this case, the moved-from value is not
        changed.

        @par Complexity
        Constant or linear in the size of `other`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param other The value to move.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    BOOST_JSON_DECL
    value(
        value&& other,
        storage_ptr sp);

    //------------------------------------------------------
    //
    // Conversion
    //
    //------------------------------------------------------

    /** Construct a null.

        A null value is a monostate.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        std::nullptr_t,
        storage_ptr sp = {}) noexcept
        : sca_(std::move(sp))
    {
    }

    /** Construct a bool.

        This constructs a `bool` value using
        the specified memory resource.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param b The initial value.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
#ifdef BOOST_JSON_DOCS
    value(
        bool b,
        storage_ptr sp = {}) noexcept;
#else
    template<class T
        ,class = typename std::enable_if<
            std::is_same<T, bool>::value>::type
    >
    value(
        T b,
        storage_ptr sp = {}) noexcept
        : sca_(b, std::move(sp))
    {
    }
#endif

    /** Construct a `std::int64_t`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param i The initial value.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        signed char i,
        storage_ptr sp = {}) noexcept
        : sca_(static_cast<std::int64_t>(
            i), std::move(sp))
    {
    }

    /** Construct a `std::int64_t`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param i The initial value.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        short i,
        storage_ptr sp = {}) noexcept
        : sca_(static_cast<std::int64_t>(
            i), std::move(sp))
    {
    }

    /** Construct a `std::int64_t`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param i The initial value.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        int i,
        storage_ptr sp = {}) noexcept
        : sca_(static_cast<std::int64_t>(i),
            std::move(sp))
    {
    }

    /** Construct a `std::int64_t`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param i The initial value.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        long i,
        storage_ptr sp = {}) noexcept
        : sca_(static_cast<std::int64_t>(i),
            std::move(sp))
    {
    }

    /** Construct a `std::int64_t`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param i The initial value.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        long long i,
        storage_ptr sp = {}) noexcept
        : sca_(static_cast<std::int64_t>(i),
            std::move(sp))
    {
    }

    /** Construct a `std::uint64_t`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param u The initial value.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        unsigned char u,
        storage_ptr sp = {}) noexcept
        : sca_(static_cast<std::uint64_t>(
            u), std::move(sp))
    {
    }

    /** Construct a `std::uint64_t`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param u The initial value.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        unsigned short u,
        storage_ptr sp = {}) noexcept
        : sca_(static_cast<std::uint64_t>(u),
            std::move(sp))
    {
    }

    /** Construct a `std::uint64_t`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param u The initial value.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        unsigned int u,
        storage_ptr sp = {}) noexcept
        : sca_(static_cast<std::uint64_t>(u),
            std::move(sp))
    {
    }

    /** Construct a `std::uint64_t`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param u The initial value.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        unsigned long u,
        storage_ptr sp = {}) noexcept
        : sca_(static_cast<std::uint64_t>(u),
            std::move(sp))
    {
    }

    /** Construct a `std::uint64_t`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param u The initial value.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        unsigned long long u,
        storage_ptr sp = {}) noexcept
        : sca_(static_cast<std::uint64_t>(u),
            std::move(sp))
    {
    }

    /** Construct a `double`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param d The initial value.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        double d,
        storage_ptr sp = {}) noexcept
        : sca_(d, std::move(sp))
    {
    }

    /** Construct a @ref string.

        The string is constructed with a copy of the
        string view `s`, using the specified memory resource.

        @par Complexity
        Linear in `s.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param s The string view to construct with.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        string_view s,
        storage_ptr sp = {})
        : str_(s, std::move(sp))
    {
    }

    /** Construct a @ref string.

        The string is constructed with a copy of the
        null-terminated string `s`, using the specified
        memory resource.

        @par Complexity
        Linear in `std::strlen(s)`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param s The null-terminated string to construct
        with.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        char const* s,
        storage_ptr sp = {})
        : str_(s, std::move(sp))
    {
    }

    /** Construct a @ref string.

        The value is constructed from `other`, using the
        same memory resource. To transfer ownership, use `std::move`:

        @par Example
        @code
        string str = "The Boost C++ Library Collection";

        // transfer ownership
        value jv( std::move(str) );

        assert( str.empty() );
        assert( *str.storage() == *jv.storage() );
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param other The string to construct with.
    */
    value(
        string other) noexcept
        : str_(std::move(other))
    {
    }

    /** Construct a @ref string.

        The value is copy constructed from `other`,
        using the specified memory resource.

        @par Complexity
        Linear in `other.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param other The string to construct with.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        string const& other,
        storage_ptr sp)
        : str_(
            other,
            std::move(sp))
    {
    }

    /** Construct a @ref string.

        The value is move constructed from `other`,
        using the specified memory resource.

        @par Complexity
        Constant or linear in `other.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param other The string to construct with.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        string&& other,
        storage_ptr sp)
        : str_(
            std::move(other),
            std::move(sp))
    {
    }

    /** Construct a @ref string.

        This is the fastest way to construct
        an empty string, using the specified
        memory resource. The variable @ref string_kind
        may be passed as the first parameter
        to select this overload:

        @par Example
        @code
        // Construct an empty string

        value jv( string_kind );
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.

        @see @ref string_kind
    */
    value(
        string_kind_t,
        storage_ptr sp = {}) noexcept
        : str_(std::move(sp))
    {
    }

    /** Construct an @ref array.

        The value is constructed from `other`, using the
        same memory resource. To transfer ownership, use `std::move`:

        @par Example
        @code
        array arr( {1, 2, 3, 4, 5} );

        // transfer ownership
        value jv( std::move(arr) );

        assert( arr.empty() );
        assert( *arr.storage() == *jv.storage() );
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param other The array to construct with.
    */
    value(array other) noexcept
        : arr_(std::move(other))
    {
    }

    /** Construct an @ref array.

        The value is copy constructed from `other`,
        using the specified memory resource.

        @par Complexity
        Linear in `other.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param other The array to construct with.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        array const& other,
        storage_ptr sp)
        : arr_(
            other,
            std::move(sp))
    {
    }

    /** Construct an @ref array.

        The value is move-constructed from `other`,
        using the specified memory resource.

        @par Complexity
        Constant or linear in `other.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param other The array to construct with.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        array&& other,
        storage_ptr sp)
        : arr_(
            std::move(other),
            std::move(sp))
    {
    }

    /** Construct an @ref array.

        This is the fastest way to construct
        an empty array, using the specified
        memory resource. The variable @ref array_kind
        may be passed as the first parameter
        to select this overload:

        @par Example
        @code
        // Construct an empty array

        value jv( array_kind );
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.

        @see @ref array_kind
    */
    value(
        array_kind_t,
        storage_ptr sp = {}) noexcept
        : arr_(std::move(sp))
    {
    }

    /** Construct an @ref object.

        The value is constructed from `other`, using the
        same memory resource. To transfer ownership, use `std::move`:

        @par Example
        @code
        object obj( {{"a",1}, {"b",2}, {"c"},3}} );

        // transfer ownership
        value jv( std::move(obj) );

        assert( obj.empty() );
        assert( *obj.storage() == *jv.storage() );
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param other The object to construct with.
    */
    value(object other) noexcept
        : obj_(std::move(other))
    {
    }

    /** Construct an @ref object.

        The value is copy constructed from `other`,
        using the specified memory resource.

        @par Complexity
        Linear in `other.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param other The object to construct with.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        object const& other,
        storage_ptr sp)
        : obj_(
            other,
            std::move(sp))
    {
    }

    /** Construct an @ref object.

        The value is move constructed from `other`,
        using the specified memory resource.

        @par Complexity
        Constant or linear in `other.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param other The object to construct with.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.
    */
    value(
        object&& other,
        storage_ptr sp)
        : obj_(
            std::move(other),
            std::move(sp))
    {
    }

    /** Construct an @ref object.

        This is the fastest way to construct
        an empty object, using the specified
        memory resource. The variable @ref object_kind
        may be passed as the first parameter
        to select this overload:

        @par Example
        @code
        // Construct an empty object

        value jv( object_kind );
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.

        @see @ref object_kind
    */
    value(
        object_kind_t,
        storage_ptr sp = {}) noexcept
        : obj_(std::move(sp))
    {
    }

    /** Construct from an initializer-list

        @li If the initializer list consists of key/value
        pairs, an @ref object is created; otherwise,

        @li if the size of the initializer list is exactly 1, the object is
        constructed directly from that sole element; otherwise,

        @li an @ref array is created.

        The contents of the initializer list are copied to the newly
        constructed value using the specified memory resource.

        @par Complexity
        Linear in `init.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param init The initializer list to construct from.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The container will acquire shared ownership of the memory
        resource.

        @par Note
        The previous behavior of this constructor was to always
        construct either an @ref object or an @ref array. In practice though,
        several C++ implementations did not treat `value{x}` as a constructor
        from initializer list. This effectively resulted in different behavior
        on different implementations. <br>

        If you need the legacy behavior define macro
        `BOOST_JSON_LEGACY_INIT_LIST_BEHAVIOR` when you are building the
        library. The macro and the functionality will be deprecated in the
        future and then removed, so we urge you to change your code for the new
        behavior as soon as possible. The simplest way to create an @ref array
        with 1 element using an initializer list is via `array{x}`.
    */
    BOOST_JSON_DECL
    value(
        std::initializer_list<value_ref> init,
        storage_ptr sp = {});

    //------------------------------------------------------
    //
    // Assignment
    //
    //------------------------------------------------------

    /** Copy assignment.

        The contents of the value are replaced with an
        element-wise copy of the contents of `other`.

        @par Complexity
        Linear in the size of `*this` plus `other`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param other The value to copy.
    */
    BOOST_JSON_DECL
    value&
    operator=(value const& other);

    /** Move assignment.

        The contents of the value are replaced with the
        contents of `other` using move semantics:

        @li If `*other.storage() == *sp`, ownership of
        the underlying memory is transferred in constant
        time, with no possibility of exceptions.
        After assignment, the moved-from value becomes
        a null with its current storage pointer.

        @li If `*other.storage() != *sp`, an
        element-wise copy is performed if
        `other.is_structured() == true`, which may throw.
        In this case, the moved-from value is not
        changed.

        @par Complexity
        Constant, or linear in
        `this->size()` plus `other.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param other The value to assign from.
    */
    BOOST_JSON_DECL
    value&
    operator=(value&& other);

    /** Assignment.

        Replace `*this` with the value formed by
        constructing from `init` and `this->storage()`.
        If the initializer list consists of key/value
        pairs, the resulting @ref object is assigned.
        Otherwise an @ref array is assigned. The contents
        of the initializer list are moved to `*this`
        using the existing memory resource.

        @par Complexity
        Linear in `init.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param init The initializer list to assign from.
    */
    BOOST_JSON_DECL
    value&
    operator=(
        std::initializer_list<value_ref> init);

    /** Assignment.

        Replace `*this` with null.

        @par Exception Safety
        No-throw guarantee.

        @par Complexity
        Linear in the size of `*this`.
    */
    value&
    operator=(std::nullptr_t) noexcept
    {
        if(is_scalar())
        {
            sca_.k = json::kind::null;
        }
        else
        {
            ::new(&sca_) scalar(
                destroy());
        }
        return *this;
    }

    /** Assignment.

        Replace `*this` with `b`.

        @par Exception Safety
        No-throw guarantee.

        @par Complexity
        Linear in the size of `*this`.

        @param b The new value.
    */
#ifdef BOOST_JSON_DOCS
    value& operator=(bool b) noexcept;
#else
    template<class T
        ,class = typename std::enable_if<
            std::is_same<T, bool>::value>::type
    >
    value& operator=(T b) noexcept
    {
        if(is_scalar())
        {
            sca_.b = b;
            sca_.k = json::kind::bool_;
        }
        else
        {
            ::new(&sca_) scalar(
                b, destroy());
        }
        return *this;
    }
#endif

    /** Assignment.

        Replace `*this` with `i`.

        @par Exception Safety
        No-throw guarantee.

        @par Complexity
        Linear in the size of `*this`.

        @param i The new value.
    */
    /** @{ */
    value& operator=(signed char i) noexcept
    {
        return operator=(
            static_cast<long long>(i));
    }

    value& operator=(short i) noexcept
    {
        return operator=(
            static_cast<long long>(i));
    }

    value& operator=(int i) noexcept
    {
        return operator=(
            static_cast<long long>(i));
    }

    value& operator=(long i) noexcept
    {
        return operator=(
            static_cast<long long>(i));
    }

    value& operator=(long long i) noexcept
    {
        if(is_scalar())
        {
            sca_.i = i;
            sca_.k = json::kind::int64;
        }
        else
        {
            ::new(&sca_) scalar(static_cast<
                std::int64_t>(i), destroy());
        }
        return *this;
    }
    /** @} */

    /** Assignment.

        Replace `*this` with `i`.

        @par Exception Safety
        No-throw guarantee.

        @par Complexity
        Linear in the size of `*this`.

        @param u The new value.
    */
    /** @{ */
    value& operator=(unsigned char u) noexcept
    {
        return operator=(static_cast<
            unsigned long long>(u));
    }

    value& operator=(unsigned short u) noexcept
    {
        return operator=(static_cast<
            unsigned long long>(u));
    }

    value& operator=(unsigned int u) noexcept
    {
        return operator=(static_cast<
            unsigned long long>(u));
    }

    value& operator=(unsigned long u) noexcept
    {
        return operator=(static_cast<
            unsigned long long>(u));
    }

    value& operator=(unsigned long long u) noexcept
    {
        if(is_scalar())
        {
            sca_.u = u;
            sca_.k = json::kind::uint64;
        }
        else
        {
            ::new(&sca_) scalar(static_cast<
                std::uint64_t>(u), destroy());
        }
        return *this;
    }
    /** @} */

    /** Assignment.

        Replace `*this` with `d`.

        @par Exception Safety
        No-throw guarantee.

        @par Complexity
        Linear in the size of `*this`.

        @param d The new value.
    */
    value& operator=(double d) noexcept
    {
        if(is_scalar())
        {
            sca_.d = d;
            sca_.k = json::kind::double_;
        }
        else
        {
            ::new(&sca_) scalar(
                d, destroy());
        }
        return *this;
    }

    /** Assignment.

        Replace `*this` with a copy of the string `s`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @par Complexity
        Linear in the sum of sizes of `*this` and `s`

        @param s The new string.
    */
    /** @{ */
    BOOST_JSON_DECL value& operator=(string_view s);
    BOOST_JSON_DECL value& operator=(char const* s);
    BOOST_JSON_DECL value& operator=(string const& s);
    /** @} */

    /** Assignment.

        The contents of the value are replaced with the
        contents of `s` using move semantics:

        @li If `*other.storage() == *this->storage()`,
        ownership of the underlying memory is transferred
        in constant time, with no possibility of exceptions.
        After assignment, the moved-from string becomes
        empty with its current storage pointer.

        @li If `*other.storage() != *this->storage()`, an
        element-wise copy is performed, which may throw.
        In this case, the moved-from string is not
        changed.

        @par Complexity
        Constant, or linear in the size of `*this` plus `s.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param s The string to move-assign from.
    */
    BOOST_JSON_DECL value& operator=(string&& s);

    /** Assignment.

        Replace `*this` with a copy of the array `arr`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @par Complexity
        Linear in the sum of sizes of `*this` and `arr`

        @param arr The new array.
    */
    BOOST_JSON_DECL value& operator=(array const& arr);

    /** Assignment.

        The contents of the value are replaced with the
        contents of `arr` using move semantics:

        @li If `*arr.storage() == *this->storage()`,
        ownership of the underlying memory is transferred
        in constant time, with no possibility of exceptions.
        After assignment, the moved-from array becomes
        empty with its current storage pointer.

        @li If `*arr.storage() != *this->storage()`, an
        element-wise copy is performed, which may throw.
        In this case, the moved-from array is not
        changed.

        @par Complexity
        Constant, or linear in the size of `*this` plus `arr.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param arr The array to move-assign from.
    */
    BOOST_JSON_DECL value& operator=(array&& arr);

    /** Assignment.

        Replace `*this` with a copy of the obect `obj`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @par Complexity
        Linear in the sum of sizes of `*this` and `obj`

        @param obj The new object.
    */
    BOOST_JSON_DECL value& operator=(object const& obj);

    /** Assignment.

        The contents of the value are replaced with the
        contents of `obj` using move semantics:

        @li If `*obj.storage() == *this->storage()`,
        ownership of the underlying memory is transferred
        in constant time, with no possibility of exceptions.
        After assignment, the moved-from object becomes
        empty with its current storage pointer.

        @li If `*obj.storage() != *this->storage()`, an
        element-wise copy is performed, which may throw.
        In this case, the moved-from object is not
        changed.

        @par Complexity
        Constant, or linear in the size of `*this` plus `obj.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param obj The object to move-assign from.
    */
    BOOST_JSON_DECL value& operator=(object&& obj);

    //------------------------------------------------------
    //
    // Modifiers
    //
    //------------------------------------------------------

    /** Change the kind to null, discarding the previous contents.

        The value is replaced with a null,
        destroying the previous contents.

        @par Complexity
        Linear in the size of `*this`.

        @par Exception Safety
        No-throw guarantee.
    */
    void
    emplace_null() noexcept
    {
        *this = nullptr;
    }

    /** Return a reference to a `bool`, changing the kind and replacing the contents.

        The value is replaced with a `bool`
        initialized to `false`, destroying the
        previous contents.

        @par Complexity
        Linear in the size of `*this`.

        @par Exception Safety
        No-throw guarantee.
    */
    bool&
    emplace_bool() noexcept
    {
        *this = false;
        return sca_.b;
    }

    /** Return a reference to a `std::int64_t`, changing the kind and replacing the contents.

        The value is replaced with a `std::int64_t`
        initialized to zero, destroying the
        previous contents.

        @par Complexity
        Linear in the size of `*this`.

        @par Exception Safety
        No-throw guarantee.
    */
    std::int64_t&
    emplace_int64() noexcept
    {
        *this = std::int64_t{};
        return sca_.i;
    }

    /** Return a reference to a `std::uint64_t`, changing the kind and replacing the contents.

        The value is replaced with a `std::uint64_t`
        initialized to zero, destroying the
        previous contents.

        @par Complexity
        Linear in the size of `*this`.

        @par Exception Safety
        No-throw guarantee.
    */
    std::uint64_t&
    emplace_uint64() noexcept
    {
        *this = std::uint64_t{};
        return sca_.u;
    }

    /** Return a reference to a `double`, changing the kind and replacing the contents.

        The value is replaced with a `double`
        initialized to zero, destroying the
        previous contents.

        @par Complexity
        Linear in the size of `*this`.

        @par Exception Safety
        No-throw guarantee.
    */
    double&
    emplace_double() noexcept
    {
        *this = double{};
        return sca_.d;
    }

    /** Return a reference to a @ref string, changing the kind and replacing the contents.

        The value is replaced with an empty @ref string
        using the current memory resource, destroying the
        previous contents.

        @par Complexity
        Linear in the size of `*this`.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    string&
    emplace_string() noexcept;

    /** Return a reference to an @ref array, changing the kind and replacing the contents.

        The value is replaced with an empty @ref array
        using the current memory resource, destroying the
        previous contents.

        @par Complexity
        Linear in the size of `*this`.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    array&
    emplace_array() noexcept;

    /** Return a reference to an @ref object, changing the kind and replacing the contents.

        The contents are replaced with an empty @ref object using the current
        `boost::container::pmr::memory_resource`. All previously obtained
        iterators and references obtained beforehand are invalidated.

        @par Complexity
        Linear in the size of `*this`.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    object&
    emplace_object() noexcept;

    /** Swap the given values.

        Exchanges the contents of this value with another value. Ownership of
        the respective `boost::container::pmr::memory_resource` objects is not
        transferred:

        @li If `*other.storage() == *this->storage()`,
        ownership of the underlying memory is swapped in
        constant time, with no possibility of exceptions.
        All iterators and references remain valid.

        @li If `*other.storage() != *this->storage()`,
        the contents are logically swapped by making copies,
        which can throw. In this case all iterators and
        references are invalidated.

        @par Complexity
        Constant or linear in the sum of the sizes of
        the values.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param other The value to swap with.
        If `this == &other`, this function call has no effect.
    */
    BOOST_JSON_DECL
    void
    swap(value& other);

    /** Swap the given values.

        Exchanges the contents of value `lhs` with another value `rhs`.
        Ownership of the respective `boost::container::pmr::memory_resource`
        objects is not transferred.

        @li If `*lhs.storage() == *rhs.storage()`,
        ownership of the underlying memory is swapped in
        constant time, with no possibility of exceptions.
        All iterators and references remain valid.

        @li If `*lhs.storage() != *rhs.storage`,
        the contents are logically swapped by a copy,
        which can throw. In this case all iterators and
        references are invalidated.

        @par Effects
        @code
        lhs.swap( rhs );
        @endcode

        @par Complexity
        Constant or linear in the sum of the sizes of
        the values.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param lhs The value to exchange.

        @param rhs The value to exchange.
        If `&lhs == &rhs`, this function call has no effect.

        @see @ref value::swap
    */
    friend
    void
    swap(value& lhs, value& rhs)
    {
        lhs.swap(rhs);
    }

    //------------------------------------------------------
    //
    // Observers
    //
    //------------------------------------------------------

    /** Returns the kind of this JSON value.

        This function returns the discriminating
        enumeration constant of type @ref json::kind
        corresponding to the underlying representation
        stored in the container.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    json::kind
    kind() const noexcept
    {
        return static_cast<json::kind>(
            static_cast<unsigned char>(
                sca_.k) & 0x3f);
    }

    /** Return `true` if this is an array

        This function is used to determine if the underlying
        representation is a certain kind.

        @par Effects
        @code
        return this->kind() == kind::array;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool
    is_array() const noexcept
    {
        return kind() == json::kind::array;
    }

    /** Return `true` if this is an object

        This function is used to determine if the underlying
        representation is a certain kind.

        @par Effects
        @code
        return this->kind() == kind::object;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool
    is_object() const noexcept
    {
        return kind() == json::kind::object;
    }

    /** Return `true` if this is a string

        This function is used to determine if the underlying
        representation is a certain kind.

        @par Effects
        @code
        return this->kind() == kind::string;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool
    is_string() const noexcept
    {
        return kind() == json::kind::string;
    }

    /** Return `true` if this is a signed integer

        This function is used to determine if the underlying
        representation is a certain kind.

        @par Effects
        @code
        return this->kind() == kind::int64;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool
    is_int64() const noexcept
    {
        return kind() == json::kind::int64;
    }

    /** Return `true` if this is a unsigned integer

        This function is used to determine if the underlying
        representation is a certain kind.

        @par Effects
        @code
        return this->kind() == kind::uint64;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool
    is_uint64() const noexcept
    {
        return kind() == json::kind::uint64;
    }

    /** Return `true` if this is a double

        This function is used to determine if the underlying
        representation is a certain kind.

        @par Effects
        @code
        return this->kind() == kind::double_;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool
    is_double() const noexcept
    {
        return kind() == json::kind::double_;
    }

    /** Return `true` if this is a bool

        This function is used to determine if the underlying
        representation is a certain kind.

        @par Effects
        @code
        return this->kind() == kind::bool_;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool
    is_bool() const noexcept
    {
        return kind() == json::kind::bool_;
    }

    /** Returns true if this is a null.

        This function is used to determine if the underlying
        representation is a certain kind.

        @par Effects
        @code
        return this->kind() == kind::null;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool
    is_null() const noexcept
    {
        return kind() == json::kind::null;
    }

    /** Returns true if this is an array or object.

        This function returns `true` if
        @ref kind() is either `kind::object` or
        `kind::array`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool
    is_structured() const noexcept
    {
        // VFALCO Could use bit 0x20 for this
        return
           kind() == json::kind::object ||
           kind() == json::kind::array;
    }

    /** Returns true if this is not an array or object.

        This function returns `true` if
        @ref kind() is neither `kind::object` nor
        `kind::array`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool
    is_primitive() const noexcept
    {
        // VFALCO Could use bit 0x20 for this
        return
           sca_.k != json::kind::object &&
           sca_.k != json::kind::array;
    }

    /** Returns true if this is a number.

        This function returns `true` when
        @ref kind() is one of the following values:
        `kind::int64`, `kind::uint64`, or
        `kind::double_`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool
    is_number() const noexcept
    {
        // VFALCO Could use bit 0x40 for this
        return
            kind() == json::kind::int64 ||
            kind() == json::kind::uint64 ||
            kind() == json::kind::double_;
    }

    //------------------------------------------------------

    /** Return an @ref array pointer if this is an array, else return `nullptr`

        If `this->kind() == kind::array`, returns a pointer
        to the underlying array. Otherwise, returns `nullptr`.

        @par Example
        The return value is used in both a boolean context and
        to assign a variable:
        @code
        if( auto p = jv.if_array() )
            return *p;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    array const*
    if_array() const noexcept
    {
        if(kind() == json::kind::array)
            return &arr_;
        return nullptr;
    }

    /** Return an @ref array pointer if this is an array, else return `nullptr`

        If `this->kind() == kind::array`, returns a pointer
        to the underlying array. Otherwise, returns `nullptr`.

        @par Example
        The return value is used in both a boolean context and
        to assign a variable:
        @code
        if( auto p = jv.if_array() )
            return *p;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    array*
    if_array() noexcept
    {
        if(kind() == json::kind::array)
            return &arr_;
        return nullptr;
    }

    /** Return an @ref object pointer if this is an object, else return `nullptr`

        If `this->kind() == kind::object`, returns a pointer
        to the underlying object. Otherwise, returns `nullptr`.

        @par Example
        The return value is used in both a boolean context and
        to assign a variable:
        @code
        if( auto p = jv.if_object() )
            return *p;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    object const*
    if_object() const noexcept
    {
        if(kind() == json::kind::object)
            return &obj_;
        return nullptr;
    }

    /** Return an @ref object pointer if this is an object, else return `nullptr`

        If `this->kind() == kind::object`, returns a pointer
        to the underlying object. Otherwise, returns `nullptr`.

        @par Example
        The return value is used in both a boolean context and
        to assign a variable:
        @code
        if( auto p = jv.if_object() )
            return *p;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    object*
    if_object() noexcept
    {
        if(kind() == json::kind::object)
            return &obj_;
        return nullptr;
    }

    /** Return a @ref string pointer if this is a string, else return `nullptr`

        If `this->kind() == kind::string`, returns a pointer
        to the underlying object. Otherwise, returns `nullptr`.

        @par Example
        The return value is used in both a boolean context and
        to assign a variable:
        @code
        if( auto p = jv.if_string() )
            return *p;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    string const*
    if_string() const noexcept
    {
        if(kind() == json::kind::string)
            return &str_;
        return nullptr;
    }

    /** Return a @ref string pointer if this is a string, else return `nullptr`

        If `this->kind() == kind::string`, returns a pointer
        to the underlying object. Otherwise, returns `nullptr`.

        @par Example
        The return value is used in both a boolean context and
        to assign a variable:
        @code
        if( auto p = jv.if_string() )
            return *p;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    string*
    if_string() noexcept
    {
        if(kind() == json::kind::string)
            return &str_;
        return nullptr;
    }

    /** Return an `int64_t` pointer if this is a signed integer, else return `nullptr`

        If `this->kind() == kind::int64`, returns a pointer
        to the underlying integer. Otherwise, returns `nullptr`.

        @par Example
        The return value is used in both a boolean context and
        to assign a variable:
        @code
        if( auto p = jv.if_int64() )
            return *p;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    std::int64_t const*
    if_int64() const noexcept
    {
        if(kind() == json::kind::int64)
            return &sca_.i;
        return nullptr;
    }

    /** Return an `int64_t` pointer if this is a signed integer, else return `nullptr`

        If `this->kind() == kind::int64`, returns a pointer
        to the underlying integer. Otherwise, returns `nullptr`.

        @par Example
        The return value is used in both a boolean context and
        to assign a variable:
        @code
        if( auto p = jv.if_int64() )
            return *p;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    std::int64_t*
    if_int64() noexcept
    {
        if(kind() == json::kind::int64)
            return &sca_.i;
        return nullptr;
    }

    /** Return a `uint64_t` pointer if this is an unsigned integer, else return `nullptr`

        If `this->kind() == kind::uint64`, returns a pointer
        to the underlying unsigned integer. Otherwise, returns
        `nullptr`.

        @par Example
        The return value is used in both a boolean context and
        to assign a variable:
        @code
        if( auto p = jv.if_uint64() )
            return *p;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    std::uint64_t const*
    if_uint64() const noexcept
    {
        if(kind() == json::kind::uint64)
            return &sca_.u;
        return nullptr;
    }

    /** Return a `uint64_t` pointer if this is an unsigned integer, else return `nullptr`

        If `this->kind() == kind::uint64`, returns a pointer
        to the underlying unsigned integer. Otherwise, returns
        `nullptr`.

        @par Example
        The return value is used in both a boolean context and
        to assign a variable:
        @code
        if( auto p = jv.if_uint64() )
            return *p;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    std::uint64_t*
    if_uint64() noexcept
    {
        if(kind() == json::kind::uint64)
            return &sca_.u;
        return nullptr;
    }

    /** Return a `double` pointer if this is a double, else return `nullptr`

        If `this->kind() == kind::double_`, returns a pointer
        to the underlying double. Otherwise, returns
        `nullptr`.

        @par Example
        The return value is used in both a boolean context and
        to assign a variable:
        @code
        if( auto p = jv.if_double() )
            return *p;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    double const*
    if_double() const noexcept
    {
        if(kind() == json::kind::double_)
            return &sca_.d;
        return nullptr;
    }

    /** Return a `double` pointer if this is a double, else return `nullptr`

        If `this->kind() == kind::double_`, returns a pointer
        to the underlying double. Otherwise, returns
        `nullptr`.

        @par Example
        The return value is used in both a boolean context and
        to assign a variable:
        @code
        if( auto p = jv.if_double() )
            return *p;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    double*
    if_double() noexcept
    {
        if(kind() == json::kind::double_)
            return &sca_.d;
        return nullptr;
    }

    /** Return a `bool` pointer if this is a boolean, else return `nullptr`

        If `this->kind() == kind::bool_`, returns a pointer
        to the underlying boolean. Otherwise, returns
        `nullptr`.

        @par Example
        The return value is used in both a boolean context and
        to assign a variable:
        @code
        if( auto p = jv.if_bool() )
            return *p;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool const*
    if_bool() const noexcept
    {
        if(kind() == json::kind::bool_)
            return &sca_.b;
        return nullptr;
    }

    /** Return a `bool` pointer if this is a boolean, else return `nullptr`

        If `this->kind() == kind::bool_`, returns a pointer
        to the underlying boolean. Otherwise, returns
        `nullptr`.

        @par Example
        The return value is used in both a boolean context and
        to assign a variable:
        @code
        if( auto p = jv.if_bool() )
            return *p;
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool*
    if_bool() noexcept
    {
        if(kind() == json::kind::bool_)
            return &sca_.b;
        return nullptr;
    }

    //------------------------------------------------------

    /** Return the stored number cast to an arithmetic type.

        This function attempts to return the stored value
        converted to the arithmetic type `T` which may not
        be `bool`:

        @li If `T` is an integral type and the stored
        value is a number which can be losslessly converted,
        the conversion is performed without error and the
        converted number is returned.

        @li If `T` is an integral type and the stored value
        is a number which cannot be losslessly converted,
        then the operation fails with an error.

        @li If `T` is a floating point type and the stored
        value is a number, the conversion is performed
        without error. The converted number is returned,
        with a possible loss of precision.

        @li Otherwise, if the stored value is not a number;
        that is, if `this->is_number()` returns `false`, then
        the operation fails with an error.

        @par Constraints
        @code
        std::is_arithmetic< T >::value && ! std::is_same< T, bool >::value
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @return The converted number.

        @param ec Set to the error, if any occurred.
    */
/** @{ */
    template<class T>
#ifdef BOOST_JSON_DOCS
    T
#else
    typename std::enable_if<
        std::is_arithmetic<T>::value &&
        ! std::is_same<T, bool>::value,
            T>::type
#endif
    to_number(system::error_code& ec) const noexcept
    {
        error e;
        auto result = to_number<T>(e);
        BOOST_JSON_FAIL(ec, e);
        return result;
    }

    template<class T>
#ifdef BOOST_JSON_DOCS
    T
#else
    typename std::enable_if<
        std::is_arithmetic<T>::value &&
        ! std::is_same<T, bool>::value,
            T>::type
#endif
    to_number(std::error_code& ec) const noexcept
    {
        system::error_code jec;
        auto result = to_number<T>(jec);
        ec = jec;
        return result;
    }
/** @} */

    /** Return the stored number as `boost::system::result<T>`.

        This function attempts to return the stored value converted to the
        arithmetic type `T` which may not be `bool`:

        @li If `T` is an integral type and the stored value is a number which
            can be losslessly converted, the conversion is performed without
            error and `result<T>` containing the converted number is returned.

        @li If `T` is an integral type and the stored value is a number which
            cannot be losslessly converted, then `result<T>` containing the
            corresponding `error_code` is returned.

        @li If `T` is a floating point type and the stored value is a number,
            the conversion is performed without error. `result<T>` containing
            the converted number, with a possible loss of precision, is
            returned.

        @li Otherwise, if the stored value is not a number; that is, if
            `this->is_number()` returns `false`, then `result<T>` containing
            the corresponding `error_code` is returned.

        @par Constraints
        @code
        std::is_arithmetic< T >::value && ! std::is_same< T, bool >::value
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @return `boost::system::result<T>` with either the converted number or
            an `error_code`.
    */
    template<class T>
#ifdef BOOST_JSON_DOCS
    system::result<T>
#else
    typename std::enable_if<
        std::is_arithmetic<T>::value && ! std::is_same<T, bool>::value,
        system::result<T>
    >::type
#endif
    try_to_number() const noexcept
    {
        system::error_code ec;
        T result = to_number<T>(ec);
        if( ec )
            return {system::in_place_error, ec};

        return {system::in_place_value, result};
    }

    /** Return the stored number cast to an arithmetic type.

        This function attempts to return the stored value
        converted to the arithmetic type `T` which may not
        be `bool`:

        @li If `T` is an integral type and the stored
        value is a number which can be losslessly converted,
        the conversion is performed without error and the
        converted number is returned.

        @li If `T` is an integral type and the stored value
        is a number which cannot be losslessly converted,
        then the operation fails with an error.

        @li If `T` is a floating point type and the stored
        value is a number, the conversion is performed
        without error. The converted number is returned,
        with a possible loss of precision.

        @li Otherwise, if the stored value is not a number;
        that is, if `this->is_number()` returns `false`, then
        the operation fails with an error.

        @par Constraints
        @code
        std::is_arithmetic< T >::value && ! std::is_same< T, bool >::value
        @endcode

        @par Complexity
        Constant.

        @return The converted number.

        @throw `boost::system::system_error` Thrown on error.
    */
    template<class T>
#ifdef BOOST_JSON_DOCS
    T
#else
    typename std::enable_if<
        std::is_arithmetic<T>::value &&
        ! std::is_same<T, bool>::value,
            T>::type
#endif
    to_number() const
    {
        return try_to_number<T>().value();
    }

    //------------------------------------------------------
    //
    // Accessors
    //
    //------------------------------------------------------

    /** Return the associated memory resource.

        This function returns the `boost::container::pmr::memory_resource` used
        by the container.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    storage_ptr const&
    storage() const noexcept
    {
        return sp_;
    }

    /** Return the associated allocator.

        This function returns an instance of @ref allocator_type constructed
        from the associated `boost::container::pmr::memory_resource`.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    allocator_type
    get_allocator() const noexcept
    {
        return sp_.get();
    }

    //------------------------------------------------------

    /** Return `result` with a reference to the underlying @ref array

        If @ref is_array() is `true`, the result contains a reference to the
        underlying @ref array, otherwise it contains an `error_code`.

        @par Example
        The return value can be used in both a boolean context and
        to assign a variable:
        @code
        if( auto r = jv.try_as_array() )
            return *r;
        @endcode

        But can also be used to throw an exception on error:
        @code
        return jv.try_as_array().value();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    /** @{ */
    BOOST_JSON_DECL
    system::result<array&>
    try_as_array() noexcept;

    BOOST_JSON_DECL
    system::result<array const&>
    try_as_array() const noexcept;
    /** @} */

    /** Return `result` with a reference to the underlying @ref object

        If @ref is_object() is `true`, the result contains a reference to the
        underlying @ref object, otherwise it contains an `error_code`.

        @par Example
        The return value can be used in both a boolean context and
        to assign a variable:
        @code
        if( auto r = jv.try_as_object() )
            return *r;
        @endcode

        But can also be used to throw an exception on error:
        @code
        return jv.try_as_object().value();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    /** @{ */
    BOOST_JSON_DECL
    system::result<object&>
    try_as_object() noexcept;

    BOOST_JSON_DECL
    system::result<object const&>
    try_as_object() const noexcept;
    /** @} */

    /** Return `result` with a reference to the underlying @ref string

        If @ref is_string() is `true`, the result contains a reference to the
        underlying @ref string, otherwise it contains an `error_code`.

        @par Example
        The return value can be used in both a boolean context and
        to assign a variable:
        @code
        if( auto r = jv.try_as_string() )
            return *r;
        @endcode

        But can also be used to throw an exception on error:
        @code
        return jv.try_as_string().value();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    /** @{ */
    BOOST_JSON_DECL
    system::result<string&>
    try_as_string() noexcept;

    BOOST_JSON_DECL
    system::result<string const&>
    try_as_string() const noexcept;
    /** @} */

    /** Return `result` with a reference to the underlying `std::int64_t`

        If @ref is_int64() is `true`, the result contains a reference to the
        underlying `std::int64_t`, otherwise it contains an `error_code`.

        @par Example
        The return value can be used in both a boolean context and
        to assign a variable:
        @code
        if( auto r = jv.try_as_int64() )
            return *r;
        @endcode

        But can also be used to throw an exception on error:
        @code
        return jv.try_as_int64().value();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    system::result<std::int64_t&>
    try_as_int64() noexcept;

    /** Return `result` with the underlying `std::int64_t`

        If @ref is_int64() is `true`, the result contains a copy of the
        underlying `std::int64_t`, otherwise it contains an `error_code`.

        @par Example
        The return value can be used in both a boolean context and
        to assign a variable:
        @code
        if( auto r = jv.try_as_int64() )
            return *r;
        @endcode

        But can also be used to throw an exception on error:
        @code
        return jv.try_as_int64().value();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    system::result<std::int64_t>
    try_as_int64() const noexcept;

    /** Return `result` with a reference to the underlying `std::uint64_t`

        If @ref is_uint64() is `true`, the result contains a reference to the
        underlying `std::uint64_t`, otherwise it contains an `error_code`.

        @par Example
        The return value can be used in both a boolean context and
        to assign a variable:
        @code
        if( auto r = jv.try_as_uint64() )
            return *r;
        @endcode

        But can also be used to throw an exception on error:
        @code
        return jv.try_as_uint64().value();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    system::result<std::uint64_t&>
    try_as_uint64() noexcept;

    /** Return `result` with the underlying `std::uint64_t`

        If @ref is_uint64() is `true`, the result contains a copy of the
        underlying `std::uint64_t`, otherwise it contains an `error_code`.

        @par Example
        The return value can be used in both a boolean context and
        to assign a variable:
        @code
        if( auto r = jv.try_as_uint64() )
            return *r;
        @endcode

        But can also be used to throw an exception on error:
        @code
        return jv.try_as_uint64().value();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    system::result<std::uint64_t>
    try_as_uint64() const noexcept;

    /** Return `result` with a reference to the underlying `double`

        If @ref is_double() is `true`, the result contains a reference to the
        underlying `double`, otherwise it contains an `error_code`.

        @par Example
        The return value can be used in both a boolean context and
        to assign a variable:
        @code
        if( auto r = jv.try_as_double() )
            return *r;
        @endcode

        But can also be used to throw an exception on error:
        @code
        return jv.try_as_double().value();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    system::result<double&>
    try_as_double() noexcept;

    /** Return `result` with the underlying `double`

        If @ref is_double() is `true`, the result contains a copy of the
        underlying `double`, otherwise it contains an `error_code`.

        @par Example
        The return value can be used in both a boolean context and
        to assign a variable:
        @code
        if( auto r = jv.try_as_double() )
            return *r;
        @endcode

        But can also be used to throw an exception on error:
        @code
        return jv.try_as_double().value();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    system::result<double>
    try_as_double() const noexcept;

    /** Return `result` with a reference to the underlying `bool`

        If @ref is_bool() is `true`, the result contains a reference to the
        underlying `bool`, otherwise it contains an `error_code`.

        @par Example
        The return value can be used in both a boolean context and
        to assign a variable:
        @code
        if( auto r = jv.try_as_bool() )
            return *r;
        @endcode

        But can also be used to throw an exception on error:
        @code
        return jv.try_as_bool().value();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    system::result<bool&>
    try_as_bool() noexcept;

    /** Return `result` with the underlying `bool`

        If @ref is_bool() is `true`, the result contains a copy of the
        underlying `bool`, otherwise it contains an `error_code`.

        @par Example
        The return value can be used in both a boolean context and
        to assign a variable:
        @code
        if( auto r = jv.try_as_bool() )
            return *r;
        @endcode

        But can also be used to throw an exception on error:
        @code
        return jv.try_as_bool().value();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    system::result<bool>
    try_as_bool() const noexcept;

    /** Return engaged `result` if the `value` is null

        If @ref is_null() is `true`, the result is engaged, otherwise it
        contains an `error_code`.

        @par Example
        The return value can be used in both a boolean context and
        to assign a variable:
        @code
        if( auto r = jv.try_as_null() )
            return *r;
        @endcode

        But can also be used to throw an exception on error:
        @code
        return jv.try_as_null().value();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    system::result<std::nullptr_t>
    try_as_null() const noexcept;

    //------------------------------------------------------

    /** Return a reference to the underlying `object`, or throw an exception.

        If @ref is_object() is `true`, returns
        a reference to the underlying @ref object,
        otherwise throws an exception.

        @par Exception Safety
        Strong guarantee.

        @throw `boost::system::system_error` `! this->is_object()`.

        @param loc `source_location` to use in thrown exception; the source
            location of the call site by default.

        @par Complexity
        Constant.
    */
    /** @{ */
    object&
    as_object(source_location const& loc = BOOST_CURRENT_LOCATION) &
    {
        auto& self = const_cast<value const&>(*this);
        return const_cast<object&>( self.as_object(loc) );
    }

    object&&
    as_object(source_location const& loc = BOOST_CURRENT_LOCATION) &&
    {
        return std::move( as_object(loc) );
    }

    BOOST_JSON_DECL
    object const&
    as_object(source_location const& loc = BOOST_CURRENT_LOCATION) const&;
    /** @} */

    /** Return a reference to the underlying @ref array, or throw an exception.

        If @ref is_array() is `true`, returns
        a reference to the underlying @ref array,
        otherwise throws an exception.

        @par Exception Safety
        Strong guarantee.

        @throw `boost::system::system_error` `! this->is_array()`.

        @param loc `source_location` to use in thrown exception; the source
            location of the call site by default.

        @par Complexity
        Constant.
    */
    /** @{ */
    array&
    as_array(source_location const& loc = BOOST_CURRENT_LOCATION) &
    {
        auto& self = const_cast<value const&>(*this);
        return const_cast<array&>( self.as_array(loc) );
    }

    array&&
    as_array(source_location const& loc = BOOST_CURRENT_LOCATION) &&
    {
        return std::move( as_array(loc) );
    }

    BOOST_JSON_DECL
    array const&
    as_array(source_location const& loc = BOOST_CURRENT_LOCATION) const&;
    /** @} */

    /** Return a reference to the underlying `string`, or throw an exception.

        If @ref is_string() is `true`, returns
        a reference to the underlying @ref string,
        otherwise throws an exception.

        @par Exception Safety
        Strong guarantee.

        @throw `boost::system::system_error` `! this->is_string()`.

        @param loc `source_location` to use in thrown exception; the source
            location of the call site by default.

        @par Complexity
        Constant.
    */
    string&
    as_string(source_location const& loc = BOOST_CURRENT_LOCATION) &
    {
        auto& self = const_cast<value const&>(*this);
        return const_cast<string&>( self.as_string(loc) );
    }

    /** Return a reference to the underlying `string`, or throw an exception.

        If @ref is_string() is `true`, returns
        a reference to the underlying @ref string,
        otherwise throws an exception.

        @par Exception Safety
        Strong guarantee.

        @throw `boost::system::system_error` `! this->is_string()`.

        @param loc `source_location` to use in thrown exception; the source
            location of the call site by default.

        @par Complexity
        Constant.
    */
    /** @{ */
    string&&
    as_string(source_location const& loc = BOOST_CURRENT_LOCATION) &&
    {
        return std::move( as_string(loc) );
    }

    BOOST_JSON_DECL
    string const&
    as_string(source_location const& loc = BOOST_CURRENT_LOCATION) const&;

    BOOST_JSON_DECL
    std::int64_t&
    as_int64(source_location const& loc = BOOST_CURRENT_LOCATION);
    /** @} */

    /** Return the underlying `std::int64_t`, or throw an exception.

        If @ref is_int64() is `true`, returns
        the underlying `std::int64_t`,
        otherwise throws an exception.

        @par Exception Safety
        Strong guarantee.

        @throw `boost::system::system_error` `! this->is_int64()`.

        @param loc `source_location` to use in thrown exception; the source
            location of the call site by default.

        @par Complexity
        Constant.

        @par Note
        This function is the const-qualified overload of @ref as_int64, which
        is intended for direct access to the underlying object, __if__ it has
        the type `std::int64_t`. It does not convert the underlying object to
        type `std::int64_t` even if a lossless conversion is possible. If you
        are not sure which kind your `value` has, and you only care about
        getting a `std::int64_t` number, consider using @ref to_number instead.
    */
    BOOST_JSON_DECL
    std::int64_t
    as_int64(source_location const& loc = BOOST_CURRENT_LOCATION) const;

    /** Return a reference to the underlying `std::uint64_t`, or throw an exception.

        If @ref is_uint64() is `true`, returns
        a reference to the underlying `std::uint64_t`,
        otherwise throws an exception.

        @par Exception Safety
        Strong guarantee.

        @throw `boost::system::system_error` `! this->is_uint64()`.

        @param loc `source_location` to use in thrown exception; the source
            location of the call site by default.

        @par Complexity
        Constant.

        @par Note
        This function is intended for direct access to the underlying object,
        __if__ it has the type `std::uint64_t`. It does not convert the
        underlying object to type `std::uint64_t` even if a lossless conversion
        is possible. If you are not sure which kind your `value` has, and you
        only care about getting a `std::uint64_t` number, consider using
        @ref to_number instead.
    */
    BOOST_JSON_DECL
    std::uint64_t&
    as_uint64(source_location const& loc = BOOST_CURRENT_LOCATION);

    /** Return the underlying `std::uint64_t`, or throw an exception.

        If @ref is_uint64() is `true`, returns
        the underlying `std::uint64_t`,
        otherwise throws an exception.

        @par Exception Safety
        Strong guarantee.

        @throw `boost::system::system_error` `! this->is_uint64()`.

        @param loc `source_location` to use in thrown exception; the source
            location of the call site by default.

        @par Complexity
        Constant.

        @par Note
        This function is the const-qualified overload of @ref as_uint64, which
        is intended for direct access to the underlying object, __if__ it has
        the type `std::uint64_t`. It does not convert the underlying object to
        type `std::uint64_t` even if a lossless conversion is possible. If you
        are not sure which kind your `value` has, and you only care about
        getting a `std::uint64_t` number, consider using
        @ref to_number instead.
    */
    BOOST_JSON_DECL
    std::uint64_t
    as_uint64(source_location const& loc = BOOST_CURRENT_LOCATION) const;

    /** Return a reference to the underlying `double`, or throw an exception.

        If @ref is_double() is `true`, returns
        a reference to the underlying `double`,
        otherwise throws an exception.

        @par Exception Safety
        Strong guarantee.

        @throw `boost::system::system_error` `! this->is_double()`.

        @param loc `source_location` to use in thrown exception; the source
            location of the call site by default.

        @par Complexity
        Constant.

        @par Note
        This function is intended for direct access to the underlying object,
        __if__ it has the type `double`. It does not convert the underlying
        object to type `double` even if a lossless conversion is possible. If
        you are not sure which kind your `value` has, and you only care about
        getting a `double` number, consider using @ref to_number instead.
    */
    BOOST_JSON_DECL
    double&
    as_double(source_location const& loc = BOOST_CURRENT_LOCATION);

    /** Return the underlying `double`, or throw an exception.

        If @ref is_double() is `true`, returns
        the underlying `double`,
        otherwise throws an exception.

        @par Exception Safety
        Strong guarantee.

        @throw `boost::system::system_error` `! this->is_double()`.

        @param loc `source_location` to use in thrown exception; the source
            location of the call site by default.

        @par Complexity
        Constant.

        @par Note
        This function is the const-qualified overload of @ref as_double, which
        is intended for direct access to the underlying object, __if__ it has
        the type `double`. It does not convert the underlying object to type
        `double` even if a lossless conversion is possible. If you are not sure
        which kind your `value` has, and you only care about getting a `double`
        number, consider using @ref to_number instead.
    */
    BOOST_JSON_DECL
    double
    as_double(source_location const& loc = BOOST_CURRENT_LOCATION) const;

    /** Return a reference to the underlying `bool`, or throw an exception.

        If @ref is_bool() is `true`, returns
        a reference to the underlying `bool`,
        otherwise throws an exception.

        @par Exception Safety
        Strong guarantee.

        @throw `boost::system::system_error` `! this->is_bool()`.

        @param loc `source_location` to use in thrown exception; the source
            location of the call site by default.

        @par Complexity
        Constant.
    */
    BOOST_JSON_DECL
    bool&
    as_bool(source_location const& loc = BOOST_CURRENT_LOCATION);

    /** Return the underlying `bool`, or throw an exception.

        If @ref is_bool() is `true`, returns
        the underlying `bool`,
        otherwise throws an exception.

        @par Exception Safety
        Strong guarantee.

        @throw `boost::system::system_error` `! this->is_bool()`.

        @param loc `source_location` to use in thrown exception; the source
            location of the call site by default.

        @par Complexity
        Constant.
    */
    BOOST_JSON_DECL
    bool
    as_bool(source_location const& loc = BOOST_CURRENT_LOCATION) const;

    //------------------------------------------------------

    /** Return a reference to the underlying `object`, without checking.

        This is the fastest way to access the underlying
        representation when the kind is known in advance.

        @par Preconditions

        @code
        this->is_object()
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    /** @{ */
    object&
    get_object() & noexcept
    {
        BOOST_ASSERT(is_object());
        return obj_;
    }

    object&&
    get_object() && noexcept
    {
        BOOST_ASSERT(is_object());
        return std::move(obj_);
    }

    object const&
    get_object() const& noexcept
    {
        BOOST_ASSERT(is_object());
        return obj_;
    }
    /** @} */

    /** Return a reference to the underlying `array`, without checking.

        This is the fastest way to access the underlying
        representation when the kind is known in advance.

        @par Preconditions

        @code
        this->is_array()
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    /** @{ */
    array&
    get_array() & noexcept
    {
        BOOST_ASSERT(is_array());
        return arr_;
    }

    array&&
    get_array() && noexcept
    {
        BOOST_ASSERT(is_array());
        return std::move(arr_);
    }

    array const&
    get_array() const& noexcept
    {
        BOOST_ASSERT(is_array());
        return arr_;
    }
    /** @} */

    /** Return a reference to the underlying `string`, without checking.

        This is the fastest way to access the underlying
        representation when the kind is known in advance.

        @par Preconditions

        @code
        this->is_string()
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    /** @{ */
    string&
    get_string() & noexcept
    {
        BOOST_ASSERT(is_string());
        return str_;
    }

    string&&
    get_string() && noexcept
    {
        BOOST_ASSERT(is_string());
        return std::move(str_);
    }

    string const&
    get_string() const& noexcept
    {
        BOOST_ASSERT(is_string());
        return str_;
    }
    /** @} */

    /** Return a reference to the underlying `std::int64_t`, without checking.

        This is the fastest way to access the underlying
        representation when the kind is known in advance.

        @par Preconditions

        @code
        this->is_int64()
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    std::int64_t&
    get_int64() noexcept
    {
        BOOST_ASSERT(is_int64());
        return sca_.i;
    }

    /** Return the underlying `std::int64_t`, without checking.

        This is the fastest way to access the underlying
        representation when the kind is known in advance.

        @par Preconditions

        @code
        this->is_int64()
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    std::int64_t
    get_int64() const noexcept
    {
        BOOST_ASSERT(is_int64());
        return sca_.i;
    }

    /** Return a reference to the underlying `std::uint64_t`, without checking.

        This is the fastest way to access the underlying
        representation when the kind is known in advance.

        @par Preconditions

        @code
        this->is_uint64()
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    std::uint64_t&
    get_uint64() noexcept
    {
        BOOST_ASSERT(is_uint64());
        return sca_.u;
    }

    /** Return the underlying `std::uint64_t`, without checking.

        This is the fastest way to access the underlying
        representation when the kind is known in advance.

        @par Preconditions

        @code
        this->is_uint64()
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    std::uint64_t
    get_uint64() const noexcept
    {
        BOOST_ASSERT(is_uint64());
        return sca_.u;
    }

    /** Return a reference to the underlying `double`, without checking.

        This is the fastest way to access the underlying
        representation when the kind is known in advance.

        @par Preconditions

        @code
        this->is_double()
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    double&
    get_double() noexcept
    {
        BOOST_ASSERT(is_double());
        return sca_.d;
    }

    /** Return the underlying `double`, without checking.

        This is the fastest way to access the underlying
        representation when the kind is known in advance.

        @par Preconditions

        @code
        this->is_double()
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    double
    get_double() const noexcept
    {
        BOOST_ASSERT(is_double());
        return sca_.d;
    }

    /** Return a reference to the underlying `bool`, without checking.

        This is the fastest way to access the underlying
        representation when the kind is known in advance.

        @par Preconditions

        @code
        this->is_bool()
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool&
    get_bool() noexcept
    {
        BOOST_ASSERT(is_bool());
        return sca_.b;
    }

    /** Return the underlying `bool`, without checking.

        This is the fastest way to access the underlying
        representation when the kind is known in advance.

        @par Preconditions

        @code
        this->is_bool()
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool
    get_bool() const noexcept
    {
        BOOST_ASSERT(is_bool());
        return sca_.b;
    }

    //------------------------------------------------------

    /** Access an element, with bounds checking.

        Returns `boost::system::result` containing a reference to the element
        of the underlying object, if `pos` is within its range. If `pos` is
        outside of that range, or the underlying value is not an object the
        result contains an `error_code`.

        @par Exception Safety
        No-throw guarantee.

        @param key The key of the element to find.

        @par Complexity
        Constant.
    */
    /** @{ */
    BOOST_JSON_DECL
    boost::system::result<value&>
    try_at(string_view key) noexcept;

    BOOST_JSON_DECL
    boost::system::result<value const&>
    try_at(string_view key) const noexcept;
    /** @} */

    /** Access an element, with bounds checking.

        This function is used to access elements of
        the underlying object, or throw an exception
        if the value is not an object.

        @par Complexity
        Constant.

        @par Exception Safety
        Strong guarantee.

        @param key The key of the element to find.

        @param loc `source_location` to use in thrown exception; the source
            location of the call site by default.

        @return `this->as_object(loc).at( key, loc )`.
    */
    /** @{ */
    value&
    at(string_view key, source_location const& loc = BOOST_CURRENT_LOCATION) &
    {
        return as_object(loc).at(key, loc);
    }

    value&&
    at(string_view key, source_location const& loc = BOOST_CURRENT_LOCATION) &&
    {
        return std::move( as_object(loc) ).at(key, loc);
    }

    value const&
    at(
        string_view key,
        source_location const& loc = BOOST_CURRENT_LOCATION) const&
    {
        return as_object(loc).at(key, loc);
    }
    /** @} */

    /** Access an element, with bounds checking.

        Returns `boost::system::result` containing a reference to the element
        of the underlying array, if `pos` is within its range. If `pos` is
        outside of that range, or the underlying value is not an array the
        result contains an `error_code`.

        @par Exception Safety
        No-throw guarantee.

        @param pos A zero-based array index.

        @par Complexity
        Constant.
    */
    /** @{ */
    BOOST_JSON_DECL
    boost::system::result<value&>
    try_at(std::size_t pos) noexcept;

    BOOST_JSON_DECL
    boost::system::result<value const&>
    try_at(std::size_t pos) const noexcept;
    /** @} */

    /** Access an element, with bounds checking.

        This function is used to access elements of
        the underlying array, or throw an exception
        if the value is not an array.

        @par Complexity
        Constant.

        @par Exception Safety
        Strong guarantee.

        @param pos A zero-based array index.

        @param loc `source_location` to use in thrown exception; the source
            location of the call site by default.

        @return `this->as_array(loc).at( pos, loc )`.
    */
    /** @{ */
    value &
    at(std::size_t pos, source_location const& loc = BOOST_CURRENT_LOCATION) &
    {
        return as_array(loc).at(pos, loc);
    }

    value&&
    at(std::size_t pos, source_location const& loc = BOOST_CURRENT_LOCATION) &&
    {
        return std::move( as_array(loc) ).at(pos, loc);
    }

    value const&
    at(
        std::size_t pos,
        source_location const& loc = BOOST_CURRENT_LOCATION) const&
    {
        return as_array(loc).at(pos, loc);
    }
    /** @} */

    /** Access an element via JSON Pointer.

        This function is used to access a (potentially nested) element of the
        value using a JSON Pointer string.

        @par Complexity
        Linear in the sizes of `ptr` and underlying array, object, or string.

        @par Exception Safety
        No-throw guarantee.

        @param ptr JSON Pointer string.

        @return `boost::system::result<value&>` containing either a reference
            to the element identified by `ptr` or a corresponding `error_code`.

        @see
            [RFC 6901 - JavaScript Object Notation (JSON) Pointer](https://datatracker.ietf.org/doc/html/rfc6901).
    */
    BOOST_JSON_DECL
    system::result<value const&>
    try_at_pointer(string_view ptr) const noexcept;

    /** Access an element via JSON Pointer.

        This function is used to access a (potentially nested) element of the
        value using a JSON Pointer string.

        @par Complexity
        Linear in the sizes of `ptr` and underlying array, object, or string.

        @par Exception Safety
        No-throw guarantee.

        @param ptr JSON Pointer string.

        @return `boost::system::result<value const&>` containing either a
            reference to the element identified by `ptr` or a corresponding
            `error_code`.

        @see
            [RFC 6901 - JavaScript Object Notation (JSON) Pointer](https://datatracker.ietf.org/doc/html/rfc6901).
    */
    BOOST_JSON_DECL
    system::result<value&>
    try_at_pointer(string_view ptr) noexcept;

    /** Access an element via JSON Pointer.

        This function is used to access a (potentially nested)
        element of the value using a JSON Pointer string.

        @par Complexity
        Linear in the sizes of `ptr` and underlying array, object, or string.

        @par Exception Safety
        Strong guarantee.

        @param ptr JSON Pointer string.

        @param loc `source_location` to use in thrown exception; the source
            location of the call site by default.

        @return reference to the element identified by `ptr`.

        @throw `boost::system::system_error` if an error occurs.

        @see
        <a href="https://datatracker.ietf.org/doc/html/rfc6901">
            RFC 6901 - JavaScript Object Notation (JSON) Pointer</a>
    */
    /** @{ */
    BOOST_JSON_DECL
    value const&
    at_pointer(
        string_view ptr,
        source_location const& loc = BOOST_CURRENT_LOCATION) const&;

    inline
    value&&
    at_pointer(
        string_view ptr,
        source_location const& loc = BOOST_CURRENT_LOCATION) &&;

    inline
    value&
    at_pointer(
        string_view ptr,
        source_location const& loc = BOOST_CURRENT_LOCATION) &;
    /** @} */

    /** Access an element via JSON Pointer.

        This function is used to access a (potentially nested)
        element of the value using a JSON Pointer string.

        @par Complexity
        Linear in the sizes of `ptr` and underlying array, object, or string.

        @par Exception Safety
        No-throw guarantee.

        @param ptr JSON Pointer string.

        @param ec Set to the error, if any occurred.

        @return pointer to the element identified by `ptr`.

        @see
        <a href="https://datatracker.ietf.org/doc/html/rfc6901">
            RFC 6901 - JavaScript Object Notation (JSON) Pointer</a>
    */
    /** @{ */
    BOOST_JSON_DECL
    value const*
    find_pointer(string_view ptr, system::error_code& ec) const noexcept;

    BOOST_JSON_DECL
    value*
    find_pointer(string_view ptr, system::error_code& ec) noexcept;

    BOOST_JSON_DECL
    value const*
    find_pointer(string_view ptr, std::error_code& ec) const noexcept;

    BOOST_JSON_DECL
    value*
    find_pointer(string_view ptr, std::error_code& ec) noexcept;
    /** @} */

    //------------------------------------------------------

    /** Set an element via JSON Pointer.

        This function is used to insert or assign to a potentially nested
        element of the value using a JSON Pointer string. The function may
        create intermediate elements corresponding to pointer segments.
        <br/>

        The particular conditions when and what kind of intermediate element
        is created is governed by the `ptr` parameter.

        Each pointer token is considered in sequence. For each token

        - if the containing value is an @ref object, then a new `null`
          element is created with key equal to unescaped token string;
          otherwise

        - if the containing value is an @ref array, and the token represents a
          past-the-end marker, then a `null` element is appended to the array;
          otherwise

        - if the containing value is an @ref array, and the token represents a
          number, then if the difference between the number and array's size
          is smaller than `opts.max_created_elements`, then the size of the
          array is increased, so that the number can reference an element in the
          array; otherwise

        - if the containing value is of different @ref kind and
          `opts.replace_any_scalar` is `true`, or the value is `null`, then

           - if `opts.create_arrays` is `true` and the token either represents
             past-the-end marker or a number, then the value is replaced with
             an empty array and the token is considered again; otherwise

           - if `opts.create_objects` is `true`, then the value is replaced
             with an empty object and the token is considered again; otherwise

        - an error is produced.

        @par Complexity
        Linear in the sum of size of `ptr`, size of underlying array, object,
        or string and `opts.max_created_elements`.

        @par Exception Safety
        Basic guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param sv JSON Pointer string.

        @param ref The value to assign to pointed element.

        @param opts The options for the algorithm.

        @return `boost::json::result<value&>` containing either a reference to
            the element identified by `ptr` or a corresponding `error_code`.

        @see
            @ref set_pointer_options,
            [RFC 6901 - JavaScript Object Notation (JSON) Pointer](https://datatracker.ietf.org/doc/html/rfc6901).
    */
    BOOST_JSON_DECL
    system::result<value&>
    try_set_at_pointer(
        string_view sv,
        value_ref ref,
        set_pointer_options const& opts = {} );

    /** Set an element via JSON Pointer.

        This function is used to insert or assign to a potentially nested
        element of the value using a JSON Pointer string. The function may
        create intermediate elements corresponding to pointer segments.
        <br/>

        The particular conditions when and what kind of intermediate element
        is created is governed by the `ptr` parameter.

        Each pointer token is considered in sequence. For each token

        - if the containing value is an @ref object, then a new `null`
        element is created with key equal to unescaped token string; otherwise

        - if the containing value is an @ref array, and the token represents a
        past-the-end marker, then a `null` element is appended to the array;
        otherwise

        - if the containing value is an @ref array, and the token represents a
        number, then if the difference between the number and array's size
        is smaller than `opts.max_created_elements`, then the size of the
        array is increased, so that the number can reference an element in the
        array; otherwise

        - if the containing value is of different @ref kind and
          `opts.replace_any_scalar` is `true`, or the value is `null`, then

           - if `opts.create_arrays` is `true` and the token either represents
             past-the-end marker or a number, then the value is replaced with
             an empty array and the token is considered again; otherwise

           - if `opts.create_objects` is `true`, then the value is replaced
             with an empty object and the token is considered again; otherwise

        - an error is produced.

        @par Complexity
        Linear in the sum of size of `ptr`, size of underlying array, object,
        or string and `opts.max_created_elements`.

        @par Exception Safety
        Basic guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param sv JSON Pointer string.

        @param ref The value to assign to pointed element.

        @param opts The options for the algorithm.

        @return Reference to the element identified by `ptr`.

        @see @ref set_pointer_options,
        <a href="https://datatracker.ietf.org/doc/html/rfc6901">
            RFC 6901 - JavaScript Object Notation (JSON) Pointer</a>.
    */
    BOOST_JSON_DECL
    value&
    set_at_pointer(
        string_view sv,
        value_ref ref,
        set_pointer_options const& opts = {} );

    /** Set an element via JSON Pointer.

        This function is used to insert or assign to a potentially nested
        element of the value using a JSON Pointer string. The function may
        create intermediate elements corresponding to pointer segments.
        <br/>

        The particular conditions when and what kind of intermediate element
        is created is governed by the `ptr` parameter.

        Each pointer token is considered in sequence. For each token

        - if the containing value is an @ref object, then a new `null`
          element is created with key equal to unescaped token string;
          otherwise

        - if the containing value is an @ref array, and the token represents a
          past-the-end marker, then a `null` element is appended to the array;
          otherwise

        - if the containing value is an @ref array, and the token represents a
          number, then if the difference between the number and array's size
          is smaller than `opts.max_created_elements`, then the size of the
          array is increased, so that the number can reference an element in the
          array; otherwise

        - if the containing value is of different @ref kind and
          `opts.replace_any_scalar` is `true`, or the value is `null`, then

           - if `opts.create_arrays` is `true` and the token either represents
             past-the-end marker or a number, then the value is replaced with
             an empty array and the token is considered again; otherwise

           - if `opts.create_objects` is `true`, then the value is replaced
             with an empty object and the token is considered again; otherwise

        - an error is produced.

        @par Complexity
        Linear in the sum of size of `ptr`, size of underlying array, object,
        or string and `opts.max_created_elements`.

        @par Exception Safety
        Basic guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param sv JSON Pointer string.

        @param ref The value to assign to pointed element.

        @param ec Set to the error, if any occurred.

        @param opts The options for the algorithm.

        @return Pointer to the element identified by `ptr`.

        @see @ref set_pointer_options,
        <a href="https://datatracker.ietf.org/doc/html/rfc6901">
            RFC 6901 - JavaScript Object Notation (JSON) Pointer</a>.
    */
    /** @{ */
    BOOST_JSON_DECL
    value*
    set_at_pointer(
        string_view sv,
        value_ref ref,
        system::error_code& ec,
        set_pointer_options const& opts = {} );

    BOOST_JSON_DECL
    value*
    set_at_pointer(
        string_view sv,
        value_ref ref,
        std::error_code& ec,
        set_pointer_options const& opts = {} );
    /** @} */

    //------------------------------------------------------

    /** Return `true` if two values are equal.

        Two values are equal when they are the
        same kind and their referenced values
        are equal, or when they are both integral
        types and their integral representations
        are equal.

        @par Complexity
        Constant or linear in the size of
        the array, object, or string.

        @par Exception Safety
        No-throw guarantee.
    */
    // inline friend speeds up overload resolution
    friend
    bool
    operator==(
        value const& lhs,
        value const& rhs) noexcept
    {
        return lhs.equal(rhs);
    }

    /** Return `true` if two values are not equal.

        Two values are equal when they are the
        same kind and their referenced values
        are equal, or when they are both integral
        types and their integral representations
        are equal.

        @par Complexity
        Constant or linear in the size of
        the array, object, or string.

        @par Exception Safety
        No-throw guarantee.
    */
    friend
    bool
    operator!=(
        value const& lhs,
        value const& rhs) noexcept
    {
        return ! (lhs == rhs);
    }

    /** Serialize @ref value to an output stream.

        This function serializes a `value` as JSON into the output stream.

        @return Reference to `os`.

        @par Complexity
        Constant or linear in the size of `jv`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param os The output stream to serialize to.

        @param jv The value to serialize.
    */
    BOOST_JSON_DECL
    friend
    std::ostream&
    operator<<(
        std::ostream& os,
        value const& jv);

    /** Parse @ref value from an input stream.

        This function parses JSON from an input stream into a `value`. If
        parsing fails, `std::ios_base::failbit` will be set for `is` and
        `jv` will be left unchanged. Regardless of whether `skipws` flag is set
        on `is`, consumes whitespace before and after JSON, because whitespace
        is considered a part of JSON. Behaves as
        [_FormattedInputFunction_](https://en.cppreference.com/w/cpp/named_req/FormattedInputFunction).

        Note: this operator cannot assume that the stream only contains a
        single JSON document, which may result in **very underwhelming
        performance**, if the stream isn't cooperative. If you know that your
        input consists of a single JSON document, consider using @ref parse
        function instead.

        @return Reference to `is`.

        @par Complexity
        Linear in the size of JSON data.

        @par Exception Safety
        Basic guarantee.
        Calls to `memory_resource::allocate` may throw.
        The stream may throw as configured by
        [`std::ios::exceptions`](https://en.cppreference.com/w/cpp/io/basic_ios/exceptions).

        @param is The input stream to parse from.

        @param jv The value to parse into.

        @see @ref parse.
    */
    BOOST_JSON_DECL
    friend
    std::istream&
    operator>>(
        std::istream& is,
        value& jv);

    /** Helper for `boost::hash` support

        Computes a hash value for `jv`. This function is used by
        `boost::hash<value>`. Similar overloads for @ref array, @ref object,
        and @ref string do not exist, because those types are supported by
        `boost::hash` out of the box.

        @return hash value for `jv`.

        @param jv `value` for which a hash is to be computed.

        @see [Boost.ContainerHash](https://boost.org/libs/container_hash).
     */
#ifndef BOOST_JSON_DOCS
    template<
        class T,
        typename std::enable_if<
            std::is_same< detail::remove_cvref<T>, value >::value >::type*
                = nullptr>
    friend
    std::size_t
    hash_value( T const& jv ) noexcept
#else
    friend
    inline
    std::size_t
    hash_value( value const& jv ) noexcept
#endif
    {
        return detail::hash_value_impl(jv);
    }

private:
    static
    void
    relocate(
        value* dest,
        value const& src) noexcept
    {
        std::memcpy(
            static_cast<void*>(dest),
            &src,
            sizeof(src));
    }

    BOOST_JSON_DECL
    storage_ptr
    destroy() noexcept;

    BOOST_JSON_DECL
    bool
    equal(value const& other) const noexcept;

    template<class T>
    auto
    to_number(error& e) const noexcept ->
        typename std::enable_if<
            std::is_signed<T>::value &&
            ! std::is_floating_point<T>::value,
                T>::type
    {
        if(sca_.k == json::kind::int64)
        {
            auto const i = sca_.i;
            if( i >= (std::numeric_limits<T>::min)() &&
                i <= (std::numeric_limits<T>::max)())
            {
                e = {};
                return static_cast<T>(i);
            }
            e = error::not_exact;
        }
        else if(sca_.k == json::kind::uint64)
        {
            auto const u = sca_.u;
            if(u <= static_cast<std::uint64_t>((
                std::numeric_limits<T>::max)()))
            {
                e = {};
                return static_cast<T>(u);
            }
            e = error::not_exact;
        }
        else if(sca_.k == json::kind::double_)
        {
            auto const d = sca_.d;
            if( d >= static_cast<double>(
                    (detail::to_number_limit<T>::min)()) &&
                d <= static_cast<double>(
                    (detail::to_number_limit<T>::max)()) &&
                static_cast<T>(d) == d)
            {
                e = {};
                return static_cast<T>(d);
            }
            e = error::not_exact;
        }
        else
        {
            e = error::not_number;
        }
        return T{};
    }

    template<class T>
    auto
    to_number(error& e) const noexcept ->
        typename std::enable_if<
            std::is_unsigned<T>::value &&
            ! std::is_same<T, bool>::value,
                T>::type
    {
        if(sca_.k == json::kind::int64)
        {
            auto const i = sca_.i;
            if( i >= 0 && static_cast<std::uint64_t>(i) <=
                (std::numeric_limits<T>::max)())
            {
                e = {};
                return static_cast<T>(i);
            }
            e = error::not_exact;
        }
        else if(sca_.k == json::kind::uint64)
        {
            auto const u = sca_.u;
            if(u <= (std::numeric_limits<T>::max)())
            {
                e = {};
                return static_cast<T>(u);
            }
            e = error::not_exact;
        }
        else if(sca_.k == json::kind::double_)
        {
            auto const d = sca_.d;
            if( d >= 0 &&
                d <= (detail::to_number_limit<T>::max)() &&
                static_cast<T>(d) == d)
            {
                e = {};
                return static_cast<T>(d);
            }
            e = error::not_exact;
        }
        else
        {
            e = error::not_number;
        }
        return T{};
    }

    template<class T>
    auto
    to_number(error& e) const noexcept ->
        typename std::enable_if<
            std::is_floating_point<
                T>::value, T>::type
    {
        if(sca_.k == json::kind::int64)
        {
            e = {};
            return static_cast<T>(sca_.i);
        }
        if(sca_.k == json::kind::uint64)
        {
            e = {};
            return static_cast<T>(sca_.u);
        }
        if(sca_.k == json::kind::double_)
        {
            e = {};
            return static_cast<T>(sca_.d);
        }
        e = error::not_number;
        return {};
    }
};

// Make sure things are as big as we think they should be
#if BOOST_JSON_ARCH == 64
BOOST_STATIC_ASSERT(sizeof(value) == 24);
#elif BOOST_JSON_ARCH == 32
BOOST_STATIC_ASSERT(sizeof(value) == 16);
#else
# error Unknown architecture
#endif

//----------------------------------------------------------

/** A key/value pair.

    This is the type of element used by the @ref object
    container.
*/
class key_value_pair
{
#ifndef BOOST_JSON_DOCS
    friend struct detail::access;
    using access = detail::access;
#endif

    BOOST_JSON_DECL
    static char const empty_[1];

    inline
    key_value_pair(
        pilfered<json::value> k,
        pilfered<json::value> v) noexcept;

public:
    /// Copy assignment (deleted).
    key_value_pair&
    operator=(key_value_pair const&) = delete;

    /** Destructor.

        The value is destroyed and all internally
        allocated memory is freed.
    */
    ~key_value_pair() noexcept
    {
        auto const& sp = value_.storage();
        if(sp.is_not_shared_and_deallocate_is_trivial())
            return;
        if(key_ == empty_)
            return;
        sp->deallocate(const_cast<char*>(key_),
            len_ + 1, alignof(char));
    }

    /** Copy constructor.

        This constructs a key/value pair with a
        copy of another key/value pair, using
        the same memory resource as `other`.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param other The key/value pair to copy.
    */
    key_value_pair(
        key_value_pair const& other)
        : key_value_pair(other,
            other.storage())
    {
    }

    /** Copy constructor.

        This constructs a key/value pair with a
        copy of another key/value pair, using
        the specified memory resource.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param other The key/value pair to copy.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The element will acquire shared ownership of the memory resource.
    */
    BOOST_JSON_DECL
    key_value_pair(
        key_value_pair const& other,
        storage_ptr sp);

    /** Move constructor.

        The pair is constructed by acquiring
        ownership of the contents of `other` and
        shared ownership of `other`'s memory resource.

        @note

        After construction, the moved-from pair holds an
        empty key, and a null value with its current
        storage pointer.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param other The pair to move.
    */
    key_value_pair(
        key_value_pair&& other) noexcept
        : value_(std::move(other.value_))
        , key_(detail::exchange(
            other.key_, empty_))
        , len_(detail::exchange(
            other.len_, 0))
    {
    }

    /** Pilfer constructor.

        The pair is constructed by acquiring ownership
        of the contents of `other` using pilfer semantics.
        This is more efficient than move construction, when
        it is known that the moved-from object will be
        immediately destroyed afterwards.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param other The value to pilfer. After pilfer
        construction, `other` is not in a usable state
        and may only be destroyed.

        @see @ref pilfer,
            <a href="http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0308r0.html">
                Valueless Variants Considered Harmful</a>
    */
    key_value_pair(
        pilfered<key_value_pair> other) noexcept
        : value_(pilfer(other.get().value_))
        , key_(detail::exchange(
            other.get().key_, empty_))
        , len_(detail::exchange(
            other.get().len_, 0))
    {
    }

    /** Constructor.

        This constructs a key/value pair.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param key The key string to use.

        @param args Optional arguments forwarded to
        the @ref value constructor.
    */
    template<class... Args>
    explicit
    key_value_pair(
        string_view key,
        Args&&... args)
        : value_(std::forward<Args>(args)...)
    {
        if(key.size() > string::max_size())
        {
            BOOST_STATIC_CONSTEXPR source_location loc = BOOST_CURRENT_LOCATION;
            detail::throw_system_error( error::key_too_large, &loc );
        }
        auto s = reinterpret_cast<
            char*>(value_.storage()->
                allocate(key.size() + 1, alignof(char)));
        std::memcpy(s, key.data(), key.size());
        s[key.size()] = 0;
        key_ = s;
        len_ = static_cast<
            std::uint32_t>(key.size());
    }

    /** Constructor.

        This constructs a key/value pair. A
        copy of the specified value is made,
        using the specified memory resource.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param p A `std::pair` with the key
            string and @ref value to construct with.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The element will acquire shared ownership of the memory resource.
    */
    explicit
    key_value_pair(
        std::pair<
            string_view,
            json::value> const& p,
        storage_ptr sp = {})
        : key_value_pair(
            p.first,
            p.second,
            std::move(sp))
    {
    }

    /** Constructor.

        This constructs a key/value pair.
        Ownership of the specified value is
        transferred by move construction.

        @par Exception Safety
        Strong guarantee.
        Calls to `memory_resource::allocate` may throw.

        @param p A `std::pair` with the key
            string and @ref value to construct with.

        @param sp A pointer to the `boost::container::pmr::memory_resource` to
        use. The element will acquire shared ownership of the memory resource.
    */
    explicit
    key_value_pair(
        std::pair<
            string_view,
            json::value>&& p,
        storage_ptr sp = {})
        : key_value_pair(
            p.first,
            std::move(p).second,
            std::move(sp))
    {
    }

    /** Return the associated memory resource.

        This returns a pointer to the memory
        resource used to construct the value.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    storage_ptr const&
    storage() const noexcept
    {
        return value_.storage();
    }

    /** Return the key of this element.

        After construction, the key may
        not be modified.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    string_view const
    key() const noexcept
    {
        return { key_, len_ };
    }

    /** Return the key of this element as a null-terminated string.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    char const*
    key_c_str() const noexcept
    {
        return key_;
    }

    /** Return the value of this element.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    /** @{ */
    json::value const&
    value() const& noexcept
    {
        return value_;
    }

    json::value&&
    value() && noexcept
    {
        return std::move( value() );
    }

    json::value&
    value() & noexcept
    {
        return value_;
    }
    /** @} */

private:
    json::value value_;
    char const* key_;
    std::uint32_t len_;
    std::uint32_t next_;
};

//----------------------------------------------------------

#ifdef BOOST_JSON_DOCS

/** Tuple-like element access.

    This overload permits the key and value
    of a `key_value_pair` to be accessed
    by index. For example:

    @code

    key_value_pair kvp("num", 42);

    string_view key = get<0>(kvp);
    value& jv = get<1>(kvp);

    @endcode

    @par Structured Bindings

    When using C++17 or greater, objects of type
    @ref key_value_pair may be used to initialize
    structured bindings:

    @code

    key_value_pair kvp("num", 42);

    auto& [key, value] = kvp;

    @endcode

    Depending on the value of `I`, the return type will be:

    @li `string_view const` if `I == 0`, or

    @li `value&`, `value const&`, or `value&&` if `I == 1`.

    Any other value for `I` is ill-formed.

    @tparam I The element index to access.

    @par Constraints

    `std::is_same_v< std::remove_cvref_t<T>, key_value_pair >`

    @return `kvp.key()` if `I == 0`, or `kvp.value()`
    if `I == 1`.

    @param kvp The @ref key_value_pair object
    to access.
*/
template<
    std::size_t I,
    class T>
__see_below__
get(T&& kvp) noexcept;

#else

template<std::size_t I>
auto
get(key_value_pair const&) noexcept ->
    typename std::conditional<I == 0,
        string_view const,
        value const&>::type
{
    static_assert(I == 0,
        "key_value_pair index out of range");
}

template<std::size_t I>
auto
get(key_value_pair&) noexcept ->
    typename std::conditional<I == 0,
        string_view const,
        value&>::type
{
    static_assert(I == 0,
        "key_value_pair index out of range");
}

template<std::size_t I>
auto
get(key_value_pair&&) noexcept ->
    typename std::conditional<I == 0,
        string_view const,
        value&&>::type
{
    static_assert(I == 0,
        "key_value_pair index out of range");
}

/** Extracts a key_value_pair's key using tuple-like interface
*/
template<>
inline
string_view const
get<0>(key_value_pair const& kvp) noexcept
{
    return kvp.key();
}

/** Extracts a key_value_pair's key using tuple-like interface
*/
template<>
inline
string_view const
get<0>(key_value_pair& kvp) noexcept
{
    return kvp.key();
}

/** Extracts a key_value_pair's key using tuple-like interface
*/
template<>
inline
string_view const
get<0>(key_value_pair&& kvp) noexcept
{
    return kvp.key();
}

/** Extracts a key_value_pair's value using tuple-like interface
*/
template<>
inline
value const&
get<1>(key_value_pair const& kvp) noexcept
{
    return kvp.value();
}

/** Extracts a key_value_pair's value using tuple-like interface
*/
template<>
inline
value&
get<1>(key_value_pair& kvp) noexcept
{
    return kvp.value();
}

/** Extracts a key_value_pair's value using tuple-like interface
*/
template<>
inline
value&&
get<1>(key_value_pair&& kvp) noexcept
{
    return std::move(kvp.value());
}

#endif

} // namespace json
} // namespace boost

#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wmismatched-tags"
#endif

#ifndef BOOST_JSON_DOCS

namespace std {

/** Tuple-like size access for key_value_pair
*/
template<>
struct tuple_size< ::boost::json::key_value_pair >
    : std::integral_constant<std::size_t, 2>
{
};

/** Tuple-like access for the key type of key_value_pair
*/
template<>
struct tuple_element<0, ::boost::json::key_value_pair>
{
    using type = ::boost::json::string_view const;
};

/** Tuple-like access for the value type of key_value_pair
*/
template<>
struct tuple_element<1, ::boost::json::key_value_pair>
{
    using type = ::boost::json::value&;
};

/** Tuple-like access for the value type of key_value_pair
*/
template<>
struct tuple_element<1, ::boost::json::key_value_pair const>
{
    using type = ::boost::json::value const&;
};

} // std

#endif

// std::hash specialization
#ifndef BOOST_JSON_DOCS
namespace std {
template <>
struct hash< ::boost::json::value > {
    BOOST_JSON_DECL
    std::size_t
    operator()(::boost::json::value const& jv) const noexcept;
};
} // std
#endif


#ifdef __clang__
# pragma clang diagnostic pop
#endif

// These are here because value, array,
// and object form cyclic references.

#include <boost/json/detail/impl/array.hpp>
#include <boost/json/impl/array.hpp>
#include <boost/json/impl/object.hpp>
#include <boost/json/impl/value.hpp>

// These must come after array and object
#include <boost/json/impl/value_ref.hpp>

#endif

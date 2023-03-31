#ifndef ETL_TYPE_TRAITS_H
#define ETL_TYPE_TRAITS_H

#include <cstddef>
#include <cstdint>

namespace Project::etl {

    /// integral constant
    template <typename T, const T VALUE>
    struct integral_constant {
        typedef T value_type;
        typedef integral_constant<T, VALUE> type;
        static const T value = VALUE;
    };
    template <typename T, const T VALUE> const T integral_constant<T, VALUE>::value;

    /// bool constant
    typedef integral_constant<bool, false> false_type;
    typedef integral_constant<bool, true>  true_type;
    template <bool B> using bool_constant = integral_constant<bool, B>;

    /// is_lvalue_reference
    template <typename T> struct is_lvalue_reference : false_type {};
    template <typename T> struct is_lvalue_reference<T&> : true_type {};
    template <typename T> inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

    /// is_rvalue_reference
    template <typename T> struct is_rvalue_reference : false_type {};
    template <typename T> struct is_rvalue_reference<T&&> : true_type {};
    template <typename T> inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

    /// is_reference
    template <typename T> struct is_reference : integral_constant<bool, is_lvalue_reference_v<T> || is_rvalue_reference_v<T>> {};
    template <typename T> inline constexpr bool is_reference_v = is_reference<T>::value;

    /// remove reference
    template <typename T> struct remove_reference       { typedef T type; };
    template <typename T> struct remove_reference<T&>   { typedef T type; };
    template <typename T> struct remove_reference<T&&>  { typedef T type; };
    template <typename T> using remove_reference_t = typename remove_reference<T>::type;

    /// is_pointer
    template <typename T> struct is_pointer     : false_type {};
    template <typename T> struct is_pointer<T*> : true_type {};
    template <typename T> inline constexpr bool is_pointer_v = is_pointer<T>::value;

    /// remove_pointer
    template <typename T> struct remove_pointer                          { typedef T type; };
    template <typename T> struct remove_pointer<T*>                      { typedef T type; };
    template <typename T> struct remove_pointer<const T*>                { typedef const T type; };
    template <typename T> struct remove_pointer<volatile T*>             { typedef volatile T type; };
    template <typename T> struct remove_pointer<const volatile T*>       { typedef const volatile T type; };
    template <typename T> struct remove_pointer<T* const>                { typedef T type; };
    template <typename T> struct remove_pointer<const T* const>          { typedef const T type; };
    template <typename T> struct remove_pointer<volatile T* const>       { typedef volatile T type; };
    template <typename T> struct remove_pointer<const volatile T* const> { typedef const volatile T type; };
    template <typename T> using remove_pointer_t = typename remove_pointer<T>::type;

    /// add_pointer
    template <typename T> struct add_pointer { typedef typename remove_reference<T>::type* type; };
    template <typename T> using add_pointer_t = typename add_pointer<T>::type;

    /// is_const
    template <typename T> struct is_const : false_type {};
    template <typename T> struct is_const<const T> : true_type {};
    template <typename T> struct is_const<const volatile T> : true_type {};
    template <typename T> inline constexpr bool is_const_v = is_const<T>::value;

    /// remove_const
    template <typename T> struct remove_const { typedef T type; };
    template <typename T> struct remove_const<const T> { typedef T type; };
    template <typename T> using remove_const_t = typename remove_const<T>::type;

    /// add_const
    template <typename T> struct add_const { typedef const T type; };
    template <typename T> struct add_const<const T> { typedef const T type; };

    /// is_volatile
    template <typename T> struct is_volatile : false_type {};
    template <typename T> struct is_volatile<volatile T> : true_type {};
    template <typename T> struct is_volatile<const volatile T> : true_type {};
    template <typename T> inline constexpr bool is_volatile_v = is_volatile<T>::value;

    /// remove_volatile
    template <typename T> struct remove_volatile { typedef T type; };
    template <typename T> struct remove_volatile<volatile T> { typedef T type; };
    template <typename T> using remove_volatile_t = typename remove_volatile<T>::type;
    template <typename T> using add_const_t = typename add_const<T>::type;

    /// add_volatile
    template <typename T> struct add_volatile { typedef volatile T type; };
    template <typename T> struct add_volatile<volatile T> { typedef volatile T type; };
    template <typename T> using add_volatile_t = typename add_volatile<T>::type;

    /// remove_const_volatile
    template <typename T> struct remove_const_volatile { typedef remove_volatile_t<remove_const_t<T>> type; };
    template <typename T> using remove_const_volatile_t = typename remove_const_volatile<T>::type;

    /// add_const_volatile
    template <typename T> struct add_const_volatile { typedef add_volatile_t<add_const_t<T>> type; };
    template <typename T> using add_const_volatile_t = typename add_const_volatile<T>::type;

    /// remove_const_volatile_ref
    template <typename T> struct remove_const_volatile_ref { typedef remove_const_volatile_t<remove_reference_t<T>> type; };
    template <typename T> using remove_const_volatile_ref_t = typename remove_const_volatile_ref<T>::type;

    /// enable_if
    template <bool B, typename T = void> struct enable_if {};
    template <typename T> struct enable_if<true, T> { typedef T type; };
    template <bool B, typename T = void> using enable_if_t = typename enable_if<B, T>::type;

    /// conditional
    template <bool B, typename T, typename F>  struct conditional { typedef T type; };
    template <typename T, typename F> struct conditional<false, T, F> { typedef F type; };
    template <bool B, typename T, typename F> using conditional_t = typename conditional<B, T, F>::type;

    /// size comparison
    template <typename T1, typename T2> struct is_same_size    { static const bool value = sizeof (T1) == sizeof (T2); };
    template <typename T1, typename T2> struct is_bigger_size  { static const bool value = sizeof (T1) > sizeof (T2); };
    template <typename T1, typename T2> struct is_smaller_size { static const bool value = sizeof (T1) < sizeof (T2); };

    template <typename T1, typename T2> inline constexpr bool is_same_size_v    = is_same_size<T1, T2>::value;
    template <typename T1, typename T2> inline constexpr bool is_bigger_size_v  = is_bigger_size<T1, T2>::value;
    template <typename T1, typename T2> inline constexpr bool is_smaller_size_v = is_same_size<T1, T2>::value;

    template <typename T1, typename T2> struct biggest_size {
        typedef conditional_t<is_same_size_v<T1, T2> || is_bigger_size_v<T1, T2>, T1, T2> type;
    };
    template <typename T1, typename T2> struct smallest_size {
        typedef conditional_t<is_same_size_v<T1, T2> || is_smaller_size_v<T1, T2>, T1, T2> type;
    };

    template <typename T1, typename T2> using biggest_size_t = typename biggest_size<T1, T2>::type;
    template <typename T1, typename T2> using smallest_size_t = typename smallest_size<T1, T2>::type;

    /// is_same
    template <typename T1, typename T2> struct is_same : public false_type {};
    template <typename T> struct is_same<T, T> : public true_type {};
    template <typename T1, typename T2> inline constexpr bool is_same_v = is_same<T1, T2>::value;

    /// is_signed
    template <typename T> struct is_signed : false_type {};
    template <> struct is_signed<char> : bool_constant<(char(255) < 0)> {};
    template <> struct is_signed<wchar_t> : public bool_constant<static_cast<bool>(wchar_t(-1) < wchar_t(0))> {};
    template <> struct is_signed<signed char> : true_type {};
    template <> struct is_signed<short> : true_type {};
    template <> struct is_signed<int> : true_type {};
    template <> struct is_signed<long> : true_type {};
    template <> struct is_signed<long long> : true_type {};
    template <> struct is_signed<float> : true_type {};
    template <> struct is_signed<double> : true_type {};
    template <> struct is_signed<long double> : true_type {};
    template <typename T> struct is_signed<const T> : is_signed<T> {};
    template <typename T> struct is_signed<volatile T> : is_signed<T> {};
    template <typename T> struct is_signed<const volatile T> : is_signed<T> {};
    template <typename T> inline constexpr bool is_signed_v = is_signed<T>::value;

    /// is_unsigned
    template <typename T> struct is_unsigned : false_type {};
    template <> struct is_unsigned<bool> : true_type {};
    template <> struct is_unsigned<char> : bool_constant<(char(255) > 0)> {};
    template <> struct is_unsigned<unsigned char> : true_type {};
    template <> struct is_unsigned<wchar_t> : public bool_constant<(wchar_t(-1) > wchar_t(0))> {};
    template <> struct is_unsigned<unsigned short> : true_type {};
    template <> struct is_unsigned<unsigned int> : true_type {};
    template <> struct is_unsigned<unsigned long> : true_type {};
    template <> struct is_unsigned<unsigned long long> : true_type {};
    template <typename T> struct is_unsigned<const T> : is_unsigned<T> {};
    template <typename T> struct is_unsigned<volatile T> : is_unsigned<T> {};
    template <typename T> struct is_unsigned<const volatile T> : is_unsigned<T> {};
    template <typename T> inline constexpr bool is_unsigned_v = is_unsigned<T>::value;

    /// add_unsigned
    template <typename T> struct add_unsigned { typedef T type; };
    template <> struct add_unsigned<char>      { typedef unsigned char type; };
    template <> struct add_unsigned<short>     { typedef unsigned short type; };
    template <> struct add_unsigned<int>       { typedef unsigned int type; };
    template <> struct add_unsigned<long>      { typedef unsigned long type; };
    template <> struct add_unsigned<long long> { typedef unsigned long long type; };
    template <typename T> using add_unsigned_t = typename add_unsigned<T>::type;
    template <typename T> struct add_unsigned<const T> : add_const<add_unsigned_t<T>> {};
    template <typename T> struct add_unsigned<volatile T> : add_volatile<add_unsigned_t<T>> {};
    template <typename T> struct add_unsigned<const volatile T> : add_const_volatile<add_unsigned_t<T>> {};

    /// remove_unsigned
    template <typename T> struct remove_unsigned           { typedef T type; };
    template <> struct remove_unsigned<unsigned char>      { typedef char type; };
    template <> struct remove_unsigned<unsigned short>     { typedef short type; };
    template <> struct remove_unsigned<unsigned int>       { typedef int type; };
    template <> struct remove_unsigned<unsigned long>      { typedef long type; };
    template <> struct remove_unsigned<unsigned long long> { typedef long long type; };
    template <typename T> using remove_unsigned_t = typename remove_unsigned<T>::type;
    template <typename T> struct remove_unsigned<const T> : add_const<remove_unsigned_t<T>> {};
    template <typename T> struct remove_unsigned<volatile T> : add_volatile<remove_unsigned_t<T>> {};
    template <typename T> struct remove_unsigned<const volatile T> : add_const_volatile<remove_unsigned_t<T>> {};

    /// is_integral
    template <typename T> struct is_integral : false_type {};
    template <> struct is_integral<bool> : true_type {};
    template <> struct is_integral<char> : true_type {};
    template <> struct is_integral<unsigned char> : true_type {};
    template <> struct is_integral<signed char> : true_type {};
    template <> struct is_integral<wchar_t> : true_type {};
    template <> struct is_integral<short> : true_type {};
    template <> struct is_integral<unsigned short> : true_type {};
    template <> struct is_integral<int> : true_type {};
    template <> struct is_integral<unsigned int> : true_type {};
    template <> struct is_integral<long> : true_type {};
    template <> struct is_integral<unsigned long> : true_type {};
    template <> struct is_integral<long long> : true_type {};
    template <> struct is_integral<unsigned long long> : true_type {};
    template <typename T> struct is_integral<const T> : is_integral<T> {};
    template <typename T> struct is_integral<volatile T> : is_integral<T> {};
    template <typename T> struct is_integral<const volatile T> : is_integral<T> {};
    template <typename T> inline constexpr bool is_integral_v = is_integral<T>::value;
    template <typename T> inline constexpr bool is_signed_integral_v = is_integral<T>::value && is_signed<T>::value;
    template <typename T> inline constexpr bool is_unsigned_integral_v = is_integral<T>::value && is_unsigned<T>::value;

    /// is_floating_point
    template <typename T> struct is_floating_point : false_type {};
    template <> struct is_floating_point<float> : true_type {};
    template <> struct is_floating_point<double> : true_type {};
    template <> struct is_floating_point<long double> : true_type {};
    template <typename T> struct is_floating_point<const T> : is_floating_point<T> {};
    template <typename T> struct is_floating_point<volatile T> : is_floating_point<T> {};
    template <typename T> struct is_floating_point<const volatile T> : is_floating_point<T> {};
    template <typename T> inline constexpr bool is_floating_point_v = is_floating_point<T>::value;

    /// is_void
    template <typename T> struct is_void : false_type {};
    template <> struct is_void<void> : true_type {};
    template <typename T> inline static const bool is_void_v = is_void<T>::value;

    /// is_arithmetic
    template <typename T> struct is_arithmetic : bool_constant<is_integral_v<T> || is_floating_point_v<T>> {};
    template <typename T> inline static const bool is_arithmetic_v = is_arithmetic<T>::value;

    /// is_fundamental
    template <typename T> struct is_fundamental : bool_constant<is_arithmetic_v<T> || is_void_v<T>> {};
    template <typename T> inline static const bool  is_fundamental_v = is_fundamental<T>::value;

    /// is_compound
    template <typename T> struct is_compound : bool_constant<! is_fundamental<T>::value> {};
    template <typename T> inline static const bool is_compound_v = is_compound<T>::value;

    /// is_trivially_copyable
    template <typename T>
    struct is_trivially_copyable : public bool_constant<is_arithmetic_v<T> || is_pointer_v<T>> {};
    template<typename T> inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;

    /// is_array
    template <typename T> struct is_array : false_type {};
    template <typename T> struct is_array<T[]> : true_type {};
    template <typename T, size_t N> struct is_array<T[N]> : true_type {};
    template <typename T> inline constexpr bool is_array_v = is_array<T>::value;

    /// is_string
    template <typename T> struct is_string : false_type {};
    template <> struct is_string<char[]> : true_type {};
    template <size_t N> struct is_string<char[N]> : true_type {};
    template <typename T> inline constexpr bool is_string_v = is_string<T>::value;

    /// is_function
    template <typename T> struct is_function : false_type {};
    template <typename R, typename... Args> struct is_function<R(Args...)> : true_type {};
    template <typename T> inline constexpr bool is_function_v = is_function<T>::value;

    /// is_function_pointer
    template <typename T> struct is_function_pointer : false_type {};
    template <typename R, typename... Args> struct is_function_pointer<R(*)(Args...)> : true_type {};
    template <typename T> inline constexpr bool is_function_pointer_v = is_function_pointer<T>::value;
   
    /// is_functor
    template <typename T>
    struct is_functor {
        template <typename U> static auto test(U* p) -> decltype(&U::operator(), void(), true_type());
        template <typename U> static auto test(...) -> false_type;
        static constexpr bool value = decltype(test<T>(nullptr))::value;
    };
    template <typename T> inline constexpr bool is_functor_v = is_functor<T>::value;

    /// remove_extent
    template <typename T> struct remove_extent { typedef T type; };
    template <typename T> struct remove_extent<T[]> { typedef T type; };
    template <typename T, size_t N> struct remove_extent<T[N]> { typedef T type; };
    template <typename T> using remove_extent_t = typename remove_extent<T>::type;

    /// decay
    template <typename T>
    struct decay {
        typedef remove_reference_t<T> U;
        typedef conditional_t<etl::is_array_v<U>, remove_extent_t<U>*, remove_const_volatile_t<U>> type;
    };
    template <typename T> using decay_t = typename decay<T>::type;

    /// integer sequence
    template<typename T, T... i>
    struct integer_sequence {
        typedef T value_type;
        static constexpr size_t size() noexcept { return sizeof...(i); }
    };

    template <typename T, T Begin, T End, T... Is>
    struct make_index_sequence_helper {
        using type = typename make_index_sequence_helper<T, Begin, End-1, End-1, Is...>::type;
    };

    template <typename T, T Begin, T... Is>
    struct make_index_sequence_helper<T, Begin, Begin, Is...> {
        using type = integer_sequence<size_t, Is...>;
    };

    template <size_t... i>
    using index_sequence = integer_sequence<size_t, i...>;

    template <size_t Begin, size_t End>
    using make_range_sequence = typename make_index_sequence_helper<size_t, Begin, End>::type;

    template <size_t N>
    using make_index_sequence = typename make_index_sequence_helper<size_t, 0, N>::type;

    template <typename... T> using index_sequence_for = make_index_sequence<sizeof...(T)>;
}

#endif //ETL_TYPE_TRAITS_H

#ifndef ETL_STRING_H
#define ETL_STRING_H

#include <cstring> // strlen, strcpy, memset
#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vsnprintf

#ifndef ETL_STRING_DEFAULT_SIZE
#define ETL_STRING_DEFAULT_SIZE 64
#endif

#ifndef ETL_SHORT_STRING_DEFAULT_SIZE
#define ETL_SHORT_STRING_DEFAULT_SIZE 16
#endif

#include "etl/algorithm.h"

namespace Project::etl {
    template <size_t N, size_t M> class SplitString;

    /// simple string class with c-style formatter
    template <size_t N = ETL_STRING_DEFAULT_SIZE>
    class String {
        char str[N];
    public:
        static_assert(N > 0, "String size can't be zero");

        typedef char value_type;
        typedef char* iterator;
        typedef const char* const_iterator;
        typedef char& reference;
        typedef const char& const_reference;

        /// empty constructor
        constexpr String() : str{} {}

        /// C-style format constructor
        String(const char* fmt, ...) : str{} {
            va_list vl;
            va_start(vl, fmt);
            vsnprintf(str, N, fmt, vl);
            va_end(vl);
            str[N - 1] = '\0';
        }

        /// copy constructor
        template <size_t M>
        constexpr String(const String<M>& other) : str{} {
            *this = other;
        }

        /// copy assignment
        template <size_t M>
        constexpr String& operator=(const String<M>& other) {
            auto n = min(N, M);
            copy(other.begin(), other.begin() + n, str);
            str[N - 1] = '\0';
            return *this;
        }

        /// move constructor
        template <size_t M>
        constexpr String(String<M>&& other) noexcept : str{} {
            *this = move(other);
        }

        /// move assignment
        template <size_t M>
        constexpr String& operator=(String<M>&& other) noexcept {
            auto n = min(N, M);
            move(other.begin(), other.begin() + n, str);
            str[N - 1] = '\0';
            other.clear();
            return *this;
        }

        /// size in bytes
        static constexpr size_t size() { return N; }

        /// string length, maximum is N - 1
        [[nodiscard]] constexpr size_t len() const { return find(str, str + N - 1, '\0') - str; }

        /// remaining space
        [[nodiscard]] constexpr size_t rem() const { return N - 1 - len(); }

        /// set all characters to 0
        constexpr void clear() { fill(str, '\0'); }

        constexpr iterator data()   { return str; }
        constexpr iterator begin()  { return str; }
        constexpr iterator end()    { return str + len(); }
        constexpr reference front() { return str[0]; }
        constexpr reference back()  { auto l = len(); return str[l ? l - 1 : 0]; }

        [[nodiscard]] constexpr const_iterator data()   const { return str; }
        [[nodiscard]] constexpr const_iterator begin()  const { return str; }
        [[nodiscard]] constexpr const_iterator end()    const { return str + len(); }
        [[nodiscard]] constexpr const_reference front() const { return str[0]; }
        [[nodiscard]] constexpr const_reference back()  const { auto l = len(); return str[l ? l - 1 : 0]; }

        constexpr reference operator [](size_t i) { return str[i % N]; }
        constexpr const_reference operator [](size_t i) const { return str[i % N]; }

        /// return true if first character is not '\0'
        constexpr explicit operator bool() const { return str[0] != '\0'; }

        /// assign from other const char*
        constexpr String& operator=(const char* other) {
            size_t i = 0;
            for (; i < N - 1 && other[i] != '\0'; i++) str[i] = other[i];
            str[i] = '\0';
            return *this;
        }

        /// assign from char
        constexpr String& operator=(char ch) {
            if (N == 1) return *this;
            str[0] = ch;
            str[1] = '\0';
            return *this;
        }

        /// append operator from other string
        template <size_t M>
        constexpr String& operator+=(const String<M>& other)  {
            auto offset = other.data() + min(rem(), other.len());
            auto last = copy(other.data(), offset, end());
            *last = '\0';
            return *this;
        }

        /// append operator from other const char*
        constexpr String& operator+=(const char* other) {
            size_t i = 0, l = len(), r = rem();
            for (; i < r && other[i] != '\0'; i++) str[l + i] = other[i];
            str[l + i] = '\0';
            return *this;
        }

        /// append operator from char
        constexpr String &operator+=(char ch) {
            if (rem() == 0) return *this;
            auto i = len();
            str[i] = ch;
            str[i + 1] = '\0';
            return *this;
        }

        template <size_t M>
        constexpr String &operator<<(const String<M>& other)  { *this += other; return *this; }
        constexpr String &operator<<(const char* other)       { *this += other; return *this; }
        constexpr String &operator<<(char ch)                 { *this += ch; return *this; }

        /// C-style formatter
        char* operator()(const char* fmt, ...) {
            va_list vl;
            va_start(vl, fmt);
            vsnprintf(str, N, fmt, vl);
            va_end(vl);
            str[N - 1] = '\0';
            return str;
        }

        /* compare functions */
        template <size_t M> int
        compare(const String<M>& other)           const { return strncmp(str, other.data(), N); }
        template <size_t M> int
        compare(const String<M>& other, size_t n) const { return strncmp(str, other.data(), n); }

        int compare(const char* other, size_t n)  const { return strncmp(str, other, n); }
        int compare(const char* other)            const { return strncmp(str, other, N); }

        /* compare operators */
        template <size_t M> bool
        operator==(const String<M>& other) const { return compare(other) == 0; }
        template <size_t M> bool
        operator>(const String<M>& other)  const { return compare(other) > 0; }
        template <size_t M> bool
        operator<(const String<M>& other)  const { return compare(other) < 0; }

        bool operator==(const char* other) const { return compare(other) == 0; }
        bool operator>(const char* other)  const { return compare(other) > 0; }
        bool operator<(const char* other)  const { return compare(other) < 0; }

        template <size_t M>
        bool isContaining(const String<M>& other, size_t n) const {
            for (size_t i : range(len())) if (strncmp(other.data(), &str[i], n) == 0) return true;
            return false;
        }
        bool isContaining(const char* other, size_t n) const {
            for (size_t i : range(len())) if (strncmp(other, &str[i], n) == 0) return true;
            return false;
        }

        template <size_t M>
        bool isContaining(const String<M>& other) const { return isContaining(other, other.len()); }
        bool isContaining(const char* other) const { return isContaining(other, strlen(other)); }

        template <size_t M = ETL_SHORT_STRING_DEFAULT_SIZE>
        auto split(const char* separator = " ") { return SplitString<N, M>(str, separator); }
    };

    /// create string, size is deduced
    template <size_t N> constexpr auto
    string(const char (&text)[N]) {
        String<N> res;
        copy(text, text + N, res.data());
        return res;
    }

    /// cast from const char*
    template <size_t N> constexpr const String<N>&
    string_cast(const char* text) { return *reinterpret_cast<const String<N>*>(text); }

    /// cast from char*
    template <size_t N> constexpr String<N>&
    string_cast(char* text) { return *reinterpret_cast<String<N>*>(text); }

    /// cast from const char[N]
    template <size_t N> constexpr const String<N>&
    string_cast(const char (&text)[N]) { return *reinterpret_cast<const String<N>*>(&text); }

    /// cast from char[N]
    template <size_t N> constexpr String<N>&
    string_cast(char (&text)[N]) { return *reinterpret_cast<String<N>*>(&text); }

    /// create string from 2 strings, size is s1.size() + s2.size() - 1
    /// @warning string buf might be unnecessarily large
    template <size_t N, size_t M> constexpr auto
    operator+(const String<N>& s1, const String<M>& s2) {
        String<N + M -1> res = s1;
        res += s2;
        return res;
    }

    /// swap specialization
    template <size_t N, size_t M> constexpr void
    swap(String<N>& s1, String<M>& s2) {
        String<N> temp(move(s1));
        s1 = move(s2);
        s2 = move(temp);
    }

    /// swap_element specialization
    template <size_t N, size_t M> constexpr void
    swap_element(String<N>& s1, String<M>& s2) {
        swap(s1, s2);
    }

    /// simple split string using strtok
    /// @tparam N string buf size, default = ETL_STRING_DEFAULT_SIZE
    /// @tparam M argv buf size, default = ETL_SHORT_STRING_DEFAULT_SIZE
    template <size_t N = ETL_STRING_DEFAULT_SIZE, size_t M = ETL_SHORT_STRING_DEFAULT_SIZE>
    class SplitString {
        size_t argc = 0;    ///< number of arguments
        char* argv[M] = {}; ///< array of argument values

    public:
        /// construct from char*
        SplitString(char* text, const char* separator = " ") {
            for (; argc < M; argc++) {
                argv[argc] = strtok(argc == 0 ? text : nullptr, separator);
                if (argv[argc] == nullptr) break;
            }
        }

        /// construct from String
        SplitString(String<N>& text, const char* separator = " ") : SplitString(text.data(), separator) {}

        char* operator [](size_t index) { return index < argc ? argv[index] : nullptr; }

        size_t len()  { return argc; }

        char** begin() { return &argv[0]; }
        char** end()   { return &argv[argc]; }

        char* front() { return argv[0]; }
        char* back()  { return argv[argc - 1]; }
    };
}

namespace Project::etl::literals {

    constexpr auto operator ""s(const char* text, size_t n) {
        String<ETL_SHORT_STRING_DEFAULT_SIZE> s;
        copy(text, text + n, s.data());
        s[ETL_SHORT_STRING_DEFAULT_SIZE - 1] = '\0';
        return s;
    }
}

#endif //ETL_STRING_H

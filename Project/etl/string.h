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
    template <size_t N, size_t M> struct SplitString;

    /// simple string class with c-style formatter
    template <size_t N = ETL_STRING_DEFAULT_SIZE>
    class String {
        char str[N] = {};
    public:
        static_assert(N > 0, "String size can't be zero");

        /// construct empty string
        constexpr String() = default;

        /// C-style format constructor
        String(const char* fmt, ...) {
            va_list vl;
            va_start(vl, fmt);
            vsnprintf(str, N, fmt, vl);
            va_end(vl);
            str[N - 1] = '\0';
        }

        /// construct from @ref other string
        template <size_t M>
        constexpr String(String<M> other) {
            *this = other;
        }

        /// size in bytes
        static constexpr size_t size() { return N; }

        /// string length, maximum is N - 1
        [[nodiscard]] constexpr size_t len() const { return find(str, str + N - 1, '\0') - str; }

        /// remaining space
        [[nodiscard]] constexpr size_t rem() const { return N - 1 - len(); }

        /// set all characters to 0
        constexpr void clear() { fill(str, '\0'); }

        constexpr char* data()    { return str; }
        constexpr char* begin()   { return str; }
        constexpr char* end()     { return str + len(); }
        constexpr char& front()   { return str[0]; }
        constexpr char& back()    { size_t l = len(); return str[l ? l - 1 : 0]; }

        [[nodiscard]] constexpr const char* data()    const { return str; }
        [[nodiscard]] constexpr const char* begin()   const { return str; }
        [[nodiscard]] constexpr const char* end()     const { return str + len(); }
        [[nodiscard]] constexpr const char& front()   const { return str[0]; }
        [[nodiscard]] constexpr const char& back()    const { size_t l = len(); return str[l ? l - 1 : 0]; }

        constexpr char& operator [](size_t i) { return str[i % N]; }
        constexpr const char& operator [](size_t i) const { return str[i % N]; }

        /// return true if first character is not '\0'
        constexpr explicit operator bool() const { return str[0] != '\0'; }

        /// assign from other string
        template <size_t M>
        constexpr String& operator=(const String<M>& other) {
            auto n = min(N, M);
            copy(other.begin(), other.begin() + n, str);
            str[N - 1] = '\0';
            return *this;
        }

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
        SplitString<N, M> split(const char* separator = " ") {
            return SplitString<N, M>(str, separator);
        }
    };

    /// create string, size is deduced
    template <size_t N> constexpr auto
    string(const char (&text)[N]) {
        String<N> res;
        copy(text, text + N, res.data());
        return res;
    }

    /// create string from 2 strings, size is s1.size() + s2.size() - 1
    /// @warning string buf might be unnecessarily large
    template <size_t N, size_t M> constexpr auto
    operator+(const String<N>& s1, const String<M>& s2) {
        String<N + M -1> res = s1;
        res += s2;
        return res;
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

        char* begin() { return argv[0]; }
        char* end()   { return argv[argc]; }

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

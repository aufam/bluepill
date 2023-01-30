#ifndef ETL_STRING_H
#define ETL_STRING_H

#include <cstring> // strlen, strcpy, memset
#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vsnprintf

#ifndef ETL_STRING_DEFAULT_SIZE
#define ETL_STRING_DEFAULT_SIZE 64
#endif

namespace Project::etl {

    /// simple string class with c-style formatter
    /// @tparam N string size. default = @ref ETL_STRING_DEFAULT_SIZE
    template <size_t N = ETL_STRING_DEFAULT_SIZE>
    class String {
        char str[N] = {};
    public:
        constexpr String() = default;
        String(const char* fmt, ...) {
            va_list vl;
            va_start(vl, fmt);
            vsnprintf(str, N, fmt, vl);
            va_end(vl);
            str[N - 1] = '\0';
        }

        static constexpr size_t size() { return N; }
        [[nodiscard]] size_t len() const { size_t l = strlen(str); return l < N ? l : N - 1; }
        [[nodiscard]] size_t rem() const { return size() - len() - 1; } ///< remaining space
        void clear() { memset(str, 0, N); }

        char* data()    { return str; }
        char* begin()   { return str; }
        char* end()     { return str + len(); }
        char& front()   { return str[0]; }
        char& back()    { return str[len() - 1]; }

        [[nodiscard]] const char* data()    const { return str; }
        [[nodiscard]] const char* begin()   const { return str; }
        [[nodiscard]] const char* end()     const { return str + len(); }
        [[nodiscard]] const char& front()   const { return str[0]; }
        [[nodiscard]] const char& back()    const { return str[len() - 1]; }

        char& operator [] (size_t i) { return str[i]; }
        const char& operator [] (size_t i) const { return str[i]; }

        explicit operator bool () { return str[0] != '\0'; }

        template <size_t M>
        String &operator = (const String<M>& other)   { strncpy(str, other.data(), size() - 1); return *this; }
        String &operator = (const char* other)        { strncpy(str, other, size() - 1); return *this; }
        String &operator = (char ch)                  { str[0] = ch; str[1] = '\0'; return *this; }

        template <size_t M>
        String &operator += (const String<M>& other)  { strncpy(end(), other.data(), rem()); return *this; }
        String &operator += (const char* other)       { strncpy(end(), other, rem()); return *this; }
        String &operator += (char ch) {
            if (rem() == 0) return *this;
            size_t i = len();
            str[i++] = ch;
            str[i] = '\0';
            return *this;
        }

        template <size_t M>
        String &operator << (const String<M>& other)  { *this += other; return *this; }
        String &operator << (const char* other)       { *this += other; return *this; }
        String &operator << (char ch)                 { *this += ch; return *this; }

        /// formatted string operation
        /// @param fmt formatted string
        /// @retval this string buffer
        char *operator ()(const char* fmt, ...) {
            va_list vl;
            va_start(vl, fmt);
            vsnprintf(str, N, fmt, vl);
            va_end(vl);
            str[N - 1] = '\0';
            return str;
        }

        template <size_t M> int compare(const String<M>& other, size_t n) const { return strncmp(str, other.data(), n); }
        template <size_t M> int compare(const String<M>& other)           const { return strcmp(str, other.data()); }
        template <size_t M> bool operator ==(const String<M>& other)      const { return compare(other) == 0; }
        template <size_t M> bool operator >(const String<M>& other)       const { return compare(other) > 0; }
        template <size_t M> bool operator <(const String<M>& other)       const { return compare(other) < 0; }
        int compare(const char* other, size_t n)                          const { return strncmp(str, other, n); }
        int compare(const char* other)                                    const { return strcmp(str, other); }
        bool operator ==(const char* other)                               const { return compare(other) == 0; }
        bool operator >(const char* other)                                const { return compare(other) > 0; }
        bool operator <(const char* other)                                const { return compare(other) < 0; }

        template <size_t M>
        bool isContaining(const String<M>& other, size_t n) const {
            for (const char& c : *this) if (strncmp(other.data(), &c, n) == 0) return true;
            return false;
        }
        bool isContaining(const char* other, size_t n) const {
            for (const char& c : *this) if (strncmp(other, &c, n) == 0) return true;
            return false;
        }
        template <size_t M>
        bool isContaining(const String<M>& other) const { return isContaining(other, other.len()); }
        bool isContaining(const char* other) const { return isContaining(other, strlen(other)); }
    };

}

#endif //ETL_STRING_H

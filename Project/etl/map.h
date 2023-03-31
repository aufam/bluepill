#ifndef ETL_MAP_H
#define ETL_MAP_H

#include "etl/vector.h"

namespace Project::etl {

    /// collection of key-value pairs, keys are unique
    template <typename K, typename V>
    class Map : public Vector<Pair<K, V>> {
        Map(Pair<K, V>* buffer, size_t nItems) : Vector<Pair<K, V>>(buffer, nItems) {}

        Pair<K, V>* insert_(const Map& other) const {
            auto temp = new Pair<K, V>[this->nItems + other.nItems];
            copy(this->begin(), this->end(), temp);
            copy(other.begin(), other.end(), temp + this->nItems);
            return temp;
        }

        Pair<K, V>* insert_(const Pair<K, V>& other) const {
            auto temp = new Pair<K, V>[this->nItems + 1];
            copy(this->begin(), this->end(), temp);
            temp[this->nItems] = other;
            return temp;
        }

    public:
        typedef K Key;
        typedef V Value;

        /// empty constructor
        constexpr Map() : Vector<Pair<K, V>>() {}

        /// variadic template function constructor
        template <typename... Ks, typename... Vs>
        Map(const Pair<Ks, Vs>&... items) : Vector<Pair<K, V>>(items...) {}

        /// copy constructor
        Map(const Map& m) : Vector<Pair<K, V>>(new Pair<K, V>[m.nItems], m.nItems) {
            copy(m.begin(), m.end(), this->buf);
        }

        /// move constructor
        Map(Map&& m) noexcept : Vector<Pair<K, V>>(move(m.buf), move(m.nItems)) {
            m.buf = nullptr;
            m.nItems = 0;
        }

        /// copy assignment
        Map& operator=(const Map& other) {
            if (this == &other) return *this;
            delete [] this->buf;
            this->nItems = other.nItems;
            this->buf = new Pair<K, V>[this->nItems];
            copy(other.begin(), other.end(), this->buf);
            return *this;
        }

        /// move assignment
        Map& operator=(Map&& other) noexcept {
            this->buf = move(other.buf);
            this->nItems = move(other.nItems);
            other.buf = nullptr;
            other.nItems = 0;
            return *this;
        }

        ~Map() {
            delete [] this->buf;
            this->buf = nullptr;
            this->nItems = 0;
        }

        /// check if a key is in this map
        bool has(const K& key) const {
            for (auto &[x, y]: *this) if (x == key) return true;
            return false;
        }


        V& operator[](const K& key) {
            for (auto &[x, y]: *this) if (x == key) return y;
            this->append(pair(key, Value{}));
            return this->back().y;
        }

        const V& operator[](const K& key) const {
            for (auto &[x, y]: *this) if (x == key) return y;
            static const auto res = Value{};
            return res;
        }
 
        Map operator+(const Map& other) const { return {insert_(other), this->nItems + other.nItems }; }
        Map operator+(const Pair<K, V>& other) const { return {insert_(other), this->nItems + 1 }; }

        void append(const Map& other) {
            auto temp = insert_(other);
            delete [] this->buf;
            this->buf = temp;
            this->nItems += other.nItems;
        }
        void append(const Pair<K, V>& other) {
            auto temp = insert_(other);
            delete [] this->buf;
            this->buf = temp;
            this->nItems++;
        }

        Map& operator+=(const Map& other) {
            append(other);
            return *this;
        }
        Map& operator+=(const Pair<K, V>& other) {
            append(other);
            return *this;
        }

        void remove(const K& key) {
            auto index = this->len();
            if (index == 0) return;

            for (auto [i, pair] : enumerate(*this)) if (pair.x == key) index = i;
            if (index >= this->len()) return;

            auto buf = new Pair<K, V>[this->len() - 1];
            size_t i = 0;
            for (auto [j, item] : enumerate(*this)) {
                if (j == index) continue;
                buf[i++] = item;
            }

            delete [] this->buf;
            this->buf = buf;
            --this->nItems;
        }
    };

    /// create map using variadic function, type is deduced
    template <typename K, typename... Ks, typename V, typename... Vs> constexpr enable_if_t<(is_same_v<K, Ks> && ...) && (is_same_v<V, Vs> && ...), Map<K, V>>
    map(const Pair<K, V>& item, const Pair<Ks, Vs>&... items) { return Map<K, V> { item, items... }; }

    /// create empty map
    template <typename K, typename V> constexpr auto
    map() { return Map<K, V> {}; }

    /// type traits
    template <typename T> struct is_map : false_type {};
    template <typename K, typename V> struct is_map<Map<K, V>> : true_type {};
    template <typename K, typename V> struct is_map<const Map<K, V>> : true_type {};
    template <typename K, typename V> struct is_map<volatile Map<K, V>> : true_type {};
    template <typename K, typename V> struct is_map<const volatile Map<K, V>> : true_type {};
    template <typename T> inline constexpr bool is_map_v = is_map<T>::value;

    template <typename K, typename V> struct remove_extent<Map<K, V>> { typedef Pair<K, V> type; };
    template <typename K, typename V> struct remove_extent<const Map<K, V>> { typedef Pair<K, V> type; };
    template <typename K, typename V> struct remove_extent<volatile Map<K, V>> { typedef Pair<K, V> type; };
    template <typename K, typename V> struct remove_extent<const volatile Map<K, V>> { typedef Pair<K, V> type; };
}

#endif //ETL_MAP_H

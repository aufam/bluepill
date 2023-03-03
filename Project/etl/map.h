#ifndef ETL_MAP_H
#define ETL_MAP_H

#include "etl/vector.h"
#include "etl/utility.h"

namespace Project::etl {

    /// collection of key-value pairs, keys are unique
    template <class K, class V>
    struct Map : Vector<Pair<K, V>> {
        typedef K Key;
        typedef V Value;

        bool has(const K& key) const {
            for (auto &[x, y]: *this) if (x == key) return true;
            return false;
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

        V& operator[](const K& key) {
            for (auto &[x, y]: *this) if (x == key) return y;
            this->append(pair(key, Value{}));
            return this->back().y;
        }

        const V& operator[](const K& key) const {
            for (auto &[x, y]: *this) if (x == key) return y;
            return Value{};
        }
    };
}

#endif //ETL_MAP_H

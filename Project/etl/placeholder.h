#ifndef ETL_PLACEHOLDER_H
#define ETL_PLACEHOLDER_H

#include "etl/utility.h"


namespace Project::etl {

    template <typename UnaryFunction>
    class Placeholder {
        UnaryFunction fn; ///< base function

    public:
        /// default constructor
        constexpr explicit Placeholder(UnaryFunction&& fn) : fn{etl::forward<UnaryFunction>(fn)} {}

        /// function call operator
        template <typename T>
        constexpr decltype(auto) operator()(T&& arg) const { return fn(etl::forward<T>(arg)); }

        /* assignment operator */

        template <typename T>
        [[nodiscard]] auto operator=(T&& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) = etl::forward<T>(other);
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        /* boolean operations */

        [[nodiscard]] constexpr auto operator!() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return !fn(etl::forward<decltype(arg)>(arg));
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        template <typename T>
        [[nodiscard]] constexpr auto operator==(T&& other) const {
            auto res = [this, other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) == other;
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        template <typename T>
        [[nodiscard]] constexpr auto operator!=(T&& other) const {
            auto res = [this, other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) != other;
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        /* arithmetic operations */
        template <typename T>
        [[nodiscard]] constexpr auto operator*(T&& other) const {
            auto res = [this, other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) * other;
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        template <typename T>
        [[nodiscard]] constexpr auto operator+(T&& other) const {
            auto res = [this, other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) + other;
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        template <typename T>
        [[nodiscard]] constexpr auto operator/(T&& other) const {
            auto res = [this, other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) / other;
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        template <typename T>
        [[nodiscard]] constexpr auto operator-(T&& other) const {
            auto res = [this, other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) - other;
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        template <typename T>
        [[nodiscard]] auto operator*=(T&& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) *= etl::forward<T>(other);
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        template <typename T>
        [[nodiscard]] auto operator+=(T&& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) += etl::forward<T>(other);
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        template <typename T>
        [[nodiscard]] auto operator/=(T&& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) /= etl::forward<T>(other);
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        template <typename T>
        [[nodiscard]] auto operator-=(T&& other) const {
            auto res = [this, other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) -= etl::forward<T>(other);
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        /* iterator operations */

        [[nodiscard]] constexpr auto operator*() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return *fn(etl::forward<decltype(arg)>(arg));
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        [[nodiscard]] constexpr auto operator++() const  {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return ++fn(etl::forward<decltype(arg)>(arg));
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        [[nodiscard]] constexpr auto operator--() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return --fn(etl::forward<decltype(arg)>(arg));
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        [[nodiscard]] constexpr auto operator++(int) const  {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg))++;
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        [[nodiscard]] constexpr auto operator--(int) const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg))--;
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        /* common member functions */

        [[nodiscard]] constexpr auto begin() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return etl::begin(fn(etl::forward<decltype(arg)>(arg)));
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        [[nodiscard]] constexpr auto end() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return etl::end(fn(etl::forward<decltype(arg)>(arg)));
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        [[nodiscard]] constexpr auto len() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return etl::len(fn(etl::forward<decltype(arg)>(arg)));
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        [[nodiscard]] constexpr auto next() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return etl::next(fn(etl::forward<decltype(arg)>(arg)));
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        [[nodiscard]] constexpr auto iter() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return etl::iter(fn(etl::forward<decltype(arg)>(arg)));
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        [[nodiscard]] constexpr auto reversed() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return etl::reversed(fn(etl::forward<decltype(arg)>(arg)));
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }

        template <size_t N>
        [[nodiscard]] constexpr auto get() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return etl::get<N>(fn(etl::forward<decltype(arg)>(arg)));
            };
            return Placeholder<decltype(res)> { etl::move(res) };
        }
    };
}

namespace Project::etl::placeholder {
    static inline constexpr Placeholder arg {
        [] (auto&& arg) -> decltype(auto) {
            return etl::forward<decltype(arg)>(arg);
        }
    };
}

#endif //ETL_PLACEHOLDER_H

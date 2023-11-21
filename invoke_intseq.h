#ifndef INVOKE_INTSEQ
#define INVOKE_INTSEQ

#include <functional>
#include <iostream>
#include <type_traits>

namespace invoke_inteq_details {

template <typename T> struct is_integer_sequence : std::false_type {};

template <typename T, T... vals>
struct is_integer_sequence<std::integer_sequence<T, vals...>> : std::true_type {
};

// checks if any of the others are an integer_sequence
template <class... others>
struct any_match : std::disjunction<is_integer_sequence<others>...> {};
} // namespace invoke_inteq_details

template <class F, class... Args>
constexpr auto invoke_intseq(F &&f, Args &&...args) {
    if constexpr (std::is_same<std::false_type, invoke_inteq_details::any_match<Args...>>::value) {
        return std::invoke(f, args...);
    } else {
    }
}
#endif // !INVOKE_INTSEQ

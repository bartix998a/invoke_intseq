#ifndef INVOKE_INTSEQ
#define INVOKE_INTSEQ

#include <functional>
#include <iostream>
#include <type_traits>
#include <optional>
#include <tuple>

namespace invoke_inteq_details {

template <typename T> struct is_integer_sequence : std::false_type {};

template <typename T, T... vals>
struct is_integer_sequence<std::integer_sequence<T, vals...>> : std::true_type {
};


// checks if any of the others are an integer_sequence
template <class... others>
struct any_match : std::disjunction<is_integer_sequence<others>...> {};

// Finds the first element that is an integer sequence
// Assumes that arguments contain integer sequence
template <class F, std::tuple t, typename First, typename ...Rest>
struct find_first_struct {
    constexpr auto findFirst(F&& f, First&& first, Rest&& ...rest){
        return findFirst<t, Rest...>(rest ...);
    }
};

// found the element
template <class F,std::tuple t, typename T, T... vals, typename ...Rest>
struct find_first_struct<F ,t, std::integer_sequence<T, vals...>, Rest...> {
    constexpr auto findFirst(F&& f, T&& first, Rest&& ...rest){
        return std::array<std::result_of<decltype(f)()>, sizeof... (vals)> (expandRecursively);
    }
};

template<class  F,std::tuple t, typename First>
struct find_first_struct<F, t, First>{
    constexpr auto findFirst(F&& f,First&& first){
        if constexpr (is_integer_sequence<First>::value){
            return first;
        }
    }

    template<>
    constexpr auto expandRecursively(){

    }

};

} // namespace invoke_inteq_details
template <class F, class... Args>
constexpr auto invoke_intseq(F &&f, Args &&...args) {
    if constexpr (std::is_same<std::false_type, invoke_inteq_details::any_match<Args...>>::value || (sizeof... (args)) == 0) {
        return std::invoke(f, args...);
    } else {
        return invoke_inteq_details::find_first_struct<std::tuple<Args...>(args...), Args...>::findFirst(args...);
    }
}
#endif // !INVOKE_INTSEQ

#ifndef INVOKE_INTSEQ
#define INVOKE_INTSEQ

#include <functional>
#include <iostream>
#include <type_traits>
#include <optional>
#include <tuple>

// When I wrote this, only God and I understood what I was doing.
// Now, God only knows.

template <class F, class... Args>
constexpr auto invoke_intseq(F &&f, Args &&...args) -> std::invoke_result_t<F, Args...>::type;

namespace invoke_inteq_details {

template <typename T> struct is_integer_sequence : std::false_type {};

// TODO: what if vals is empty?
template <typename T, T... vals>
struct is_integer_sequence<std::integer_sequence<T, vals...>> : std::true_type {
};


// checks if any of the others are an integer_sequence
template <class... others>
struct any_match : std::disjunction<is_integer_sequence<others>...> {};

/*
* Helper function that calls a function f with combined arguments of tuple, togeter with the rest of the arguments.
* If the tuple t = {t_1, t_2, ..., t_n} and the rest of the arguments are {a_1, a_2, ..., a_m}, then
* this function will call f(t_1, t_2, ..., t_n, a_1, a_2, ..., a_m).
*
* Recursively calls itself until the tuple is empty.
* pos is the position of the last element in the tuple.
* TODO: isnt there a better way to do this?
* Like, for example, unpacking pack of arguments into a function?
*/

// return call_intseq<pos, F, t, First, Rest...>().expand_tuple(f, first, rest...);
template <size_t pos, class F, std::tuple t, typename First, typename ...Rest>
struct call_intseq {
    constexpr auto expand_tuple(F&& f, First&& first, Rest&& ...rest){
        if constexpr (pos > 0){
            // TODO: which method is it?
            return expand_tuple<pos - 1, F, t, std::result_of<decltype(get<pos>(t))>, Rest...>(f, get<pos>(t), first, rest ...);
        } else {
            return invoke_intseq<F, std::result_of<decltype(get<pos>(t))>, Rest...>(f, get<pos>(t), first,  rest ...);
        }
    }
};

// Find first no 0
/* Finds the first element that is an integer sequence.
 * Assumes that arguments contain at least one integer sequence.
 */
// I think that it is wrong, since we will start expanding on the first
// argument, but further down there might be many more integer sequences...
// Or at least it should call some helper function that will execute integer sequence together with the tuple.
// For example, if we have arguments (1, {2, 3}, 4, {5, 6}), then we should call
// invoke_intseq(f, 1, {2, 3}, 4, {5, 6}) -> invoke_intseq(f, 1, 2, 3, 4, {5, 6}) -> invoke_intseq(f, 1, 2, 3, 4, 5, 6)
// template <size_t pos, class F, std::tuple t, typename First, typename... Rest>
template <size_t pos, class F, std::tuple t, typename First, typename... Rest>
struct find_first_struct {
    constexpr auto find_first(F&& f, First&& first, Rest&& ...rest){
        // If this is the LAST integer sequence, then we should call the helper function
        if constexpr (is_integer_sequence<First>::value && 
            std::is_same<std::false_type, invoke_inteq_details::any_match<Rest...>>::value
        ){
            // If there is not "Rest" is the no 2 called?
            return call_intseq<pos, F, t, First, Rest...>().expand_tuple(f, first, rest...);
        } else if constexpr (is_integer_sequence<First>::value){
            // Not the last integer sequence

        } else {
            // TODO: append to tuple?
            return find_first<pos + 1, F, t, Rest...>(f, rest ...);
        }
    }
};


// Calculates the size needed for an resulting array
template <typename... Args>
constexpr size_t calc_size(Args&&... args);

template <typename T>
constexpr size_t calc_size(T seq){
    return 1;
}
template <typename T>
constexpr size_t calc_size(std::integer_sequence<T> seq){
    return seq.size();
}

template <typename... Args>
constexpr size_t calc_size(Args&&... args) {
    if constexpr (sizeof...(args) == 0) {
        return 1;  // Return 1 if the parameter pack is empty
    } else {
        return (calc_size(args) + ...);  // Sum of sizes for non-empty packs
    }
}

// found the element
// Find first no 1
// This is a specification of the above function, when we have only one argument left.
// Stores the result of invoking function f for each element of integer_sequence
template <size_t pos, class F, std::tuple t, typename T, T... vals, typename ...Rest>
struct find_first_struct<pos, F ,t, std::integer_sequence<T, vals...>, Rest...> {
    constexpr auto find_first(F&& f, T&& first, Rest&& ...rest){
        // FIX: result_of is deprecated, use invoke_result
        return std::array<std::invoke_result<decltype(f)()>, sizeof...(vals)>
                (call_intseq<pos - 1, F, t, std::integral_constant<T, vals>, Rest...>(f, std::integral_constant<T, vals>::value, rest)...);
    }
};

// No 2
template <size_t pos, class F, std::tuple t, typename First>
struct find_first_struct<pos, F, t, First>{
    constexpr auto find_first(F&& f, First&& first){
        if constexpr (is_integer_sequence<First>::value){
            return first;
        }
    };
};
} // namespace invoke_inteq_details

template <class F, class... Args>
// TODO
constexpr auto invoke_intseq(F &&f, Args &&...args) {
    // TODO: does the order matter here?
    if constexpr (std::is_same<std::false_type, invoke_inteq_details::any_match<Args...>>::value || (sizeof... (args)) == 0) {
        return std::invoke(f, args...);
    } else {
        // TODO: where is the iterating part?
        // Which find_first is called here?
        return invoke_inteq_details::find_first_struct< 0, F, std::tuple<Args...>(args...), Args...>::find_first(f, args...);
    }
}
#endif // !INVOKE_INTSEQ
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

#ifndef INVOKE_INTSEQ
#define INVOKE_INTSEQ

#include <functional>
#include <iostream>
#include <type_traits>
#include <optional>
#include <tuple>

//When I wrote this, only God and I understood what I was doing
//Now, God only knows

template <class F, class... Args>
constexpr auto invoke_intseq(F &&f, Args &&...args);

namespace invoke_inteq_details {

template <typename T> struct is_integer_sequence : std::false_type {};

template <typename T, T... vals>
struct is_integer_sequence<std::integer_sequence<T, vals...>> : std::true_type {
};


// checks if any of the others are an integer_sequence
template <class... others>
struct any_match : std::disjunction<is_integer_sequence<others>...> {};

template <size_t pos, class F, class t, typename First, typename ...Rest>
struct call_intseq {
    constexpr auto expand_tuple( F&& f, First&& first, Rest&& ...rest){
        return;
    }
};

template <size_t pos, class F, typename... T, typename First, typename ...Rest>
struct call_intseq<pos, F,std::tuple<T...>, First, Rest...> {
    constexpr auto expand_tuple(std::tuple<T...> t, F&& f, First&& first, Rest&& ...rest){
        if constexpr (pos > 0){
            return expand_tuple<pos - 1, F, t, std::result_of<decltype(get<pos>(t))>, Rest...>(f, get<pos>(t), rest ...);
        } else {
            return invoke_intseq<F, std::result_of<decltype(get<pos>(t))>, Rest...>(f, get<pos>(t), rest ...);
        }

    }
};

template <size_t pos, class F, class t, typename First, typename ...Rest>
struct find_first_struct {
    constexpr auto findFirst(F&& f, First&& first, Rest&& ...rest){
        return;
    }
};

template <size_t pos, class F, class ...T, typename First, typename ...Rest>
struct find_first_struct<pos, F, std::tuple<T...>, First, Rest...> {
    constexpr auto findFirst(std::tuple<T...> t, F&& f, First&& first, Rest&& ...rest){
        return findFirst<pos + 1, F, T..., Rest...>(t, f, rest ...);
    }
};


template <size_t pos, class F, class ...T, typename seq, seq ...vals,typename First, typename ...Rest>
struct find_first_struct<pos, F, std::tuple<T...>, std::integer_sequence<seq,vals...>,First, Rest...> {
    constexpr auto findFirst(std::tuple<T...> t, F&& f, First&& first, Rest&& ...rest){
        return std::array<std::result_of<decltype(f)()>, sizeof... (vals)> (
            call_intseq<pos - 1, F, T..., std::integral_constant<T, vals>, Rest...>(t ,f, std::integral_constant<T, vals>::value, rest)...);
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



} // namespace invoke_inteq_details
template <class F, class... Args>
constexpr auto invoke_intseq(F &&f, Args &&...args) {
    if constexpr (!invoke_inteq_details::any_match<Args...>::value || (sizeof... (args)) == 0) {
        return std::invoke(f, args...);
    } else {
        // FIXME: uncoment
        return invoke_inteq_details::find_first_struct<
            0, F, Args... ,Args...>::findFirst(std::tuple<Args...>(args...), f, args...);
    }
}
#endif // !INVOKE_INTSEQ

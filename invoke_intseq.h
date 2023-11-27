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
    constexpr auto findFirst(t palceholder_tuple,F&& f, First&& first, Rest&& ...rest){
        return;
    }
};

template <size_t pos, class F, class ...T, typename First, typename ...Rest>
struct find_first_struct<pos, F, std::tuple<T...>, First, Rest...> {
    constexpr auto findFirst(std::tuple<T...> t, F&& f, First&& first, Rest&& ...rest){
        return findFirst<pos + 1, F, T..., Rest...>(t, f, rest ...);
    }
};


template <size_t pos, class F, class ...T, typename seq, seq ...vals, typename ...Rest>
struct find_first_struct<pos, F, std::tuple<T...>, std::integer_sequence<seq,vals...>, Rest...> {
    constexpr auto findFirst(std::tuple<T...> t, F&& f, std::integer_sequence<seq,vals...> s, Rest&& ...rest){
        return std::array<std::result_of<decltype(f)()>, sizeof... (vals)> (
            call_intseq<pos - 1, F, T..., std::integral_constant<T, vals>, Rest...>(t ,f, std::integral_constant<T, vals>::value, rest)...);
    }
};

// Forward declaration with universal references
template <typename... Args>
constexpr size_t calc_size(Args&&... args);

// Specialization for std::integer_sequence
template <typename T, T... Ints>
constexpr size_t calc_size(std::integer_sequence<T, Ints...>) {
    return sizeof...(Ints);
}

// General case for single non-integer_sequence argument
template <typename T>
constexpr size_t calc_size(T) {
    return 1;
}

// Recursive variadic template for multiple arguments
template <typename First, typename... Rest>
constexpr size_t calc_size(First&& first, Rest&&... rest) {
    return calc_size(std::forward<First>(first)) * calc_size(std::forward<Rest>(rest)...);
}

template <>
constexpr size_t calc_size() {
    return 1;
}

// FIXME:
    static_assert(calc_size(2, 2) == 1, "this");
    static_assert(calc_size(7, std::integer_sequence<int, 0, 1>{}, std::integer_sequence<int, 4, 5>{}) == 4, "this");
    static_assert(calc_size() == 1, "this");
    static_assert(calc_size(
        std::integer_sequence<int, 0, 1>{},
        std::integer_sequence<int, 0, 1>{},
        std::integer_sequence<int, 0, 1>{}
    ) == 8, "this");
    static_assert(calc_size(std::integer_sequence<int, 0, 1>{}, 2) == 2, "this");
    static_assert(calc_size(std::integer_sequence<int, 4, 4>{}) == 2, "this");

    // Base case for the recursion - when only one sequence is left
    template <typename T, T... Ints, typename... Accumulated>
    static auto generate_impl(const std::tuple<Accumulated...>& acc, 
                              std::integer_sequence<T, Ints...>) {
        return std::make_tuple(std::tuple_cat(acc, std::make_tuple(Ints))...);
    }

    template <typename T, typename... Accumulated>
    static auto generate_impl(const std::tuple<Accumulated...>& acc, 
                              T val) {
        return std::make_tuple(std::tuple_cat(acc, std::make_tuple(val)));
    }

    // Recursive case - handles multiple integer sequences
    template <typename T, typename... TailSeq, typename... Accumulated>
    static auto generate_impl(const std::tuple<Accumulated...>& acc,
                              T val,
                              TailSeq... rest) {
        // Make one tuple from multiple tuples
        return generate_impl(std::tuple_cat(acc, std::make_tuple(val)), rest...);
    }


    // Recursive case - handles multiple integer sequences
    template <typename T, T... HeadInts, typename... TailSeq, typename... Accumulated>
    static auto generate_impl(const std::tuple<Accumulated...>& acc,
                              std::integer_sequence<T, HeadInts...>,
                              TailSeq... rest) {
        // Make one tuple from multiple tuples
        return std::apply(
            [](auto&&... args) { return std::tuple_cat(args...); }, 
            std::make_tuple(generate_impl(std::tuple_cat(acc, std::make_tuple(HeadInts)), rest...)...)
        );
    }

    template <typename... Seqs>
    static auto generate(Seqs... seqs) {
        auto acc = std::tuple<>{};
        return generate_impl(acc, seqs...);
    }

} // namespace invoke_inteq_details
template <class F, class... Args>
constexpr auto invoke_intseq(F &&f, Args &&...args) {
    if constexpr (!invoke_inteq_details::any_match<Args...>::value || (sizeof... (args)) == 0) {
        return std::invoke(f, args...);
    } else {
        return invoke_inteq_details::find_first_struct<
            0, F, std::tuple<Args...> ,Args...>::findFirst(std::tuple<Args...>(args...), f, args...);
    }
}
#endif // !INVOKE_INTSEQ

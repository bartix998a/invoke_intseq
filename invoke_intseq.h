#ifndef INVOKE_INTSEQ
#define INVOKE_INTSEQ

#include <functional>
#include <iostream>
#include <optional>
#include <tuple>
#include <type_traits>
#include <vector>

// When I wrote this, only God and I understood what I was doing
// Now, God only knows

template <class F, class... Args>
constexpr auto invoke_intseq(F &&f, Args &&...args) -> decltype(auto);

namespace invoke_inteq_details {

template <typename T> struct is_integer_sequence : std::false_type {};

template <typename T, T... vals>
struct is_integer_sequence<std::integer_sequence<T, vals...>> : std::true_type {
};

// checks if any of the others are an integer_sequence
template <class... others>
struct any_match : std::disjunction<is_integer_sequence<others>...> {};

template <size_t pos, class F, class t, typename First, typename... Rest>
struct call_intseq {
    constexpr auto expand_tuple(F &&f, First &&first, Rest &&...rest) {
        return;
    }
};

template <size_t pos, class F, typename... T, typename First, typename... Rest>
struct call_intseq<pos, F, std::tuple<T...>, First, Rest...> {
    constexpr auto expand_tuple(std::tuple<T...> t, F &&f, First &&first,
                                Rest &&...rest) {
        if constexpr (pos > 0) {
            return expand_tuple<pos - 1, F, t,
                                std::result_of<decltype(get<pos>(t))>, Rest...>(
                f, get<pos>(t), rest...);
        } else {
            return invoke_intseq<F, std::result_of<decltype(get<pos>(t))>,
                                 Rest...>(f, get<pos>(t), rest...);
        }
    }
};

template <size_t pos, class F, class t, typename First, typename... Rest>
struct find_first_struct {
    constexpr auto findFirst(t palceholder_tuple, F &&f, First &&first,
                             Rest &&...rest) {
        return;
    }
};

template <size_t pos, class F, class... T, typename First, typename... Rest>
struct find_first_struct<pos, F, std::tuple<T...>, First, Rest...> {
    constexpr auto findFirst(std::tuple<T...> t, F &&f, First &&first,
                             Rest &&...rest) {
        return findFirst<pos + 1, F, T..., Rest...>(t, f, rest...);
    }
};

template <size_t pos, class F, class... T, typename seq, seq... vals,
          typename... Rest>
struct find_first_struct<pos, F, std::tuple<T...>,
                         std::integer_sequence<seq, vals...>, Rest...> {
    constexpr auto findFirst(std::tuple<T...> t, F &&f,
                             std::integer_sequence<seq, vals...> s,
                             Rest &&...rest) {
        return std::array<std::result_of<decltype(f)()>, sizeof...(vals)>(
            call_intseq<pos - 1, F, T..., std::integral_constant<T, vals>,
                        Rest...>(t, f, std::integral_constant<T, vals>::value,
                                 rest)...);
    }
};

// Forward declaration with universal references
template <typename... Args> constexpr static size_t calc_size(Args &&...args);

// Specialization for std::integer_sequence
template <typename T, T... Ints>
constexpr static size_t calc_size(std::integer_sequence<T, Ints...>) {
    return sizeof...(Ints);
}

// General case for single non-integer_sequence argument
template <typename T> constexpr static size_t calc_size(T) { return 1; }

// Recursive variadic template for multiple arguments
template <typename First, typename... Rest>
constexpr static size_t calc_size(First &&first, Rest &&...rest) {
    return calc_size(std::forward<First>(first)) *
           calc_size(std::forward<Rest>(rest)...);
}

template <> constexpr size_t calc_size() { return 1; }

struct Comb_gen {
    // IMPORTANT: Four of the methods below are for recurrent generation of
    // all possible combinations of integer sequences.
    // They are splitted into two categories:
    // 1. next element to process is an integer sequence
    // 2. next element to process is a single integer
    //
    // Each category is splited into generic ad base cases.

    // Base case for the recursion - when only one sequence is left
    // Base for category 1
    template <typename T, T... Ints, typename... Accumulated>
    constexpr static auto generate_impl(const std::tuple<Accumulated...> &acc,
                                        std::integer_sequence<T, Ints...>) {
        return std::make_tuple(std::tuple_cat(acc, std::make_tuple(Ints))...);
    }

    // Base for category 2
    template <typename T, typename... Accumulated>
    constexpr static auto generate_impl(const std::tuple<Accumulated...> &acc,
                                        T val) {
        return std::make_tuple(std::tuple_cat(acc, std::make_tuple(val)));
    }

    // Recursive case - handles multiple integer sequences
    // Recursive case for category 1
    template <typename T, typename... TailSeq, typename... Accumulated>
    constexpr static auto generate_impl(const std::tuple<Accumulated...> &acc,
                                        T val, TailSeq... rest) {
        // Make one tuple from multiple tuples
        return generate_impl(std::tuple_cat(acc, std::make_tuple(val)),
                             rest...);
    }

    // Recursive case - handles multiple integer sequences
    // Recursive case for category 2
    template <typename T, T... HeadInts, typename... TailSeq,
              typename... Accumulated>
    constexpr static auto generate_impl(const std::tuple<Accumulated...> &acc,
                                        std::integer_sequence<T, HeadInts...>,
                                        TailSeq... rest) {
        // Make one tuple from multiple tuples
        return std::apply(
            [](auto &&...args) { return std::tuple_cat(args...); },
            std::make_tuple(generate_impl(
                std::tuple_cat(acc, std::make_tuple(HeadInts)), rest...)...));
    }

    // Generates all possible combinations of integer sequences.
    // Ex.:
    // generate(std::integer_sequence<int, 0, 1>{}, 2,
    // std::integer_sequence<int, 3, 4>{}) will generate
    // {{0, 2, 3}, {0, 2, 4}, {1, 2, 3}, {1, 2, 4}}
    // (where {} denotes a tuple)
    template <typename... Args> constexpr static auto generate(Args... args) {
        auto acc = std::tuple<>{};
        size_t size = calc_size(args...);
        return generate_impl(acc, args...);
    }

    // Applies function f to a single inner-tuple (so one possible combination
    // of a_1, ..., a_n, where every element a_i is an integer, not a sequence).
    template <class F, class Tuple, size_t... I>
    constexpr static auto apply_to_tuple(F &&f, Tuple &&t,
                                         std::index_sequence<I...>)
        -> decltype(auto) {
        if constexpr (std::is_same_v<void, decltype(f(std::get<I>(t)...))>) {
            // If F returns void, just invoke it
            (..., f(std::get<I>(std::forward<Tuple>(t))));
        } else {
            // Otherwise, return the result
            return f(std::get<I>(std::forward<Tuple>(t))...);
        }
    }

    template <size_t Index, size_t Size, class F, class Array,
              class two_dim_tuple>
    constexpr static void process_tuples(F &&f, Array &arr,
                                         const two_dim_tuple &t) {
        if constexpr (Index < Size) {
            arr[Index] = apply_to_tuple(
                std::forward<F>(f), std::get<Index>(t),
                std::make_index_sequence<std::tuple_size_v<
                    std::tuple_element_t<Index, two_dim_tuple>>>{});
            process_tuples<Index + 1, Size>(std::forward<F>(f), arr, t);
        }
    }

    template <size_t Index, size_t Size, class F, class two_dim_tuple>
    constexpr static void process_tuples(F &&f, const two_dim_tuple &t) {
        if constexpr (Index < Size) {
            apply_to_tuple(std::forward<F>(f), std::get<Index>(t),
                           std::make_index_sequence<std::tuple_size_v<
                               std::tuple_element_t<Index, two_dim_tuple>>>{});
            process_tuples<Index + 1, Size>(std::forward<F>(f), t);
        }
    }

    template <class F, class two_dim_tuple, size_t Size>
    constexpr static auto convert_to_array(F &&f, const two_dim_tuple &t)
        -> decltype(auto) {
        if constexpr (std::is_same_v<
                          void, decltype(apply_to_tuple(
                                    std::forward<F>(f), std::get<0>(t),
                                    std::make_index_sequence<
                                        std::tuple_size_v<std::tuple_element_t<
                                            0, two_dim_tuple>>>{}))>) {
            // If F returns void, just process tuples without storing
            process_tuples<0, Size>(std::forward<F>(f), t);
        } else {
            // Otherwise, process tuples and store results in an array
            using ReturnType = decltype(apply_to_tuple(
                std::forward<F>(f), std::get<0>(t),
                std::make_index_sequence<std::tuple_size_v<
                    std::tuple_element_t<0, two_dim_tuple>>>{}));
            std::array<ReturnType, Size> arr{};
            process_tuples<0, Size>(std::forward<F>(f), arr, t);
            return arr;
        }
    }

    template <class F, typename... Args>
    constexpr static auto apply_to_comb(F &&f, Args &&...args)
        -> decltype(auto) {

        auto myTuple = generate(args...);
        if constexpr (std::tuple_size<decltype(myTuple)>::value != 0) {
            // using restype = std::invoke_result_t<F, decltype(std::get<0>(myTuple))>;

            using restype = decltype(apply_to_tuple(
                std::forward<F>(f), std::get<0>(myTuple),
                std::make_index_sequence<std::tuple_size_v<
                    std::tuple_element_t<0, decltype(myTuple)>>>{}));

            if constexpr (std::is_same_v<void, restype>) {
                std::apply([&f](auto &...x) { (..., std::apply(f, x)); },
                           myTuple);

            } else {
                std::vector<restype> result{};
                // std::apply(
                //     [&f, &result](auto &...x) {
                //         (..., result.push_back(std::apply(f, x)));
                //     },
                //     myTuple);

                std::apply(
                    [&](auto &&...x) {
                        (..., result.push_back(std::apply(f, x)));
                    },
                    myTuple);
                return result;
            }
        }
        // return convert_to_array<F, decltype(myTuple), arraySize>(
        //     std::forward<F>(f), myTuple);
    }
};

} // namespace invoke_inteq_details
template <class F, class... Args>
constexpr auto invoke_intseq(F &&f, Args &&...args) -> decltype(auto) {
    if constexpr (!invoke_inteq_details::any_match<Args...>::value ||
                  sizeof...(args) == 0) {
        // Direct invocation for cases where special processing is not required
        return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    } else {
        return invoke_inteq_details::Comb_gen::apply_to_comb(
            std::forward<F>(f), std::forward<Args>(args)...);
    }
}

// FIXME:
static_assert(invoke_inteq_details::calc_size(2, 2) == 1, "this");
static_assert(
    invoke_inteq_details::calc_size(7, std::integer_sequence<int, 0, 1>{},
                                    std::integer_sequence<int, 4, 5>{}) == 4,
    "this");
static_assert(invoke_inteq_details::calc_size() == 1, "this");
static_assert(
    invoke_inteq_details::calc_size(std::integer_sequence<int, 0, 1>{},
                                    std::integer_sequence<int, 0, 1>{},
                                    std::integer_sequence<int, 0, 1>{}) == 8,
    "this");
static_assert(
    invoke_inteq_details::calc_size(std::integer_sequence<int, 0, 1>{}, 2) == 2,
    "this");
static_assert(
    invoke_inteq_details::calc_size(std::integer_sequence<int, 4, 4>{}) == 2,
    "this");
#endif // !INVOKE_INTSEQ

#ifndef TEST
#define TEST
#include <array>
#include <functional>
#include <iostream>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

// Helper to print a tuple
// fun0 for short
// Prints elements of tuple with indexes contained in index_sequence.
// for example, invoking it with index_sequence<2, 4> will print the 3rd and 5th
// elements of the tuple.
// index_sequence is integer sequence for type size_t
template <typename Tuple, std::size_t... Is>
void print_tuple_impl(const Tuple &t, std::index_sequence<Is...>) {
    ((std::cout << std::get<Is>(t) << (Is < sizeof...(Is) - 1 ? " " : "\n")),
     ...);
}

// fun1 for short
// Specialization for 0
template <typename... T> void print_tuple(const std::tuple<T...> &t) {
    print_tuple_impl(t, std::index_sequence_for<T...>{});
}

// fun2 for short
// Recursive function to generate and print combinations
template <typename T, T... Ints, typename... Rest>
void print_combinations_impl(const std::tuple<Rest...> &acc,
                             std::integer_sequence<T, Ints...>) {
    (print_tuple(std::tuple_cat(acc, std::make_tuple(Ints))), ...);
}

// fun3 for short
// specialisation for 2
template <typename T, T... HeadInts, typename... TailSeq,
          typename... Accumulated>
void print_combinations_impl(
    const std::tuple<Accumulated...> &acc, // tuplarz
    std::integer_sequence<T, HeadInts...>, // first integer sequence
    TailSeq... rest)                       // rest of integer sequences
{
    (..., // fold prints using comma operator
          // this does nothing except printing, results are discarded
     print_combinations_impl(std::tuple_cat(acc, std::make_tuple(HeadInts)),
                             rest...));
    /*
     * We have 3 seqences, since we called fun4({1, 2}, {3, 4}, {5, 6})
     * The first invocation of this function occurs in fun4 and is like this:
     * fun3({}, {1, 2}, {3, 4}, {5, 6})
     * Now, the fold unfolds.
     * First, we have:
     * (fun3({1}, {3, 4}, {5, 6}), fun3({2}, {3, 4}, {5, 6}))
     * Before being executed it unfolds, stil using fun3, because it is the best
     * match Now the fold looks like this: (fun3({1, 3}, {5, 6}), fun3({1, 4},
     * {5, 6}), fun3({2, 3}, {5, 6}), fun3({2, 4}, {5, 6}))
     * <----------------------------| Now, the fold unfolds again, this time
     * using fun2, because it is the best match | So, fun3 is acctually fun2,
     * and the fold looks like this (there is no new function - instead compiler
     * realised that this best matches fun2):: (fun2({1, 3}, {5, 6}), fun2({1,
     * 4}, {5, 6}), fun2({2, 3}, {5, 6}), fun2({2, 4}, {5, 6})) Now, lets look
     * at the first element of the fold: fun2({1, 3}, {5, 6}) It invokes
     * something that looks like: for (ii in Inst) {
     * print_tuple(std::tuple_cat(acc, std::make_tuple(ii)));
     * },
     * and print_tuple (fun1) prints the tuple (no fancy shit, just elegant
     * printing, no recursion, no nothing - simple loops).
     *
     */
}

// fun4 for short
template <typename... Seqs> void print_all_combinations(Seqs... seqs) {
    auto tuplarz = std::tuple<>{};
    print_combinations_impl(tuplarz, seqs...); // Calls 3
}

// Struct version
// Same logic, just different look
struct TuplePrinter {
    template <typename Tuple, std::size_t... Is>
    static void print_impl(const Tuple &t, std::index_sequence<Is...>) {
        ((std::cout << std::get<Is>(t)
                    << (Is < sizeof...(Is) - 1 ? " " : "\n")),
         ...);
    }

    template <typename... T> static auto print(const std::tuple<T...> &t) {
        print_impl(t, std::index_sequence_for<T...>{});
    }

    template <typename T> static auto print(const T &t) {
        std::cout << t << " ";
    }
};

struct CombinationsGenerator {
    // Base case for the recursion - when only one sequence is left
    template <typename T, T... Ints, typename... Accumulated>
    static auto generate_impl(const std::tuple<Accumulated...> &acc,
                              std::integer_sequence<T, Ints...>) {
        return std::make_tuple(std::tuple_cat(acc, std::make_tuple(Ints))...);
    }

    template <typename T, typename... Accumulated>
    static auto generate_impl(const std::tuple<Accumulated...> &acc, T val) {
        return std::make_tuple(std::tuple_cat(acc, std::make_tuple(val)));
    }

    // Recursive case - handles multiple integer sequences
    template <typename T, typename... TailSeq, typename... Accumulated>
    static auto generate_impl(const std::tuple<Accumulated...> &acc, T val,
                              TailSeq... rest) {
        // Make one tuple from multiple tuples
        return generate_impl(std::tuple_cat(acc, std::make_tuple(val)),
                             rest...);
    }

    // Recursive case - handles multiple integer sequences
    template <typename T, T... HeadInts, typename... TailSeq,
              typename... Accumulated>
    static auto generate_impl(const std::tuple<Accumulated...> &acc,
                              std::integer_sequence<T, HeadInts...>,
                              TailSeq... rest) {
        // Make one tuple from multiple tuples
        return std::apply(
            [](auto &&...args) { return std::tuple_cat(args...); },
            std::make_tuple(generate_impl(
                std::tuple_cat(acc, std::make_tuple(HeadInts)), rest...)...));
    }

    template <typename... Seqs> static auto generate(Seqs... seqs) {
        auto acc = std::tuple<>{};
        return generate_impl(acc, seqs...);
    }

    // Forward declaration with universal references
    template <typename... Args>
    constexpr static size_t calc_size(Args &&...args);

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

    // Helper function to apply F to each element of a tuple
    template <class F, class Tuple, size_t... I>
    constexpr static auto apply_to_tuple(F &&f, Tuple &&t,
                                         std::index_sequence<I...>) {
        return std::invoke(std::forward<F>(f),
                           std::get<I>(std::forward<Tuple>(t))...);
    }

    template <size_t Index, size_t Size, class F, class Array,
              class TwoDimTuple>
    constexpr static void process_tuples(F &&f, Array &arr,
                                         const TwoDimTuple &t) {
        if constexpr (Index < Size) {
            arr[Index] = apply_to_tuple(
                std::forward<F>(f), std::get<Index>(t),
                std::make_index_sequence<std::tuple_size_v<
                    std::tuple_element_t<Index, TwoDimTuple>>>{});
            process_tuples<Index + 1, Size>(std::forward<F>(f), arr, t);
        }
    }

    template <class F, class TwoDimTuple, size_t Size>
    constexpr static auto convert_to_array(F &&f, const TwoDimTuple &t) {
        using ReturnType = decltype(apply_to_tuple(
            std::forward<F>(f), std::get<0>(t),
            std::make_index_sequence<
                std::tuple_size_v<std::tuple_element_t<0, TwoDimTuple>>>{}));

        std::array<ReturnType, Size> arr{};
        process_tuples<0, Size>(std::forward<F>(f), arr, t);
        return arr;
    }

    template <class F, typename... Seqs>
    constexpr static auto apply_to_something_weird(F &&f, Seqs &&... seqs) {
        auto myTuple = generate(seqs...);
        constexpr size_t arraySize = std::tuple_size<decltype(myTuple)>::value;
        return convert_to_array<F, decltype(myTuple), arraySize>( std::forward<F>(f), myTuple);
    }
};
#endif // TEST

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
// 0
// Prints elements of tuple with indexes contained in index_sequence.
// for example, invoking it with index_sequence<2, 4> will print the 3rd and 5th
// elements of the tuple.
// index_sequence is integer sequence for type size_t
template <typename Tuple, std::size_t... Is>
void print_tuple_impl(const Tuple &t, std::index_sequence<Is...>) {
    ((std::cout << std::get<Is>(t) << (Is < sizeof...(Is) - 1 ? " " : "\n")),
     ...);
}

// 1
// Specialization for 0
template <typename... T> void print_tuple(const std::tuple<T...> &t) {
    print_tuple_impl(t, std::index_sequence_for<T...>{});
}

// 2
// Recursive function to generate and print combinations
template <typename T, T... Ints, typename... Rest>
void print_combinations_impl(const std::tuple<Rest...> &acc,
                             std::integer_sequence<T, Ints...>) {
    (print_tuple(std::tuple_cat(acc, std::make_tuple(Ints))), ...);
}

// 3
// specialisation for 2
template <typename T, T... HeadInts, typename... TailSeq,
          typename... Accumulated>
void print_combinations_impl(const std::tuple<Accumulated...> &acc, // tuplarz
                             std::integer_sequence<T, HeadInts...>, // first integer sequence
                             TailSeq... rest)  // rest of integer sequences
{
    (..., (print_combinations_impl(std::tuple_cat(acc, std::make_tuple(HeadInts)), rest...)));

}

// 4
template <typename... Seqs> void print_all_combinations(Seqs... seqs) {
    auto tuplarz = std::tuple<>{};
    print_combinations_impl(tuplarz, seqs...); // Calls 3
}


// Struct version
struct TuplePrinter {
    template <typename Tuple, std::size_t... Is>
    static void print_impl(const Tuple &t, std::index_sequence<Is...>) {
        ((std::cout << std::get<Is>(t) << (Is < sizeof...(Is) - 1 ? " " : "\n")),
         ...);
    }

    template <typename... T>
    static void print(const std::tuple<T...> &t) {
        print_impl(t, std::index_sequence_for<T...>{});
    }
};

struct CombinationsGenerator {
    template <typename T, T... Ints, typename... Rest>
    static void generate_impl(const std::tuple<Rest...> &acc,
                              std::integer_sequence<T, Ints...>) {
        (TuplePrinter::print(std::tuple_cat(acc, std::make_tuple(Ints))), ...);
    }

    template <typename T, T... HeadInts, typename... TailSeq, typename... Accumulated>
    static void generate_impl(const std::tuple<Accumulated...> &acc,
                              std::integer_sequence<T, HeadInts...>,
                              TailSeq... rest) {
        (..., (generate_impl(std::tuple_cat(acc, std::make_tuple(HeadInts)), rest...)));
    }

    template <typename... Seqs>
    static void generate(Seqs... seqs) {
        auto acc = std::tuple<>{};
        generate_impl(acc, seqs...);
    }
};
#endif // TEST

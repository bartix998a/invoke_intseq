#ifndef TEST
#define TEST
#include <functional>
#include <iostream>
#include <type_traits>
#include <optional>
#include <tuple>

// Helper to print a tuple
template<typename Tuple, std::size_t... Is>
void print_tuple_impl(const Tuple& t, std::index_sequence<Is...>) {
    ((std::cout << std::get<Is>(t) << (Is < sizeof...(Is) - 1 ? ", " : "\n")), ...);
}

template<typename... T>
void print_tuple(const std::tuple<T...>& t) {
    print_tuple_impl(t, std::index_sequence_for<T...>{});
}

// Recursive function to generate and print combinations
template<typename T, T... Ints, typename... Rest>
void print_combinations_impl(const std::tuple<Rest...>& acc, std::integer_sequence<T, Ints...>) {
    (print_tuple(std::tuple_cat(acc, std::make_tuple(Ints))), ...);
}

template<typename T, T... HeadInts, typename... TailSeq, typename... Accumulated>
void print_combinations_impl(const std::tuple<Accumulated...>& acc, std::integer_sequence<T, HeadInts...>, TailSeq... rest) {
    (..., (print_combinations_impl(std::tuple_cat(acc, std::make_tuple(HeadInts)), rest...)));
}

template<typename... Seqs>
void print_all_combinations(Seqs... seqs) {
    print_combinations_impl(std::tuple<>{}, seqs...);
}


#endif // TEST

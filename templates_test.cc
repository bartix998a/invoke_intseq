#include "templates_test.h"
#include <utility>
// Helper function to print an individual tuple
template <typename Tuple, std::size_t... Is>
void printTupleImpl(const Tuple& t, std::index_sequence<Is...>) {
}

// Overload for printing a tuple
template <typename... T>
void printTuple(const std::tuple<T...>& t) {
    std::cout << "(";
    printTupleImpl(t, std::index_sequence_for<T...>{});
    std::cout << ")";
}

// Function to print the entire set of combinations
template <typename... Tuples>
void printCombinations(const std::tuple<Tuples...>& combinations) {
    std::cout << "(";
    std::apply(
        [&](const auto&... tuple) {
            int index = 0;
            ((printTuple(tuple), std::cout << (++index < sizeof...(Tuples) ? ", " : "")), ...);
        },
        combinations
    );
    std::cout << ")\n";
}

int main() {
    auto seq1 = std::integer_sequence<int, 0, 1>{};
    auto seq2 = 2;
    auto seq3 = std::integer_sequence<int, 4, 5>{};

    // Start measuring time for compile-time execution
    // print_all_combinations(
    //     seq1,
    //     seq2,
    //     seq3
    // );

    // print_tuple_impl(std::make_tuple(0, 1, 2, 3, 4), std::index_sequence<2, 4>());

    // Struct version
    auto combinations = CombinationsGenerator::generate(seq1, seq3, seq2);
    std::cout << "\n===========\n";
    std::cout << std::get<0>(std::get<0>(combinations)) << " ";
    std::cout << std::get<1>(std::get<0>(combinations)) << " ";
    std::cout << std::get<2>(std::get<0>(combinations)) << "\n";

    std::cout << std::get<0>(std::get<1>(combinations)) << " ";
    std::cout << std::get<1>(std::get<1>(combinations)) << " ";
    std::cout << std::get<2>(std::get<1>(combinations)) << "\n";

    std::cout << std::get<0>(std::get<2>(combinations)) << " ";
    std::cout << std::get<1>(std::get<2>(combinations)) << " ";
    std::cout << std::get<2>(std::get<2>(combinations)) << "\n";

    std::cout << std::get<0>(std::get<3>(combinations)) << " ";
    std::cout << std::get<1>(std::get<3>(combinations)) << " ";
    std::cout << std::get<2>(std::get<3>(combinations)) << "\n";

    // std::cout << std::get<0>(std::get<4>(combinations)) << " ";
    // std::cout << std::get<1>(std::get<4>(combinations)) << " ";
    // std::cout << std::get<2>(std::get<4>(combinations)) << "\n";
    //
    // std::cout << std::get<0>(std::get<5>(combinations)) << " ";
    // std::cout << std::get<1>(std::get<5>(combinations)) << " ";
    // std::cout << std::get<2>(std::get<5>(combinations)) << "\n";
    //
    // std::cout << std::get<0>(std::get<6>(combinations)) << " ";
    // std::cout << std::get<1>(std::get<6>(combinations)) << " ";
    // std::cout << std::get<2>(std::get<6>(combinations)) << "\n";
    //
    // std::cout << std::get<0>(std::get<7>(combinations)) << " ";
    // std::cout << std::get<1>(std::get<7>(combinations)) << " ";
    // std::cout << std::get<2>(std::get<7>(combinations)) << "\n";

    std::cout << "\n";
    return 0;
}

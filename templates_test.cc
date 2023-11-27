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
    auto seq2 = std::integer_sequence<int, 6, 9>{};
    auto seq3 = std::integer_sequence<int, 4, 5>{};

    // Start measuring time for compile-time execution
    // print_all_combinations(
    //     seq1,
    //     seq2,
    //     seq3
    // );

    // print_tuple_impl(std::make_tuple(0, 1, 2, 3, 4), std::index_sequence<2, 4>());
    // constexpr size_t SIZE = CombinationsGenerator::calc_size(seq1, seq2, seq3);

    auto combinations = CombinationsGenerator::generate(seq1, seq3, seq2);
    auto func = [](const auto&... args) -> int {
        return (0 + ... + args); // Fold expression to sum the elements
    };

    auto func2 = [](const auto&... args) {
    };
    auto resultArray = CombinationsGenerator::apply_to_something_weird(func, seq1, seq3, seq2, seq1, seq2, 4);
    // auto resultArray2 = CombinationsGenerator::apply_to_something_weird(func2, seq1, seq3, seq2, seq1, seq2, 4);
    for (auto i : resultArray) {
        std::cout << i << " ";
    }
    std::cout << resultArray.size() << "\n";
        
    // for (auto i : resultArray2) {
    //     std::cout << i << " ";
    // }
    // std::cout << resultArray2.size() << "\n";
    // auto resultArray = convert_to_array([](auto a...) { return (a + ...); }, combinations, SIZE);

    // Struct version
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

#include "templates_test.h"
#include "invoke_intseq.h"
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
    auto seq1 = std::integer_sequence<int, 0, 9>{};
    auto seq2 = std::integer_sequence<int, 6, 8>{};
    auto seq3 = std::integer_sequence<int, 4, 3>{};

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

    auto func2 = [](const auto&... args) -> std::string {
        return "siema";
    };
    auto resultArray = CombinationsGenerator::apply_to_something_weird(func, std::integer_sequence<int, 1, 2, 3>(), seq3, seq2, 4);
    auto resultArray2 = CombinationsGenerator::apply_to_something_weird(func2, 4, seq1, 2, 3, 5, seq3, seq2, seq2);
    for (auto i : resultArray) {
        std::cout << i << "\n";
    }

    constexpr auto make_number = [](auto x, auto y, auto z) {
        std::cout << "MN\n";
        return 100 * x + 10 * y + z;
    };
    static_assert(invoke_inteq_details::calc_size(std::integer_sequence<int>(), 2) == 0, "this");
    auto res = CombinationsGenerator::apply_to_something_weird([](auto...) { return 0; }, std::integer_sequence<int, 1, 2, 3>(), seq1, 5);
    static_assert(std::ranges::range<decltype(invoke_intseq([](auto...) { return 0; }, std::integer_sequence<int, 1, 2, 3>(), 4, 5))>);
    auto res2 = CombinationsGenerator::apply_to_something_weird(make_number, std::integer_sequence<int, 1, 2, 3>(), seq1, 5);
    std::cout << resultArray.size() << "\n";
    auto res3 = invoke_intseq(make_number, 1, std::integer_sequence<int, 2, 3>(), std::integer_sequence<int, 4, 5>());
    auto exp = std::array{124, 125, 134, 135};
    std::cout << "\n===========\n";
    std::cout << "Res3";
    std::cout << "\n===========\n";
    for (auto i : res3) {
        std::cout << i << "\n";
    }
    std::cout << "\n===========\n";



    std::cout << "\n===========\n";
    std::cout << "\n===========\n";
    for (auto i : res2) {
        std::cout << i << "\n";
    }
    std::cout << "\n===========\n";
    std::cout << "\n===========\n";
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

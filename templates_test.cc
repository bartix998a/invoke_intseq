#include "templates_test.h"
#include <utility>

int main() {
    auto seq1 = std::integer_sequence<int, 1, 2>{};
    auto seq2 = std::integer_sequence<int, 3, 4>{};
    auto seq3 = std::integer_sequence<int, 5, 6>{};

    // Start measuring time for compile-time execution
    print_all_combinations(
        seq1,
        seq2,
        seq3
    );

    // print_tuple_impl(std::make_tuple(0, 1, 2, 3, 4), std::index_sequence<2, 4>());

    // Struct version
    CombinationsGenerator::generate(seq1, seq2, seq3);
    return 0;
}

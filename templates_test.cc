#include "templates_test.h"

int main() {
    auto seq1 = std::integer_sequence<int, 1, 2>{};
    auto seq2 = std::integer_sequence<int, 3, 4>{};
    auto seq3 = std::integer_sequence<int, 5, 6>{};
    auto seq4 = std::integer_sequence<int, 7, 8>{};

    print_all_combinations(seq1, seq2, seq3, seq4);

    return 0;
}


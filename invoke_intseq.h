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

template <typename T> struct is_integer_sequence : std::false_type {
    using type = T;
};

template <typename T, T... vals>
struct is_integer_sequence<std::integer_sequence<T, vals...>> : std::true_type {
    using type = std::integral_constant<T, 0>;
};

// checks if any of the others are an integer_sequence
template <class... others>
struct any_match : std::disjunction<is_integer_sequence<others>...> {};

// Trait to check if an std::integer_sequence is empty
template <typename T> struct is_empty_integer_sequence : std::false_type {};

template <typename T>
struct is_empty_integer_sequence<std::integer_sequence<T>> : std::true_type {};

// Check if any of the arguments is an empty integer sequence
template <class... others>
struct any_empty_integer_sequence
    : std::disjunction<is_empty_integer_sequence<others>...> {};

// START
// Now, create a utility to transform a tuple of arguments
template <typename F, typename... Args> struct transform_arg_types {
    using type =
        std::invoke_result_t<F, typename is_integer_sequence<Args>::type...>;
};

// STOP

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

template <typename FResult> struct Comb_gen {
    // IMPORTANT: Four of the methods below are for recurrent generation of
    // all possible combinations of integer sequences.
    // They are splitted into two categories:
    // 1. next element to process is an integer sequence
    // 2. next element to process is a single integer
    //
    // Each category is splited into generic ad base cases.

    // Base case for the recursion - when only one sequence is left
    // Base for category 1

    template <typename T1, typename T2, typename... TailSeq>
    struct gen_impl_str {
        constexpr static auto generate_impl() {
            return; // no tuple so no nothing essentially
        }
    };

    template <typename... Accumulated, typename T, typename... TailSeq>
    struct gen_impl_str<std::tuple<Accumulated &&...>, T, TailSeq...> {
        constexpr static auto
        generate_impl(const std::tuple<Accumulated &&...> &acc, T &&val,
                      TailSeq &&...rest) {
            // Make one tuple from multiple tuples
            std::tuple<T &&> tup(std::forward<T>(val));
            return gen_impl_str<std::tuple<Accumulated &&..., T &&>,
                                TailSeq...>::
                generate_impl(
                    std::tuple_cat(
                        std::forward<std::tuple<Accumulated &&...>>(acc),
                        std::forward<std::tuple<T &&>>(tup)),
                    std::forward<TailSeq>(rest)...);
        }
    };

    // Recursive case - handles multiple integer sequences
    // Recursive case for category 2
    template <typename T, T... HeadInts, typename... TailSeq,
              typename... Accumulated>
    struct gen_impl_str<std::tuple<Accumulated &&...>,
                        std::integer_sequence<T, HeadInts...>, TailSeq...> {
        constexpr static auto
        generate_impl(const std::tuple<Accumulated &&...> &acc,
                      std::integer_sequence<T, HeadInts...>,
                      TailSeq &&...rest) {
            // Make one tuple from multiple tuples
            return std::apply(
                [](auto &&...args) { return std::tuple_cat(args...); },
                std::make_tuple((
                    gen_impl_str<
                        std::tuple<Accumulated &&...,
                                   std::integral_constant<T, HeadInts> &&>,
                        TailSeq...>::
                        generate_impl(
                            std::tuple_cat(
                                std::forward<std::tuple<Accumulated &&...>>(
                                    acc),
                                std::tuple<std::integral_constant<T, HeadInts>>(
                                    std::integral_constant<T, HeadInts>())),
                            std::forward<TailSeq>(rest)...))...));
        }
    };

    template <typename T, T... Ints, typename... Accumulated>
    struct gen_impl_str<std::tuple<Accumulated &&...>,
                        std::integer_sequence<T, Ints...>> {
        constexpr auto generate_impl(const std::tuple<Accumulated &&...> &acc,
                                     std::integer_sequence<T, Ints...>) {
            return std::make_tuple(std::tuple_cat(
                std::forward<std::tuple<Accumulated &&...>>(acc),
                std::make_tuple(std::integral_constant<T, Ints>()))...);
        }
    };

    // Base for category 2
    template <typename T, typename... Accumulated>
    struct gen_impl_str<std::tuple<Accumulated &&...>, T> {
        constexpr static auto
        generate_impl(const std::tuple<Accumulated &&...> &acc, T &&val) {
            std::tuple<T &&> tup(std::forward<T>(val));
            return std::make_tuple(
                std::tuple_cat(std::forward<std::tuple<Accumulated &&...>>(acc),
                               std::forward<std::tuple<T &&>>(tup)));
        }
    };

    // Generates all possible combinations of integer sequences.
    // Ex.:
    // generate(std::integer_sequence<int, 0, 1>{}, 2,
    // std::integer_sequence<int, 3, 4>{}) will generate
    // {{0, 2, 3}, {0, 2, 4}, {1, 2, 3}, {1, 2, 4}}
    // (where {} denotes a tuple)
    template <typename... Args> constexpr static auto generate(Args &&...args) {
        auto acc = std::tuple<>{};
        return gen_impl_str<std::tuple<>, Args...>::generate_impl(
            acc, std::forward<Args>(args)...);
    }

    template <class F, typename... Args>
    constexpr static auto apply_to_comb(F &&f, Args &&...args)
        -> decltype(auto) {

        auto invoke_results = generate(std::forward<Args>(args)...);
        if constexpr (std::tuple_size<decltype(invoke_results)>::value != 0) {

            if constexpr (std::is_same_v<void, FResult>) {
                std::apply(
                    [&f](auto &&...x) {
                        (..., std::apply(std::forward<F>(f), x));
                    },
                    std::forward<>(invoke_results));

            } else if constexpr (!std::is_reference_v<FResult>) {
                std::vector<FResult> result{};

                std::apply(
                    [&](auto &&...x) {
                        (...,
                         result.push_back(std::apply(std::forward<F>(f), x)));
                    },
                    std::forward<>(invoke_results));
                return result;
            } else {
                using temp_for_sure_fixme = std::remove_reference_t<FResult>;
                std::vector<temp_for_sure_fixme> result{};
                // FIXME: just a test:forward
                // result.push_back();

                std::apply(
                    [&](auto &&...x) {
                        (...,
                         result.push_back((std::apply(std::forward<F>(f), x))));
                    },
                    std::forward<>(invoke_results));
                return result;
            }
        }
    }
};

} // namespace invoke_inteq_details

template <class F, class... Args>
constexpr auto invoke_intseq(F &&f, Args &&...args) -> decltype(auto) {
    using result_type =
        invoke_inteq_details::transform_arg_types<decltype(f), Args...>::type;

    if constexpr (!invoke_inteq_details::any_match<Args...>::value ||
                  sizeof...(args) == 0) {
        // Direct invocation for cases where special processing is not required
        return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    } else if constexpr (!invoke_inteq_details::any_empty_integer_sequence<
                             Args...>::value) {
        return invoke_inteq_details::Comb_gen<result_type>::apply_to_comb(
            std::forward<F>(f), std::forward<Args>(args)...);
    } else {
        return std::vector<result_type>{};
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

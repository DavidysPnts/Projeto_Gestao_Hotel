#ifndef CONCEPTS_HPP
#define CONCEPTS_HPP

#include <concepts>
#include <vector>

template <typename T>
concept Calculable = requires(const T& t, int nights) {
    { t.calculate_price(nights) } -> std::convertible_to<float>;
};

template <Calculable T>
float total_price(const std::vector<T>& items, int nights) {
    float total = 0.0f;
    for (const auto& item : items) {
        total += item.calculate_price(nights);
    }
    return total;
}

#endif
#include "vision/algorithms/utils/classification.hpp"
#include <iostream>
#include <numeric>
#include <vector>

int main () {
    Classification test_operators;
    // clang-format off
    points_t ref = {
        { 1, { 0, 10 } }, { 2, { 10, 10 }},
        { 3, { 0, 0 } }, { 4, { 10, 0 } }
    };
    points_t points_scaled = {
        { 1, { 0, 5 } }, { 2, { 5, 5 } },
        { 3, { 0, 0 } }, { 4, { 5, 0 } }
    };
    // clang-format on
    std::cout << "classification demo" << std::endl;
    std::cout << "scale: " << test_operators.scale (ref, points_scaled, 5) << std::endl;
    return 0;
}

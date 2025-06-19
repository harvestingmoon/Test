#include "Application.hpp"
#include <iostream>

int main() {
    try {
        Application app(1000, 800, "K-Means Clustering with SFML & Eigen");
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
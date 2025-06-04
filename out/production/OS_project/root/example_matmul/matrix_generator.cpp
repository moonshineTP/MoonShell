#include <iostream>
#include <fstream>
#include <vector>
#include <random>

void write_random_matrix(const std::string& filename, int rows, int cols,
    std::uniform_int_distribution<>& dist, std::mt19937& gen) {
    std::ofstream file(filename);
    file << rows << "," << cols << "\n";
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            file << dist(gen); // Random int between 1 and 99
            if (j < cols - 1) file << ",";
        }
        file << "\n";
    }
    file.close();
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        throw std::invalid_argument("Usage: matrix_generator. <shape0> <shape1> <shape2>");
    }

    int shape0 = std::atoi(argv[1]);
    int shape1 = std::atoi(argv[2]);
    int shape2 = std::atoi(argv[3]);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 99);

    // Generate matrix1: shape0 x shape1
    write_random_matrix("matrix1.csv", shape0, shape1, dist, gen);

    // Generate matrix2: shape1 x shape2
    write_random_matrix("matrix2.csv", shape1, shape2, dist, gen);

    std::cout << "Matrices generated successfully in:\n";
    std::cout << "matrix1.csv (" << shape0 << " x " << shape1 << ")\n";
    std::cout << "matrix2.csv (" << shape1 << " x " << shape2 << ")\n";
    return 0;
}
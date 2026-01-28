// You can use a command like this to measure cache misses:
//
// `sudo perf stat -e L1-dcache-load-misses,LLC-load-misses ./mm.exe 1000`
//
// See if you can determine the optimal loop structure without perf, and then
// evaluate the choice using perf.  You may need to redesign some other aspects
// of the code to get everything to work correctly.

#include <chrono>
#include <cstring>
#include <iostream>
#include <random>
#include <string>

/// Allocate a square matrix as an array of arrays
///
/// @param n The dimension of the matrix
double **allocateMatrix(int n) {
  double **matrix = new double *[n];
  for (int i = 0; i < n; ++i)
    matrix[i] = new double[n];
  return matrix;
}

/// Free a square matrix (array of arrays)
///
/// @param matrix The matrix to free
/// @param n      The dimension of the matrix
void freeMatrix(double **matrix, int n) {
  for (int i = 0; i < n; ++i)
    delete[] matrix[i];
  delete[] matrix;
}

/// Fill a square matrix with random doubles
///
/// @param matrix The matrix to fill
/// @param n      The dimension of the matrix
void fillMatrix(double **matrix, int n) {
  std::random_device device;
  std::mt19937 generator(device());
  std::uniform_real_distribution<> distribution(0.0, 10.0);
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j)
      matrix[i][j] = distribution(generator);
}

int main(int argc, char *argv[]) {
  // Validate command-line arguments
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <matrix_size>" << std::endl;
    return 1;
  }
  int n = std::stoi(argv[1]);
  if (n <= 0)
    return 1;

  // Allocate memory for three matrices, and fill them
  double **A = allocateMatrix(n);
  double **B = allocateMatrix(n);
  double **C = allocateMatrix(n);
  fillMatrix(A, n);
  fillMatrix(B, n);

  // Initialize the output array outside of the main loop
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j)
      C[i][j] = 0.0;

  // Perform the multiplication from a timed block
  auto start_time = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      for (int k = 0; k < n; ++k) {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
  auto end_time = std::chrono::high_resolution_clock::now();

  // Report the result, then clean up
  auto dur = std::chrono::duration_cast<std::chrono::duration<double>>(
                 end_time - start_time)
                 .count();
  std::cout << "Successfully multiplied two " << n << "x" << n << " arrays in "
            << dur << " seconds." << std::endl;
  freeMatrix(A, n);
  freeMatrix(B, n);
  freeMatrix(C, n);
}
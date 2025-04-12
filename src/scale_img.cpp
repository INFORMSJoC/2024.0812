/**
 * @file scale_img.cpp
 * @brief This program reads a graph with floating-point edge weights and
 * outputs the same graph with scaled integer weights.
 *
 * The program reads a graph from standard input, where the first line contains
 * the number of nodes and the number of edges. Subsequent lines describe the
 * edges, each with a source node, a destination node, and a floating-point
 * weight. The program scales each floating-point weight by 1.0e8 (100,000,000)
 * and rounds it to the nearest integer. It then outputs the graph with these
 * scaled integer weights.
 *
 * Input Format:
 *   The input is read from standard input (std::cin).
 *   - The first line contains two integers: n (number of nodes) and m (number
 *     of edges).
 *   - Subsequent lines (m lines) each contain three values: i j w, representing
 *     an edge from node i to node j with floating-point weight w.
 *   - Lines starting with '#' or containing only spaces are considered comments
 *     and are ignored.
 *
 * Output Format:
 *   The output is written to standard output (std::cout).
 *   - The first line contains two integers: n (number of nodes) and m (number
 *     of edges).
 *   - Subsequent lines (m lines) each contain three values: i j W, representing
 *     an edge from node i to node j with scaled integer weight W.
 *   - The scaled weight W is calculated as round(w * 1.0e8).
 *
 * Example Input:
 *   # This is a comment
 *   3 3
 *   1 2 1.2345
 *   2 3 -0.5000000023
 *   1 3 0.8
 *
 * Example Output:
 *   3 3
 *   1 2 123450000
 *   2 3 -50000000
 *   1 3 80000000
 */
#include <iostream>
#include <cmath>
#include <limits>

using namespace std;

// Function to skip comment lines or lines with only spaces in an input stream.
// Comments start with '#' and extend to the end of the line.
void skipComment(istream& f) {
  char ch;  // Temporary character to hold the read char
  // Read characters until a non-comment/non-space character is encountered or
  // the end of the stream is reached.
  while (f.get(ch) && (ch == '#' || ch == ' ')) {
    // Ignore all characters until the end of the line.
    f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  // If a non-comment/non-space character was found, put it back into the
  // stream.
  if (f.good()) {
    f.unget();
  }
}

int main() {
  int n, m, i, j;   // n: Number of nodes, m: Number of edges, i, j: Node indices
  double w;         // w: Weight of an edge (floating-point)
  long long int W;  // W: Scaled weight of an edge (integer)

  skipComment(std::cin);  // Skip any leading comments or blank lines in the
                          // standard input

  // Read the number of nodes (n) and edges (m) from the standard input.
  cin >> n >> m;

  // Output the number of nodes and edges to the standard output.
  cout << n << " " << m << endl;

  // Iterate through each of the 'm' edges.
  for (int k = 0; k != m; ++k) {
    // Read the source node (i), destination node (j), and weight (w) of the edge.
    cin >> i >> j >> w;

    // Scale the weight 'w' by 1.0e8 (100,000,000) and round it to the nearest integer.
    // This effectively converts a floating-point weight to a fixed-point representation
    // with 8 decimal places of precision.
    W = round(w * 1.0e8);

    // Output the source node (i), destination node (j), and the scaled weight (W) to the standard output.
    cout << i << " " << j << " " << W << endl;
  }
 
  return EXIT_SUCCESS;
}

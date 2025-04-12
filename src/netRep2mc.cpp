/**
 * @file netRep2mc.cpp
 * @brief This program converts a network representation to a format suitable for a max-cut/qubo solve, adding a weight of 1 to each edge.
 *
 * The program reads a network (graph) description from standard input. This description includes the number of nodes in two partitions (n1, n2) and the number of edges (m).
 * Each subsequent line describes an edge between two nodes. The program then outputs a modified representation of the network,
 * where each edge is assigned a weight of 1. The output format is designed to be compatible with certain model checking tools.
 *
 * Input Format:
 *   The input is read from standard input (std::cin).
 *   - The first line contains three integers: n1 (number of nodes in the first partition), n2 (number of nodes in the second partition), and m (number of edges).
 *   - Subsequent lines (m lines) each contain two integers: i j, representing an edge between node i and node j.
 *   - Lines starting with '%' or containing only spaces are considered comments and are ignored.
 *
 * Output Format:
 *   The output is written to standard output (std::cout).
 *   - The first line contains two integers: n1 (number of nodes in the first partition) and m (number of edges).
 *   - Subsequent lines (m lines) each contain three values: i j 1, representing an edge between node i and node j with a weight of 1.
 */

#include <iostream>
#include <cmath>
#include <limits>

using namespace std;
using Index = unsigned int;

// Function to skip comment lines or lines with only spaces in an input stream.
// Comments start with '#' and extend to the end of the line.
void skipComment(istream& f) {
  char ch;  // Temporary character to hold the read char
  // Read characters until a non-comment/non-space character is encountered or
  // the end of the stream is reached.
  while (f.get(ch) && (ch == '%' || ch == ' ')) {
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
  Index n1, n2, m; // n1 and n2: Number of nodes, m: Number of edges
  int i, j;        // i, j: Node indices

  skipComment(std::cin);  // Skip any leading comments or blank lines in the
                          // standard input

  // Read the number of nodes (n) and edges (m) from the standard input.
  cin >> n1 >> n2 >> m;

  // Output the number of nodes and edges to the standard output.
  cout << n1 << " " << m << endl;

  // Iterate through each of the 'm' edges.
  for (Index k = 0; k != m; ++k) {
    // Read the source node (i) and destination node (j) of the edge.
    cin >> i >> j;

    // Output the source node (i), destination node (j), and weight 1
    cout << i << " " << j << " " << 1 << endl;
  }
 
  return EXIT_SUCCESS;
}

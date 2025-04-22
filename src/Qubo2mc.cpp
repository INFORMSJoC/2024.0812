/**
 * @file Qubo2mc.cpp
 * @brief This program converts a Quadratic Unconstrained Binary Optimization
 * (QUBO) problem instance into a Max-Cut problem instance.
 *
 * The program reads a QUBO instance from standard input, which describes a
 * symmetric n x n matrix Q. Column and row indices are in the set {1,..,n}.
 * The matrix is interpreted as an undirected weighted graph with n nodes and
 * edge (i,j) (with weight Q_ij) for every non-zero entry Q_ij of Q.
 * Diagonal enries of Q are loops of the graph. The code transforms this QUBO
 * instance into a Max-Cut instance by adding a new node (node 0) and connecting
 * it to existing nodes with specific weights. The weights of these new edges
 * are calculated based on the original edge weights connected to each node. The
 * program then outputs the transformed graph in a format suitable for Max-Cut
 * solvers.
 *
 * Input Format:
 *   The input is read from standard input (std::cin).
 *   - The first line contains two integers: n (size of matrix Q) and m (number
 *     of non-zero entris of Q) in the QUBO instance.
 *   - Subsequent lines (m lines) each contain three values: i j w, representing
 *     row and columns ndices of an entry with weight w.
 *   - Row/column indices are 1-based in the input.
 *   - Lines starting with '#' or containing only spaces are considered comments
 *     and are ignored.
 *
 * Output Format:
 *   The output is written to standard output (std::cout).
 *   - The first line contains two integers: n+1 (number of nodes, including the
 *     added node 0) and m' (the new number of edges).
 *   - Subsequent lines (m' lines) each contain three values: i j w,
 *     representing an edge between node i and node j with weight w.
 *   - Node indices are 1-based in the output.
 *   - The edges are sorted first by the source node and then by the destination
 *     node.
 *
 */
#include <algorithm>  // For sort
#include <cstdlib>    // For general utilities like EXIT_SUCCESS
#include <iostream>   // For input/output operations (cin, cout)
#include <limits>     // For numerical limits (e.g., max of streamsize)
#include <vector>     // For using dynamic arrays (vectors)

using namespace std;  // Use the standard namespace to avoid having to write
                      // std:: repeatedly
using Index = unsigned int;  // Define an alias 'Index' for unsigned int for
                             // better readability

// Structure to represent an edge in a graph
struct Edge {
  Index fr;    // 'fr' represents the 'from' node of the edge
  Index to;    // 'to' represents the 'to' node of the edge
  double wgt;  // 'wgt' represents the weight of the edge

  // Constructor to initialize an Edge object
  Edge(Index a, Index b, double c) : fr(a), to(b), wgt(c) {}
};

// Comparison function for sorting edges based on 'fr', then 'to'
bool compareEdges(const Edge& a, const Edge& b) {
  if (a.fr != b.fr) {
    return a.fr < b.fr;  // Sort by 'fr' in ascending order
  }
  return a.to < b.to;  // If 'fr' is the same, sort by 'to' in ascending order
}

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
  int n, m;  // 'n' represents the number of nodes, 'm' represents the number of
             // edges in the QUBO instance. As said, matrix Q is interpreted as
             // an undirected weighted graph with n nodes and possibly loops

  skipComment(std::cin);  // Skip any leading comments or blank lines in the
                          // standard input

  cin >> n >> m;  // Read the number of nodes and edges from the input

  vector<double> nodeSum(n, 0.0);  // Initialize a vector 'nodeSum' of
                                   // size 'n' with all elements set
                                   // to 0.0. This vector will store
                                   // the sum of weights of edges
                                   // connected to each node.
  vector<Edge> E;  // Initialize an empty vector 'E' to store the edges

  // Loop through each edge in the input (QUBO instance)
  for (Index k = 0; k != m; ++k) {
    Index i, j;  // 'i' and 'j' represent the nodes connected by the edge
    double w;    // 'w' represents the weight of the edge
    cin >> i >> j >> w;  // Read the edge information (nodes and weight)
    --i;                 // Adjust node indices to be 0-based (input is 1-based)
    --j;                 // Adjust node indices to be 0-based (input is 1-based)
    if (i != j) {        // If it's not a self-loop (i.e., a regular edge)
      E.push_back(
          Edge(i + 1, j + 1, w));  // Add the edge to the 'E' vector (nodes are
                                   // stored as 1-based for output)
      nodeSum[i] += w;             // Add the weight to the sum for node 'i'
      nodeSum[j] += w;             // Add the weight to the sum for node 'j'
    } else {                       // If it's a self-loop (i.e., i == j)
      nodeSum[i] +=
          1.0 * w;  // Add the weight to the sum for node 'i' (self-loop)
    }
  }

  // Add new edges to connect each node to a new node (node 0) based on nodeSum
  for (Index k = 0; k != n; ++k)
    // if the weight is not close to zero (avoiding adding edges with negligible
    // weights)
    if (nodeSum[k] < -1.0e-12 || nodeSum[k] > 1.0e-12)
      E.push_back(Edge(
          0, k + 1, -nodeSum[k]));  // Add an edge from node 0 to node k+1 with
                                    // weight -nodeSum[k] This is the core of
                                    // the QUBO to Max-Cut transformation.

  cout << n + 1 << " " << E.size()
       << endl;  // Output the number of nodes (n+1, including the new node 0)
                 // and the number of edges

  sort(E.begin(), E.end(),
       compareEdges);  // Sort the edges based on the 'compareEdges' function

  // Output the edges in the sorted order
  for (Index k = 0; k != E.size(); ++k)
    cout << E[k].fr + 1 << " " << E[k].to + 1 << " " << E[k].wgt
         << endl;  // Output the edge information (1-based indexing)

  return EXIT_SUCCESS;  // Indicate successful execution
}

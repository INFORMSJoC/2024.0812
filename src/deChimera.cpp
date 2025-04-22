/**
 * @file deChimera.cpp
 * @brief This program processes a graph represented by a list of edges and
 * removes isolated nodes.
 *
 * The program reads a graph from standard input, where the first line contains
 * the number of nodes and the number of edges. Subsequent lines describe the
 * edges, each with a source node, a destination node, and a weight. The program
 * remodes 0-weighted edges and then identifies and removes isolated nodes
 * (nodes with no connections) from the graph. It re-indexes the remaining nodes
 * to form a compact graph representation.
 *
 * The program performs the following steps:
 * 1. Reads the graph's structure (number of nodes and edges) from standard
 *    input.
 * 2. Reads each edge, storing the source, destination, and weight only if the
 *    weight is non-zero.
 * 3. Calculates the degree (number of connections) for each node.
 * 4. Identifies isolated nodes (nodes with a degree of 0).
 * 5. Creates a new indexing for non-isolated nodes, effectively removing the
 *    isolated ones.
 * 6. Outputs the new number of nodes (non-isolated) and the number of edges to
 *    standard output.
 * 7. Outputs the edges of the modified graph, using the new node indices, to
 *    standard output.
 * 8. Ensures that the source node index is always less than or equal to the
 *    destination node index for each edge.
 * 9. Skips comment lines or lines with only spaces in the input stream.
 * 10. Ignores edges with a weight of 0.
 *
 * Input Format:
 *   The input is read from standard input (std::cin).
 *   - The first line contains two integers: n (number of nodes) and m (number
 *     of edges).
 *   - Subsequent lines (m lines) each contain three values: f t w, representing
 *     an edge from node f to node t with weight w. Node indices are 1-based.
 *   - Lines starting with '#' or containing only spaces are considered comments
 *     and are ignored.
 *
 * Output Format:
 *   The output is written to standard output (std::cout).
 *   - The first line contains two integers: new_n (the number of non-isolated
 *     nodes) and m' (the number of edges with non-zero weight).
 *   - Subsequent lines (m' lines) each contain three values: f t w,
 *     representing an edge from node f to node t with weight w, using the new 
 *     node indices.
 *   - The node indices are 1-based.
 *
 */

#include <algorithm>  // For standard algorithms (e.g., std::swap)
#include <iostream>   // For input/output operations (cin, cout)
#include <limits>     // For numerical limits (e.g., max of streamsize)
#include <vector>     // For using dynamic arrays (vectors)

using Index = unsigned int;  // Define a type alias 'Index' for unsigned
                             // integers, representing array indices
using namespace std;  // Use the standard namespace to avoid having to write
                      // std:: repeatedly

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
  Index n, m;  // n: number of nodes, m: number of edges

  // read input graph
  skipComment(std::cin);  // Skip any leading comments or blank lines in the
                          // standard input
  std::cin >> n >>
      m;  // Read the number of nodes (n) and edges (m) from the standard input

  std::vector<Index> degree(
      n,
      0);  // Create a vector 'degree' of size 'n', initialized with zeros.
           // degree[i] will store the degree (number of connections) of node i.
  std::vector<tuple<Index, Index, double>>
      edges;  // Store edges as tuples
              // Each tuple represents an edge: (source node, destination node,
              // weight)

  edges.reserve(m);  // Pre-allocate space for 'm' edges in the 'edges' vector
                     // to avoid frequent reallocations

  // Loop through each edge
  for (Index e = 0; e != m; ++e) {
    Index f, t;               // f: source node, t: destination node
    double w;                 // w: weight of the edge
    std::cin >> f >> t >> w;  // Read the source node, destination node, and
                              // weight of the current edge
    if (w != 0) {             // check if w is different than 0
      edges.emplace_back(
          f - 1, t - 1,
          w);  // store the edge, decrementing f and t to 0 based index
               // Add the edge (f-1, t-1, w) to the 'edges' vector.
               // emplace_back constructs the tuple directly in the vector,
               // which is more efficient than push_back.
      ++degree[f - 1];  // Increment the degree of the source node.
      ++degree[t - 1];  // Increment the degree of the destination node.
    }
  }

  vector<Index> name(n);  // Create a vector 'name' of size 'n'.
                          // 'name' will store the new indices of the nodes
                          // after removing isolated nodes.
  Index new_n =
      0;  // Initialize a counter for the number of non-isolated nodes.
  // Iterate through each node
  for (Index k = 0; k != n; ++k) {
    if (degree[k] >
        0) {  // Check if the degree of node k is greater than 0 (not isolated).
      name[k] =
          new_n++;  // Assign a new index to node k and increment the counter.
    }
  }

  cout << new_n << " " << edges.size()
       << std::endl;  // Output the new number of nodes (non-isolated) and the
                      // number of edges

  // Iterate through each edge in the 'edges' vector.
  for (const auto& edge : edges) {
    Index f = name[std::get<0>(
        edge)];  // Get the source node index from the tuple and use it to get
                 // its new index from the name array
    Index t = name[std::get<1>(
        edge)];  // Get the destination node index from the tuple and use it to
                 // get its new index from the name array
    double w = std::get<2>(edge);  // Get the weight from the tuple.
    if (f > t) {  // Ensure that the source node index is always less than or
                  // equal to the destination node index.
      std::swap(f, t);  // Use std::swap for efficiency if f > t.
    }
    std::cout << f + 1 << " " << t + 1 << " " << w
              << std::endl;  // Output the edge (source, destination, weight),
                             // incrementing f and t to 1 based index
  }

  return EXIT_SUCCESS;  // Indicate that the program executed successfully.
}

/**
 * @file negate.cpp
 * @brief This program reads a graph from standard input and outputs the same graph with negated edge weights.
 *
 * The program reads a graph represented by a list of edges from standard input.
 * For each edge, it negates the weight and outputs the modified edge to standard output.
 * The graph's structure (number of nodes and edges) remains unchanged.
 *
 * Input Format:
 *   The input is read from standard input (std::cin).
 *   - The first line contains two integers: n (number of nodes) and m (number of edges).
 *   - Subsequent lines (m lines) each contain three values: a b w, representing an edge from node a to node b with weight w.
 *
 * Output Format:
 *   The output is written to standard output (std::cout).
 *   - The first line contains two integers: n (number of nodes) and m (number of edges).
 *   - Subsequent lines (m lines) each contain three values: a b -w, representing an edge from node a to node b with the negated weight -w.
 */

#include <iostream>

using namespace std;

int main() {
    int n, m, a, b;
    double w;

    // Read the number of nodes (n) and edges (m).
    cin >> n >> m;

    // Output the number of nodes and edges.
    cout << n << " " << m << endl;

    // Iterate through each edge.
    for (int i = 0; i != m; ++i) {
        // Read the source node (a), destination node (b), and weight (w) of the edge.
        cin >> a >> b >> w;

        // Output the source node (a), destination node (b), and negated weight (-w).
        cout << a << " " << b << " " << -w << endl;
    }

    return EXIT_SUCCESS;
}

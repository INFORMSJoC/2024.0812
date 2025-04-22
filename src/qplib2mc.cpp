/**
 * @file qplib2mc.cpp
 * @brief This program converts a graph representation from a specific format
 *        used in the qplib repository to a format suitable for a max-cut 
 *        solver. It only applies to instances with non constraints.
 *
 * The program reads a graph description from standard input, which includes
 * information about nodes, edges, and loops. It processes this
 * information to create a modified graph representation that can be used by a
 * max-cut solver. The key transformations include:
 *   - Adjusting node indices to be 1-based in the output.
 *   - Negating and halving the edge weights.
 *   - Calculating and adding loops for each node based on the sum of the 
 *     weights of incident edges and provided loop.
 *   - Potentially adding a dummy node to represent loops in an extended
 *     loopless grph.
 *
 * The input format is specific to the problem domain and includes:
 *   - A filename (on the first line).
 *   - Two ignored lines.
 *   - The number of nodes (n).
 *   - The number of edges (m).
 *   - m lines describing the edges (source, destination, weight).
 *   - An initial loop value (z).
 *   - The number of additional loop entries (nd).
 *   - nd lines describing additional loop entries (node index, loop
 *     weight).
 *
 * The output format is designed for a max-cut solver:
 *   - The (potentially modified) number of nodes and the total number of edges
 *     (including added loop elements).
 *   - The edges with adjusted weights.
 *   - The added loop elements, represented as edges connected to a dummy
 *     node (if needed).
 *
 * Comments in the input file start with '#'.
 * The output file is named after the input filename with ".txt" appended.
 */
#include <cassert>   // For assertions (debugging aid)
#include <cctype>    // For character handling (not used in this version, but might have been used previously)
#include <fstream>   // For file I/O (ofstream, ifstream)
#include <iostream>  // For console I/O (cin, cout)
#include <string>    // For string manipulation
#include <vector>    // For dynamic arrays (vectors)
#include <limits>    // For numeric limits (e.g., max of streamsize)
#include <numeric>   // For numerical algorithms (not directly used in the current code, but might have been in earlier versions)

using namespace std;

// Function to skip comment lines or lines with only spaces in an input stream.
// Comments start with '#' and extend to the end of the line.
void skipComment(istream& f) {
    char ch;
    // Read characters until a non-comment/non-space character is encountered or the end of the stream is reached.
    while (f.get(ch) && (ch == '#' || ch == ' ')) {
        // Ignore all characters until the end of the line.
        f.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }
    // If a non-comment/non-space character was found, put it back into the stream.
    if (f.good()) { 
        f.unget(); 
    }
}

int main() {
    string name;         // Variable to store the input file's name (without extension).
    string nameOut;      // Variable to store the output file's name (with ".txt" extension).
    int n, m;            // 'n' is the number of nodes, 'm' is the number of edges in the graph.

    // Read the first line from standard input (usually the filename).
    getline(cin, name);
    // Construct the output filename by appending ".txt" to the input filename.
    nameOut = name + ".txt";

    // Skip the second and third lines of the input (as per the problem's specification).
    getline(cin, name); 
    getline(cin, name); 

    // Read the number of nodes 'n' from the input.
    cin >> n;
    // Skip any comment lines or blank lines following the value of n.
    skipComment(cin);

    // Create a vector 'Sum' of size 'n', initialized with 0.0.
    // 'Sum[i]' will store the sum of weights connected to node 'i'.
    vector<double> Sum(n, 0.0); 
    // Create a vector 'Diag' of size 'n' to store diagonal values.
    vector<double> Diag(n);
    
    double Obj = 0.0; // This variable is declared but not used. It might be part of a larger algorithm or a previous version of the code

    // Read the number of edges 'm' from the input.
    cin >> m;
    // Skip any comment lines or blank lines following the value of m.
    skipComment(cin);

    // Create vectors to store the edges of the graph.
    vector<int> fr(m);    // 'fr[i]' is the source node of the i-th edge.
    vector<int> to(m);    // 'to[i]' is the destination node of the i-th edge.
    vector<double> c(m);  // 'c[i]' is half the weight of the i-th edge.

    // Read the edges and their weights from the input.
    for (int i = 0; i < m; ++i) {
        int u, v;   // Temporary variables for source and destination nodes.
        double w;  // Temporary variable for the edge's weight.

        cin >> u >> v >> w; // Read the source node, destination node, and weight of the i-th edge.
        --u;        // Adjust the node indices to be 0-based (input is 1-based).
        --v;        // Adjust the node indices to be 0-based.
        assert(u >= 0 && u < n); // Check if the node indices are within the valid range.
        assert(v >= 0 && v < n); // Check if the node indices are within the valid range.
        fr[i] = u; // Store the source node of the i-th edge.
        to[i] = v; // Store the destination node of the i-th edge.
        c[i] = w / 2; // Store half the weight of the i-th edge.
        Sum[u] += c[i]; // Add half the weight to the sum of weights for the source node.
        Sum[v] += c[i]; // Add half the weight to the sum of weights for the destination node.
    }

    double z;  // Variable to store the initial diagonal value.
    int nd;    // Variable to store the number of additional diagonal entries.

    // Read the initial diagonal value 'z' from the input.
    cin >> z;
    // Skip any comment lines or blank lines following the value of z.
    skipComment(cin);

    // Fill the 'Diag' vector with the initial diagonal value 'z'.
    fill(Diag.begin(), Diag.end(), z); 

    // Read the number of additional diagonal entries 'nd' from the input.
    cin >> nd;
    // Skip any comment lines or blank lines following the value of nd.
    skipComment(cin);

    // Read the additional diagonal entries and update the 'Diag' vector.
    for (int i = 0; i < nd; ++i) {
        int u;   // Temporary variable for the node index.
        double w; // Temporary variable for the diagonal value.

        cin >> u >> w; // Read the node index and the diagonal value.
        --u;           // Adjust the node index to be 0-based.
        Diag[u] = w; // Update the diagonal value for the specified node.
    }

    // Create vectors to store the diagonal entries that will be added to the output.
    vector<int> fr_diag;   // Source nodes for the added diagonal entries.
    vector<int> to_diag;   // Destination nodes for the added diagonal entries.
    vector<double> c_diag; // Weights for the added diagonal entries.

    // Iterate through all the nodes to generate the diagonal elements.
    for (int i = 0; i < n; ++i) {
        // Calculate the diagonal value for the i-th node.
        double val = -2 * Diag[i] - Sum[i];
        // If the diagonal value is non-zero, add it to the new vectors.
        if (val != 0.0) {
            fr_diag.push_back(i);      // Store the node index as the source.
            to_diag.push_back(n);     // Store 'n' as the destination (special case for diagonal).
            c_diag.push_back(val);    // Store the diagonal value.
        }
    }
    nd = fr_diag.size(); // Update nd to represent the new number of diagonal entries
    
    // If there are any new diagonal elements, increment n to account for the dummy node.
    if (nd > 0)
       ++n;

    // Open the output file.
    ofstream out(nameOut);

    // Write the number of nodes and edges to the output file.
    out << n << " " << m + nd << endl; 

    // Write the edge data to the output file.
    for (int i = 0; i < m; ++i) {
        out << fr[i] + 1 << " " << to[i] + 1 << " " << -c[i] / 2.0 << endl;
    }

    // Write the diagonal data to the output file.
    for(int i = 0; i < nd; ++i){
        out << fr_diag[i] + 1 << " " << to_diag[i] + 1 << " " << -c_diag[i] / 2.0 << endl;
    }
    
    // Close the output file.
    out.close();

    return EXIT_SUCCESS;
}

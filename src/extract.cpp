/**
 * @file extract.cpp
 * @brief This program extracts instance names from a summary file based on specified criteria.
 *
 * The program takes a summary file as input, which contains information about various instances.
 * It filters these instances based on user-defined parameters such as maximum density percentage
 * and minimum negative edge percentage. Optionally, it can also filter instances based on a list
 * of "interesting" instance names provided in a separate file.
 *
 * The program reads command-line arguments to determine the input summary file, output file,
 * optional interesting instances file, maximum density percentage, and minimum negative percentage.
 * It then processes the summary file line by line, extracting relevant data and applying the
 * filtering criteria.
 *
 * The output is a list of instance names that meet the specified criteria, written to the output file.
 * Additionally, the program prints a summary to the standard output, indicating the number of
 * extracted instances and the range of their sizes (in terms of the number of nodes).
 *
 * The program uses helper functions `trim` and `split` to process strings efficiently.
 *
 * Usage:
 *   ./extract -s <summary_file> -o <output_file> [-i <interesting_file>] [-d <max_density_perc>] [-n <min_negative_perc>] [-h]
 *
 *   -s <summary_file>       [mandatory] The path to the summary file.
 *   -o <output_file>        [mandatory] The path to the output file where extracted instance names will be written.
 *   -i <interesting_file>   [optional]  The path to a file containing a list of interesting instance names.
 *   -d <max_density_perc>   [optional]  The maximum density percentage (0.0 - 100.0) of instances to extract.
 *   -n <min_negative_perc>  [optional]  The minimum percentage (0.0 - 100.0) of negative edges in instances to extract.
 *   -h                      [optional]  Prints this help message.
 *
 * Example summary file format (comma-separated values):
 *   name,n,m,density,mean_deg,sd_deg,n_neg,n_zero,n_pos,max_precision,mean_w,sd_w
 *   instance1,10,45,1.0,9,0,100.0,0.0,0.0,10,1.0,0.0
 *   instance2,20,100,0.5,10,2,50.0,25.0,25.0,20,0.5,0.2
 *   ...
 */
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>  // Added for string splitting

using namespace std;
using Index = unsigned int;

// Function to trim leading and trailing whitespace from a string.
string trim(const string& str) {
  // Find the first non-space character.
  size_t first = str.find_first_not_of(' ');
  if (first == string::npos) {
    return "";  // String is all spaces
  }
  // Find the last non-space character.
  size_t last = str.find_last_not_of(' ');
  // Return the substring between the first and last non-space characters.
  return str.substr(first, (last - first + 1));
}

// Function to split a string into tokens using a delimiter.
vector<string> split(const string& s, const string& delimiter) {
  vector<string> tokens;
  size_t start = 0;
  size_t end = s.find(delimiter);
  while (end != string::npos) {
    tokens.push_back(s.substr(start, end - start));
    start = end + delimiter.length();
    end = s.find(delimiter, start);
  }
  tokens.push_back(s.substr(start));  // Add the last token
  for (Index i = 0; i != tokens.size(); ++i)
    if (tokens[i] == "") tokens[i] = "0";
  return tokens;
}

int main(int argc, char* argv[]) {
  // Read the parameters
  int opt;
  bool print_help = false;
  double max_density_perc = 100.0;
  double min_negative_perc = 0.0;
  char* sum_name = nullptr;
  char* out_name = nullptr;
  char* intr_name = nullptr;

  cerr << endl;
  if (argc == 1) {
    print_help = true;
  } else {
    while ((opt = getopt(argc, argv, "s:o:i:d:n:h")) != -1) {
      switch (opt) {
        case 's':
          sum_name = optarg;
          break;
        case 'o':
          out_name = optarg;
          break;
        case 'i':
          intr_name = optarg;
          break;
        case 'd':
          max_density_perc = stod(optarg);
          break;
        case 'n':
          min_negative_perc = stod(optarg);
          break;
        case 'h':
        default:
          print_help = true;
      }
    }
  }

  // Parameter Validation and Help Message
  if (!print_help) {
    if (sum_name == nullptr) {
      cerr << "Error: Parameter -s <input_file> is mandatory" << endl;
      print_help = true;
    }

    if (out_name == nullptr) {
      cerr << "Error: Parameter -o <outut_file> is mandatory" << endl;
      print_help = true;
    }

    if (max_density_perc < 0 || max_density_perc > 100.0) {
      cerr << "Error: Illegal value of <max_density_percentage>" << endl;
      print_help = true;
    }

    if (min_negative_perc < 0 || min_negative_perc > 100.0) {
      cerr << "Error: Illegal value of <min_negative_percentage>" << endl;
      print_help = true;
    }

    if (min_negative_perc == 0.0 && max_density_perc == 100.0) {
      cerr << "Error: At least one of the two parameters -d and -n must be set"
           << endl;
      print_help = true;
    }
  }
  if (print_help) {
    string tmp(argv[0]);
    cerr << "Usage: " << argv[0] << " -s <summary_file> "
         << "-o <output_file> [-i <interesting_file>]" << endl;
    cerr << string(tmp.length() + 8, ' ') << "[-d <max_density_perc>] "
         << "[-n <min_negative_perc>] "
         << "[-h]" << endl
         << endl;
    cerr << " -s <summary_file> [mandatory]" << endl;
    cerr << " -o <output_file> the names of the extracted instances "
         << "[mandatory]" << endl;
    cerr << " -i <interesting_file> the names of the interesting instances "
         << "[default: all]" << endl;
    cerr << " -d <max_density_perc> (>=0 and <= 100.0): max density "
         << "percentage " << endl;
    cerr << "    of the instances to be extracted" << endl;
    cerr << " -n <min_negative_perc>: (>=0 and <= 100.0) minimum "
         << "percentage " << endl;
    cerr << "    of negative edges of the instances to be extracted." << endl;
    cerr << " -h [flag]: print this message." << endl;
    cerr << endl;
    exit(EXIT_FAILURE);
  }

  // File Handling and Error Checks
  ifstream sumf(sum_name);
  if (!sumf.is_open()) {
    cerr << "Error: File " << sum_name << " does not exist!" << endl << endl;
    exit(EXIT_FAILURE);
  }

  ifstream intrf;
  if (intr_name != nullptr) {
    intrf.open(intr_name);
    if (!intrf.is_open()) {
      cerr << "Error: File " << intr_name << " does not exist!" << endl << endl;
      exit(EXIT_FAILURE);
    }
  }

  ofstream outf(out_name);
  if (!outf.is_open()) {
    cerr << "Error: Cannot create file " << out_name << "!" << endl << endl;
    exit(EXIT_FAILURE);
  }

  // Data Structures Initialization
  Index n_extract = 0;
  unordered_set<string> Inst_names;
  string line;
  const string delimeter = ",";

  // Read Interesting Instance Names (if provided)
  if (intr_name != nullptr) {
    while (getline(intrf, line)) {
      line = trim(line);
      if (line.empty() || line[0] == '#') continue;  // skip comment lines
      // Remove potential carriage return character
      if (!line.empty() && line.back() == '\r') {
        line.pop_back();
      }
      Inst_names.insert(line);
    }
    intrf.close();
  }

  sumf.ignore(numeric_limits<streamsize>::max(), '\n');  // skip header line

  bool first_time = true;
  Index min_nodes = 0;
  Index max_nodes = 0;

  // Main Processing Loop: Reading the Summary File
  while (getline(sumf, line)) {
    // Split the line into tokens
    vector<string> tokens = split(line, delimeter);

    // Error checking for the number of tokens
    if (tokens.size() < 15) {
      cerr << "Error: Invalid line format: " << line << endl;
      continue;  // skip to next line
    }

    // Extracting values from tokens (more readable)
    string name = tokens[0];
    Index n = stoi(tokens[1]);
    Index m = stoi(tokens[2]);
    double density = stod(tokens[3]);
    Index max_deg = stoi(tokens[4]);
    Index mean_deg = stoi(tokens[5]);
    Index sd_deg = stoi(tokens[6]);
    double n_neg = stod(tokens[7]);
    double n_zero = stod(tokens[8]);
    double n_pos = stod(tokens[9]);
    Index max_precision = stoi(tokens[10]);
    double mean_w = stod(tokens[11]);
    double sd_w = stod(tokens[12]);
    Index interesting = stoi(tokens[13]);
    Index toroidal = stoi(tokens[14]);

    // Calculate Thresholds
    Index max_edges = static_cast<Index>(
        (static_cast<double>(n) * (n - 1) / 2) * max_density_perc / 100.0);
    Index neg_edges = static_cast<Index>(m * n_neg / 100.0 + 0.5);
    Index zero_edges = static_cast<Index>(m * n_zero / 100.0 + 0.5);
    Index target = static_cast<Index>(m * min_negative_perc / 100.0 + 0.5);

    // Filtering and Extraction
    if (m <= max_edges && neg_edges >= target &&
        (intr_name == nullptr || Inst_names.find(name) != Inst_names.end())) {
      outf << name << endl;
      ++n_extract;
      if (first_time) {
        min_nodes = n;
        max_nodes = n;
        first_time = false;
      } else {
        min_nodes = min(min_nodes, n);
        max_nodes = max(max_nodes, n);
      }
    }
  }

  // Closing Files
  outf.close();
  sumf.close();

  // Output Summary
  cout << "Extracted " << n_extract << " instances with sizes between "
       << min_nodes << " and " << max_nodes << " nodes." << endl
       << endl;
  return EXIT_SUCCESS;
}

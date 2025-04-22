/**
 * @file tablegenerator.cpp
 * @brief This program analyzes the performance of multiple algorithms on a set
 * of problem instances and generates a table of comparative statistics. This
 * code is needed to produce the table of results as indicated in Dunning et al.
 * 2018.
 *
 * The program reads results from a file containing the performance data of
 * various algorithms across different instances and seeds (runs).
 * It then computes a range of statistics to compare the algorithms, such as:
 *   - First Equal Percentage (FE): How often an algorithm achieves the best
 *     result.
 *   - First Strict Percentage (FS): How often an algorithm strictly outperforms
 *     others.
 *   - Best Achieved Percentage (BA): How often an algorithm achieves the best
 *     result among all seeds.
 *   - Earliest Best Achieved Percentage (EBA): How often an algorithm achieves
 *     the best result earliest.
 *   - Worst Deviation (WD), Mean Deviation (MD), Best Deviation (BD): Measures
 *     of how far an algorithm's results deviate from the best.
 *   - Average Rank (AR): The average rank of an algorithm across all instances
 *     and seeds.
 *
 * The program can filter the instances and algorithms to be analyzed based on
 * lists provided in separate files.
 * It can also scale the time limits used in the analysis.
 * The output is a CSV file containing the computed statistics for each
 * algorithm.
 * Optionally, it can also generate a file containing the names of "difficult"
 * instances, where the best result is found by a limited number of algorithms.
 *
 * Input:
 *   - A parameter file specifying the results file, instance and algorithm
 *     selection, and output file name.
 *   - A results file containing the performance data of the algorithms.
 *   - Optional files listing the instances and algorithms to be analyzed.
 *
 * Output:
 *   - A CSV file with the computed statistics for each algorithm.
 *   - Optionally, a file listing "difficult" instances.
 *
 * Command-line options:
 *   - -p <parameter_file>: Specifies the parameter file (mandatory).
 *   - -s <time scaling>: Scales the time limits (default: 1.0).
 *   - -a: Uses absolute values instead of percentages for some statistics.
 *   - -d <file_name>: Generates a file with "difficult" instance names.
 *   - -l <level>: Sets the difficulty level for instance selection (used with
 *        -d).
 *   - -h: Prints the help message.
 *
 * Parameter file format:
 *   The parameter file must contain the following information, in this strict
 *   order, possibly in several lines:
 *   1. Name of the results file (e.g., "results.csv").
 *   2. Instances to analyze: either the string "all_instances" or
 *      "some_instances".
 *   3. If "some_instances" is specified, the name of the file containing the
 *      list of instance names (one per line). This file can contain blank 
 *      lines or lines starting with '#', which are ignored.
 *   4. Algorithms to consider: either the string "all_algorithms" or
 *      "some_algorithms".
 *   5. If "some_algorithms" is specified, the name of the file containing the
 *      list of algorithm names (one per line). This file can contain blank 
 *      lines or lines starting with '#', which are ignored.
 *   6. Name of the output file for the computed statistics (e.g.,
 *      "statistics.csv").
**/

using namespace std;

#include <unistd.h>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using Index = unsigned int;
using MatString = vector<vector<string>>;
using MatDouble = vector<vector<double>>;
using VecString = vector<string>;
using VecDouble = vector<double>;

//-------------------------------------------------------------------
class tablegenerator {
 public:
  double time_limit_scaling;
  bool absolute_values;

 private:
  string nameresults;   // this contains the names of the output file
  string instance_set;  // this may be "all_instances" or "some_instances"
  string instance_names_file;  // file containing names of instances in trial
  string algorithm_set;  // this may be "all_algorithms" or "some_algorithms"
  string algorithm_names_file;  // file containing names of algorithms in trial
  string statfilename;  // file containing the name of the statistics file
  Index nalgorithms;    // number of algorithms
  Index ninstances;     // number of instances
  Index n_seeds;        // number of seeds
  vector<vector<vector<double>>> resultsdata;
  vector<vector<vector<string>>> resultsdata_str;
  vector<vector<vector<string>>> resultstime;

  unordered_map<string, Index> Inst_names;
  unordered_map<string, Index> Algo_names;
  unordered_map<string, Index> Seed_names;

  // STATISTICS
  MatDouble SumBySeeds_mat;
  MatString MaxBySeeds_mat, MinBySeeds_mat, TimeMaxBySeeds_mat;
  VecDouble MaxByAlg_SumBySeeds_vect;
  VecDouble FE;
  MatDouble MaxByAlgButOne_SumBySeeds_mat;
  VecString MaxByAlg_MaxBySeeds_vect, TimeMaxByAlg_MaxBySeeds_vect;
  VecDouble FS;
  VecDouble BA;
  MatString MaxByAlgButOne_MaxBySeeds_mat;
  VecDouble SBA1, SBA2, EBA;
  VecDouble WD, MD, BD;
  VecDouble AR, ASR;

 private:
  void AllocateStatistics();
  void SumBySeeds();
  void MaxByAlg(MatDouble& in_mat, VecDouble& out_vect);
  //  void MaxByAlg(MatString& in_mat, VecString& out_vect);
  void MaxByAlg(MatString& in_mat, MatString& time_in_mat, VecString& out_vect,
                VecString& time_out_vect);
  void FirstEqualPercentage();
  void MaxByAlgButOne(MatDouble& in_mat, MatDouble& out_mat);
  void MaxByAlgButOne(MatString& in_mat, MatString& out_mat);
  void FirstStrictPercentage();
  void MaxBySeeds();
  void BestAchievedPercentage();
  void EarliestBestAchievedPercentage();
  void MinBySeeds();
  void WorstDeviations();
  void MeanDeviations();
  void BestDeviations();
  void AvgRank();

 public:
  tablegenerator() = default;

  void read_parameters(char* filename);
  void read_selected_instances();
  void read_selected_algorithms();
  void read_results_file();
  void extract(int level, char* filename);
  void extractChamp(Index cMetric, char* Alg_name, char* filename);
  void ComputeStatistics();
  void writetable();
  void Set_instances_set() { instance_set = "all_instances"; }
};

// Helper function to adjust the position of the decimal point in a
// string representation of a number.  It handles scientific notation
// (e.g., "1.2e3") and moves the decimal accordingly.  It also removes
// leading/trailing zeros and returns the final decimal point
// position.
auto dotpos = [](string& v) {
  // Initialize exponent and decimal position.
  int e = 0;
  int pos = v.length();

  // Check for scientific notation (e.g., "1.2e3").
  if (v.find('e') != string::npos) {
    // Extract exponent value.
    e = stoi(v.substr(v.find('e') + 1, string::npos));
    // Remove the exponent part from the string.
    v = v.substr(0, v.find('e'));
  }

  // Check for a decimal point.
  if (v.find('.') != string::npos) {
    // Find the position of the decimal point.
    pos = v.find('.');
    // Remove the decimal point.
    v.erase(v.begin() + pos);
  } else {
    // If no decimal point, the effective position is at the end.
    pos = v.length();
  }

  // Adjust the decimal position based on the exponent.
  pos += e;

  // Handle negative decimal positions (e.g., moving decimal left).
  if (pos < 0) {
    // Add leading zeros to shift decimal to the right.
    v.insert(0, -pos, '0');
    pos = 0;
  }
  // Handle positive decimal positions (e.g., moving decimal right).
  if (pos > static_cast<int>(v.length())) {
    // Add trailing zeros to shift decimal to the left.
    v.append(pos - v.length(), '0');
  }

  // Remove leading zeros after decimal adjustment.
  int trail = 0;
  for (; trail < pos; ++trail) {
    if (v[trail] != '0') break;
  }
  if (trail > 0) {
    v.erase(v.begin(), v.begin() + trail);
    pos -= trail;
  }

  return pos;
};

// Comparison function for string representations of numbers, taking into
// account the decimal point position and potentially different lengths.
auto gte = [](string u, string v) {
  // Get the adjusted decimal positions.
  int du = dotpos(u);
  int dv = dotpos(v);

  // Normalize lengths for comparison if decimal positions are the same.
  if (du == dv) {
    if (u.length() < v.length()) {
      u.append(v.length() - u.length(), '0');
    } else if (u.length() > v.length()) {
      v.append(u.length() - v.length(), '0');  // Pad v instead of u
    }
  }
  // Comparison logic:
  // - If u's decimal position is to the right of v's (du > dv), u is greater.
  // - If decimal positions are the same, lexicographically compare the strings.
  // - If neither condition is met, v is greater.
  if ((du > dv) || (du == dv && u.compare(v) > 0)) {
    return 1;  // u is greater than v
  } else if (du == dv && u.compare(v) == 0) {
    return 0;  // u is equal to v
  } else {
    return -1;  // u is less than v
  }
};

// Trims leading and trailing whitespace from a string.
auto trim = [](string str) {
  // Remove trailing spaces.
  str.erase(str.find_last_not_of(' ') + 1);
  // Remove leading spaces.
  str.erase(0, str.find_first_not_of(' '));
  return str;
};

// Extracts the correct value and time from a string containing a
// history of values and times.  It searches for the last entry within
// a given time limit.
auto trueValue = [](string& str, double limit) {
  const char delimiter = ';';  // Delimiter for separating value-time pairs
  const char separator = ':';  // Separator for value and time within a pair
  string token;                // Temporary holder for each value-time pair
  string val;                  // Extracted value
  string time;                 // Extracted time

  // Iterate through the string from the end (ignoring the last two characters).
  for (int pos = str.size() - 2; pos > 0;) {
    // Find the last occurrence of the delimiter within the current position.
    int new_pos = str.rfind(delimiter, pos);
    // If no delimiter is found, set to beginning of the string
    if (new_pos == static_cast<int>(string::npos)) new_pos = 0;

    // Extract the value-time pair.
    token = str.substr(new_pos + 1, pos - new_pos);
    // Extract value and time based on the separator.
    val = token.substr(0, token.find(separator));
    time = token.substr(token.find(separator) + 1, token.size() - 1);

    // Check if the time is within the limit.
    if (stod(time) <= limit) {
      // If at the very end of the string and no valid value is found
      // before the end, clear val.
      if (pos == static_cast<int>(str.size() - 2)) val = "";
      break;  // Exit the loop if a valid entry is found.
    }

    // Move the search position to the next delimiter.
    pos = new_pos - 1;
    // reset the values if not found
    val = "0";
    time = "0";
  }
  // Return the extracted value and time as a pair.
  return make_pair(val, time);
};

//-------------------------------------------------------------------
// sorting facilities
// The struct 'couple' contains a criteria (statistics) and an index
struct couple {
  double criteria;
  double criteria2;
  Index index;
};
// The function will return 1 if couple2 < couple1 else 0
bool compare_couple(const couple& couple1, const couple& couple2) {
  if (couple2.criteria < couple1.criteria)
    return 1;
  else if (couple2.criteria > couple1.criteria)
    return 0;

  if (couple2.criteria2 < couple1.criteria2)
    return 1;
  else if (couple2.criteria2 > couple1.criteria2)
    return 0;

  if (couple2.index > couple1.index) return 1;
  return 0;
}

//-------------------------------------------------------------------
// INPUT DATA
//-------------------------------------------------------------------

void tablegenerator::read_parameters(char* filename) {
  // This file contains in this strict order (possibly in several lines):
  // - Name of the results file
  // - instances to analyze: the string "all_instances" or "some_instances"
  // - <instance file name> (only if instance to analyze is "some_instances")
  // - algorithms to consider: the string "all_algorithms" or "some_algorithms"
  // - <algorithm file name> (only if algorithms to consider is
  //   "some_algorithms")
  // - name of the output file with the computed statistics
  //
  // 'instance file name' and 'algorithm file name' may contain blank lines
  // or lines starting with character '#'. These lines are skipped.
  // Using this feature, an instance of an algorithm can be removed
  // from the list by simply prepend '#' to their names.

  ifstream fin;

  fin.open(filename);
  if (!fin.is_open()) {
    cerr << "Parameter file " << filename << " does not exist" << endl;
    exit(EXIT_FAILURE);
  };

  fin >> nameresults;

  fin >> instance_set;
  if (instance_set != "all_instances") {
    if (instance_set != "some_instances") {
      cerr << "String \"all_instances\" or \"some_instances\" missing in "
           << "parameter file " << filename << endl;
      exit(EXIT_FAILURE);
    }
    fin >> instance_names_file;
  }

  fin >> algorithm_set;
  if (algorithm_set != "all_algorithms") {
    if (algorithm_set != "some_algorithms") {
      cerr << "String \"all_algorithms\" or \"some_algorithms\" missing in "
           << "parameter file " << filename << endl;
      exit(EXIT_FAILURE);
    }
    fin >> algorithm_names_file;
  }

  fin >> statfilename;
  if (statfilename.length() == 0) {
    cerr << "The output file name is missing in parameter file " << filename
         << endl;
    exit(EXIT_FAILURE);
  }

  fin.close();
}

//-------------------------------------------------------------------
void tablegenerator::read_selected_instances() {
  if (instance_set == "all_instances") return;
  ifstream fin;
  fin.open(instance_names_file);
  if (!fin.is_open()) {
    cerr << "File " << instance_names_file << " does not exist" << endl;
    exit(EXIT_FAILURE);
  };
  string line;
  string token;
  Index InstIdx = 0;
  while (getline(fin, line)) {
    line = trim(line);
    if (line == "" || line[0] == '#') continue;  // skip comment lines
    Index pos = line.find("\r");  // in MSDOS files, lines end with \r\n
    token = line.substr(0, pos);
    if (Inst_names.try_emplace(token, InstIdx).second) ++InstIdx;
  }
  ninstances = InstIdx;
  fin.close();
}

//-------------------------------------------------------------------
void tablegenerator::read_selected_algorithms() {
  if (algorithm_set == "all_algorithms") return;
  ifstream fin;
  fin.open(algorithm_names_file);
  if (!fin.is_open()) {
    cerr << "File " << algorithm_names_file << " does not exist" << endl;
    exit(EXIT_FAILURE);
  };
  string line;
  string token;
  Index AlgoIdx = 0;
  while (getline(fin, line)) {
    line = trim(line);
    if (line == "" || line[0] == '#') continue;  // skip comment lines
    Index pos = line.find("\r");  // in MSDOS files, lines end with \r\n
    token = line.substr(0, pos);
    if (Algo_names.try_emplace(token, AlgoIdx).second) ++AlgoIdx;
  }
  nalgorithms = AlgoIdx;
  fin.close();
}

//-------------------------------------------------------------------
void tablegenerator::read_results_file() {
  ifstream fin;
  fin.open(nameresults);
  if (!fin.is_open()) {
    cerr << "File " << nameresults << " does not exist" << endl;
    exit(EXIT_FAILURE);
  }
  string line;
  string line0;
  // file results contains a header in the first row, which we skip
  getline(fin, line);
  //
  const string delimeter = ",";
  Index pos = 0;
  string token;
  vector<bool> used_instances;
  vector<bool> used_algorithms;

  if (instance_set == "some_instances") {
    used_instances.resize(ninstances, false);
  }

  if (algorithm_set == "some_algorithms") {
    used_algorithms.resize(nalgorithms, false);
  }

  // read subsequent lines

  Index skipped_alg = 0;
  Index skipped_inst = 0;

  Index InstIdx = 0;
  Index AlgoIdx = 0;
  Index SeedIdx = 0;
  Index nLine = 0;
  while (getline(fin, line)) {
    Index Ind = 0;
    bool skip = false;
    bool doBreak = false;
    double limit = 0.0;
    Index Algo = 0;
    Index Seed = 0;
    Index Inst = 0;
    string Time;
    string Value;
    pair<string, string> P;
    nLine++;
    line += delimeter;
    line0 = line;
    while ((pos = line.find(delimeter)) != static_cast<Index>(string::npos)) {
      token = line.substr(0, pos);
      line.erase(0, pos + delimeter.length());
      switch (Ind) {
        case 0:  // timestamp
          break;
        case 1:  // graphname
          if (instance_set == "some_instances") {
            if (Inst_names.find(token) == Inst_names.end()) {
              skip = true;
              ++skipped_inst;
            } else {
              used_instances[Inst_names[token]] = true;
              Inst = Inst_names[token];
            }
          } else {
            if (Inst_names.try_emplace(token, InstIdx).second) ++InstIdx;
            Inst = Inst_names[token];
          }
          break;
        case 2:  // algorithm
          if (algorithm_set == "some_algorithms") {
            if (Algo_names.find(token) == Algo_names.end()) {
              skip = true;
              ++skipped_alg;
            } else {
              used_algorithms[Algo_names[token]] = true;
              Algo = Algo_names[token];
            }
          } else {
            if (Algo_names.try_emplace(token, AlgoIdx).second) ++AlgoIdx;
            Algo = Algo_names[token];
          }
          break;
        case 3:  // seed
          if (Seed_names.try_emplace(token, SeedIdx).second) ++SeedIdx;
          Seed = Seed_names[token];
          break;
        case 4:  // time limit
          limit = stod(token) * time_limit_scaling;
        case 5:  // objective
          Value = token;
          break;
        case 6:  // time
          Time = token;
          break;
        case 7:  // history
          P = trueValue(token, limit);
          if (P.first.size() > 0) {
            Value = P.first;
            Time = P.second;
          }
          doBreak = true;
          break;
      }
      Ind++;
      if (skip or doBreak) break;
    }
    if (!skip) {
      if (Seed >= resultsdata.size()) {
	Index size = Seed + 1;
        resultsdata.resize(size);
        resultsdata_str.resize(size);
        resultstime.resize(size);
      }
      if (Inst >= resultsdata[Seed].size()) {
	Index size = Inst + 1;
        if (instance_set == "some_instances")
	  size = ninstances;
	resultsdata[Seed].resize(size);
	resultsdata_str[Seed].resize(size);
	resultstime[Seed].resize(size);
      }
      if (Algo >= resultsdata[Seed][Inst].size()) {
	Index size = Algo + 1;
        if (algorithm_set == "some_algorithms")
	  size = nalgorithms;
	resultsdata[Seed][Inst].resize(size);
	resultsdata_str[Seed][Inst].resize(size);
	resultstime[Seed][Inst].resize(size);
      }
      
      // If the triple Seed,Inst,Algo appears again, we overwrite the
      // previous entry
      resultsdata_str[Seed][Inst][Algo] = Value;
      resultsdata[Seed][Inst][Algo] = stod(Value);
      resultstime[Seed][Inst][Algo] = Time;
    }
  }
  n_seeds = SeedIdx;
  fin.close();

  if (instance_set == "all_instances")
    ninstances = InstIdx;
  else {
    bool found = false;
    for (Index i = 0; i != ninstances; ++i)
      if (!used_instances[i]) {
        found = true;
        break;
      }
    if (found) {
      cerr << endl
           << "WARNING: The following instances in file " << instance_names_file
           << endl
           << "         do not appear in file " << nameresults << endl
           << "         Execution is aborted." << endl
           << endl;
      for (auto& name : Inst_names)
        if (!used_instances[Inst_names[name.first]]) cerr << name.first << endl;
      cerr << endl;
    }
    if (found) exit(EXIT_FAILURE);
  }

  if (algorithm_set == "all_algorithms")
    nalgorithms = AlgoIdx;
  else {
    bool found = false;
    for (Index i = 0; i != nalgorithms; ++i)
      if (!used_algorithms[i]) {
        found = true;
        break;
      }
    if (found) {
      cerr << endl
           << "WARNING: The following algorithms in file "
           << algorithm_names_file << endl
           << "         do not appear in file " << nameresults << endl
           << "         Execution is aborted." << endl
           << endl;
      for (auto& name : Algo_names)
        if (!used_algorithms[Algo_names[name.first]])
          cerr << name.first << endl;
      cerr << endl;
    }
    if (found) exit(EXIT_FAILURE);
  }

  cout << "Read " << nLine << " records. " << endl;
  cout << skipped_inst << " where skipped because uninteresting "
       << "instances" << endl;
  cout << skipped_alg << " where skipped because uninteresting "
       << "algorithms" << endl;
}

//-------------------------------------------------------------------
// START COMPUTING METRICS
//-------------------------------------------------------------------
void tablegenerator::ComputeStatistics() {
  AllocateStatistics();
  SumBySeeds();
  MaxByAlg(SumBySeeds_mat, MaxByAlg_SumBySeeds_vect);
  FirstEqualPercentage();
  MaxByAlgButOne(SumBySeeds_mat, MaxByAlgButOne_SumBySeeds_mat);
  FirstStrictPercentage();
  MaxBySeeds();
  MaxByAlg(MaxBySeeds_mat, TimeMaxBySeeds_mat, MaxByAlg_MaxBySeeds_vect,
           TimeMaxByAlg_MaxBySeeds_vect);
  BestAchievedPercentage();
  EarliestBestAchievedPercentage();
  MaxByAlgButOne(MaxBySeeds_mat, MaxByAlgButOne_MaxBySeeds_mat);
  MinBySeeds();
  WorstDeviations();
  MeanDeviations();
  BestDeviations();
  AvgRank();
}
//-------------------------------------------------------------------
void tablegenerator::AllocateStatistics() {
  // allocate matrices ninstances x nalgorithms

  SumBySeeds_mat.resize(ninstances);
  MaxBySeeds_mat.resize(ninstances);
  TimeMaxBySeeds_mat.resize(ninstances);
  MinBySeeds_mat.resize(ninstances);
  MaxByAlgButOne_SumBySeeds_mat.resize(ninstances);
  MaxByAlgButOne_MaxBySeeds_mat.resize(ninstances);
  for (Index i = 0; i != ninstances; ++i) {
    SumBySeeds_mat[i].resize(nalgorithms);
    MaxBySeeds_mat[i].resize(nalgorithms);
    TimeMaxBySeeds_mat[i].resize(nalgorithms);
    MinBySeeds_mat[i].resize(nalgorithms);
    MaxByAlgButOne_SumBySeeds_mat[i].resize(nalgorithms);
    MaxByAlgButOne_MaxBySeeds_mat[i].resize(nalgorithms);
  }

  MaxByAlg_SumBySeeds_vect.resize(ninstances);
  MaxByAlg_MaxBySeeds_vect.resize(ninstances);
  TimeMaxByAlg_MaxBySeeds_vect.resize(ninstances);

  FE.resize(nalgorithms);
  FS.resize(nalgorithms);
  BA.resize(nalgorithms);
  EBA.resize(nalgorithms);
  WD.resize(nalgorithms);
  MD.resize(nalgorithms);
  BD.resize(nalgorithms);
  AR.resize(nalgorithms);
}

//-------------------------------------------------------------------
void tablegenerator::SumBySeeds() {
  // This function computes:
  // for each instance and for each algorithm the sum of the values for each
  // seed XLS: Somma5

  for (Index i = 0; i < ninstances; i++)
    for (Index h = 0; h < nalgorithms; h++) {
      SumBySeeds_mat[i][h] = 0;
      for (Index seed = 0; seed < n_seeds; seed++)
        SumBySeeds_mat[i][h] += resultsdata[seed][i][h];
    }
}
//-------------------------------------------------------------------
void tablegenerator::MaxByAlg(MatDouble& in_mat, VecDouble& out_vect) {
  // This function computes:
  // for each instance the maximum of the sum of the values for each seed
  // in_mat[ninstances][nalgorithms] may be SumBySeeds or MaxBySeeds
  // out_vect[ninstances] can be a temporary vector of dimension ninstances

  for (Index i = 0; i < ninstances; i++) {
    out_vect[i] = in_mat[i][0];
    for (Index h = 1; h < nalgorithms; h++)
      if (out_vect[i] < in_mat[i][h]) out_vect[i] = in_mat[i][h];
  }
}
//-------------------------------------------------------------------
void tablegenerator::MaxByAlg(MatString& in_mat, MatString& time_in_mat,
                              VecString& out_vect, VecString& time_out_vect) {
  // This function computes:
  // for each instance the maximum of the sum of the values for each seed
  // in_mat[ninstances][nalgorithms] may be SumBySeeds or MaxBySeeds
  // out_vect[ninstances] can be a temporary vector of dimension ninstances

  for (Index i = 0; i < ninstances; i++) {
    out_vect[i] = in_mat[i][0];
    time_out_vect[i] = time_in_mat[i][0];
    for (Index h = 1; h < nalgorithms; h++)
      if ((gte(out_vect[i], in_mat[i][h])) < 0 ||
          ((gte(out_vect[i], in_mat[i][h]) == 0) &&
           (gte(time_out_vect[i], time_in_mat[i][h]) > 0))) {
        out_vect[i] = in_mat[i][h];
        time_out_vect[i] = time_in_mat[i][h];
      }
  }
}
//-------------------------------------------------------------------
void tablegenerator::FirstEqualPercentage() {
  // FE(h) = |\{ i |   \sum_s x^s_h,i = max_h \sum_s x^s_h,i \}|   / ninstances

  for (Index h = 0; h < nalgorithms; h++) {
    FE[h] = 0;
    for (Index i = 0; i < ninstances; i++)
      if (SumBySeeds_mat[i][h] == MaxByAlg_SumBySeeds_vect[i])
        FE[h] = FE[h] + 1.0;
    if (!absolute_values) FE[h] /= ninstances;
  }
}
//-------------------------------------------------------------------
void tablegenerator::MaxByAlgButOne(MatDouble& in_mat, MatDouble& out_mat) {
  // This function computes:
  // for each instance and for each algorithm the maximum excluding the
  // algorithm itself of the sum of the values for each seed

  for (Index i = 0; i < ninstances; i++) {
    for (Index h = 0; h < nalgorithms; h++) {
      Index h1;
      if (h > 0) {
        out_mat[i][h] = in_mat[i][0];
        for (h1 = 1; h1 < h; h1++)
          if (out_mat[i][h] < in_mat[i][h1]) out_mat[i][h] = in_mat[i][h1];
        h1 = h + 1;
      } else {
        out_mat[i][0] = in_mat[i][1];
        h1 = 2;
      }
      for (; h1 < nalgorithms; h1++)
        if (out_mat[i][h] < in_mat[i][h1]) out_mat[i][h] = in_mat[i][h1];
    }
  }
}
//-------------------------------------------------------------------
void tablegenerator::MaxByAlgButOne(MatString& in_mat, MatString& out_mat) {
  // This function computes:
  // for each instance and for each algorithm the maximum excluding the
  // algorithm itself of the sum of the values for each seed

  for (Index i = 0; i < ninstances; i++) {
    for (Index h = 0; h < nalgorithms; h++) {
      Index h1;
      if (h > 0) {
        out_mat[i][h] = in_mat[i][0];
        for (h1 = 1; h1 < h; h1++)
          if (gte(out_mat[i][h], in_mat[i][h1]) < 0)
            out_mat[i][h] = in_mat[i][h1];
        h1 = h + 1;
      } else {
        out_mat[i][0] = in_mat[i][1];
        h1 = 2;
      }
      for (; h1 < nalgorithms; h1++)
        if (gte(out_mat[i][h], in_mat[i][h1]) < 0)
          out_mat[i][h] = in_mat[i][h1];
    }
  }
}
//-------------------------------------------------------------------
void tablegenerator::FirstStrictPercentage() {
  // FS(h) = |\{ i |   \sum_s x^s_h,i > max_h \sum_s x^s_h,i \}|   / ninstances

  for (Index h = 0; h < nalgorithms; h++) {
    FS[h] = 0;
    for (Index i = 0; i < ninstances; i++)
      if (SumBySeeds_mat[i][h] > MaxByAlgButOne_SumBySeeds_mat[i][h])
        FS[h] = FS[h] + 1.0;
    if (!absolute_values) FS[h] /= ninstances;
  }
}
//-------------------------------------------------------------------
void tablegenerator::MaxBySeeds() {
  // This function computes:
  // for each instance and for each algorithm the max of the values for each
  // seed XLS: Max5

  for (Index i = 0; i < ninstances; i++)
    for (Index h = 0; h < nalgorithms; h++) {
      MaxBySeeds_mat[i][h] = resultsdata_str[0][i][h];
      TimeMaxBySeeds_mat[i][h] = resultstime[0][i][h];
      for (Index seed = 1; seed < n_seeds; seed++)
        if (gte(MaxBySeeds_mat[i][h], resultsdata_str[seed][i][h]) < 0 ||
            (gte(MaxBySeeds_mat[i][h], resultsdata_str[seed][i][h]) == 0 &&
             gte(TimeMaxBySeeds_mat[i][h], resultstime[seed][i][h]) > 0)) {
          MaxBySeeds_mat[i][h] = resultsdata_str[seed][i][h];
          TimeMaxBySeeds_mat[i][h] = resultstime[seed][i][h];
        }
    }
}
//-------------------------------------------------------------------
void tablegenerator::MinBySeeds() {
  // This function computes:
  // for each instance and for each algorithm the min of the values for each
  // seed

  for (Index i = 0; i < ninstances; i++)
    for (Index h = 0; h < nalgorithms; h++) {
      MinBySeeds_mat[i][h] = resultsdata_str[0][i][h];
      for (Index seed = 1; seed < n_seeds; seed++)
        if (gte(MinBySeeds_mat[i][h], resultsdata_str[seed][i][h]) > 0)
          MinBySeeds_mat[i][h] = resultsdata_str[seed][i][h];
    }
}
//-------------------------------------------------------------------
void tablegenerator::BestAchievedPercentage() {
  // BA(h) = |\{ i |   \max_s x^s_h,i = max_h \max_s x^s_h,i \}|   / ninstances

  for (Index h = 0; h < nalgorithms; h++) {
    BA[h] = 0;
    for (Index i = 0; i < ninstances; i++)
      if (gte(MaxBySeeds_mat[i][h], MaxByAlg_MaxBySeeds_vect[i]) == 0)
        BA[h] += 1.0;
    if (!absolute_values) BA[h] /= ninstances;
  }
}
//-------------------------------------------------------------------
void tablegenerator::EarliestBestAchievedPercentage() {
  // EBA(h) = |\{ i |   \max_s x^s_h,i = max_h \max_s x^s_h,i \}|   / ninstances

  for (Index h = 0; h < nalgorithms; h++) {
    EBA[h] = 0;
    for (Index i = 0; i < ninstances; i++)
      if ((gte(MaxBySeeds_mat[i][h], MaxByAlg_MaxBySeeds_vect[i]) == 0) &&
          (gte(TimeMaxBySeeds_mat[i][h], TimeMaxByAlg_MaxBySeeds_vect[i]) == 0))
        EBA[h] += 1.0;
    if (!absolute_values) EBA[h] /= ninstances;
  }
}
//-------------------------------------------------------------------
void tablegenerator::WorstDeviations() {
  // This must be as small as possible. Denotes if the algorithm "h" has high
  // chances to be the best WD(h) = 1 − ( sum_i  (min_s  x^s_h,i)/(max_h1 max_s
  // x^s_h1,i )) / |I|

  for (Index h = 0; h < nalgorithms; h++) {
    double somma = 0;
    for (Index i = 0; i < ninstances; i++) {
      double den = stod(MaxByAlg_MaxBySeeds_vect[i]);
      if (den > 0) somma += stod(MinBySeeds_mat[i][h]) / den;
    }
    WD[h] = 1 - somma / ninstances;
  }
}
//-------------------------------------------------------------------
void tablegenerator::MeanDeviations() {
  // MD(h) = 1 −  sum_i  ( \sum_s  x^s_h,i / n_seeds) / ( max_h1,i  x^s_h1,i ))
  // / |I|
  for (Index h = 0; h < nalgorithms; h++) {
    double somma = 0;
    for (Index i = 0; i < ninstances; i++) {
      double den = stod(MaxByAlg_MaxBySeeds_vect[i]);
      if (den > 0) somma += (SumBySeeds_mat[i][h] / n_seeds) / den;
    }
    MD[h] = 1 - somma / ninstances;
  }
}
//-------------------------------------------------------------------
void tablegenerator::BestDeviations() {
  // BD(h) = 1 − ( sum_i  (max_s  x^s_h,i)/(max_h1 max_s x^s_h1,i )) / |I|

  for (Index h = 0; h < nalgorithms; h++) {
    double somma = 0;
    for (Index i = 0; i < ninstances; i++) {
      double den = stod(MaxByAlg_MaxBySeeds_vect[i]);
      if (den > 0) somma += stod(MaxBySeeds_mat[i][h]) / den;
    }
    BD[h] = 1 - somma / ninstances;
  }
}
//-------------------------------------------------------------------
void tablegenerator::AvgRank() {
  // AR(h) =  \sum_i \sum_s R(h,i,s) / 5 |I|
  //  where R(h,i,s) = rank of the objective value of heuristic h on replicate s
  //  of instance i
  //                   among the objective values of all heuristics in H on
  //                   replicate s for instance i.
  //  In the case of ties, all heuristics are given the minimum rank.
  //  Rank 1 indicates the best performance among all heuristics, and rank 37
  //  indicates the worst performance.

  for (Index h = 0; h < nalgorithms; h++) {
    AR[h] = 0.0;
    for (Index i = 0; i < ninstances; i++)
      for (Index seed = 0; seed < n_seeds; seed++) {
        string* vect = new string[nalgorithms];
        for (Index h1 = 0; h1 < nalgorithms; h1++)
          vect[h1] = resultsdata_str[seed][i][h1];

        Index r = 0;  // r computes the number of results better than h
        Index s = 0;  // s computes the number of results equal to h
        for (Index h1 = 0; h1 < nalgorithms; h1++) {
          if ((h1 != h) && (gte(vect[h1], vect[h]) > 0)) r++;
          if ((h1 != h) && (gte(vect[h1], vect[h]) == 0)) s++;
        }
        delete[] vect;
        AR[h] += (1 + r);
      }
    AR[h] /= (n_seeds * ninstances);
  }
}
//-------------------------------------------------------------------
void tablegenerator::writetable() {
  // This functions produces a .csv file with delimiters

  // sort the algorithms according FE
  couple* sortarray = new couple[nalgorithms];
  for (Index h = 0; h < nalgorithms; h++) {
    sortarray[h].criteria = FE[h];
    sortarray[h].criteria2 = -MD[h];
    sortarray[h].index = h;
  }
  sort(sortarray, sortarray + nalgorithms, &compare_couple);

  // read algorithm names
  ifstream fin;
  fin.open("data/Alg_names.csv");
  if (!fin.is_open()) {
    cerr << "File " << "data/Alg_names.csv" << " does not exist" << endl;
    exit(EXIT_FAILURE);
  };

  unordered_map<string, string> names;
  string line;
  string delimeter = ",";
  Index n_line = 0;

  while (getline(fin, line)) {
    ++n_line;
    Index pos = line.find(delimeter);
    if (pos == static_cast<Index>(string::npos)) {
      cerr << "Error at line " << n_line << ") " << line << endl;
      exit(EXIT_FAILURE);
    }
    if (!names.try_emplace(line.substr(0, pos), line.substr(pos + 1)).second) {
      cerr << "Error at line " << n_line << ") " << line << endl;
      exit(EXIT_FAILURE);
    }
  }
  fin.close();

  // print table
  ofstream fout;
  fout.open(statfilename);

  fout << "Heuristic,FE,FS,BA,EBA,WD,MD,BD,AR" << endl;

  for (Index h = 0; h < nalgorithms; h++) {
    string name;
    Index hidx = sortarray[h].index;

    for (const auto& [key, value] : Algo_names)
      if (value == hidx) {
        name = key;
        break;
      }
    fout << names[name];
    if (absolute_values) {
      fout << "," << fixed << setprecision(0) << FE[hidx];
      fout << "," << fixed << setprecision(0) << FS[hidx];
      fout << "," << fixed << setprecision(0) << BA[hidx];
      fout << "," << fixed << setprecision(0) << EBA[hidx];
    } else {
      fout << "," << fixed << setprecision(1) << FE[hidx] * 100;
      fout << "," << fixed << setprecision(1) << FS[hidx] * 100;
      fout << "," << fixed << setprecision(1) << BA[hidx] * 100;
      fout << "," << fixed << setprecision(1) << EBA[hidx] * 100;
    }
    fout << "," << fixed << setprecision(2) << WD[hidx] * 100;
    fout << "," << fixed << setprecision(2) << MD[hidx] * 100;
    fout << "," << fixed << setprecision(2) << BD[hidx] * 100;
    fout << "," << fixed << setprecision(1) << AR[hidx];

    fout << endl;
  }
  fout.close();
  delete[] sortarray;
}

void tablegenerator::extract(int level, char* filename) {
  double threshold;
  if (level < 0)
    threshold = nalgorithms / 2.0;
  else
    threshold = level;
  ofstream fout;
  int rejected = 0;
  int accepted = 0;
  fout.open(filename);

  for (auto& inst : Inst_names) {
    int i = Inst_names[inst.first];
    int count = 0;
    string value, best;
    best = "0";

    for (Index h = 0; h < nalgorithms; ++h) {
      Index local_count = 0;
      for (Index s = 0; s < n_seeds; ++s) {
        value = resultsdata_str[s][i][h];
        int d = gte(best, value);
        if (d == 0)
          ++local_count;
        else if (d < 0) {
          count = 0;
          local_count = 1;
          best = value;
        }
      }
      if (local_count == n_seeds) ++count;
    }

    if (count > threshold) {
      ++rejected;
    } else if (count >= 0) {
      fout << inst.first << endl;
      ++accepted;
    }
  }

  cout << "Rejected: " << rejected << endl;
  cout << "Accepted: " << accepted << endl;

  fout.close();
}

void tablegenerator::extractChamp(Index cMetric, char* name,
				  char* filename) {
  ofstream fout;
  int rejected = 0;
  int accepted = 0;
  string s_name(name);
  Index h;

  if (Algo_names.find(s_name) == Algo_names.end()) {
    cerr << "*** Algorithm " << s_name << " does not exist!" << endl;
    exit(EXIT_FAILURE);
  } else {
    h = Algo_names[s_name];
  }

  fout.open(filename);

  for (auto& inst : Inst_names) {
    int i = Inst_names[inst.first];
    bool found;

    switch (cMetric) {
    case 0:
      found = SumBySeeds_mat[i][h] == MaxByAlg_SumBySeeds_vect[i];
      break;
    case 1:
      found = SumBySeeds_mat[i][h] > MaxByAlgButOne_SumBySeeds_mat[i][h];
      break;
    case 2:
      found = gte(MaxBySeeds_mat[i][h], MaxByAlg_MaxBySeeds_vect[i]) == 0;
      break;
    case 3:
      found = (gte(MaxBySeeds_mat[i][h], MaxByAlg_MaxBySeeds_vect[i]) == 0) &&
	(gte(TimeMaxBySeeds_mat[i][h], TimeMaxByAlg_MaxBySeeds_vect[i]) == 0);
      break;
    }

    if (found) {
      fout << inst.first << endl;
      ++accepted;
    } else
      ++rejected;
  }

  cout << "Rejected: " << rejected << endl;
  cout << "Accepted: " << accepted << endl;

  fout.close();
}

//-------------------------------------------------------------------
int main(int argc, char** argv) {
  char* parameterfile = nullptr;
  char* difficult = nullptr;  // the file with the names of the difficult
                              // instances
  char* rInstances = nullptr; // the file with the names of the instances
                              // where algorithm champ is best in the 
                              // cMetric ranking
  char* champ = nullptr;
  Index cMetric = 0;
  int opt;
  int level = -1;  // the level of easiness for the instances
                   // to be put in file difficult
  double scaling = -1.0;
  bool print_help = false;
  bool absolute_values = false;

  if (argc == 1)
    print_help = true;
  else
    while ((opt = getopt(argc, argv, "p:s:had:l:c:r:m:")) != -1) {
      switch (opt) {
        case 'a':
          absolute_values = true;
          break;
        case 'p':
          parameterfile = optarg;
          break;
        case 'd':
          difficult = optarg;
          break;
        case 'l':
          level = atoi(optarg);
          break;
        case 's':
          scaling = atof(optarg);
          break;
        case 'c':
          champ = optarg;
          break;
        case 'r':
          rInstances = optarg;
          break;
        case 'm':
          cMetric = atoi(optarg);
          break;
        case 'h':
        default:
          print_help = true;
      }
    }

  if (!print_help) {
    cerr << endl << "*** ";
    
    if (parameterfile == nullptr) {
      cerr << "Parameter -p <parameter_file> is mandatory" << endl;
      print_help = true;
    }

    if (level >= 0) {
      if (difficult == nullptr) {
        cerr << "Option -l requires option -d <file_name>" << endl;
        print_help = true;
      }
    }

    if (difficult != nullptr) {
      if (scaling >= 0 || absolute_values) {
        cerr << "Option -d is not compatible with options -s and -a" << endl;
        print_help = true;
      }
    }

    if (rInstances != nullptr && champ == nullptr) {
      cerr << "Option -r <file_name> requires option -c <algorithm>" << endl;
      print_help = true;
    }

    if (rInstances == nullptr && champ != nullptr) {
      cerr << "Option -c <algorithm> requires option -r <file_name>" << endl;
      print_help = true;
    }

    if (cMetric > 0) {
      if (rInstances == nullptr || champ == nullptr) {
        cerr << "Option -m requires options -c <algorithm> and -r <file_name>"
	     << endl;
        print_help = true;
      }
    }

    if (cMetric > 3) {
        cerr << "<metric> value must be between 0 and 3" << endl;
        print_help = true;
    }
    
    if (scaling < 0) scaling = 1.0;

    if (scaling > 1.0 || scaling <= 0.0) {
      cerr << "time scaling must be > 0 and <= 1.0" << endl;
      print_help = true;
    }
  }
  if (print_help) {
    string tmp(argv[0]);
    cerr << endl
         << "Usage: " << argv[0] << " -p <parameter_file> "
         << "[-s <time scaling>] [-a] " << endl;
    cerr << string(tmp.length() + 8, ' ') << "[-d <file_name>] "
         << "[-l <level>]" << endl;
    cerr << string(tmp.length() + 8, ' ') << "[-c <algorithm> -r <file_name> "
         << "[-m <metric>]]" << endl
         << endl;
    cerr << " -p <parametr_file> is mandatory" << endl;
    cerr << " -s <time scaling> (>0 and <= 1.0) [default: 1.0]: all time limits"
         << endl
         << "    are scaled by this factor." << endl;
    cerr
        << " -a flag [default: false]: statistics are made with absolute values"
        << endl
        << "    rather than percentages." << endl;
    cerr << " -d <file_name>: the names of all instances whose best value is "
            "found"
         << endl
         << "    by at most <level> algorithms for all seed values are put to "
            "this"
         << endl
         << "    file." << endl;
    cerr << " -l <level> (>=0) [default: number of algorithms / 2])." << endl;
    cerr << " -r <file_name>: the names of all instances where <algorithm>"
	 << " is best" << endl << "    in <metric> ranking." << endl;
    cerr << " -c <algorithm> (no check is performed whether this is among"
	 << " the" << endl << "    algorithms in the results file)." << endl;
    cerr << " -m <metric> (>=0 and <= 3) [default: 0]. 0:FE, 1:FS, 2:BA"
	 << " 3:EBA." << endl;
    cerr << endl;
    exit(EXIT_FAILURE);
  }

  tablegenerator TB;
  TB.time_limit_scaling = scaling;
  TB.read_parameters(parameterfile);
  if (difficult != nullptr) TB.Set_instances_set();
  TB.read_selected_instances();
  TB.read_selected_algorithms();
  TB.read_results_file();
  TB.absolute_values = absolute_values;

  cout << "END OF INPUT " << endl;

  if (difficult != nullptr)
    TB.extract(level, difficult);
  else {
    cout << "START STATISTICS" << endl;
    TB.ComputeStatistics();
    if (rInstances != nullptr)
      TB.extractChamp(cMetric, champ, rInstances);
    else
      TB.writetable();
    cout << "END STATISTICS" << endl;
  }
  return EXIT_SUCCESS;
}

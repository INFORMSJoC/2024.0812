# Data for Experiment Replication

This directory contains the data necessary to replicate the experiments. It includes various file types that serve as input for the analysis and the associated scripts.

**Contents:**

*   **Instance Lists:** These `.txt` files specify the problem instances used in the analysis. Each instance name is listed on a separate line. Blank lines and lines beginning with '#' are ignored.
*   **Algorithm Lists:** Files named `Algorithms_*` contain lists of algorithm names. These lists define which algorithms are included in a particular analysis. Each algorithm name is on a separate line. Blank lines and lines starting with '#' are ignored. The filename indicates the specific set of algorithms included in that list.
*   **Algorithm Name Mapping:** The `Alg_names.csv` file maps the abbreviated algorithm names used in the data to their corresponding full names. This is useful for generating human-readable output and tables. The file is in CSV format with two columns: the first column contains the abbreviated name, and the second column contains the full name.
*   **Configuration Files:** Files named `conf_*` are configuration files that instruct the `tablegenerator` program on how to generate the various output tables.
*   **Graph Metrics:** The `metrics_interesting.csv` file contains graph metrics computed for the "interesting instances" in the DGS study and coincides with the
file `metrics.csv`, which is available for download at [MQLib/data](https://github.com/MQLib/MQLib/tree/master/data). The file is in CSV format. The first line is the header, and subsequent lines contain the data.
*   **Summary Data:** The `summary_DGS.csv` file contains summary data for all instances in the DGS study. This file is used to extract subsets of instances based on specific characteristics (e.g., density or percentage of negative-weight edges). The file is in CSV format. The first line is the header, and subsequent lines contain the data.
*   **BigTorii/:** This subdirectory contains the BigTorii dataset.
*   **Chimera/:** This subdirectory contains the Chimera dataset.
*   **McSparse/:** This subdirectory provides instructions on how to obtain the data for the McSparse experiments.
*   **RawResults/:** This subdirectory contains the raw results from the algorithm runs. These files are the primary input for the `tablegenerator` program. Each file typically contains performance data for multiple algorithms across different instances and seeds (runs). The format is CSV. The first line is the header, and subsequent lines contain the data. The columns are:
    1.  `timestamp`: The timestamp of the run.
    2.  `graphname`: The name of the graph instance.
    3.  `algorithm`: The name of the algorithm.
    4.  `seed`: The random seed used for the run.
    5.  `time limit`: The time limit imposed on the algorithm.
    6.  `objective`: The objective value achieved by the algorithm.
    7.  `time`: The execution time of the algorithm.
    8.  `history`: This field contains additional performance history data presented as a list enclosed in square brackets. The list consists of `<value>:<time>` pairs separated by semicolons. Each `<value>` represents the weight of a solution, and `<time>` indicates the elapsed time in seconds from the start of execution when that solution was generated.

**Purpose:**

The files in this directory are essential for:

1.  Configuring the `tablegenerator` program to analyze specific instances and algorithms.
2.  Providing the raw performance data for the `tablegenerator` to process.
3.  Generating output tables and statistics.
4.  Filtering and sorting data based on various metrics.
5.  Generating data for plotting performance curves.

**Dependencies:**

The scripts in the `scripts/` directory rely on the structure and content of these files.

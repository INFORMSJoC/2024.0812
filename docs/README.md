# DGS instances

The data of the DGS instances can be downloaded with the python code
`downloadAllGraphs.py` available at

<https://github.com/MQLib/MQLib/blob/master/scripts>.

The instructions on how to install and use this code are provided at

<https://github.com/MQLib/MQLib/blob/master/scripts/README.md>.

The raw data file containing the results from all runs of 37 heuristics in the DGS study, along with the results from NN-SSS, is located at `data/RawResults/DGS_all_NN.csv`. Due to its large size (over 400MB), this file has been divided into five smaller files: `Res_DGS_all_NN_aa`, `Res_DGS_all_NN_ab`, `Res_DGS_all_NN_ac`, `Res_DGS_all_NN_ad`, and `Res_DGS_all_NN_ae`. The original file, `Res_DGS_all_NN.csv`, can be recreated using the following commands:

        cd data/RawResults
        cat Res_DGS_all_NN_a* > Res_DGS_all_NN.csv

All executable programs used in the subsequent steps were built using these commands:

        cd <home>
        rm -rf build
        mkdir build
        make

Comprehensive instructions for creating each table and figure are detailed below. Please ensure the directory `results` exists within your home directory (`<home>`). This directory `results` will store all files produced by the provided code and scripts.

Alternatively, to generate all tables and figures at once, the script `<home>/scripts/make_all.sh` can be executed:

       cd <home>
       bash scripts/make_all.sh

This script assumes that the `build` directory contains all necessary executables and that the `Res_DGS_all_NN.csv` file has been reconstructed according to the instructions above.

## The "interesting" instances

The names of all "interesting" instances are generated with the command

        cd <home>
        build/tablegenerator -p data/conf_interesting -d results/DGS_interesting.txt

where `conf_interesting` is the file with the following content:

    data/RawResults/DGS_study_results.csv
    all_instances
    some_algorithms data/Algorithms_all
    no_file

The output looks like:

        Read 609760 records. 
        0 where skipped because uninteresting instances
        0 where skipped because uninteresting algorithms
        END OF INPUT 
        Rejected: 661
        Accepted: 2635

and file `DGS_interesting.txt` is created in the directory
`results`. If everything works, this file should coincide
with the one with the same name but stored in the directory `data`.

The one shown above is the way used in this study to generate all 
statistics in order to be consistent with the test-bed used in the 
DGS study. The results data file defined in `conf_interesting` is 
the one of the DGS study available at the MQLib. Running 
`tablegenerator` on the results data file `Res_DGS_all_NN.csv`,
the one produced in this study, with the command

        cd <home>
        build/tablegenerator -p data/conf_interesting2 -d results/DGS_interesting2.txt

and the following `conf_interesting2` file

        data/RawResults/Res_DGS_all_NN.csv
        all_instances
        some_algorithms data/Algorithms_all
        no_file

gives a slightly different results with 658 rejected instances and 2638 accepted.

## Table 4 of Supplementary Material

To generate the table, use the command

        cd <home>
        build/tablegenerator -p data/conf_Table4SM

where `conf_Table4SM` is the file with the following content:

        data/RawResults/DGS_study_results.csv
        some_instances data/DGS_interesting.txt
        some_algorithms data/Algorithms_all
        results/Stat_DGS_study.csv

## Table 1

To generate the table, use the command

        cd <home>
        build/tablegenerator -p data/conf_Table1

where `conf_Table1` is the file with the following content:

        data/RawResults/Res_DGS_all_NN.csv
        some_instances data/DGS_interesting.txt
        some_algorithms data/Algorithms_all
        results/Stat_DGS_all.csv

## Table 2

To generate the table, use the command

        cd <home>
        build/tablegenerator -p data/conf_Table2

where `conf_Table2` is the file with the following content:

        data/RawResults/Res_DGS_all_NN.csv
        some_instances data/DGS_interesting.txt
        some_algorithms data/Algorithms_all_NN
        results/Stat_DGS_all_NN.csv

## Table 3 (Table 6 of Supplementary Material)

To generate the table, use the command

        cd <home>
        build/extract -s data/summary_DGS.csv -o sparse.txt -i data/DGS_interesting.txt -d 3
        build/tablegenerator -p data/conf_Table3
	rm -f sparse.txt

where `conf_Table3` is the file with the following content:

        data/RawResults/Res_DGS_all_NN.csv
        some_instances sparse.txt
        some_algorithms data/Algorithms_all_NN
        results/Stat_DGS_sparse_all_NN.csv

## Table 9 of Supplementary Material

To generate the table, use the command

        cd <home>
        build/tablegenerator -p data/conf_Table9SM

where `conf_Table9SM` is the file with the following content:

        data/RawResults/Res_DGS-tor_all_NN_P.csv
        all_instances
        some_algorithms data/Algorithms_all
        results/Stat_DGS-tor_all.csv

## Table 4 (Table 10 of Supplementary Material)

To generate the table, use the command

        cd <home>
        build/tablegenerator -p data/conf_Table4

where `conf_Table4` is the file with the following content:

        data/RawResults/Res_DGS-tor_all_NN_P.csv
        all_instances
        some_algorithms data/Algorithms_all_NN_P
        results/Stat_DGS-tor_all_NN_P.csv

## Table 5 (Table 5 of Supplementary Material)

To generate the table, use the command

        cd <home>
        build/tablegenerator -p data/conf_Table5

where `conf_Table5` is the file with the following content:

        data/RawResults/Res_DGS_all_NN.csv
        some_instances results/DGS_hard.txt
        some_algorithms data/Algorithms_all_NN
        results/Stat_DGS-hard_all_NN.csv

File `DGS_hard.txt` is generated with the command

        build/tablegenerator -p data/conf_hard -d results/DGS_hard.txt -l 0

where `conf_hard` is the file with the following content:

        data/RawResults/Res_DGS_all_NN.csv
        all_instances
        some_algorithms data/Algorithms_all_NN
	no_file

## Table 7 of Supplementary Material

To generate the table, use the command

        cd <home>
        build/extract -s data/summary_DGS.csv -o results/20neg.txt -i data/DGS_interesting.txt -n 20
        build/tablegenerator -p data/conf_Table7SM

where `conf_Table7SM` is the file with the following content:

        data/RawResults/Res_DGS_all_NN.csv
        some_instances results/20neg.txt
        some_algorithms data/Algorithms_all_NN
        results/Stat_DGS_20neg_all_NN.csv

## Table 8 of Supplementary Material

To generate the table, use the command

        cd <home>
        build/extract -s data/summary_DGS.csv -o results/50neg.txt -i data/DGS_interesting.txt -n 50
        build/tablegenerator -p data/conf_Table8SM

where `conf_Table8SM` is the file with the following content:

        data/RawResults/Res_DGS_all_NN.csv
        some_instances results/50neg.txt
        some_algorithms data/Algorithms_all_NN
        results/Stat_DGS_50neg_all_NN.csv

## Figures 7 and 8

The X/Y coordinates of each plot of the two figures are stored in a
directory `results` and generated with the script `genPlot.sh`. For a
given parameter $K$, this many runs are simulated and analyzed as
prescribed in the configuration file `conf_Figures78`. Call $T$ the
maximum allowed time used to produce the results file defined in
`conf_Figures78`. For the $i$-th run, the maximum allowed time is set to
$i\times T/K$ and the value of a given metric $M$ for each algorithm
`<alg>` specified in `conf` is recorded in the file
`results/<alg>.dat`.

For both figures 7 and 8, the same following configuration `conf_Figures78` is
used:

        data/RawResults/Res_DGS_all_NN.csv
        some_instances data/DGS_interesting.txt
        some_algorithms data/Algorithms_all_NN
        no_file

The command lines for generating the data of Figure 7 are the following:

        cd <home>
        bash scripts/genPlot.sh data/conf_Figures78 10 1

The commands for Figure 8 are:

        cd <home>
        bash scripts/genPlot.sh data/conf_Figures78 10 4

These commands generate one file named `<name>.dat` per each algorithm
that would be listed in the file `<home>/results/Stat_DGS_all_NN.csv`,
where `<name>` is the algorithm name used in that file. Each file
contains as many line of X/Y coordinates as indicated in the second
parameter given to the script (10 in the case of figures 7 and 8).

The numeric codes for the metrics, to be used as the third parameter of
the script, are as follows:

        1: FE,  2: FS,  3: BA,  4: EBA,  5: WD,  6: MD,  7: BD, 8: AR.

## Tables 1 and 2 of Supplementary Material

For each `<sample size>` in the set {119, 264, 527, 862, 1317}, the corresponding
section of the tables is obtained with the following commands:

        cd <home>
        bash scripts/pre_sort.sh <sample_size>

A `csv` file is produced in the directory `results` with name
`Big_stat_<sample size>.csv` containing a table with two columns: one
with the metric name and the other with the corresponding value $|n_d-n_a|$.

# Chimera instances

The instances are contained in the 1445 zipped files of the directory
`<home>/data/Chimera`.

To generate new instances obtained from the above by removing the zero
weighted edges and the resulting isolated nodes, do the following.

Create a directory `data/de_chimera0`, where the new instances
will be stored. Then enter the directory `data/Chimera` and
execute the script `scripts/de_chim.sh`.

        cd <home>/data
        mkdir de_chimera0
        bash ../scripts/de_chim.sh

## Table 6 (Table 11 of Supplementary Material)

To generate the table, use the command

        cd <home>
        build/tablegenerator -p data/conf_Table11SM

where `conf_Table11SM` is the file with the following content:

        data/RawResults/Res_chimera_all_NN_SELBY.csv
        some_instances data/chimera_onc16.txt
        some_algorithms data/Algorithms_all_NN_SELBY
        results/Stat_chimera_all_NN_SELBY.csv

The file `chimera_onc16.txt` contains the name of the Chimera
instances excluding those with name starting with `pga`.

# BigTorii instances

The instances are contained in the 40 zipped files of the directory

        <home>/data/BigTorii.

## Table 7 (Table 12 of Supplementary Material)

To generate the table, use the command

        cd <home>
        build/tablegenerator -p data/conf_Table12SM

where `conf_Table12SM` is the file with the following content:

        data/RawResults/Res_BigTorii_all_NN_P.csv
        all_instances
        some_algorithms data/Algorithms_all
        results/Stat_BigTorii_all.csv

## Table 13 of Supplementary Material

To generate the table, use the command

        cd <home>
        build/tablegenerator -p data/conf_Table13SM

where `conf_Table13SM` is the file with the following content:

        data/RawResults/Res_BigTorii_all_NN_P.csv
        some_instances data/BigTorii_g.txt
        some_algorithms data/Algorithms_all
        results/Stat_BigTorii_g_all.csv

## Table 14 of Supplementary Material

To generate the table, use the command

        cd  <home>
        build/tablegenerator -p data/conf_Table14SM

where `conf_Table14SM` is the file with the following content:

        data/RawResults/Res_BigTorii_all_NN_P.csv
        some_instances data/BigTorii_pm.txt
        some_algorithms data/Algorithms_all
        results/Stat_BigTorii_pm_all.csv

## Table 8 (Table 15 of Supplementary Material)

To generate the table, use the command

        cd <home>
        build/tablegenerator -p data/conf_Table8

where `conf_Table8` is the file with the following content:

        data/RawResults/Res_BigTorii_all_NN_P.csv
        all_instances
        some_algorithms data/Algorithms_all_NN
        results/Stat_BigTorii_all_NN.csv

## Table 9 (Table 16 of Supplementary Material)

To generate the table, use the command

        cd <home>
        build/tablegenerator -p data/conf_Table9

where `conf_Table9` is the file with the following content:

        data/RawResults/Res_BigTorii_all_NN_P.csv
        all_instances
        some_algorithms data/Algorithms_all_NN_P
        results/Stat_BigTorii_all_NN_P.csv

# DIMACS instances

The two instances `torus_g3D_15` and `torus_pm3D_15` can be downloaded
from

<https://www.dimacs.rutgers.edu/archive/Challenges/Seventh/Instances>

choosing the files `torusg3-15.dat.gz` and `toruspm3-15-50.dat.gz`.dat`. 
The instances `torus_g3D_15n` and `torus_pm3D_15n` are generated from 
`torus_g3D_15` and `torus_pm3D_15`, respectively, using the code
`<home>/src/negate.cpp`. For example,

        build/negate < torus_g3D_15.dat > torus_g3D_15n.dat 

## Table 17 of Supplementary Material

To generate the table, use the command

        cd <home>
        build/tablegenerator -p data/conf_Table17SM

where `conf_Table17SM` is the file with the following content:

        data/RawResults/Res_dimacs_all_NN.csv
        all_instances
        some_algorithms data/Algorithms_all_NN
        results/Stat_dimacs_all_NN.csv

# QPLib instances

The data of the QPLib instances can be downloaded from the site

<https://qplib.zib.de/instances.html>.

In this archive there are 23 QUBO instances that can be identified by
selecting the options "no constraints" ('N' in column 'C'). Each
instance is stored in four different formats. To build the instance
set, the file with format 'qplib' has to be downloaded and than
translated to the same format of all the other instances of the study
using the code `<home>/src/qplib2mc.cpp`. For example:

        <home>/build/qplib2mc < QPLIB_3506.qplib > QPLIB_3506.txt

## Table 10 (Table 18 of Supplementary Material)

To generate the table, use the command

        cd <home>
        build/tablegenerator -p data/conf_Table10

where `conf_Table10` is the file with the following content:

        data/RawResults/Res_qplib_all_NN.csv
        all_instances
        some_algorithms data/Algorithms_all_NN
        results/Stat_qplib_all_NN.csv

# McSparse instances

The data of the McSparse instances can be downloaded using the
instructions given in the file [README.md](../data/McSparse/README.md)
of the directory `<home>/data/McSparse`.

## Table 19 of Supplementary Material

To generate the table, use the command

        cd <home>
        build/tablegenerator -p data/conf_Table19SM -a

where `conf_Table19SM` is the file with the following content:

        data/RawResults/Res_DGS_all_NN.csv
	some_instances data/DGS_interesting.txt
	some_algorithms data/Algorithms_all
	results/Stat_DGS_all_abs.csv

## Table 20 of Supplementary Material

To generate the table, use the command

        cd <home>
	build/tablegenerator -p data/conf_Table20SMa -r results/DGS_NN_best.txt -c NN-SSS -m 1
	build/tablegenerator -p data/conf_Table20SMb -a

where `conf_Table20SMa` is the file with the following content:

        data/RawResults/Res_DGS_all_NN.csv
	some_instances data/DGS_interesting.txt
	some_algorithms data/Algorithms_all_NN
	no_file

and `conf_Table20SMb` is the file with the following content:

        data/RawResults/Res_DGS_all_NN.csv
	some_instances results/DGS_NN_best.txt
	some_algorithms data/Algorithms_all
	results/Stat_DGS_all_NN_NN_best.csv

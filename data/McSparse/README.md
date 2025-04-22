# Obtaining Data for McSparse Instances

This document describes how to obtain and prepare the data files for the McSparse instances used in the experiments, as referenced in Table 12.

**Downloading Data from MQLib:**

Many of the required data files are available in the MQLib. You can download them using the following command:

    python3 downloadGraph.py 'name'.zip > 'name'.zip

For each of the instances of Table 12, the instruction to setup the corresponding data file follow.

### be120.3.5

The original data file be120.3.5.sparse is downloadable from

    https://biqmac.aau.at/library/biq/be/be120.3.5.sparse

be120.3.5 is obtained from be120.3.5.sparse by running the command

    Qubo2mc < be120.3.5.sparse > be120.3.5


Problem g000442 of the MQLib contains the off-diagonal entries of
be120.3.5.sparse.

### be250.3

The data file is obtainable from be250.3.sparse by running the command

    Qubo2mc < be250.3.sparse > be250.3

Instance be250.3.sparse is downloadable from

    https://biqmac.aau.at/library/biq/be/be250.3.sparse


### gka2c

This instance is contained in MQLib with name gka.2c.

### gka4d

This instance is contained in MQLib with name gka.4d.

### gka5c

This instance is contained in MQLib with name gka.5c.

### gka7a

This instance is contained in MQLib with name gka.7a.

### bqp250-3

The data file is obtained from the original bqp250-3.sparse by running the command

    Qubo2mc < bqp250-3.sparse > bqp250-3

Instance bqp250-3.sparse is downloadable from

    https://biqmac.aau.at/library/biq/beasley/bqp250-3.sparse


Problem g002146 of the MQLib contains the off-diagonal entries of
bqp250-3.sparse.

### imgseg_138032_s8r

Is obtained from imgseg_138032 by running the command

    scale_img < imgseg_138032 > imgseg_138032_s8r

The instance imgseg_138032 is contained in MQLib.

### mannino_k487b

This instance is downloadable from

    http://bqp.cs.uni-bonn.de/library/html/mannino_k487b.html
    

### g000981

This instance is contained in MQLib.

### pm1s_100.3

This instance is contained in MQLib with name g001134

### pw01_100.0

This instance is contained in MQLib with name g000391.

### bio-diseasome

It is obtained from bio-diseasome.mtx by running the command

    netRep2mc < bio-diseasome.mtx > bio-diseasome

The file bio-diseasome.mtx can be downloaded from

    https://networkrepository.com/bio-diseasome.php.

### ca-netscience

The data file ca-netscience.mtx can be downloaded from

    https://networkrepository.com/ca_netscience.php

Remove character '%' (if present) starting the second line of file ca-netscience.mtx. Finally, generate file ca-netscience with the
command:

    netRep2mc < ca-netscience.mtx > ca-netscience

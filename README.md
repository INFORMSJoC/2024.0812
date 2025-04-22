[![INFORMS Journal on Computing Logo](https://INFORMSJoC.github.io/logos/INFORMS_Journal_on_Computing_Header.jpg)](https://pubsonline.informs.org/journal/ijoc)

# SSS Algorithms for Max-Cut

This archive is distributed in association with the [INFORMS Journal on
Computing](https://pubsonline.informs.org/journal/ijoc) under the [MIT License](LICENSE).

The software and data in this repository are a snapshot of the software and data
that were used in the research reported on in the paper 
[SSS Algorithms for Max-Cut](https://doi.org/10.1287/ijoc.2024.0812) by C. Gentile, G. Rinaldi, and E. Salgado. 


## Cite

To cite the contents of this repository, please cite both the paper and this repo, using their respective DOIs.

https://doi.org/10.1287/ijoc.2024.0812

https://doi.org/10.1287/ijoc.2024.0812.cd

Below is the BibTex for citing this snapshot of the repository.

```
@misc{Gentile2024,
  author =        {C. Gentile and G. Rinaldi and E. Salgado},
  publisher =     {INFORMS Journal on Computing},
  title =         {{SSS Algorithms for Max-Cut}},
  year =          {2024},
  doi =           {10.1287/ijoc.2024.0812.cd},
  url =           {https://github.com/INFORMSJoC/2024.0812},
  note =          {Available for download at https://github.com/INFORMSJoC/2024.0812},
}  
```

## Description

The purpose of this repository is to enable the reproduction of the tables and figures presented in the paper and its Supplementary Material. Additionally, the provided software allows for the calculation of further statistics using the raw experimental data described in the paper.

These raw data encompass the results of over 750,000 executions of the 37 algorithms from the [MQLib](https://github.com/MQLib/MQLib), as well as the three algorithms detailed in the paper. For each execution, a complete history is recorded, including the sequence of best solutions found and the timestamps of their generation. This history facilitates the simulation of new executions with maximum allowed times scaled by any factor between 0 and 1.

## Computing Environment

The C++ codes and Bash scripts are intended for use in a Linux environment equipped with standard libraries and utilities such as the g++ compiler, make, bc, and the Bash shell.

Instructions on building and utilizing the software are provided in the [README](docs/README.md) file located in the `<home>/docs` directory.

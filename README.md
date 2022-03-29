# biobank-plink-assoc
This repository provides a tool to perform Genome-wide-association study (GWAS). 
The data analysis is achived using [plink](https://github.com/chrchang/plink-ng). The contribution of this repository is that 
1. ```convert\(ph\g)enotype_convert```: A more flexible parsing tool that can read formatted text outputs with less stringent format requirment. For example, Data from UK Biobank can be parsed with the right parameters.
2. ```convert-assoc-qt```: A rudimentary cross-platform GUI using [Qt](https://www.qt.io/) that guides the user through the steps of performing association studies. 

## Components and their relations
```
├───CMakeLists.txt: Cmake for Qt GUI program
├───convert: library for parsing texts to plink-compatible format
│   ├───genotype_convert.h/.cpp
│   └───phenotype_convert.h/.cpp
├───convert-assoc-qt: GUI program
├───plink_1.9:
│   └───CMakeLists.txt: "Binary dependency-free" version of plink
├───sample_input: Sample input files to test functionality
├───test-vsproj: tests for convert (Visual Studio project only)
└───zlib-1.2.11: Dependency for plink
    └───CMakeLists.txt
```
## Installation guide
As of now, we do not provide an installer or a binary build, and Qt is required to build the program. [Install Qt](https://www.qt.io/download-open-source) to get started.

On Windows machines, use ```MinGW``` or ```LLVM``` toolkits to build this program, as ```MSVC``` cannot compile ```plink```.

Note: The main program uses ```plink``` as a binary executable; however, build ```convert_assoc``` project does not build ```plink``` automatically. Either *build all projects* at least once to build it, or provide an alternative ```plink``` binary if you wish.

## Usage guide
The program is (hopefully) self-explanatory. The program guides the user through the [flowchart](https://github.com/DSPsleeporg/biobank-plink-assoc/blob/main/dataflow_tikz_chart.pdf) to 
1. Parse the genotype and phenotype files with varying column layouts and delimiters
2. Choose a binary partition of phenotypes in the case of discrete phenotypes

A video demo using the sample inputs is in the plan.

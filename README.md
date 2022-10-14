## MPM-Geomechanics

The MPM-Geomechanics is an implementation of the Material Point Method to simulate large strain geomechanical problems.

The main objectives of this program are:

- Provide an open source platform suitable for the study and application of MPM in various geomechanical problems.

- provide an adequate computational tool to simulate geomechanical problems involving large deformations.

## Program Features

The main features of the program are:

- Three-dimensional formulation
- Dynamic formulation
- Shared memory parallelization
- Elastic constitutive models
- Elasto-plastic constitutive models
- Softening/Hardening by deformation
- Coupled hydromechanical formulation (in development)

## Compilation

The `/make` directory contains the makefile. To compile the program run make in the `/make` folder:

`/make$ make`

## Documentation

The program documentation is generated using Doxygen. The DoxyFile file is located in the `/doxygen` directory. To generate the documentation, run:

`/doxygen$ doxygen`

The HTML documentation is located in the `/doxygen/html` directory. To generate the PDF file, run make inside the `doxy/latex` directory.

`/doxygen/latex$ make`

## Execution

In order the run the program call it with the name of the input file, for example:

`/make$ ./MPM-Geomechanics example.json`
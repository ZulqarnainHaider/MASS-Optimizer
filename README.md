# MASS-Optimizer

# Installation Requirements and Steps

## Install CPLEX
* Download and Install CPLEX 12.8
* Add following PATH variables to .bashrc
    * export LD_LIBRARY_PATH="/opt/cplex128/cplex/bin/X86-64_linux":$LD_LIBRARY_PATH 
    * export LD_LIBRARY_PATH="/opt/cplex128/cplex/bin/x86-64_linux":$LD_LIBRARY_PATH 
    * export CLASSPATH="/opt/cplex1263/cplex/lib/cplex.jar":$CLASSPATH

## Install SCIP
* Download scipoptsuite-5.0.1 binaries for Linux, i.e., scipoptsuite-5.0.1.tgz 
    * Go to /path/to/scipoptsuite-5.0.1.tgz
    * `tar -xvf scipoptsuite-5.0.1.tgz`
* Compile scipoptsuite-5.0.1
    * `cd scipoptsuite-5.0.1`
    * `mkdir build`
    * `cd build`
    * `cmake ..`
    * `make`
    * `make check`
* Install scipoptsuite-5.0.1
    * Go to /path/to/scipoptsuite-5.0.1.tgz
    * `mkdir local`
    * Install SCIP. For example, `make install DESTDIR=/path/to/local`
* Add PATH to SCIP  
    * `export SCIPOPTDIR="/path/to/local":$SCIPOPTDIR`
    * Add <export SCIPOPTDIR="/path/to/local":$SCIPOPTDIR> to .bashrc file 
* run SCIP Standalone  
    * To run SCIP standalone after compilation, enter `/path/to/local/usr/local/bin/scip`
  
## Install Julia
* Download and install Julia Version 1.1.0
* To use Julia, 

# Add CPLEX, SCIP, OOESAlgorithm and other required packages in Julia
* To use SCIP in Julia, use the following steps:
    * Add PATH to SCIP installation  
    * `export SCIPOPTDIR="/path/to/local":$SCIPOPTDIR`
    * Add <export SCIPOPTDIR="/path/to/local":$SCIPOPTDIR> to .bashrc file
    * run Julia to enter Julia REPL
    * `import Pkg`
    *  To enter the pkg mode (Pkg REPL) enter: `]` 
    * `add SCIP@v0.6.1`
    * `pin SCIP`  #this will pin the SCIP.jl version to 0.6.1 so it is compatible with SCIP installation
    * `build SCIP`
    * `using SCIP`
* To use CPLEX in Julia, use the following steps:
    * In .bashrc add the path to CPLEX binaries in folder x86-64_linux inside CPLEX installattion by adding the following environment variable: <export CPLEX_STUDIO_BINARIES="/opt/cplex128/cplex/bin/x86-64_linux":$CPLEX_STUDIO_BINARIES>
    * `import Pkg`
    * `Pkg.add("CPLEX")`
    * `Pkg.build("CPLEX")`
    * `using CPLEX`
 * To check the versions of CPLEX and SCIP installed in Julia, run `Pkg.installed()`
 * Find more details on (https://github.com/SCIP-Interfaces/SCIP.jl) and (https://github.com/JuliaOpt/CPLEX.jl)
 * To use OOESAlgorithm in Julia, use the following steps:
    * `import Pkg`
    * `Pkg.add("OOESAlgorithm")`
    * `Pkg.build("OOESAlgorithm")`
    * `using OOESAlgorithm`
    * See details here: (http://eng.usf.edu/~amsierra/documents/Documentation_OOESAlg.pdf)
 * Also add and build _DataFrames_ and _CSV_ packages
  
# Steps to create .lp Files
* In the first step, data file Input_File.txt is converted into a .lp file model.lp. 
  
## Requirements/File Structure
* The directory HOM-LPCreator must contain the following files: 
    * Input_File.txt : input data file in designated format
    * HOM_LPCreator: executable file which contains the object code and the library code
    *	Main.cpp: source code file
    *	Makefile: make utility for building executable from the source code
    * Model.lp : The output file in .lp format which contains the optimization model

* The directory HOM-LPCreator should contain the following directories: 
    * build : contains object files
    *	nbproject: contains project makefiles
    
## Modifying the Makefiles
*	Go to Path ..\HOM-LPCreator\nbproject
* Modify the makefiles to add Include paths to CPLEX libraries to the makefiles
    * Open Makefile-Release.mk
    * Modify the **#Link Libraries and Options**, and **#Build Targets** to link CPLEX libraries and include paths by changing the following parts:<br/>
    * Under **# Link Libraries and Options**, modify the following parts: <br/>
        * `LDLIBSOPTIONS=/opt/cplex128/cplex/lib/x86-64_linux/static_pic/libilocplex.a /opt/cplex128/cplex/lib/x86-64_linux/static_pic/libcplex.a /opt/cplex128/concert/lib/x86-64_linux/static_pic/libconcert.a lm -lpthread -ldl`<br/>
    * Under **# Build Targets**, modify the following parts: <br/>
        * `HOM_LPCreator: /opt/cplex128/cplex/lib/x86-64_linux/static_pic/libilocplex.a`

        * `HOM_LPCreator: /opt/cplex128/cplex/lib/x86-64_linux/static_pic/libcplex.a`

        * `HOM_LPCreator: /opt/cplex128/concert/lib/x86-64_linux/static_pic/libconcert.a`
    
        * `$(COMPILE.cc) -O2 -DIL_STD -I/opt/cplex128/concert/include-I/opt/cplex128/concert/include-MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp`
  
    * Open Makefile-Debug.mk
    * Modify the **#Link Libraries and Options**, and **#Build Targets** to link CPLEX libraries and include paths by changing the following parts:<br/>
    * Under **# Link Libraries and Options**, modify the following parts: <br/>
        * `LDLIBSOPTIONS=/opt/cplex128/cplex/lib/x86-64_linux/static_pic /libilocplex.a /opt/cplex128/cplex/lib/x86-64_linux/static_pic /libcplex.a /opt/cplex128/cplex/lib/x86-64_linux/static_pic /libconcert.a -lm -lpthread -ldl`
    * Under **# Build Targets**, modify the following parts: <br/>
        * `HOM_LPCreator: /opt/cplex128/cplex/lib/x86-64_linux/static_pic/libilocplex.a`

        * `HOM_LPCreator: /opt/cplex128/cplex/lib/x86-64_linux/static_pic/libcplex.a`

        * `HOM_LPCreator: /opt/cplex128/concert/lib/x86-64_linux/static_pic/libconcert.a`

        * `$(COMPILE.cc) -g -DIL_STD -I/opt/cplex128/concert/include-I/opt/cplex128/concert/include-MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp`

## Executing the Script
* Given a data file Input_File.txt, run the following script on command line inside the directory HOM-LPCreator:
    * `#! /bin/sh`
    * `make clean`
    * `make`
    *`./HOM_LPCreator Input_File.txt`

* The program gives Model.lp as its output
* The program also gives optimizer_output_file1.txt and optimizer_output_file2.txt as output. These files will in turn be modified by the _Optimizer_


# Running the final script file with julia
* To tell your operating system that it should run the script using Julia, you can use what is called the shebang syntax. To do this, just use the following line on the very top of your script: #!/usr/bin/env julia
* With this as the first line of the script, the OS will search for "julia" on the path, and use it to run the script.
  

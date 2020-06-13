# MASS-Optimizer

# Installation Requirements and Steps


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
    * From within build directory, Install SCIP. For example, `make install DESTDIR=/path/to/local`
* Add PATH to SCIP  
    * `export SCIPOPTDIR="/path/to/local/usr/local":$SCIPOPTDIR`
    * Add <export SCIPOPTDIR="/path/to/local/usr/local":$SCIPOPTDIR> to .bashrc file 
* run SCIP Standalone  
    * To run SCIP standalone after compilation, enter `/path/to/local/usr/local/bin/scip`
  
## Install Julia
* Download and install Julia Version 1.1.0. See for example: (https://github.com/gher-ulg/Documentation/wiki/Install-julia-on-Linux)
* To use Julia, enter `~/julia-1.1.0/bin/julia`
* To add packages, use `import Pkg` and `Pkg.add("package_name")`
* To set the number of threads on the operating system level before julia starts, edit the bashrc file to add the following environment variable: export JULIA_NUM_THREADS=10:$JULIA_NUM_THREADS. The number of threads can be modified.
* Make sure the number of threads used by _optimizer_ is less than the number of threads available to Julia.  

# Add SCIP, OOESAlgorithm and other required packages in Julia
* To use SCIP in Julia, use the following steps:
    * Add PATH to SCIP installation  
    * `export SCIPOPTDIR="/path/to/local":$SCIPOPTDIR`
    * Add <export SCIPOPTDIR="/path/to/local":$SCIPOPTDIR> to .bashrc file
    * run `Julia` to enter Julia REPL
    * `import Pkg`
    *  To enter the pkg mode (Pkg REPL) enter: `]` 
    * `add SCIP@v0.6.1`
    * `pin SCIP`  #this will pin the SCIP.jl version to 0.6.1 so it is compatible with SCIP installation
    * `build SCIP`
    * `using SCIP`
    * The file /.julia/packages/SCIP/.../src/mpb_interface.jl of the package SCIP.jl v0.6.1 has to be modified manually. In lines 185, 186, 192, 193, 207 and 208, numvar(m) has to be changed by MathProgBase.numvar(m).
* To use CPLEX in Julia, use the following steps:
 * To check the version of SCIP installed in Julia, run `Pkg.installed()`
 * Find more details on (https://github.com/SCIP-Interfaces/SCIP.jl).
 * To use OOESAlgorithm in Julia, use the following steps:
    * `import Pkg`
    * `Pkg.add("OOESAlgorithm")`
    * `Pkg.build("OOESAlgorithm")`
    * `using OOESAlgorithm`
    * See details here: (http://eng.usf.edu/~amsierra/documents/Documentation_OOESAlg.pdf)
 * To use multiple threads for CPLEX or SCIP solvers while solving MIP in OOES algorithm, use the following steps:
    * The file /.julia/packages/OOESAlgorithm/faYKc/src/initial_operations.jl has to be modified manually. For SCIP, modify the line 89 to `mip_solver=SCIPSolver("display/verblevel", 0, "parallel/minnthreads", 4, "parallel/maxnthreads", 8, "limits/gap", relative_gap)`. For CPLEX, modify the line 87 to `mip_solver=CplexSolver(CPX_PARAM_SCRIND=0, CPX_PARAM_THREADS=8, CPX_PARAM_EPGAP=relative_gap, CPX_PARAM_TILIM=60)`.

  
# Steps to create .lp Files
* In the first step, data file Input_File.txt is converted into a .lp file model.lp. 
  
## Requirements/File Structure
* The directory optimizer must contain the following files: 
    * Input_File.txt : input data file in designated format
    <!--- * HOM_LPCreator: executable file which contains the object code and the library code
    *	Main.cpp: source code file 
    *	Makefile: make utility for building executable from the source code -->
    * LPCreator.jl: Julia code for creating Model.lp file given a data input file
    * optimizer.jl: Julia code for running optimizer and writing output
    * Model.lp : The output file in .lp format which contains the optimization model

<!--- * The directory optimizer should contain the following directories: 
    <> * build : contains object files
    <> *	nbproject: contains project makefiles -->
    
<!--- ## Modifying the Makefiles
*	Go to Path ..\optimizer\nbproject
* Modify the makefiles to add Include paths to CPLEX libraries to the makefiles
    * Open Makefile-Release.mk
    * Modify the **#Link Libraries and Options**, and **#Build Targets** to link CPLEX libraries and include paths by changing the following parts:<br/>
    * Under **# Link Libraries and Options**, modify the following parts: <br/>
        * `LDLIBSOPTIONS=/path/to/cplex128/cplex/lib/x86-64_linux/static_pic/libilocplex.a /path/to/cplex128/cplex/lib/x86-64_linux/static_pic/libcplex.a /path/to/cplex128/concert/lib/x86-64_linux/static_pic/libconcert.a lm -lpthread -ldl`<br/>
    * Under **# Build Targets**, modify the following parts: <br/>
        * `HOM_LPCreator: /path/to/cplex128/cplex/lib/x86-64_linux/static_pic/libilocplex.a`

        * `HOM_LPCreator: /path/to/cplex128/cplex/lib/x86-64_linux/static_pic/libcplex.a`

        * `HOM_LPCreator: /path/to/cplex128/concert/lib/x86-64_linux/static_pic/libconcert.a`
    
        * `$(COMPILE.cc) -O2 -DIL_STD -I/path/to/cplex128/concert/include-I/opt/cplex128/concert/include-MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp`
  
    * Open Makefile-Debug.mk
    * Modify the **#Link Libraries and Options**, and **#Build Targets** to link CPLEX libraries and include paths by changing the following parts:<br/>
    * Under **# Link Libraries and Options**, modify the following parts: <br/>
        * `LDLIBSOPTIONS=/path/to/cplex128/cplex/lib/x86-64_linux/static_pic /libilocplex.a /path/to/cplex128/cplex/lib/x86-64_linux/static_pic /libcplex.a /path/to/cplex128/cplex/lib/x86-64_linux/static_pic /libconcert.a -lm -lpthread -ldl`
    * Under **# Build Targets**, modify the following parts: <br/>
        * `HOM_LPCreator: /path/to/cplex128/cplex/lib/x86-64_linux/static_pic/libilocplex.a`

        * `HOM_LPCreator: /path/to/cplex128/cplex/lib/x86-64_linux/static_pic/libcplex.a`

        * `HOM_LPCreator: /path/to/cplex128/concert/lib/x86-64_linux/static_pic/libconcert.a`

        * `$(COMPILE.cc) -g -DIL_STD -I/path/to/cplex128/concert/include-I/opt/cplex128/concert/include-MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp` -->

# Execute the Optimizer Script
* Given a data file Input_File.txt, run the following standalone script line by line in command line inside the directory optimizer:
    <!--- * `#! /bin/sh`
    * `make clean`
    * `make`
    * `./HOM_LPCreator Input_File.txt` -->
    * `path/to/julia -p8 LPCreator.jl`  For example: /home/zulqarnain/workspace/WWTP/julia-1.0.5/bin/julia -p8 LPCreator.jl where -p8 represents the number of threads/processes Julia can use.  
    * `path/to/julia -p8 optimizer.jl`  For example: /home/zulqarnain/workspace/WWTP/julia-1.0.5/bin/julia -p8 optimizer.jl where -p8 represents the number of threads/processes Julia can use. 
    *  _LPCreator.jl_ gives Model.lp as its output. The program also gives optimizer_output_file1.txt as output. The file is in turn modified by _optimizer.jl_
* Else, create a _script_ file, for instance _optimizer.sh_, and copy and paste above lines of code inside the file. Then, run the following commands from inside the directory optimzier
    * `chmod +x optimizer.sh`
    * `ls -l optimizer.sh`
    * `./optimizer.sh`

## Modifying the Arguments
* The Julia file _optimizer.jl_ contains a call to OOESAlgorithm. For example:  
`solutions = OOES("/path/to/Model.lp",pareto_frontier=true,mipsolver=4,threads=8,parallelization=2,timelimit=60.0)`
* The default MIP Solver is set to SCIP since mipsolver=4. To modify other arguments including the path to _Model.lp_ file, edit the optimizer.jl file.
* For details about arguments and possible options, see (https://github.com/alvsierra286/OOESAlgorithm.jl) and (http://eng.usf.edu/~amsierra/documents/Documentation_OOESAlg.pdf). 
  

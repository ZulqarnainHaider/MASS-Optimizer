#! /bin/sh
make clean
make
./HOM_LPCreator Input_File.txt
julia -p8 optimizer.jl

using CSV
using DataFrames
using OOESAlgorithm
df_output_1  =  CSV.read("./optimizer_output_file1.txt")
solutions = OOES("./Model.lp",pareto_frontier=true,mipsolver=4,threads=8,parallelization=3,timelimit=600.0)
function myfn1!(df_output_1, solutions) 
       for i in 1:length(solutions) 
            soln = DataFrame(Sol = solutions[i].vars)
			names!(soln, [Symbol("Sol_$i")])
            solnwq=soln[1:size(df_output_1)[1],:]
            df_output_1 = hcat(df_output_1, solnwq, makeunique=true)
       end
       return df_output_1
end
df_output_1_1 = myfn1!(df_output_1, solutions)
CSV.write("./optimizer_output_file1.txt", df_output_1_1; delim=' ')

df_output_2  = DataFrame([String, Float64, Float64], length(solutions))
rename!(df_output_2, :x1 => :"NDP")
rename!(df_output_2, :x2 => :"ZC (M\$/year)")
rename!(df_output_2, :x3 => :"ZE (ton CO2-eq/year)")
for i in 1:length(solutions) 
    df_output_2[!, 1][i]= "Sol_$i"
	df_output_2[!, 2][i]= (solutions[i].obj_vals[2]) #1,000,000
	df_output_2[!, 3][i]= (solutions[i].obj_vals[3]) #907185
end
CSV.write("./optimizer_output_file2.txt", df_output_2; delim=' ')
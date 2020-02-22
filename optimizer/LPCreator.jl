using JuMP, MathOptFormat
using Printf
using DataFrames
using DelimitedFiles
using CSV
#Old Parameters#

data = CSV.read("Input_File.txt", header=0)

size_r = Int(data[1][1]);
size_k = Int(data[2][1]);
size_t = Int(data[3][1]);

mu = data[1][2];
GTW = data[2][2];
GTR = data[3][2];
MG = data[4][2];
CTW = data[5][2];
CTR = data[6][2];
alfa = data[7][2];
n = data[8][2];
#New Parameters#
CPA = 0.0;
CPB = 0.0;
#Old Parameters#

P_r = Array{Float64}(undef, size_r)
for r = 1:size_r
    P_r[r] = data[r][3] ;
end

Cap_kt = Array{Float64}(undef, size_k, size_t)
for k = 1:size_k
    for t = 1:size_t
        if k==1
            Cap_kt[k,t] = data[t][4];
        else
            Cap_kt[k,t] = Cap_kt[1,t];
        end
    end
end

CC_kt = Array{Float64}(undef, size_k, size_t)
for k = 1:size_k
    for t = 1:size_t
        if k==1
            CC_kt[k,t] = data[t][5];
        else
            CC_kt[k,t] = CC_kt[1,t];
        end
    end
end

OC_t = Array{Float64}(undef, size_t)
for t = 1:size_t
    OC_t[t] = data[t][6];
end

SRW_t = Array{Float64}(undef, size_t)
for t = 1:size_t
    SRW_t[t] = data[t][7];
end

GP_t = Array{Float64}(undef, size_t)
for t = 1:size_t
    GP_t[t] = data[t][8];
end

L_rk = Array{Float64}(undef, size_r, size_k)
for r = 1:size_r
    for k = 1:size_k
        L_rk[r,k] = data[k][r+8];
    end
end

#Define Variables#
m = Model() # Creating an empty Model
@variable(m,w_kt[k=1:size_k, t=1:size_t], Bin);
@variable(m,q_rk[r=1:size_r, k=1:size_k], Bin);
@variable(m,AWT_rk[r=1:size_r, k=1:size_k], Int);
@variable(m,ART_rk[r=1:size_r, k=1:size_k], Int);
@variable(m,z_rkt[r=1:size_r, k=1:size_k, t=1:size_t], Bin);

###########1###############
@expression(m, const1, sum(w_kt[k,t]*Cap_kt[k,t] for k=1:size_k, t=1:size_t) + sum(GTW*AWT_rk[r,k]*L_rk[r,k] for r=1:size_r, k=1:size_k) + sum(GTR*ART_rk[r,k]*L_rk[r,k] for r=1:size_r, k=1:size_k))
@constraint(m, const1 <= MG);
###########4###############
@expression(m, expr4[k=1:size_k], sum(mu*P_r[r]*q_rk[r,k] for r=1:size_r) - sum(Cap_kt[k,t]*w_kt[k,t] for t=1:size_t))
@constraint(m, [k=1:size_k], expr4[k] <= 0);
###########6###############
@constraint(m,[r=1:size_r, k=1:size_k], mu*P_r[r]*q_rk[r,k] <= AWT_rk[r,k]);
###########7###############
@constraint(m,[r=1:size_r, k=1:size_k], alfa*mu*P_r[r]*q_rk[r,k] <= ART_rk[r,k]);
###########2###############
@expression(m, expr2[k=1:size_k], sum(w_kt[k,t] for t=1:size_t))
@constraint(m,[r=1:size_r, k=1:size_k], expr2[k] >= q_rk[r,k]);
###########3###############
@expression(m, expr3[r=1:size_r], sum(q_rk[r,k] for k=1:size_k))
@constraint(m,[r=1:size_r], expr3[r] == 1);
###########5###############
@expression(m, expr5[k=1:size_k], sum(w_kt[k,t] for t=1:size_t))
@constraint(m,[k=1:size_k], expr5[k] <= 1);
###########McCormick Constraints###############
@constraint(m,[r=1:size_r,k=1:size_k,t=1:size_t], z_rkt[r,k,t] <= w_kt[k,t]);
@constraint(m,[r=1:size_r,k=1:size_k,t=1:size_t], z_rkt[r,k,t] <= q_rk[r,k]);
@constraint(m,[r=1:size_r,k=1:size_k,t=1:size_t], z_rkt[r,k,t] >= w_kt[k,t] + q_rk[r,k] -1);
###########OF1###############
@expression(m, exprOF1, sum(CC_kt[k,t]*(mu/n)*P_r[r]*z_rkt[r,k,t] for r=1:size_r,k=1:size_k,t=1:size_t)
 + sum(OC_t[t]*mu*P_r[r]*q_rk[r,k] for r=1:size_r,k=1:size_k,t=1:size_t)
 - sum(SRW_t[t]*alfa*mu*P_r[r]*q_rk[r,k] for r=1:size_r,k=1:size_k,t=1:size_t)
 + sum(CTW*AWT_rk[r,k]*L_rk[r,k] for r=1:size_r,k=1:size_k)
 + sum(CTR*ART_rk[r,k]*L_rk[r,k] for r=1:size_r,k=1:size_k))
 @constraint(m, exprOF1 == 0);
 ###########OF2###############
 @expression(m, exprOF2, sum(GP_t[t]*mu*P_r[r]*z_rkt[r,k,t] for r=1:size_r,k=1:size_k,t=1:size_t)
  + sum(GTW*AWT_rk[r,k]*L_rk[r,k] for r=1:size_r,k=1:size_k)
  + sum(GTR*ART_rk[r,k]*L_rk[r,k] for r=1:size_r,k=1:size_k))
  @constraint(m, exprOF2 == 0);
 ###########OF3###############
 @constraint(m, w_kt[1,1] == 0);

 ###########Objective Function###############
 @expression(m, exprObj, sum(w_kt[k,t] for k=1:size_k,t=1:size_t)
  + sum(q_rk[r,k] for r=1:size_r,k=1:size_k)
  + sum(AWT_rk[r,k] for r=1:size_r,k=1:size_k)
  + sum(ART_rk[r,k] for r=1:size_r,k=1:size_k)
  + sum(z_rkt[r,k,t] for r=1:size_r,k=1:size_k,t=1:size_t))

@objective(m, Min, exprObj)

lp_model = MathOptFormat.LP.Model()
MOI.copy_to(lp_model, backend(m))
MOI.write_to_file(lp_model, "Model.lp")

io = open("optimizer_output_file1.txt", "w");
write(io, "Var_Name\t", "Var\t", "r\t", "k\t", "t\n" );
for k = 1:size_k
    for t = 1:size_t
        println(io, "w(",k,")(",t,")\t","w\t","-\t",k,"\t",t)
    end
end
for r = 1:size_r
    for k = 1:size_k
        println(io, "q(",r,")(",k,")\t","q\t",r,"\t",k,"\t","-\t")
    end
end
close(io);

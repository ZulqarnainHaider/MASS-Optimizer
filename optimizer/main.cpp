#include <cstdlib>
#include <ilcplex/ilocplex.h>   /*This is a package for using CPLEX in C++ */
#include <iostream>             /*This is a package for reading/writing from/in screen in C++ */
#include<fstream>              /*This is a package for reading/writing files C++ */

#define Positive_Infinity 10000000000
#define Negative_Infinity -10000000000
#define RelativeGap 1e-6
#define NumThreads 1

/*Old Parameters*/
double mu;
double GTW;
double GTR;
double MG;
double CTW;
double CTR;
double alfa;
double n;

/*New Parameters*/
double CPA;
double CPB;

/*Old Parameters*/
double** Cap_kt;
double** CC_kt;
double* OC_t;
double* SRW_t;
double* GP_t;
double** L_rk;
double* P_r;
int Set_r;
int Set_k;
int Set_t;

/*Functions to read/write Input/Output files*/
std::ifstream InputFile;
std::ofstream OutputFile;

/*CPLEX Variables Declaration*/
ILOSTLBEGIN
typedef IloArray<IloNumVarArray> NumVar2DArray;
typedef IloArray<NumVar2DArray> NumVar3DArray;
IloEnv env;
IloModel model(env);
IloCplex cplex(model);
IloObjective Objective_Function;
IloExpr Expr(env);
NumVar3DArray z_rkt;
NumVar2DArray w_kt;
NumVar2DArray q_rk;
NumVar2DArray AWT_rk;
NumVar2DArray ART_rk;


using namespace std;

void Reading_The_Input_File(char *Input_File) {
    InputFile.open(Input_File);

    InputFile >>Set_r;
    InputFile >>Set_k;
    InputFile >>Set_t;

    InputFile >> mu;
    InputFile >> GTW;
    InputFile >> GTR;
    InputFile >> MG;
    InputFile >> CTW;
    InputFile >> CTR;
    InputFile >> alfa;
    InputFile >> n;

    /*New Parameters*/
    //InputFile >> CPA;
    //InputFile >> CPB;

    P_r = new double[Set_r];
    for (int r = 0; r < Set_r; r++) {
        InputFile >> P_r[r];
    }

    Cap_kt = new double* [Set_k];
    for (int k = 0; k < Set_k; k++) {
        Cap_kt[k] = new double [Set_t];
    }
    for (int k = 0; k < Set_k; k++) {
        for (int t = 0; t < Set_t; t++) {
            if (k == 0) {
                InputFile >> Cap_kt[k][t];
            } else {
                Cap_kt[k][t] = Cap_kt[0][t];
            }
        }
    }

    CC_kt = new double* [Set_k];
    for (int k = 0; k < Set_k; k++) {
        CC_kt[k] = new double [Set_t];
    }
    for (int k = 0; k < Set_k; k++) {
        for (int t = 0; t < Set_t; t++) {
            if (k == 0) {
                InputFile >> CC_kt[k][t];
            } else {
                CC_kt[k][t] = CC_kt[0][t];
            }
        }
    }

    OC_t = new double[Set_t];
    for (int t = 0; t < Set_t; t++) {
        InputFile >> OC_t[t];
    }

    SRW_t = new double[Set_t];
    for (int t = 0; t < Set_t; t++) {
        InputFile >> SRW_t[t];
    }

    GP_t = new double[Set_t];
    for (int t = 0; t < Set_t; t++) {
        InputFile >> GP_t[t];
    }

    L_rk = new double* [Set_r];
    for (int r = 0; r < Set_r; r++) {
        L_rk[r] = new double [Set_k];
    }
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            InputFile >> L_rk[r][k];
        }
    }
}

void Model_Variables() {
    /* The following will be used to make the name of variables compatible with our model */
    char VarName[25];

    /*Creating w_kt */
    w_kt = NumVar2DArray(env, Set_k);
    for (int k = 0; k < Set_k; k++) {
        w_kt[k] = IloNumVarArray(env, Set_t);
    }
    for (int k = 0; k < Set_k; k++) {
        for (int t = 0; t < Set_t; t++) {
            sprintf(VarName, "w(%d)(%d)", k + 1, t + 1);
            w_kt[k][t] = IloNumVar(env, 0.0, 1, ILOBOOL, VarName);
        }
    }
    /*Creating q_rk */
    q_rk = NumVar2DArray(env, Set_r);
    for (int r = 0; r < Set_r; r++) {
        q_rk[r] = IloNumVarArray(env, Set_k);
    }
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            sprintf(VarName, "q(%d)(%d)", r + 1, k + 1);
            q_rk[r][k] = IloNumVar(env, 0.0, 1, ILOBOOL, VarName);
        }
    }
    /*Creating AWT_rk */
    AWT_rk = NumVar2DArray(env, Set_r);
    for (int r = 0; r < Set_r; r++) {
        AWT_rk[r] = IloNumVarArray(env, Set_k);
    }
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            sprintf(VarName, "AWT(%d)(%d)", r + 1, k + 1);
            AWT_rk[r][k] = IloNumVar(env, 0.0, Positive_Infinity, ILOINT, VarName);
        }
    }
    /*Creating ART_rk */
    ART_rk = NumVar2DArray(env, Set_r);
    for (int r = 0; r < Set_r; r++) {
        ART_rk[r] = IloNumVarArray(env, Set_k);
    }
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            sprintf(VarName, "ART(%d)(%d)", r + 1, k + 1);
            ART_rk[r][k] = IloNumVar(env, 0.0, Positive_Infinity, ILOINT, VarName);
        }
    }

    /*Creating z_rkt (McCormick_Relaxation)*/
    z_rkt = NumVar3DArray(env, Set_r);
    for (int r = 0; r < Set_r; r++) {
        z_rkt[r] = NumVar2DArray(env, Set_k);
        for (int k = 0; k < Set_k; k++) {
            z_rkt[r][k] = IloNumVarArray(env, Set_t);
        }
    }
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            for (int t = 0; t < Set_t; t++) {
                sprintf(VarName, "z(%d)(%d)(%d)", r + 1, k + 1, t + 1);
                z_rkt[r][k][t] = IloNumVar(env, 0.0, 1, ILOBOOL, VarName);
            }
        }
    }
}

void Model_Constraints() {
    
		
    /*5*/
    for (int k = 0; k < Set_k; k++) {
        Expr.clear();
        for (int t = 0; t < Set_t; t++) {
            Expr += w_kt[k][t];
        }
        model.add(Expr <= 1);
    }
    
    /*3*/
    for (int r = 0; r < Set_r; r++) {
        Expr.clear();
        for (int k = 0; k < Set_k; k++) {
            Expr += q_rk[r][k];
        }
        model.add(Expr == 1);
    }

	/*4*/
    for (int k = 0; k < Set_k; k++) {
        Expr.clear();
        for (int t = 0; t < Set_t; t++) {
            Expr -= (Cap_kt[k][t] * w_kt[k][t]);
        }
		for (int r = 0; r < Set_r; r++) {
            Expr += (mu * P_r[r] * q_rk[r][k]);
        }
        model.add(Expr <= 0);
    }
	
	/*1*/
    Expr.clear();
    for (int k = 0; k < Set_k; k++) {
        for (int t = 0; t < Set_t; t++) {
            Expr += (w_kt[k][t] * Cap_kt[k][t]);
        }
    }
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            Expr += (GTW * AWT_rk[r][k] * L_rk[r][k]);
        }
    }
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            Expr += (GTR * ART_rk[r][k] * L_rk[r][k]);
        }
    }
    model.add(Expr <= MG);

    /*6*/
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            model.add((mu * P_r[r] * q_rk[r][k]) <= AWT_rk[r][k]);
        }
    }

    /*7*/
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            model.add((alfa * mu * P_r[r] * q_rk[r][k]) <= ART_rk[r][k]);
        }
    }

    /*2*/
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            Expr.clear();
            for (int t = 0; t < Set_t; t++) {
                Expr += w_kt[k][t];
            }
            model.add(Expr >= q_rk[r][k]);
        }
    }


    /*  Expr.clear();
        for (int r = 0; r < Set_r; r++) {
            Expr += (AWT_rk[r][0]);
        }
        model.add(Expr <= 16580103.6);
        Expr.clear();
        for (int r = 0; r < Set_r; r++) {
            Expr += (AWT_rk[r][2]);
        }
        model.add(Expr <= 13816753.0);*/
}

void McCormick_Envelope_Constraints() {
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            for (int t = 0; t < Set_t; t++) {
                model.add(z_rkt[r][k][t] <= w_kt[k][t]);
                model.add(z_rkt[r][k][t] <= q_rk[r][k]);
                model.add(z_rkt[r][k][t] >= w_kt[k][t] + q_rk[r][k] - 1);
            }
        }
    }
}

void Objective_Functions() {
    Expr.clear();
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            for (int t = 0; t < Set_t; t++) {
                Expr += (CC_kt[k][t] * (mu / n) * P_r[r] * z_rkt[r][k][t]);
            }
        }
    }
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            for (int t = 0; t < Set_t; t++) {
                Expr += (OC_t[t] * mu * P_r[r] * q_rk[r][k]);
            }
        }
    }
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            for (int t = 0; t < Set_t; t++) {
                Expr -= (SRW_t[t] * alfa * mu * P_r[r] * q_rk[r][k]);
            }
        }
    }
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            Expr += (CTW * AWT_rk[r][k] * L_rk[r][k]);
        }
    }
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            Expr += (CTR * ART_rk[r][k] * L_rk[r][k]);
        }
    }
    model.add(Expr == 0);

    Expr.clear();
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            for (int t = 0; t < Set_t; t++) {
                Expr += (GP_t[t] * mu * P_r[r] * z_rkt[r][k][t]);
            }
        }
    }
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            Expr += (GTW * AWT_rk[r][k] * L_rk[r][k]);
        }
    }
    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            Expr += (GTR * ART_rk[r][k] * L_rk[r][k]);
        }
    }
    model.add(Expr == 0);

    Expr.clear();
    Expr += w_kt[0][0];
    model.add(Expr == 0);
    /*export the variable names onto a file*/
    Expr.clear();
	ofstream myfile1;
	myfile1.open ("optimizer_output_file1.txt");
	myfile1 << "Var_Name" << '\t' << "Var" << '\t' << "r" << '\t' << "k" << '\t' << "t" << '\n';
    for (int k = 0; k < Set_k; k++) {
        for (int t = 0; t < Set_t; t++) {
            Expr += w_kt[k][t];
			myfile1 << "w("<< k <<")(" << t <<")" << '\t' << "w" << '\t' << "-" << '\t' << k << '\t' << t << '\n';
        }
    }

    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            Expr += q_rk[r][k];
			myfile1 << "q("<< r <<")(" << k <<")" << '\t' << "q" << '\t' << r << '\t' << k << '\t' << "-" << '\n';
        }
    }

    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            Expr += AWT_rk[r][k];
        }
    }

    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            Expr += ART_rk[r][k];
        }
    }

    for (int r = 0; r < Set_r; r++) {
        for (int k = 0; k < Set_k; k++) {
            for (int t = 0; t < Set_t; t++) {
                Expr += z_rkt[r][k][t];
            }
        }
    }
    Objective_Function = IloMinimize(env, Expr);
    model.add(Objective_Function);
	myfile1.close();
}

int main(int argc, char** argv) {
    Reading_The_Input_File(argv[1]);
    Model_Variables();
    Model_Constraints();
    McCormick_Envelope_Constraints();
    Objective_Functions();
    cplex.exportModel("Model.lp");
    return 0;
}


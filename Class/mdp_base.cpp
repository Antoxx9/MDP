#include <utility> 
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <string.h>
#include <queue>
#include <map>
#include <cmath>
#include <sstream>
#include <time.h>
#include <set>

#define ff first
#define ss second
#define INF 1000000000

using namespace std;


class mdp_base {

public:

	vector< vector<double> > matrix;
	vector<int> search_space;
	vector<int> partial_sol;
	vector<int> best_sol;
	int rows, m_set;
	double best_value;

	mdp_base(){};


	// Method to calculate the euclidean distance between 2 vectors.
	double euclidean_distance(vector< double > &si, vector< double > &sj){
		double sum = 0;
		for(int i = 0; i < (int)si.size(); i++){
			sum += ((si[i] - sj[i])*(si[i] - sj[i]));
		}
		sum = sqrt(sum);
		return sum;
	}

	// Method to add an element si to a set without m elements.
	double add_set_i(vector<int> &set, int index){
		double resp = 0;
		for(int i = 0; i < (int)set.size(); i++){
			if(set[i] == 1){
				resp += euclidean_distance(matrix[i], matrix[index]);
			}
		}
		return resp;
	}

	// Method to calculate the cost of exchanging 2 elements in the current solution set.
	double exchange(vector<int> &set, int new_s, int old_s){
		double resp = 0;
		for(int i = 0; i < (int)set.size(); i++){
			if(set[i] == 1 && i != old_s){
				resp += (euclidean_distance(matrix[i], matrix[new_s]) - euclidean_distance(matrix[i], matrix[old_s]));
			}
		}
		return resp;
	}

	// Method to calculate the distance between a si and a set X
	double distance_X(vector<double> &si, vector<int> &X){
		double distance = 0;
		for(int j = 0; j < X.size(); j++){
			if(X[j] == 1){
				distance += euclidean_distance(si, matrix[j]);
			}
		}
		return distance;
	}

	// Destructive Method D2 to find an initial solution.
	vector<int> destructive_2(){
		vector<int> initial_sol = search_space;
		int selected = rows;
		double min_dist, aux;
		int min_index;
		while(selected > m_set){
			min_dist = INF;
			min_index = -1;
			for(int i = 0; i < (int)initial_sol.size(); i++){
				if(initial_sol[i] == 1){
					aux = distance_X(matrix[i], initial_sol);
				}
				else{
					aux = INF;
				}
				if(aux < min_dist){
					min_index = i;
					min_dist = aux;
				}
			}
			initial_sol[min_index] = 0;
			selected--;
		}
		return initial_sol;
	}

	// Method to get a random initial solution.
	vector<int> random_sol(){
		vector<int> initial_sol;
		initial_sol.resize(rows);
		int size = 0;
		int random;
		srand(time(NULL));
		while(size < m_set){
			random = rand()%rows;
			if(initial_sol[random] == 0){
				size++;
				initial_sol[random] = 1;
				search_space[random] = 0;
			}
		}
		return initial_sol;
	}

	// Method to calculate the value of a solution
	double sol_value(vector<int> & sol){
		double value = 0;
		for(int i = 0; i < rows-1; i++){
			if(sol[i] == 1){
				for(int j = i+1; j < rows; j++){
					if(sol[j] == 1){
						value += euclidean_distance(matrix[i], matrix[j]);
						printf("Value: %f\n",value);
					}
				}
			}
		}
		return value;
	}

	void local_search_first_best(){
		printf("Local search\n");
		partial_sol.resize(rows);
		best_sol.resize(rows);
		double new_value; 
		int no_changes = 0;
		int size = 0;
		best_sol = random_sol();
		for(int i = 0; i < rows; i++){
			if(best_sol[i] == 1){
				printf("%d ",i);
			}
		}
		printf("\n");
		best_value = sol_value(best_sol);
		printf("First value = %f\n",best_value);
		while(no_changes < 500){
			new_value = 0;
			for(int i = 0; i < rows; i++){
				if(best_sol[i] == 0){
					for(int j = 0; j!= i && j < rows; j++){
						if(best_sol[j] == 1){
							new_value = best_value + exchange(best_sol, i, j);
							if(new_value > best_value){
								//printf("Change\n");
								best_sol[j] = 0;
								best_sol[i] = 1;
								best_value = new_value;
								no_changes = 0;
								i = rows+1;
								break;
							}
						}
					}
				}
			}
			no_changes++;
		}
	}

	// Method to read a state and kepp it on the matrix, recieves the path of the txt file
	void read_state(string path){
		ifstream i_file;
		int lines;
		int x,y;
		double val;

		i_file.open(path.c_str());
		string linea;
		
		i_file >> rows;
		i_file >> m_set;
		search_space.resize(rows);

		matrix.resize(rows);
		for(int i = 0; i < rows; i++){
			lines += i;
			matrix[i].resize(rows);
			search_space[i] = 1;
		}
		for(int i = 0; i < lines; i++){
			i_file >> x;
			i_file >> y;
			i_file >> val;
			matrix[x][y] = val;
			matrix[y][x] = val;
			matrix[x][x] = 0;
		}
		i_file.close();
	}

	// Print the matrix 
	void print_matrix(){
		for(int i = 0; i < rows; i++){
			for(int j = 0; j < rows; j++){
				printf("%f ", matrix[i][j]);
			}
			printf("\n");
		}
	}

};

int main(){
	mdp_base mdp;

	string path = "../GKD-a/GKD-a_1_n10_m2.txt";
	mdp.read_state(path);
	printf("READED\n");
	vector<int> test;
	mdp.print_matrix();
	mdp.local_search_first_best();
	//for(int i = 0; i < mdp.rows; i++){
	//	if(mdp.best_sol[i] == 1){
	//		printf("%d ",i);
	//	}
	//}
	//printf("\n");
	//printf("Best value = %f\n",mdp.best_value);
	mdp.best_sol = mdp.random_sol();
	mdp.best_value = mdp.sol_value(mdp.best_sol);
	printf("First value = %f\n",mdp.best_value);
}
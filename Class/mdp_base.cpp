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
#include <cstdlib>
#include <sstream>
#include <time.h>
#include <set>
#include <limits.h>

#define ff first
#define ss second
#define INF 1000000000

using namespace std;


class mdp_base {

public:

	vector< vector<double> > distance_matrix;
	vector<int> search_space;
	vector<int> current_sol;
	vector<int> best_sol;
	int rows, m_set, neighborhood_size;
	double current_value, best_value;

	mdp_base(){};

	// Method to read a state and kepp it on the distance_matrix, recieves the path of the txt file
	void read_state(string path){
		ifstream i_file;
		int lines = 0;
		int x,y	;
		double val;

		i_file.open(path.c_str());
		string linea;
		
		i_file >> rows;
		i_file >> m_set;
		neighborhood_size = m_set * (rows - m_set); 
		search_space.resize(rows);

		distance_matrix.resize(rows);
		for(int i = 0; i < rows; i++){
			lines += i;
			distance_matrix[i].resize(rows);
			search_space[i] = 1;
		}
		for(int i = 0; i < lines; i++){
			i_file >> x;
			i_file >> y;
			i_file >> val;
			distance_matrix[x][y] = val;
			distance_matrix[y][x] = val;
			distance_matrix[x][x] = 0;
		}
		i_file.close();
	}

	// Print the distance_matrix 
	void print_matrix(){
		for(int i = 0; i < rows; i++){
			for(int j = 0; j < rows; j++){
				printf("%f ", distance_matrix[i][j]);
			}
			printf("\n");
		}
	}

	// Method to calculate the cost of exchanging 2 elements in the current solution set.
	double exchange(vector<int> &set, int new_s, int old_s){
		double resp = 0;
		for(int i = 0; i < (int)set.size(); i++){
			if(set[i] == 1 && i != old_s){
				resp += (distance_matrix[i][new_s] - distance_matrix[i][old_s]);
			}
		}
		return resp;
	}


	// Method to calculate the distance between a si and a set X
	double distance_X(int si, vector<int> &X){
		double distance = 0;
		for(int j = 0; j < X.size(); j++){
			if(X[j] == 1){
				distance += distance_matrix[si][j];
			}
		}
		return distance;
	}


	// Method to calculate the value of a solution
	double sol_value(vector<int> & sol){
		double value = 0;
		for(int i = 0; i < rows-1; i++){
			if(sol[i] == 1){
				for(int j = i+1; j < rows; j++){
					if(sol[j] == 1){
						value += distance_matrix[i][j];
					}
				}
			}
		}
		return value;
	}

	// Destructive Method D2 to find an initial solution.
	void constructive_2() {
		int selected = 0;
		double max_distance, aux_distance;
		int max_index;

		best_sol = search_space;
		max_distance = INT_MIN;
		max_index = -1;
		for (int i = 0; i < rows; i++) {
			aux_distance = distance_X(i, best_sol);
			if (aux_distance > max_distance) {
				max_distance = aux_distance;
				max_index = i;	
			}
		}
		for (int i = 0; i < rows; i++) {
			best_sol[i] = 0;
		}
		best_sol[max_index] = 1;
		selected++;
		while (selected < m_set) {
			max_distance = INT_MIN;
			max_index = -1;
			for (int i = 0; i < rows; i++) {
				if (best_sol[i] == 0) {
					aux_distance = distance_X(i, best_sol);
				}
				if (aux_distance > max_distance) {
					max_distance = aux_distance;
					max_index = i;
				}
			}
			best_sol[max_index] = 1;
			selected++;
		}
		best_value = sol_value(best_sol);
		current_sol = best_sol;
		current_value = best_value;
	}

	// Destructive Method D2 to find an initial solution.
	void destructive_2(){
		best_sol = search_space;
		int selected = rows;
		double min_dist, aux;
		int min_index;
		while(selected > m_set){
			min_dist = INF;
			min_index = -1;
			for(int i = 0; i < (int)best_sol.size(); i++){
				if(best_sol[i] == 1){
					aux = distance_X(i, best_sol);
				}
				else{
					aux = INF;
				}
				if(aux < min_dist){
					min_index = i;
					min_dist = aux;
				}
			}
			best_sol[min_index] = 0;
			selected--;
		}
		best_value = sol_value(best_sol);
		current_sol = best_sol;
		current_value = best_value;
	}

	void construction_GR_C2(double alpha){
		int selected = 0;
		double max_distance, min_distance, aux_distance;
		vector<int> RCL;
		srand(time(NULL));
		int random;

		best_sol = search_space;
		max_distance = INT_MIN;
		min_distance = INT_MAX;
		for (int i = 0; i < rows; i++) {
			aux_distance = distance_X(i, best_sol);
			if (aux_distance > max_distance) {
				max_distance = aux_distance;
			}
			if (aux_distance < min_distance) {
				min_distance = aux_distance;	
			}
		}

		for (int i = 0; i < rows; i++) {
			aux_distance = distance_X(i, best_sol);
			if (aux_distance >= min_distance + alpha * (max_distance - min_distance)) { 
				RCL.push_back(i);
			}
		}
		random = rand() % RCL.size();


		for (int i = 0; i < rows; i++) {
			best_sol[i] = 0;
		}
		best_sol[RCL[random]] = 1;
		selected++;

		while (selected < m_set) {
			RCL.clear();
			max_distance = INT_MIN;
			min_distance = INT_MAX;
			for (int i = 0; i < rows; i++) {
				if (best_sol[i] == 0) {
					aux_distance = distance_X(i, best_sol);
					if (aux_distance > max_distance) {
						max_distance = aux_distance;
					}
					if (aux_distance < min_distance) {
						min_distance = aux_distance;	
					}
				}
			}	
			for (int i = 0; i < rows; i++) {
				if (best_sol[i] == 0) {
					aux_distance = distance_X(i, best_sol);
					if (aux_distance >= min_distance + alpha * (max_distance - min_distance)) { 
						RCL.push_back(i);
					}
				}
			}
			random = rand() % RCL.size();			
			best_sol[RCL[random]] = 1;
			selected++;
		}
		best_value = sol_value(best_sol);
		current_sol = best_sol;
		current_value = best_value;	
	}


	void construction_GR_D2(double alpha) {
		int selected = rows;
		double max_distance, min_distance, aux_distance;
		vector<int> RCL;
		srand(time(NULL));
		int random;

		best_sol = search_space;
		//cout << "Comenzando" << endl;
		while (selected > m_set) {
			//cout << "En el while" << endl;
			RCL.clear();
			max_distance = INT_MIN;
			min_distance = INT_MAX;
			for (int i = 0; i < rows; i++) {
				if (best_sol[i] == 1) {
					aux_distance = distance_X(i, best_sol);
					//cout << aux_distance <<  endl;
					if (aux_distance > max_distance) {
						max_distance = aux_distance;
					}
					if (aux_distance < min_distance) {
						min_distance = aux_distance;	
					}
				}
			}
			for (int i = 0; i < rows; i++) {
				if (best_sol[i] == 1) { 
					aux_distance = distance_X(i, best_sol);
					if (aux_distance <= min_distance + alpha * (max_distance - min_distance)) { 
						RCL.push_back(i);
					}
				}
			}
			random = rand() % RCL.size();			
			best_sol[RCL[random]] = 0;
			selected--;
		}
		best_value = sol_value(best_sol);
		current_sol = best_sol;
		current_value = best_value;
	}

	// Method to get a random initial solution.
	void random_sol(){
		best_sol.resize(rows);

		int size = 0;
		int random;
		srand(time(NULL));
		while(size < m_set){
			random = rand()%rows;
			if (best_sol[random] == 0) {
				size++;
				best_sol[random] = 1;
				search_space[random] = 0;
			}
		}
		best_value = sol_value(best_sol);
		current_sol = best_sol;
		current_value = best_value;
	}

	bool get_first_best() {
		double new_value = 0;
		for(int i = 0; i < rows; i++){
			if(current_sol[i] == 0){
				for(int j = 0; j < rows; j++){
					if(current_sol[j] == 1){
						new_value = current_value + exchange(current_sol, i, j);
						if (new_value > current_value){
							current_sol[i] = 1;
							current_sol[j] = 0;
							current_value = new_value;
							best_sol = current_sol;
							best_value = current_value;
							return false;
						}
					}
				}
			}
		}
		return true;
	}

	bool best_of_percentage(int percent){
		int neighbors_visited = 0;
		int num_neighbors = ceil((percent / 100.0) * neighborhood_size);
		int best_i = -1, best_j = -1;
		int i, j, random_i, random_j;
		double best_exchange = INT_MIN;
		double aux_exchange;
		srand(time(NULL));
		random_i = rand() % rows;
		i = random_i;
		do {
			if (current_sol[i] == 0) {
				random_j = rand() % rows;
				j = random_j;
				do {
					if (current_sol[j] == 1) {
						aux_exchange = exchange(current_sol, i, j);
						if (aux_exchange > best_exchange) {
							best_i = i;
							best_j = j;
							best_exchange = aux_exchange;
						}
						neighbors_visited++;
					}
					j = (j + 1) % rows;
				} while (j != random_j && neighbors_visited < num_neighbors);
			}
			i = (i + 1) % rows;
		} while (i != random_i && neighbors_visited < num_neighbors);
		if (current_value + best_exchange > best_value) {
			current_sol[best_i] = 1;
			current_sol[best_j] = 0;
			current_value = current_value + best_exchange;
			best_sol = current_sol;
			best_value = current_value;
			return false;
		}
		return true;
	}

	void local_search(int flavor, int percent) {
		bool isLocalMin = false;
		double new_value = 0; 

		while(!isLocalMin){
			if (flavor == 1) {
				isLocalMin = get_first_best();
			}
			else if (flavor == 2) {
				isLocalMin = best_of_percentage(percent);
			}
		}
	}

	void GRASP(int construction, int niter, double alpha_rate, int flavor, int percent) {
		int n_contructions = 0;
		vector<int> star_sol;
		double alpha = 0.5;
		double star_value = INT_MIN;

		while (0.1 <= alpha && alpha <= 0.9) {
			if (construction == 0) {
				construction_GR_C2(alpha);
			}
			else {
				construction_GR_D2(alpha); 
			}
			local_search(flavor, percent);
			if (best_value > star_value) {
				star_sol = best_sol;
				star_value = best_value;
				n_contructions = 0;
			}
			else {
				n_contructions++;
			}
			if (n_contructions == niter && construction == 0) {
				alpha += alpha_rate;
				n_contructions = 0;	
			}
			else if (n_contructions == niter && construction == 1) {
				alpha -= alpha_rate;
				n_contructions = 0;	
			}
		}
		best_sol = star_sol;
		best_value = star_value;
	}
};

int main(int argc, char *argv[]) {
	if (argc < 4 || (((string)argv[3] == "-p") && argc < 5)) {
		cout << "Usage: " << argv[0] << " file_path [-rcd] [-gcgd niter alpha_rate] [-bp percentage]" << endl;
		cout << "Initial solution:" << endl;
		cout << "  -r    Random initial solution." << endl;
		cout << "  -c    Constructive2 method for initial solution." << endl;
		cout << "  -d    Destructive2 method for initial solution." << endl;
		cout << endl;
		cout << "Metaheuristics:" << endl;
		cout << "  -gc   Constructive heuristic GRASP_C2." << endl;
		cout << "  -gd   Destructive heuristic GRASP_D2." << endl;
		cout << "  niter Number of iterations (with no improvement) needed to" << endl; 
		cout << "        increment or decrease alpha value." << endl; 
		cout << "  alpha_rate Rate of increment or decrement of alpha value." << endl;
		cout << endl;
		cout << "Local search:" << endl;	
		cout << "  -b    Best first local search." << endl;
		cout << "  -p    Percentage of neighborhood local search." << endl;
		cout << "  percentage   Percentage of neighbors to visit by local search." << endl; 
		cout << "               If -p option is selected this must be specified." << endl;
		return 0;
	}
	clock_t t = clock();
	mdp_base mdp;
	string path = argv[1];
	mdp.read_state(path);
	bool metah = false;
	//printf("READED\n");
	//mdp.print_matrix(); 
	if ((string)argv[2] == "-r") {
		mdp.random_sol();
	}
	else if ((string)argv[2] == "-c") {
		mdp.constructive_2();
	}
	else if ((string)argv[2] == "-d") {
		mdp.destructive_2();
	}
	else if ((string)argv[2] == "-gc") {
		if ((string)argv[5] == "-b") {
			mdp.GRASP(0, atoi(argv[3]), atof(argv[4]), 1, 0);
		}
		else if ((string)argv[5] == "-p") {
			mdp.GRASP(0, atoi(argv[3]), atof(argv[4]), 2, atoi(argv[6]));
		}
		metah = true;
	}
	else if ((string)argv[2] == "-gd") {
		if ((string)argv[5] == "-b") {
			mdp.GRASP(1, atoi(argv[3]), atof(argv[4]), 1, 0);
		}
		else if ((string)argv[5] == "-p") {
			mdp.GRASP(1, atoi(argv[3]), atof(argv[4]), 2, atoi(argv[6]));
		}
		metah = true;
	}
	if ((string)argv[3] == "-b" && !metah) {
		//cout << "-b" << endl;
		mdp.local_search(1, 0);
	}
	else if ((string)argv[3] == "-p" && !metah) {
		//cout << "-p" << endl;
		mdp.local_search(2, atoi(argv[4]));
	}
	
	t = clock() - t;
	for (int i = 0; i < mdp.rows; i++) {
		if (mdp.best_sol[i] == 1) {
			printf("%d ",i);
		}
	}
	printf("\n");
	printf("%f\n",mdp.best_value);
	printf ("%f\n",((float)t)/CLOCKS_PER_SEC);
}
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
		int x,y;
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
		//cout << "Seleccionado " << selected << ": " << max_index << endl;
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
			//cout << "Seleccionado " << selected << ": " << max_index << endl;
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

	// Method to get a random initial solution.
	void random_sol(){
		best_sol.resize(rows);

		int size = 0;
		int random;
		srand(time(NULL));
		while(size < m_set){
			random = rand()%rows;
			if(best_sol[random] == 0){
				size++;
				best_sol[random] = 1;
				search_space[random] = 0;
			}
		}
		best_value = sol_value(best_sol);
		current_sol = best_sol;
		current_value = best_value;
	}

	bool get_first_best(){
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
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	bool best_for_percentage(int percent){
		int neighbors_visited = 0;
		int num_neighbors = ceil((percent / 100.0) * neighborhood_size);
		int best_exchange = INT_MIN;
		int best_i = -1, best_j = -1;
		int aux_exchange;
		for (int i = 0; i < rows && neighbors_visited < num_neighbors; i++) {
			if (current_sol[i] == 0) {
				for (int j = 0; j < rows && neighbors_visited < num_neighbors; j++) {
					if (current_sol[j] == 1) {
						aux_exchange = exchange(current_sol, i, j);
						if (aux_exchange > best_exchange) {
							best_i = i;
							best_j = j;
							best_exchange = aux_exchange;
						}
						neighbors_visited++;
					}
				}
			}
		}
		if (current_value + best_exchange > best_value) {
			current_sol[best_i] = 1;
			current_sol[best_j] = 0;
			current_value = current_value + best_exchange;
			best_sol = current_sol;
			best_value = current_value;
			return true;
		}
		return false;
	}

	void local_search(int flavor){
		bool stop = false;
		double new_value = 0; 

		for(int i = 0; i < rows; i++){
			if(best_sol[i] == 1){
				printf("%d ",i);
			}
		}
		printf("\n");
		printf("First value = %f\n",best_value);

		while(!stop){
			if (flavor == 1) {
				stop = !get_first_best();
			}
			else if (flavor == 2) {
				stop = !best_for_percentage(100);
			}
		}
	}
};

int main(int argc, char *argv[]) {
	if (argc < 4) {
		cout << "Usage: " << argv[0] << " file_path -rcd -bp" << endl;
		cout << "Options:" << endl;
		cout << "  -r    Random initial solution." << endl;
		cout << "  -c    Constructive2 method for initial solution." << endl;
		cout << "  -d    Destructive2 method for initial solution." << endl;
		cout << endl;
		cout << "  -b    Best first local search." << endl;
		cout << "  -p    Percentage of neighborhood local search." << endl;
		return 0;
	}
	mdp_base mdp;
	cout << argv[1];
	string path = argv[1];
	mdp.read_state(path);
	printf("READED\n");
	//mdp.print_matrix();
	//cout << "Argumento: " << argv[2] << endl;
	//cout << "Comparacion: " << ((string)argv[2] == "-r") << endl;
	if (string(argv[2]) == "-r") {
		//cout << "Random" << endl;
		mdp.random_sol();
	}
	else if (string(argv[2]) == "-c") {
		mdp.constructive_2();
	}
	else if (string(argv[2]) == "-d") {
		mdp.destructive_2();
	}

	if (string(argv[3]) == "-b") {
		mdp.local_search(1);
	}
	else if (string(argv[3]) == "-p") {
		mdp.local_search(2);
	}
	//mdp.random_sol();
	//mdp.local_search(1);
	for (int i = 0; i < mdp.rows; i++) {
		if (mdp.best_sol[i] == 1) {
			printf("%d ",i);
		}
	}
	printf("\n");
	printf("Best value = %f\n",mdp.best_value);
}
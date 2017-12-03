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
	vector<int> freq;
	vector<double> quality;
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
		freq.resize(rows);
		quality.resize(rows);

		for(int i = 0; i < rows; i++){
			lines += i;
			distance_matrix[i].resize(rows);
			search_space[i] = 1;
			freq[i] = 0;
			quality[i] = 0.0;
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

	// Method to calculate the distance between a trial solution x and a set of solutions ref_set.
	int distance_sol(vector<int> &trial, vector<int> &ref_set, vector< vector<int> > &poblacion){
		int count = 0;
		for (int i = 0; i < rows; i++) {
			if (trial[i] == 1) {
				for (int j = 0; j < (int)ref_set.size(); j++) {
					if (ref_set[j] == 1) {
						count += poblacion[j][i];
					}
				}
			}
		}
		return -count;
	}


	// Method to calculate the value of a solution
	double sol_value(vector<int> &sol){
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

	// Constructive Method D2 to find an initial solution.
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
		srand(time(NULL) + rand());
		int random;

		best_sol = search_space;
		while (selected > m_set) {
			RCL.clear();
			max_distance = INT_MIN;
			min_distance = INT_MAX;
			for (int i = 0; i < rows; i++) {
				if (best_sol[i] == 1) {
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

	void construction_TABU_C2(int niter, double beta, double delta, int flavor, int percent) {
		queue<vector<int> > best_sols;
		int queue_size = 0;
		int n = 0, max_freq;
		double max_q;
		int selected = 0;
		double max_distance, aux_distance, min_distance, max_distance_prime;
		int max_index;
		double star_value = 0;
		vector<int> star_sol;
		while (n < niter) {
			best_sol = search_space;
			selected = 0;
			max_freq = 1;
			max_q = 1;
			max_distance = INT_MIN;
			min_distance = INT_MAX;
			max_distance_prime = INT_MIN;
			max_index = -1;

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
				if (freq[i] > max_freq) {
					max_freq = freq[i];
				}
				if (quality[i] > max_q) {
					max_q = quality[i];	
				}
			}
			for (int i = 0; i < rows; i++) {
				aux_distance = distance_X(i, best_sol);
				aux_distance -= beta * (max_distance - min_distance) * (freq[i] / max_freq);
				aux_distance += delta * (max_distance - min_distance) * (quality[i] / max_q);
				if (aux_distance > max_distance_prime) {
					max_distance_prime = aux_distance;
					max_index = i;
				}
			}
			for (int i = 0; i < rows; i++) {
				best_sol[i] = 0;
			}
			best_sol[max_index] = 1;
			freq[max_index] += 1;
			selected++;
			max_freq = 0;
			while (selected < m_set) {
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
					if (freq[i] > max_freq) {
						max_freq = freq[i];
					}
					if (quality[i] > max_q) {
						max_q = quality[i];	
					}
				}

				max_distance_prime = INT_MIN;
				max_index = -1;
				for (int i = 0; i < rows; i++) {
					if (best_sol[i] == 0) {
						aux_distance = distance_X(i, best_sol);
						aux_distance -= beta * (max_distance - min_distance) * (freq[i] / max_freq);
						aux_distance += delta * (max_distance - min_distance) * (quality[i] / max_q);
						if (aux_distance > max_distance_prime) {
							max_distance_prime = aux_distance;
							max_index = i;
						}
					}
				}
				best_sol[max_index] = 1;
				freq[max_index] += 1;
				selected++;
			}
			best_value = sol_value(best_sol);
			if (best_value > star_value) {
				star_value = best_value;
				star_sol = best_sol;
			}
			for(int i = 0; i < rows; i++){
				if (best_sol[i] == 1){
					quality[i] = (quality[i] * (freq[i] - 1) + best_value) / freq[i];
				}
			}
			current_sol = best_sol;
			current_value = best_value;
			local_search(flavor, percent);
			if (best_value > star_value) {
				star_value = best_value;
				star_sol = best_sol;
			}
			n++;
		}
		best_value = star_value;
		best_sol = star_sol;
		current_sol = best_sol;
		current_value = best_value;
	}

	void construction_TABU_D2(int niter, double beta, double delta, int flavor, int percent) {
		queue<vector<int> > best_sols;
		int queue_size = 0;
		int n = 0, max_freq;
		double max_q;
		int selected = m_set;
		double max_distance, aux_distance, min_distance, min_distance_prime;
		int min_index;
		double star_value = 0;
		vector<int> star_sol;
		while (n < niter) {
			selected = rows;
			max_freq = 1;
			max_q = 1;
			best_sol = search_space;
			max_distance = INT_MIN;
			min_distance = INT_MAX;
			min_index = -1;
			while (selected > m_set) {
				max_distance = INT_MIN;
				min_distance = INT_MAX;
				for (int i = 0; i < rows; i++) {
					if (best_sol[i] == 1) {
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
					if (freq[i] > max_freq) {
						max_freq = freq[i];
					}
					if (quality[i] > max_q) {
						max_q = quality[i];	
					}
				}
				min_distance_prime = INT_MAX;
				min_index = -1;
				for (int i = 0; i < rows; i++) {
					if (best_sol[i] == 1) {
						aux_distance = distance_X(i, best_sol);
						aux_distance -= beta * (max_distance - min_distance) *(freq[i] / max_freq);
						aux_distance += delta * (max_distance - min_distance) *(quality[i] / max_q);
						if (aux_distance < min_distance_prime) {
							min_distance_prime = aux_distance;
							min_index = i;
						}
					}
				}
				if(min_index != -1){
					best_sol[min_index] = 0;
					freq[min_index] += 1;
					selected--;
				}
			}
			best_value = sol_value(best_sol);
			if(best_value > star_value){
				star_value = best_value;
				star_sol = best_sol;
			}
			for(int i = 0; i < rows; i++){
				if(best_sol[i] == 0){
					quality[i] = quality[i]*(freq[i]-1)+best_value;
					quality[i] = quality[i]/freq[i];
				}
			}
			current_sol = best_sol;
			current_value = best_value;
			local_search(flavor, percent);
			if (best_value > star_value) {
				star_value = best_value;
				star_sol = best_sol;
			}
			max_freq = 0;
			max_q = 0;
			n++;
		}
		best_value = star_value;
		best_sol = star_sol;
		current_sol = best_sol;
		current_value = best_value;	
	}

	void combination_D2(vector<int> &sol_1, vector<int> &sol_2){
		int selected = 0, min_index;
		double min_dist, aux;
		vector<int> union_sol;

		for (int i = 0; i < rows; i++) {
			if (sol_1[i] == 1 || sol_2[i] == 1) {
				union_sol.push_back(1);
				selected++;
			}
			else {
				union_sol.push_back(0);	
			}
		}

		for (int j = 0; j < rows; j++) {
			if (union_sol[j] == 1) {
				printf("%d ",j);
			}
		}
		cout << endl;

		while(selected > m_set){
			min_dist = INT_MAX;
			min_index = -1;
			for(int i = 0; i < rows; i++){
				if (union_sol[i] == 1) {
					aux = distance_X(i, union_sol);
				}
				else {
					aux = INT_MAX; 
				}
				if(aux < min_dist) {
					min_index = i;
					min_dist = aux;
				}
			}
			union_sol[min_index] = 0;
			selected--;
		}
		for (int j = 0; j < rows; j++) {
			if (union_sol[j] == 1) {
				printf("%d ",j);
			}
		}
		cout << " ------ " << sol_value(union_sol) << endl;
		best_sol = union_sol;
		best_value = sol_value(union_sol);
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
				cout << "Nueva solucion" << endl;
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

	void TABU(int construction, int niter, double beta_rate, double delta_rate, int flavor, int percent) {
		int n_contructions = 0;
		vector<int> star_sol;
		double alpha = 0.5;
		double star_value = INT_MIN;
		queue<vector<int> > solutions;
		srand(time(NULL));

		if (construction == 0) {
			construction_TABU_C2(niter, beta_rate, delta_rate, flavor, percent);
		}
		else {
			construction_TABU_D2(niter, beta_rate, delta_rate, flavor, percent); 
		}
	}

	void Scatter_Search(int n, int b, int q){
		vector< vector<int> > poblacion, combination_set;
		vector<int> ref_set, aux_indexes;
		vector<double> aux_values;
		int ref_set_members, quality_members, min_sol_index, max_sol_index, updates = -1;; 
		double min_sol_value, max_sol_value, aux_value;

		for (int i = 0; i < n; i++) {
			construction_GR_D2(0.5);
			local_search(1,0);
			poblacion.push_back(best_sol);
		} 
		cout << "--------- Mejorado -----------------" << endl;	
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < rows; j++) {
				if (poblacion[i][j] == 1) {
					printf("%d ",j);
				}
			} 
			cout << " -------- " << sol_value(poblacion[i]) << endl;
		}

		cout << "Tamanio poblacion: " << poblacion.size() << endl;

		// Seleccion de q% de b miembros del conjunto de referencia por calidad. 
		ref_set.resize(n);
		for(int i = 0; i < n; i++){
			ref_set[i] = 0;
		}	
		//ref_set_members = ceil(n * (b / 100.0));
		quality_members = ceil(b * (q / 100.0));
		//cout << "Tamanio ref_set: " << ref_set_members << endl;
		cout << "Tamanio ref_set: " << b << endl;
		cout << "Calidad:" << quality_members << endl;
		aux_indexes.resize(quality_members);
		aux_values.resize(quality_members);
		min_sol_index = 0;
		min_sol_value = INT_MAX;
		for(int i = 0; i < quality_members; i++){
			ref_set[i] = 1;
			aux_indexes[i] = i;
			aux_values[i] = sol_value(poblacion[i]);
			if (aux_values[i] < min_sol_value) {
				min_sol_value = aux_values[i];
				min_sol_index = i;
			}
		}
		cout << "Min index: " << min_sol_index << endl;
		cout << "Min value: " << min_sol_value << endl;
		for (int i = quality_members; i < n; i++) {
			aux_value = sol_value(poblacion[i]);
			if (aux_value > min_sol_value) {
				cout << "i: " << i << endl;
				cout << "Min index: " << min_sol_index << endl;
				ref_set[aux_indexes[min_sol_index]] = 0;
				ref_set[i] = 1;
				aux_indexes[min_sol_index] = i;
				aux_values[min_sol_index] = aux_value;
				min_sol_value = INT_MAX;
				cout << "Calculando minino ---------" << endl;
				for (int j = 0; j < quality_members; j++) {
					cout << "Valor auxiliar " << aux_values[j] << endl;
					if (aux_values[j] < min_sol_value) {
						min_sol_value = aux_values[j];
						min_sol_index = j;
					}
				}
				cout << min_sol_value << endl;
				cout << min_sol_index << endl;
				cout << "Nuevo min index: " << min_sol_index << endl;	
			}
		}
		
		for (int k = 0; k < n; k++) {
			if (ref_set[k] == 1) {
				cout << k << " ";
			}
		}
		cout << endl;

		// Seleccion de b-quality_members miembros del conjunto de referencia por diversidad.
		while (quality_members < b) {
			cout << "While" << endl;
			cout << "miembros: " << quality_members << endl;
			max_sol_value = INT_MIN;
			max_sol_index = -1;
			for (int i = 0; i < n; i++) {
				if (ref_set[i] == 0) {
					cout << "Considerando solucion: " << i << endl; 
					aux_value = distance_sol(poblacion[i], ref_set, poblacion);
					cout << "Distancia: " << aux_value << endl;
					if (aux_value > max_sol_value) {
						max_sol_value = aux_value;
						max_sol_index = i;
					}
				}
			}
			cout << "Solucion mas diversa: " << max_sol_index << endl;
			ref_set[max_sol_index] = 1;
			aux_indexes.push_back(max_sol_index);
			aux_values.push_back(sol_value(poblacion[max_sol_index]));
			quality_members++;
		}

		for (int i = 0; i < quality_members; i++) {
			cout << aux_indexes[i] << " " << aux_values[i] << endl;
		}
		
		for (int k = 0; k < n; k++) {
			if (ref_set[k] == 1) {
				cout << k << " ";
			}
		}
		cout << endl;
		while(updates != 0) {
			updates = 0;
			for (int i = 0; i < n; i++) {
				if (ref_set[i] == 1) {
					for (int j = i + 1; j < n; j++) {
						if (ref_set[j] == 1) {
							//Combinacion
							cout << "Combinacion " << i << " " << j << endl;
							combination_D2(poblacion[i],poblacion[j]);
							//Mejoramiento
							local_search(1,0);
							for (int k = 0; k < rows; k++) {
								if (best_sol[k] == 1) {
									printf("%d ",k);
								}
							}
							cout << " -------- " << sol_value(best_sol) << endl;
							combination_set.push_back(best_sol);
						}
					}
				}
			}
			//Actualizacion
			cout << "--------- Comenzando actulizacion ---------------" << endl;
			for (int k = 0; k < (int)combination_set.size(); k++) {
				min_sol_value = INT_MAX;
				min_sol_index = -1;
				for (int l = 0; l < b; l++){
					if (aux_values[l] < min_sol_value) {
						min_sol_value = aux_values[l];
						min_sol_index = l;
					}
				}
				cout << "Minimo: " << aux_indexes[min_sol_index] << " --- " << min_sol_value << endl; 
				aux_value = sol_value(combination_set[k]);
				if (aux_value > min_sol_value) {
					aux_values[min_sol_index] = aux_value;
					poblacion[aux_indexes[min_sol_index]] = combination_set[k];
					updates++;
				}
			}

			cout << "--------- poblacion actualizada ---------------" << endl;
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < rows; j++) {
					if (poblacion[i][j] == 1) {
						printf("%d ",j);
					}
				} 
				cout << " -------- " << sol_value(poblacion[i]) << endl;
			}

			cout << "---------- updates ------------- " << updates << endl;
		}

		for (int k = 0; k < b; k++) {
			max_sol_value = INT_MIN;
			max_sol_index = -1;
			if (aux_values[k] > max_sol_value) {
				max_sol_value = aux_values[k];
				max_sol_index = k;
			}
		}

		best_sol = poblacion[aux_indexes[max_sol_index]];
		best_value = max_sol_value;		
	}
};
 
int main(int argc, char *argv[]) {
	if (argc < 4 || (((string)argv[3] == "-p") && argc < 5)) {
		cout << "Usage: " << argv[0] << " file_path [-rcd] [-gcgd niter alpha_rate] [-tctd niter beta_weight delta_weight] [-bp percentage]" << endl;
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
		cout << "  -tc   Constructive heuristic TABU_C2." << endl;
		cout << "  -td   Destructive heuristic TABU_D2." << endl;
		cout << "  niter Number of iterations needed to make tabu constructions." << endl; 
		cout << "  beta_weight Weight to give to the frequency of an element in the solution." << endl;
		cout << "  delta_weight Weight to give to the quality of an element in the solution" << endl;
		cout << endl;
		cout << "  -ss   Scatter search." << endl;
		cout << "  n 	 Size of the initial population." << endl; 
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
	else if ((string)argv[2] == "-tc") {
		if ((string)argv[6] == "-b") {
			mdp.TABU(0, atoi(argv[3]), atof(argv[4]), atof(argv[5]), 1, 0);
		}
		else if ((string)argv[6] == "-p") {
			mdp.TABU(0, atoi(argv[3]), atof(argv[4]), atof(argv[5]), 2, atoi(argv[7]));
		}
		metah = true;
	}
	else if ((string)argv[2] == "-td") {
		if ((string)argv[6] == "-b") {
			mdp.TABU(1, atoi(argv[3]), atof(argv[4]), atof(argv[5]), 1, 0);
		}
		else if ((string)argv[6] == "-p") {
			mdp.TABU(1, atoi(argv[3]), atof(argv[4]), atof(argv[5]), 2, atoi(argv[7]));
		}
		metah = true;
	}
	else if ((string)argv[2] == "-ss") {
		mdp.Scatter_Search(atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));
		metah = true;
	}
	if ((string)argv[3] == "-b" && !metah) {
		mdp.local_search(1, 0);
	}
	else if ((string)argv[3] == "-p" && !metah) {
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
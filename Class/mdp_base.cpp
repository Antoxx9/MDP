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

using namespace std;


class mdp_base {

public:

	vector< vector<double> > matrix;
	//set< vector< double > > search_space;
	vector<int> partial_sol;
	vector<int> best_sol;
	int rows, m_set;

	mdp_base(){};


	// Method to get de center of a set
	double center_s(vector<int> &set){
		double resp = 0;
		int size = 0;
		for(int i = 0; i < (int)set.size(); i++){
			if(set[i] == 1){
				for(int j = 0; j < (int)matrix[i].size(); j++){
					resp += matrix[i][j];
				}
				size++;
			}
		}
		return resp/size;
	}

	// Method to calculate the euclidean distance between 2 vectors.
	double euclidean_distance(vector< double > &si, vector< double > &sj){
		double sum = 0;
		int x1,y1,x2,y2;
		for(int i = 0; i < (int)si.size(); i++){
			sum += pow((si[i] - sj[i]),2);
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
	double exchange(vector<int> &set, int new, int old){
		double resp = 0;
		for(int i = 0; i < (int)set.size(); i++){
			if(set[i] == 1 && i != old){
				resp += (euclidean_distance(matrix[i], matrix[new]) - euclidean_distance(matrix[i], matrix[old]));
			}
		}
		return resp;
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

		matrix.resize(rows);
		for(int i = 0; i < rows; i++){
			lines += i;
			matrix[i].resize(rows);
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

	string path = "../GKD-a/GKD-a_10_n10_m3.txt";
	mdp.read_state(path);
	mdp.print_matrix();
}
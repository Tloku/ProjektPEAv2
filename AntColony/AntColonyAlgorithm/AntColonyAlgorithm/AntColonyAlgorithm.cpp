#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm> 
#include <fstream>
#include <ctime>
#include <stdlib.h> //abs func
#include <sstream>

std::vector<int> randomPathPermutation();
int calculateDistance(std::vector<int> path);
void loadINIFile();
void saveToCsv(std::string txtFileName, int iterations, int optimalSolution, std::vector<std::string> optimalPath, std::vector<int>* pathResult,
	double* timer, int* solution, double* error);
void loadFromFile(std::string fileName);
double calculateRelativeError(int, int);
double getRandomDoubleValue();
double calculateRelativeError(int result, int optimalSolution);

std::vector<std::vector<int>> cities;
std::vector<std::vector<int>> pheromoneMatrix;
std::vector<int> tabuList;
std::vector<int> path;
double* timer;
int* solution;
std::clock_t start;
int N;
int M; //Number of ants
float alpha, beta, p, evaporation; // p contains in <0, 1>
float visibility; // visibility is reciprocal of the distance between two cities


int main()
{
}


std::vector<int> randomPathPermutation()
{
	std::vector<int> tempToSwap(N);
	int iter = 0;
	for (auto i = 0; i < N; i++) {
		tempToSwap[i] = i;
	}
	std::random_shuffle(tempToSwap.begin(), tempToSwap.end());
	return tempToSwap;
}

int calculateDistance(std::vector<int> path)
{
	int cost = 0;
	for (auto i = 0; i < path.size() - 1; i++) {
		cost += cities[path[i]][path[i + 1]];
	}
	cost += cities[path[N - 1]][path[0]];
	return cost;
}

double getRandomDoubleValue()
{
	return ((double)rand() / RAND_MAX + 1.0);
}

double calculateRelativeError(int result, int optimalSolution)
{
	return ((abs((double)optimalSolution - (double)result) / (double)optimalSolution) * 100);
}

void loadINIFile()
{
	std::fstream file;
	std::string fileName = "Badania.INI";
	int number_of_nodes = 0;
	int weight;
	std::clock_t* counter;
	std::string txtFileName;
	double* error;
	int iterations;
	int optimalSolution;
	std::vector<std::string> optimalPath;
	std::vector<int>* pathResult;
	std::string tmpVector = "";

	file.open(fileName, std::ios::in);

	if (file.good())
	{
		std::string line;
		while (getline(file, line))
		{
			std::istringstream iss(line);

			iss >> txtFileName;
			iss >> iterations;
			iss >> optimalSolution;
			while (iss >> tmpVector) {
				if (tmpVector == "[" || tmpVector == "]")
					continue;

				optimalPath.push_back(tmpVector);
			}
			txtFileName = "instancje/" + txtFileName;

			timer = new double[iterations];
			counter = new clock_t[iterations];
			solution = new int[iterations];
			pathResult = new std::vector<int>[iterations];
			error = new double[iterations];

			for (int k = 0; k < iterations; k++) {
				timer[k] = 0;
				solution[k] = 0;
				error[k] = 0;
			}


			for (int i = 0; i < iterations; i++) {
				loadFromFile(txtFileName);
				std::cout << "Plik: " << txtFileName << "\n";
				std::cout << "Optymalny wynik: " << optimalSolution << "\n";
				std::cout << "Optymalna sciezka: ";
				for (auto a : optimalPath)
					std::cout << a << " ";
				std::cout << "\n";

				counter[i] = std::clock();
				//solution[i] = simulatedAnnealing(cities, pow(N, 2), coolingRate, 3 * N, N * 2, optimalSolution);
				timer[i] = (std::clock() - counter[i]) / (double)CLOCKS_PER_SEC;
				//pathResult[i] = bestPath;
				//error[i] = calculateRelativeError(solution[i], optimalSolution);
				std::cout << "Wynik: " << solution[i] << "\n";
				std::cout << "Czas wykonania: " << timer[i] << "\n";
				std::cout << "Blad: " << error[i] << " [%]" << std::endl;
				std::cout << "\n\n";
			}
			saveToCsv(txtFileName, iterations, optimalSolution, optimalPath, pathResult, timer, solution, error);
			optimalPath.clear();
		}
		delete[]solution;
		delete[]timer;
	}
}

void saveToCsv(std::string txtFileName, int iterations, int optimalSolution, std::vector<std::string> optimalPath, std::vector<int>* pathResult,
	double* timer, int* solution, double* error)
{
	std::fstream file;
	std::string fileName = "test_out_AC.csv";

	file.open(fileName, std::ios::out | std::ios::app);

	if (file.good()) {
		file << txtFileName << ";";
		file << iterations << ";";
		file << optimalSolution << ";";
		for (auto a : optimalPath)
			file << a << " ";
		file << ";\n";

		for (int i = 0; i < iterations; i++) {
			file << std::to_string(timer[i]) << ";" << solution[i] << ";";
			file << std::to_string(error[i]) << ";";
			file << "[";
			for (auto j : pathResult[i])
				file << j << " ";
			file << "];\n";
		}

	}
}

void loadFromFile(std::string fileName)
{
	std::fstream file;
	int weight;
	std::vector<std::vector<int>> tmpVec;
	file.open(fileName, std::ios::in);
	if (file.good() == false) {
		std::cout << "Nie udalo sie otworzyc pliku!\n";
	}
	else {
		file >> N;
		path.resize(N);
		for (int i = 0; i < N; i++) {
			tmpVec.push_back(std::vector<int>());
			for (int j = 0; j < N; j++) {
				file >> weight;
				if (i != j) {
					tmpVec[i].push_back(weight);
				}
				else if (i == j) {
					tmpVec[i].push_back(INT_MAX);
				}
			}
		}
	}
	cities = tmpVec;
	file.close();
}

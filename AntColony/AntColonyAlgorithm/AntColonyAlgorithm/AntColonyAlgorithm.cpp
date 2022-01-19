#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm> 
#include <fstream>
#include <ctime>
#include <stdlib.h> //abs func
#include <sstream>
#include "Ant.h"
#include <random>

void loadINIFile();
void saveToCsv(std::string txtFileName, int iterations, int optimalSolution, std::vector<std::string> optimalPath, std::vector<int>* pathResult,
	double* timer, int* solution, double* error);
void loadFromFile(std::string fileName);
double calculateRelativeError(int, int);
void initAntsVector();
void initProbabilityVector();
void initPheromoneMatrix();
int chooseCity(int antIndex);
void calculateProbabilityOfChoosingCity(int antIndex);
float getRandomFloat(float a, float b);
void runAllAnts();
void runOneAnt(int antIndex);
void placePheromoneDAS(int fromCity, int toCity);
void placePheromoneCAS();
void reducePheromone();
int calculateApproximatePathCost();
void setPheroValue();
int startAntColonyAlgorithm(int iterations);
std::vector<int> randomPathPermutation();
int calculateDistance(std::vector<int> path);

std::vector<std::vector<int>> cities;
std::vector<std::vector<double>> pheromoneMatrix;

std::vector<int> path;
std::vector<Ant> antsVector;
int bestCost = INT_MAX;
std::vector<int> bestPath;
double pheroValue;
const double RANDOM_PR_VALUE = 0.3;
double* timer;
int* solution;
std::clock_t start;
int N;
int M; //Number of ants
std::vector<double> probability;
double alpha = 1;
double beta = 3;
const double qDASConstant = 0.55;
const double evaporation = 0.5;
const double Qcycl = 100;

int main()
{
	srand(time(NULL));
	loadINIFile();
}

int startAntColonyAlgorithm(int iterations) {
	for (int i = 0; i < iterations; i++) {
		M = N;
		initAntsVector();
		initProbabilityVector();
		setPheroValue();
		initPheromoneMatrix();
		runAllAnts();
	}

	return bestCost;
}

void initProbabilityVector() {
	probability.resize(N, 0.0001);
}

void initAntsVector()
{
	antsVector.resize(N);
	for (int i = 0; i < antsVector.size(); i++) {
		Ant tmpAnt = Ant(N, i);
		antsVector[i] = tmpAnt;
	}
}

void setPheroValue() {
	pheroValue = (float)M / calculateApproximatePathCost();
}

int calculateApproximatePathCost() {
	int iterations = 100;
	long totalCost = 0;
	for (int i = 0; i < iterations; i++) {
		std::vector<int> path = randomPathPermutation();
		totalCost += calculateDistance(path);
	}

	return totalCost / iterations;
}

void initPheromoneMatrix()
{
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			pheromoneMatrix[i][j] = pheroValue;
		}
	}
}

void runAllAnts() {
	for (int i = 0; i < antsVector.size(); i++) {
		runOneAnt(i);
	}
	placePheromoneCAS();
	for (int i = 0; i < antsVector.size(); i++) {
		antsVector[i].clear();
	}
}

void runOneAnt(int antIndex) {
	reducePheromone();
	int currentCity;
	while (!antsVector[antIndex].checkIfAllCitiesAreVisited()) {
		currentCity = antsVector[antIndex].getCurrentCity();
		int nextCity = chooseCity(antIndex);
		antsVector[antIndex].moveAnt(nextCity);
		//placePheromoneDAS(currentCity, nextCity);
		antsVector[antIndex].calculateTotalDistance(cities[currentCity][nextCity]);
	}

	currentCity = antsVector[antIndex].getCurrentCity();
	int initialCity = antsVector[antIndex].getInitialCity();
	antsVector[antIndex].moveAnt(initialCity);
	//placePheromoneDAS(currentCity, initialCity);
	antsVector[antIndex].calculateTotalDistance(cities[currentCity][initialCity]);

	if (bestCost > antsVector[antIndex].getTotalDistance()) {
		bestCost = antsVector[antIndex].getTotalDistance();
		bestPath = antsVector[antIndex].getAntPath();
	}
}

void placePheromoneDAS(int fromCity, int toCity) {
	pheromoneMatrix[fromCity][toCity] += qDASConstant;
}

void placePheromoneCAS() {
	int cost = 0;
	int min = INT_MAX;
	std::vector<int> greedyBestPath;

	for (int i = 0; i < N; i++) {
		cost = antsVector[i].getTotalDistance();
		
		if (cost < min) {
			min = cost;
			greedyBestPath = antsVector[i].getAntPath();
		}

		for (int j = 0; j < greedyBestPath.size() - 1; j++) {
			pheromoneMatrix[greedyBestPath[j]][greedyBestPath[j + 1]] += (double)Qcycl / cost;
		}
	}
}

void reducePheromone() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			pheromoneMatrix[i][j] *= (1 - evaporation);
		}
	}
}

int chooseCity(int antIndex) {
	int currentCity = antsVector[antIndex].getCurrentCity();
	if (getRandomFloat(0.01, 0.5) < RANDOM_PR_VALUE) {

		int maxU = -1;
		double maxV = -1;

		for (int i = 0; i < N; i++) {
			if (antsVector[antIndex].checkIfCityIsVisited(i)) {
				continue;
			}
			double tmp = pow(pheromoneMatrix[currentCity][i], alpha) * pow(1.0 / cities[currentCity][i], beta);
			if (tmp > maxV) {
				maxV = tmp;
				maxU = i;
			}
		}
		return maxU;
	}

	calculateProbabilityOfChoosingCity(antIndex);
	float tmp = getRandomFloat(0.45, 1.0);
	while (true) {
		int index = rand() % N;
		float probabilities = probability[index];

		if (probabilities >= tmp && !antsVector[antIndex].checkIfCityIsVisited(index)) {
			return index;
		}

		if (probabilities != 0.0) {
			probability[index] += 0.2;
		}
	}
}

void calculateProbabilityOfChoosingCity(int antIndex) {
	int currentCity = antsVector[antIndex].getCurrentCity();
	double nominator = 0.0;
	double denominator = 0.0;
	for (int i = 0; i < N; i++) {
		if (antsVector[antIndex].getCurrentCity() == i) continue;
		if (antsVector[antIndex].checkIfCityIsVisited(i)) continue;

		double tau = (double) pow(pheromoneMatrix[currentCity][i], alpha);
		double eta;
		if (cities[currentCity][i] != 0) {
			 eta = (double)pow(1.0 / cities[currentCity][i], beta);
		} 
		else {
			 eta = (double)pow(1.0 / 0.1, beta);
		}
		denominator += tau * eta;
	}

	for (int i = 0; i < N; i++) {
		if (antsVector[antIndex].checkIfCityIsVisited(i)) {
			probability[i] = 0.0;
		}
		else if(cities[currentCity][i] != 0) {
			nominator = pow(pheromoneMatrix[currentCity][i], alpha) * pow(1.0 / cities[currentCity][i], beta);
			probability[i] = nominator / denominator;
		}
		else if (cities[currentCity][i] == 0) {
			nominator = pow(pheromoneMatrix[currentCity][i], alpha) * pow(1.0 / 0.001, beta);
			probability[i] = nominator / denominator;
		}
	}
}

float getRandomFloat(float a, float b)
{
	static std::default_random_engine e;
	static std::uniform_real_distribution<> dis(a, b); 
	return dis(e);
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
				bestPath.clear();
				bestCost = INT_MAX;

				counter[i] = std::clock();
				solution[i] = startAntColonyAlgorithm(100);
				timer[i] = (std::clock() - counter[i]) / (double)CLOCKS_PER_SEC;
				pathResult[i] = bestPath;
				error[i] = calculateRelativeError(solution[i], optimalSolution);
				std::cout << "Znaleziona droga: ";
				for (auto a : bestPath)
					std::cout << a << " ";
				std::cout << "\n";
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
	std::string fileName = "test_out_AC_alpha_" + std::to_string(alpha) + "_beta_" + std::to_string(beta) + "_evap_"+std::to_string(evaporation) + "_CAS.csv";

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
	std::vector<std::vector<double>> tmpPheromoneMatrix;
	file.open(fileName, std::ios::in);
	if (file.good() == false) {
		std::cout << "Nie udalo sie otworzyc pliku!\n";
	}
	else {
		file >> N;
		path.resize(N);
		for (int i = 0; i < N; i++) {
			tmpVec.push_back(std::vector<int>());
			tmpPheromoneMatrix.push_back(std::vector<double>());
			for (int j = 0; j < N; j++) {
				file >> weight;
				tmpPheromoneMatrix[i].push_back(0);
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
	pheromoneMatrix = tmpPheromoneMatrix;
	file.close();
}

double calculateRelativeError(int result, int optimalSolution)
{
	return ((abs((double)optimalSolution - (double)result) / (double)optimalSolution) * 100);
}
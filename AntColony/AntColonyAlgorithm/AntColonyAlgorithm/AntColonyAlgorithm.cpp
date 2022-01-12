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
void initPheromoneMatirx();
int chooseCity(int antIndex);
void calculateProbabilityOfChoosingCity(int antIndex);
float getRandomFloat(float a, float b);
void runAllAnts();
void runOneAnt(int antIndex);
void placePheromoneDAS(int fromCity, int toCity);
void reducePheromone();
void startAntColonyAlgorithm();

std::vector<std::vector<int>> cities;
std::vector<std::vector<double>> pheromoneMatrix;
std::vector<float> probabilty;
std::vector<int> path;
std::vector<Ant> antsVector;
int bestCost = INT_MAX;
std::vector<int> bestPath;
const float INITIAL_PHERO_VALUE = 0.5;
const float RANDOM_PR_VALUE = 0.15;
double* timer;
int* solution;
std::clock_t start;
int N;
int M; //Number of ants
float alpha, beta, p; // p contains in <0, 1>
float visibility; // visibility is reciprocal of the distance between two cities
const float qDASConstant = 0.2;
const float evaporation = 0.1;

int main()
{
	loadINIFile();
	

}

void startAntColonyAlgorithm() {

}

int chooseCity(int antIndex) {
	if (getRandomFloat(0.01, 0.5) < RANDOM_PR_VALUE) {
		int randomCity = rand() % N;
		int index = -1; 
		for (int i = 0; i < N; i++) {
			if (!antsVector[antIndex].checkIfCityIsVisited(i)) {
				index++;
			}
			if (index == randomCity) {
				return index;
			}
		}
	}

	calculateProbabilityOfChoosingCity(antIndex);

	float tmp = getRandomFloat(0.45, 1.0);
	while (true) {
		int index = rand() % N;
		float probabilities = probabilty[index];

		if (probabilities >= tmp) {
			return probabilty[index];
		}

		if (probabilities != 0.0) {
			probabilty[index] += 0.2;
			probabilities = probabilty[index];
		}
	}
}

void calculateProbabilityOfChoosingCity(int antIndex) {
	int currentCity = antsVector[antIndex].getCurrentCity();
	float nominator = 0.0;
	float denominator = 0.0;
	for (int i = 0; i < N; i++) {
		if (!antsVector[antIndex].checkIfCityIsVisited(i)) {
			denominator += (float)pow(pheromoneMatrix[currentCity][i], alpha) * pow((float)1.0 / cities[currentCity][i], beta);
		}
	}

	for (int i = 0; i < N; i++) {
		if (antsVector[antIndex].checkIfCityIsVisited(i)) {
			probabilty[i] = 0.0;
		}
		else {
			nominator = (float)pow(pheromoneMatrix[currentCity][i], alpha) * pow((float)1.0 / cities[currentCity][i], beta);
			probabilty[i] = nominator / denominator;
		}
	}
}

float getRandomFloat(float a, float b)
{
	static std::default_random_engine e;
	static std::uniform_real_distribution<> dis(a, b); 
	return dis(e);
}

void runAllAnts() {
	for (int i = 0; i < antsVector.size(); i++) {
		runOneAnt(i);
	}
}

void runOneAnt(int antIndex) {
	reducePheromone();
	int currentCity;
	while (!antsVector[antIndex].checkIfAllCitiesAreVisited()) {
		currentCity = antsVector[antIndex].getCurrentCity();
		int nextCity = chooseCity(antIndex); 
		antsVector[antIndex].moveAnt(nextCity);
		placePheromoneDAS(currentCity, nextCity);
		antsVector[antIndex].calculateTotalDistance(cities[currentCity][nextCity]);
	}

	currentCity = antsVector[antIndex].getCurrentCity();
	int initialCity = antsVector[antIndex].getInitialCity();
	antsVector[antIndex].moveAnt(initialCity);
	placePheromoneDAS(currentCity, initialCity);	
	antsVector[antIndex].calculateTotalDistance(cities[currentCity][initialCity]);

	if (bestCost > antsVector[antIndex].getTotalDistance()) {
		bestCost = antsVector[antIndex].getTotalDistance();
		bestPath = antsVector[antIndex].getAntPath();
	}
}

void placePheromoneDAS(int fromCity, int toCity) {
	pheromoneMatrix[fromCity][toCity] += qDASConstant;
}

void reducePheromone() {
	for (int i = 0; i < pheromoneMatrix.size(); i++) {
		for (int j = 0; j < pheromoneMatrix.size(); j++) {
			pheromoneMatrix[i][j] *= (1 - evaporation);
		}
	}
}

void initPheromoneMatirx()
{
	for (int i = 0; i < pheromoneMatrix.size(); i++) {
		for (int j = 0; j < pheromoneMatrix.size(); j++) {
			pheromoneMatrix[i][j] = INITIAL_PHERO_VALUE;
		}
	}
}

void initAntsVector()
{
	antsVector.resize(N);
	int i = 0;
	for (Ant ant : antsVector) {
		Ant tmpAnt = Ant(N, i++);
		ant = tmpAnt;
	}
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

double calculateRelativeError(int result, int optimalSolution)
{
	return ((abs((double)optimalSolution - (double)result) / (double)optimalSolution) * 100);
}
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm> 
#include <fstream>
#include <ctime>
#include <stdlib.h> //abs func
#include <sstream>

void loadFromFile(std::string fileName);
void loadINIFile();
void saveToCsv(std::string, int, int, std::vector<std::string>, std::vector<int>*, double*, int*, double*);
double calculateRelativeError(int, int);
std::vector<int> randomPathPermutation();
int calculateDistance(std::vector<int>);
void swapCities(int);
std::vector<int> findBestNeighbour();
bool checkIfMoveIsInTabu(int, int);
int tabuSearch(int);

int firstCity, secondCity;
int citiesChanged[2];
std::vector<std::vector<int>> cities;
std::vector<int> tabuList;
std::vector<int> path;
std::vector<int> bestPath;
double* timer;
int* solution;
std::clock_t start;
int N;
int tabuListSize;

int main()
{
	srand(time(NULL));
	loadINIFile();
}


int tabuSearch(int optimalSolution)
{
	path.resize(N);
	int iterations = 0;
	int constCriterium = 15000;
	long stopCriterium = constCriterium*N;
	path = randomPathPermutation();
	int bestPathDistance = calculateDistance(path);
	bestPath = path;
	double timeToLive = 2 * N;
	double timeWorking = 0.0;
	double timeToFind = 0.0;
	int maxError = 5;

	if (N >= 30 && N < 50) {
		maxError = 10;
	}
	else if (N >= 50 && N < 75) {
		maxError = 15;
	}
	else if (N >= 75 && N < 100) {
		maxError = 20;
	}
	else if (N >= 100 && N < 150) {
		maxError = 30;
	}
	else if (N >= 150) {
		maxError = 50;
	}

	start = std::clock();
	while (stopCriterium) {
		stopCriterium--;
		iterations++;

		if (calculateRelativeError(bestPathDistance, optimalSolution) > maxError && iterations > N*50){
			path = randomPathPermutation();
			constCriterium -= 10;
			iterations = 0;
		}

		path = findBestNeighbour();
		int currentPathDistance = calculateDistance(path);
		int distanceDifference = bestPathDistance - currentPathDistance;

		if (distanceDifference > 0) {
			bestPath = path;
			bestPathDistance = currentPathDistance;
			timeToFind = (std::clock() - start) / (double)CLOCKS_PER_SEC;
			stopCriterium = constCriterium * N;
		}
		else {
			tabuList.push_back(citiesChanged[0]);
			tabuList.push_back(citiesChanged[1]);
			tabuList.push_back(INT_MAX); //INT_MAX s³u¿y jako przerwa pomiêdzy ruchami zawartymi w tablicy
			tabuListSize += 3;
		}

		if (tabuListSize == 75) { //prawdziwy maksymalny rozmiar tabuList to zawsze 2/3*tabuListSize
			swapCities(N);
			tabuList.clear();
			tabuListSize = 0;
		}

		timeWorking = (std::clock() - start) / (double)CLOCKS_PER_SEC;

		if (timeWorking > timeToLive || calculateRelativeError(bestPathDistance, optimalSolution) <= maxError) {
			std::cout << "Wynik: " << bestPathDistance << std::endl;
			std::cout << "Droga: ";
			for (auto i : bestPath) {
				std::cout << i << " ";
			}
			std::cout << std::endl;
			std::cout << "Czas szukania najlepszego wyniku: " << timeToFind << " [s]" << std::endl;
			return bestPathDistance;
		}
	}
	return bestPathDistance;
}

std::vector<int> findBestNeighbour()
{
	std::vector<int> optimalPath(path);
	bool checkIfMoveIsInTabuList = false;

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			std::vector<int> changedPath(optimalPath);
			do {
				firstCity = rand() % N;
				secondCity = rand() % N;
			} while (firstCity == secondCity);
			
			std::swap(changedPath[firstCity], changedPath[secondCity]);
			checkIfMoveIsInTabuList = checkIfMoveIsInTabu(firstCity, secondCity);

			if (!checkIfMoveIsInTabuList && calculateDistance(changedPath) < calculateDistance(optimalPath)) {
				optimalPath = changedPath;
				citiesChanged[0] = firstCity;
				citiesChanged[1] = secondCity;
			}
		}
	}
	return optimalPath;
}

bool checkIfMoveIsInTabu(int from, int to)
{
	for (int i = 0; i < tabuListSize-1; i++) {
		if (tabuList[i] == from && tabuList[i + 1] == to)
			return true;
	}
	return false;
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

void swapCities(int size)
{
	do {
		firstCity = rand() % size;
		secondCity = rand() % size;
	} while (firstCity == secondCity);

	std::swap(path[firstCity], path[secondCity]);
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
				solution[i] = tabuSearch(optimalSolution);
				timer[i] = (std::clock() - counter[i]) / (double)CLOCKS_PER_SEC;
				pathResult[i] = bestPath;
				error[i] = calculateRelativeError(solution[i], optimalSolution);
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
	std::string fileName = "test_out_TS_tabuList_25.csv";

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

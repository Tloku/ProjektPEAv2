#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm> 
#include <fstream>
#include <ctime>
#include <stdlib.h> //abs func
#include <sstream>

<<<<<<< HEAD
int simulatedAnnealing(std::vector<std::vector<int>>, float , float , long long, double);
=======
int simulatedAnnealing(std::vector<std::vector<int>>, float , float , int , double, int);
>>>>>>> 134c1e249fc63d0b307adefba5760ac677dc889e
void swapCities(int);
void invertCities(int);
void insertCity(int);
int calculateDistance(std::vector<int>);
std::vector<int> randomPathPermutation();
double getProbability(int, double);
void loadFromFile(std::string fileName);
void loadINIFile();
void saveToCsv(std::string, int, int, std::vector<std::string>, std::vector<int>*, double*, int*, double*);
double getRandomDoubleValue();
double calculateRelativeError(int, int);
double calculateTemperatureBoltzmann(double temperature);
double calculateTemperatureCauchy(double temperature);
double calculateTemperatureGeometric(double temperature);

int firstCity, secondCity;
std::vector<std::vector<int>> cities;
std::vector<int> path;
double* timer;
int* solution;
std::clock_t start;
int N;
std::vector<int> bestPath;
const float coolingRate = 0.005;

int main()
{
    srand(time(NULL));
	loadINIFile();
	//loadFromFile("instancje/si175.tsp");
}


int simulatedAnnealing(std::vector<std::vector<int>> cities, float initialTemperature, float coolingRate, int stopCriterium, double timeToLive, int optimalSolution)
{
    double temperature = initialTemperature;
	double temperatureWhenFound = 0.0;
    double time = 0.0;
    std::vector<int> permutatedPath = randomPathPermutation();
	double timeToFind = 0.0;
	double timeWorking = 0.0;
	std::clock_t counter;
    int result = INT_MAX;
    int currentDistance;
	counter = std::clock();
	long iterations = 0;

	while (temperature > 1.0) {
		path = permutatedPath;
		currentDistance = calculateDistance(path);
		iterations++;

		if (iterations > N*50 && calculateRelativeError(result, optimalSolution) > 15) {
			iterations = 0;
			temperature = initialTemperature / 1.1;
			path = randomPathPermutation();
		}

		for (int i = stopCriterium; i > 0; i--) {
			//swapCities(N);
			//invertCities(N);
			insertCity(N);

			currentDistance = calculateDistance(path);
			int distanceDifference = result - currentDistance;

			if (distanceDifference > 0) {
				result = currentDistance;
				bestPath = path;
				timeToFind = (std::clock() - counter) / (double)CLOCKS_PER_SEC;
				temperatureWhenFound = temperature;
			}

			if (distanceDifference > 0 || (distanceDifference < 0 && getProbability(distanceDifference, temperature) > getRandomDoubleValue())) {
				permutatedPath = path;
				break;
			}
			else {
				//std::swap(path[firstCity], path[secondCity]);
				//std::reverse(path.begin() + firstCity, path.begin() + secondCity);

				if (firstCity > secondCity) {
					path.insert(path.begin() + firstCity, path[secondCity - 1]);
					path.erase(path.begin() + secondCity);
				}
				else {
					path.insert(path.begin() + secondCity, path[firstCity - 1]);
					path.erase(path.begin() + (firstCity - 1));
				}

			}
			timeWorking = (std::clock() - counter) / (double)CLOCKS_PER_SEC;

			if (timeWorking > timeToLive || (temperature > 1.0 && temperature < 2.0)) {
				std::cout << "Wynik: " << result << std::endl;
				std::cout << "Droga: ";
				for (auto i : bestPath) {
					std::cout << i << " ";
				}
				std::cout << std::endl;
				std::cout << "Czas szukania najlepszego wyniku: " << timeToFind << " [s]" << std::endl;
				std::cout << "Temperatura poczatkowa: " << initialTemperature << std::endl;
				std::cout << "Wartosc temperatury, gdy znaleziono najlepszy wynik: " << temperatureWhenFound << std::endl;
				std::cout << "Temperatura koncowa: " << temperature << std::endl;
				return result;
			}
		}
		temperature = calculateTemperatureGeometric(temperature);
	}
}

double calculateTemperatureBoltzmann(double temperature)
{
	return -temperature / log(0.99);
}

double calculateTemperatureCauchy(double temperature)
{
	return temperature / 0.99;
}

double calculateTemperatureGeometric(double temperature)
{
	return temperature *= (1 - coolingRate);
}

double calculateRelativeError(int result, int optimalSolution)
{
	return ((abs((double)optimalSolution - (double)result)/ (double)optimalSolution)*100);
}

double getRandomDoubleValue()
{
	return ((double)rand() / RAND_MAX + 1.0);
}

std::vector<int> randomPathPermutation()
{
    std::vector<int> tempToSwap(N);
    int iter = 0;
    for (auto i = 0; i < N; i++){
        tempToSwap[i] = i;
    }
    std::random_shuffle(tempToSwap.begin(), tempToSwap.end());
    return tempToSwap;
}

void swapCities(int size)
{  
    do {
        firstCity = rand() % size;
        secondCity = rand() % size;
    } while (firstCity == secondCity);
    
    std::swap(path[firstCity], path[secondCity]);
}

void invertCities(int size)
{
	do {
		firstCity = rand() % size;
		secondCity = rand() % size;
	} while (firstCity == secondCity || firstCity > secondCity);

	std::reverse(path.begin()+firstCity, path.begin()+secondCity);
}      

void insertCity(int size)
{
	do {
		firstCity = rand() % size;  // to miasto zostanie wstawione
		secondCity = rand() % size; // w miejsce za tym miastem (jesli fC > sC, jeśli sC jest większe, to wtedy wstawianie odbywa się na odwrót)
	} while (firstCity == secondCity);

	if (firstCity > secondCity) {
		path.insert(path.begin() + secondCity, path[firstCity - 1]);
		path.erase(path.begin() + firstCity);
	}
	else {
		path.insert(path.begin() + firstCity, path[secondCity - 1]);
		path.erase(path.begin() + (secondCity - 1));
	}

	//TOOD : Ogarnij dla przypadku, gdzie fC lub sC jest 0


	///*path.insert(path.begin() + 7, path[2]);
	//path.erase(path.begin() + 2);*/
	////jeśli secondCity > firstCity


	//path.insert(path.begin() + 3, path[6]);
	//path.erase(path.begin() + 7);
	////jeśli firstCity > secondCity
}
 



int calculateDistance(std::vector<int> path)
{
    int cost = 0;

	for (auto i = 0; i < path.size() - 1; i++) {
        cost += cities[path[i]][path[i + 1]];
    }
    cost += cities[path[N-1]][path[0]];

    return cost;
}

double getProbability(int difference, double temperature) {
    return exp(difference / temperature);
}

void loadINIFile()
{
	std::fstream file;
	std::string fileName = "Badania.INI";
	int number_of_nodes = 0;
	int weight;
	std::clock_t* counter;
	std::string txtFileName;
	double *error;
	int iterations;
	int optimalSolution;
	std::vector<std::string> optimalPath;
	std::vector<int> *pathResult;
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
				solution[i] = simulatedAnnealing(cities, pow(N, 2), coolingRate, 3 * N, N * 2, optimalSolution);
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
	std::string fileName = "test_out_SA.csv";

	file.open(fileName, std::ios::out | std::ios::app);

	if (file.good()) {
		file << txtFileName << ";";
		file << iterations << ";";
		file << optimalSolution << ";";
		for (auto a : optimalPath)
			file << a  << " ";
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
				if (i!=j) {
					tmpVec[i].push_back(weight);
				}
				else if (i==j) {
					tmpVec[i].push_back(INT_MAX);
				}
			}
		}
	}
	cities = tmpVec;
	file.close();
}
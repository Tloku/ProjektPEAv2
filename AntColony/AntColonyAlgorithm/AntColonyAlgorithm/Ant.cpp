#include "Ant.h"

Ant::Ant() {}

Ant::Ant(int n, int initialCity) {
	setInitialCity(initialCity);
	setCurrentCity(initialCity);
	setSize(n);
	totalDistance = 0;

	std::vector<int> tmp;
	std::vector<bool> tmpB(getSize(), false);
	tmpB[initialCity] = true;
	tmp.push_back(initialCity);

	setAntPath(tmp);
	setAntVisitedCities(tmpB);

	tmp.clear();
	tmp.shrink_to_fit();
	tmpB.clear();
	tmpB.shrink_to_fit();
}


void Ant::moveAnt(int toCity)
{
	this->currentCity = toCity;
	antPath.push_back(currentCity);
	antVisitedCities[toCity] = true;
}

void Ant::calculateTotalDistance(int distance)
{
	setTotalDistance(getTotalDistance() + distance);
}

bool Ant::checkIfCityIsVisited(int city)
{
	return antVisitedCities[city];
}

bool Ant::checkIfAllCitiesAreVisited()
{
	std::vector<int> tmpPath = getAntPath();
	return tmpPath.size() == getSize();
}

void Ant::clear()
{
	for (bool visited : antVisitedCities) {
		visited = false;
	}
	antPath.clear();
}

void Ant::setAntPath(std::vector<int> antPath)
{
	this->antPath = antPath;
}

std::vector<int> Ant::getAntPath()
{
	return antPath;
}

void Ant::setAntVisitedCities(std::vector<bool> antVisitedCities)
{
	this->antVisitedCities = antVisitedCities;
}

std::vector<bool> Ant::getAntVisitedCities()
{
	return antVisitedCities;
}

void Ant::setInitialCity(int initialCity)
{
	this->initialCity = initialCity;
}

int Ant::getInitialCity()
{
	return initialCity;
}

void Ant::setTotalDistance(int totalDistance)
{
	this->totalDistance = totalDistance;
}

int Ant::getTotalDistance()
{
	return totalDistance;
}

void Ant::setCurrentCity(int city)
{
	this->currentCity = city;
}

int Ant::getCurrentCity()
{
	return currentCity;
}

void Ant::setSize(int size)
{
	this->size = size;
}

int Ant::getSize()
{
	return size;
}

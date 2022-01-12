#include <vector>


class Ant {
private:
	std::vector<int> antPath;
	std::vector<bool> antVisitedCities;
	int initialCity;
	int totalDistance;
	int currentCity;
	int size;

public:
	Ant();
	Ant(int n, int initialCity);

	void moveAnt(int toCity);
	void calculateTotalDistance(int distance);
	bool checkIfCityIsVisited(int city);
	bool checkIfAllCitiesAreVisited();

	void setAntPath(std::vector<int> antPath);
	std::vector<int> getAntPath();
	void setAntVisitedCities(std::vector<bool> antVisitedCities);
	std::vector<bool> getAntVisitedCities();
	void setInitialCity(int);
	int getInitialCity();
	void setTotalDistance(int);
	int getTotalDistance();
	void setCurrentCity(int);
	int getCurrentCity();
	void setSize(int);
	int getSize();
};
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <random>

using namespace std;

// Struct to represent a city with x and y coordinates
struct City {
    int x, y;
};

// Function to calculate Euclidean distance between two cities
double euclideanDistance(const City &a, const City &b) {
    // Use the distance formula to calculate the straight-line distance
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// Function to calculate the total cost of the given tour
double calculateTourCost(const vector<City> &cities, const vector<int> &tour) {
    double cost = 0.0;
    int numCities = tour.size();
    // Sum up the distance between consecutive cities in the tour
    for (int i = 0; i < numCities - 1; i++) {
        cost += euclideanDistance(cities[tour[i]], cities[tour[i + 1]]);
    }
    // Add the distance to return to the starting city
    cost += euclideanDistance(cities[tour[numCities - 1]], cities[tour[0]]);
    return cost;
}

// Function to solve TSP using Simulated Annealing
vector<int> simulatedAnnealing(const vector<City> &cities, int maxIterations = 10000, double initialTemperature = 10000, double coolingRate = 0.995) {
    int numCities = cities.size();

    // Initial solution: a random permutation of the city indices
    vector<int> currentTour(numCities);
    for (int i = 0; i < numCities; i++) currentTour[i] = i;

    random_device rd;
    mt19937 g(rd());
    shuffle(currentTour.begin(), currentTour.end(), g);

    double currentCost = calculateTourCost(cities, currentTour);
    vector<int> bestTour = currentTour;
    double bestCost = currentCost;

    double temperature = initialTemperature;

    // Simulated Annealing main loop
    for (int iteration = 0; iteration < maxIterations; iteration++) {
        // Generate a neighboring solution by swapping two random cities
        vector<int> newTour = currentTour;
        int city1 = rand() % numCities;
        int city2 = (city1 + 1 + rand() % (numCities - 1)) % numCities;
        swap(newTour[city1], newTour[city2]);

        double newCost = calculateTourCost(cities, newTour);
        double costDifference = newCost - currentCost;

        // Decide whether to accept the new solution
        if (costDifference < 0 || (exp(-costDifference / temperature) > ((double)rand() / RAND_MAX))) {
            currentTour = newTour;
            currentCost = newCost;
            // Update the best solution if necessary
            if (currentCost < bestCost) {
                bestTour = currentTour;
                bestCost = currentCost;
            }
        }

        // Gradually cool down the system
        temperature *= coolingRate;
    }

    // Output the final cost for reference
    cout << "Final cost: " << bestCost << endl;
    return bestTour;
}

// Helper function to trim whitespace from strings (for reading .tsp files)
string trim(const string &str) {
    auto start = str.begin();
    while (start != str.end() && isspace(*start)) ++start;
    auto end = str.end();
    do {
        --end;
    } while (end != start && isspace(*end));
    return string(start, end + 1);
}

// Function to load cities from a .tsp file
vector<City> loadCitiesFromFile(const string &filename) {
    vector<City> cities;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return cities;
    }

    string line;
    bool startReading = false;

    while (getline(file, line)) {
        line = trim(line);
        if (line == "NODE_COORD_SECTION") {
            startReading = true;
            continue;
        }
        if (startReading && line != "EOF") {
            istringstream iss(line);
            int id, x, y;
            if (iss >> id >> x >> y) {
                City city = {x, y};
                cities.push_back(city);
            } else {
                cerr << "Error: Invalid line format '" << line << "'" << endl;
            }
        }
    }

    return cities;
}

int main() {
    srand(static_cast<unsigned int>(time(0))); // Seed random number generator

    // Load cities from file
    string filename = "dataset/xql662.tsp";
    vector<City> cities = loadCitiesFromFile(filename);

    if (cities.size() < 2) {
        cerr << "Error: Not enough cities to calculate a tour." << endl;
        return 1;
    }

    // Perform Simulated Annealing
    vector<int> bestTour = simulatedAnnealing(cities);

    // Print the best tour
    cout << "Best tour: ";
    for (int city : bestTour) {
        cout << city << " ";
    }
    cout << endl;

    return 0;
}

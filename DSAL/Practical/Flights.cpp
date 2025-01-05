#include <iostream>
#include <string>
using namespace std;

const int MAX_CITIES = 100;
const int MAX_FLIGHTS = 1000;

struct Flight {
  string from;
  string to;
  int cost;
};

class FlightGraph {
  string cities[MAX_CITIES];
  Flight flights[MAX_FLIGHTS];
  int cityCount = 0;
  int flightCount = 0;

  int findCityIndex(const string &city) {
    for (int i = 0; i < cityCount; ++i) {
      if (cities[i] == city) return i;
    }
    return -1;
  }

  void addCity(const string &city) {
    if (findCityIndex(city) == -1) {
      cities[cityCount++] = city;
    }
  }

 public:
  void addFlight(const string &from, const string &to, int cost) {
    addCity(from);
    addCity(to);
    flights[flightCount++] = { from, to, cost };
  }

  void display() {
    cout << "\nFlight Connections:\n";
    for (int i = 0; i < cityCount; ++i) {
      string city = cities[i];
      cout << city << " -> ";
      bool hasConnection = false;
      for (int j = 0; j < flightCount; ++j) {
        if (flights[j].from == city) {
          cout << "(" << flights[j].to << ", cost: " << flights[j].cost << ") ";
          hasConnection = true;
        }
      }
      if (!hasConnection) cout << "No flights";
      cout << endl;
    }
  }

  void displayMatrix() {
    if (cityCount == 0) {
      cout << "\nNo cities in the graph.\n";
      return;
    }

    cout << "\nAdjacency Matrix (Cost):\n";

    cout << "\t";
    for (int i = 0; i < cityCount; ++i) {
      cout << cities[i] << "\t";
    }
    cout << endl;

    // Matrix content
    for (int i = 0; i < cityCount; ++i) {
      cout << cities[i] << "\t";
      for (int j = 0; j < cityCount; ++j) {
        int cost = 0;
        bool found = false;
        for (int k = 0; k < flightCount; ++k) {
          int fromIndex = findCityIndex(flights[k].from);
          int toIndex = findCityIndex(flights[k].to);
          if (fromIndex == i && toIndex == j) {
            cost = flights[k].cost;
            found = true;
            break;
          }
        }
        cout << cost << "\t";
      }
      cout << endl;
    }
  }
};

int main() {
  FlightGraph g;
  int n;
  cout << "Enter number of flights: ";
  cin >> n;

  cout << "Enter flight details (from_city to_city cost):\n";
  for (int i = 0; i < n; ++i) {
    string from, to;
    int cost;
    cin >> from >> to >> cost;
    g.addFlight(from, to, cost);
  }

  g.display();
  g.displayMatrix();
  return 0;
}
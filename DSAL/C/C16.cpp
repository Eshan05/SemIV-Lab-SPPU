// 16. Tour operator organizes guided bus trips across the Maharashtra. Tourists may have different preferences. Tour operator offers a choice from many different routes. Every day the bus moves from starting city S to another city F as chosen by client. On this way, the tourists can see the sights alongside the route traveled from S to F. Client may have preference to choose route. There is a restriction on the routes that the tourists may choose from, the bus has to take a short route from S to F or a route having one distance unit longer than the minimal distance. Two routes from S to F are considered different if there is at least one road from a city A to a city B which is part of one route, but not of the other route.

#include <iostream>
#include <string>
using namespace std;

const int MAXV = 30;

int n = 0;
string city[MAXV];
int mat[MAXV][MAXV];
int directedGraph = 0;

int distArr[MAXV];
int visited[MAXV];
int path[MAXV];
int pathLen = 0;
int targetNode = -1;
int shortestLen = -1;
int shortestCount = 0;
int plusOneCount = 0;

void clearLine() {
	char c;
	while (cin.get(c)) if (c == '\n') break;
}

int readInt(const string& prompt) {
	int x;
	while (true) {
		cout << prompt;
		if (cin >> x) return x;
		cout << "Invalid number. Try again.\n";
		cin.clear();
		clearLine();
	}
}

string readLine(const string& prompt) {
	cout << prompt;
	string s;
	getline(cin >> ws, s);
	return s;
}

int findCity(const string& name) {
	for (int i = 0; i < n; ++i) if (city[i] == name) return i;
	return -1;
}

void resetGraph() {
	for (int i = 0; i < MAXV; ++i) {
		for (int j = 0; j < MAXV; ++j) mat[i][j] = 0;
	}
	n = 0;
	directedGraph = 0;
}

void addRoad(int u, int v) {
	if (u < 0 || v < 0 || u >= n || v >= n || u == v) return;
	mat[u][v] = 1;
	if (!directedGraph) mat[v][u] = 1;
}

void displayGraph() {
	if (n == 0) {
		cout << "Graph empty\n";
		return;
	}
	cout << "\nCities:\n";
	for (int i = 0; i < n; ++i) cout << i << " -> " << city[i] << "\n";

	cout << "\nRoads:\n";
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			if (mat[i][j]) cout << city[i] << " -> " << city[j] << "\n";
		}
	}
}

void bfsDist(int s) {
	int q[MAXV], f = 0, r = -1;
	for (int i = 0; i < n; ++i) distArr[i] = -1;
	distArr[s] = 0;
	q[++r] = s;

	while (f <= r) {
		int u = q[f++];
		for (int v = 0; v < n; ++v) {
			if (mat[u][v] && distArr[v] == -1) {
				distArr[v] = distArr[u] + 1;
				q[++r] = v;
			}
		}
	}
}

void printCurrentPath() {
	for (int i = 0; i < pathLen; ++i) {
		cout << city[path[i]];
		if (i + 1 < pathLen) cout << " -> ";
	}
	cout << "\n";
}

void dfsEnumerate(int u, int depth) {
	if (depth > shortestLen + 1) return;

	if (u == targetNode) {
		if (depth == shortestLen) {
			shortestCount++;
			cout << "Shortest route " << shortestCount << ": ";
			printCurrentPath();
		} else if (depth == shortestLen + 1) {
			plusOneCount++;
			cout << "+1 route " << plusOneCount << ": ";
			printCurrentPath();
		}
		return;
	}

	for (int v = 0; v < n; ++v) {
		if (mat[u][v] && !visited[v]) {
			visited[v] = 1;
			path[pathLen++] = v;
			dfsEnumerate(v, depth + 1);
			pathLen--;
			visited[v] = 0;
		}
	}
}

void findRoutes() {
	if (n == 0) {
		cout << "Build graph first\n";
		return;
	}

	string sName = readLine("Start city S: ");
	string fName = readLine("Destination city F: ");
	int s = findCity(sName), f = findCity(fName);
	if (s == -1 || f == -1) {
		cout << "Invalid city name\n";
		return;
	}

	bfsDist(s);
	if (distArr[f] == -1) {
		cout << "No route exists from " << sName << " to " << fName << "\n";
		return;
	}

	shortestLen = distArr[f];
	targetNode = f;
	shortestCount = 0;
	plusOneCount = 0;

	for (int i = 0; i < n; ++i) visited[i] = 0;
	pathLen = 0;
	visited[s] = 1;
	path[pathLen++] = s;

	cout << "\nMinimum distance from S to F: " << shortestLen << "\n";
	cout << "Allowed route lengths: " << shortestLen << " and " << (shortestLen + 1) << "\n\n";
	dfsEnumerate(s, 0);

	cout << "\nSummary:\n";
	cout << "Shortest routes count: " << shortestCount << "\n";
	cout << "(Shortest+1) routes count: " << plusOneCount << "\n";
	cout << "Total valid routes: " << (shortestCount + plusOneCount) << "\n";
}

void buildGraph() {
	resetGraph();
	n = readInt("Enter number of cities (max 30): ");
	if (n < 0) n = 0;
	if (n > MAXV) n = MAXV;

	directedGraph = readInt("Directed roads? (1=yes, 0=no): ");
	if (directedGraph != 1) directedGraph = 0;

	for (int i = 0; i < n; ++i) city[i] = readLine("City " + to_string(i) + " name: ");

	int e = readInt("Enter number of roads: ");
	for (int i = 0; i < e; ++i) {
		cout << "Road " << (i + 1) << ":\n";
		string a = readLine("  From city: ");
		string b = readLine("  To city: ");
		int u = findCity(a), v = findCity(b);
		if (u == -1 || v == -1) cout << "  Invalid city name, road skipped\n";
		else addRoad(u, v);
	}
	cout << "Graph built\n";
}

int main() {
	resetGraph();

	while (true) {
		cout << "\n===== Route Selection TUI (C16) =====\n";
		cout << "1. Build/Rebuild Graph\n";
		cout << "2. Add Road\n";
		cout << "3. Display Graph\n";
		cout << "4. Find allowed routes (shortest or shortest+1)\n";
		cout << "5. Exit\n";

		int ch = readInt("Enter choice: ");

		if (ch == 1) {
			buildGraph();
		} else if (ch == 2) {
			if (n == 0) {
				cout << "Build graph first\n";
			} else {
				string a = readLine("From city: ");
				string b = readLine("To city: ");
				int u = findCity(a), v = findCity(b);
				if (u == -1 || v == -1) cout << "Invalid city name\n";
				else {
					addRoad(u, v);
					cout << "Road added\n";
				}
			}
		} else if (ch == 3) {
			displayGraph();
		} else if (ch == 4) {
			findRoutes();
		} else if (ch == 5) {
			break;
		} else {
			cout << "Invalid choice\n";
		}
	}

	return 0;
}

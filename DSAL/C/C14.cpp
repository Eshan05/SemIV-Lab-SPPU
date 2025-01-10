// 14. There are flight paths between cities. If there is a flight between city A and city B then there is an edge between the cities. The cost of the edge can be the time that flight take to reach city B from A, or the amount of fuel used for the journey. Represent this as a graph. The node can be represented by airport name or name of the city. Use adjacency list representation of the graph or use adjacency matrix representation of the graph. Check whether the graph is connected or not. Justify the storage representation used.

#include <iostream>
#include <string>
using namespace std;

const int MAXC = 50;
const int INF = 1000000000;

struct AdjNode {
	int v;
	int w;
	AdjNode* next;
};

int n = 0;
string city[MAXC];
int mat[MAXC][MAXC];
AdjNode* adj[MAXC];
int directedGraph = 0;
int edgeCount = 0;

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
	for (int i = 0; i < MAXC; ++i) {
		for (int j = 0; j < MAXC; ++j) mat[i][j] = (i == j ? 0 : INF);
		AdjNode* cur = adj[i];
		while (cur != 0) {
			AdjNode* nx = cur->next;
			delete cur;
			cur = nx;
		}
		adj[i] = 0;
	}
	n = 0;
	edgeCount = 0;
	directedGraph = 0;
}

void addListEdge(int u, int v, int w) {
	AdjNode* node = new AdjNode;
	node->v = v;
	node->w = w;
	node->next = adj[u];
	adj[u] = node;
}

void addFlight(int u, int v, int w) {
	if (u < 0 || v < 0 || u >= n || v >= n || u == v) return;

	if (mat[u][v] == INF) edgeCount++;
	mat[u][v] = w;
	addListEdge(u, v, w);

	if (!directedGraph) {
		mat[v][u] = w;
		addListEdge(v, u, w);
	}
}

void displayMatrix() {
	if (n == 0) {
		cout << "Graph empty\n";
		return;
	}
	cout << "\nAdjacency Matrix (costs)\n      ";
	for (int i = 0; i < n; ++i) cout << i << "   ";
	cout << "\n";
	for (int i = 0; i < n; ++i) {
		cout << i << "(" << city[i] << ") ";
		for (int j = 0; j < n; ++j) {
			if (mat[i][j] == INF) cout << "INF ";
			else cout << mat[i][j] << "   ";
		}
		cout << "\n";
	}
}

void displayList() {
	if (n == 0) {
		cout << "Graph empty\n";
		return;
	}
	cout << "\nAdjacency List (destination,cost)\n";
	for (int i = 0; i < n; ++i) {
		cout << i << "(" << city[i] << ") -> ";
		AdjNode* cur = adj[i];
		while (cur != 0) {
			cout << "(" << cur->v << "," << cur->w << ")";
			cur = cur->next;
			if (cur != 0) cout << " -> ";
		}
		cout << "\n";
	}
}

int bfsConnectedFrom0_Matrix() {
	if (n == 0) return 1;
	int vis[MAXC], q[MAXC], f = 0, r = -1;
	for (int i = 0; i < n; ++i) vis[i] = 0;
	q[++r] = 0;
	vis[0] = 1;

	while (f <= r) {
		int u = q[f++];
		for (int v = 0; v < n; ++v) {
			if (u != v && mat[u][v] != INF && !vis[v]) {
				vis[v] = 1;
				q[++r] = v;
			}
		}
	}

	for (int i = 0; i < n; ++i) if (!vis[i]) return 0;
	return 1;
}

int bfsConnectedFrom0_List() {
	if (n == 0) return 1;
	int vis[MAXC], q[MAXC], f = 0, r = -1;
	for (int i = 0; i < n; ++i) vis[i] = 0;
	q[++r] = 0;
	vis[0] = 1;

	while (f <= r) {
		int u = q[f++];
		AdjNode* cur = adj[u];
		while (cur != 0) {
			int v = cur->v;
			if (!vis[v]) {
				vis[v] = 1;
				q[++r] = v;
			}
			cur = cur->next;
		}
	}

	for (int i = 0; i < n; ++i) if (!vis[i]) return 0;
	return 1;
}

void justifyStorage() {
	if (n == 0) {
		cout << "Graph empty\n";
		return;
	}

	int maxEdges = directedGraph ? n * (n - 1) : (n * (n - 1)) / 2;
	double density = 0.0;
	if (maxEdges > 0) density = (100.0 * edgeCount) / maxEdges;

	cout << "\nStorage Justification\n";
	cout << "Vertices: " << n << ", Edges: " << edgeCount << ", Density: " << density << "%\n";
	if (density < 35.0) {
		cout << "Adjacency List is generally better for sparse graphs (less memory, faster edge iteration).\n";
	} else {
		cout << "Adjacency Matrix is reasonable for denser graphs (fast O(1) edge existence check).\n";
	}
	cout << "This program maintains both, so you can study and compare both representations directly.\n";
}

void buildGraph() {
	resetGraph();
	n = readInt("Enter number of cities (max 50): ");
	if (n < 0) n = 0;
	if (n > MAXC) n = MAXC;

	directedGraph = readInt("Directed graph? (1=yes, 0=no): ");
	if (directedGraph != 1) directedGraph = 0;

	for (int i = 0; i < n; ++i) {
		city[i] = readLine("City " + to_string(i) + " name: ");
	}

	int e = readInt("Enter number of flight paths: ");
	for (int i = 0; i < e; ++i) {
		cout << "Flight " << (i + 1) << ":\n";
		string a = readLine("  From city: ");
		string b = readLine("  To city: ");
		int w = readInt("  Cost (time/fuel/etc): ");
		int u = findCity(a), v = findCity(b);
		if (u == -1 || v == -1) cout << "  Invalid city name, flight skipped\n";
		else addFlight(u, v, w);
	}
	cout << "Graph built\n";
}

int main() {
	resetGraph();

	while (true) {
		cout << "\n===== Flight Graph TUI (C14) =====\n";
		cout << "1. Build/Rebuild Graph\n";
		cout << "2. Add Flight\n";
		cout << "3. Display Adjacency Matrix\n";
		cout << "4. Display Adjacency List\n";
		cout << "5. Check Connectivity (Matrix)\n";
		cout << "6. Check Connectivity (List)\n";
		cout << "7. Justify Representation\n";
		cout << "8. Exit\n";

		int ch = readInt("Enter choice: ");

		if (ch == 1) {
			buildGraph();
		} else if (ch == 2) {
			if (n == 0) {
				cout << "Build graph first\n";
			} else {
				string a = readLine("From city: ");
				string b = readLine("To city: ");
				int w = readInt("Cost: ");
				int u = findCity(a), v = findCity(b);
				if (u == -1 || v == -1) cout << "Invalid city name\n";
				else {
					addFlight(u, v, w);
					cout << "Flight added\n";
				}
			}
		} else if (ch == 3) {
			displayMatrix();
		} else if (ch == 4) {
			displayList();
		} else if (ch == 5) {
			cout << (bfsConnectedFrom0_Matrix() ? "Connected (from matrix view)\n" : "Not connected (from matrix view)\n");
		} else if (ch == 6) {
			cout << (bfsConnectedFrom0_List() ? "Connected (from list view)\n" : "Not connected (from list view)\n");
		} else if (ch == 7) {
			justifyStorage();
		} else if (ch == 8) {
			break;
		} else {
			cout << "Invalid choice\n";
		}
	}

	resetGraph();
	return 0;
}

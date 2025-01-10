// 15. You have a business with several offices; you want to lease phone lines to connect them up with each other; and the phone company charges different amounts of money to connect different pairs of cities. You want a set of lines that connects all your offices with a minimum total cost. Solve the problem by suggesting appropriate data structures.

#include <iostream>
#include <string>
using namespace std;

const int MAXV = 60;
const int MAXE = 400;
const int INF = 1000000000;

struct Edge {
	int u;
	int v;
	int w;
};

int n = 0;
string office[MAXV];
int mat[MAXV][MAXV];
Edge edges[MAXE];
int ec = 0;

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

int findOffice(const string& name) {
	for (int i = 0; i < n; ++i) if (office[i] == name) return i;
	return -1;
}

void resetGraph() {
	for (int i = 0; i < MAXV; ++i) {
		for (int j = 0; j < MAXV; ++j) mat[i][j] = (i == j ? 0 : INF);
	}
	n = 0;
	ec = 0;
}

void addEdgeUndirected(int u, int v, int w) {
	if (u < 0 || v < 0 || u >= n || v >= n || u == v) return;
	if (ec >= MAXE) {
		cout << "Edge storage full\n";
		return;
	}
	mat[u][v] = w;
	mat[v][u] = w;
	edges[ec].u = u;
	edges[ec].v = v;
	edges[ec].w = w;
	ec++;
}

void displayGraph() {
	if (n == 0) {
		cout << "Graph empty\n";
		return;
	}
	cout << "\nOffices:\n";
	for (int i = 0; i < n; ++i) cout << i << " -> " << office[i] << "\n";

	cout << "\nEdges (u,v,w):\n";
	for (int i = 0; i < ec; ++i) {
		cout << "(" << office[edges[i].u] << ", " << office[edges[i].v] << ", " << edges[i].w << ")\n";
	}
}

void primWithSteps(int start) {
	if (n == 0) {
		cout << "Graph empty\n";
		return;
	}
	int key[MAXV], parent[MAXV], used[MAXV];
	for (int i = 0; i < n; ++i) {
		key[i] = INF;
		parent[i] = -1;
		used[i] = 0;
	}
	key[start] = 0;

	int total = 0;
	cout << "\n--- Prim's Algorithm Steps ---\n";
	for (int step = 0; step < n; ++step) {
		int u = -1, best = INF;
		for (int i = 0; i < n; ++i) {
			if (!used[i] && key[i] < best) {
				best = key[i];
				u = i;
			}
		}
		if (u == -1) {
			cout << "Graph is disconnected; MST not possible for all vertices.\n";
			return;
		}

		used[u] = 1;
		if (parent[u] != -1) {
			total += mat[parent[u]][u];
			cout << "Select edge: " << office[parent[u]] << " - " << office[u]
					 << " (cost " << mat[parent[u]][u] << ")\n";
		} else {
			cout << "Start at vertex: " << office[u] << "\n";
		}

		for (int v = 0; v < n; ++v) {
			if (!used[v] && mat[u][v] < key[v]) {
				cout << "  Update best connection for " << office[v] << ": "
						 << "parent=" << office[u] << ", key=" << mat[u][v] << "\n";
				key[v] = mat[u][v];
				parent[v] = u;
			}
		}
	}

	cout << "Total MST cost by Prim: " << total << "\n";
}

int dsuParent[MAXV], dsuRank[MAXV];

void dsuInit(int nV) {
	for (int i = 0; i < nV; ++i) {
		dsuParent[i] = i;
		dsuRank[i] = 0;
	}
}

int dsuFind(int x) {
	if (dsuParent[x] == x) return x;
	dsuParent[x] = dsuFind(dsuParent[x]);
	return dsuParent[x];
}

void dsuUnion(int a, int b) {
	int ra = dsuFind(a), rb = dsuFind(b);
	if (ra == rb) return;
	if (dsuRank[ra] < dsuRank[rb]) dsuParent[ra] = rb;
	else if (dsuRank[rb] < dsuRank[ra]) dsuParent[rb] = ra;
	else {
		dsuParent[rb] = ra;
		dsuRank[ra]++;
	}
}

void sortEdgesByWeight(Edge arr[], int m) {
	for (int i = 0; i < m - 1; ++i) {
		for (int j = 0; j < m - i - 1; ++j) {
			if (arr[j].w > arr[j + 1].w) {
				Edge t = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = t;
			}
		}
	}
}

void kruskalWithSteps() {
	if (n == 0) {
		cout << "Graph empty\n";
		return;
	}
	Edge arr[MAXE];
	for (int i = 0; i < ec; ++i) arr[i] = edges[i];
	sortEdgesByWeight(arr, ec);
	dsuInit(n);

	int usedEdges = 0;
	int total = 0;
	cout << "\n--- Kruskal's Algorithm Steps ---\n";
	for (int i = 0; i < ec; ++i) {
		int u = arr[i].u, v = arr[i].v, w = arr[i].w;
		int ru = dsuFind(u), rv = dsuFind(v);

		cout << "Consider edge " << office[u] << " - " << office[v]
				 << " (cost " << w << ")";

		if (ru != rv) {
			cout << " -> TAKE\n";
			dsuUnion(ru, rv);
			total += w;
			usedEdges++;
			if (usedEdges == n - 1) break;
		} else {
			cout << " -> SKIP (would form cycle)\n";
		}

		if (i + 1 < ec && arr[i + 1].w == w) {
			cout << "  Note: another edge with same weight exists; alternative MST may be possible.\n";
		}
	}

	if (usedEdges != n - 1) {
		cout << "Graph is disconnected; MST not possible for all vertices.\n";
	} else {
		cout << "Total MST cost by Kruskal: " << total << "\n";
	}
}

void buildGraph() {
	resetGraph();
	n = readInt("Enter number of offices (max 60): ");
	if (n < 0) n = 0;
	if (n > MAXV) n = MAXV;

	for (int i = 0; i < n; ++i) office[i] = readLine("Office " + to_string(i) + " name: ");

	int m = readInt("Enter number of undirected edges: ");
	for (int i = 0; i < m; ++i) {
		cout << "Edge " << (i + 1) << ":\n";
		string a = readLine("  Office A: ");
		string b = readLine("  Office B: ");
		int w = readInt("  Connection cost: ");
		int u = findOffice(a), v = findOffice(b);
		if (u == -1 || v == -1) cout << "  Invalid office name, edge skipped\n";
		else addEdgeUndirected(u, v, w);
	}
	cout << "Graph built\n";
}

int main() {
	resetGraph();

	while (true) {
		cout << "\n===== MST TUI (C15) =====\n";
		cout << "1. Build/Rebuild Graph\n";
		cout << "2. Add Edge\n";
		cout << "3. Display Graph\n";
		cout << "4. Run Prim (with steps)\n";
		cout << "5. Run Kruskal (with steps)\n";
		cout << "6. Run Both and Compare\n";
		cout << "7. Exit\n";

		int ch = readInt("Enter choice: ");

		if (ch == 1) {
			buildGraph();
		} else if (ch == 2) {
			if (n == 0) {
				cout << "Build graph first\n";
			} else {
				string a = readLine("Office A: ");
				string b = readLine("Office B: ");
				int w = readInt("Cost: ");
				int u = findOffice(a), v = findOffice(b);
				if (u == -1 || v == -1) cout << "Invalid office name\n";
				else {
					addEdgeUndirected(u, v, w);
					cout << "Edge added\n";
				}
			}
		} else if (ch == 3) {
			displayGraph();
		} else if (ch == 4) {
			if (n == 0) cout << "Build graph first\n";
			else {
				string s = readLine("Start office name for Prim: ");
				int st = findOffice(s);
				if (st == -1) cout << "Invalid office name\n";
				else primWithSteps(st);
			}
		} else if (ch == 5) {
			kruskalWithSteps();
		} else if (ch == 6) {
			if (n == 0) cout << "Build graph first\n";
			else {
				string s = readLine("Start office name for Prim: ");
				int st = findOffice(s);
				if (st == -1) cout << "Invalid office name\n";
				else primWithSteps(st);
				kruskalWithSteps();
			}
		} else if (ch == 7) {
			break;
		} else {
			cout << "Invalid choice\n";
		}
	}

	return 0;
}

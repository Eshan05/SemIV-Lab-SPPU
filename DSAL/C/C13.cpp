// 13. Represent a given graph using adjacency matrix/list to perform DFS and using adjacency list to perform BFS. Use the map of the area around the college as the graph. Identify the prominent land marks as nodes and perform DFS and BFS on that.

#include <iostream>
#include <string>
using namespace std;

const int MAXV = 50;

struct AdjNode {
	int v;
	AdjNode* next;
};

int n = 0;
string names[MAXV];
int matrixG[MAXV][MAXV];
AdjNode* listG[MAXV];

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

int findVertex(const string& name) {
	for (int i = 0; i < n; ++i) if (names[i] == name) return i;
	return -1;
}

void resetGraph() {
	for (int i = 0; i < MAXV; ++i) {
		for (int j = 0; j < MAXV; ++j) matrixG[i][j] = 0;
		AdjNode* cur = listG[i];
		while (cur != 0) {
			AdjNode* nx = cur->next;
			delete cur;
			cur = nx;
		}
		listG[i] = 0;
	}
	n = 0;
}

void addListEdgeOneWay(int u, int v) {
	AdjNode* node = new AdjNode;
	node->v = v;
	node->next = listG[u];
	listG[u] = node;
}

void addUndirectedEdge(int u, int v) {
	if (u < 0 || v < 0 || u >= n || v >= n || u == v) return;
	matrixG[u][v] = 1;
	matrixG[v][u] = 1;

	addListEdgeOneWay(u, v);
	addListEdgeOneWay(v, u);
}

void displayMatrix() {
	if (n == 0) {
		cout << "Graph empty\n";
		return;
	}
	cout << "\nAdjacency Matrix\n    ";
	for (int i = 0; i < n; ++i) cout << i << " ";
	cout << "\n";
	for (int i = 0; i < n; ++i) {
		cout << i << " : ";
		for (int j = 0; j < n; ++j) cout << matrixG[i][j] << " ";
		cout << "(" << names[i] << ")\n";
	}
}

void displayList() {
	if (n == 0) {
		cout << "Graph empty\n";
		return;
	}
	cout << "\nAdjacency List\n";
	for (int i = 0; i < n; ++i) {
		cout << i << "(" << names[i] << ") -> ";
		AdjNode* cur = listG[i];
		while (cur != 0) {
			cout << cur->v << "(" << names[cur->v] << ") ";
			cur = cur->next;
			if (cur != 0) cout << "-> ";
		}
		cout << "\n";
	}
}

void dfsMatrix(int start) {
	int visited[MAXV];
	int st[MAXV], top = -1;
	for (int i = 0; i < n; ++i) visited[i] = 0;

	st[++top] = start;
	cout << "DFS (Adjacency Matrix): ";
	while (top != -1) {
		int u = st[top--];
		if (visited[u]) continue;
		visited[u] = 1;
		cout << names[u] << " ";

		for (int v = n - 1; v >= 0; --v) {
			if (matrixG[u][v] && !visited[v]) st[++top] = v;
		}
	}
	cout << "\n";
}

void dfsList(int start) {
	int visited[MAXV];
	int st[MAXV], top = -1;
	for (int i = 0; i < n; ++i) visited[i] = 0;

	st[++top] = start;
	cout << "DFS (Adjacency List): ";
	while (top != -1) {
		int u = st[top--];
		if (visited[u]) continue;
		visited[u] = 1;
		cout << names[u] << " ";

		int neigh[MAXV], c = 0;
		AdjNode* cur = listG[u];
		while (cur != 0) {
			if (!visited[cur->v]) neigh[c++] = cur->v;
			cur = cur->next;
		}
		for (int i = 0; i < c - 1; ++i) {
			for (int j = i + 1; j < c; ++j) {
				if (neigh[i] < neigh[j]) {
					int t = neigh[i];
					neigh[i] = neigh[j];
					neigh[j] = t;
				}
			}
		}
		for (int i = 0; i < c; ++i) st[++top] = neigh[i];
	}
	cout << "\n";
}

void bfsList(int start) {
	int visited[MAXV];
	int q[MAXV], f = 0, r = -1;
	for (int i = 0; i < n; ++i) visited[i] = 0;

	q[++r] = start;
	visited[start] = 1;
	cout << "BFS (Adjacency List): ";

	while (f <= r) {
		int u = q[f++];
		cout << names[u] << " ";

		int neigh[MAXV], c = 0;
		AdjNode* cur = listG[u];
		while (cur != 0) {
			if (!visited[cur->v]) neigh[c++] = cur->v;
			cur = cur->next;
		}
		for (int i = 0; i < c - 1; ++i) {
			for (int j = i + 1; j < c; ++j) {
				if (neigh[i] > neigh[j]) {
					int t = neigh[i];
					neigh[i] = neigh[j];
					neigh[j] = t;
				}
			}
		}
		for (int i = 0; i < c; ++i) {
			int v = neigh[i];
			if (!visited[v]) {
				visited[v] = 1;
				q[++r] = v;
			}
		}
	}
	cout << "\n";
}

void buildGraph() {
	resetGraph();
	int vn = readInt("Enter number of vertices (max 50): ");
	if (vn < 0) vn = 0;
	if (vn > MAXV) vn = MAXV;
	n = vn;

	for (int i = 0; i < n; ++i) {
		names[i] = readLine("Vertex " + to_string(i) + " name: ");
	}

	int e = readInt("Enter number of undirected edges: ");
	for (int i = 0; i < e; ++i) {
		cout << "Edge " << (i + 1) << ":\n";
		string a = readLine("  From vertex name: ");
		string b = readLine("  To vertex name: ");
		int u = findVertex(a), v = findVertex(b);
		if (u == -1 || v == -1) {
			cout << "  Invalid vertex name, edge skipped\n";
		} else {
			addUndirectedEdge(u, v);
		}
	}
	cout << "Graph built successfully\n";
}

int main() {
	resetGraph();

	while (true) {
		cout << "\n===== Graph TUI (C13) =====\n";
		cout << "1. Build/Rebuild Graph\n";
		cout << "2. Add Edge\n";
		cout << "3. Display Adjacency Matrix\n";
		cout << "4. Display Adjacency List\n";
		cout << "5. DFS using Matrix\n";
		cout << "6. DFS using List\n";
		cout << "7. BFS using List\n";
		cout << "8. Exit\n";

		int ch = readInt("Enter choice: ");

		if (ch == 1) {
			buildGraph();
		} else if (ch == 2) {
			if (n == 0) {
				cout << "Build graph first\n";
				continue;
			}
			string a = readLine("From vertex name: ");
			string b = readLine("To vertex name: ");
			int u = findVertex(a), v = findVertex(b);
			if (u == -1 || v == -1) cout << "Invalid vertex name\n";
			else {
				addUndirectedEdge(u, v);
				cout << "Edge added\n";
			}
		} else if (ch == 3) {
			displayMatrix();
		} else if (ch == 4) {
			displayList();
		} else if (ch == 5) {
			if (n == 0) cout << "Build graph first\n";
			else {
				string s = readLine("Start vertex name: ");
				int st = findVertex(s);
				if (st == -1) cout << "Invalid start vertex\n";
				else dfsMatrix(st);
			}
		} else if (ch == 6) {
			if (n == 0) cout << "Build graph first\n";
			else {
				string s = readLine("Start vertex name: ");
				int st = findVertex(s);
				if (st == -1) cout << "Invalid start vertex\n";
				else dfsList(st);
			}
		} else if (ch == 7) {
			if (n == 0) cout << "Build graph first\n";
			else {
				string s = readLine("Start vertex name: ");
				int st = findVertex(s);
				if (st == -1) cout << "Invalid start vertex\n";
				else bfsList(st);
			}
		} else if (ch == 8) {
			break;
		} else {
			cout << "Invalid choice\n";
		}
	}

	resetGraph();
	return 0;
}

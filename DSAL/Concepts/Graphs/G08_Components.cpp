// 7a. Connected components: Label every vertex by the component it belongs to.
// Demo: Three separate pieces, found once with BFS and once with DFS; the
// partition is the same, only the visiting order inside a piece differs.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;

static int n = 0;
static int edgeCount = 0;
static bool directed = false;
static int matrixG[MAXV][MAXV];
static vector<int> adj[MAXV];

static int compBfs[MAXV];
static int compDfs[MAXV];
static int compCount = 0;
static vector<vector<int>> members;

static int readInt(const string &prompt) {
  int x;
  while (true) {
    cout << prompt;
    if (cin >> x) {
      cin.ignore(1024, '\n');
      return x;
    }
    if (cin.eof()) {
      cout << "\nInput closed.\n";
      exit(0);
    }
    cout << "Invalid number. Try again.\n";
    cin.clear();
    cin.ignore(1024, '\n');
  }
}

static int readRange(const string &prompt, int lo, int hi) {
  while (true) {
    int x = readInt(prompt);
    if (x >= lo && x <= hi) return x;
    cout << "  Number must be in " << lo << ".." << hi << ".\n";
  }
}

static void resetGraph() {
  for (int i = 0; i < MAXV; i++) {
    adj[i].clear();
    for (int j = 0; j < MAXV; j++) matrixG[i][j] = 0;
  }
  n = 0;
  edgeCount = 0;
}

static void addEdge(int u, int v) {
  matrixG[u][v] = 1;
  adj[u].push_back(v);
  if (!directed) {
    matrixG[v][u] = 1;
    adj[v].push_back(u);
  }
  edgeCount++;
}

static void buildGraph() {
  int mode = readRange("Graph type (1 = Undirected, 2 = Directed): ", 1, 2);
  int newN = readRange("Number of vertices (0.." + to_string(MAXV) + "): ",
                       0, MAXV);

  resetGraph();
  directed = (mode == 2);
  n = newN;

  if (n == 0) {
    cout << "Build complete: 0 vertices, 0 edges.\n";
    return;
  }
  cout << "Enter each edge as two vertex numbers in 0.." << (n - 1) << ".\n";
  int e = 0;
  while (true) {
    e++;
    cout << "\nEdge " << e << "\n";
    int u = readInt("  From vertex (-1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To vertex (-1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      e--;
      continue;
    }
    if (u == v) {
      cout << "  Rejected: Self-loops are skipped in this demo.\n";
      e--;
      continue;
    }
    if (matrixG[u][v]) {
      cout << "  Rejected: That edge already exists.\n";
      e--;
      continue;
    }
    addEdge(u, v);
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << edgeCount
       << (edgeCount == 1 ? " edge.\n" : " edges.\n");
}

static void loadDemo(bool asDirected) {
  resetGraph();
  directed = asDirected;

  if (asDirected) {
    n = 6;
    cout << "Directed demo: 6 vertices.\n";
    cout << "  Arcs 0 -> 1, 2 -> 1, 2 -> 3, and 4 -> 5.\n";
    cout << "  Two weak components; no directed cycle.\n\n";
    addEdge(0, 1);
    addEdge(2, 1);
    addEdge(2, 3);
    addEdge(4, 5);
  } else {
    n = 9;
    cout << "Undirected demo: 9 vertices.\n";
    cout << "  Triangle 0 - 1 - 2, pair 3 - 4, square 5 - 6 - 7 - 8.\n";
    cout << "  Three components of sizes 3, 2 and 4.\n\n";
    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 0);
    addEdge(3, 4);
    addEdge(5, 6);
    addEdge(6, 7);
    addEdge(7, 8);
    addEdge(8, 5);
  }

  cout << "Demo loaded: " << n << " vertices.\n";
}

static void showAdjacency() {
  cout << "\n--- Adjacency List ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  for (int i = 0; i < n; i++) {
    cout << setw(3) << i << ":";
    if (adj[i].empty()) cout << " (None)";
    else
      for (int v : adj[i]) cout << " " << v;
    cout << "\n";
  }
  cout << (directed ? "Weak components ignore arc direction.\n"
                    : "Each line lists neighbours in insertion order.\n");
}

static bool linked(int u, int v) {
  if (directed) return matrixG[u][v] || matrixG[v][u];
  return matrixG[u][v] != 0;
}

static void labelByBfs(int comp[]) {
  compCount = 0;
  members.clear();

  for (int i = 0; i < n; i++) comp[i] = -1;

  for (int start = 0; start < n; start++) {
    if (comp[start] != -1) continue;

    vector<int> piece;
    vector<int> q;
    int front = 0;
    q.push_back(start);
    comp[start] = compCount;

    while (front < (int)q.size()) {
      int u = q[front];
      front++;
      piece.push_back(u);
      for (int v = 0; v < n; v++) {
        if (!linked(u, v)) continue;
        if (comp[v] != -1) continue;
        comp[v] = compCount;
        q.push_back(v);
      }
    }

    members.push_back(piece);
    compCount++;
  }
}

static void dfsPiece(int u, int label, vector<int> &piece) {
  compDfs[u] = label;
  piece.push_back(u);
  for (int v : adj[u]) {
    if (compDfs[v] != -1) continue;
    dfsPiece(v, label, piece);
  }
  if (directed) {
    for (int p = 0; p < n; p++) {
      if (!matrixG[p][u]) continue;
      if (compDfs[p] != -1) continue;
      dfsPiece(p, label, piece);
    }
  }
}

static void labelByDfs() {
  for (int i = 0; i < n; i++) compDfs[i] = -1;

  int label = 0;
  members.clear();
  for (int start = 0; start < n; start++) {
    if (compDfs[start] != -1) continue;
    vector<int> piece;
    dfsPiece(start, label, piece);
    members.push_back(piece);
    label++;
  }
  compCount = label;
}

static void report(const char *how) {
  cout << "\n--- " << (directed ? "Weak components" : "Components") << " by "
       << how << " ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }

  for (int c = 0; c < compCount; c++) {
    cout << "Component " << c + 1 << " (Size " << members[c].size() << "):";
    for (int v : members[c]) cout << " " << v;
    cout << "\n";
  }
  cout << "Count: " << compCount << "\n";
  if (compCount == 1) cout << "The graph is connected.\n";
  else cout << "The graph is not connected.\n";
}

static void runBfs() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  labelByBfs(compBfs);
  report("BFS");
  cout << "Label:";
  for (int i = 0; i < n; i++) cout << " " << compBfs[i] + 1;
  cout << "\n";
}

static void runDfs() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  labelByDfs();
  report("DFS");
  cout << "Label:";
  for (int i = 0; i < n; i++) cout << " " << compDfs[i] + 1;
  cout << "\n";
}

static void runCompare() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  labelByBfs(compBfs);
  int countBfs = compCount;
  int bfsLabel[MAXV];
  for (int i = 0; i < n; i++) bfsLabel[i] = compBfs[i];

  labelByDfs();
  int countDfs = compCount;

  bool same = true;
  for (int i = 0; i < n && same; i++) {
    for (int j = i + 1; j < n; j++) {
      bool samePieceBfs = bfsLabel[i] == bfsLabel[j];
      bool samePieceDfs = compDfs[i] == compDfs[j];
      if (samePieceBfs != samePieceDfs) {
        same = false;
        break;
      }
    }
  }

  cout << "\nBFS labels: " << countBfs << " components.\n";
  cout << "DFS labels: " << countDfs << " components.\n";
  cout << "Same partition: " << (same ? "Yes" : "No") << "\n";
  cout << "Both runs group the vertices the same way; only the visit order\n";
  cout << "inside a component can differ.\n";
}

static string edgeLabel(int u, int v) {
  string s = "(" + to_string(u);
  s += directed ? " -> " : " - ";
  s += to_string(v) + ")";
  return s;
}

static void addEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nAdd edge (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
  while (true) {
    int u = readInt("  From vertex (-1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To vertex (-1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }

    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: Self-loops are skipped in this demo.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (matrixG[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " already exists.\n";
      if (!directed) {
        cout << "    Undirected edges are unordered: " << u << " - " << v
             << " is the same edge as " << v << " - " << u << ".\n";
      }
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    addEdge(u, v);
    cout << "  Added E" << (edgeCount - 1) << " " << edgeLabel(u, v) << "\n";
    cout << "    matrix[" << u << "][" << v << "] = 1";
    if (!directed) cout << ", matrix[" << v << "][" << u << "] = 1";
    cout << "\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
         << " now.\n";
    return;
  }
}

static void removeNeighbour(int u, int v) {
  for (int i = 0; i < (int)adj[u].size(); i++) {
    if (adj[u][i] != v) continue;
    adj[u].erase(adj[u].begin() + i);
    return;
  }
}

static void recountEdges() {
  edgeCount = 0;
  for (int u = 0; u < n; u++) {
    for (int v = 0; v < n; v++) {
      if (!matrixG[u][v]) continue;
      if (directed || u <= v) edgeCount++;
    }
  }
}

static void removeEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove an edge (V = " << n << ", so vertices 0.." << (n - 1)
       << ")\n";
  while (true) {
    int u = readInt("  From vertex (-1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To vertex (-1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }

    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (!matrixG[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " is not in the graph.\n";
      if (!directed) {
        cout << "    Undirected edges are unordered: " << u << " - " << v
             << " is the same edge as " << v << " - " << u << ".\n";
      }
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    matrixG[u][v] = 0;
    if (!directed) matrixG[v][u] = 0;
    removeNeighbour(u, v);
    if (!directed && u != v) removeNeighbour(v, u);
    edgeCount--;

    cout << "  Removed " << edgeLabel(u, v) << "\n";
    cout << "    matrix[" << u << "][" << v << "] = 0";
    if (!directed) cout << ", matrix[" << v << "][" << u << "] = 0";
    cout << "\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
         << " now.\n";
    return;
  }
}

static void addVertexInteractive() {
  if (n >= MAXV) {
    cout << "Vertex limit reached: MAXV = " << MAXV << ".\n";
    return;
  }

  int fresh = n;
  for (int i = 0; i <= fresh; i++) {
    matrixG[fresh][i] = 0;
    matrixG[i][fresh] = 0;
  }
  adj[fresh].clear();
  n++;

  cout << "\nAdded V" << fresh << " with no edges.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
}

static void removeVertexInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  int v;
  while (true) {
    v = readInt("  Vertex to remove (-1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (v >= 0 && v < n) break;
    cout << "  Rejected: Vertex must be in 0.." << (n - 1) << ".\n";
  }

  int kept[MAXV][MAXV];
  for (int a = 0; a < n; a++) {
    for (int b = 0; b < n; b++) kept[a][b] = 0;
  }
  for (int a = 0; a < n; a++) {
    if (a == v) continue;
    for (int b = 0; b < n; b++) {
      if (b == v) continue;
      int na = (a < v) ? a : a - 1;
      int nb = (b < v) ? b : b - 1;
      kept[na][nb] = matrixG[a][b];
    }
  }

  vector<int> keptAdj[MAXV];
  for (int a = 0; a < n; a++) {
    if (a == v) continue;
    int na = (a < v) ? a : a - 1;
    for (int w : adj[a]) {
      if (w == v) continue;
      keptAdj[na].push_back((w < v) ? w : w - 1);
    }
  }

  int before = edgeCount;
  n--;
  for (int a = 0; a < MAXV; a++) {
    adj[a].clear();
    for (int b = 0; b < MAXV; b++) matrixG[a][b] = 0;
  }
  for (int a = 0; a < n; a++) {
    adj[a] = keptAdj[a];
    for (int b = 0; b < n; b++) matrixG[a][b] = kept[a][b];
  }
  recountEdges();

  cout << "\nRemoved V" << v << ".\n";
  if (v < n) cout << "  Vertices above V" << v << " shift down by one.\n";
  cout << "  Edges lost: " << before - edgeCount;
  cout << " (Recounted from the matrix.)\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
}

static void clearAllEdges() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nThe graph already has no edges.\n";
    return;
  }

  int before = edgeCount;
  for (int u = 0; u < n; u++) {
    adj[u].clear();
    for (int w = 0; w < n; w++) matrixG[u][w] = 0;
  }
  edgeCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
}

static void graphOperationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: " << (directed ? "Directed" : "Undirected")
         << " | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Add an edge to the current graph\n";
    cout << "2. Remove an edge from the current graph\n";
    cout << "3. Add a vertex to the current graph\n";
    cout << "4. Remove a vertex from the current graph\n";
    cout << "5. Clear all edges\n";
    cout << "0. Back to the main menu\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      addEdgeInteractive();
    } else if (choice == 2) {
      removeEdgeInteractive();
    } else if (choice == 3) {
      addVertexInteractive();
    } else if (choice == 4) {
      removeVertexInteractive();
    } else if (choice == 5) {
      clearAllEdges();
    } else if (choice == 0) {
      return;
    } else {
      cout << "Invalid option.\n";
    }
  }
}

int main() {
  resetGraph();

  while (true) {
    cout << "\n=== G08: Connected Components ===\n";
    cout << "Graph: " << (directed ? "Directed" : "Undirected")
         << " | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build a graph\n";
    cout << "2. Load undirected demo (9 vertices)\n";
    cout << "3. Load directed demo (6 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Label components with BFS\n";
    cout << "6. Label components with DFS\n";
    cout << "7. Run both and compare\n";
    cout << "8. Show adjacency list\n";
    cout << "9. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo(false);
    } else if (choice == 3) {
      loadDemo(true);
    } else if (choice == 4) {
      graphOperationsMenu();
    } else if (choice == 5) {
      runBfs();
    } else if (choice == 6) {
      runDfs();
    } else if (choice == 7) {
      runCompare();
    } else if (choice == 8) {
      showAdjacency();
    } else if (choice == 9) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}

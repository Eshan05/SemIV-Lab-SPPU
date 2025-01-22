// 9b. Topological sort with DFS: The reverse of the finish order.
// Demo: A vertex finishes only after everything reachable from it finishes, so
// reading finish times backwards puts every arc before its target.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;

static int n = 0;
static int edgeCount = 0;
static int matrixG[MAXV][MAXV];
static vector<int> adj[MAXV];

static int colorOf[MAXV];
static vector<int> finishOrder;
static vector<int> backFrom;
static vector<int> backTo;
static int timerStep = 0;
static bool sawCycle = false;

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
  edgeCount++;
}

static void buildGraph() {
  int newN = readRange("Number of vertices (0.." + to_string(MAXV) + "): ",
                       0, MAXV);

  resetGraph();
  n = newN;

  if (n == 0) {
    cout << "Build complete: 0 vertices, 0 edges.\n";
    return;
  }
  cout << "Directed arcs only. Enter two vertex numbers in 0.." << (n - 1)
       << ".\n";
  int e = 0;
  while (true) {
    e++;
    cout << "\nArc " << e << "\n";
    int u = readInt("  From vertex (-1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To vertex (-1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      e--;
      continue;
    }
    if (matrixG[u][v]) {
      cout << "  Rejected: That arc already exists.\n";
      e--;
      continue;
    }
    addEdge(u, v);
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << edgeCount
       << (edgeCount == 1 ? " arc.\n" : " arcs.\n");
}

static void loadDemo(bool cyclic) {
  resetGraph();
  n = 6;

  if (cyclic) {
    cout << "Cyclic demo: 6 vertices.\n";
    cout << "  DAG 0 -> 1 -> 3 -> 4 -> 5 plus 0 -> 2 -> 3, then 5 -> 1.\n";
    cout << "  The arc 5 -> 1 returns to a gray vertex, so no order exists.\n\n";
    addEdge(0, 1);
    addEdge(0, 2);
    addEdge(1, 3);
    addEdge(2, 3);
    addEdge(3, 4);
    addEdge(4, 5);
    addEdge(5, 1);
  } else {
    cout << "DAG demo: 6 vertices.\n";
    cout << "  Arcs 0 -> 1, 0 -> 2, 1 -> 3, 2 -> 3, 3 -> 4, 4 -> 5.\n";
    cout << "  Vertex 5 finishes first and ends up last in the order.\n\n";
    addEdge(0, 1);
    addEdge(0, 2);
    addEdge(1, 3);
    addEdge(2, 3);
    addEdge(3, 4);
    addEdge(4, 5);
  }

  cout << "Demo loaded: 6 vertices.\n";
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
  cout << "Each line lists out-neighbours in insertion order.\n";
}

static void dfs(int u, int depth) {
  string pad(depth * 2, ' ');
  colorOf[u] = 1;
  cout << pad << "Enter V" << u << " (Gray)\n";

  for (int v : adj[u]) {
    if (colorOf[v] == 0) {
      cout << pad << "  Arc " << u << " -> " << v << ": White, descend.\n";
      dfs(v, depth + 1);
    } else if (colorOf[v] == 1) {
      sawCycle = true;
      backFrom.push_back(u);
      backTo.push_back(v);
      cout << pad << "  Arc " << u << " -> " << v << ": Gray, back arc.\n";
    } else {
      cout << pad << "  Arc " << u << " -> " << v << ": Black, skip.\n";
    }
  }

  colorOf[u] = 2;
  timerStep++;
  finishOrder.push_back(u);
  cout << pad << "Leave V" << u << " (Black), finish time " << timerStep << "\n";
}

static void runDfsTopo() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  for (int i = 0; i < MAXV; i++) colorOf[i] = 0;
  finishOrder.clear();
  backFrom.clear();
  backTo.clear();
  timerStep = 0;
  sawCycle = false;

  cout << "\n--- DFS finish order ---\n";
  for (int start = 0; start < n; start++) {
    if (colorOf[start] != 0) continue;
    cout << "\nRoot V" << start << "\n";
    dfs(start, 0);
  }

  cout << "\nFinish order:";
  for (int v : finishOrder) cout << " " << v;
  cout << "\n";

  if (sawCycle) {
    cout << (backFrom.size() == 1 ? "Back arc detected: "
                                  : "Back arcs detected: ");
    for (int i = 0; i < (int)backFrom.size(); i++) {
      if (i > 0) cout << ", ";
      cout << "V" << backFrom[i] << " -> V" << backTo[i];
    }
    cout << ". Topological order does not exist.\n";
    return;
  }

  cout << "Topological order (Finish order reversed):";
  for (int i = (int)finishOrder.size() - 1; i >= 0; i--) {
    cout << " " << finishOrder[i];
  }
  cout << "\n";

  bool ok = true;
  for (int u = 0; u < n && ok; u++) {
    for (int v : adj[u]) {
      int posU = -1;
      int posV = -1;
      int at = 0;
      for (int i = (int)finishOrder.size() - 1; i >= 0; i--) {
        if (finishOrder[i] == u) posU = at;
        if (finishOrder[i] == v) posV = at;
        at++;
      }
      if (posU > posV) ok = false;
    }
  }
  cout << "Every arc points forward in this order: " << (ok ? "Yes" : "No")
       << "\n";
}

static string edgeLabel(int u, int v) {
  return "(" + to_string(u) + " -> " + to_string(v) + ")";
}

static void addEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nAdd arc (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
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
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (matrixG[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " already exists.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }

    addEdge(u, v);
    cout << "  Added E" << (edgeCount - 1) << " " << edgeLabel(u, v) << "\n";
    cout << "    matrix[" << u << "][" << v << "] = 1\n";
    if (u == v) cout << "    A self-loop closes a 1-cycle.\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " arc" : " arcs")
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
      if (matrixG[u][v]) edgeCount++;
    }
  }
}

static void removeEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove an arc (V = " << n << ", so vertices 0.." << (n - 1)
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
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (!matrixG[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " is not in the graph.\n";
      cout << "    Arcs are ordered: " << v << " -> " << u << " is a separate"
           << " arc and is not removed.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }

    matrixG[u][v] = 0;
    removeNeighbour(u, v);
    edgeCount--;

    cout << "  Removed " << edgeLabel(u, v) << "\n";
    cout << "    matrix[" << u << "][" << v << "] = 0\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " arc" : " arcs")
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

  cout << "\nAdded V" << fresh << " with no arcs.\n";
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
  cout << "  Arcs lost: " << before - edgeCount;
  cout << " (Recounted from the matrix.)\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
}

static void clearAllEdges() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nThe graph already has no arcs.\n";
    return;
  }

  int before = edgeCount;
  for (int u = 0; u < n; u++) {
    adj[u].clear();
    for (int w = 0; w < n; w++) matrixG[u][w] = 0;
  }
  edgeCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " arc.\n" : " arcs.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
}

static void graphOperationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Directed | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Add an arc to the current graph\n";
    cout << "2. Remove an arc from the current graph\n";
    cout << "3. Add a vertex to the current graph\n";
    cout << "4. Remove a vertex from the current graph\n";
    cout << "5. Clear all arcs\n";
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
    cout << "\n=== G12: Topological Sort (DFS) ===\n";
    cout << "Graph: Directed | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build a directed graph\n";
    cout << "2. Load DAG demo (6 vertices)\n";
    cout << "3. Load cyclic demo (6 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Run DFS topological sort\n";
    cout << "6. Show adjacency list\n";
    cout << "7. Exit\n";

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
      runDfsTopo();
    } else if (choice == 6) {
      showAdjacency();
    } else if (choice == 7) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}

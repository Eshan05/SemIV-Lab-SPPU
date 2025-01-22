// 4a. BFS: A queue visits vertices in rings of equal distance.
// Demo: One graph, two scans. The matrix enqueues neighbours by number, the
// list by insertion order, so the ring contents differ.

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

static bool seen[MAXV];
static int parentOf[MAXV];
static int distOf[MAXV];
static vector<int> order;

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
    n = 5;
    cout << "Directed demo: 5 vertices.\n";
    cout << "  Arcs 0 -> 2 -> 3 -> 4 and 0 -> 1 -> 3.\n";
    cout << "  V0's out-neighbours are listed as 2 then 1.\n\n";
    addEdge(0, 2);
    addEdge(0, 1);
    addEdge(2, 3);
    addEdge(1, 3);
    addEdge(3, 4);
  } else {
    n = 7;
    cout << "Undirected demo: 7 vertices.\n";
    cout << "  Diamond 0 - 1 - 3 - 2 - 0, tail 3 - 4, separate edge 5 - 6.\n";
    cout << "  V0's neighbours are listed as 2 then 1.\n\n";
    addEdge(0, 2);
    addEdge(0, 1);
    addEdge(1, 3);
    addEdge(2, 3);
    addEdge(3, 4);
    addEdge(5, 6);
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
  cout << (directed ? "Each line lists out-neighbours in insertion order.\n"
                    : "Each line lists neighbours in insertion order.\n");
}

static string queueText(const vector<int> &q, int front) {
  if (front >= (int)q.size()) return "(Empty)";
  string s;
  for (int i = front; i < (int)q.size(); i++) {
    if (!s.empty()) s += " ";
    s += to_string(q[i]);
  }
  return s;
}

static void bfs(bool byMatrix, int s) {
  vector<int> q;
  int front = 0;
  q.push_back(s);
  seen[s] = true;
  distOf[s] = 0;
  parentOf[s] = -1;

  cout << "Enqueue V" << s << ". Queue: " << s << "\n";

  while (front < (int)q.size()) {
    int u = q[front];
    front++;
    order.push_back(u);
    cout << "Dequeue V" << u << " at distance " << distOf[u] << ".\n";

    if (byMatrix) {
      for (int v = 0; v < n; v++) {
        if (!matrixG[u][v]) continue;
        if (seen[v]) {
          cout << "  V" << v << " already seen, skip.\n";
          continue;
        }
        seen[v] = true;
        distOf[v] = distOf[u] + 1;
        parentOf[v] = u;
        q.push_back(v);
        cout << "  Enqueue V" << v << " at distance " << distOf[v] << ".\n";
      }
    } else {
      for (int v : adj[u]) {
        if (seen[v]) {
          cout << "  V" << v << " already seen, skip.\n";
          continue;
        }
        seen[v] = true;
        distOf[v] = distOf[u] + 1;
        parentOf[v] = u;
        q.push_back(v);
        cout << "  Enqueue V" << v << " at distance " << distOf[v] << ".\n";
      }
    }

    cout << "Queue now: " << queueText(q, front) << "\n";
  }
}

static void showRings() {
  int maxDist = 0;
  for (int i = 0; i < n; i++) {
    if (distOf[i] > maxDist) maxDist = distOf[i];
  }

  cout << "\nRings from the start:\n";
  for (int d = 0; d <= maxDist; d++) {
    cout << "  Distance " << d << ":";
    bool any = false;
    for (int i = 0; i < n; i++) {
      if (distOf[i] == d) {
        cout << " " << i;
        any = true;
      }
    }
    if (!any) cout << " (None)";
    cout << "\n";
  }
}

static void runBfs(bool byMatrix) {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  int s = readRange("  Start vertex (0.." + to_string(n - 1) + "): ", 0, n - 1);

  for (int i = 0; i < MAXV; i++) {
    seen[i] = false;
    parentOf[i] = -1;
    distOf[i] = -1;
  }
  order.clear();

  cout << "\n--- BFS from V" << s << " by "
       << (byMatrix ? "adjacency matrix" : "adjacency list") << " ---\n";
  bfs(byMatrix, s);

  cout << "\nVisit order:";
  for (int v : order) cout << " " << v;
  cout << "\nDistance   :";
  for (int i = 0; i < n; i++) cout << " " << distOf[i];
  cout << "\nParent     :";
  for (int i = 0; i < n; i++) cout << " " << parentOf[i];
  cout << "\n";

  showRings();

  if ((int)order.size() < n) {
    cout << "Reached " << order.size() << " of " << n
         << " vertices. Unreached:";
    for (int i = 0; i < n; i++) {
      if (!seen[i]) cout << " " << i;
    }
    cout << "\n";
  }
}

static void runCompare() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  int s = readRange("  Start vertex (0.." + to_string(n - 1) + "): ", 0, n - 1);

  for (int i = 0; i < MAXV; i++) {
    seen[i] = false;
    parentOf[i] = -1;
    distOf[i] = -1;
  }
  order.clear();
  cout << "\n=== Matrix scan ===\n";
  bfs(true, s);
  cout << "Order:";
  for (int v : order) cout << " " << v;
  cout << "\n";

  for (int i = 0; i < MAXV; i++) {
    seen[i] = false;
    parentOf[i] = -1;
    distOf[i] = -1;
  }
  order.clear();
  cout << "\n=== List scan ===\n";
  bfs(false, s);
  cout << "Order:";
  for (int v : order) cout << " " << v;
  cout << "\n";

  cout << "\nDistances agree; only the order inside a ring can differ.\n";
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
    cout << "\n=== G05: BFS ===\n";
    cout << "Graph: " << (directed ? "Directed" : "Undirected")
         << " | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build a graph\n";
    cout << "2. Load undirected demo (7 vertices)\n";
    cout << "3. Load directed demo (5 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. BFS by adjacency matrix\n";
    cout << "6. BFS by adjacency list\n";
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
      runBfs(true);
    } else if (choice == 6) {
      runBfs(false);
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

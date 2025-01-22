// 6. Bidirectional BFS: Grow a frontier from the start and one from the goal.
// Demo: A long path with one shortcut; the two fronts meet in the middle, so
// each side explores about half as many vertices as a one-sided BFS.

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

static int distF[MAXV];
static int distB[MAXV];
static int parentF[MAXV];
static int parentB[MAXV];
static int meetAt = -1;

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

static void loadDemo() {
  resetGraph();
  directed = false;
  n = 8;

  cout << "Undirected demo: 8 vertices.\n";
  cout << "  Path 0 - 1 - 2 - 3 - 4 - 5 - 6 - 7 with shortcut 3 - 6.\n";
  cout << "  Try 0 to 7: The fronts meet near the shortcut.\n\n";

  addEdge(0, 1);
  addEdge(1, 2);
  addEdge(2, 3);
  addEdge(3, 4);
  addEdge(4, 5);
  addEdge(5, 6);
  addEdge(6, 7);
  addEdge(3, 6);

  cout << "Demo loaded: 8 vertices.\n";
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
  cout << (directed ? "Each line lists out-neighbours. Backward steps need a "
                      "reverse scan.\n"
                    : "Each line lists neighbours in insertion order.\n");
}

static bool forwardEdge(int u, int v) { return matrixG[u][v] != 0; }

static bool backwardEdge(int p, int u) { return matrixG[p][u] != 0; }

static string listText(const vector<int> &q) {
  if (q.empty()) return "(Empty)";
  string s;
  for (int v : q) {
    if (!s.empty()) s += " ";
    s += to_string(v);
  }
  return s;
}

static void expandForward(vector<int> &frontier) {
  vector<int> next;
  cout << "Forward layer: " << listText(frontier) << "\n";

  for (int u : frontier) {
    for (int v = 0; v < n; v++) {
      if (!forwardEdge(u, v)) continue;
      if (distF[v] != -1) continue;
      distF[v] = distF[u] + 1;
      parentF[v] = u;
      next.push_back(v);
      cout << "  Forward reaches V" << v << " at " << distF[v] << ".\n";
      if (distB[v] != -1 && meetAt == -1) meetAt = v;
    }
  }
  frontier = next;
}

static void expandBackward(vector<int> &frontier) {
  vector<int> next;
  cout << "Backward layer: " << listText(frontier) << "\n";

  for (int u : frontier) {
    for (int p = 0; p < n; p++) {
      if (!backwardEdge(p, u)) continue;
      if (distB[p] != -1) continue;
      distB[p] = distB[u] + 1;
      parentB[p] = u;
      next.push_back(p);
      cout << "  Backward reaches V" << p << " at " << distB[p] << ".\n";
      if (distF[p] != -1 && meetAt == -1) meetAt = p;
    }
  }
  frontier = next;
}

static void runBidirectional() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  int s = readRange("  Start vertex (0.." + to_string(n - 1) + "): ", 0, n - 1);
  int t = readRange("  Goal vertex (0.." + to_string(n - 1) + "): ", 0, n - 1);

  if (s == t) {
    cout << "Start and goal are the same vertex; path length 0.\n";
    return;
  }

  for (int i = 0; i < MAXV; i++) {
    distF[i] = -1;
    distB[i] = -1;
    parentF[i] = -1;
    parentB[i] = -1;
  }
  meetAt = -1;

  distF[s] = 0;
  distB[t] = 0;

  vector<int> frontF;
  vector<int> frontB;
  frontF.push_back(s);
  frontB.push_back(t);

  cout << "\n--- Bidirectional BFS from V" << s << " to V" << t << " ---\n";

  int rounds = 0;
  while (!frontF.empty() && !frontB.empty() && meetAt == -1) {
    rounds++;
    cout << "\nRound " << rounds << "\n";
    if ((int)frontF.size() <= (int)frontB.size()) {
      expandForward(frontF);
    } else {
      expandBackward(frontB);
    }
    if (meetAt == -1 && frontF.empty()) {
      cout << "Forward frontier is empty.\n";
    }
    if (meetAt == -1 && frontB.empty()) {
      cout << "Backward frontier is empty.\n";
    }
  }

  if (meetAt == -1) {
    cout << "\nNo path from V" << s << " to V" << t << ".\n";
    return;
  }

  vector<int> path;
  int cur = meetAt;
  while (cur != -1) {
    path.push_back(cur);
    cur = parentF[cur];
  }
  int left = 0;
  int right = (int)path.size() - 1;
  while (left < right) {
    int tmp = path[left];
    path[left] = path[right];
    path[right] = tmp;
    left++;
    right--;
  }

  cur = parentB[meetAt];
  while (cur != -1) {
    path.push_back(cur);
    cur = parentB[cur];
  }

  cout << "\nThe fronts met at V" << meetAt << ".\n";
  cout << "Path:";
  for (int v : path) cout << " " << v;
  cout << "\nLength: " << path.size() - 1 << " edges.\n";

  int seenF = 0;
  int seenB = 0;
  for (int i = 0; i < n; i++) {
    if (distF[i] != -1) seenF++;
    if (distB[i] != -1) seenB++;
  }
  cout << "Vertices touched: " << seenF << " forward, " << seenB
       << " backward, of " << n << ".\n";
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
    cout << "\n=== G07: Bidirectional BFS ===\n";
    cout << "Graph: " << (directed ? "Directed" : "Undirected")
         << " | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build a graph\n";
    cout << "2. Load demo (8 vertices)\n";
    cout << "3. Graph operations\n";
    cout << "4. Bidirectional BFS\n";
    cout << "5. Show adjacency list\n";
    cout << "6. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo();
    } else if (choice == 3) {
      graphOperationsMenu();
    } else if (choice == 4) {
      runBidirectional();
    } else if (choice == 5) {
      showAdjacency();
    } else if (choice == 6) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}

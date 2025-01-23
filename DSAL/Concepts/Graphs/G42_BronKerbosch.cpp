// 30a. Maximal cliques, found with Bron-Kerbosch: Grow one clique in R, keep the
// still possible vertices in P, and the ones already ruled out in X. A call with
// both P and X empty has a clique that cannot grow. Demo: A K4, a triangle and a
// loose edge, where the four largest vertices form one clique and the triangle is
// maximal without being maximum.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;
const int TRACE_LIMIT = 60;
const int STORE_MAX = 200;

static int n = 0;
static int edgeCount = 0;
static bool adjM[MAXV][MAXV];
static vector<int> adj[MAXV];

static bool inP[MAXV];
static bool inX[MAXV];
static vector<int> cliqueR;

static int foundCliques = 0;
static int calls = 0;
static int skippedByPivot = 0;
static int shown = 0;

static int maxSize = 0;
static vector<int> maxClique;
static int maxTies = 0;

static int cliqueStore[STORE_MAX][MAXV];
static int storeSize[STORE_MAX];
static int storedCliques = 0;

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
    for (int j = 0; j < MAXV; j++) adjM[i][j] = false;
  }
  n = 0;
  edgeCount = 0;
}

static void addEdge(int u, int v) {
  adjM[u][v] = true;
  adjM[v][u] = true;
  adj[u].push_back(v);
  adj[v].push_back(u);
  edgeCount++;
}

static string edgeLabel(int u, int v) {
  return "(" + to_string(u) + " - " + to_string(v) + ")";
}

static string plural(int count, const string &one, const string &many) {
  return to_string(count) + " " + (count == 1 ? one : many);
}

static int degreeOf(int v) {
  int total = 0;
  for (int w = 0; w < n; w++) {
    if (adjM[v][w]) total++;
  }
  return total;
}

static string setText(const bool set[]) {
  string text = "{";
  bool first = true;
  for (int v = 0; v < n; v++) {
    if (!set[v]) continue;
    if (!first) text += " ";
    text += to_string(v);
    first = false;
  }
  if (first) return "(Empty)";
  return text + "}";
}

static string listText(const vector<int> &items) {
  string text = "{";
  for (int i = 0; i < (int)items.size(); i++) {
    if (i > 0) text += " ";
    text += to_string(items[i]);
  }
  return text + "}";
}

static bool anyIn(const bool set[]) {
  for (int v = 0; v < n; v++) {
    if (set[v]) return true;
  }
  return false;
}

static int neighboursInP(int v, const bool P[]) {
  int total = 0;
  for (int u = 0; u < n; u++) {
    if (P[u] && adjM[v][u]) total++;
  }
  return total;
}

static void reportClique() {
  foundCliques++;
  int size = (int)cliqueR.size();

  if (storedCliques < STORE_MAX) {
    for (int i = 0; i < size; i++) cliqueStore[storedCliques][i] = cliqueR[i];
    storeSize[storedCliques] = size;
    storedCliques++;
  }

  if (size > maxSize) {
    maxSize = size;
    maxClique = cliqueR;
    maxTies = 1;
  } else if (size == maxSize) {
    maxTies++;
  }
}

static void printSets(const bool P[], const bool X[]) {
  cout << "R = " << listText(cliqueR) << ", P = " << setText(P)
       << ", X = " << setText(X) << "\n";
}

static void traceSay(const string &text) {
  if (shown >= TRACE_LIMIT) return;
  shown++;
  cout << text << "\n";
}

static void bronKerbosch(bool P[MAXV], bool X[MAXV], int depth) {
  calls++;
  string pad(depth * 2, ' ');

  if (shown < TRACE_LIMIT) {
    cout << pad << "Call " << calls << ": ";
    printSets(P, X);
    shown++;
  }

  if (!anyIn(P) && !anyIn(X)) {
    traceSay(pad + "  P and X are empty, so R cannot grow: " +
             listText(cliqueR) + " is maximal.");
    reportClique();
    return;
  }

  int pivot = -1;
  int pivotCover = -1;
  for (int v = 0; v < n; v++) {
    if (!P[v] && !X[v]) continue;
    int cover = neighboursInP(v, P);
    if (cover > pivotCover) {
      pivotCover = cover;
      pivot = v;
    }
  }

  int candidates = 0;
  for (int v = 0; v < n; v++) {
    if (P[v]) candidates++;
  }
  int toTry = candidates - pivotCover;
  skippedByPivot += pivotCover;

  traceSay(pad + "  Pivot V" + to_string(pivot) + " is joined to " +
           to_string(pivotCover) + " of the " +
           plural(candidates, "vertex", "vertices") + " in P, so try " +
           to_string(toTry) + " of them.");

  for (int v = 0; v < n; v++) {
    if (!P[v]) continue;
    if (adjM[v][pivot]) continue;

    bool nextP[MAXV];
    bool nextX[MAXV];
    for (int u = 0; u < n; u++) {
      nextP[u] = P[u] && adjM[v][u];
      nextX[u] = X[u] && adjM[v][u];
    }

    cliqueR.push_back(v);
    traceSay(pad + "  Add V" + to_string(v) +
             ", since it is not covered by the pivot.");
    bronKerbosch(nextP, nextX, depth + 1);
    cliqueR.pop_back();

    P[v] = false;
    X[v] = true;
    traceSay(pad + "  Done with V" + to_string(v) +
             ", so it moves from P to X.");
  }
}

static void runBronKerbosch() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nWith no edges every single vertex is a clique of its own, and no\n";
    cout << "clique has a second member. Add an edge first.\n";
    return;
  }

  foundCliques = 0;
  calls = 0;
  skippedByPivot = 0;
  shown = 0;
  maxSize = 0;
  maxTies = 0;
  maxClique.clear();
  cliqueR.clear();
  storedCliques = 0;
  for (int v = 0; v < n; v++) {
    inP[v] = true;
    inX[v] = false;
  }

  cout << "\n--- Bron-Kerbosch ---\n";
  cout << "R holds the clique being built, P the vertices that could still join\n";
  cout << "it, and X the ones that joined an earlier branch already, so a clique\n";
  cout << "touching X is not new. When P and X are both empty, R is maximal.\n";
  cout << "The pivot is the vertex touching the most of P: Every candidate it\n";
  cout << "touches comes up inside that branch anyway, so it is skipped here.\n\n";

  bool P[MAXV];
  bool X[MAXV];
  for (int v = 0; v < n; v++) {
    P[v] = inP[v];
    X[v] = inX[v];
  }
  bronKerbosch(P, X, 0);

  if (shown >= TRACE_LIMIT) {
    cout << "\n(Trace stops after " << TRACE_LIMIT
         << " lines. The search itself ran to the end.)\n";
  }

  cout << "\n--- Maximal cliques ---\n";
  cout << "Found " << plural(foundCliques, "maximal clique", "maximal cliques")
       << " in " << plural(calls, "call", "calls") << ".\n";
  cout << "Candidates skipped by the pivot: " << skippedByPivot << ".\n";
  cout << "\nEvery maximal clique, in the order the search found it:\n";
  for (int c = 0; c < storedCliques; c++) {
    cout << "  " << (c + 1) << ". {";
    for (int i = 0; i < storeSize[c]; i++) {
      if (i > 0) cout << " ";
      cout << cliqueStore[c][i];
    }
    cout << "} (Size " << storeSize[c] << ")\n";
  }
  if (foundCliques > storedCliques) {
    cout << "  (Only the first " << storedCliques << " fit in the list.)\n";
  }

  cout << "\nMaximum clique: " << listText(maxClique) << " (Size " << maxSize
       << ")\n";
  if (maxTies > 1) {
    cout << maxTies << " maximal cliques reach that size, so the answer is not\n";
    cout << "unique.\n";
  }
  int smallerStored = 0;
  for (int c = 0; c < storedCliques; c++) {
    if (storeSize[c] < maxSize) smallerStored++;
  }
  if (maxSize < n) {
    cout << "\nMaximal is not the same as maximum: A clique is maximal when no\n";
    cout << "vertex outside it is joined to all of its members, and maximum when\n";
    cout << "no clique in the graph is larger.\n";
    if (smallerStored > 0) {
      cout << "The " << smallerStored << " smaller maximal cliques here are maximal\n";
      cout << "and still lose to size " << maxSize << ".\n";
    } else if (foundCliques == storedCliques) {
      cout << "Here every maximal clique has the same size, so none of them is\n";
      cout << "smaller than the maximum.\n";
    }
  } else {
    cout << "Every vertex is in that clique, so the graph is complete and it has\n";
    cout << "exactly one maximal clique.\n";
  }
  cout << "\nA graph on n vertices can have as many as 3 to the n/3 power maximal\n";
  cout << "cliques, so the search is exponential in the worst case even with the\n";
  cout << "pivot.\n";
}

static void showMatrix() {
  cout << "\n--- Adjacency matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  cout << "      ";
  for (int j = 0; j < n; j++) cout << setw(4) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << setw(4) << i << "  ";
    for (int j = 0; j < n; j++) cout << setw(4) << (adjM[i][j] ? 1 : 0);
    cout << "\n";
  }
  cout << "The table is symmetric, and the diagonal is 0 because a self-loop is\n";
  cout << "rejected: A clique needs distinct vertices.\n";
}

static void showAdjacency() {
  cout << "\n--- Adjacency list ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  for (int i = 0; i < n; i++) {
    cout << setw(3) << i << ":";
    if (adj[i].empty()) cout << " (None)";
    else
      for (int v : adj[i]) cout << " " << v;
    cout << "   (Degree " << degreeOf(i) << ")\n";
  }
  cout << "Every line lists one vertex and the vertices joined to it.\n";
}

static void buildGraph() {
  int fresh = readRange("Number of vertices (0.." + to_string(MAXV) + "): ", 0,
                        MAXV);
  resetGraph();
  n = fresh;

  if (n == 0) {
    cout << "Build complete: 0 vertices, 0 edges.\n";
    return;
  }
  cout << "Undirected edges only, vertices 0.." << (n - 1) << ".\n";

  while (true) {
    int limit = n * (n - 1) / 2;
    cout << "\nEdge " << (edgeCount + 1) << " (Limit " << limit
         << ", -1 to stop)\n";
    if (edgeCount == limit) {
      cout << "The graph is complete, so every pair is already joined.\n";
      break;
    }
    int u = readInt("  From vertex (0.." + to_string(n - 1) + ", -1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To vertex (0.." + to_string(n - 1) + ", -1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: A vertex cannot be joined to itself in a clique.\n";
      continue;
    }
    if (adjM[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " already exists.\n";
      continue;
    }
    addEdge(u, v);
    cout << "  Added E" << (edgeCount - 1) << " " << edgeLabel(u, v) << "\n";
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << edgeCount
       << (edgeCount == 1 ? " edge.\n" : " edges.\n");
}

static void loadMainDemo() {
  resetGraph();
  n = 7;
  cout << "Mixed demo: 7 vertices, 10 edges.\n";
  cout << "  K4 on {0, 1, 2, 3}, triangle {3, 4, 5}, and the loose edge 5 - 6.\n";
  cout << "  Three maximal cliques: Size 4, size 3 and size 2. Only one of them\n";
  cout << "  is maximum.\n\n";

  addEdge(0, 1);
  addEdge(0, 2);
  addEdge(0, 3);
  addEdge(1, 2);
  addEdge(1, 3);
  addEdge(2, 3);
  addEdge(3, 4);
  addEdge(4, 5);
  addEdge(3, 5);
  addEdge(5, 6);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadCompleteDemo() {
  resetGraph();
  n = 5;
  cout << "Complete demo: 5 vertices, 10 edges.\n";
  cout << "  Every pair is joined, so the whole vertex set is the one maximal\n";
  cout << "  clique and it is also the maximum.\n\n";

  for (int u = 0; u < n; u++) {
    for (int v = u + 1; v < n; v++) addEdge(u, v);
  }

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadSparseDemo() {
  resetGraph();
  n = 6;
  cout << "Sparse demo: 6 vertices, 6 edges, a ring with no triangle.\n";
  cout << "  Every clique holds at most two vertices, so each edge is a maximal\n";
  cout << "  clique of size 2 and the maximum is only 2.\n\n";

  for (int v = 0; v < n; v++) addEdge(v, (v + 1) % n);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void addEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == n * (n - 1) / 2) {
    cout << "The graph is already complete, so every pair is joined.\n";
    return;
  }

  cout << "\nAdd edge (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
  while (true) {
    int u = readInt("  From vertex (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To vertex (0.." + to_string(n - 1) + ", -1 to cancel): ");
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
      cout << "  Rejected: A vertex cannot be joined to itself in a clique.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (adjM[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " already exists.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    addEdge(u, v);
    cout << "  Added E" << (edgeCount - 1) << " " << edgeLabel(u, v) << "\n";
    cout << "    matrix[" << u << "][" << v << "] = 1 and matrix[" << v << "]["
         << u << "] = 1\n";
    cout << "    adj[" << u << "] and adj[" << v << "] each gained a neighbour.\n";
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

static void removeEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove an edge (V = " << n << ", so vertices 0.." << (n - 1)
       << ")\n";
  while (true) {
    int u = readInt("  From vertex (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To vertex (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (!adjM[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " is not in the graph.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    adjM[u][v] = false;
    adjM[v][u] = false;
    removeNeighbour(u, v);
    removeNeighbour(v, u);
    edgeCount--;

    cout << "  Removed " << edgeLabel(u, v) << "\n";
    cout << "    matrix[" << u << "][" << v << "] = 0 and matrix[" << v << "]["
         << u << "] = 0\n";
    cout << "    adj[" << u << "] and adj[" << v << "] each lost a neighbour.\n";
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
    adjM[fresh][i] = false;
    adjM[i][fresh] = false;
  }
  adj[fresh].clear();
  n++;

  cout << "\nAdded V" << fresh << " with no edges.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
  cout << "  A vertex with no edge is a maximal clique of size 1.\n";
}

static void removeVertexInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  int v;
  while (true) {
    v = readInt("  Vertex to remove (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (v >= 0 && v < n) break;
    cout << "  Rejected: Vertex must be in 0.." << (n - 1) << ".\n";
  }

  vector<int> keptFrom;
  vector<int> keptTo;
  for (int u = 0; u < n; u++) {
    if (u == v) continue;
    for (int w = u + 1; w < n; w++) {
      if (w == v) continue;
      if (!adjM[u][w]) continue;
      keptFrom.push_back(u < v ? u : u - 1);
      keptTo.push_back(w < v ? w : w - 1);
    }
  }

  int before = edgeCount;
  int newN = n - 1;
  resetGraph();
  n = newN;

  for (int i = 0; i < (int)keptFrom.size(); i++) addEdge(keptFrom[i], keptTo[i]);

  cout << "\nRemoved V" << v << ".\n";
  if (v < n) cout << "  Vertices above V" << v << " shift down by one.\n";
  cout << "  Edges lost: " << before - edgeCount << ".\n";
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
    for (int w = 0; w < n; w++) adjM[u][w] = false;
  }
  edgeCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
  cout << "  With no edges every vertex is its own maximal clique.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Undirected | V = " << n << ", E = " << edgeCount << "\n";
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
    cout << "\n=== G42: Maximal Cliques (Bron-Kerbosch) ===\n";
    cout << "Graph: Undirected | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build an undirected graph\n";
    cout << "2. Load the mixed demo (7 vertices)\n";
    cout << "3. Load the complete demo (5 vertices)\n";
    cout << "4. Load the sparse demo (6 vertices)\n";
    cout << "5. Graph operations\n";
    cout << "6. Run Bron-Kerbosch\n";
    cout << "7. Show the adjacency matrix\n";
    cout << "8. Show the adjacency list\n";
    cout << "9. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadMainDemo();
    } else if (choice == 3) {
      loadCompleteDemo();
    } else if (choice == 4) {
      loadSparseDemo();
    } else if (choice == 5) {
      operationsMenu();
    } else if (choice == 6) {
      runBronKerbosch();
    } else if (choice == 7) {
      showMatrix();
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

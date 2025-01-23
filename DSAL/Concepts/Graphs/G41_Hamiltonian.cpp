// 29. Hamiltonian path and circuit, found by backtracking: Put a vertex in the
// next position, and when the path cannot be finished, take it out again. A
// cheap prune kills a branch as soon as an unvisited vertex can never be
// entered. Demo: A graph with a Hamiltonian path but no circuit, because its
// two sides hold two and three vertices.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;
const int STEP_LIMIT = 20000;
const int TRACE_LIMIT = 40;

static int n = 0;
static int edgeCount = 0;
static int matrixG[MAXV][MAXV];
static vector<int> adj[MAXV];

static int hamPath[MAXV];
static int answerPath[MAXV];
static int answerLen = 0;
static bool onPath[MAXV];
static int hamLen = 0;
static int searchStart = 0;
static bool wantCircuit = false;
static bool found = false;
static bool aborted = false;
static int steps = 0;
static int deadEnds = 0;
static int prunes = 0;
static int traceShown = 0;

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
  matrixG[v][u] = 1;
  adj[u].push_back(v);
  adj[v].push_back(u);
  edgeCount++;
}

static string edgeLabel(int u, int v) {
  return "(" + to_string(u) + " - " + to_string(v) + ")";
}

static int degreeOf(int v) {
  int total = 0;
  for (int w = 0; w < n; w++) total += matrixG[v][w];
  return total;
}

static string pathText() {
  string text;
  for (int i = 0; i < hamLen; i++) {
    if (i > 0) text += " - ";
    text += to_string(hamPath[i]);
  }
  return text;
}

static string answerText() {
  string text;
  for (int i = 0; i < answerLen; i++) {
    if (i > 0) text += " - ";
    text += to_string(answerPath[i]);
  }
  return text;
}

static void traceLine(int depth, const string &text) {
  if (traceShown >= TRACE_LIMIT) return;
  traceShown++;
  cout << string(depth * 2, ' ') << text << "\n";
}

static bool hasUnvisitedNeighbour(int w) {
  for (int x = 0; x < n; x++) {
    if (matrixG[w][x] && !onPath[x]) return true;
  }
  return false;
}

static bool pruneHere(int endVertex, string &why) {
  int lastOnly = 0;
  int lastVertex = -1;
  for (int w = 0; w < n; w++) {
    if (onPath[w]) continue;
    if (hasUnvisitedNeighbour(w)) continue;
    lastOnly++;
    lastVertex = w;
  }

  if (lastOnly >= 2) {
    why = "Prune: " + to_string(lastOnly) + " unvisited vertices have no";
    why += " unvisited neighbour, and only one vertex can be last.";
    return true;
  }
  if (lastOnly == 1 && !matrixG[lastVertex][endVertex]) {
    why = "Prune: V" + to_string(lastVertex) + " has no unvisited neighbour,";
    why += " so only V" + to_string(endVertex) + " could lead into it,";
    why += " and those two do not touch.";
    return true;
  }
  return false;
}

static void dfsHam(int depth) {
  if (found || aborted) return;

  steps++;
  if (steps > STEP_LIMIT) {
    aborted = true;
    return;
  }

  int u = hamPath[hamLen - 1];

  if (hamLen == n) {
    if (!wantCircuit || matrixG[u][searchStart]) {
      found = true;
      answerLen = hamLen;
      for (int i = 0; i < hamLen; i++) answerPath[i] = hamPath[i];
      return;
    }
    deadEnds++;
    traceLine(depth, "Full order " + pathText() + ", but V" + to_string(u) +
                        " does not touch V" + to_string(searchStart) +
                        ", so it is not a circuit.");
    return;
  }

  string why;
  if (pruneHere(u, why)) {
    prunes++;
    traceLine(depth, why);
    return;
  }

  for (int v = 0; v < n && !found && !aborted; v++) {
    if (!matrixG[u][v]) continue;
    if (onPath[v]) {
      traceLine(depth, "Edge " + to_string(u) + " - " + to_string(v) +
                          ": V" + to_string(v) + " is already in the path.");
      continue;
    }

    traceLine(depth, "Try edge " + to_string(u) + " - " + to_string(v) +
                        ", position " + to_string(hamLen + 1) + " of " +
                        to_string(n) + ".");
    onPath[v] = true;
    hamPath[hamLen] = v;
    hamLen++;
    traceLine(depth + 1, "Enter V" + to_string(v) + ", path " + pathText() +
                             ".");
    dfsHam(depth + 1);
    hamLen--;
    onPath[v] = false;
  }

  if (!found && !aborted) {
    deadEnds++;
    traceLine(depth, "Dead end at V" + to_string(u) + ": No unvisited" +
                        " neighbour can be entered from here.");
  }
}

static void runSearch(bool circuitMode) {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (circuitMode && n < 3) {
    cout << "\nA circuit needs at least three vertices, so there is nothing to\n";
    cout << "search here.\n";
    return;
  }

  wantCircuit = circuitMode;
  found = false;
  aborted = false;
  steps = 0;
  deadEnds = 0;
  prunes = 0;
  traceShown = 0;
  hamLen = 0;
  answerLen = 0;
  for (int v = 0; v < MAXV; v++) onPath[v] = false;

  cout << "\n--- Hamiltonian " << (circuitMode ? "circuit" : "path")
       << " search ---\n";
  cout << "Positions are filled left to right. An edge to a vertex already in\n";
  cout << "the path is skipped, and a branch that cannot be finished is taken\n";
  cout << "back. Neighbours are tried in increasing order.\n";
  if (circuitMode) {
    cout << "A circuit is a cycle, so the search starts at V0 once: Every other\n";
    cout << "start is a rotation of a walk that already started there.\n";
  }

  int lastStart = circuitMode ? 1 : n;
  for (int s = 0; s < lastStart && !found && !aborted; s++) {
    searchStart = s;
    hamPath[0] = s;
    hamLen = 1;
    onPath[s] = true;
    traceLine(0, "Start at V" + to_string(s) + ", position 1 of " +
                     to_string(n) + ", path " + pathText() + ".");
    dfsHam(0);
    hamLen = 0;
    onPath[s] = false;
    if (!circuitMode && !found && !aborted) {
      traceLine(0, "No path starts at V" + to_string(s) + ".");
    }
  }

  cout << "\n--- Result ---\n";
  if (aborted) {
    cout << "Stopped after " << STEP_LIMIT << " steps without an answer.\n";
    cout << "The search space grows like n!, so a budget keeps the run short.\n";
  } else if (found) {
    if (circuitMode) {
      cout << "Circuit: " << answerText() << " - " << answerPath[0] << "\n";
      cout << "Each of the " << n << " vertices appears once, and V"
           << answerPath[answerLen - 1] << " touches V" << answerPath[0]
           << " to close it.\n";
    } else {
      cout << "Path   : " << answerText() << "\n";
      cout << "Each of the " << n << " vertices appears once, and every pair of\n";
      cout << "neighbours on the line is joined by an edge.\n";
    }
  } else {
    cout << "No Hamiltonian " << (circuitMode ? "circuit" : "path")
         << " exists.\n";
    if (circuitMode) {
      cout << "Every order from V0 was tried, and each one either left a vertex\n";
      cout << "out or could not return to V0.\n";
    } else {
      cout << "Every start and every order was tried, so none visits all " << n
           << " vertices exactly once.\n";
    }
    if (circuitMode) {
      int lowDegree = -1;
      for (int v = 0; v < n; v++) {
        if (degreeOf(v) < 2) lowDegree = v;
      }
      if (lowDegree != -1) {
        cout << "V" << lowDegree << " has degree " << degreeOf(lowDegree)
             << ", and a circuit must enter and leave it.\n";
      }
    }
    int dead = -1;
    for (int v = 0; v < n; v++) {
      if (degreeOf(v) == 0) dead = v;
    }
    if (dead != -1) cout << "V" << dead << " has no edge at all.\n";
  }

  cout << "\nSteps: " << steps << ", dead ends: " << deadEnds
       << ", prunes: " << prunes << ".\n";
  if (traceShown >= TRACE_LIMIT) {
    cout << "(Trace stops after " << TRACE_LIMIT
         << " lines. The search itself ran to the end.)\n";
  }
  cout << "One step is one vertex placed, so the work grows like n! in the\n";
  cout << "worst case, and the prune only removes branches that cannot finish.\n";
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
    for (int j = 0; j < n; j++) cout << setw(4) << matrixG[i][j];
    cout << "\n";
  }
  cout << "The table is symmetric, because the graph is undirected. A 1 means\n";
  cout << "the pair is joined, and the diagonal is 0 because self-loops are\n";
  cout << "rejected here.\n";
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
    cout << "\n";
  }
  cout << "Neighbours appear in insertion order, and the search reads them in\n";
  cout << "increasing vertex order instead.\n";
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
  cout << "A self-loop is rejected: A vertex cannot appear twice in a row.\n";

  while (true) {
    cout << "\nEdge " << (edgeCount + 1) << " (Limit " << (n * (n - 1) / 2)
         << ", -1 to stop)\n";
    int u = readInt("  From vertex (-1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To vertex (-1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: A self-loop cannot be a step in a path.\n";
      continue;
    }
    if (matrixG[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " already exists.\n";
      continue;
    }
    addEdge(u, v);
    cout << "  Added E" << (edgeCount - 1) << " " << edgeLabel(u, v) << "\n";
    cout << "  Degrees: V" << u << " = " << degreeOf(u) << ", V" << v << " = "
         << degreeOf(v) << "\n";
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << edgeCount
       << (edgeCount == 1 ? " edge.\n" : " edges.\n");
}

static void loadPathOnlyDemo() {
  resetGraph();
  n = 5;
  cout << "Path-only demo: 5 vertices, 6 edges.\n";
  cout << "  Sides {0, 1} and {2, 3, 4}, with every pair across the cut joined.\n";
  cout << "  A path can walk 2 - 0 - 3 - 1 - 4, but a circuit would need the two\n";
  cout << "  sides to be the same size, and they hold two and three vertices.\n\n";

  addEdge(0, 2);
  addEdge(0, 3);
  addEdge(0, 4);
  addEdge(1, 2);
  addEdge(1, 3);
  addEdge(1, 4);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadNoPathDemo() {
  resetGraph();
  n = 4;
  cout << "No-path demo: 4 vertices, 3 edges, a star around V0.\n";
  cout << "  V0 joins V1, V2 and V3. A path reaches V0 once, so it can leave it\n";
  cout << "  only once, and two leaves stay unvisited.\n\n";

  addEdge(0, 1);
  addEdge(0, 2);
  addEdge(0, 3);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadCircuitDemo() {
  resetGraph();
  n = 7;
  cout << "Circuit demo: 7 vertices, 8 edges.\n";
  cout << "  Ring 0 - 1 - 2 - 3 - 4 - 5 - 6 - 0, plus the chord 0 - 3.\n";
  cout << "  The ring alone is a Hamiltonian circuit, so the first attempt finds\n";
  cout << "  one without any backtracking.\n\n";

  addEdge(0, 1);
  addEdge(1, 2);
  addEdge(2, 3);
  addEdge(3, 4);
  addEdge(4, 5);
  addEdge(5, 6);
  addEdge(6, 0);
  addEdge(0, 3);

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
      cout << "  Rejected: A self-loop cannot be a step in a path.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (matrixG[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " already exists.\n";
      cout << "    Undirected edges are unordered, so " << u << " - " << v
           << " is " << v << " - " << u << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    addEdge(u, v);
    cout << "  Added E" << (edgeCount - 1) << " " << edgeLabel(u, v) << "\n";
    cout << "    matrix[" << u << "][" << v << "] = 1 and matrix[" << v << "]["
         << u << "] = 1\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
         << " now. Degrees: V" << u << " = " << degreeOf(u) << ", V" << v
         << " = " << degreeOf(v) << "\n";
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
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    matrixG[u][v] = 0;
    matrixG[v][u] = 0;
    removeNeighbour(u, v);
    removeNeighbour(v, u);
    edgeCount--;

    cout << "  Removed " << edgeLabel(u, v) << "\n";
    cout << "    matrix[" << u << "][" << v << "] = 0 and matrix[" << v << "]["
         << u << "] = 0\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
         << " now. Degrees: V" << u << " = " << degreeOf(u) << ", V" << v
         << " = " << degreeOf(v) << "\n";
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
  cout << "  Its degree is 0, so no path can include it.\n";
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

  edgeCount = 0;
  for (int a = 0; a < n; a++) {
    for (int b = a + 1; b < n; b++) {
      if (matrixG[a][b]) edgeCount++;
    }
  }

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
  cout << "  With no edges, only a single vertex is a path.\n";
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
    cout << "\n=== G41: Hamiltonian Path and Circuit ===\n";
    cout << "Graph: Undirected | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build an undirected graph\n";
    cout << "2. Load the path-only demo (5 vertices)\n";
    cout << "3. Load the no-path demo (4 vertices)\n";
    cout << "4. Load the circuit demo (7 vertices)\n";
    cout << "5. Graph operations\n";
    cout << "6. Find a Hamiltonian path\n";
    cout << "7. Find a Hamiltonian circuit\n";
    cout << "8. Show the adjacency matrix\n";
    cout << "9. Show the adjacency list\n";
    cout << "10. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadPathOnlyDemo();
    } else if (choice == 3) {
      loadNoPathDemo();
    } else if (choice == 4) {
      loadCircuitDemo();
    } else if (choice == 5) {
      operationsMenu();
    } else if (choice == 6) {
      runSearch(false);
    } else if (choice == 7) {
      runSearch(true);
    } else if (choice == 8) {
      showMatrix();
    } else if (choice == 9) {
      showAdjacency();
    } else if (choice == 10) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}

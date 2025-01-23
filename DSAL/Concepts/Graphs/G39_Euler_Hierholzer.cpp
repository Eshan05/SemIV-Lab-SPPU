// 28a. Eulerian path and circuit, found with Hierholzer: Walk from the start
// until no unused edge leaves the current vertex, then back out of that stuck
// walk, and every detour lands in its rightful place. Degrees decide yes or no
// before the walk starts. Demo: Two banks, two islands and seven bridges, where
// all four degrees are odd, so no walk can cross every bridge once.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;
const int MAXE = 120;
const int TRACE_LIMIT = 60;

static int n = 0;
static int edgeCount = 0;
static int mult[MAXV][MAXV];
static int fromE[MAXE];
static int toE[MAXE];

static int leftG[MAXV][MAXV];

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
    for (int j = 0; j < MAXV; j++) mult[i][j] = 0;
  }
  n = 0;
  edgeCount = 0;
}

static int degreeOf(int v) {
  int total = 0;
  for (int w = 0; w < n; w++) total += mult[v][w];
  return total + mult[v][v];
}

static void addEdge(int u, int v) {
  int id = edgeCount;
  fromE[id] = u;
  toE[id] = v;
  mult[u][v]++;
  if (u != v) mult[v][u]++;
  edgeCount++;
}

static string edgeLabelOf(int id) {
  return "E" + to_string(id) + " (" + to_string(fromE[id]) + " - " +
         to_string(toE[id]) + ")";
}

static void removeCopyOfId(int id) {
  int u = fromE[id];
  int v = toE[id];
  mult[u][v]--;
  if (u != v) mult[v][u]--;
  for (int k = id; k < edgeCount - 1; k++) {
    fromE[k] = fromE[k + 1];
    toE[k] = toE[k + 1];
  }
  edgeCount--;
}

static void printDegreeTable() {
  cout << "  Vertex :";
  for (int v = 0; v < n; v++) cout << setw(4) << v;
  cout << "\n  Degree :";
  for (int v = 0; v < n; v++) cout << setw(4) << degreeOf(v);
  cout << "\n";
}

static bool edgesConnected() {
  int start = -1;
  for (int v = 0; v < n; v++) {
    if (degreeOf(v) > 0) {
      start = v;
      break;
    }
  }
  if (start == -1) return true;

  bool seen[MAXV] = {false};
  vector<int> stackV;
  stackV.push_back(start);
  seen[start] = true;
  while (!stackV.empty()) {
    int u = stackV.back();
    stackV.pop_back();
    for (int w = 0; w < n; w++) {
      if (mult[u][w] == 0 || seen[w]) continue;
      seen[w] = true;
      stackV.push_back(w);
    }
  }

  for (int v = 0; v < n; v++) {
    if (degreeOf(v) > 0 && !seen[v]) return false;
  }
  return true;
}

static int classify() {
  cout << "\n--- Degree check ---\n";
  printDegreeTable();

  vector<int> odd;
  for (int v = 0; v < n; v++) {
    if (degreeOf(v) % 2 == 1) odd.push_back(v);
  }
  cout << "  Odd    :";
  if (odd.empty()) cout << " (None)";
  for (int i = 0; i < (int)odd.size(); i++) cout << " V" << odd[i];
  cout << "\n";

  if (!edgesConnected()) {
    cout << "\nAnswer: No Euler path.\n";
    cout << "The edges are not all in one component, and one walk cannot jump\n";
    cout << "from a component to another.\n";
    return 0;
  }

  if (odd.empty()) {
    cout << "\nAnswer: Euler circuit.\n";
    cout << "Every degree is even, so each visit can both arrive and leave. The\n";
    cout << "walk may begin anywhere and it ends where it began.\n";
    return 2;
  }

  if ((int)odd.size() == 2) {
    cout << "\nAnswer: Euler path, but no circuit.\n";
    cout << "Only V" << odd[0] << " and V" << odd[1]
         << " are odd, so they must be the two ends.\n";
    cout << "Every other vertex has an even degree, so it can be entered and left\n";
    cout << "in pairs.\n";
    return 1;
  }

  cout << "\nAnswer: No Euler path.\n";
  cout << (int)odd.size() << " vertices are odd, but a walk has only two ends.\n";
  cout << "Each odd vertex has to be an end, so at most two of them can be odd.\n";
  return 0;
}

static int pickStart(int verdict) {
  for (int v = 0; v < n; v++) {
    if (degreeOf(v) == 0) continue;
    if (verdict == 2) return v;
    if (degreeOf(v) % 2 == 1) return v;
  }
  return -1;
}

static void printStack(const vector<int> &stackV) {
  cout << "    Stack, bottom on the left: [";
  for (int i = 0; i < (int)stackV.size(); i++) {
    if (i > 0) cout << " ";
    cout << stackV[i];
  }
  if (stackV.empty()) cout << "Empty";
  cout << "]\n";
}

static void printTour(const vector<int> &tour) {
  cout << "  ";
  for (int i = (int)tour.size() - 1; i >= 0; i--) {
    cout << (i == (int)tour.size() - 1 ? "" : " - ") << tour[i];
  }
  cout << "\n";
}

static void runHierholzer() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nThe graph has no edges, so there is nothing to walk.\n";
    cout << "Add an edge first.\n";
    return;
  }

  int verdict = classify();
  if (verdict == 0) {
    cout << "\nHierholzer is not started, because no walk can use every edge\n";
    cout << "exactly once. Remove or add an edge to change a degree.\n";
    return;
  }

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) leftG[i][j] = mult[i][j];
  }

  int start = pickStart(verdict);
  cout << "\n--- Hierholzer from V" << start << " ---\n";
  if (verdict == 2) {
    cout << "A circuit may begin anywhere, so begin at V" << start << ".\n";
  } else {
    cout << "An open path must begin at an odd vertex, so begin at V" << start
         << ".\n";
  }
  cout << "Take any unused edge while one leaves the current vertex. When none\n";
  cout << "is left, record the vertex as finished and back up one step. The walk\n";
  cout << "uses a copy, so the graph itself stays as it was.\n";

  vector<int> stackV;
  vector<int> tour;
  stackV.push_back(start);
  cout << "\nPush V" << start << " on the stack.\n";

  int used = 0;
  int shown = 0;
  bool teaSipped = false;

  while (!stackV.empty()) {
    int u = stackV.back();
    int v = -1;
    for (int x = 0; x < n; x++) {
      if (leftG[u][x] > 0) {
        v = x;
        break;
      }
    }

    if (v != -1) {
      leftG[u][v]--;
      if (u != v) leftG[v][u]--;
      used++;
      stackV.push_back(v);
      if (shown < TRACE_LIMIT) {
        cout << "\nStep " << used << ": Take " << u << " - " << v
             << " (Copies of that pair left: "
             << (u == v ? leftG[u][u] : leftG[u][v]) << ")\n";
        if (!teaSipped && u == v) {
          cout << "  A self-loop leaves the walk where it stands, so the vertex\n";
          cout << "  appears twice in a row and nothing else changes.\n";
          teaSipped = true;
        }
        printStack(stackV);
        shown++;
      }
      continue;
    }

    tour.push_back(u);
    stackV.pop_back();
    if (shown < TRACE_LIMIT) {
      cout << "\nFinish V" << u << ": No unused edge leaves it, so record it on\n";
      cout << "the tour and back up.\n";
      printStack(stackV);
      shown++;
    }
  }

  if (shown >= TRACE_LIMIT) {
    cout << "\n(Trace stops after " << TRACE_LIMIT
         << " recorded steps. The walk itself ran to the end.)\n";
  }

  cout << "\n--- Euler " << (verdict == 2 ? "circuit" : "path") << " ---\n";
  printTour(tour);

  int remaining = 0;
  for (int a = 0; a < n; a++) {
    for (int b = a; b < n; b++) remaining += leftG[a][b];
  }

  cout << "\nEdges walked: " << used << " of " << edgeCount << ".\n";
  if (remaining == 0) {
    cout << "No copy is left over, so every edge was used exactly once.\n";
  } else {
    cout << "Copies left over: " << remaining << ".\n";
  }

  if (verdict == 2) {
    cout << "\nThe tour ends at V" << tour.front() << ", which is where it began.\n";
    cout << "That is why an all-even graph gives a circuit: At every vertex the\n";
    cout << "arrivals and departures pair up, so nothing can be left behind.\n";
  } else {
    cout << "\nThe tour begins at V" << tour.back() << " and ends at V"
         << tour.front() << ", the two odd vertices.\n";
    cout << "A closed walk would have to come back to V" << tour.back()
         << ", which would add one more edge to its degree.\n";
  }

  cout << "\nWhy the reversal works: When the walk gets stuck, the stack holds the\n";
  cout << "route that led there. Every detour off that route was also finished\n";
  cout << "first, so writing the finished vertices backwards puts each detour\n";
  cout << "after the vertex that started it, and the route reads as one walk.\n";
  cout << "Each edge is used once, so the whole run costs O(V + E).\n";
}

static void showMatrix() {
  cout << "\n--- Multiplicity matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  cout << "      ";
  for (int j = 0; j < n; j++) cout << setw(4) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << setw(4) << i << "  ";
    for (int j = 0; j < n; j++) cout << setw(4) << mult[i][j];
    cout << "\n";
  }
  cout << "Off the diagonal a cell counts the edges between that pair, so 2 means\n";
  cout << "two bridges in the same place. The table is symmetric. The diagonal\n";
  cout << "counts self-loops once, while each one adds two to its degree.\n";
}

static void showEdges() {
  cout << "\n--- Edge list ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "(None)\n";
    cout << "Vertices are there, but no edge joins them.\n";
    return;
  }
  for (int id = 0; id < edgeCount; id++) cout << "  " << edgeLabelOf(id) << "\n";

  cout << "\nRepeated pairs:\n";
  bool anyPair = false;
  for (int u = 0; u < n; u++) {
    for (int v = u; v < n; v++) {
      if (mult[u][v] < 2) continue;
      cout << "  " << u << " - " << v << " x" << mult[u][v] << "\n";
      anyPair = true;
    }
  }
  if (!anyPair) cout << "  (None)\n";
  cout << "A repeated pair is why this graph is a multigraph, and why the edge\n";
  cout << "list needs an id for every copy.\n";
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
  cout << "Undirected multigraph, vertices 0.." << (n - 1) << ".\n";
  cout << "The same pair may be entered again: Two bridges between one pair of\n";
  cout << "banks are two separate edges. A self-loop is allowed too.\n";

  while (true) {
    cout << "\nEdge " << (edgeCount + 1) << " (Limit " << MAXE
         << ", -1 to stop)\n";
    if (edgeCount == MAXE) {
      cout << "Edge limit reached: MAXE = " << MAXE << ".\n";
      break;
    }
    int u = readInt("  From vertex (-1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To vertex (-1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      continue;
    }
    if (u == v) {
      cout << "  Note: A self-loop is a closed walk of one edge, so it adds two\n";
      cout << "  to the degree of V" << u << " and changes no parity.\n";
    }
    int before = edgeCount;
    addEdge(u, v);
    cout << "  Added " << edgeLabelOf(before) << "\n";
    cout << "  Degrees: V" << u << " = " << degreeOf(u) << ", V" << v << " = "
         << degreeOf(v) << "\n";
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << edgeCount
       << (edgeCount == 1 ? " edge.\n" : " edges.\n");
}

static void loadBridgeDemo() {
  resetGraph();
  n = 4;
  cout << "The seven-bridge demo: 4 vertices, 7 edges.\n";
  cout << "  V0 and V1 are the banks, V2 and V3 the islands.\n";
  cout << "  V0 - V3 x2, V1 - V3 x2, V2 - V3, V0 - V2, V1 - V2.\n";
  cout << "  Every vertex ends up with an odd degree, so the walk has more than\n";
  cout << "  two places it must stop.\n\n";

  addEdge(0, 3);
  addEdge(0, 3);
  addEdge(1, 3);
  addEdge(1, 3);
  addEdge(2, 3);
  addEdge(0, 2);
  addEdge(1, 2);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
  cout << "Degrees:";
  for (int v = 0; v < n; v++) cout << " V" << v << " = " << degreeOf(v);
  cout << "\n";
}

static void loadCircuitDemo() {
  resetGraph();
  n = 5;
  cout << "Circuit demo: 5 vertices, 6 edges, all degrees even.\n";
  cout << "  Ring 0 - 1 - 2 - 3 - 0, plus a second copy of 0 - 2.\n";
  cout << "  The parallel pair makes two different edge ids for one pair, which\n";
  cout << "  a plain matrix of 0 and 1 could not store.\n";
  cout << "  V4 stands alone with degree 0, and an isolated vertex never changes\n";
  cout << "  the answer.\n\n";

  addEdge(0, 1);
  addEdge(1, 2);
  addEdge(2, 3);
  addEdge(3, 0);
  addEdge(0, 2);
  addEdge(0, 2);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
  cout << "Degrees:";
  for (int v = 0; v < n; v++) cout << " V" << v << " = " << degreeOf(v);
  cout << "\n";
}

static void loadPathDemo() {
  resetGraph();
  n = 4;
  cout << "Open-path demo: 4 vertices, 3 edges, two odd vertices.\n";
  cout << "  A single run 0 - 1 - 2 - 3 uses each edge once, but it cannot close.\n\n";

  addEdge(0, 1);
  addEdge(1, 2);
  addEdge(2, 3);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
  cout << "Degrees:";
  for (int v = 0; v < n; v++) cout << " V" << v << " = " << degreeOf(v);
  cout << "\n";
}

static void addEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == MAXE) {
    cout << "Edge limit reached: MAXE = " << MAXE << ".\n";
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

    bool repeated = (u == v) ? (mult[u][u] > 0) : (mult[u][v] > 0);
    int before = edgeCount;
    addEdge(u, v);
    cout << "  Added " << edgeLabelOf(before) << "\n";
    if (repeated) {
      cout << "    That pair already had a copy, so now there are " << mult[u][v]
           << ".\n";
    }
    if (u == v) {
      cout << "    A self-loop adds two to the degree of V" << u << ".\n";
    }
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
         << " now. Degrees: V" << u << " = " << degreeOf(u);
    if (u != v) cout << ", V" << v << " = " << degreeOf(v);
    cout << "\n";
    return;
  }
}

static void removeEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove one copy of an edge (V = " << n << ", so vertices 0.."
       << (n - 1) << ")\n";
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
    if (mult[u][v] == 0) {
      cout << "  Rejected: There is no edge " << u << " - " << v << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int found = -1;
    for (int id = 0; id < edgeCount; id++) {
      bool match = (fromE[id] == u && toE[id] == v) ||
                   (fromE[id] == v && toE[id] == u);
      if (!match) continue;
      found = id;
      break;
    }

    int copiesBefore = mult[u][v];
    string label = edgeLabelOf(found);
    removeCopyOfId(found);

    cout << "  Removed " << label << "\n";
    if (copiesBefore > 1) {
      cout << "    " << mult[u][v];
      if (mult[u][v] == 1) cout << " copy of ";
      else cout << " copies of ";
      cout << u << " - " << v << " left.\n";
    }
    if (found < edgeCount) cout << "  Edges after E" << found << " shift down.\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
         << " now. Degrees: V" << u << " = " << degreeOf(u);
    if (u != v) cout << ", V" << v << " = " << degreeOf(v);
    cout << "\n";
    return;
  }
}

static void addVertexInteractive() {
  if (n >= MAXV) {
    cout << "Vertex limit reached: MAXV = " << MAXV << ".\n";
    return;
  }

  int fresh = n;
  for (int i = 0; i < MAXV; i++) {
    mult[fresh][i] = 0;
    mult[i][fresh] = 0;
  }
  n++;

  cout << "\nAdded V" << fresh << " with no edges.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
  cout << "  Its degree is 0, which is even, so it never makes a walk fail.\n";
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

  vector<int> keptFrom;
  vector<int> keptTo;
  for (int id = 0; id < edgeCount; id++) {
    if (fromE[id] == v || toE[id] == v) continue;
    keptFrom.push_back(fromE[id] < v ? fromE[id] : fromE[id] - 1);
    keptTo.push_back(toE[id] < v ? toE[id] : toE[id] - 1);
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
    for (int w = 0; w < n; w++) mult[u][w] = 0;
  }
  edgeCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
  cout << "  With no edges, there is no walk to find.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Undirected multigraph | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Add an edge to the current graph\n";
    cout << "2. Remove one copy of an edge\n";
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
    cout << "\n=== G39: Euler Path and Circuit (Hierholzer) ===\n";
    cout << "Graph: Undirected multigraph | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Build an undirected multigraph\n";
    cout << "2. Load the seven-bridge demo (4 vertices)\n";
    cout << "3. Load the circuit demo (5 vertices)\n";
    cout << "4. Load the open-path demo (4 vertices)\n";
    cout << "5. Graph operations\n";
    cout << "6. Check degrees and decide\n";
    cout << "7. Run Hierholzer\n";
    cout << "8. Show the multiplicity matrix\n";
    cout << "9. Show the edge list\n";
    cout << "10. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadBridgeDemo();
    } else if (choice == 3) {
      loadCircuitDemo();
    } else if (choice == 4) {
      loadPathDemo();
    } else if (choice == 5) {
      operationsMenu();
    } else if (choice == 6) {
      if (n == 0) cout << "Build the graph first.\n";
      else if (edgeCount == 0) cout << "\nThe graph has no edges to check.\n";
      else classify();
    } else if (choice == 7) {
      runHierholzer();
    } else if (choice == 8) {
      showMatrix();
    } else if (choice == 9) {
      showEdges();
    } else if (choice == 10) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}

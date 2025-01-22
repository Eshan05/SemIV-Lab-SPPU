// 16. Complementary reachability: A path in the complement graph uses only the
// pairs G does not join, and one shrinking set of unvisited vertices grows it
// without any complement edge ever being written down.
// Demo: The complement of a complete bipartite graph is two cliques, so two
// vertices from opposite sides cannot reach each other at all.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;
const int MAXE = 120;

static int n = 0;
static int edgeCount = 0;
static bool matrixG[MAXV][MAXV];
static vector<int> adj[MAXV];
static int compId[MAXV];

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
    for (int j = 0; j < MAXV; j++) matrixG[i][j] = false;
  }
  n = 0;
  edgeCount = 0;
}

static void addEdge(int u, int v) {
  matrixG[u][v] = true;
  matrixG[v][u] = true;
  adj[u].push_back(v);
  adj[v].push_back(u);
  edgeCount++;
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
  cout << "Undirected edges only. Enter two vertex numbers in 0.." << (n - 1)
       << ".\n";
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
      cout << "  Rejected: A self-loop is never a missing pair.\n";
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

static void loadDemo(int which) {
  resetGraph();

  if (which == 1) {
    n = 5;
    cout << "Complete-bipartite demo: 5 vertices.\n";
    cout << "  Sides 0 1 2 and 3 4, with all six cross edges and nothing else.\n";
    cout << "  The complement joins a vertex only to its own side, so it falls\n";
    cout << "  into two cliques.\n\n";
    addEdge(0, 3);
    addEdge(0, 4);
    addEdge(1, 3);
    addEdge(1, 4);
    addEdge(2, 3);
    addEdge(2, 4);
  } else if (which == 2) {
    n = 5;
    cout << "Path demo: 5 vertices.\n";
    cout << "  Path 0 - 1 - 2 - 3 - 4, only four pairs joined.\n";
    cout << "  Six pairs stay missing, enough to keep the complement connected.\n\n";
    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 3);
    addEdge(3, 4);
  } else {
    n = 5;
    cout << "Complete demo: 5 vertices.\n";
    cout << "  All ten pairs joined, so the complement has no edge at all.\n";
    cout << "  Every vertex is then a component of its own.\n\n";
    addEdge(0, 1);
    addEdge(0, 2);
    addEdge(0, 3);
    addEdge(0, 4);
    addEdge(1, 2);
    addEdge(1, 3);
    addEdge(1, 4);
    addEdge(2, 3);
    addEdge(2, 4);
    addEdge(3, 4);
  }

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static int complementDegree(int u) {
  int degree = 0;
  for (int v = 0; v < n; v++) {
    if (v == u) continue;
    if (!matrixG[u][v]) degree++;
  }
  return degree;
}

static int complementEdgeCount() {
  int total = 0;
  for (int u = 0; u < n; u++) total += complementDegree(u);
  return total / 2;
}

static void showComplement() {
  cout << "\n--- Complement Graph ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }

  for (int u = 0; u < n; u++) {
    cout << setw(3) << u << ":";
    bool any = false;
    for (int v = 0; v < n; v++) {
      if (v == u) continue;
      if (matrixG[u][v]) continue;
      cout << " " << v;
      any = true;
    }
    if (!any) cout << " (None)";
    cout << "\n";
  }

  cout << "Degrees:";
  for (int u = 0; u < n; u++) cout << " " << complementDegree(u);
  cout << "\n";
  cout << "Edges: " << complementEdgeCount() << " of " << (n * (n - 1)) / 2
       << " possible pairs. The rest are the edges G already has.\n";
}

static void printWaiting(const vector<int> &list) {
  cout << "  Waiting:";
  if (list.empty()) {
    cout << " (Empty)\n";
    return;
  }
  for (int v : list) cout << " " << v;
  cout << "\n";
}

static void complementSearch(int s, bool reached[], bool verbose) {
  for (int v = 0; v < n; v++) reached[v] = false;
  reached[s] = true;

  vector<int> waiting;
  for (int v = 0; v < n; v++) {
    if (v != s) waiting.push_back(v);
  }

  if (verbose) {
    cout << "\nStart at V" << s << ", the only vertex reached so far.\n";
    printWaiting(waiting);
  }

  vector<int> queue;
  queue.push_back(s);
  for (int head = 0; head < (int)queue.size(); head++) {
    int u = queue[head];

    if (verbose) {
      cout << "\nPop V" << u << ".\n";
      cout << "  Neighbours of V" << u << " in G:";
      if (adj[u].empty()) cout << " (None)";
      else
        for (int w : adj[u]) cout << " " << w;
      cout << "\n";
    }

    vector<int> taken;
    vector<int> kept;
    for (int v : waiting) {
      if (matrixG[u][v]) {
        kept.push_back(v);
      } else {
        taken.push_back(v);
      }
    }
    waiting = kept;

    for (int v : taken) {
      reached[v] = true;
      queue.push_back(v);
    }

    if (verbose) {
      cout << "  Waiting vertices that are not on that list:";
      if (taken.empty()) {
        cout << " (None), so this pop adds nothing.\n";
      } else {
        for (int v : taken) cout << " V" << v;
        cout << ", so they join the queue at once.\n";
      }
      printWaiting(waiting);
    }
  }

  if (verbose) {
    cout << "\nReached:";
    bool any = false;
    for (int v = 0; v < n; v++) {
      if (!reached[v]) continue;
      cout << " " << v;
      any = true;
    }
    if (!any) cout << " (None)";
    cout << "\n";
  }
}

static void traceSearch() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  int s;
  while (true) {
    s = readInt("  Start vertex (-1 to cancel): ");
    if (s == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (s >= 0 && s < n) break;
    cout << "  Rejected: Vertex must be in 0.." << (n - 1) << ".\n";
  }

  cout << "\n--- Search Over the Complement ---\n";
  bool reached[MAXV];
  complementSearch(s, reached, true);

  cout << "\nEach pop scans what is still waiting and each vertex leaves that\n";
  cout << "list once, so the search costs about V * V steps and never touches\n";
  cout << "an edge of the complement.\n";
}

static int markComponents() {
  bool done[MAXV];
  bool reached[MAXV];
  for (int v = 0; v < n; v++) {
    done[v] = false;
    compId[v] = 0;
  }

  int id = 0;
  for (int s = 0; s < n; s++) {
    if (done[s]) continue;
    id++;
    complementSearch(s, reached, false);
    for (int v = 0; v < n; v++) {
      if (!reached[v]) continue;
      compId[v] = id;
      done[v] = true;
    }
  }
  return id;
}

static int listComponents() {
  cout << "\n--- Complement Components ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return 0;
  }

  int total = markComponents();
  for (int c = 1; c <= total; c++) {
    int size = 0;
    for (int v = 0; v < n; v++) {
      if (compId[v] == c) size++;
    }
    cout << "Component " << c << " (" << size
         << (size == 1 ? " vertex):" : " vertices):");
    for (int v = 0; v < n; v++) {
      if (compId[v] == c) cout << " " << v;
    }
    cout << "\n";
  }

  cout << "\nCount: " << total << ".\n";
  if (total == 1) {
    cout << "One component, so every pair is joined by a path of missing pairs.\n";
  } else {
    cout << "Two vertices in different components have no path of missing pairs,\n";
    cout << "so every route between them must use an edge G already has.\n";
  }
  return total;
}

static void showReachTable() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  int total = markComponents();
  cout << "\n--- Reachability Through Missing Edges ---\n";
  cout << "      ";
  for (int v = 0; v < n; v++) cout << setw(3) << v;
  cout << "\n";
  for (int u = 0; u < n; u++) {
    cout << setw(3) << u << "  ";
    for (int v = 0; v < n; v++) {
      cout << setw(3) << (compId[u] == compId[v] ? "Y" : ".");
    }
    cout << "\n";
  }

  int pairs = 0;
  int joined = 0;
  for (int u = 0; u < n; u++) {
    for (int v = u + 1; v < n; v++) {
      pairs++;
      if (compId[u] == compId[v]) joined++;
    }
  }

  cout << "Pairs joined: " << joined << " of " << pairs << ".\n";
  cout << "Components: " << total
       << ". Reachability in an undirected graph is symmetric, so the table\n";
  cout << "mirrors itself across the diagonal.\n";
}

static string edgeLabel(int u, int v) {
  return "(" + to_string(u) + " - " + to_string(v) + ")";
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
      cout << "  Rejected: A self-loop is never a missing pair.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (matrixG[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " already exists.\n";
      cout << "    Undirected edges are unordered: " << v << " - " << u
           << " is the same edge as " << u << " - " << v << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    addEdge(u, v);
    cout << "  Added " << edgeLabel(u, v) << "\n";
    cout << "    The pair leaves the complement, so may split its component.\n";
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
      if (matrixG[u][v] && u <= v) edgeCount++;
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
      cout << "    Undirected edges are unordered: " << v << " - " << u
           << " is the same edge as " << u << " - " << v << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    matrixG[u][v] = false;
    matrixG[v][u] = false;
    removeNeighbour(u, v);
    removeNeighbour(v, u);
    edgeCount--;

    cout << "  Removed " << edgeLabel(u, v) << "\n";
    cout << "    The pair joins the complement, so it can only help reachability.\n";
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
    matrixG[fresh][i] = false;
    matrixG[i][fresh] = false;
  }
  adj[fresh].clear();
  n++;

  cout << "\nAdded V" << fresh << " with no edges.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
  cout << "  It has no edge in G, so the complement joins it to everything.\n";
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
      kept[(a < v) ? a : a - 1][(b < v) ? b : b - 1] = matrixG[a][b];
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
    for (int b = 0; b < MAXV; b++) matrixG[a][b] = false;
  }
  for (int a = 0; a < n; a++) {
    adj[a] = keptAdj[a];
    for (int b = 0; b < n; b++) matrixG[a][b] = kept[a][b];
  }
  recountEdges();

  cout << "\nRemoved V" << v << ".\n";
  if (v < n) cout << "  Vertices above V" << v << " shift down by one.\n";
  cout << "  Edges lost: " << before - edgeCount
       << ", and every pair it was missing is gone too.\n";
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
    for (int w = 0; w < n; w++) matrixG[u][w] = false;
  }
  edgeCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
  cout << "  Every pair is now missing, so the complement is a complete graph.\n";
}

static void graphOperationsMenu() {
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
    cout << "\n=== G21: Complementary Reachability ===\n";
    cout << "Graph: Undirected | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build an undirected graph\n";
    cout << "2. Load complete-bipartite demo (5 vertices)\n";
    cout << "3. Load path demo (5 vertices)\n";
    cout << "4. Load complete demo (5 vertices)\n";
    cout << "5. Graph operations\n";
    cout << "6. Show the complement graph\n";
    cout << "7. Trace a complement search from a vertex\n";
    cout << "8. List the complement components\n";
    cout << "9. Reachability table\n";
    cout << "10. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo(1);
    } else if (choice == 3) {
      loadDemo(2);
    } else if (choice == 4) {
      loadDemo(3);
    } else if (choice == 5) {
      graphOperationsMenu();
    } else if (choice == 6) {
      showComplement();
    } else if (choice == 7) {
      traceSearch();
    } else if (choice == 8) {
      listComponents();
    } else if (choice == 9) {
      showReachTable();
    } else if (choice == 10) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}

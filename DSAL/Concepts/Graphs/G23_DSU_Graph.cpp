// 17b. DSU on a graph: An edge either joins two components or closes a cycle,
// so one pass over the edge list finds every cycle edge and counts the
// components at the same time.
// Demo: A square with a chord and a separate pair gives two cycle edges, then
// a tree with one extra branch gives none, and a stepped run shows the count
// dropping with every merge.

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
static int aSide[MAXE];
static int bSide[MAXE];
static vector<int> adj[MAXV];

static int parentOf[MAXV];
static int sizeOf[MAXV];
static int groupCount = 0;

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

static int addEdge(int u, int v) {
  if (u > v) {
    int t = u;
    u = v;
    v = t;
  }
  int id = edgeCount;
  aSide[id] = u;
  bSide[id] = v;
  matrixG[u][v] = true;
  matrixG[v][u] = true;
  adj[u].push_back(v);
  if (v != u) adj[v].push_back(u);
  edgeCount++;
  return id;
}

static string edgeLabelOf(int id) {
  return "E" + to_string(id) + " (" + to_string(aSide[id]) + " - " +
         to_string(bSide[id]) + ")";
}

static void dropEdgeId(int removed) {
  for (int id = removed; id < edgeCount - 1; id++) {
    aSide[id] = aSide[id + 1];
    bSide[id] = bSide[id + 1];
  }
  edgeCount--;
}

static void removeNeighbour(int u, int v) {
  for (int i = 0; i < (int)adj[u].size(); i++) {
    if (adj[u][i] != v) continue;
    adj[u].erase(adj[u].begin() + i);
    return;
  }
}

static void resetDsu() {
  for (int i = 0; i < MAXV; i++) {
    parentOf[i] = i;
    sizeOf[i] = (i < n) ? 1 : 0;
  }
  groupCount = n;
}

static int rootOf(int x) {
  while (parentOf[x] != x) x = parentOf[x];
  return x;
}

static int joinRoots(int ru, int rv) {
  if (ru == rv) return -1;

  int top;
  int under;
  if (sizeOf[ru] > sizeOf[rv]) {
    top = ru;
    under = rv;
  } else if (sizeOf[rv] > sizeOf[ru]) {
    top = rv;
    under = ru;
  } else {
    top = (ru < rv) ? ru : rv;
    under = (top == ru) ? rv : ru;
  }

  parentOf[under] = top;
  sizeOf[top] += sizeOf[under];
  groupCount--;
  return top;
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

static void loadCycleDemo() {
  resetGraph();
  n = 6;
  cout << "Cycle demo: 6 vertices.\n";
  cout << "  Square 0 - 1 - 2 - 3 - 0 with chord 1 - 3, separate pair 4 - 5.\n";
  cout << "  The square closes on its fourth edge, the chord closes again.\n\n";

  addEdge(0, 1);
  addEdge(1, 2);
  addEdge(2, 3);
  addEdge(3, 0);
  addEdge(1, 3);
  addEdge(4, 5);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadForestDemo() {
  resetGraph();
  n = 7;
  cout << "Forest demo: 7 vertices.\n";
  cout << "  Tree 0 - 1 - 2 - 3 with branch 1 - 4, separate edge 5 - 6.\n";
  cout << "  No edge repeats a component, so the scan finds no cycle.\n\n";

  addEdge(0, 1);
  addEdge(1, 2);
  addEdge(2, 3);
  addEdge(1, 4);
  addEdge(5, 6);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void runCycleScan() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  resetDsu();

  cout << "\n--- Cycle scan, edge by edge ---\n";
  int kept = 0;
  int cycles = 0;
  for (int id = 0; id < edgeCount; id++) {
    int u = aSide[id];
    int v = bSide[id];
    int ru = rootOf(u);
    int rv = rootOf(v);

    if (ru == rv) {
      cycles++;
      cout << edgeLabelOf(id) << " Both ends answer to root " << ru
           << " already.\n";
      if (u == v) {
        cout << "  A self-loop joins a vertex to itself, so it closes a\n";
        cout << "  1-cycle on its own.\n";
      } else {
        cout << "  Kept edges already join them, so this edge closes a cycle.\n";
      }
      continue;
    }

    int top = joinRoots(ru, rv);
    kept++;
    cout << edgeLabelOf(id) << " Roots " << ru << " and " << rv
         << " differ, so parent[" << ((top == ru) ? rv : ru) << "] = " << top
         << ".\n";
  }

  cout << "\nKept " << kept << (kept == 1 ? " edge" : " edges") << ", closed "
       << cycles << (cycles == 1 ? " cycle" : " cycles") << ".\n";
  if (cycles == 0) {
    cout << "No edge repeated a component, so the graph is a forest.\n";
  } else {
    cout << "Every closed edge has both ends inside one component, and the\n";
    cout << "kept edges form a spanning forest of the graph.\n";
  }
  cout << "Components: " << groupCount << ".\n";
}

static void runStepThrough() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nThe graph has no edges, so there is nothing to join.\n";
    return;
  }

  resetDsu();
  cout << "\n--- Growing the DSU one edge at a time ---\n";
  cout << "Start: " << groupCount << " components, one per vertex.\n";

  int kept = 0;
  int skipped = 0;
  for (int id = 0; id < edgeCount; id++) {
    cout << "\nNext edge is " << edgeLabelOf(id) << ".\n";
    int go = readRange("  Add it? (1 = yes, 0 = stop): ", 0, 1);
    if (go == 0) {
      cout << "  Stopped before touching " << edgeLabelOf(id) << ".\n";
      break;
    }

    int ru = rootOf(aSide[id]);
    int rv = rootOf(bSide[id]);
    if (ru == rv) {
      skipped++;
      cout << "  Both ends answer to root " << ru << ", so this edge closes a\n";
      cout << "  cycle. Components stay at " << groupCount << ".\n";
      continue;
    }

    int before = groupCount;
    int top = joinRoots(ru, rv);
    kept++;
    cout << "  Roots " << ru << " and " << rv << " differ, so they merge.\n";
    cout << "  parent[" << ((top == ru) ? rv : ru) << "] = " << top
         << ". Components: " << before << " -> " << groupCount << ".\n";
  }

  cout << "\nAfter " << kept << (kept == 1 ? " merge and " : " merges and ")
       << skipped << (skipped == 1 ? " skipped edge: " : " skipped edges: ");
  cout << groupCount << (groupCount == 1 ? " component.\n" : " components.\n");
  if (groupCount == 1) {
    cout << "One root answers for every vertex, so the graph is connected.\n";
  } else {
    cout << "More than one root, so the graph still has separate pieces.\n";
  }
  cout << "The graph itself is unchanged, so you can step through again.\n";
}

static void runComponents() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  resetDsu();
  for (int id = 0; id < edgeCount; id++) {
    int ru = rootOf(aSide[id]);
    int rv = rootOf(bSide[id]);
    joinRoots(ru, rv);
  }

  cout << "\n--- Components by DSU ---\n";
  int index = 0;
  for (int r = 0; r < n; r++) {
    if (parentOf[r] != r) continue;
    index++;
    int members = 0;
    for (int x = 0; x < n; x++) {
      if (rootOf(x) == r) members++;
    }
    cout << "Component " << index << " (" << members
         << (members == 1 ? " vertex):" : " vertices):");
    for (int x = 0; x < n; x++) {
      if (rootOf(x) == r) cout << " " << x;
    }
    cout << "\n";
  }
  cout << "Count: " << groupCount << ".\n";
  cout << "A union per edge is all it took, so the count costs one pass over\n";
  cout << "the edge list rather than a search from every vertex.\n";
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
  cout << "Each line lists neighbours in insertion order.\n";
}

static void showEdges() {
  cout << "\n--- Edge List ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "(None)\n";
    cout << "Vertices are there, but no edge joins them.\n";
    return;
  }
  for (int id = 0; id < edgeCount; id++) {
    cout << "  " << edgeLabelOf(id) << "\n";
  }
  cout << "The scan walks this list top to bottom, so the ids show which edge\n";
  cout << "is examined first.\n";
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
    if (matrixG[u][v]) {
      cout << "  Rejected: " << u << " - " << v << " already exists.\n";
      cout << "    Undirected edges are unordered: " << v << " - " << u
           << " is the same edge as " << u << " - " << v << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int id = addEdge(u, v);
    cout << "  Added " << edgeLabelOf(id) << "\n";
    if (u == v) cout << "    A self-loop is a cycle of length 1.\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
         << " now.\n";
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
      cout << "  Rejected: " << u << " - " << v << " is not in the graph.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int found = -1;
    for (int id = 0; id < edgeCount; id++) {
      if (aSide[id] == u && bSide[id] == v) {
        found = id;
        break;
      }
      if (aSide[id] == v && bSide[id] == u) {
        found = id;
        break;
      }
    }

    matrixG[u][v] = false;
    matrixG[v][u] = false;
    removeNeighbour(u, v);
    if (u != v) removeNeighbour(v, u);

    string label = edgeLabelOf(found);
    dropEdgeId(found);

    cout << "  Removed " << label << "\n";
    if (found < edgeCount) {
      cout << "  Edges above E" << found << " shift down by one.\n";
    }
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
  cout << "  A lone vertex is a component of its own.\n";
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

  vector<int> keptA;
  vector<int> keptB;
  for (int id = 0; id < edgeCount; id++) {
    if (aSide[id] == v || bSide[id] == v) continue;
    keptA.push_back(aSide[id] < v ? aSide[id] : aSide[id] - 1);
    keptB.push_back(bSide[id] < v ? bSide[id] : bSide[id] - 1);
  }

  int before = edgeCount;
  n--;

  for (int a = 0; a < MAXV; a++) {
    adj[a].clear();
    for (int b = 0; b < MAXV; b++) matrixG[a][b] = false;
  }

  edgeCount = (int)keptA.size();
  for (int id = 0; id < edgeCount; id++) {
    aSide[id] = keptA[id];
    bSide[id] = keptB[id];
    matrixG[aSide[id]][bSide[id]] = true;
    matrixG[bSide[id]][aSide[id]] = true;
    adj[aSide[id]].push_back(bSide[id]);
    if (aSide[id] != bSide[id]) adj[bSide[id]].push_back(aSide[id]);
  }

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
    for (int w = 0; w < n; w++) matrixG[u][w] = false;
  }
  edgeCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
  cout << "  Every vertex is a component on its own now.\n";
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
    cout << "\n=== G23: DSU on a Graph (Cycles, Components) ===\n";
    cout << "Graph: Undirected | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build an undirected graph\n";
    cout << "2. Load cycle demo (6 vertices)\n";
    cout << "3. Load forest demo (7 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Scan every edge at once\n";
    cout << "6. Step through the edges and watch the count\n";
    cout << "7. Count components with DSU\n";
    cout << "8. Show adjacency list\n";
    cout << "9. Show the edge list\n";
    cout << "10. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadCycleDemo();
    } else if (choice == 3) {
      loadForestDemo();
    } else if (choice == 4) {
      operationsMenu();
    } else if (choice == 5) {
      runCycleScan();
    } else if (choice == 6) {
      runStepThrough();
    } else if (choice == 7) {
      runComponents();
    } else if (choice == 8) {
      showAdjacency();
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

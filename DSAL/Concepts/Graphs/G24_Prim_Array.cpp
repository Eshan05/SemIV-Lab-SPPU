// 18a. Prim, dense array: Every pass takes the closest vertex outside the
// tree, and one array of keys holds the cheapest offer to each vertex, so the
// graph itself never needs a heap.
// Demo: A seven-vertex graph grows one vertex at a time, then a square with
// equal weights shows how a tie is settled.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

const int MAXV = 15;
const int MAXE = 120;
const int INF = 1000000;

static int n = 0;
static int edgeCount = 0;
static int wG[MAXV][MAXV];
static int aSide[MAXE];
static int bSide[MAXE];
static int wEdge[MAXE];
static vector<pair<int, int>> adj[MAXV];

static int keyOf[MAXV];
static int parentOf[MAXV];
static bool inTree[MAXV];

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
    for (int j = 0; j < MAXV; j++) wG[i][j] = 0;
  }
  n = 0;
  edgeCount = 0;
}

static int addEdge(int u, int v, int w) {
  if (u > v) {
    int t = u;
    u = v;
    v = t;
  }
  int id = edgeCount;
  aSide[id] = u;
  bSide[id] = v;
  wEdge[id] = w;
  wG[u][v] = w;
  wG[v][u] = w;
  adj[u].push_back(make_pair(v, w));
  adj[v].push_back(make_pair(u, w));
  edgeCount++;
  return id;
}

static string edgeLabelOf(int id) {
  return "E" + to_string(id) + " (" + to_string(aSide[id]) + " - " +
         to_string(bSide[id]) + ", weight " + to_string(wEdge[id]) + ")";
}

static void dropEdgeId(int removed) {
  for (int id = removed; id < edgeCount - 1; id++) {
    aSide[id] = aSide[id + 1];
    bSide[id] = bSide[id + 1];
    wEdge[id] = wEdge[id + 1];
  }
  edgeCount--;
}

static void removeNeighbour(int u, int v) {
  for (int i = 0; i < (int)adj[u].size(); i++) {
    if (adj[u][i].first != v) continue;
    adj[u].erase(adj[u].begin() + i);
    return;
  }
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
  cout << "Undirected weighted edges. Weights are whole numbers in 1..99.\n";
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
      cout << "  Rejected: A self-loop never helps a spanning tree.\n";
      e--;
      continue;
    }
    int w = readInt("  Weight (1..99): ");
    if (w < 1 || w > 99) {
      cout << "  Rejected: Weight must be in 1..99.\n";
      e--;
      continue;
    }
    if (wG[u][v] != 0) {
      cout << "  Rejected: That edge already exists.\n";
      e--;
      continue;
    }
    addEdge(u, v, w);
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << edgeCount
       << (edgeCount == 1 ? " edge.\n" : " edges.\n");
}

static void loadDemo() {
  resetGraph();
  n = 7;
  cout << "Demo: 7 vertices, 11 edges.\n";
  cout << "  0 - 1 (7), 0 - 3 (5), 1 - 2 (8), 1 - 3 (9), 1 - 4 (7),\n";
  cout << "  2 - 4 (5), 3 - 4 (15), 3 - 5 (6), 4 - 5 (8), 4 - 6 (9),\n";
  cout << "  5 - 6 (11).\n";
  cout << "  One cheap edge leaves each new vertex, so the tree fills up in\n";
  cout << "  six passes.\n\n";

  addEdge(0, 3, 5);
  addEdge(0, 1, 7);
  addEdge(1, 2, 8);
  addEdge(1, 3, 9);
  addEdge(1, 4, 7);
  addEdge(2, 4, 5);
  addEdge(3, 4, 15);
  addEdge(3, 5, 6);
  addEdge(4, 5, 8);
  addEdge(4, 6, 9);
  addEdge(5, 6, 11);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadTieDemo() {
  resetGraph();
  n = 4;
  cout << "Tie demo: 4 vertices in a square, every edge weight 1.\n";
  cout << "  Four edges of the same weight, so any three of them span the\n";
  cout << "  square. The array picks the lower vertex id at every tie.\n\n";

  addEdge(0, 1, 1);
  addEdge(1, 2, 1);
  addEdge(2, 3, 1);
  addEdge(3, 0, 1);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void printKeyRows() {
  cout << "  Vertex :";
  for (int v = 0; v < n; v++) cout << setw(4) << v;
  cout << "\n  Key    :";
  for (int v = 0; v < n; v++) {
    if (keyOf[v] >= INF) cout << setw(4) << "-";
    else cout << setw(4) << keyOf[v];
  }
  cout << "\n  Parent :";
  for (int v = 0; v < n; v++) {
    if (parentOf[v] < 0) cout << setw(4) << "-";
    else cout << setw(4) << parentOf[v];
  }
  cout << "\n";
}

static void printTreeMembers() {
  cout << "  Tree:";
  for (int v = 0; v < n; v++) {
    if (inTree[v]) cout << " " << v;
  }
  cout << ".\n";
}

static void relaxFrom(int u) {
  bool any = false;
  for (int i = 0; i < (int)adj[u].size(); i++) {
    int v = adj[u][i].first;
    int w = adj[u][i].second;
    if (inTree[v]) continue;
    any = true;
    if (keyOf[v] >= INF) {
      keyOf[v] = w;
      parentOf[v] = u;
      cout << "    V" << v << ": No offer yet, so " << w << " through V" << u
           << ".\n";
    } else if (w < keyOf[v]) {
      cout << "    V" << v << ": " << keyOf[v] << " becomes " << w
           << " through V" << u << ".\n";
      keyOf[v] = w;
      parentOf[v] = u;
    } else {
      cout << "    V" << v << ": " << keyOf[v] << " stays, since " << w
           << " is no better.\n";
    }
  }
  if (!any) cout << "  No outside neighbour, so nothing to update.\n";
}

static void runPrim() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nNo edges, so there is nothing to grow.\n";
    return;
  }

  int start = readRange("\nStart vertex (0.." + to_string(n - 1) + "): ", 0,
                        n - 1);

  for (int i = 0; i < MAXV; i++) {
    keyOf[i] = INF;
    parentOf[i] = -1;
    inTree[i] = false;
  }
  keyOf[start] = 0;

  vector<int> takenA;
  vector<int> takenB;
  vector<int> takenW;
  int total = 0;

  cout << "\n--- Prim from V" << start << " ---\n";
  cout << "Tree: " << start << ". Total weight 0.\n";
  inTree[start] = true;
  int treeSize = 1;

  cout << "Offers from V" << start << ":\n";
  relaxFrom(start);
  printTreeMembers();

  int pass = 0;
  while (treeSize < n) {
    int best = -1;
    vector<int> tied;
    for (int v = 0; v < n; v++) {
      if (inTree[v] || keyOf[v] >= INF) continue;
      if (best == -1 || keyOf[v] < keyOf[best]) {
        best = v;
        tied.clear();
        tied.push_back(v);
      } else if (keyOf[v] == keyOf[best]) {
        tied.push_back(v);
      }
    }
    if (best == -1) break;

    pass++;
    cout << "\nPass " << pass << "\n";
    printKeyRows();
    if (pass == 1) {
      cout << "  Key is the cheapest offer so far, and Parent is the vertex\n";
      cout << "  that made it. A tree vertex keeps the weight it entered with.\n";
    }

    if ((int)tied.size() > 1) {
      cout << "  " << tied.size() << " vertices are tied at " << keyOf[best]
           << ", so the lowest one, V" << best << ", goes first.\n";
    }

    int from = parentOf[best];
    cout << "  Take V" << best << " at " << keyOf[best] << " through V" << from
         << ".\n";
    inTree[best] = true;
    treeSize++;
    total += keyOf[best];
    takenA.push_back(from < best ? from : best);
    takenB.push_back(from < best ? best : from);
    takenW.push_back(keyOf[best]);
    cout << "  Edge " << (from < best ? from : best) << " - "
         << (from < best ? best : from) << ", weight " << keyOf[best]
         << ". Total weight is " << total << ".\n";
    cout << "  Offers from V" << best << ":\n";
    relaxFrom(best);
    printTreeMembers();
  }

  cout << "\n--- Result ---\n";
  if (treeSize <= 1) {
    cout << "Only V" << start << " is in the tree, so no edge was taken.\n";
    return;
  }

  cout << "Edges, in the order they were added:\n";
  for (int i = 0; i < (int)takenA.size(); i++) {
    cout << "  " << takenA[i] << " - " << takenB[i] << "   weight "
         << takenW[i] << "\n";
  }
  cout << "Total weight: " << total << ".\n";
  cout << "Edges taken: " << treeSize - 1 << ". A spanning tree of " << treeSize
       << " vertices needs exactly " << treeSize - 1 << ".\n";

  if (treeSize == n) {
    cout << "Every vertex is in the tree, so no edge crosses out of it. This\n";
    cout << "is a minimum spanning tree.\n";
  } else {
    cout << treeSize << " of " << n << " vertices are reachable from V" << start
         << ", so this is a minimum\n";
    cout << "spanning tree of that piece only, not of the whole graph.\n";
  }

  cout << "\nThe cut is scanned once per pass, so " << pass << " passes cost about "
       << pass * n << "\ncomparisons. A nearly complete graph has about V x V "
       << "edges, so this is\nthe shape to use there, and a sparse graph just "
       << "wastes the scan.\n";
}

static void showMatrix() {
  cout << "\n--- Weight Matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  cout << "     ";
  for (int j = 0; j < n; j++) cout << setw(4) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << setw(4) << i << " ";
    for (int j = 0; j < n; j++) {
      if (wG[i][j] == 0) cout << setw(4) << "-";
      else cout << setw(4) << wG[i][j];
    }
    cout << "\n";
  }
  cout << "A dash means no edge. The matrix is symmetric, so each weight\n";
  cout << "appears twice.\n";
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
  cout << "Prim scans the cut by vertex, so this order does not matter. The\n";
  cout << "Kruskal files walk this list instead.\n";
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
      cout << "  Rejected: A self-loop never helps a spanning tree.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (wG[u][v] != 0) {
      cout << "  Rejected: " << u << " - " << v << " already exists.\n";
      cout << "    Undirected edges are unordered: " << v << " - " << u
           << " is the same edge as " << u << " - " << v << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int w = readInt("  Weight (1..99): ");
    if (w < 1 || w > 99) {
      cout << "  Rejected: Weight must be in 1..99.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int id = addEdge(u, v, w);
    cout << "  Added " << edgeLabelOf(id) << "\n";
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
    if (wG[u][v] == 0) {
      cout << "  Rejected: " << u << " - " << v << " is not in the graph.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int found = -1;
    for (int id = 0; id < edgeCount; id++) {
      if ((aSide[id] == u && bSide[id] == v) ||
          (aSide[id] == v && bSide[id] == u)) {
        found = id;
        break;
      }
    }

    wG[u][v] = 0;
    wG[v][u] = 0;
    removeNeighbour(u, v);
    removeNeighbour(v, u);

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
    wG[fresh][i] = 0;
    wG[i][fresh] = 0;
  }
  adj[fresh].clear();
  n++;

  cout << "\nAdded V" << fresh << " with no edges.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
  cout << "  A lone vertex has no edge to offer, so Prim can never reach it.\n";
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
  vector<int> keptW;
  for (int id = 0; id < edgeCount; id++) {
    if (aSide[id] == v || bSide[id] == v) continue;
    keptA.push_back(aSide[id] < v ? aSide[id] : aSide[id] - 1);
    keptB.push_back(bSide[id] < v ? bSide[id] : bSide[id] - 1);
    keptW.push_back(wEdge[id]);
  }

  int before = edgeCount;
  n--;

  for (int a = 0; a < MAXV; a++) {
    adj[a].clear();
    for (int b = 0; b < MAXV; b++) wG[a][b] = 0;
  }

  edgeCount = (int)keptA.size();
  for (int id = 0; id < edgeCount; id++) {
    aSide[id] = keptA[id];
    bSide[id] = keptB[id];
    wEdge[id] = keptW[id];
    wG[aSide[id]][bSide[id]] = wEdge[id];
    wG[bSide[id]][aSide[id]] = wEdge[id];
    adj[aSide[id]].push_back(make_pair(bSide[id], wEdge[id]));
    adj[bSide[id]].push_back(make_pair(aSide[id], wEdge[id]));
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
    for (int w = 0; w < n; w++) wG[u][w] = 0;
  }
  edgeCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
  cout << "  With no edges, there is nothing for Prim to grow.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Undirected, weighted | V = " << n << ", E = " << edgeCount
         << "\n";
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
    cout << "\n=== G24: Prim's Algorithm (Dense Array) ===\n";
    cout << "Graph: Undirected, weighted | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Build a weighted graph\n";
    cout << "2. Load demo graph (7 vertices)\n";
    cout << "3. Load tie demo (4 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Grow the tree from a start vertex\n";
    cout << "6. Show the weight matrix\n";
    cout << "7. Show the edge list\n";
    cout << "8. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo();
    } else if (choice == 3) {
      loadTieDemo();
    } else if (choice == 4) {
      operationsMenu();
    } else if (choice == 5) {
      runPrim();
    } else if (choice == 6) {
      showMatrix();
    } else if (choice == 7) {
      showEdges();
    } else if (choice == 8) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}

// 18b. Prim, binary heap: The same greedy step, but a min-heap holds the
// offers, so the closest vertex is one pop instead of a scan of every vertex.
// Demo: The seven-vertex graph gives the same tree as the array file, and the
// entries that are popped and thrown away show what lazy deletion costs.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>
using namespace std;

const int MAXV = 15;
const int MAXE = 120;
const int INF = 1000000;

struct Offer {
  int w;
  int v;
  int from;
};

struct CheapFirst {
  bool operator()(const Offer &a, const Offer &b) const {
    if (a.w != b.w) return a.w > b.w;
    return a.v > b.v;
  }
};

static int n = 0;
static int edgeCount = 0;
static int wG[MAXV][MAXV];
static int aSide[MAXE];
static int bSide[MAXE];
static int wEdge[MAXE];
static vector<pair<int, int>> adj[MAXV];

static bool inTree[MAXV];
static priority_queue<Offer, vector<Offer>, CheapFirst> heapQ;
static int pushesMade = 0;

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
  cout << "  One cheap edge leaves each new vertex, so the heap hands over\n";
  cout << "  one vertex per pop.\n\n";

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
  cout << "  square. The heap breaks a tie by the lower vertex id, so it takes\n";
  cout << "  the same edges as the array file.\n\n";

  addEdge(0, 1, 1);
  addEdge(1, 2, 1);
  addEdge(2, 3, 1);
  addEdge(3, 0, 1);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void printCutRow() {
  cout << "  Cut    : {";
  bool first = true;
  for (int v = 0; v < n; v++) {
    if (!inTree[v]) continue;
    if (!first) cout << " ";
    cout << v;
    first = false;
  }
  cout << "} | {";
  first = true;
  for (int v = 0; v < n; v++) {
    if (inTree[v]) continue;
    if (!first) cout << " ";
    cout << v;
    first = false;
  }
  cout << "}\n";
}

static void printHeap() {
  priority_queue<Offer, vector<Offer>, CheapFirst> copy = heapQ;
  int size = (int)copy.size();
  cout << "  Heap (" << size << (size == 1 ? " entry" : " entries")
       << ", cheapest first):";
  if (size == 0) {
    cout << " (Empty)\n";
    return;
  }
  cout << "\n";
  int shown = 0;
  while (!copy.empty()) {
    Offer top = copy.top();
    copy.pop();
    cout << (shown % 4 == 0 ? "    " : ", ");
    cout << "V" << top.v << " at " << top.w;
    shown++;
    if (shown % 4 == 0) cout << "\n";
  }
  if (shown % 4 != 0) cout << "\n";
}

static void pushOffers(int u) {
  bool any = false;
  for (int i = 0; i < (int)adj[u].size(); i++) {
    int v = adj[u][i].first;
    int w = adj[u][i].second;
    if (inTree[v]) continue;
    any = true;
    heapQ.push(Offer{w, v, u});
    pushesMade++;
    cout << "    Push V" << v << " at " << w << " through V" << u << ".\n";
  }
  if (!any) cout << "    No outside neighbour, so nothing is pushed.\n";
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

  while (!heapQ.empty()) heapQ.pop();
  for (int i = 0; i < MAXV; i++) inTree[i] = false;
  pushesMade = 0;

  vector<int> takenA;
  vector<int> takenB;
  vector<int> takenW;
  int total = 0;
  int stale = 0;
  int pops = 0;

  cout << "\n--- Prim from V" << start << " (Heap) ---\n";
  inTree[start] = true;
  int treeSize = 1;

  cout << "Total weight 0. The cut is the tree on the left and the rest on the\n";
  cout << "right, and the heap always hands over its lightest edge.\n";
  printCutRow();
  cout << "Push offers from V" << start << ":\n";
  pushOffers(start);

  int step = 0;
  while (treeSize < n && !heapQ.empty()) {
    step++;
    cout << "\nStep " << step << "\n";
    printHeap();

    Offer top = heapQ.top();
    heapQ.pop();
    pops++;

    if (inTree[top.v]) {
      stale++;
      cout << "  Pop V" << top.v << " at " << top.w << " through V" << top.from
           << ".\n";
      cout << "  V" << top.v << " is in the tree already, so this entry is"
           << " thrown away.\n";
      continue;
    }

    int a = (top.from < top.v) ? top.from : top.v;
    int b = (top.from < top.v) ? top.v : top.from;
    inTree[top.v] = true;
    treeSize++;
    total += top.w;
    takenA.push_back(a);
    takenB.push_back(b);
    takenW.push_back(top.w);

    cout << "  Pop V" << top.v << " at " << top.w << " through V" << top.from
         << ".\n";
    cout << "  V" << top.v << " is outside the tree, so take it, the lightest\n";
    cout << "  edge across the cut.\n";
    cout << "  Edge " << a << " - " << b << ", weight " << top.w
         << ". Total weight is " << total << ".\n";
    cout << "  Push offers from V" << top.v << ":\n";
    pushOffers(top.v);
    printCutRow();
  }

  cout << "\n--- Result ---\n";
  if (treeSize <= 1) {
    cout << "Only V" << start << " is in the tree, so no edge was taken.\n";
    return;
  }

  cout << "Edges, in the order the heap handed them over:\n";
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

  cout << "\nEvery edge above was the lightest across the cut at the moment it was\n";
  cout << "taken, and that is the whole argument: The lightest edge across any\n";
  cout << "cut belongs to some minimum spanning tree. The heap does not change\n";
  cout << "that, it only finds the same edge faster. Cycles can defend these\n";
  cout << "edges too, but only by arguing about a tree this run never built, so\n";
  cout << "the cut is the certificate actually in hand.\n";
  int levels = 0;
  int reach = 1;
  while (reach < pushesMade) {
    reach *= 2;
    levels++;
  }
  int leftover = (int)heapQ.size();

  cout << "\nPushes: " << pushesMade
       << ", one per edge, because an edge is offered from the end that\n";
  cout << "joins first. Pops: " << pops << ". Entries thrown away: " << stale
       << ".\n";
  if (leftover > 0) {
    cout << "Entries left in the heap: " << leftover
         << ". They are never popped, since the\n";
    cout << "loop stops as soon as every vertex is in.\n";
  }
  cout << "The array version scans " << treeSize - 1 << " x " << n << " = "
       << (treeSize - 1) * n << " keys. Here there are " << pushesMade + pops
       << " heap\noperations of about " << levels << " levels each, so roughly "
       << (pushesMade + pops) * levels << " steps. On a\nsmall graph the "
       << "bookkeeping loses, and on a sparse graph the heap wins,\n";
  cout << "because a scan costs one key per vertex while a heap step costs\n";
  cout << "about log E.\n";
  cout << "Standard C++ has no heap that can lower one entry in place, so a\n";
  cout << "cheaper offer is pushed again and the older one is left to be\n";
  cout << "discarded. A heap with a real decrease-key would rewrite the entry\n";
  cout << "instead, so nothing would be wasted.\n";
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
  cout << "The heap is fed from these neighbours, so a heap run never reads\n";
  cout << "this list in order. The Kruskal files walk it directly.\n";
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
    cout << "\n=== G25: Prim's Algorithm (Binary Heap) ===\n";
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

// 19c. Kruskal, bucket sort: One bucket per weight turns the sort into a
// pass to fill the buckets and a walk over the weights, so no two weights are
// ever compared.
// Demo: The same six edges in the same order, from 110 steps with no
// comparison at all.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

const int MAXV = 15;
const int MAXE = 120;
const int MAXW = 99;
const int INF = 1000000;

static int n = 0;
static int edgeCount = 0;
static int wG[MAXV][MAXV];
static int aSide[MAXE];
static int bSide[MAXE];
static int wEdge[MAXE];
static vector<pair<int, int>> adj[MAXV];

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
  cout << "  The buckets hold the edges by weight, so the walk visits them from\n";
  cout << "  light to heavy without comparing anything.\n\n";

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
  cout << "  square. A bucket keeps the input order, so the walk takes the first\n";
  cout << "  three edges: A different tree from Prim's, same total weight.\n\n";

  addEdge(0, 1, 1);
  addEdge(1, 2, 1);
  addEdge(2, 3, 1);
  addEdge(3, 0, 1);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static int parentOf[MAXV];
static int sizeOf[MAXV];
static int groupCount = 0;

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

static void runKruskal() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nNo edges, so there is nothing to keep.\n";
    return;
  }

  vector<int> bucket[MAXW + 1];
  for (int id = 0; id < edgeCount; id++) bucket[wEdge[id]].push_back(id);

  int usedBuckets = 0;
  for (int w = 0; w <= MAXW; w++) {
    if (!bucket[w].empty()) usedBuckets++;
  }

  cout << "\n--- Kruskal: Bucket the edges by weight, then walk the buckets ---\n";
  cout << edgeCount << " edges go into " << MAXW << " buckets, one per weight, and "
       << "the ids\n";
  cout << "inside a bucket keep their input order.\n\n";

  for (int w = 0; w <= MAXW; w++) {
    if (bucket[w].empty()) continue;
    cout << setw(4) << w << ":";
    for (int i = 0; i < (int)bucket[w].size(); i++) cout << " E" << bucket[w][i];
    cout << "\n";
  }
  cout << "\n" << (MAXW - usedBuckets) << " of " << MAXW
       << " buckets stay empty and are skipped, so no two\n";
  cout << "weights are ever compared.\n";

  resetDsu();
  int kept = 0;
  int rejected = 0;
  int examined = 0;
  int total = 0;
  int pos = 0;
  vector<int> keptIds;

  for (int w = 0; w <= MAXW && kept < n - 1; w++) {
    for (int i = 0; i < (int)bucket[w].size() && kept < n - 1; i++) {
      int id = bucket[w][i];
      pos++;
      examined++;

      int u = aSide[id];
      int v = bSide[id];
      int ru = rootOf(u);
      int rv = rootOf(v);

      cout << "\n" << pos << ". " << edgeLabelOf(id) << "\n";

      if (ru == rv) {
        rejected++;
        cout << "   Both ends answer to root " << ru
             << " already, so reject it.\n";
        cout << "   Total stays " << total << ", edges still " << kept << " of "
             << (n - 1) << ".\n";
        continue;
      }

      int top = joinRoots(ru, rv);
      kept++;
      total += wEdge[id];
      keptIds.push_back(id);
      cout << "   Roots " << ru << " and " << rv << " differ, so keep it.\n";
      cout << "   parent[" << ((top == ru) ? rv : ru) << "] = " << top
           << ". Total " << total << ", edges " << kept << " of " << (n - 1)
           << ".\n";
    }
  }

  cout << "\n--- Result ---\n";
  cout << "Kept edges, lightest first:\n";
  for (int i = 0; i < (int)keptIds.size(); i++) {
    int id = keptIds[i];
    cout << "  " << aSide[id] << " - " << bSide[id] << "   weight "
         << wEdge[id] << "\n";
  }
  cout << "Total weight: " << total << ".\n";

  int leftOver = edgeCount - examined;
  cout << "Examined " << examined << " of " << edgeCount
       << " edges: " << kept << " kept, " << rejected << " rejected";
  if (leftOver > 0) cout << ", " << leftOver << " left over";
  cout << ".\n";

  if (leftOver > 0) {
    cout << "The walk stopped as soon as " << (n - 1)
         << " edges were kept, so the\n";
    cout << "heaviest " << leftOver << " edges were never pulled out of their "
         << "buckets.\n";
  }

  if (groupCount == 1) {
    cout << "\nOne set now holds every vertex, so this is a minimum spanning\n";
    cout << "tree.\n";
  } else {
    cout << "\nThe graph falls into " << groupCount
         << " pieces, so no tree can join them all.\n";
    cout << "Kept " << kept << " edges for " << n
         << " vertices: That is a spanning forest, one\n";
    cout << "tree per piece.\n";
  }

  cout << "\nFilling the buckets is one pass over " << edgeCount
       << " edges and one pass over " << MAXW << "\nweights, so " << edgeCount
       << " + " << MAXW << " = " << edgeCount + MAXW << " steps and no comparison at "
       << "all.\n";
  cout << "The sorted file pays about " << edgeCount * 4
       << " comparisons for the same order, and the\n";
  cout << "scan file pays more; this one trades comparisons for a bucket per\n";
  cout << "weight, which only works while the weights stay small whole numbers.\n";
  cout << "A weight of a million would ask for a million buckets.\n";
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
  cout << "This list is read once to fill the buckets, and the ids also fix\n";
  cout << "the order inside a bucket when two edges share a weight.\n";
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
    cout << "\n=== G28: Kruskal's Algorithm (Bucket Sort) ===\n";
    cout << "Graph: Undirected, weighted | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Build a weighted graph\n";
    cout << "2. Load demo graph (7 vertices)\n";
    cout << "3. Load tie demo (4 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Run Kruskal with weight buckets\n";
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
      runKruskal();
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

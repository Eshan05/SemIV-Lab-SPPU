// 33. Level graph and Dinic's algorithm: A breadth-first pass labels each
// vertex with its distance from the source in the residual, and a depth-first
// pass sends a blocking flow using only arcs that step one level down. Demo:
// The diamond network needs two level graphs, while a wide layered network
// needs only one.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;
const int TRACE_LIMIT = 70;

static int n = 0;
static int arcCount = 0;
static int capM[MAXV][MAXV];
static vector<int> outAdj[MAXV];

static int resM[MAXV][MAXV];
static int flowM[MAXV][MAXV];
static int level[MAXV];
static int ptr[MAXV];
static int dfsPath[MAXV];
static int dfsLen = 0;
static int foundPath[MAXV];
static int foundLen = 0;
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

static string plural(int count, const string &one, const string &many) {
  return to_string(count) + " " + (count == 1 ? one : many);
}

static string arcText(int u, int v) {
  return "(" + to_string(u) + " -> " + to_string(v) + ")";
}

static void say(const string &line) {
  if (traceShown >= TRACE_LIMIT) return;
  traceShown++;
  cout << line << "\n";
}

static string pathText(const int path[], int len) {
  string text;
  for (int i = 0; i < len; i++) {
    if (i > 0) text += " -> ";
    text += "V" + to_string(path[i]);
  }
  return text;
}

static void resetNetwork() {
  for (int i = 0; i < MAXV; i++) {
    outAdj[i].clear();
    for (int j = 0; j < MAXV; j++) capM[i][j] = 0;
  }
  n = 0;
  arcCount = 0;
}

static void addArc(int u, int v, int capacity) {
  capM[u][v] = capacity;
  outAdj[u].push_back(v);
  arcCount++;
}

static bool hasArc(int u, int v) { return capM[u][v] > 0; }

static bool buildLevels(int s, int t) {
  for (int i = 0; i < n; i++) level[i] = -1;

  int queue[MAXV];
  int head = 0;
  int tail = 0;
  queue[tail++] = s;
  level[s] = 0;

  while (head < tail) {
    int u = queue[head++];
    for (int v = 0; v < n; v++) {
      if (resM[u][v] <= 0) continue;
      if (level[v] != -1) continue;
      level[v] = level[u] + 1;
      queue[tail++] = v;
    }
  }

  if (level[t] == -1) return false;

  string text;
  for (int i = 0; i < n; i++) {
    if (level[i] == -1) continue;
    if (text.size() > 0) text += ", ";
    text += "V" + to_string(i) + " at " + to_string(level[i]);
  }
  say("  Level graph: " + text + ".");
  return true;
}

static int dfsBlock(int u, int t, int limit) {
  dfsPath[dfsLen++] = u;

  if (u == t) {
    foundLen = dfsLen;
    for (int i = 0; i < dfsLen; i++) foundPath[i] = dfsPath[i];
    dfsLen--;
    return limit;
  }

  for (int &v = ptr[u]; v < n; v++) {
    if (resM[u][v] <= 0) continue;
    if (level[v] != level[u] + 1) continue;

    int room = resM[u][v];
    int sent = dfsBlock(v, t, room < limit ? room : limit);
    if (sent > 0) {
      resM[u][v] -= sent;
      resM[v][u] += sent;
      if (capM[u][v] > 0) flowM[u][v] += sent;
      else flowM[v][u] -= sent;
      dfsLen--;
      return sent;
    }
  }

  level[u] = -1;
  dfsLen--;
  return 0;
}

static void runDinic() {
  if (n == 0) {
    cout << "Build the network first.\n";
    return;
  }

  int s;
  int t;
  while (true) {
    s = readInt("  Source (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (s == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (s >= 0 && s < n) break;
    cout << "  Rejected: Source must be in 0.." << (n - 1) << ".\n";
  }
  while (true) {
    t = readInt("  Sink (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (t == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (t >= 0 && t < n) break;
    cout << "  Rejected: Sink must be in 0.." << (n - 1) << ".\n";
  }
  if (s == t) {
    cout << "  Rejected: Source and sink must be different vertices.\n";
    return;
  }

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      resM[i][j] = capM[i][j];
      flowM[i][j] = 0;
    }
  }
  traceShown = 0;
  int phases = 0;
  int augmentations = 0;
  long long total = 0;

  cout << "\n--- Dinic from V" << s << " to V" << t << " ---\n";
  cout << "A level graph labels every reachable vertex with its distance in\n";
  cout << "arcs from the source. A blocking flow then uses only arcs that go\n";
  cout << "one level down, so every path it finds is a shortest path, and it\n";
  cout << "keeps going until no such path is left. Then a new BFS relabels.\n";

  while (true) {
    if (!buildLevels(s, t)) {
      say("  The sink is no longer reachable, so the level graph stops here.");
      break;
    }
    phases++;

    for (int i = 0; i < n; i++) ptr[i] = 0;
    int phaseFlow = 0;
    int phasePaths = 0;

    while (true) {
      dfsLen = 0;
      foundLen = 0;
      int sent = dfsBlock(s, t, 1 << 29);
      if (sent == 0) break;

      phaseFlow += sent;
      phasePaths++;
      augmentations++;
      total += sent;
      cout << "  Phase " << phases << " path: " << pathText(foundPath, foundLen)
           << ". Sent " << sent << ".\n";
    }

    cout << "  Phase " << phases << ": " << plural(phasePaths, "path", "paths")
         << ", " << plural(phaseFlow, "unit", "units") << ".\n";
    if (phaseFlow == 0) break;
  }

  cout << "\n--- Flow on each arc ---\n";
  for (int u = 0; u < n; u++) {
    for (int v = 0; v < n; v++) {
      if (!hasArc(u, v)) continue;
      cout << "  " << arcText(u, v) << ": " << flowM[u][v] << " of "
           << capM[u][v] << ".\n";
    }
  }

  cout << "\nMax flow = " << total << ".\n";
  cout << "BFS phases: " << plural(phases, "phase", "phases")
       << ". Augmenting paths: " << augmentations << ".\n";
  cout << "One BFS serves every path in its phase, which is the saving over a\n";
  cout << "method that runs a fresh search for each path.\n";
  if (traceShown >= TRACE_LIMIT) {
    cout << "(Trace stops after " << TRACE_LIMIT
         << " lines. The flow itself ran to the end.)\n";
  }
}

static void showCapacityMatrix() {
  cout << "\n--- Capacity matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  cout << "      ";
  for (int j = 0; j < n; j++) cout << setw(5) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << setw(4) << i << "  ";
    for (int j = 0; j < n; j++) cout << setw(5) << capM[i][j];
    cout << "\n";
  }
  cout << "A zero means no arc. The residual starts from this table, and the\n";
  cout << "level graph is read from the residual, not from the original arcs.\n";
}

static void showArcs() {
  cout << "\n--- Arc list ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  int shown = 0;
  for (int u = 0; u < n; u++) {
    for (int v = 0; v < n; v++) {
      if (!hasArc(u, v)) continue;
      shown++;
      cout << "  A" << (shown - 1) << ": " << arcText(u, v) << " capacity "
           << capM[u][v] << "\n";
    }
  }
  if (shown == 0) cout << "(Empty)\n";
  cout << "Dinic reads the residual of these arcs, so a reverse arc can appear\n";
  cout << "in a level graph even though it is not listed here.\n";
}

static void buildNetwork() {
  int fresh = readRange("Number of vertices (0.." + to_string(MAXV) + "): ", 0,
                        MAXV);
  resetNetwork();
  n = fresh;

  if (n == 0) {
    cout << "Build complete: 0 vertices, 0 arcs.\n";
    return;
  }

  int limit = n * (n - 1);
  cout << "Directed arcs only, vertices 0.." << (n - 1) << ".\n";
  cout << "A capacity is the most that may cross an arc, from 1 to 99.\n";

  while (true) {
    cout << "\nArc " << (arcCount + 1) << " (Limit " << limit
         << ", -1 to stop)\n";
    int u = readInt("  From vertex (0.." + to_string(n - 1) +
                    ", -1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To vertex (0.." + to_string(n - 1) + ", -1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: An arc cannot leave and enter the same vertex.\n";
      continue;
    }
    if (hasArc(u, v)) {
      cout << "  Rejected: " << arcText(u, v) << " already exists.\n";
      continue;
    }
    if (hasArc(v, u)) {
      cout << "  Rejected: The reverse " << arcText(v, u)
           << " already exists, and the residual builds its own reverse arc.\n";
      continue;
    }
    int capacity = readRange("  Capacity (1..99): ", 1, 99);
    addArc(u, v, capacity);
    cout << "  Added A" << (arcCount - 1) << " " << arcText(u, v)
         << " capacity " << capacity << "\n";
    cout << "  Arcs = " << arcCount << " now.\n";
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << arcCount
       << (arcCount == 1 ? " arc.\n" : " arcs.\n");
}

static void loadDiamondDemo() {
  resetNetwork();
  n = 4;
  cout << "Diamond demo: 4 vertices, 5 arcs.\n";
  cout << "  0 -> 1 (3), 0 -> 2 (2), 1 -> 2 (1), 1 -> 3 (2), 2 -> 3 (3).\n";
  cout << "  The first level graph sends four units along its two shortest\n";
  cout << "  paths. The second level graph sends the last unit along 1 -> 2.\n\n";

  addArc(0, 1, 3);
  addArc(0, 2, 2);
  addArc(1, 2, 1);
  addArc(1, 3, 2);
  addArc(2, 3, 3);

  cout << "Demo loaded: " << n << " vertices, " << arcCount << " arcs.\n";
}

static void loadLayeredDemo() {
  resetNetwork();
  n = 6;
  cout << "Layered demo: 6 vertices, 6 arcs.\n";
  cout << "  0 -> 1 (4), 0 -> 2 (3), 1 -> 3 (4), 2 -> 3 (3),\n";
  cout << "  3 -> 4 (10), 4 -> 5 (10).\n";
  cout << "  Every shortest path has the same length, so one level graph and\n";
  cout << "  one blocking flow send all seven units.\n\n";

  addArc(0, 1, 4);
  addArc(0, 2, 3);
  addArc(1, 3, 4);
  addArc(2, 3, 3);
  addArc(3, 4, 10);
  addArc(4, 5, 10);

  cout << "Demo loaded: " << n << " vertices, " << arcCount << " arcs.\n";
}

static void addArcInteractive() {
  if (n == 0) {
    cout << "Build the network first.\n";
    return;
  }
  if (arcCount == n * (n - 1)) {
    cout << "The network is already complete in both directions, so every\n";
    cout << "ordered pair of vertices has an arc.\n";
    return;
  }

  cout << "\nAdd an arc (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
  while (true) {
    int u = readInt("  From vertex (0.." + to_string(n - 1) +
                    ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To vertex (0.." + to_string(n - 1) +
                    ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: An arc cannot leave and enter the same vertex.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (hasArc(u, v)) {
      cout << "  Rejected: " << arcText(u, v) << " already exists.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (hasArc(v, u)) {
      cout << "  Rejected: The reverse " << arcText(v, u) << " already exists,\n";
      cout << "  and the residual builds its own reverse arc.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    int capacity = readRange("  Capacity (1..99): ", 1, 99);
    addArc(u, v, capacity);
    cout << "  Added A" << (arcCount - 1) << " " << arcText(u, v)
         << " capacity " << capacity << "\n";
    cout << "    capM[" << u << "][" << v << "] = " << capacity << "\n";
    cout << "    outAdj[" << u << "] gained V" << v << ".\n";
    cout << "  Arcs = " << arcCount << " now.\n";
    return;
  }
}

static void removeArcInteractive() {
  if (n == 0) {
    cout << "Build the network first.\n";
    return;
  }

  cout << "\nRemove an arc (V = " << n << ", so vertices 0.." << (n - 1)
       << ")\n";
  while (true) {
    int u = readInt("  From vertex (0.." + to_string(n - 1) +
                    ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To vertex (0.." + to_string(n - 1) +
                    ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (!hasArc(u, v)) {
      cout << "  Rejected: " << arcText(u, v) << " is not in the network.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }

    capM[u][v] = 0;
    for (int i = 0; i < (int)outAdj[u].size(); i++) {
      if (outAdj[u][i] != v) continue;
      outAdj[u].erase(outAdj[u].begin() + i);
      break;
    }
    arcCount--;

    cout << "  Removed " << arcText(u, v) << "\n";
    cout << "    capM[" << u << "][" << v << "] = 0\n";
    cout << "    outAdj[" << u << "] lost V" << v << ".\n";
    cout << "  Arcs = " << arcCount << " now.\n";
    return;
  }
}

static void changeCapacityInteractive() {
  if (n == 0) {
    cout << "Build the network first.\n";
    return;
  }

  cout << "\nChange a capacity (V = " << n << ", so vertices 0.." << (n - 1)
       << ")\n";
  while (true) {
    int u = readInt("  From vertex (0.." + to_string(n - 1) +
                    ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To vertex (0.." + to_string(n - 1) +
                    ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (!hasArc(u, v)) {
      cout << "  Rejected: " << arcText(u, v) << " is not in the network.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    int capacity = readRange("  New capacity (1..99): ", 1, 99);
    int before = capM[u][v];
    capM[u][v] = capacity;
    cout << "  " << arcText(u, v) << " goes from " << before << " to "
         << capacity << ".\n";
    cout << "    capM[" << u << "][" << v << "] holds " << capacity << " now.\n";
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
    capM[fresh][i] = 0;
    capM[i][fresh] = 0;
  }
  outAdj[fresh].clear();
  n++;

  cout << "\nAdded V" << fresh << " with no arcs.\n";
  cout << "  V = " << n << ", arcs = " << arcCount << ".\n";
  cout << "  It can be a source or a sink, but nothing reaches or leaves it\n";
  cout << "  until an arc is added.\n";
}

static void removeVertexInteractive() {
  if (n == 0) {
    cout << "Build the network first.\n";
    return;
  }

  int v;
  while (true) {
    v = readInt("  Vertex to remove (0.." + to_string(n - 1) +
                ", -1 to cancel): ");
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
      kept[na][nb] = capM[a][b];
    }
  }

  vector<int> keptAdj[MAXV];
  for (int a = 0; a < n; a++) {
    if (a == v) continue;
    int na = (a < v) ? a : a - 1;
    for (int w : outAdj[a]) {
      if (w == v) continue;
      keptAdj[na].push_back((w < v) ? w : w - 1);
    }
  }

  int before = arcCount;
  n--;
  for (int a = 0; a < MAXV; a++) {
    outAdj[a].clear();
    for (int b = 0; b < MAXV; b++) capM[a][b] = 0;
  }
  for (int a = 0; a < n; a++) {
    outAdj[a] = keptAdj[a];
    for (int b = 0; b < n; b++) capM[a][b] = kept[a][b];
  }

  arcCount = 0;
  for (int a = 0; a < n; a++) {
    for (int b = 0; b < n; b++) {
      if (capM[a][b] > 0) arcCount++;
    }
  }

  cout << "\nRemoved V" << v << ".\n";
  if (v < n) cout << "  Vertices above V" << v << " shift down by one.\n";
  cout << "  Arcs lost: " << before - arcCount;
  cout << " (Recounted from the matrix.)\n";
  cout << "  V = " << n << ", arcs = " << arcCount << ".\n";
}

static void clearArcs() {
  if (n == 0) {
    cout << "Build the network first.\n";
    return;
  }
  if (arcCount == 0) {
    cout << "\nThe network already has no arcs.\n";
    return;
  }

  int before = arcCount;
  for (int u = 0; u < n; u++) {
    outAdj[u].clear();
    for (int w = 0; w < n; w++) capM[u][w] = 0;
  }
  arcCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " arc.\n" : " arcs.\n");
  cout << "  V = " << n << ", arcs = 0. The vertices are still there.\n";
  cout << "  With no arcs the level graph holds only the source.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Network operations ---\n";
    cout << "Network: Directed | V = " << n << ", arcs = " << arcCount << "\n";
    cout << "1. Add an arc to the current network\n";
    cout << "2. Remove an arc from the current network\n";
    cout << "3. Change an arc capacity\n";
    cout << "4. Add a vertex to the current network\n";
    cout << "5. Remove a vertex from the current network\n";
    cout << "6. Clear all arcs\n";
    cout << "0. Back to the main menu\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      addArcInteractive();
    } else if (choice == 2) {
      removeArcInteractive();
    } else if (choice == 3) {
      changeCapacityInteractive();
    } else if (choice == 4) {
      addVertexInteractive();
    } else if (choice == 5) {
      removeVertexInteractive();
    } else if (choice == 6) {
      clearArcs();
    } else if (choice == 0) {
      return;
    } else {
      cout << "Invalid option.\n";
    }
  }
}

int main() {
  resetNetwork();

  while (true) {
    cout << "\n=== G47: Level Graph and Dinic ===\n";
    cout << "Network: Directed | V = " << n << ", arcs = " << arcCount << "\n";
    cout << "1. Build a directed network\n";
    cout << "2. Load the diamond demo (4 vertices)\n";
    cout << "3. Load the layered demo (6 vertices)\n";
    cout << "4. Network operations\n";
    cout << "5. Run Dinic\n";
    cout << "6. Show the capacity matrix\n";
    cout << "7. Show the arc list\n";
    cout << "8. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildNetwork();
    } else if (choice == 2) {
      loadDiamondDemo();
    } else if (choice == 3) {
      loadLayeredDemo();
    } else if (choice == 4) {
      operationsMenu();
    } else if (choice == 5) {
      runDinic();
    } else if (choice == 6) {
      showCapacityMatrix();
    } else if (choice == 7) {
      showArcs();
    } else if (choice == 8) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}

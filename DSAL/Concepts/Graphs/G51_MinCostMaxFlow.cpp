// 35. Min-cost max-flow, found by successive shortest augmenting paths:
// Send flow along the cheapest path in the residual, then repeat, letting
// reverse arcs carry a negative cost so a later unit can undo a dear earlier
// choice. Demo: A four-vertex network where the cheapest first path blocks the
// best second one, and the residual reroutes it for the same total cost.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;
const int TRACE_LIMIT = 80;
const int INF = 1000000;

static int n = 0;
static int arcCount = 0;
static int capM[MAXV][MAXV];
static int costM[MAXV][MAXV];
static vector<int> outAdj[MAXV];

static int resM[MAXV][MAXV];
static int flowM[MAXV][MAXV];
static int dist[MAXV];
static int parent[MAXV];
static bool inQueue[MAXV];
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

static string pathText(const vector<int> &path) {
  string text;
  for (int i = 0; i < (int)path.size(); i++) {
    if (i > 0) text += " -> ";
    text += "V" + to_string(path[i]);
  }
  return text;
}

static void resetNetwork() {
  for (int i = 0; i < MAXV; i++) {
    outAdj[i].clear();
    for (int j = 0; j < MAXV; j++) {
      capM[i][j] = 0;
      costM[i][j] = 0;
    }
  }
  n = 0;
  arcCount = 0;
}

static void addArc(int u, int v, int capacity, int cost) {
  capM[u][v] = capacity;
  costM[u][v] = cost;
  outAdj[u].push_back(v);
  arcCount++;
}

static bool hasArc(int u, int v) { return capM[u][v] > 0; }

static int residualCost(int u, int v) {
  if (capM[u][v] > 0) return costM[u][v];
  return -costM[v][u];
}

static bool shortestPath(int s, int t) {
  for (int i = 0; i < n; i++) {
    dist[i] = INF;
    parent[i] = -1;
    inQueue[i] = false;
  }

  int queue[MAXV * MAXV];
  int head = 0;
  int tail = 0;
  dist[s] = 0;
  queue[tail++] = s;
  inQueue[s] = true;

  while (head < tail) {
    int u = queue[head++];
    inQueue[u] = false;
    for (int v = 0; v < n; v++) {
      if (resM[u][v] <= 0) continue;
      int step = residualCost(u, v);
      if (dist[u] + step >= dist[v]) continue;
      dist[v] = dist[u] + step;
      parent[v] = u;
      if (!inQueue[v]) {
        inQueue[v] = true;
        queue[tail++] = v;
      }
    }
  }

  if (dist[t] == INF) {
    say("  No path from V" + to_string(s) + " to V" + to_string(t) +
        " is left in the residual.");
    return false;
  }
  return true;
}

static void runMinCostFlow() {
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

  cout << "\n--- Min-cost max-flow from V" << s << " to V" << t << " ---\n";
  cout << "Each round takes the cheapest path by cost, not the fewest arcs,\n";
  cout << "and pushes the largest amount that path can carry. A reverse arc\n";
  cout << "costs the negative of the arc it undoes, so a later round can move\n";
  cout << "flow off a dear arc without losing the units already sent.\n";

  long long totalFlow = 0;
  long long totalCost = 0;
  int rounds = 0;

  while (shortestPath(s, t)) {
    vector<int> path;
    for (int v = t; v != -1; v = parent[v]) path.push_back(v);
    for (int i = 0; i < (int)path.size() / 2; i++) {
      int tmp = path[i];
      path[i] = path[path.size() - 1 - i];
      path[path.size() - 1 - i] = tmp;
    }

    int push = INF;
    for (int i = 0; i + 1 < (int)path.size(); i++) {
      int u = path[i];
      int v = path[i + 1];
      if (resM[u][v] < push) push = resM[u][v];
    }

    rounds++;
    say("  Round " + to_string(rounds) + ": " + pathText(path) + ".");
    say("  Cost per unit: " + to_string(dist[t]) + ". Bottleneck: " +
        to_string(push) + ".");

    for (int i = 0; i + 1 < (int)path.size(); i++) {
      int u = path[i];
      int v = path[i + 1];
      int before = resM[u][v];
      int backBefore = resM[v][u];
      resM[u][v] -= push;
      resM[v][u] += push;
      if (capM[u][v] > 0) flowM[u][v] += push;
      else flowM[v][u] -= push;
      say("    " + arcText(u, v) + ": Residual " + to_string(before) + " -> " +
          to_string(resM[u][v]) + ", reverse " + to_string(backBefore) +
          " -> " + to_string(resM[v][u]) + ".");
    }

    totalFlow += push;
    totalCost += (long long)push * dist[t];
    cout << "  Pushed " << push << ": Flow " << totalFlow << ", cost "
         << totalCost << ".\n";
  }

  cout << "\n--- Flow on each arc ---\n";
  for (int u = 0; u < n; u++) {
    for (int v = 0; v < n; v++) {
      if (!hasArc(u, v)) continue;
      cout << "  " << arcText(u, v) << ": " << flowM[u][v] << " of "
           << capM[u][v] << ", cost " << costM[u][v] << " each.\n";
    }
  }

  cout << "\nMax flow = " << totalFlow << ". Min cost = " << totalCost
       << ".\n";
  cout << "The flow cannot grow, because no path to the sink is left. Among\n";
  cout << "all flows of that size, this one is cheapest: Every round added a\n";
  cout << "unit at the smallest price the residual allowed, and the reverse\n";
  cout << "arcs make undoing an earlier choice possible.\n";
  cout << "Augmenting rounds: " << plural(rounds, "round", "rounds") << ".\n";
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
  cout << "A zero means no arc. The residual starts here, and a reverse arc\n";
  cout << "of the residual is not shown in this table.\n";
}

static void showCostMatrix() {
  cout << "\n--- Cost matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  cout << "      ";
  for (int j = 0; j < n; j++) cout << setw(5) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << setw(4) << i << "  ";
    for (int j = 0; j < n; j++) cout << setw(5) << costM[i][j];
    cout << "\n";
  }
  cout << "Cost is read only where an arc exists. A reverse residual arc costs\n";
  cout << "the negative of the original arc, which is what lets flow retreat.\n";
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
           << capM[u][v] << ", cost " << costM[u][v] << "\n";
    }
  }
  if (shown == 0) cout << "(Empty)\n";
  cout << "A unit crossing an arc pays that cost, so a longer path can still\n";
  cout << "be cheaper than a short but dear one.\n";
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
  cout << "Capacity is the most that may cross an arc, from 1 to 99.\n";
  cout << "Cost is paid per unit, from 0 to 99.\n";

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
    int cost = readRange("  Cost (0..99): ", 0, 99);
    addArc(u, v, capacity, cost);
    cout << "  Added A" << (arcCount - 1) << " " << arcText(u, v)
         << " capacity " << capacity << ", cost " << cost << "\n";
    cout << "  Arcs = " << arcCount << " now.\n";
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << arcCount
       << (arcCount == 1 ? " arc.\n" : " arcs.\n");
}

static void loadRerouteDemo() {
  resetNetwork();
  n = 4;
  cout << "Reroute demo: 4 vertices, 5 arcs.\n";
  cout << "  0 -> 1 carries 1 at cost 0, and 0 -> 2 carries 1 at cost 1.\n";
  cout << "  1 -> 2 carries 1 at cost 0, and 1 -> 3 carries 1 at cost 1.\n";
  cout << "  2 -> 3 carries 1 at cost 0.\n";
  cout << "  The free path 0 - 1 - 2 - 3 goes first. The second round then\n";
  cout << "  uses 2 -> 1 backwards, so both units end up on the cheap arcs.\n\n";

  addArc(0, 1, 1, 0);
  addArc(0, 2, 1, 1);
  addArc(1, 2, 1, 0);
  addArc(1, 3, 1, 1);
  addArc(2, 3, 1, 0);

  cout << "Demo loaded: " << n << " vertices, " << arcCount << " arcs.\n";
}

static void loadCapacityDemo() {
  resetNetwork();
  n = 5;
  cout << "Capacity demo: 5 vertices, 5 arcs.\n";
  cout << "  0 -> 1 carries 2 at cost 1, and 0 -> 2 carries 1 at cost 2.\n";
  cout << "  1 -> 2 carries 1 at cost 0, and 1 -> 3 carries 2 at cost 2.\n";
  cout << "  2 -> 3 carries 2 at cost 1.\n";
  cout << "  One unit takes the cheap 1 -> 2 detour, and the capacity that is\n";
  cout << "  left on the direct arcs carries the rest.\n\n";

  addArc(0, 1, 2, 1);
  addArc(0, 2, 1, 2);
  addArc(1, 2, 1, 0);
  addArc(1, 3, 2, 2);
  addArc(2, 3, 2, 1);

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
    int cost = readRange("  Cost (0..99): ", 0, 99);
    addArc(u, v, capacity, cost);
    cout << "  Added A" << (arcCount - 1) << " " << arcText(u, v)
         << " capacity " << capacity << ", cost " << cost << "\n";
    cout << "    capM[" << u << "][" << v << "] = " << capacity
         << ", costM[" << u << "][" << v << "] = " << cost << "\n";
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
    costM[u][v] = 0;
    for (int i = 0; i < (int)outAdj[u].size(); i++) {
      if (outAdj[u][i] != v) continue;
      outAdj[u].erase(outAdj[u].begin() + i);
      break;
    }
    arcCount--;

    cout << "  Removed " << arcText(u, v) << "\n";
    cout << "    capM[" << u << "][" << v << "] = 0, costM[" << u << "][" << v
         << "] = 0\n";
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

static void changeCostInteractive() {
  if (n == 0) {
    cout << "Build the network first.\n";
    return;
  }

  cout << "\nChange a cost (V = " << n << ", so vertices 0.." << (n - 1)
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
    int cost = readRange("  New cost (0..99): ", 0, 99);
    int before = costM[u][v];
    costM[u][v] = cost;
    cout << "  " << arcText(u, v) << " goes from cost " << before << " to "
         << cost << ".\n";
    cout << "    costM[" << u << "][" << v << "] holds " << cost << " now.\n";
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
    costM[fresh][i] = 0;
    costM[i][fresh] = 0;
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

  int keptCap[MAXV][MAXV];
  int keptCost[MAXV][MAXV];
  for (int a = 0; a < n; a++) {
    for (int b = 0; b < n; b++) {
      keptCap[a][b] = 0;
      keptCost[a][b] = 0;
    }
  }
  for (int a = 0; a < n; a++) {
    if (a == v) continue;
    for (int b = 0; b < n; b++) {
      if (b == v) continue;
      int na = (a < v) ? a : a - 1;
      int nb = (b < v) ? b : b - 1;
      keptCap[na][nb] = capM[a][b];
      keptCost[na][nb] = costM[a][b];
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
    for (int b = 0; b < MAXV; b++) {
      capM[a][b] = 0;
      costM[a][b] = 0;
    }
  }
  for (int a = 0; a < n; a++) {
    outAdj[a] = keptAdj[a];
    for (int b = 0; b < n; b++) {
      capM[a][b] = keptCap[a][b];
      costM[a][b] = keptCost[a][b];
    }
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
    for (int w = 0; w < n; w++) {
      capM[u][w] = 0;
      costM[u][w] = 0;
    }
  }
  arcCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " arc.\n" : " arcs.\n");
  cout << "  V = " << n << ", arcs = 0. The vertices are still there.\n";
  cout << "  With no arcs the flow is 0 and so is the cost.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Network operations ---\n";
    cout << "Network: Directed | V = " << n << ", arcs = " << arcCount << "\n";
    cout << "1. Add an arc to the current network\n";
    cout << "2. Remove an arc from the current network\n";
    cout << "3. Change an arc capacity\n";
    cout << "4. Change an arc cost\n";
    cout << "5. Add a vertex to the current network\n";
    cout << "6. Remove a vertex from the current network\n";
    cout << "7. Clear all arcs\n";
    cout << "0. Back to the main menu\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      addArcInteractive();
    } else if (choice == 2) {
      removeArcInteractive();
    } else if (choice == 3) {
      changeCapacityInteractive();
    } else if (choice == 4) {
      changeCostInteractive();
    } else if (choice == 5) {
      addVertexInteractive();
    } else if (choice == 6) {
      removeVertexInteractive();
    } else if (choice == 7) {
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
    cout << "\n=== G51: Min-Cost Max-Flow ===\n";
    cout << "Network: Directed | V = " << n << ", arcs = " << arcCount << "\n";
    cout << "1. Build a directed network\n";
    cout << "2. Load the reroute demo (4 vertices)\n";
    cout << "3. Load the capacity demo (5 vertices)\n";
    cout << "4. Network operations\n";
    cout << "5. Run min-cost max-flow\n";
    cout << "6. Show the capacity matrix\n";
    cout << "7. Show the cost matrix\n";
    cout << "8. Show the arc list\n";
    cout << "9. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildNetwork();
    } else if (choice == 2) {
      loadRerouteDemo();
    } else if (choice == 3) {
      loadCapacityDemo();
    } else if (choice == 4) {
      operationsMenu();
    } else if (choice == 5) {
      runMinCostFlow();
    } else if (choice == 6) {
      showCapacityMatrix();
    } else if (choice == 7) {
      showCostMatrix();
    } else if (choice == 8) {
      showArcs();
    } else if (choice == 9) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}

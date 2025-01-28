// 31b. The travelling salesman, by branch and bound: Walk out from city 0,
// adding one city at a time, and stop a branch as soon as its bound says the
// rest of the loop cannot beat the best tour already found. The bound is the
// cheapest road leaving each city still to visit. Demo: Five towns where a
// greedy first tour costs 72 and the bound then proves that 57 is the best
// there is.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXC = 10;
const long long INF = 1000000000000LL;
const int TRACE_LIMIT = 80;

static int n = 0;
static int edgeCount = 0;
static long long distM[MAXC][MAXC];

static bool used[MAXC];
static vector<int> path;
static long long pathCost = 0;
static long long bestCost = INF;
static vector<int> bestPath;

static long long nodes = 0;
static long long cuts = 0;
static long long updates = 0;
static long long toursSeen = 0;
static long long shown = 0;

static long long greedyCost = INF;
static vector<int> greedyPath;
static long long seedNodes = -1;
static long long rawNodes = -1;

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

static void resetTSP() {
  for (int i = 0; i < MAXC; i++) {
    for (int j = 0; j < MAXC; j++) distM[i][j] = INF;
  }
  n = 0;
  edgeCount = 0;
  bestCost = INF;
  bestPath.clear();
  greedyCost = INF;
  greedyPath.clear();
  seedNodes = -1;
  rawNodes = -1;
}

static void addRoad(int u, int v, long long w) {
  distM[u][v] = w;
  distM[v][u] = w;
  edgeCount++;
}

static bool hasRoad(int u, int v) { return distM[u][v] < INF; }

static string roadText(int u, int v) {
  return "(" + to_string(u) + " - " + to_string(v) + ")";
}

static string plural(int count, const string &one, const string &many) {
  return to_string(count) + " " + (count == 1 ? one : many);
}

static string tourText(const vector<int> &tour) {
  string text;
  for (int i = 0; i < (int)tour.size(); i++) {
    if (i > 0) text += " - ";
    text += to_string(tour[i]);
  }
  return text;
}

static void say(const string &line) {
  if (shown >= TRACE_LIMIT) return;
  shown++;
  cout << line << "\n";
}

static void showMatrix() {
  cout << "\n--- Distance matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  cout << "      ";
  for (int j = 0; j < n; j++) cout << setw(5) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << setw(4) << i << "  ";
    for (int j = 0; j < n; j++) {
      if (i == j) {
        cout << setw(5) << "-";
      } else if (hasRoad(i, j)) {
        cout << setw(5) << distM[i][j];
      } else {
        cout << setw(5) << "..";
      }
    }
    cout << "\n";
  }
  cout << "A dash is the same city twice and two dots mean no road. The table is\n";
  cout << "symmetric, since a road costs the same both ways.\n";
}

static void showEdges() {
  cout << "\n--- Road list ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  int id = 0;
  for (int u = 0; u < n; u++) {
    for (int v = u + 1; v < n; v++) {
      if (!hasRoad(u, v)) continue;
      cout << setw(3) << ("E" + to_string(id)) << ": " << roadText(u, v)
           << " costs " << distM[u][v] << "\n";
      id++;
    }
  }
  if (id == 0) cout << "(No roads)\n";
  cout << "V = " << n << ", E = " << edgeCount << ".\n";
}

static void buildTSP() {
  int fresh = readRange("Number of cities (0.." + to_string(MAXC) + "): ", 0,
                        MAXC);
  resetTSP();
  n = fresh;

  if (n == 0) {
    cout << "Build complete: 0 cities, 0 roads.\n";
    return;
  }

  int limit = n * (n - 1) / 2;
  cout << "Roads are undirected, and a missing pair simply has no road.\n";

  while (true) {
    cout << "\nRoad " << (edgeCount + 1) << " (Limit " << limit
         << ", -1 to stop)\n";
    if (edgeCount == limit) {
      cout << "Every pair already has a road, so the graph is complete.\n";
      break;
    }
    int u = readInt("  From city (0.." + to_string(n - 1) + ", -1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To city (0.." + to_string(n - 1) + ", -1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Cities must be in 0.." << (n - 1) << ".\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: A city needs no road to itself.\n";
      continue;
    }
    if (hasRoad(u, v)) {
      cout << "  Rejected: " << roadText(u, v) << " already exists.\n";
      continue;
    }
    int w = readRange("  Cost (1..99): ", 1, 99);
    addRoad(u, v, w);
    cout << "  Added E" << (edgeCount - 1) << " " << roadText(u, v)
         << " costing " << w << "\n";
  }

  cout << "\nBuild complete: " << plural(n, "city", "cities") << ", "
       << plural(edgeCount, "road", "roads") << ".\n";
}

static void loadMapDemo() {
  resetTSP();
  n = 5;
  cout << "Map demo: 5 towns, 10 roads, every pair joined.\n";
  cout << "  The five towns of the Held-Karp file, with rounded distances:\n";
  cout << "  V0 (7, 14), V1 (3, 4), V2 (9, 7), V3 (19, 0), V4 (0, 18).\n";
  cout << "  The best tour costs 57 and greed pays 72, so the search starts with\n";
  cout << "  a tour it already knows and then proves that nothing beats 57.\n\n";

  addRoad(0, 1, 11);
  addRoad(0, 2, 7);
  addRoad(0, 3, 18);
  addRoad(0, 4, 8);
  addRoad(1, 2, 7);
  addRoad(1, 3, 16);
  addRoad(1, 4, 14);
  addRoad(2, 3, 12);
  addRoad(2, 4, 14);
  addRoad(3, 4, 26);

  cout << "Demo loaded: " << n << " cities, " << edgeCount << " roads.\n";
}

static void loadRingDemo() {
  resetTSP();
  n = 7;
  cout << "Seven-town demo: 7 towns, 18 roads.\n";
  cout << "  A ring with shortcuts, and the three long diagonals 0 - 3, 1 - 4 and\n";
  cout << "  2 - 5 left out. A tour must therefore follow the ring's order at\n";
  cout << "  least partly, which gives the bound plenty to cut.\n\n";

  addRoad(0, 1, 12);
  addRoad(1, 2, 15);
  addRoad(2, 3, 10);
  addRoad(3, 4, 14);
  addRoad(4, 5, 11);
  addRoad(5, 6, 13);
  addRoad(6, 0, 16);

  addRoad(0, 2, 26);
  addRoad(0, 4, 33);
  addRoad(0, 5, 35);
  addRoad(1, 3, 25);
  addRoad(1, 5, 31);
  addRoad(1, 6, 29);
  addRoad(2, 4, 24);
  addRoad(2, 6, 29);
  addRoad(3, 5, 25);
  addRoad(3, 6, 24);
  addRoad(4, 6, 27);

  cout << "Demo loaded: " << n << " cities, " << edgeCount << " roads.\n";
}

static int gridStep(int dr, int dc) {
  if (dr < 0) dr = -dr;
  if (dc < 0) dc = -dc;
  static const int table[9] = {0, 5, 10, 5, 7, 11, 10, 11, 14};
  return table[dr * 3 + dc];
}

static void loadGridDemo() {
  resetTSP();
  n = 9;
  cout << "Grid demo: 9 towns, 36 roads, every pair joined.\n";
  cout << "  Nine towns on a 3 by 3 grid, five apart:\n";
  cout << "    V0 V1 V2    across the top,    V3 V4 V5 across the middle,\n";
  cout << "    V6 V7 V8    across the bottom.\n";
  cout << "  Straight-line distances round to 5 for a step, 7 for a diagonal of\n";
  cout << "  one square, 10 for two steps, 11 for a knight's move and 14 for the\n";
  cout << "  long diagonal.\n\n";

  for (int u = 0; u < 9; u++) {
    for (int v = u + 1; v < 9; v++) {
      addRoad(u, v, gridStep(u / 3 - v / 3, u % 3 - v % 3));
    }
  }

  cout << "Demo loaded: " << n << " cities, " << edgeCount << " roads.\n";
}

static bool greedyTour() {
  greedyPath.clear();
  greedyCost = INF;
  if (n == 0) return false;

  vector<bool> seen(n, false);
  greedyPath.push_back(0);
  seen[0] = true;

  long long total = 0;
  int current = 0;
  for (int step = 1; step < n; step++) {
    int next = -1;
    for (int u = 0; u < n; u++) {
      if (seen[u]) continue;
      if (!hasRoad(current, u)) continue;
      if (next == -1 || distM[current][u] < distM[current][next]) next = u;
    }
    if (next == -1) return false;
    total += distM[current][next];
    seen[next] = true;
    greedyPath.push_back(next);
    current = next;
  }

  if (!hasRoad(current, 0)) return false;
  total += distM[current][0];
  greedyPath.push_back(0);
  greedyCost = total;
  return true;
}

static long long lowerBound(int at) {
  long long total = pathCost;

  if ((int)path.size() < n) {
    long long cheapest = INF;
    for (int u = 1; u < n; u++) {
      if (used[u]) continue;
      if (distM[at][u] < cheapest) cheapest = distM[at][u];
    }
    if (cheapest >= INF) return INF;
    total += cheapest;
  }

  for (int u = 1; u < n; u++) {
    if (used[u]) continue;
    long long cheapest = INF;
    for (int v = 0; v < n; v++) {
      if (v == u) continue;
      if (used[v] && v != 0) continue;
      if (distM[u][v] < cheapest) cheapest = distM[u][v];
    }
    if (cheapest >= INF) return INF;
    total += cheapest;
  }

  return total;
}

static void explore(int depth) {
  int at = path.back();
  nodes++;
  long long bound = lowerBound(at);
  string pad(depth * 2, ' ');

  say(pad + "At V" + to_string(at) + " on " + tourText(path) + ": Cost " +
      to_string(pathCost) + ", bound " + to_string(bound) + ", best " +
      (bestCost >= INF ? string("(None)") : to_string(bestCost)) + ".");

  if ((int)path.size() == n) {
    if (!hasRoad(at, 0)) {
      say(pad + "  No road home from V" + to_string(at) + ", so this path dies.");
      return;
    }
    long long total = pathCost + distM[at][0];
    toursSeen++;
    if (total < bestCost) {
      bestCost = total;
      bestPath = path;
      bestPath.push_back(0);
      updates++;
      say(pad + "  Home road costs " + to_string(distM[at][0]) +
          ": A full tour for " + to_string(total) + ".");
    }
    return;
  }

  if (bound >= bestCost) {
    cuts++;
    say(pad + "  Bound " + to_string(bound) + " cannot beat " +
        to_string(bestCost) + ", so this whole branch is dead.");
    return;
  }

  for (int u = 1; u < n; u++) {
    if (used[u]) continue;
    if (!hasRoad(at, u)) continue;
    used[u] = true;
    path.push_back(u);
    pathCost += distM[at][u];
    say(pad + "  Take V" + to_string(u) + " for " + to_string(distM[at][u]) +
        ", so the path costs " + to_string(pathCost) + ".");
    explore(depth + 1);
    pathCost -= distM[at][u];
    path.pop_back();
    used[u] = false;
  }
}

static void runBranchBound(bool withSeed) {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  bestCost = INF;
  bestPath.clear();
  path.clear();
  path.push_back(0);
  pathCost = 0;
  nodes = 0;
  cuts = 0;
  updates = 0;
  toursSeen = 0;
  shown = 0;
  for (int v = 0; v < n; v++) used[v] = (v == 0);

  cout << "\n--- Branch and bound ---\n";
  cout << "The tour grows from city 0 one city at a time. At every step the bound\n";
  cout << "adds up the cheapest road leaving the city we stand on and the cheapest\n";
  cout << "road leaving each city still to visit, because every one of those roads\n";
  cout << "has to appear in the rest of the loop. That sum can only understate the\n";
  cout << "truth, so a branch whose sum already reaches the best tour is dead.\n";

  if (withSeed) {
    if (greedyTour()) {
      bestCost = greedyCost;
      bestPath = greedyPath;
      cout << "\nGreedy first tour: " << tourText(bestPath) << " for "
           << bestCost << ".\n";
      cout << "That number is the first best, so every branch must beat it.\n";
    } else {
      cout << "\nGreed found no tour, so the search starts without a first best.\n";
    }
  } else {
    cout << "\nNo greedy tour this time: The first full loop found becomes the\n";
    cout << "best, and until then nothing can be cut.\n";
  }

  if (n == 1) {
    bestCost = 0;
    bestPath.clear();
    bestPath.push_back(0);
    cout << "\nOne city needs no travelling, so the tour is 0 and it costs 0.\n";
  } else {
    cout << "\n";
    explore(0);
  }

  if (shown >= TRACE_LIMIT) {
    cout << "\n(Trace stops after " << TRACE_LIMIT
         << " lines. The search itself ran to the end.)\n";
  }

  cout << "\n--- Result ---\n";
  if (bestPath.empty()) {
    cout << "No full tour exists: Some city cannot be reached along a closed\n";
    cout << "route, so this graph has no travelling salesman answer.\n";
    return;
  }

  cout << "Best tour: " << tourText(bestPath) << "\n";
  cout << "Cost: " << bestCost << ".\n";
  cout << "Nodes visited: " << nodes << ", cuts: " << cuts
       << ", new bests: " << updates << ".\n";
  cout << "Complete loops examined: " << toursSeen << ".\n";

  if (updates == 0 && withSeed) {
    cout << "\nThe greedy tour was never beaten, so the whole search went into\n";
    cout << "proving that nothing cheaper exists.\n";
  }

  if (withSeed) {
    seedNodes = nodes;
  } else {
    rawNodes = nodes;
  }

  if (seedNodes >= 0 && rawNodes >= 0) {
    cout << "\nSeeded run: " << seedNodes << " nodes. Unseeded run: " << rawNodes
         << " nodes.\n";
    if (seedNodes == rawNodes) {
      cout << "The greedy start made no difference here: The search found a\n";
      cout << "tour just as good by itself, and found it early.\n";
    } else if (seedNodes < rawNodes) {
      cout << "The greedy start saved "
           << plural(rawNodes - seedNodes, "node", "nodes")
           << ", so a good first best paid off.\n";
    } else {
      cout << "The greedy start added "
           << plural(seedNodes - rawNodes, "node", "nodes")
           << " here, so a first best is no promise of less work.\n";
    }
  }

  if (cuts == 0) {
    cout << "\nNothing was cut, so every partial loop was followed to its end.\n";
    cout << "That happens when no first best arrives early enough to compare\n";
    cout << "against.\n";
  } else {
    cout << "\nEvery cut removed a whole tree of loops, not just one, because the\n";
    cout << "branches below it could only cost more. That is why the bound pays\n";
    cout << "for itself even though it never checks a single road by itself.\n";
  }
}

static void addRoadInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == n * (n - 1) / 2) {
    cout << "Every pair already has a road.\n";
    return;
  }

  cout << "\nAdd a road (V = " << n << ", so cities 0.." << (n - 1) << ")\n";
  while (true) {
    int u = readInt("  From city (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To city (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Cities must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this road.\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: A city needs no road to itself.\n";
      cout << "  Please re-enter this road.\n";
      continue;
    }
    if (hasRoad(u, v)) {
      cout << "  Rejected: " << roadText(u, v) << " already exists.\n";
      cout << "  Please re-enter this road.\n";
      continue;
    }

    int w = readRange("  Cost (1..99): ", 1, 99);
    addRoad(u, v, w);
    seedNodes = -1;
    rawNodes = -1;
    cout << "  Added E" << (edgeCount - 1) << " " << roadText(u, v)
         << " costing " << w << "\n";
    cout << "    distM[" << u << "][" << v << "] and distM[" << v << "][" << u
         << "] both cost " << w << ".\n";
    cout << "  E = " << plural(edgeCount, "road", "roads") << " now.\n";
    return;
  }
}

static void removeRoadInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove a road (V = " << n << ", so cities 0.." << (n - 1) << ")\n";
  while (true) {
    int u = readInt("  From city (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To city (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Cities must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this road.\n";
      continue;
    }
    if (!hasRoad(u, v)) {
      cout << "  Rejected: " << roadText(u, v) << " is not there.\n";
      cout << "  Please re-enter this road.\n";
      continue;
    }

    distM[u][v] = INF;
    distM[v][u] = INF;
    edgeCount--;
    seedNodes = -1;
    rawNodes = -1;

    cout << "  Removed " << roadText(u, v) << "\n";
    cout << "    distM[" << u << "][" << v << "] and distM[" << v << "][" << u
         << "] are back to no road.\n";
    cout << "  E = " << plural(edgeCount, "road", "roads") << " now.\n";
    return;
  }
}

static void changeRoadInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nChange a road cost\n";
  while (true) {
    int u = readInt("  From city (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To city (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Cities must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this road.\n";
      continue;
    }
    if (!hasRoad(u, v)) {
      cout << "  Rejected: " << roadText(u, v) << " is not there.\n";
      cout << "  Please re-enter this road.\n";
      continue;
    }

    long long before = distM[u][v];
    int w = readRange("  New cost (1..99): ", 1, 99);
    distM[u][v] = w;
    distM[v][u] = w;
    seedNodes = -1;
    rawNodes = -1;

    cout << "  " << roadText(u, v) << " goes from " << before << " to " << w
         << ".\n";
    cout << "    distM[" << u << "][" << v << "] and distM[" << v << "][" << u
         << "] both hold " << w << " now.\n";
    return;
  }
}

static void addCityInteractive() {
  if (n >= MAXC) {
    cout << "City limit reached: MAXC = " << MAXC << ".\n";
    return;
  }

  int fresh = n;
  for (int i = 0; i <= fresh; i++) {
    distM[fresh][i] = INF;
    distM[i][fresh] = INF;
  }
  n++;
  seedNodes = -1;
  rawNodes = -1;

  cout << "\nAdded V" << fresh << " with no roads.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
  cout << "  An isolated city makes a full tour impossible until a road\n";
  cout << "  reaches it.\n";
}

static void removeCityInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  int v;
  while (true) {
    v = readInt("  City to remove (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (v >= 0 && v < n) break;
    cout << "  Rejected: City must be in 0.." << (n - 1) << ".\n";
  }

  vector<int> keptFrom;
  vector<int> keptTo;
  vector<long long> keptCost;
  for (int u = 0; u < n; u++) {
    if (u == v) continue;
    for (int w = u + 1; w < n; w++) {
      if (w == v) continue;
      if (!hasRoad(u, w)) continue;
      keptFrom.push_back(u < v ? u : u - 1);
      keptTo.push_back(w < v ? w : w - 1);
      keptCost.push_back(distM[u][w]);
    }
  }

  int before = edgeCount;
  int newN = n - 1;
  resetTSP();
  n = newN;

  for (int i = 0; i < (int)keptFrom.size(); i++) {
    addRoad(keptFrom[i], keptTo[i], keptCost[i]);
  }

  cout << "\nRemoved V" << v << ".\n";
  if (v < n) cout << "  Cities above V" << v << " shift down by one.\n";
  cout << "  Roads lost: " << before - edgeCount << ".\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
}

static void clearRoads() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nThe graph already has no roads.\n";
    return;
  }

  int before = edgeCount;
  for (int u = 0; u < n; u++) {
    for (int w = 0; w < n; w++) distM[u][w] = INF;
  }
  edgeCount = 0;
  seedNodes = -1;
  rawNodes = -1;

  cout << "\nRemoved all " << plural(before, "road", "roads") << ".\n";
  cout << "  V = " << n << ", E = 0. The cities are still there.\n";
  cout << "  With no roads no tour exists, and the search will say so.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Undirected, weighted | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Add a road\n";
    cout << "2. Remove a road\n";
    cout << "3. Change a road cost\n";
    cout << "4. Add a city\n";
    cout << "5. Remove a city\n";
    cout << "6. Clear all roads\n";
    cout << "0. Back to the main menu\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      addRoadInteractive();
    } else if (choice == 2) {
      removeRoadInteractive();
    } else if (choice == 3) {
      changeRoadInteractive();
    } else if (choice == 4) {
      addCityInteractive();
    } else if (choice == 5) {
      removeCityInteractive();
    } else if (choice == 6) {
      clearRoads();
    } else if (choice == 0) {
      return;
    } else {
      cout << "Invalid option.\n";
    }
  }
}

int main() {
  resetTSP();

  while (true) {
    cout << "\n=== G45: Travelling Salesman (Branch and Bound) ===\n";
    cout << "Graph: Undirected, weighted | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Build a connected set of cities\n";
    cout << "2. Load the map demo (5 cities)\n";
    cout << "3. Load the seven-town demo (7 cities)\n";
    cout << "4. Load the grid demo (9 cities)\n";
    cout << "5. Graph operations\n";
    cout << "6. Search with a greedy first tour\n";
    cout << "7. Search from scratch\n";
    cout << "8. Show the distance matrix\n";
    cout << "9. Show the road list\n";
    cout << "10. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildTSP();
    } else if (choice == 2) {
      loadMapDemo();
    } else if (choice == 3) {
      loadRingDemo();
    } else if (choice == 4) {
      loadGridDemo();
    } else if (choice == 5) {
      operationsMenu();
    } else if (choice == 6) {
      runBranchBound(true);
    } else if (choice == 7) {
      runBranchBound(false);
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

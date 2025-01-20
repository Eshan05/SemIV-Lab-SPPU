// 16. Tour operator organizes guided bus trips across the Maharashtra. Tourists may have different preferences. Tour operator offers a choice from many different routes. Every day the bus moves from starting city S to another city F as chosen by client. On this way, the tourists can see the sights alongside the route traveled from S to F. Client may have preference to choose route. There is a restriction on the routes that the tourists may choose from, the bus has to take a short route from S to F or a route having one distance unit longer than the minimal distance. Two routes from S to F are considered different if there is at least one road from a city A to a city B which is part of one route, but not of the other route.

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

namespace {

  constexpr int MAXV = 30;

  struct Edge {
    int to;
    int w; // positive integer distance
  };

  struct Graph {
    bool directed = false;
    bool weighted = false; // if false, treat all edges as w=1
    vector<string> names;
    unordered_map<string, int> idx;
    vector<vector<Edge>> adj;
    vector<vector<Edge>> rev; // reverse edges (for directed). For undirected it's same as adj.

    void clear() {
      directed = false;
      weighted = false;
      names.clear();
      idx.clear();
      adj.clear();
      rev.clear();
    }

    int n() const { return static_cast<int>(names.size()); }

    int findCity(const string &name) const {
      auto it = idx.find(name);
      if (it == idx.end()) return -1;
      return it->second;
    }

    bool addCity(const string &name) {
      if (n() >= MAXV) return false;
      if (idx.find(name) != idx.end()) return false;
      int id = n();
      names.push_back(name);
      idx[name] = id;
      adj.push_back({});
      rev.push_back({});
      return true;
    }

    static bool upsertEdge(vector<Edge> &list, int to, int w) {
      for (auto &e : list) {
        if (e.to == to) {
          e.w = w;
          return true;
        }
      }
      list.push_back({ to, w });
      return true;
    }

    static bool eraseEdge(vector<Edge> &list, int to) {
      for (size_t i = 0; i < list.size(); ++i) {
        if (list[i].to == to) {
          list.erase(list.begin() + static_cast<long long>(i));
          return true;
        }
      }
      return false;
    }

    bool addRoad(int u, int v, int w) {
      if (u < 0 || v < 0 || u >= n() || v >= n() || u == v) return false;
      if (w < 1) return false;
      if (!weighted) w = 1;

      upsertEdge(adj[u], v, w);
      upsertEdge(rev[v], u, w);
      if (!directed) {
        upsertEdge(adj[v], u, w);
        upsertEdge(rev[u], v, w);
      }
      return true;
    }

    bool removeRoad(int u, int v) {
      if (u < 0 || v < 0 || u >= n() || v >= n() || u == v) return false;
      bool removed = false;
      removed |= eraseEdge(adj[u], v);
      removed |= eraseEdge(rev[v], u);
      if (!directed) {
        removed |= eraseEdge(adj[v], u);
        removed |= eraseEdge(rev[u], v);
      }
      return removed;
    }

    bool allWeightsOne() const {
      for (int u = 0; u < n(); ++u) {
        for (const auto &e : adj[u]) {
          if (e.w != 1) return false;
        }
      }
      return true;
    }

    void display() const {
      if (n() == 0) {
        cout << "Graph empty\n";
        return;
      }
      cout << "\nCities:\n";
      for (int i = 0; i < n(); ++i) cout << i << " -> " << names[i] << "\n";

      cout << "\nRoads:\n";
      for (int u = 0; u < n(); ++u) {
        for (const auto &e : adj[u]) {
          cout << names[u] << " -> " << names[e.to];
          if (weighted) cout << " (w=" << e.w << ")";
          cout << "\n";
        }
      }
    }
  };

  struct Settings {
    bool verbose = true;
    int maxPrintedRoutes = 200;
    int maxTraceLines = 400;
  };

  void clearLine() {
    char c;
    while (cin.get(c)) {
      if (c == '\n') break;
    }
  }

  int readInt(const string &prompt) {
    int x;
    while (true) {
      cout << prompt;
      if (cin >> x) return x;
      cout << "Invalid number. Try again.\n";
      cin.clear();
      clearLine();
    }
  }

  string readLine(const string &prompt) {
    cout << prompt;
    string s;
    getline(cin >> ws, s);
    return s;
  }

  bool readBool01(const string &prompt) {
    int x = readInt(prompt);
    return x == 1;
  }

  void printPath(const Graph &g, const vector<int> &path) {
    for (size_t i = 0; i < path.size(); ++i) {
      cout << g.names[path[i]];
      if (i + 1 < path.size()) cout << " -> ";
    }
  }

  // -------------------------- Unweighted BFS helpers --------------------------

  vector<int> bfsUnweighted(const vector<vector<Edge>> &adj, int s, bool verbose, const Graph &g, const string &label) {
    int n = static_cast<int>(adj.size());
    vector<int> dist(n, -1);
    queue<int> q;
    dist[s] = 0;
    q.push(s);
    if (verbose) {
      cout << "\n[BFS] " << label << "\n";
      cout << "Start: " << g.names[s] << " (dist=0)\n";
    }
    while (!q.empty()) {
      int u = q.front();
      q.pop();
      if (verbose) cout << "Pop " << g.names[u] << ", dist=" << dist[u] << "\n";
      for (const auto &e : adj[u]) {
        int v = e.to;
        if (dist[v] != -1) continue;
        dist[v] = dist[u] + 1;
        q.push(v);
        if (verbose) cout << "  Discover " << g.names[v] << " with dist=" << dist[v] << " via " << g.names[u] << "\n";
      }
    }
    return dist;
  }

  // -------------------------- Bidirectional BFS (unweighted) --------------------------

  struct BiBfsResult {
    int dist = -1; // in number of roads
    int expandedF = 0;
    int expandedB = 0;
  };

  BiBfsResult bidirectionalBfsUnweighted(const Graph &g, int s, int t, const Settings &st) {
    BiBfsResult res;
    int n = g.n();
    vector<int> distF(n, -1), distB(n, -1);
    queue<int> qF, qB;
    distF[s] = 0;
    distB[t] = 0;
    qF.push(s);
    qB.push(t);

    int best = numeric_limits<int>::max();

    if (st.verbose) {
      cout << "\n=== Algorithm 1: Bidirectional BFS (unweighted) ===\n";
      cout << "Distance meaning: each road counts as 1 unit.\n";
      cout << "Goal: find d(S,F) fast by searching from BOTH ends until frontiers meet.\n\n";
      cout << "Step 1) Forward frontier starts at S=" << g.names[s] << "\n";
      cout << "Step 2) Backward frontier starts at F=" << g.names[t] << " (using reverse edges if directed)\n";
      cout << "Step 3) Expand one layer at a time; when a city is seen from both sides, we have a candidate distance.\n";
    }

    auto expandOne = [&](queue<int> &q, vector<int> &distThis, const vector<int> &distOther, const vector<vector<Edge>> &adjThis, int &expandedCounter, const string &sideName) {
      int layerSize = static_cast<int>(q.size());
      if (st.verbose) cout << "\nExpand " << sideName << " side (" << layerSize << " node(s) in this layer)\n";
      for (int i = 0; i < layerSize; ++i) {
        int u = q.front();
        q.pop();
        expandedCounter++;
        if (st.verbose) cout << "  Pop " << g.names[u] << " (dist=" << distThis[u] << ")\n";

        if (distOther[u] != -1) {
          best = min(best, distThis[u] + distOther[u]);
          if (st.verbose) cout << "    Meeting at " << g.names[u] << ": " << distThis[u] << "+" << distOther[u] << " = " << (distThis[u] + distOther[u]) << "\n";
        }

        for (const auto &e : adjThis[u]) {
          int v = e.to;
          if (distThis[v] != -1) continue;
          distThis[v] = distThis[u] + 1;
          q.push(v);
          if (st.verbose) cout << "    Discover " << g.names[v] << " (dist=" << distThis[v] << ") from " << g.names[u] << "\n";
          if (distOther[v] != -1) {
            best = min(best, distThis[v] + distOther[v]);
            if (st.verbose) cout << "      Meeting at " << g.names[v] << ": " << distThis[v] << "+" << distOther[v] << " = " << (distThis[v] + distOther[v]) << "\n";
          }
        }
      }
    };

    // Simple, safe loop (n<=30): alternate expanding layers until both queues empty or best can't improve.
    while (!qF.empty() && !qB.empty()) {
      int frontF = distF[qF.front()];
      int frontB = distB[qB.front()];
      if (best != numeric_limits<int>::max() && frontF + frontB >= best) break;

      if (qF.size() <= qB.size()) expandOne(qF, distF, distB, g.adj, res.expandedF, "FORWARD");
      else expandOne(qB, distB, distF, g.rev, res.expandedB, "BACKWARD");
    }

    if (best == numeric_limits<int>::max()) best = -1;
    res.dist = best;

    if (st.verbose) {
      cout << "\nResult: d(S,F) = " << res.dist << " road(s)\n";
      cout << "Expanded nodes: forward=" << res.expandedF << ", backward=" << res.expandedB << "\n";
    }

    return res;
  }

  // -------------------------- Dijkstra (weighted) --------------------------

  constexpr long long INF = (numeric_limits<long long>::max() / 4);

  struct DijkstraResult {
    vector<long long> dist;
    vector<int> parent; // single parent for demonstration (NOT enough to reconstruct all shortest paths)
    int popCount = 0;
    int relaxCount = 0;
  };

  DijkstraResult dijkstra(const vector<vector<Edge>> &adj, int s, const Graph &g, const Settings &st, const string &label) {
    int n = static_cast<int>(adj.size());
    DijkstraResult out;
    out.dist.assign(n, INF);
    out.parent.assign(n, -1);

    using P = pair<long long, int>; // (dist, node)
    priority_queue<P, vector<P>, greater<P>> pq;
    out.dist[s] = 0;
    pq.push({ 0, s });

    if (st.verbose) {
      cout << "\n[Dijkstra] " << label << "\n";
      cout << "Start: " << g.names[s] << " (dist=0)\n";
      cout << "Rule: repeatedly pick the unprocessed city with smallest tentative distance, then relax outgoing roads.\n";
    }

    while (!pq.empty()) {
      auto [d, u] = pq.top();
      pq.pop();
      out.popCount++;
      if (d != out.dist[u]) continue; // stale
      if (st.verbose) cout << "Pop " << g.names[u] << " with confirmed dist=" << d << "\n";

      for (const auto &e : adj[u]) {
        if (e.w < 1) continue; // safety
        int v = e.to;
        long long cand = d + static_cast<long long>(e.w);
        out.relaxCount++;
        if (st.verbose) {
          cout << "  Try edge " << g.names[u] << " -> " << g.names[v] << " (w=" << e.w << ")";
          cout << ": candidate dist=" << cand;
          if (cand < out.dist[v]) cout << "  [IMPROVE]\n";
          else cout << "  [no improve]\n";
        }
        if (cand < out.dist[v]) {
          out.dist[v] = cand;
          out.parent[v] = u;
          pq.push({ cand, v });
        }
      }
    }

    return out;
  }

  // -------------------------- Modified DFS + pruning (length > D+1) --------------------------

  struct RouteStats {
    long long shortestLen = -1;
    long long shortestCount = 0;
    long long plusOneCount = 0;
    long long printedShortest = 0;
    long long printedPlusOne = 0;
    bool outputTruncated = false;
  };

  RouteStats countRoutesByDfsBoundOnly(const Graph &g, int s, int t, long long D) {
    RouteStats stats;
    stats.shortestLen = D;
    if (D < 0) return stats;

    int n = g.n();
    vector<int> used(n, 0);
    used[s] = 1;

    function<void(int, long long)> dfs = [&](int u, long long len) {
      if (len > D + 1) return;
      if (u == t) {
        if (len == D) stats.shortestCount++;
        else if (len == D + 1) stats.plusOneCount++;
        return;
      }
      for (const auto &e : g.adj[u]) {
        int v = e.to;
        if (used[v]) continue;
        long long w = g.weighted ? e.w : 1;
        used[v] = 1;
        dfs(v, len + w);
        used[v] = 0;
      }
    };

    dfs(s, 0);
    return stats;
  }

  struct DfsTraceLimiter {
    int printed = 0;
    int maxLines = 0;
    bool verbose = false;

    bool canPrint() {
      if (!verbose) return false;
      if (printed >= maxLines) return false;
      printed++;
      return true;
    }
  };

  void dfsEnumerateBounded(
      const Graph &g,
      int s,
      int t,
      long long D,
      const Settings &st,
      RouteStats &stats,
      vector<int> &path,
      vector<int> &used,
      long long currentLen,
      DfsTraceLimiter &trace) {
    if (currentLen > D + 1) {
      if (trace.canPrint()) {
        cout << "Prune: length " << currentLen << " > " << (D + 1) << " | path=";
        printPath(g, path);
        cout << "\n";
      }
      return;
    }

    int u = path.back();
    if (u == t) {
      if (currentLen == D) {
        stats.shortestCount++;
        if (stats.printedShortest < st.maxPrintedRoutes) {
          stats.printedShortest++;
          cout << "Shortest route " << stats.shortestCount << ": ";
          printPath(g, path);
          cout << "\n";
        } else {
          stats.outputTruncated = true;
        }
      } else if (currentLen == D + 1) {
        stats.plusOneCount++;
        if (stats.printedPlusOne < st.maxPrintedRoutes) {
          stats.printedPlusOne++;
          cout << "+1 route " << stats.plusOneCount << ": ";
          printPath(g, path);
          cout << "\n";
        } else {
          stats.outputTruncated = true;
        }
      }
      return;
    }

    for (const auto &e : g.adj[u]) {
      int v = e.to;
      if (used[v]) continue;
      long long w = g.weighted ? e.w : 1;
      used[v] = 1;
      path.push_back(v);
      if (trace.canPrint()) {
        cout << "Go: " << g.names[u] << " -> " << g.names[v] << " (add " << w << ")";
        cout << " | newLen=" << (currentLen + w) << "\n";
      }
      dfsEnumerateBounded(g, s, t, D, st, stats, path, used, currentLen + w, trace);
      path.pop_back();
      used[v] = 0;
    }
  }

  RouteStats solveByModifiedDFSPruning(const Graph &g, int s, int t, long long D, const Settings &st, const string &label) {
    RouteStats stats;
    stats.shortestLen = D;

    if (D < 0) return stats;

    cout << "\n=== Algorithm: " << label << " ===\n";
    cout << "We will enumerate all SIMPLE routes from S to F, but prune early:\n";
    cout << "  If current_length > d(S,F)+1, stop exploring that partial route.\n";
    cout << "Allowed route lengths: " << D << " and " << (D + 1) << "\n\n";

    vector<int> path;
    vector<int> used(g.n(), 0);
    path.push_back(s);
    used[s] = 1;

    DfsTraceLimiter trace;
    trace.verbose = st.verbose;
    trace.maxLines = st.maxTraceLines;

    dfsEnumerateBounded(g, s, t, D, st, stats, path, used, 0, trace);

    cout << "\nSummary (" << label << "):\n";
    cout << "Shortest routes count (len=" << D << "): " << stats.shortestCount << "\n";
    cout << "+1 routes count (len=" << (D + 1) << "): " << stats.plusOneCount << "\n";
    cout << "Total valid routes: " << (stats.shortestCount + stats.plusOneCount) << "\n";
    if (stats.outputTruncated) {
      cout << "(Note) Output truncated: increased maxPrintedRoutes to print more.\n";
    }
    return stats;
  }

  // -------------------------- DP on shortest path DAG (shortest only) --------------------------

  struct DagResult {
    long long shortestDist = -1;
    vector<long long> ways;
    vector<vector<int>> dag;
  };

  DagResult buildShortestPathDagAndCount(const Graph &g, int s, int t, const vector<long long> &distS, const vector<long long> &distToT, const Settings &st) {
    DagResult out;
    int n = g.n();
    out.ways.assign(n, 0);
    out.dag.assign(n, {});
    if (distS[t] >= INF) {
      out.shortestDist = -1;
      return out;
    }
    out.shortestDist = distS[t];

    if (st.verbose) {
      cout << "\n=== Algorithm 4: DP on the shortest-path DAG (shortest only) ===\n";
      cout << "Idea: keep ONLY 'tight' roads that can appear on a shortest route.\n";
      cout << "Tight road condition: distS[u] + w(u,v) + distToF[v] == distS[F]\n";
      cout << "This forms a DAG (dist strictly increases), so we can DP-count paths.\n\n";
    }

    for (int u = 0; u < n; ++u) {
      if (distS[u] >= INF) continue;
      for (const auto &e : g.adj[u]) {
        int v = e.to;
        long long w = g.weighted ? e.w : 1;
        if (distS[v] >= INF || distToT[v] >= INF) continue;
        if (distS[u] + w + distToT[v] == distS[t]) {
          out.dag[u].push_back(v);
        }
      }
    }

    // Process nodes in increasing distS.
    vector<int> order(n);
    for (int i = 0; i < n; ++i) order[i] = i;
    sort(order.begin(), order.end(), [&](int a, int b) {
      if (distS[a] != distS[b]) return distS[a] < distS[b];
      return a < b;
    });

    out.ways[s] = 1;
    if (st.verbose) {
      cout << "DP table fill (ways[u] = number of shortest routes from S to u):\n";
      cout << "Initialize ways[" << g.names[s] << "] = 1\n\n";
    }
    for (int u : order) {
      if (distS[u] >= INF) continue;
      if (out.ways[u] == 0) continue;
      if (st.verbose) cout << "At city " << g.names[u] << " (dist=" << distS[u] << ", ways=" << out.ways[u] << ")\n";
      for (int v : out.dag[u]) {
        out.ways[v] += out.ways[u];
        if (st.verbose) cout << "  Propagate to " << g.names[v] << ": ways now " << out.ways[v] << "\n";
      }
    }

    if (st.verbose) {
      cout << "\nResult: number of SHORTEST routes = " << out.ways[t] << "\n";
      cout << "(This algorithm counts shortest routes only; it does not count +1 routes.)\n";
    }

    return out;
  }

  void enumerateDagPaths(
      const Graph &g,
      int u,
      int t,
      const vector<vector<int>> &dag,
      vector<int> &path,
      long long &printed,
      long long maxPrint,
      long long &total) {
    if (u == t) {
      total++;
      if (printed < maxPrint) {
        printed++;
        cout << "Shortest route " << total << ": ";
        printPath(g, path);
        cout << "\n";
      }
      return;
    }
    for (int v : dag[u]) {
      path.push_back(v);
      enumerateDagPaths(g, v, t, dag, path, printed, maxPrint, total);
      path.pop_back();
    }
  }

  // -------------------------- Layered / state-space graph (d and d+1) --------------------------

  struct LayeredResult {
    long long D = -1;
    unsigned long long waysF0 = 0;
    unsigned long long waysF1 = 0;
  };

  struct StateEdge {
    int toState;
    int toNode;
    int toExtra;
    long long newLen;
    int roadW;
  };

  LayeredResult countByLayeredStateSpaceDPOnly(const Graph &g, int s, int t, const vector<long long> &distS) {
    LayeredResult out;
    int n = g.n();
    if (distS.empty() || distS[t] >= INF) {
      out.D = -1;
      return out;
    }
    out.D = distS[t];

    auto stateId = [&](int node, int extra) {
      return node * 2 + extra;
    };
    int S0 = stateId(s, 0);
    int F0 = stateId(t, 0);
    int F1 = stateId(t, 1);

    vector<vector<StateEdge>> stAdj(2 * n);
    vector<long long> stLen(2 * n, INF);

    for (int u = 0; u < n; ++u) {
      if (distS[u] >= INF) continue;
      for (int extra = 0; extra <= 1; ++extra) {
        long long lenHere = distS[u] + extra;
        int from = stateId(u, extra);
        stLen[from] = lenHere;
        if (lenHere > out.D + 1) continue;
        for (const auto &e : g.adj[u]) {
          int v = e.to;
          if (distS[v] >= INF) continue;
          long long w = g.weighted ? e.w : 1;
          long long newLen = lenHere + w;
          if (newLen > out.D + 1) continue;
          long long newExtraLL = newLen - distS[v];
          if (newExtraLL < 0 || newExtraLL > 1) continue;
          int newExtra = static_cast<int>(newExtraLL);
          int to = stateId(v, newExtra);
          stAdj[from].push_back({ to, v, newExtra, newLen, static_cast<int>(w) });
        }
      }
    }

    vector<int> states;
    states.reserve(2 * n);
    for (int node = 0; node < n; ++node) {
      if (distS[node] >= INF) continue;
      for (int extra = 0; extra <= 1; ++extra) {
        long long L = distS[node] + extra;
        if (L <= out.D + 1) states.push_back(stateId(node, extra));
      }
    }
    sort(states.begin(), states.end(), [&](int a, int b) {
      if (stLen[a] != stLen[b]) return stLen[a] < stLen[b];
      return a < b;
    });

    vector<unsigned long long> ways(2 * n, 0);
    ways[S0] = 1;
    for (int stFrom : states) {
      if (ways[stFrom] == 0) continue;
      for (const auto &se : stAdj[stFrom]) ways[se.toState] += ways[stFrom];
    }

    out.waysF0 = ways[F0];
    out.waysF1 = ways[F1];
    return out;
  }

  LayeredResult solveByLayeredStateSpace(const Graph &g, int s, int t, const vector<long long> &distS, const Settings &st) {
    LayeredResult out;
    int n = g.n();
    if (distS[t] >= INF) {
      out.D = -1;
      return out;
    }
    out.D = distS[t];

    if (st.verbose) {
      cout << "\n=== Algorithm 3: Layered / State-space graph (counts d and d+1) ===\n";
      cout << "Key idea: for each city u we keep TWO states:\n";
      cout << "  (u,0) = reached u with total length = distS[u] (no extra used)\n";
      cout << "  (u,1) = reached u with total length = distS[u]+1 (used the +1 slack)\n";
      cout << "We only allow transitions that keep extra <= 1.\n";
      cout << "This captures exactly routes of length d and d+1 to F.\n\n";
      cout << "Shortest distance d(S,F) = " << out.D << "\n";
      cout << "Allowed total lengths: " << out.D << " and " << (out.D + 1) << "\n\n";
    }

    auto stateId = [&](int node, int extra) {
      return node * 2 + extra;
    };
    int S0 = stateId(s, 0);
    int F0 = stateId(t, 0);
    int F1 = stateId(t, 1);

    vector<vector<StateEdge>> stAdj(2 * n);
    vector<long long> stLen(2 * n, INF);

    for (int u = 0; u < n; ++u) {
      if (distS[u] >= INF) continue;
      for (int extra = 0; extra <= 1; ++extra) {
        long long lenHere = distS[u] + extra;
        int from = stateId(u, extra);
        stLen[from] = lenHere;
        if (lenHere > out.D + 1) continue; // can't be part of any valid solution

        for (const auto &e : g.adj[u]) {
          int v = e.to;
          if (distS[v] >= INF) continue;
          long long w = g.weighted ? e.w : 1;
          long long newLen = lenHere + w;
          if (newLen > out.D + 1) continue;
          long long newExtraLL = newLen - distS[v];
          if (newExtraLL < 0 || newExtraLL > 1) continue;
          int newExtra = static_cast<int>(newExtraLL);
          int to = stateId(v, newExtra);
          stAdj[from].push_back({ to, v, newExtra, newLen, static_cast<int>(w) });
        }
      }
    }

    // Topological order by increasing state-length.
    vector<int> states;
    states.reserve(2 * n);
    for (int node = 0; node < n; ++node) {
      if (distS[node] >= INF) continue;
      for (int extra = 0; extra <= 1; ++extra) {
        long long L = distS[node] + extra;
        if (L <= out.D + 1) states.push_back(stateId(node, extra));
      }
    }
    sort(states.begin(), states.end(), [&](int a, int b) {
      if (stLen[a] != stLen[b]) return stLen[a] < stLen[b];
      return a < b;
    });

    vector<unsigned long long> ways(2 * n, 0);
    ways[S0] = 1;
    if (st.verbose) {
      cout << "DP over the state graph (ways[state] = number of ways to reach that state):\n";
      cout << "Start at (" << g.names[s] << ", extra=0) with ways=1\n\n";
    }

    for (int stFrom : states) {
      if (ways[stFrom] == 0) continue;
      int u = stFrom / 2;
      int extra = stFrom % 2;
      if (st.verbose) {
        cout << "State (" << g.names[u] << ", extra=" << extra << ")";
        cout << " means totalLen=" << stLen[stFrom] << ", ways=" << ways[stFrom] << "\n";
      }
      for (const auto &se : stAdj[stFrom]) {
        ways[se.toState] += ways[stFrom];
        if (st.verbose) {
          cout << "  via " << g.names[u] << " -> " << g.names[se.toNode] << " (w=" << se.roadW << ")";
          cout << " => (" << g.names[se.toNode] << ", extra=" << se.toExtra << ")";
          cout << " totalLen=" << se.newLen;
          cout << " | ways now " << ways[se.toState] << "\n";
        }
      }
    }

    out.waysF0 = ways[F0];
    out.waysF1 = ways[F1];

    cout << "\nCounts from layered/state-space DP:\n";
    cout << "Shortest routes (len=" << out.D << "): " << out.waysF0 << "\n";
    cout << "+1 routes (len=" << (out.D + 1) << "): " << out.waysF1 << "\n";
    cout << "Total valid routes: " << (out.waysF0 + out.waysF1) << "\n";

    // Enumerate actual routes by DFS on the state graph (acyclic by length).
    cout << "\nListing routes (first " << st.maxPrintedRoutes << " of each kind):\n";
    unsigned long long totalShortest = 0;
    unsigned long long totalPlusOne = 0;
    unsigned long long printedShortest = 0;
    unsigned long long printedPlusOne = 0;

    vector<int> path;
    path.push_back(s);

    function<void(int)> dfsState = [&](int stHere) {
      int u = stHere / 2;
      int extra = stHere % 2;
      if (u == t) {
        if (extra == 0) {
          totalShortest++;
          if (printedShortest < static_cast<unsigned long long>(st.maxPrintedRoutes)) {
            printedShortest++;
            cout << "Shortest route " << totalShortest << ": ";
            printPath(g, path);
            cout << "\n";
          }
        } else {
          totalPlusOne++;
          if (printedPlusOne < static_cast<unsigned long long>(st.maxPrintedRoutes)) {
            printedPlusOne++;
            cout << "+1 route " << totalPlusOne << ": ";
            printPath(g, path);
            cout << "\n";
          }
        }
        return;
      }
      for (const auto &se : stAdj[stHere]) {
        path.push_back(se.toNode);
        dfsState(se.toState);
        path.pop_back();
      }
    };

    dfsState(S0);

    if ((totalShortest != out.waysF0) || (totalPlusOne != out.waysF1)) {
      cout << "\n(Note) Enumeration totals differ from DP counts.";
      cout << " This can happen if output was limited or if the graph contains edges with invalid weights.";
      cout << "\n";
    }

    return out;
  }

  // -------------------------- Utility: compute shortest distances (BFS or Dijkstra) --------------------------

  vector<long long> distFromS(const Graph &g, int s, const Settings &st, bool useWeights) {
    if (!useWeights) {
      vector<int> d = bfsUnweighted(g.adj, s, st.verbose, g, "Compute distS (unweighted)");
      vector<long long> out(d.size(), INF);
      for (size_t i = 0; i < d.size(); ++i)
        if (d[i] != -1) out[i] = d[i];
      return out;
    }
    return dijkstra(g.adj, s, g, st, "Compute distS (weighted)").dist;
  }

  vector<long long> distToTarget(const Graph &g, int t, const Settings &st, bool useWeights) {
    if (!useWeights) {
      vector<int> d = bfsUnweighted(g.rev, t, st.verbose, g, "Compute distToF (unweighted, via reverse graph)");
      vector<long long> out(d.size(), INF);
      for (size_t i = 0; i < d.size(); ++i)
        if (d[i] != -1) out[i] = d[i];
      return out;
    }
    return dijkstra(g.rev, t, g, st, "Compute distToF (weighted, via reverse graph)").dist;
  }

  // -------------------------- TUI build graph --------------------------

  void buildGraph(Graph &g) {
    g.clear();
    int n = readInt("Enter number of cities (max 30): ");
    if (n < 0) n = 0;
    if (n > MAXV) n = MAXV;

    g.directed = readBool01("Directed roads? (1=yes, 0=no): ");
    g.weighted = readBool01("Weighted roads? (1=yes, 0=no): ");

    for (int i = 0; i < n; ++i) {
      while (true) {
        string name = readLine("City " + to_string(i) + " name: ");
        if (name.empty()) {
          cout << "  Name cannot be empty.\n";
          continue;
        }
        if (!g.addCity(name)) {
          cout << "  Duplicate name (or max cities reached). Try again.\n";
          continue;
        }
        break;
      }
    }

    int e = readInt("Enter number of roads: ");
    for (int i = 0; i < e; ++i) {
      cout << "Road " << (i + 1) << ":\n";
      string a = readLine("  From city: ");
      string b = readLine("  To city: ");
      int u = g.findCity(a);
      int v = g.findCity(b);
      if (u == -1 || v == -1) {
        cout << "  Invalid city name, road skipped\n";
        continue;
      }
      int w = 1;
      if (g.weighted) w = readInt("  Distance w (positive int): ");
      if (!g.addRoad(u, v, w)) cout << "  Invalid road, skipped\n";
    }
    cout << "Graph built\n";
  }

  void addRoadInteractive(Graph &g) {
    if (g.n() == 0) {
      cout << "Build graph first\n";
      return;
    }
    string a = readLine("From city: ");
    string b = readLine("To city: ");
    int u = g.findCity(a);
    int v = g.findCity(b);
    if (u == -1 || v == -1) {
      cout << "Invalid city name\n";
      return;
    }
    int w = 1;
    if (g.weighted) w = readInt("Distance w (positive int): ");
    if (g.addRoad(u, v, w)) cout << "Road added/updated\n";
    else cout << "Failed to add road\n";
  }

  void removeRoadInteractive(Graph &g) {
    if (g.n() == 0) {
      cout << "Build graph first\n";
      return;
    }
    string a = readLine("From city: ");
    string b = readLine("To city: ");
    int u = g.findCity(a);
    int v = g.findCity(b);
    if (u == -1 || v == -1) {
      cout << "Invalid city name\n";
      return;
    }
    if (g.removeRoad(u, v)) cout << "Road removed\n";
    else cout << "No such road\n";
  }

  bool pickSAndF(const Graph &g, int &s, int &f) {
    if (g.n() == 0) {
      cout << "Build graph first\n";
      return false;
    }
    string sName = readLine("Start city S: ");
    string fName = readLine("Destination city F: ");
    s = g.findCity(sName);
    f = g.findCity(fName);
    if (s == -1 || f == -1) {
      cout << "Invalid city name\n";
      return false;
    }
    return true;
  }

  // -------------------------- Run algorithms --------------------------

  void runBidirectionalBfsSolver(const Graph &g, const Settings &st) {
    int s, f;
    if (!pickSAndF(g, s, f)) return;

    BiBfsResult bibfs = bidirectionalBfsUnweighted(g, s, f, st);
    if (bibfs.dist == -1) {
      cout << "No route exists (unweighted road-count metric).\n";
      return;
    }

    // Enumerate using DFS with pruning on road-count length.
    Graph gu = g;
    gu.weighted = false; // force w=1 in DFS
    RouteStats stats = solveByModifiedDFSPruning(gu, s, f, bibfs.dist, st, "Bidirectional BFS + DFS enumeration (road-count) ");
    (void)stats;
  }

  void runDijkstraWeightedSolver(const Graph &g, const Settings &st) {
    int s, f;
    if (!pickSAndF(g, s, f)) return;

    cout << "\n=== Algorithm 2: Dijkstra (weighted roads) ===\n";
    cout << "Distance meaning: sum of road weights. (All weights must be positive.)\n";
    cout << "We first compute shortest distance d(S,F) using Dijkstra, then list routes with total length d or d+1.\n";

    DijkstraResult dj = dijkstra(g.adj, s, g, st, "Run Dijkstra from S");
    long long D = dj.dist[f] >= INF ? -1 : dj.dist[f];
    if (D == -1) {
      cout << "No route exists (weighted metric).\n";
      return;
    }
    cout << "\nResult: shortest weighted distance d(S,F) = " << D << "\n";
    cout << "Dijkstra stats: pop=" << dj.popCount << ", relaxAttempts=" << dj.relaxCount << "\n";

    solveByModifiedDFSPruning(g, s, f, D, st, "Dijkstra + DFS enumeration (weighted) ");
  }

  void runLayeredStateSpaceSolver(const Graph &g, const Settings &st) {
    int s, f;
    if (!pickSAndF(g, s, f)) return;

    bool useWeights = g.weighted;
    vector<long long> dS = distFromS(g, s, st, useWeights);
    if (dS[f] >= INF) {
      cout << "No route exists in this metric.\n";
      return;
    }

    solveByLayeredStateSpace(g, s, f, dS, st);
  }

  void runDagDpSolver(const Graph &g, const Settings &st) {
    int s, f;
    if (!pickSAndF(g, s, f)) return;

    bool useWeights = g.weighted;
    vector<long long> dS = distFromS(g, s, st, useWeights);
    vector<long long> dToF = distToTarget(g, f, st, useWeights);

    if (dS[f] >= INF) {
      cout << "No route exists in this metric.\n";
      return;
    }

    DagResult dag = buildShortestPathDagAndCount(g, s, f, dS, dToF, st);

    cout << "\nListing shortest routes using the DAG (first " << st.maxPrintedRoutes << "):\n";
    vector<int> path;
    path.push_back(s);
    long long printed = 0;
    long long total = 0;
    enumerateDagPaths(g, s, f, dag.dag, path, printed, st.maxPrintedRoutes, total);
    cout << "\nEnumeration summary:\n";
    cout << "Total shortest routes found = " << total << "\n";
    if (total != dag.ways[f]) {
      cout << "(Note) DP count was " << dag.ways[f] << ", enumeration got " << total << ".\n";
      cout << "If output was truncated, enumeration still counts total but only prints first N.\n";
    }
  }

  void runModifiedDfsSolver(const Graph &g, const Settings &st) {
    int s, f;
    if (!pickSAndF(g, s, f)) return;

    bool useWeights = g.weighted;
    vector<long long> dS = distFromS(g, s, st, useWeights);
    if (dS[f] >= INF) {
      cout << "No route exists in this metric.\n";
      return;
    }
    long long D = dS[f];
    solveByModifiedDFSPruning(g, s, f, D, st, "Modified DFS + pruning by (d+1) bound");
  }

  void runAllAndCompare(const Graph &g, const Settings &st) {
    int s, f;
    if (!pickSAndF(g, s, f)) return;

    cout << "\n=== Run ALL algorithms and compare ===\n";
    cout << "Note: Bidirectional BFS uses road-count distance (each road=1).\n";
    cout << "      Dijkstra/Layered/DP-DAG/DFS use weighted metric if graph is weighted.\n\n";

    // Quiet settings for internal computations
    Settings stQuiet = st;
    stQuiet.verbose = false;

    // 1) Road-count metric (each road=1)
    Graph gu = g;
    gu.weighted = false;

    BiBfsResult bibfs = bidirectionalBfsUnweighted(gu, s, f, stQuiet);
    long long D_road = bibfs.dist;
    RouteStats statsRoad = (D_road == -1) ? RouteStats{} : countRoutesByDfsBoundOnly(gu, s, f, D_road);
    LayeredResult layeredRoad;
    unsigned long long dagRoadShortest = 0;
    if (D_road != -1) {
      vector<long long> dS = distFromS(gu, s, stQuiet, false);
      layeredRoad = countByLayeredStateSpaceDPOnly(gu, s, f, dS);
      vector<long long> dToF = distToTarget(gu, f, stQuiet, false);
      dagRoadShortest = static_cast<unsigned long long>(buildShortestPathDagAndCount(gu, s, f, dS, dToF, stQuiet).ways[f]);
    }

    // 2) Weighted metric (sum of weights if graph is weighted)
    long long D_w = -1;
    RouteStats statsW;
    LayeredResult layeredW;
    unsigned long long dagWShortest = 0;
    {
      bool useWeights = g.weighted;
      vector<long long> dS = distFromS(g, s, stQuiet, useWeights);
      if (dS[f] < INF) {
        D_w = dS[f];
        statsW = countRoutesByDfsBoundOnly(g, s, f, D_w);
        layeredW = countByLayeredStateSpaceDPOnly(g, s, f, dS);
        vector<long long> dToF = distToTarget(g, f, stQuiet, useWeights);
        dagWShortest = static_cast<unsigned long long>(buildShortestPathDagAndCount(g, s, f, dS, dToF, stQuiet).ways[f]);
      }
    }

    cout << "\n--- Comparison Summary ---\n";
    cout << "Road-count metric (each road=1):\n";
    cout << "  d(S,F) from Bidirectional BFS = " << D_road << "\n";
    cout << "  DFS+prune counts: shortest=" << statsRoad.shortestCount << ", +1=" << statsRoad.plusOneCount << "\n";
    cout << "  Layered DP counts:  shortest=" << layeredRoad.waysF0 << ", +1=" << layeredRoad.waysF1 << "\n";
    cout << "  Shortest-path DAG DP (shortest only) = " << dagRoadShortest << "\n\n";
    cout << "Weight metric (sum of weights if enabled):\n";
    cout << "  d(S,F) from Dijkstra/BFS = " << D_w << "\n";
    cout << "  DFS+prune counts: shortest=" << statsW.shortestCount << ", +1=" << statsW.plusOneCount << "\n";
    cout << "  Layered DP counts:  shortest=" << layeredW.waysF0 << ", +1=" << layeredW.waysF1 << "\n";
    cout << "  Shortest-path DAG DP (shortest only) = " << dagWShortest << "\n";
    cout << "\nHow to read this:\n";
    cout << "- If the graph is unweighted, all methods should agree on d and the counts.\n";
    cout << "- If the graph is weighted, Bidirectional BFS is solving a DIFFERENT problem (min #roads, not min total weight).\n";
    cout << "- DP-DAG counts only shortest routes; Layered + DFS count both shortest and shortest+1.\n";
  }

} // namespace

int main() {
  Graph g;
  Settings st;

  while (true) {
    cout << "\n===== C16: All Algorithms Demo (STL) =====\n";
    cout << "1. Build/Rebuild Graph\n";
    cout << "2. Add/Update Road\n";
    cout << "3. Remove Road\n";
    cout << "4. Display Graph\n";
    cout << "5. Run: Bidirectional BFS (unweighted)\n";
    cout << "6. Run: Dijkstra (weighted roads)\n";
    cout << "7. Run: Layered / State-space graph\n";
    cout << "8. Run: DP on shortest-path DAG (shortest only)\n";
    cout << "9. Run: Modified DFS + Pruning (bound d+1)\n";
    cout << "10. Run ALL and compare\n";
    cout << "11. Toggle verbose explanations (currently: " << (st.verbose ? "ON" : "OFF") << ")\n";
    cout << "12. Set max printed routes (currently: " << st.maxPrintedRoutes << ")\n";
    cout << "13. Exit\n";

    int ch = readInt("Enter choice: ");

    if (ch == 1) {
      buildGraph(g);
    } else if (ch == 2) {
      addRoadInteractive(g);
    } else if (ch == 3) {
      removeRoadInteractive(g);
    } else if (ch == 4) {
      g.display();
    } else if (ch == 5) {
      runBidirectionalBfsSolver(g, st);
    } else if (ch == 6) {
      runDijkstraWeightedSolver(g, st);
    } else if (ch == 7) {
      runLayeredStateSpaceSolver(g, st);
    } else if (ch == 8) {
      runDagDpSolver(g, st);
    } else if (ch == 9) {
      runModifiedDfsSolver(g, st);
    } else if (ch == 10) {
      runAllAndCompare(g, st);
    } else if (ch == 11) {
      st.verbose = !st.verbose;
      cout << "Verbose is now: " << (st.verbose ? "ON" : "OFF") << "\n";
    } else if (ch == 12) {
      int x = readInt("New maxPrintedRoutes: ");
      if (x < 1) x = 1;
      st.maxPrintedRoutes = x;
      cout << "Updated.\n";
    } else if (ch == 13) {
      break;
    } else {
      cout << "Invalid choice\n";
    }
  }

  return 0;
}

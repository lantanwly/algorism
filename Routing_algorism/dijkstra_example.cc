// Simple, well-documented Dijkstra example (adjacency list + priority_queue)
// Input format:
//   n m
//   m lines: u v w   (1-based nodes, undirected edges)
//   s               (source node)
// Output:
//   n distances from s: dist[1] dist[2] ... dist[n]
//   unreachable nodes printed as INF

#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const ll INF = (1LL<<60);

int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    bool debug = (argc > 1 && string(argv[1]) == "debug");

    int n, m;
    if(!(cin >> n >> m)) return 0; // no input

    vector<vector<pair<int,int>>> g(n+1);
    for(int i=0;i<m;i++){
        int u,v,w; cin >> u >> v >> w;
        if(u<1 || u>n || v<1 || v>n) continue; // simple guard
        g[u].push_back({v,w});
        g[v].push_back({u,w}); // comment this line if graph is directed
    }

    int s; cin >> s;
    if(s < 1 || s > n){
        cerr << "Invalid source node\n";
        return 0;
    }

    vector<ll> dist(n+1, INF);
    dist[s] = 0;
    if(debug){
        cerr << "[debug] initialized dist: ";
        for(int i=1;i<=n;i++){
            if(dist[i]==INF) cerr << "INF "; else cerr << dist[i] << ' ';
        }
        cerr << "\n";
    }
    // predecessor array for path reconstruction
    vector<int> pre(n+1, -1);

    // min-heap: (distance, node)
    priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> pq;
    pq.push({0, s});

    while(!pq.empty()){
        auto [d,u] = pq.top(); pq.pop();
        if(debug) cerr << "[debug] pop (d=" << d << ", u=" << u << ")\n";
        if(d != dist[u]){
            if(debug) cerr << "[debug]  stale entry for u=" << u << ", d=" << d << ", current dist[u]=" << dist[u] << " - skip\n";
            continue; // stale entry
        }
        for(auto &e : g[u]){
            int v = e.first; int w = e.second;
            if(dist[v] > d + w){
                ll old = dist[v];
                dist[v] = d + w;
                pre[v] = u; // record predecessor for path
                pq.push({dist[v], v});
                if(debug) cerr << "[debug]  relax: u=" << u << " -> v=" << v << " w=" << w << ", dist[" << v << "] " << old << " -> " << dist[v] << "\n";
            }
        }
    }

    if(debug){
        cerr << "[debug] final dist: ";
        for(int i=1;i<=n;i++){
            if(dist[i]==INF) cerr << "INF "; else cerr << dist[i] << ' ';
        }
        cerr << "\n";
    }

    // Print distances
    for(int i=1;i<=n;i++){
        if(dist[i] == INF) cout << "INF";
        else cout << dist[i];
        if(i < n) cout << ' ';
    }
    cout << '\n';

    // Print shortest path to each node from s using '->' link
    for(int i=1;i<=n;i++){
        cout << i << ": ";
        if(dist[i] == INF){
            cout << "INF\n";
            continue;
        }
        // reconstruct path by walking predecessors
        vector<int> path;
        for(int cur = i; cur != -1; cur = pre[cur]) path.push_back(cur);
        reverse(path.begin(), path.end());
        // print path like 1->2->3
        for(size_t k=0;k<path.size();k++){
            cout << path[k];
            if(k+1<path.size()) cout << "->";
        }
        cout << '\n';
    }

    return 0;
}

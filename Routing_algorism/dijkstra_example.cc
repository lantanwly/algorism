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

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

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

    // min-heap: (distance, node)
    priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> pq;
    pq.push({0, s});

    while(!pq.empty()){
        auto [d,u] = pq.top(); pq.pop();
        if(d != dist[u]) continue; // stale entry
        for(auto &e : g[u]){
            int v = e.first; int w = e.second;
            if(dist[v] > d + w){
                dist[v] = d + w;
                pq.push({dist[v], v});
            }
        }
    }

    // Print distances
    for(int i=1;i<=n;i++){
        if(dist[i] == INF) cout << "INF";
        else cout << dist[i];
        if(i < n) cout << ' ';
    }
    cout << '\n';

    return 0;
}

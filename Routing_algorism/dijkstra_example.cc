// 简单的、注释详细的Dijkstra算法示例（使用邻接表 + 优先队列）
// 输入格式：
//   n m
//   m 行：u v w   (1-基于的节点，无向边)
//   s               (源节点)
// 输出：
//   从s到n个节点的距离：dist[1] dist[2] ... dist[n]
//   不可达节点打印为INF

#include <bits/stdc++.h>
using namespace std;
using ll = long long; //C11写法，定义ll为long long类型的别名。等于typedef long long ll;
const ll INF = (1LL<<60);  //1（int）强制转化为long long类型，然后左移60位，得到2^60，赋值给INF
// 定义无穷大，使用long long避免溢出

int main(int argc, char** argv){
    ios::sync_with_stdio(false);  // 关闭C++流与C流的同步，提高输入输出效率
    cin.tie(nullptr);  // 解绑cin和cout，减少缓冲区刷新
    bool debug = (argc > 1 && string(argv[1]) == "debug");  // 检查命令行参数是否为"debug"，启用调试输出

    int n, m;//n是节点数，m是边数
    if(!(cin >> n >> m)) return 0; // 如果没有输入，退出程序

    vector<vector<pair<int,int>>> g(n+1);  // 图的邻接表：g[i]存储从节点i出发的边，pair<邻接节点, 权重>
    for(int i=0;i<m;i++){
        int u,v,w; cin >> u >> v >> w;
        if(u<1 || u>n || v<1 || v>n) continue; // 简单检查节点范围，避免无效输入
        g[u].push_back({v,w});
        g[v].push_back({u,w}); // 无向图，所以双向添加边；如果是单向图，注释掉这一行
    }

    int s; cin >> s;//源节点
    if(s < 1 || s > n){
        cerr << "Invalid source node\n";  // 错误输出到标准错误流
        return 0;
    }

    vector<ll> dist(n+1, INF);  // 距离数组，初始化为无穷大
    dist[s] = 0;  // 源节点距离为0
    if(debug){
        cerr << "[debug] initialized dist: ";  // 调试输出初始距离
        for(int i=1;i<=n;i++){
            if(dist[i]==INF) cerr << "INF "; else cerr << dist[i] << ' ';
        }
        cerr << "\n";
    }
    // 前驱数组，用于路径重构
    vector<int> pre(n+1, -1);  // pre[i]存储节点i的前驱节点，初始化为-1

    // 最小堆：(距离, 节点)
    priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> pq;  // 小顶堆，存储<距离, 节点>
    pq.push({0, s});  // 将源节点推入优先队列

    while(!pq.empty()){
        pair<ll,int> p = pq.top(); pq.pop();  // 弹出当前最小距离的节点
        ll d = p.first; int u = p.second;  // 提取距离和节点
        if(debug) cerr << "[debug] pop (d=" << d << ", u=" << u << ")\n";
        if(d != dist[u]){  // 如果弹出的距离不是当前最优距离，跳过（过时条目）
            if(debug) cerr << "[debug]  stale entry for u=" << u << ", d=" << d << ", current dist[u]=" << dist[u] << " - skip\n";
            continue;
        }
        for(auto &e : g[u]){  // 遍历节点u的所有邻接边
            int v = e.first; int w = e.second;  // v是邻接节点，w是边权重
            if(dist[v] > d + w){  // 如果通过u到达v的距离更短
                ll old = dist[v];
                dist[v] = d + w;  // 更新距离
                pre[v] = u; // 记录前驱
                pq.push({dist[v], v});  // 将更新后的节点推入队列
                if(debug) cerr << "[debug]  relax: u=" << u << " -> v=" << v << " w=" << w << ", dist[" << v << "] " << old << " -> " << dist[v] << "\n";
            }
        }
    }

    if(debug){
        cerr << "[debug] final dist: ";  // 调试输出最终距离
        for(int i=1;i<=n;i++){
            if(dist[i]==INF) cerr << "INF "; else cerr << dist[i] << ' ';
        }
        cerr << "\n";
    }

    // 打印距离
    for(int i=1;i<=n;i++){
        if(dist[i] == INF) cout << "INF";
        else cout << dist[i];
        if(i < n) cout << ' ';
    }
    cout << '\n';

    // 使用'->'链接打印从s到每个节点的最短路径
    for(int i=1;i<=n;i++){
        cout << i << ": ";
        if(dist[i] == INF){
            cout << "INF\n";
            continue;
        }
        // 通过前驱重构路径
        vector<int> path;
        for(int cur = i; cur != -1; cur = pre[cur]) path.push_back(cur);  // 从目标节点回溯到源节点
        reverse(path.begin(), path.end());  // 反转路径，使其从源到目标
        // 打印路径，如1->2->3
        for(size_t k=0;k<path.size();k++){
            cout << path[k];
            if(k+1<path.size()) cout << "->";
        }
        cout << '\n';
    }

    return 0;
}

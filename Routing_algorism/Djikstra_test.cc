#include<bits/stdc++.h>
using namespace std;
using ll=long long;
const ll INF=(1LL<<60);

int main(int argc,char**argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    bool debug=(argc>1&&string(argv[1])=="debug");

    //输出：
    int n,m; //n为节点数，m为边的数量
    if(!(cin>>n>>m)) return 0;

    //建立邻接表
    vector<vector<pair<int,int>>> g(n+1);//用于存储邻接点和权值
    for(int i=0;i<m;i++){
        int u,v,w;cin>>u>>v>>w;
        if(u<1 || u>n || v<1 || v>n) continue;
        g[u].push_back({v,w});
        g[v].push_back({u,w});
    }

    //最短距离
    vector<ll> dist(n+1,INF);
    int s; cin>>s;
    dist[s]=0;

    //优先队列，用于取出最短的点
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    pq.push({0,s}); //源节点入队

    //前驱数组
    vector<int> pre(n+1,-1);

    while(!pq.empty()){
        pair<ll,int> p=pq.top(); pq.pop();
        int d=p.first; int u=p.second;
        if(d!=dist[u]) continue;
        for(auto &e:g[u]){
            int v=e.first;int w=e.second;
            if(dist[v]>d+w){
                dist[v]=d+w;
                pre[v]=u;
                pq.push({dist[v],v});
            }
        }
    }

    for(int i=1;i<=n;i++){
        cout<<dist[i];
        cout<<" ";
    }

    cout<<"\n";
    for(int i=1;i<=n;i++){
        cout<<i<<": ";
        //反向迭代输出路径
        vector<int> path;
        for(int k=i;pre[k]!=-1;k=pre[k]) path.push_back(k);
        reverse(path.begin(),path.end());
        cout<<s<<"->";
            //打印路径
        for(size_t k=0;k<path.size();k++){
            cout<<path[k];
            if(k+1<path.size()) cout<<"->";
        }
        cout<<"\n";
    }
    



    return 0;
}
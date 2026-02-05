#include<bits/stdc++.h>
using  namespace std;
using ll=long long;
const ll INF=(1LL<<60);//设定INF=2^60

int main(int argc,char** argv){
    ios::sync_with_stdio(false);//将c++输入输出和c流解绑
    cin.tie(nullptr);//将cin和cout 解绑
    bool debug=(argc>1 && string(argv[1])=="debug");

    int n,m;//n是节点，m是边数
    if(!(cin>>n>>m)) return 0;//检查是否无输入

    vector<vector<pair<int,int>>>    g(n+1);//图的邻接表，g[i]表示从i点出发的边点合集，pair<点，权值>
    for(int i=0;i<m;i++){
        int u,v,w;cin>>u>>v>>w;
        if(u<1 || u>n || v<1 || v>n) continue; //检查输入范围
        g[u].push_back({v,w}); //将节点存入邻接表
        g[v].push_back({u,w}); //双向

    }
    int s;cin>>s;//输入源节点
    if(s<1 || s>n){//检查源节点输入
        cerr<<"Invalid source Node!";
        return 0;
    }

    vector<ll> dist(n+1,INF);//建立距离数组，记录各点到源的距离
    dist[s]=0;//源节点距离为0

    //还需要前驱数组用于生成路径
    vector<int> pre(n+1,-1);//初始化话为-1

    //需要一个优先队列，用于取出距离最短的点
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    //数据类型，底层容器，比较函数（按第一个元素从小到大排序，距离相等，按第二个元素从小到大排序）
    pq.push({0,s});//将源节点推入优先队列

    while(!pq.empty()){
        pair<ll,int> p=pq.top();pq.pop();
        ll d=p.first;int u=p.second;
        
        if(d!=dist[u]){ //排除优先队列中的多余节点
            continue;
        }

        for(auto &e:g[u]){
            int v=e.first;int w=e.second;//v是邻接点，w是权值
            if(dist[v]>d+w){
                dist[v]=d+w;
                pre[v]=u;
                pq.push({dist[v],v});//将更新后的节点推入队列
            }
        }


    }

    //打印距离
    for(int i=1;i<=n;i++){
        if(dist[i]==INF) cout<<"INF";
        else cout<<dist[i];
        if(i<n) cout<<" ";
    }
    cout<<"\n";

    //输出路径
    for(int i=1;i<=n;i++){
        cout<<i<<":";
        if(dist[i]==INF){
            cout<<"INF\n";
            continue;
        }
        vector<int> path;
        for(int cur=i;cur!=-1;cur=pre[cur]) path.push_back(cur);   //从目标节点回溯到源
        reverse(path.begin(),path.end());// 反转路径
        //打印路径
        for(size_t k=0;k<path.size();k++){//size_t更规范
            cout<<path[k];
            if(k+1<path.size()) cout<<"->";
        }
        cout<<"\n";
    }



    return 0;
}
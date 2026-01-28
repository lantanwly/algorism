#include<cstdio>
#include<iostream>
#include<cstring>
#define INF 0x3f3f3f3f  // 安全的无穷大值（避免溢出，memset可正确赋值）
using namespace std;

const int N=1e3+10;  // 节点数上限（适配邻接矩阵规模）
int n,m;             // n：节点数，m：边数
int mp[N][N];        // 邻接矩阵存储图
int dis[N];          // 源点到各节点的最短距离
int vis[N];          // 标记节点是否已确定最短路径
int pre[N];          // 记录最短路径上的前驱节点（可选）

// 初始化邻接矩阵：所有边初始为无穷大（表示无边）
void initmap(){
    memset(mp,INF,sizeof(mp));
    // 补充：节点到自身的距离为0（可选，但逻辑更严谨）
    // for(int i=1;i<N;i++){
    //     mp[i][i] = 0;
    // }
}

// Dijkstra算法（邻接矩阵版）：求解源点s到所有节点的最短路径
void dijkstra(int s) {
    memset(vis,0,sizeof(vis));  // 初始化所有节点为「未确定」状态
    memset(dis,INF,sizeof(dis));// 初始化最短距离为无穷大
    dis[s] = 0;                 // 源点到自身的距离为0

    while(1) {
        int mini = 0, min_ = INF;
        // 步骤1：找到未确定节点中距离源点最近的节点mini
        for(int j=1;j<=n;j++){
            if(!vis[j] && dis[j] < min_){
                min_ = dis[j];
                mini = j;
            }
        }
        // 无未确定节点，退出循环（所有节点最短路径已确定）
        if(mini == 0) break;
        vis[mini] = 1;  // 标记mini为「已确定」

        // 步骤2：用mini更新所有邻接节点的最短距离
        for(int k=1;k<=n;k++){
            // 仅当k未确定、mini到k有边、新路径更短时更新
            if(!vis[k] && mp[mini][k] != INF && dis[k] > dis[mini] + mp[mini][k]){
                dis[k] = dis[mini] + mp[mini][k];
                pre[k] = mini;  // 记录前驱节点（可选）
            }
        }
    } // 补全while循环的闭合大括号（之前缺失）
} // 补全dijkstra函数的闭合大括号（之前缺失）


//求到z的最短路徑
void output(int z){
    if(z==0) return;
    output(pre[z]);
    cout<<z<<"->";
}

int main() {
    memset(pre,-1,sizeof(pre)); // 初始化前驱节点数组
    // 循环读取输入：n=0时终止
    while(cin>>n>>m && n!=0){
        initmap();  // 初始化邻接矩阵
        
        // 输入m条边（无向图，u<->v，权值w）
        for(int i=0;i<m;i++){
            int u,v,w;
            cin>>u>>v>>w;
            // 无向图：双向赋值，且保留最小权值（避免重边）
            if(mp[u][v] > w){
                mp[u][v] = w;
                mp[v][u] = w;
            }
        } // 补全边输入循环的闭合大括号
        
        // 关键修正：所有边输入完成后，再调用Dijkstra（之前错在循环内调用）
        dijkstra(1);  // 求解从节点1到所有节点的最短路径
        
        // 关键修正：print→printf（C++无print函数）
        cout<<dis[n]<<"\n";  // 输出节点1到节点n的最短距离
    }
    cout<<endl;
    for(int i=1;i<n;i++){
        output(i);
        cout<<endl;
    }
    return 0;
} // 补全main函数的闭合大括号

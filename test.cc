#include <iostream>
using namespace std;

int main(int argc, char** argv){
    // 1. 先关闭同步（避免系统级的缓冲干扰）
    ios::sync_with_stdio(false); //停止cin cout和scanf printf的同步
    // 2. 解绑cin和cout（核心）,cout在缓冲区满或者遇到换行符时才刷新
    cin.tie(nullptr);
    //3.整体逻辑：只有 “传入了参数” 且 “参数是 debug” 时，debug 才为 true，否则为 false。
    bool debug = (argc > 1 && string(argv[1]) == "debug");  // 检查命令行参数是否为"debug"，启用调试输出
    //argc是命令行参数的个数，argv是参数数组，argv[0]是程序名，argv[1]是第一个参数

    int x=0, y=0, z=0;
    if(!(cin>>x>>y)) return 0;
    cout<<x<<" "<<y;

    return 0;
}

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

    int x, y, z;
    // 输出内容：不带换行符，避免行缓冲刷新
    cout << "测试1："; 
    cout << "测试2：";
    cout << "测试3：";

    // 一次性输入3个数字（避免逐行输入触发刷新）
    cin >> x >> y >> z;

    // 程序结束前，手动刷新缓冲区（验证内容都在缓冲区里）
    cout << "\n输入的数字：" << x << " " << y << " " << z << endl;

    return 0;
}

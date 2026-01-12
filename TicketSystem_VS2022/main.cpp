#include "ticket_system.h"
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

void show_menu() {
    cout << "\n========= 火车订票系统（命令行演示） =========\n";
    cout << "1. 新建示例列车（自动生成站点、票价）\n";
    cout << "2. 列出所有列车信息\n";
    cout << "3. 查询区间余票\n";
    cout << "4. 订票\n";
    cout << "5. 退票\n";
    cout << "6. 显示已售票与收入\n";
    cout << "7. 保存到磁盘\n";
    cout << "8. 从磁盘加载\n";
    cout << "0. 退出\n";
    cout << "请选择：";
}

int main() {
#ifdef _WIN32
    // 设置控制台为 UTF-8，确保中文输出正确（需要 /utf-8 编译选项，或在项目属性中设置）
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    TicketSystem sys;
    // 尝试加载（如果存在）
    sys.loadFromFiles("trains.dat", "tickets.dat");

    while (true) {
        show_menu();
        int cmd;
        if (!(cin >> cmd)) break;
        if (cmd == 0) break;
        if (cmd == 1) {
            int N, seats;
            cout << "请输入站点数量 N (建议 > 10): ";
            cin >> N;
            cout << "请输入座位数 (例如 50): ";
            cin >> seats;
            string train_name;
            cout << "请输入列车编号（如 G100）: ";
            cin >> train_name;
            sys.createExampleTrain(train_name, N, seats);
            cout << "已创建示例列车 " << train_name << " (站点数 " << N << ", 座位 " << seats << ")\n";
        } else if (cmd == 2) {
            sys.listTrains();
        } else if (cmd == 3) {
            string tid; int a,b;
            cout << "列车编号: "; cin >> tid;
            cout << "起点站序号(从0起): "; cin >> a;
            cout << "终点站序号(从0起): "; cin >> b;
            if (a>=b) { cout<<"起点必须小于终点\n"; continue; }
            int rem = sys.queryRemainingSeats(tid, a, b);
            if (rem < 0) cout << "没有找到列车或区间不合法\n"; else cout << "区间["<<a<<","<<b<<")剩余座位: " << rem << "\n";
        } else if (cmd == 4) {
            string tid, name; int a,b;
            cout << "列车编号: "; cin >> tid;
            cout << "乘客姓名: "; cin >> name;
            cout << "起点站序号(从0起): "; cin >> a;
            cout << "终点站序号(从0起): "; cin >> b;
            if (a>=b) { cout<<"起点必须小于终点\n"; continue; }
            auto res = sys.bookTicket(tid, name, a, b);
            if (res.first) {
                cout << "订票成功！ 票号=" << res.second.ticket_id << " 座位=" << res.second.seat_no
                     << " 票价=" << res.second.price << "\n";
            } else {
                cout << "订票失败：无法完成订票\n";
            }
        } else if (cmd == 5) {
            string ticket_id;
            cout << "请输入票号: "; cin >> ticket_id;
            if (sys.refundTicket(ticket_id)) cout << "退票成功\n";
            else cout << "退票失败（未找到票号）\n";
        } else if (cmd == 6) {
            sys.showSalesAndRevenue();
        } else if (cmd == 7) {
            sys.saveToFiles("trains.dat", "tickets.dat");
            cout << "已保存\n";
        } else if (cmd == 8) {
            sys.loadFromFiles("trains.dat", "tickets.dat");
            cout << "已加载\n";
        } else {
            cout << "无效命令\n";
        }
    }

    cout << "退出程序，正在保存...\n";
    sys.saveToFiles("trains.dat", "tickets.dat");
    cout << "已保存，拜拜！\n";
    return 0;
}

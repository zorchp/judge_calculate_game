#include <cstdio>
#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include <climits>
#include <signal.h>
#include <unistd.h>
// #include <termios.h>// 设置终端, 回显时候会卡顿
#include <ncurses.h> // 捕获 非回车输入


int calc(int a, int b, char op) {
    switch (op) {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return a / b;
    }
    return 0;
}


void show_process_bar(int second) {
    // 进度条, 时间, 是否显示时间, 进度条长度
    auto process_bar = [](int second, bool show_time = false, int len = 30) {
        int interval = second * 1000 / len;

        for (int i{1}; i <= len; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if (show_time)
                std::cout << i * interval / 1000 << "s [" << std::string(i, '=')
                          << "=>" << std::string(len - i, ' ') << "]\r"
                          << std::flush;
            else
                std::cout << "[" << std::string(i, '=') << "=>"
                          << std::string(len - i, ' ') << "]\r" << std::flush;
        }
        putchar('\n');
    };

    std::thread t1(process_bar, second);
    t1.detach();
}

// 添加信号处理
void addsig(int sig, void(handler)(int)) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler; // 信号触发后执行的函数
    sigfillset(&sa.sa_mask);
    sigaction(sig, &sa, NULL);
}

// 信号
int right_ans_cnt{};

void time_handler(int sig) {
    endwin();
    printf("time out \n"); //
    printf("best record: %d\n Game over!\n", right_ans_cnt);
    exit(-1);
}

const char ops[] = "+-*/";
const char right = ',';
const char wrong = '.';

void eventloop() {
    //
    int per_time = 5;
    int wrong_ans{}; // 信号

    initscr();
    cbreak();
    noecho();
    srand(time(0)); // 确保随机
    for (;;) {
        // show_process_bar(per_time);
        int a = rand() % 10;
        int b = 1 + rand() % 10; // 除数不为零
        int op_idx = rand() % 4;
        char op = ops[op_idx];

        int ans = calc(a, b, op);
        // 保证整除
        if (op == '/' && a % b) a -= a % b;

        bool real_ans = rand() % 2;
        // 错误情况, 随机做一个结果出来, 必须保证这个结果一定是错误的
        if (!real_ans) { // 错误
            if (a & 1) { // 采用加入随机数的方法
                int tmp_ans = calc(ans, rand() % 10, ops[rand() % 4]);
                ans = (ans == tmp_ans) ? ans + rand() % 10 + op : tmp_ans;
            } else { // 修改运算符的方法
                int tmp_ans_op = calc(a, b, ops[(op_idx + 1) % 4]);
                ans = (ans == tmp_ans_op) ? ans + rand() % 10 + op : tmp_ans_op;
            }
            if (ans == INT_MAX) continue;
        }

        clear();
        printw("%d %c %d = %d\n\r", a, op, b, ans);
        refresh();

        alarm(per_time);

        int input_ans = getch();

        if (input_ans != right && input_ans != wrong) {
            endwin();
            perror("unexpected input\n");
            exit(-1);
        }
        if ((input_ans == right && !real_ans) ||
            (input_ans == wrong && real_ans)) {
            wrong_ans = 1;
            alarm(0); // 重置定时器
            break;
        }
        ++right_ans_cnt;
        // std::cout << "✔️  \r" << std::flush;
        // alarm(0);
    }
    if (wrong_ans) {
        endwin();
        printf("wrong answer\n");
        printf("best record: %d\n Game over!\n", right_ans_cnt);
    }
}

int main(int argc, char const* argv[]) {
    //
    addsig(SIGALRM, time_handler);
    printf("按下回车 开始游戏:\n");
    printf("'%c' 表示正确, '%c' 表示错误\n", right, wrong);
    getchar(); // 捕获回车
    eventloop();
    // char c = get1char();
}

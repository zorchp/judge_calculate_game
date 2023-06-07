#include <cstdio>
#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include <signal.h>
#include <unistd.h>
// #include <ncurses.h> // 捕获 非回车输入
#include "get1char.cpp"

extern char get1char(void);


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
        std::cout << '\n';
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

void time_handler(int sig) {
    puts("time out \n"); //
    exit(-1);
}

void eventloop() {
    //
    const char ops[] = "+-*/";
    const char right = '[';
    const char wrong = ']';
    int per_time = 5;
    int wrong_ans{}; // 信号
    int right_ans_cnt{};
    addsig(SIGALRM, time_handler);
    printf("按下回车 开始游戏:\n");
    printf("'%c' 表示正确, '%c' 表示错误\n", right, wrong);
    getchar(); // 捕获回车
    for (;;) {
        // 设置定时器
        alarm(per_time);
        // show_process_bar(per_time);
        srand(time(0)); // 确保随机
        int a = rand() % 10;
        int b = rand() % 10;
        char op = ops[rand() % 4];

        if (op == '/' && (b == 0 || a % b)) {
            alarm(0);
            continue; // 除数为 0 或者不能整除
        }
        int ans = calc(a, b, op);
        bool real_ans = rand() % 2;
        // 错误情况, 随机做一个结果出来, 必须保证这个结果一定是错误的
        if (!real_ans) {
            int tmp_ans = calc(ans, rand() % 10, ops[rand() % 4]);
            ans = (ans == tmp_ans) ? ans + rand() % 10 + 1 : tmp_ans;
        }
        char buf[1024];
        snprintf(buf, 512, "%d %c %d = %d \r", a, op, b, ans);
        printf("%s", buf);

        char input_ans = get1char();
        if (input_ans != right && input_ans != wrong) {
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
        // 退格, 去掉上一个算式的显示
        for (int i{}; i <= 1 + strlen(buf); ++i) putchar('\b');
    }
    if (wrong_ans) {
        std::cout << "error answer!\n";
        std::cout << "best record: " << right_ans_cnt << "\n";
    }
    std::cout << "oops! game over\n";
}

int main(int argc, char const* argv[]) {
    //
    eventloop();
    // char c = get1char();
}

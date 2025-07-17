#include <termios.h>
#include <unistd.h>

struct TermiosGuard
{
    struct termios oldt;

    TermiosGuard()
    {
        tcgetattr(STDIN_FILENO, &oldt); // 获取当前终端设置
        struct termios newt = oldt;

        // 禁用 canonical 模式和 echo（即输入立即生效，不回显）ECHO ICANON
        newt.c_lflag &= ~(ISIG | NOFLSH); // ISIG 关闭 Ctrl+C 等信号

        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // 应用 raw 模式
    }

    ~TermiosGuard()
    {
        // 恢复原始设置
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
};

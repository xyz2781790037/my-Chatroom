#ifndef TERMIOS_GUARD_H
#define TERMIOS_GUARD_H
#include <termios.h>
#include <unistd.h>

struct TermiosGuard
{
    struct termios oldt;
    struct termios newt;
    TermiosGuard()
    {
        tcgetattr(STDIN_FILENO, &oldt); // 获取当前终端设置
        newt = oldt;

        // 禁用 canonical 模式和 echo（即输入立即生效，不回显）ECHO ICANON
        // newt.c_lflag &= ~(ISIG); // ISIG 关闭 Ctrl+C 等信号
        newt.c_cc[VEOF] = _POSIX_VDISABLE;
        newt.c_cc[VSUSP] = _POSIX_VDISABLE;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // 应用 raw 模式
    }
    void encryption(){
        struct termios wet = newt;
        wet.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &wet);
    }
    void removeEncryption(){
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }
    ~TermiosGuard()
    {
        // 恢复原始设置
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
};
#endif
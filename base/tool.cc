#include "tool.h"
std::string tool::swapsort(std::string a, std::string b, std::string type){
    a = a.substr(5);
    b = b.substr(5);
    if (a > b)
    {
        std::swap(a, b);
    }
    return type + a + ":" + b;
}
std::string tool::spellName(std::string a, std::string b, std::string type){
    a = a.substr(5);
    b = b.substr(5);
    return type + a + ":" + b;
}
std::string tool::segExcessiveSpace(std::string &order, int count){
    while (count < order.size()){
        if ((order[count] == ' ' && count == 0) || (order[count] == ' ' && count == order.size() - 1) || (count + 1 < order.size() && order[count] == ' ' && order[count + 1] == ' ')){
            order.erase(count, 1);
        }
        else{
            count++;
        }
    }
    return order;
}
bool tool::compareLevel(std::string a, std::string b){
    if (a.size() == b.size()){
        if (a == b){
            return false;
        }
        else if (a > b){
            return true;
        }
        return false;
    }
    else if (a.size() < b.size()){
        return true;
    }
    return false;
}
std::string tool::tolowerStr(std::string a){
    for(char &c : a){
        c = tolower(c);
    }
    return a;
}
int tool::getTerminalWidth(){
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
        return w.ws_col;
    return 80; // fallback
}

void tool::clearInputLines(const std::string &input){
    int width = getTerminalWidth();
    int lines = (input.length() + width - 1) / width;  // 占据的实际行数
    // 向上移动到最初输入行
    for (int i = 0; i < lines; ++i) {
        std::cout << "\33[A";
    }
    // 清除每一行，从上到下
    for (int i = 0; i < lines; ++i) {
        std::cout << "\r\33[2K"; // 清除整行
        if (i != lines - 1) {
            std::cout << "\33[B"; // 向下移动一行
        }
    }
    // 再次回到最初位置
    for (int i = 0; i < lines - 1; ++i) {
        std::cout << "\33[A";
    }
    std::cout.flush();
}
int tool::getFilefd(std::string filename){
    int fileFd = open(filename.c_str(), O_RDONLY, 0644);
    if(fileFd < 0){
        if(errno != ENOENT){
            LOG_ERROR << "open failed:" << strerror(errno);
            close(fileFd);
        }
        return -1;
    }
    struct stat st;
    if (fstat(fileFd, &st) == -1){
        perror("fstat");
        close(fileFd);
        return -1;
    }

    if (!S_ISREG(st.st_mode)){
        // 不是普通文件，关闭并拒绝使用
        close(fileFd);
        errno = EISDIR;
        return -1;
    }
    return fileFd;
}
std::string tool::fileSize(int fileFd){
    struct stat st;
    if (fstat(fileFd, &st) == 0) {
        off_t bytes = st.st_size;
        const char *sizes[] = {"B", "KB", "MB", "GB", "TB"};
        int order = 0;
        double size = static_cast<double>(bytes);

        while (size >= 1024 && order < 4)
        {
            order++;
            size /= 1024;
        }

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << size << " " << sizes[order];
        return oss.str();
    }
    return "null";
}
void tool::clear(){
    std::cout << "\33[A";
    std::cout << "\r\33[2K";
}
bool tool::isValidInput(const std::string& input,std::string patterns) {
    std::regex pattern(patterns); // 只允许字母、数字、下划线
    return std::regex_match(input, pattern);
}
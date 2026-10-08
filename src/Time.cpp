#include "../h/Time.hpp"
#include <ctime>

std::string Time::getTime() {
    std::time_t t = std::time(nullptr);
    char buf[8];
    std::strftime(buf, sizeof(buf), "%H:%M", std::localtime(&t));
    return buf;
}

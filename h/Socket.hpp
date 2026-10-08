#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include "Message.hpp"

class Socket {
public:
    Socket();
    explicit Socket(int fd);

    ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket &&other) noexcept;
    Socket& operator=(Socket &&other) noexcept;

    void connect(const char* address, uint16_t port);

    void bind(uint16_t port);
    void listen(int backlog = 10);

    Socket accept();

    bool send(const Message& msg) const;
    ssize_t receive(void *buffer, size_t size);
    static void deserialize(Message& msg, char* buffer){
        std::memcpy(&msg, buffer, sizeof(Message));
    };

    void close();

    int getFileDescriptor() const;

private:
    int fd;
};

#endif // !SOCKET_HPP

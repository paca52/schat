#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <cstddef>
#include <cstdint>
#include <unistd.h>

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

    ssize_t send(const void *buffer, size_t size) const;
    ssize_t receive(void *buffer, size_t size);

    void close();

    int getFileDescriptor() const;

private:
    int fd;
};

#endif // !SOCKET_HPP

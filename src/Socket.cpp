#include "../h/Socket.hpp"
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

Socket::Socket() {
    fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        throw std::runtime_error("Failed to create a socket");
}
}

Socket::Socket(int fd) : fd(fd) { }

Socket::~Socket() {
    close();
}

Socket::Socket(Socket &&other) noexcept : fd(other.fd) {
    other.fd = -1;
}

Socket& Socket::operator=(Socket &&other) noexcept {
    if (this != &other) {
        close();
        fd = other.fd;
        other.fd = -1;
    }

    return *this;
}

void Socket::bind(uint16_t port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        throw std::runtime_error("Failed to bind the socket");
    }
}

void Socket::listen(int backlog) {
    if (::listen(fd, backlog) == -1) {
        throw std::runtime_error("Failed to listen on socket");
    }
}

Socket Socket::accept() {
    int clientFD = ::accept(fd, nullptr, nullptr);

    if (clientFD == -1) {
        throw std::runtime_error("Failed to accept connection");
    }

    return Socket(clientFD);
}

ssize_t Socket::send(const void *buffer, size_t size) const {
    return ::send(fd, buffer, size, 0);
}

ssize_t Socket::receive(void *buffer, size_t size) {
    return ::recv(fd, buffer, size, 0);
}

void Socket::close() {
    if (fd == -1) return;
    ::close(fd);
    fd = -1;
}

int Socket::getFileDescriptor() const {
    return fd;
}

void Socket::connect(const char *address, uint16_t port) {
    sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (inet_pton(AF_INET, address, &server.sin_addr) <= 0) {
        throw std::runtime_error("Invalid address");
    }

    if (::connect(fd, (sockaddr*)&server, sizeof(server)) == -1) {
        throw std::runtime_error("Failed to connect");
    }
}

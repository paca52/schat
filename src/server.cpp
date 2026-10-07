#include "../h/Socket.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <poll.h>
#include <unistd.h>

constexpr uint16_t PORT = 8080;
constexpr std::size_t MAX_CLIENTS = 100;
constexpr std::size_t BUFFER_SIZE = 1024;

void broadcast(const std::vector<Socket>& clients, const std::string& message, int except_fd = -1) {
    for (const Socket& client : clients) {
        if (client.getFileDescriptor() == except_fd)
            continue;

        client.send(message.data(), message.size());
    }
}

int main(void) {
    try {
        Socket server;
        server.bind(PORT);
        server.listen();

        std::cout << "Server listening on port: " << PORT << '\n';

        std::vector<Socket> clients;

        while (true) {
            std::vector<pollfd> fds;

            fds.push_back({
                server.getFileDescriptor(),
                POLLIN,
                0
            });

            fds.push_back({
                STDIN_FILENO,
                POLLIN,
                0
            });

            for (const Socket& client : clients) {
                fds.push_back({
                    client.getFileDescriptor(),
                    POLLIN,
                    0
                });
            }

            // Number of clients represented in `fds`
            const std::size_t polled_clients = clients.size();

            int ready = poll(fds.data(), fds.size(), -1);

            if (ready == -1) {
                perror("poll");
                break;
            }

            // New client
            if (fds[0].revents & POLLIN) {
                Socket client = server.accept();

                if (clients.size() >= MAX_CLIENTS) {
                    std::cout << "Maximum number of clients reached\n";
                } else {
                    std::cout << "Client connected [fd = "
                        << client.getFileDescriptor() << "]\n";

                    clients.push_back(std::move(client));
                }
            }

            // Server/admin input
            if (fds[1].revents & POLLIN) {
                std::string message;

                if (!std::getline(std::cin, message)) {
                    std::cout << "Server input closed\n";
                    break;
                }

                message += '\n';

                std::cout << "[ADMIN] " << message;

                broadcast(clients, message);
            }

            // Only process clients that existed when poll() was called
            std::vector<std::size_t> disconnected;

            for (std::size_t i = 0; i < polled_clients; ++i) {
                pollfd& pfd = fds[i + 2];

                if (pfd.revents & (POLLHUP | POLLERR | POLLNVAL)) {
                    disconnected.push_back(i);
                    continue;
                }

                if (!(pfd.revents & POLLIN))
                    continue;

                char buffer[BUFFER_SIZE];

                ssize_t bytes =
                    clients[i].receive(buffer, sizeof(buffer));

                if (bytes <= 0) {
                    std::cout << "Client disconnected [fd = "
                        << clients[i].getFileDescriptor() << "]\n";

                    disconnected.push_back(i);
                    continue;
                }

                std::string message(buffer, bytes);

                std::cout << "[CLIENT "
                    << clients[i].getFileDescriptor()
                    << "] "
                    << message;

                broadcast(
                    clients,
                    message,
                    clients[i].getFileDescriptor()
                );
            }

            // Remove disconnected clients backwards
            for (auto it = disconnected.rbegin(); it != disconnected.rend(); ++it) {
                clients.erase(clients.begin() + *it);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

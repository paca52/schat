#include "../h/Socket.hpp"
#include "../h/Message.hpp"

#include <cstring>
#include <iostream>
#include <string>
#include <poll.h>
#include <unistd.h>

constexpr uint16_t PORT = 8080;
constexpr std::size_t BUFFER_SIZE = 2048;
const char *localHost = "127.0.0.1";


int main(void) {
    try {
        Socket socket;
        socket.connect(localHost, PORT);

        std::cout << "Connected to server!\n";
        std::cout << "> " << std::flush;

        pollfd fds[2];

        fds[0] = {
            socket.getFileDescriptor(),
            POLLIN,
            0
        };

        fds[1] = {
            STDIN_FILENO,
            POLLIN,
            0
        };

        while (true) {
            int ready = poll(fds, 2, -1);

            if (ready == -1) {
                perror("poll");
                break;
            }

            // Message from server
            if (fds[0].revents & POLLIN) {
                char buffer[BUFFER_SIZE];

                ssize_t bytes = socket.receive(buffer, sizeof(Message));

                if (bytes <= 0) {
                    std::cout << "\nServer disconnected.\n";
                    break;
                }
                
                Message message;
                Socket::deserialize(message, buffer);

                std::cout << "\r"
                          << message.text
                          << "> "
                          << std::flush;
            }

            // User typed something
            if (fds[1].revents & POLLIN) {
                Message message;
                std::cin.clear();
                if (!std::cin.getline(message.text, MAX_MESSAGE_LENGTH - 2)) {
                    break;
                }

                int len = strlen(message.text);
                message.text[len] = '\n';
                message.text[len + 1] = '\0';

                socket.send(message);

                std::cout << "> " << std::flush;
            }

            // Connection errors
            if (fds[0].revents & (POLLHUP | POLLERR | POLLNVAL)) {
                std::cout << "\nServer connection lost.\n";
                break;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

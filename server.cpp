#include <iostream>
#include <fstream> // to work with files
#include <sys/types.h> // for working with data types
#include <sys/socket.h> // for working with sockets
#include <netinet/in.h> // for working with Internet addresses
#include <unistd.h>
#include <arpa/inet.h> // for converting Internet addresses
#include <ctime>  // for working with time

// Function to check that a number is prime
bool is_prime(int num) {
    if (num <= 1)
        return false;
    for (int i = 2; i * i <= num; ++i)
        if (num % i == 0)
            return false;

    return true;
}

// Function for recording messages in the log file
void log_message(const std::string& message) {
    std::ofstream log_file("server.log", std::ios::app);
    if (log_file.is_open()) {
        std::time_t t = std::time(nullptr);
        log_file << std::asctime(std::localtime(&t)) << message << std::endl;
        log_file.close();
    } else {
        std::cerr << "Unable to open log file for writing." << std::endl;
    }
}

int main() {
    int server_fd, new_socket, addr_len;
    struct sockaddr_in address;
    const int PORT = 1053;
    const int MAX_BUFFER = 1024;
    char buffer[MAX_BUFFER];

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Set address family, IP address and port number
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to the address and port number
    bind(server_fd, (struct sockaddr *) &address, sizeof(address));

    // Listen for incoming connections
    listen(server_fd, 3);
    addr_len = sizeof(address);

    log_message("Server started on port " + std::to_string(PORT) + ".");

    while (true) {
        // Accept new connection
        new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addr_len);

        // Log client connection
        std::string client_addr = inet_ntoa(address.sin_addr);
        log_message("Client connected: " + client_addr + ":" + std::to_string(PORT));

        int received_numbers = 0, prime_numbers = 0, min_val = INT32_MAX, max_val = INT32_MIN;

        // Receive 50 numbers from client
        for (int i = 0; i < 50; ++i) {
            // Receive number from client
            recv(new_socket, buffer, MAX_BUFFER, 0);

            int num = atoi(buffer);
            received_numbers++;

            log_message("Received number " + std::to_string(num) + " from client ");

            // Check if received number is prime
            bool prime = is_prime(num);

            // Send response to client indicating if the number is prime
            send(new_socket, &prime, sizeof(prime), 0);

            // Update statistics if the number is prime
            if (prime) {
                prime_numbers++;
                min_val = std::min(min_val, num);
                max_val = std::max(max_val, num);
            }
        }
        // Send statistics back to client
        send(new_socket, &received_numbers, sizeof(received_numbers), 0);
        send(new_socket, &prime_numbers, sizeof(prime_numbers), 0);
        send(new_socket, &min_val, sizeof(min_val), 0);
        send(new_socket, &max_val, sizeof(max_val), 0);

        log_message("Send received number: " + std::to_string(received_numbers));
        log_message("Send prime numbers: " + std::to_string(prime_numbers));
        log_message("Send Minimum value: " + std::to_string(min_val));
        log_message("Send Maximum value: " + std::to_string(max_val));



        close(new_socket); // close the socket
        close(server_fd); // close the server

        // Log client disconnection
        log_message("Client " + client_addr + ":" + std::to_string(PORT) + " disconnected.");

        return 0;
    }
}
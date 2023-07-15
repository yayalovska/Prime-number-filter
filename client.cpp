#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <ctime>
#include <cstdlib> // C standard library functions

// logs a message to a file
void log(const std::string& message) {
    std::ofstream log_file;
    log_file.open("client.log", std::ios_base::app);
    std::time_t current_time = std::time(nullptr);
    log_file << std::asctime(std::localtime(&current_time)) << ": " << message << std::endl;
    log_file.close();
}

int main() {
    // seeds the random number generator with the current time in seconds
    srand(time(NULL));

    int sock;
    struct sockaddr_in server_addr;
    const int PORT = 1053;
    const int MAX_BUFFER = 1024;
    char buffer[MAX_BUFFER];

    sock = socket(AF_INET, SOCK_STREAM, 0); // creates a TCP socket

    server_addr.sin_family = AF_INET; // sets the address family to IPv4
    server_addr.sin_port = htons(PORT); // sets the port number

    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // sets the IP address to localhost
    connect(sock, (struct sockaddr *) &server_addr, sizeof server_addr);

    log("Connected to server");


    // creates a file object for outputting prime numbers
    std::ofstream output_file("prime_numbers.txt");

// generates 50 random numbers between 1 and 1000, sends them to the server,
// and receives whether each number is prime or not
    for (int i = 0; i < 50; ++i) {
        int random_number = rand() % 1000 + 1;
        sprintf(buffer, "%d", random_number);
        send(sock, buffer, strlen(buffer) + 1, 0);
        bool prime;
        recv(sock, &prime, sizeof(prime), 0);
        if (prime) {
            output_file << random_number << std::endl;
        }
    }



    {
        // Receive the results from the server
        int received_numbers, prime_numbers, min_val, max_val;
        recv(sock, &received_numbers, sizeof(received_numbers), 0);
        recv(sock, &prime_numbers, sizeof(prime_numbers), 0);
        recv(sock, &min_val, sizeof(min_val), 0);
        recv(sock, &max_val, sizeof(max_val), 0);

        log("Received results from server. Received numbers: " + std::to_string(received_numbers));
        log("Prime numbers: " + std::to_string(prime_numbers));
        log("Min value: " + std::to_string(min_val));
        log("Max value: " + std::to_string(max_val));

        std::cout << "Numbers processed: " << received_numbers << std::endl;
        std::cout << "Prime numbers: " << prime_numbers << std::endl;
        std::cout << "Minimum prime number: " << min_val << std::endl;
        std::cout << "Maximum prime number: " << max_val << std::endl;
    }
    log ("Disconnected.");

    output_file.close(); // close the file
    close(sock); // close the socket

    return 0;
}
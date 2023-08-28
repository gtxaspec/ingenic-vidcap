#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <errno.h>  // Include for the errno variable

#include "capture_and_encoding.h"
#include "version.h"

#define SERVER_PORT 8080
#define BUFFER_SIZE 4096  // 4 KB

void displayUsage() {
    printf("usage: ingenic-vidcap [args...]\n\n"
        " --help            display this help message\n"
        " --noir            do not turn on IR LEDs (for use behind glass)\n"
        " --flip            flips the image 180deg for ceiling mount\n"
        " --force-color     stay in color mode, even at night\n");
    exit(0);
}

int main(int argc, char** argv) {
    printf("ingenic-vidcap version: %s\n", VERSION);

    // parse args
    int i;
    for (i = 0; i < argc; i++) {
        char *arg = argv[i];

        if (*arg == '-') {
            arg++;
            if (*arg == '-') arg++; // tolerate 2 dashes

            if (strcmp(arg, "help") == 0) {
                displayUsage();
                exit(0);
            } else if (strcmp(arg, "noir") == 0) {
                set_cam_option("ir_leds", 0);
            } else if (strcmp(arg, "force-color") == 0) {
                set_cam_option("force_color", 1);
            } else if (strcmp(arg, "flip") == 0) {
                set_cam_option("flip", 1);
            } else {
                printf("unrecognized argument %s\n\n", argv[i]);
                displayUsage();
                exit(2);
            }
        }
    }

    // Initialize camera capture and encoding
    if (capture_and_encoding() != 0) {
        printf("unable to setup camera stream\n");
        exit(1);
    }

    printf("Camera stream setup successfully.\n");

    // Setup TCP server
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    int opt_val = 1;
    setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, &opt_val, sizeof(opt_val));
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(1);
    }

    if (listen(server_socket, 1) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(1);
    }

    while (1) {
        printf("Waiting for client connection...\n");
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }
        printf("Client connected.\n");

        while (1) {
            int result = get_stream(client_socket, 0);
            if (result < 0) {
                perror("Error in get_stream");  // This will also display the error associated with errno
                printf("get_stream returned: %d, errno: %d\n", result, errno);
                break;
            }
        }

        close(client_socket);
        printf("Client disconnected.\n");
    }

    close(server_socket);
    return 0;
}

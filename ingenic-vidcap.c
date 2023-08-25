#include <stdio.h>
#include <sys/types.h>  
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
 
#include "capture_and_encoding.h"
#include "version.h"

char const* inputFileName = "/tmp/h264_fifo";

void displayUsage() {
    printf("usage: t20-rtspd [args...]\n\n"
        " --help            display this help message\n"
        " --noir            do not turn on IR LEDs (for use behind glass)\n"
        " --flip            flips the image 180deg for ceiling mount\n"
        " --force-color     stay in color mode, even at night\n");
    exit(0);
}

int main(int argc, char** argv) {
    printf("my-carrier-server version: %s\n", VERSION);

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
    unlink(inputFileName);

    if (mkfifo(inputFileName, 0777) < 0) {
        printf("mkfifo Failed\n");
        exit(1);
    }

    int fd = open(inputFileName, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fd < 0) {
        printf("Failed open fifo\n");
        exit(1);
    }   
    while (1) {
        if (get_stream(fd ,0) < 0) break; // Get stream and write to fifo.
    }

    return 0; // only to prevent compiler warning
}

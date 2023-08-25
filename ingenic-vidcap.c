#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
 
#include "capture_and_encoding.h"
#include "version.h"

char const* inputVideoFileName = "/tmp/h264_fifo";
char const* inputVideoFileNameLow = "/tmp/h264_fifo_low";
char const* inputAudioFileName = "/tmp/g711_fifo";

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
    unlink(inputVideoFileName);
    unlink(inputVideoFileNameLow);

    if (mkfifo(inputVideoFileName, 0777) < 0) {
        printf("mkfifo Failed\n");
        exit(1);
    }
    if (mkfifo(inputVideoFileNameLow, 0777) < 0) {
        printf("mkfifo Failed\n");
        exit(1);
    }

    int fd = open(inputVideoFileName, O_RDWR | O_CREAT | O_TRUNC, 0777);
    int fd_low = open(inputVideoFileNameLow, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fd < 0) {
        printf("Failed open fifo\n");
        exit(1);
    }
    while (1) {
        if (get_stream(fd , fd_low,0) < 0) break; // Get stream and write to fifo.
    }

    return 0; // only to prevent compiler warning
}

forked from:
https://github.com/geekman/t20-rtspd

Capture video and output to /tmp/h264_fifo
use go2rtc:
` - exec:cat /tmp/h264_fifo | tee`

TO DO:

Audio support

Compilation
============

1. Download the T20 toolchain, either from [the Dafang-Hacks GitHub repo](https://github.com/Dafang-Hacks/Ingenic-T10_20/tree/master/resource/toolchain) or the [Tuya GitHub repo](https://github.com/TuyaInc/TUYA_IPC_SDK/tree/master/mips-linux-4.7.2_64Bit).

2. Set up the `PATH` environment variable to add the toolchain directory.

2. Clone this repo

3. Run the `make` command.
   You should end up with the `ingenic-vidcap` binary in the directory.


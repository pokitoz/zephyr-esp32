# See https://github.com/zephyrproject-rtos/docker-image
FROM ghcr.io/zephyrproject-rtos/zephyr-build:v0.26-branch

WORKDIR /workdir/
RUN west init
WORKDIR  /workdir/zephyr
RUN west update
RUN west zephyr-export
RUN west blobs fetch hal_espressif

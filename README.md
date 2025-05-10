# zephyr-esp32

Base template for esp32 projects
See https://docs.zephyrproject.org/latest/boards/espressif/esp32_devkitc/doc/index.html for documentation.

Setup "Dev Containers", "Remote Development
" in VSCode to build.

Settings -> Dev Containers -> Execute in WSL (Applies to all profiles)

sudo apt install docker-buildx

west init
west update
west blobs fetch hal_espressif

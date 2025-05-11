# zephyr-esp32

Base template for esp32 projects
See https://docs.zephyrproject.org/latest/boards/espressif/esp32_devkitc/doc/index.html for documentation.

Setup "Dev Containers", "Remote Development" in VSCode to build.
Settings -> Dev Containers -> Execute in WSL (Applies to all profiles)
sudo apt install docker-buildx


west build -p always -b esp32_devkitc/esp32/procpu


https://docs.espressif.com/projects/esp-test-tools/en/latest/esp32/production_stage/tools/flash_download_tool.html
Select ESP32
Select zephyr.bin @0x1000
Select COM port
Start

Open Teraterm and see logs from UART.


import asyncio
import collections
from bleak import BleakClient, BleakScanner
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# UUIDs must match the ones from your Zephyr code
SERVICE_UUID = "12345678-9abc-4def-8012-3456789abcdf".lower()
CHAR_UUID = "abcdef01-2345-6789-abcd-ef0123456789".lower()

# Store the last N samples
MAX_POINTS = 200
data_queue = collections.deque(maxlen=MAX_POINTS)

def handle_notify(_, data):
    if len(data) >= 2:
        value = int.from_bytes(data[:2], byteorder='little', signed=True)
        data_queue.append(value)

async def run_ble():
    print("Scanning for BLE device...")
    device = await BleakScanner.find_device_by_filter(
        lambda d, ad: SERVICE_UUID in [s.lower() for s in ad.service_uuids or []]
    )
    if not device:
        print("No matching device found.")
        return

    async with BleakClient(device) as client:
        print(f"Connected to {device.name}")
        await client.start_notify(CHAR_UUID, handle_notify)

        # Keep BLE running in background while plotting
        while True:
            await asyncio.sleep(1)

def update_plot(frame, line):
    if data_queue:
        line.set_ydata(list(data_queue) + [0] * (MAX_POINTS - len(data_queue)))
        line.set_xdata(range(len(data_queue)))
    return line,

def main():
    loop = asyncio.get_event_loop()

    # Start BLE in background
    asyncio.ensure_future(run_ble())

    # Set up the plot
    fig, ax = plt.subplots()
    ax.set_ylim(0, 4096)  # Assuming 12-bit ADC
    ax.set_xlim(0, MAX_POINTS)
    line, = ax.plot([], [], lw=2)
    ax.set_title("Live ADC Stream from ESP32 via BLE")
    ax.set_xlabel("Samples")
    ax.set_ylabel("ADC Value")

    ani = animation.FuncAnimation(fig, update_plot, fargs=(line,), interval=50)
    plt.tight_layout()
    plt.show()

    loop.run_forever()

if __name__ == "__main__":
    main()

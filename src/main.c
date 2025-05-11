
#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <math.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/gatt.h>

#include <zephyr/drivers/adc.h>

#define ADC_RESOLUTION 12
#define ADC_GAIN ADC_GAIN_1
#define ADC_REFERENCE ADC_REF_INTERNAL
#define ADC_ACQ_TIME ADC_ACQ_TIME_DEFAULT
#define ADC_CHANNEL_ID 0
#define BUFFER_SIZE 1

const struct device *adc_dev;
int16_t sample_buffer[BUFFER_SIZE];

static void configure_adc(void)
{
    adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc1));
    struct adc_channel_cfg channel_cfg = {
        .gain = ADC_GAIN,
        .reference = ADC_REFERENCE,
        .acquisition_time = ADC_ACQ_TIME,
        .channel_id = ADC_CHANNEL_ID,
        .differential = 0,
    };
    adc_channel_setup(adc_dev, &channel_cfg);
}

/* BLE UUIDs (custom service + characteristic) */
#define BT_UUID_AUDIO_SERVICE_VAL \
    BT_UUID_128_ENCODE(0x12345678, 0x9abc, 0x4def, 0x8012, 0x3456789abcdf)
static struct bt_uuid_128 audio_service_uuid = BT_UUID_INIT_128(BT_UUID_AUDIO_SERVICE_VAL);

#define BT_UUID_AUDIO_CHAR_VAL \
    BT_UUID_128_ENCODE(0xabcdef01, 0x2345, 0x6789, 0xabcd, 0xef0123456789)
static struct bt_uuid_128 audio_char_uuid = BT_UUID_INIT_128(BT_UUID_AUDIO_CHAR_VAL);

static void audio_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    printk("Notifications %s\n", value ? "enabled" : "disabled");
}

static ssize_t read_audio(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, sample_buffer, sizeof(sample_buffer));
}

BT_GATT_SERVICE_DEFINE(audio_svc,
                       BT_GATT_PRIMARY_SERVICE(&audio_service_uuid),
                       BT_GATT_CHARACTERISTIC(&audio_char_uuid.uuid,
                                              BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_READ,
                                              BT_GATT_PERM_READ,
                                              read_audio, NULL, sample_buffer),
                       BT_GATT_CCC(audio_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), );

static void adc_sample_and_notify(void)
{

    static struct adc_sequence sequence = {
        .channels = BIT(ADC_CHANNEL_ID),
        .buffer = sample_buffer,
        .buffer_size = sizeof(sample_buffer),
        .resolution = ADC_RESOLUTION,
    };

    if (adc_read(adc_dev, &sequence) != 0)
    {
        printk("ADC read failed\n");
        return;
    }
    printk("Sound level: %u\n", sample_buffer[0]);

    // Notify BLE central
    bt_gatt_notify(NULL, &audio_svc.attrs[1], &sample_buffer[0], sizeof(sample_buffer[0]));
}

void main(void)
{
    int err;

    printk("Starting BLE + ADC streaming example...\n");

    configure_adc();

    err = bt_enable(NULL);
    if (err)
    {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }
    printk("Bluetooth initialized\n");

    err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, NULL, 0, NULL, 0);
    if (err)
    {
        printk("Advertising failed to start (err %d)\n", err);
        return;
    }
    printk("Advertising started\n");

    while (1)
    {
        adc_sample_and_notify();
        k_msleep(20);
    }
}

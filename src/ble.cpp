#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "printer.h"

#define PRINTER_SERVICE_UUID "7E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define FREE_BUFFER_SIZE_UUID "7E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define TEMPERATURE_UUID "7E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define BATTERY_VOLTS_UUID "7E400004-B5A3-F393-E0A9-E50E24DCCA9E"
#define LACK_OF_PAPER_UUID "7E400005-B5A3-F393-E0A9-E50E24DCCA9E"
#define PRINT_IMAGE_DATA_UUID "7E400006-B5A3-F393-E0A9-E50E24DCCA9E"

class MiniPrinterServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *s)
    {
        Serial.println("New connection");
        printer_des.connected = true;
    }
    void onDisconnect(BLEServer *s)
    {
        Serial.println("Delete connection");
        printer_des.connected = false;
        s->startAdvertising();
    }
};

class MiniPrinterCharateristicsCallback : public BLECharacteristicCallbacks
{
public:
    void onWrite(BLECharacteristic *c)
    {
        size_t data_len = c->getLength();
        uint8_t nlines = (data_len + PRINTER_LINE_BYTES - 1) / PRINTER_LINE_BYTES;
        uint8_t *data = c->getData();

        Serial.print("data_len: ");
        Serial.println(data_len);
        Serial.print("nlines: ");
        Serial.println(nlines);

        xSemaphoreTake(printer_des.buffer_mutex, 0);

        memcpy(printer_des.buffer[printer_des.write_ptr], data, data_len);
        // memset(printer_des.buffer[printer_des.write_ptr], *data, PRINTER_LINE_BYTES);
        printer_des.write_ptr = (printer_des.write_ptr + nlines) % PRINTER_MAX_NLINE;
        printer_des.size += nlines;
        printer_des.free -= nlines;

        xSemaphoreGive(printer_des.buffer_mutex);
    }

    void onRead(BLECharacteristic *c)
    {
        BLEUUID uuid = c->getUUID();
        if (uuid.equals(BLEUUID::fromString(FREE_BUFFER_SIZE_UUID)))
        {
            Serial.println("client read free buffer size...");

            xSemaphoreTake(printer_des.buffer_mutex, 0);
            uint32_t free_buffer_size = printer_des.free;
            c->setValue(free_buffer_size);
            xSemaphoreGive(printer_des.buffer_mutex);
        }
        else if (uuid.equals(BLEUUID::fromString(TEMPERATURE_UUID)))
        {
            Serial.println("client read temperature...");
            float temp = printer_des.temp;
            c->setValue(temp);
        }
        else if (uuid.equals(BLEUUID::fromString(BATTERY_VOLTS_UUID)))
        {
            Serial.println("client read free battery volts...");
            float vlots = printer_des.battery;
            c->setValue(vlots);
        }
        else if (uuid.equals(BLEUUID::fromString(LACK_OF_PAPER_UUID)))
        {
            Serial.println("client read if is lack of paper...");
            int lackof_paper = printer_des.lackof_paper;
            c->setValue(lackof_paper);
        }
        else
        {
            Serial.println("INVALID uuid to read: ");
        }
    }
};

void ble_init(void)
{
    // Create the BLE Device
    BLEDevice::init("mini printer v1");

    // Create the BLE Server
    BLEServer *printer_server = BLEDevice::createServer();
    printer_server->setCallbacks(new MiniPrinterServerCallbacks());

    // Create the BLE Service
    BLEService *printer_service = printer_server->createService(PRINTER_SERVICE_UUID);

    // Create BLE Characteristics
    MiniPrinterCharateristicsCallback *callback = new MiniPrinterCharateristicsCallback();

    BLECharacteristic *free_buffer_size = printer_service->createCharacteristic(FREE_BUFFER_SIZE_UUID, BLECharacteristic::PROPERTY_READ);
    BLECharacteristic *temperature = printer_service->createCharacteristic(TEMPERATURE_UUID, BLECharacteristic::PROPERTY_READ);
    BLECharacteristic *battery_volts = printer_service->createCharacteristic(BATTERY_VOLTS_UUID, BLECharacteristic::PROPERTY_READ);
    BLECharacteristic *lack_of_paper = printer_service->createCharacteristic(LACK_OF_PAPER_UUID, BLECharacteristic::PROPERTY_READ);
    BLECharacteristic *print_image_data = printer_service->createCharacteristic(PRINT_IMAGE_DATA_UUID, BLECharacteristic::PROPERTY_WRITE);

    free_buffer_size->setCallbacks(callback);
    temperature->setCallbacks(callback);
    battery_volts->setCallbacks(callback);
    lack_of_paper->setCallbacks(callback);
    print_image_data->setCallbacks(callback);

    // Start the service
    printer_service->start();

    // Start advertising
    printer_server->getAdvertising()->start();
}
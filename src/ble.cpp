#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "printer.h"

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MiniPrinterServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *s)
    {
        Serial.println("new connection");
        printer_des.connected = true;
    }
    void onDisconnect(BLEServer *s)
    {
        Serial.println("delete connection");
        printer_des.connected = false;
        s->startAdvertising();
    }
};

class MiniPrinterDateReceiveCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *c)
    {
        size_t data_len = c->getLength();
        uint8_t *data = c->getData();

        Serial.print("data_len: ");
        Serial.println(data_len);

        Serial.print("data: ");
        Serial.println(*data);

        Serial.print("wptr: ");
        Serial.println(printer_des.write_ptr);

        Serial.print("size: ");
        Serial.println(printer_des.size);

        Serial.print("connected: ");
        Serial.println(printer_des.connected);

        Serial.print("temp: ");
        Serial.println(printer_des.temp);

        Serial.print("lack paper: ");
        Serial.println(printer_des.lackof_paper);

        Serial.print("battery: ");
        Serial.println(printer_des.battery);

        // memcpy(printer_des.buffer[printer_des.write_ptr], data, data_len);
        memset(printer_des.buffer[printer_des.write_ptr], *data, 48);
        printer_des.write_ptr = (printer_des.write_ptr + 1) % PRINTER_MAX_NLINE;
        printer_des.size++;
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
    BLEService *printer_service = printer_server->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    BLECharacteristic *pTxCharacteristic = printer_service->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY);

    pTxCharacteristic->addDescriptor(new BLE2902());

    BLECharacteristic *data_input_character = printer_service->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE);

    data_input_character->setCallbacks(new MiniPrinterDateReceiveCallbacks());

    // Start the service
    printer_service->start();

    // Start advertising
    printer_server->getAdvertising()->start();
}
#include "BLEDevice.h" 

static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b"); 

static BLEUUID  charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8"); 

 

static boolean doConnect = false; 

static boolean connected = false; 

static boolean doScan = false; 

static BLERemoteCharacteristic* pRemoteCharacteristic; 

static BLEAdvertisedDevice* myDevice; 

 

static void notifyCallback( 

 BLERemoteCharacteristic* pBLERemoteCharacteristic, 

 uint8_t* pData, 

 size_t length, 

 bool isNotify) { 

  Serial.print("Notify callback for characteristic "); 

  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str()); 

  Serial.print(" of data length "); 

  Serial.println(length); 

  Serial.print("data: "); 

  Serial.write(pData, length); 

  Serial.println(); 

} 

 

class MyClientCallback : public BLEClientCallbacks { 

 void onConnect(BLEClient* pclient) { 

 } 

 

 void onDisconnect(BLEClient* pclient) { 

  connected = false; 

  Serial.println("onDisconnect"); 

 } 

}; 

 

bool connectToServer() { 

  Serial.print("Forming a connection to "); 

  Serial.println(myDevice->getAddress().toString().c_str()); 

   

  BLEClient* pClient = BLEDevice::createClient(); 

  Serial.println(" - Created client"); 

 

  pClient->setClientCallbacks(new MyClientCallback()); 

  pClient->connect(myDevice);  

  Serial.println(" - Connected to server"); 

  pClient->setMTU(517);  

  

  BLERemoteService* pRemoteService = pClient->getService(serviceUUID); 

  if (pRemoteService == nullptr) { 

   Serial.print("Failed to find our service UUID: "); 

   Serial.println(serviceUUID.toString().c_str()); 

   pClient->disconnect(); 

   return false; 

  } 

  Serial.println(" - Found our service"); 

 

  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID); 

  if (pRemoteCharacteristic == nullptr) { 

   Serial.print("Failed to find our characteristic UUID: "); 

   Serial.println(charUUID.toString().c_str()); 

   pClient->disconnect(); 

   return false; 

  } 

  Serial.println(" - Found our characteristic"); 

 

   

  if(pRemoteCharacteristic->canRead()) { 

   std::string value = pRemoteCharacteristic->readValue(); 

   Serial.print("The characteristic value was: "); 

   Serial.println(value.c_str()); 

  } 

 

  if(pRemoteCharacteristic->canNotify()) 

   pRemoteCharacteristic->registerForNotify(notifyCallback); 

 

  connected = true; 

  return true; 

} 

 

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks { 

/** 

 * Called for each advertising BLE server. 

 */ 

 void onResult(BLEAdvertisedDevice advertisedDevice) { 

  Serial.print("BLE Advertised Device found: "); 

  Serial.println(advertisedDevice.toString().c_str()); 

 

  // We have found a device, let us now see if it contains the service we are looking for. 

  if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) { 

 

   BLEDevice::getScan()->stop(); 

   myDevice = new BLEAdvertisedDevice(advertisedDevice); 

   doConnect = true; 

   doScan = true; 

 

  } // Found our server 

 } // onResult 

}; // MyAdvertisedDeviceCallbacks 

 

void setup() { 

 Serial.begin(115200); 

 Serial.println("Starting Arduino BLE Client application..."); 

 BLEDevice::init(""); 

 BLEScan* pBLEScan = BLEDevice::getScan(); 

 pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); 

 pBLEScan->setInterval(1349); 

 pBLEScan->setWindow(449); 

 pBLEScan->setActiveScan(true); 

 pBLEScan->start(5, false); 

} // End of setup. 

void loop() { 

 

 if (doConnect == true) { 

  if (connectToServer()) { 

   Serial.println("We are now connected to the BLE Server."); 

  } else { 

   Serial.println("We have failed to connect to the server; there is nothin more we will do."); 

  } 

  doConnect = false; 

 } 

 if (connected) { 

  String newValue = "Time since boot: " + String(millis()/1000); 

  Serial.println("Setting new characteristic value to \"" + newValue + "\""); 

    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length()); 

 }else if(doScan){ 

  BLEDevice::getScan()->start(0); // this is just example to start scan after disconnect, most likely there is better way to do it in arduino 

 } 

  

 delay(1000); // Delay a second between loops. 

} // End of loop
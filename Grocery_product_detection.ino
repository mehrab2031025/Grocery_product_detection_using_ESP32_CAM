// 27_EdgeImpulse_FOMO.ino
#define MAX_RESOLUTION_VGA 1

/**
 * Run Edge Impulse FOMO model on the Esp32 camera
 */

// replace with the name of your library

#include <HX711_ADC.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h> 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_I2C_ADDRESS 0x3C

Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

const int HX711_dout = 13;
const int HX711_sck = 12;

HX711_ADC LoadCell(HX711_dout, HX711_sck);
#include <Z_transform_inferencing.h>
#include "esp32cam.h"
#include "esp32cam/tinyml/edgeimpulse/FOMO.h"


using namespace Eloquent::Esp32cam;

Cam cam;
TinyML::EdgeImpulse::FOMO fomo;

String test_veg[7] = {};     // Temporary array to store detected objects for 5 seconds
float fin_weight[10] = {};
String unit_price[4][2] = {{"Banana", "0.16"},
                           {"Onion", "0.1"},
                           {"Garlic", "0.14"},
                           {"Potato", "0.03"}};
 
float TOTAL = 0;
unsigned long startTime = 0; // Timer to track 5 seconds
int testIndex = 0;           // Index for `test_veg`
const float WEIGHT_THRESHOLD = 10;
const float RETURN_WEIGHT = 10;

void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    Wire.begin(14, 15); // 14 is SDA and 15 is SCK
    screen.begin(SSD1306_SWITCHCAPVCC, SCREEN_I2C_ADDRESS);
    screen.clearDisplay();

    // Display welcome message
    screen.setTextSize(2);
    screen.setTextColor(WHITE);
    screen.setCursor(0, 0);
    screen.println("Welcome to");
    screen.display();
    screen.setTextSize(2.5);
    screen.setTextColor(WHITE);
    screen.setCursor(0, 32);
    screen.println("AutoBill");
    screen.display();
    delay(3000);
    screen.clearDisplay();

    LoadCell.begin();
    float calibrationValue;
    calibrationValue = 414.14;

    unsigned long stabilizingtime = 3000;
    boolean _tare = true;
    LoadCell.start(stabilizingtime, _tare);

    if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
    }
    else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
    }

    cam.aithinker();
    cam.highQuality();
    cam.highestSaturation();
    cam.vga();

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());
}

void loop() {

  Serial.println("In delay 5 sec");
  delay(5000);


testIndex = 0;
for (int i = 0; i < 7; i++) test_veg[i] = "";  // Erasing the test_veg


do {
    if (!cam.capture()) {
        Serial.println(cam.getErrorMessage());
        return;
    }
    if (!fomo.detectObjects(cam)) {
        Serial.println(fomo.getErrorMessage());
        return;
    }

    String detectedObj = "";
    if (fomo.hasObjects()) {
        fomo.forEach([&detectedObj](size_t, ei_impulse_result_bounding_box_t bbox) {
            if (detectedObj == "") detectedObj = bbox.label;
        });
    }

    if (detectedObj != "") {                 // only store if we got something
        test_veg[testIndex] = detectedObj;
        testIndex++;
        Serial.print("object detected --> ");
        
    }

    for (int i = 0; i < testIndex; i++) Serial.print(test_veg[i]);
    Serial.println();
    Serial.print("no object detected");
    delay(500);

} while (testIndex < 7);


    // Throwing out 40 values
    for (int i = 0; i < 40; i++){
    readWeight(); // Function to read weight from load cell
    delay(100);
 
    } 

    float Weight = readWeight();

    // Check if 5 seconds have passed
  // if (millis() - startTime >= 5000) {
    // Find the most frequent object in test_veg
    String mostFrequent = findMostFrequent(test_veg, 7);


    // Add to fin_veg and update the display
    if (mostFrequent != "" && Weight > WEIGHT_THRESHOLD && Weight > 0) {  
      float price = calculatePrice(mostFrequent, Weight, unit_price);
      TOTAL = TOTAL + price;
      updateOLED(mostFrequent, Weight, TOTAL);
      while (readWeight() > RETURN_WEIGHT)
      {}
    } else {
      updateOLED("Nothing", 0, TOTAL);
    }

  //   startTime = millis();
  // }
  }




String findMostFrequent(String arr[], int size) {
  String maxObj = "";
  int maxCount = 0;

  for (int i = 0; i < size; i++) {
    if (arr[i] == "") continue; // Skip empty slots

    int count = 0;
    for (int j = 0; j < size; j++) {
      if (arr[i] == arr[j]) count++;
    }

    if (count > maxCount) {
      maxCount = count;
      maxObj = arr[i];
    }
  }

  return maxObj;
}


void updateOLED(String message1, float message2, float message3) {
  screen.clearDisplay();
  screen.setTextSize(1);
  screen.setTextColor(WHITE);
  screen.setCursor(0, 0);
  screen.println(message1 + "->" + String(message2) + " g");
  screen.display();

  screen.setTextSize(1);
  screen.setCursor(0, 32);
  screen.println("Total: " + String(message3) + " tk");
  screen.display();

}


float readWeight() {
  static boolean newDataReady = 0;

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
  float i = LoadCell.getData();
  Serial.print("Load_cell output val: ");
  Serial.println(i);
  newDataReady = 0;
  return i;
}
}


float calculatePrice(String V, float W, String U[][2]){
  for (int i= 0; i<4 ; i++){
    if(U[i][0] == V){
      float pricePerGram = U[i][1].toFloat();
      return pricePerGram * W;
    }
  }
  return 0;
}


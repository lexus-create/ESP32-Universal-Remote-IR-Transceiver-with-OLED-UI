
#include <Wire.h>              // I2C communication (for OLED)
#include <Adafruit_GFX.h>      // OLED graphics library
#include <Adafruit_SSD1306.h>  // OLED driver
#include <IRremote.hpp>        // IR receive + transmit
#include <SD.h>                // SD card read/write
#include <SPI.h>               // SD card communication protocol

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

String irsaved[50];
int irCount = 0;
int lastSecondShown = -1;

const int SDA_PIN = 23;
const int SCL_PIN = 22;
const int CS_PIN = 16;
const int but1 = 19;
const int but2 = 18;
unsigned long receiveStartTime = 0;
char end_byte = 0xEF;

static const unsigned char PROGMEM image_SmallArrowUp_bits[] = {0x0c,0x00,0x0c,0x00,0x3f,0x00,0x3f,0x00,0xff,0xc0,0xff,0xc0};

static const unsigned char PROGMEM image_SmallArrowUp_copy_1_bits[] = {0xff,0xc0,0xff,0xc0,0x3f,0x00,0x3f,0x00,0x0c,0x00,0x0c,0x00};

const int ir_transmit = 21;
const int ir_read = 5;

int selectedIndex = 0;
int selectedtransmit = 0;
int oldtransmit = 0;

bool isreceiving = false;
bool intransmit = false;
bool hasdonearray = false;
bool inrepeat = false;

File captured_file;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

IRsend irsend(ir_transmit);

void transmitMode(void) {
    display.clearDisplay();
    File captured_file = SD.open("/captured.txt");
    while (captured_file.available() && irCount < 50) {
        irsaved[irCount] = captured_file.readStringUntil('\n');
        irCount++;
    }
    hasdonearray = true;
    captured_file.close();
    display.clearDisplay();
    intransmit = true;
    



}



void drawScreen_repeat(void) {
    display.clearDisplay();
    // string 1
    display.setTextColor(1);
    display.setTextWrap(false);
    display.setCursor(23, 29);
    display.print("Sending Repeated Last Signal");
    display.display();
}

void repeat_mode(void) {
    if (hasdonearray == true) {
        drawScreen_repeat();
        inrepeat = true;
        unsigned long code = strtoul(irsaved[oldtransmit].c_str(), NULL, 16);
        for (int i = 0; i < 10; i++) {
            irsend.sendNEC(code, 32);
            delay(1);
        }
    } else {
        display.clearDisplay();

        display.setTextColor(1);
        display.setTextWrap(false);
        display.setCursor(8, 29);
        display.print("Transmit Once First");
        display.display();
    }
    delay(1000);
    display.clearDisplay();
    inrepeat = false;
}

void drawScreen_2(String signal) {
    display.clearDisplay();
    // string 1
    display.setTextColor(1);
    display.setTextWrap(false);
    display.setCursor(19, 0);
    display.print("Select A Signal ");
    // string 2
    display.setCursor(26, 30);
    display.print(signal);
    // SmallArrowUp
    display.drawBitmap(59, 18, image_SmallArrowUp_bits, 10, 6, 1);
    // SmallArrowUp copy 1
    display.drawBitmap(59, 43, image_SmallArrowUp_copy_1_bits, 10, 6, 1);
    display.display();
}

void EnterArraySearch(void) {
    drawScreen_2(irsaved[selectedtransmit]);
    if (digitalRead(but1) == LOW) {
        if (selectedtransmit < irCount - 1) {
            selectedtransmit++;
        } else {
            selectedtransmit = 0;
        }
        
    }
    if (digitalRead(but2) == LOW) {
        oldtransmit = selectedtransmit;
        intransmit = false;
        unsigned long code = strtoul(irsaved[selectedtransmit].c_str(), NULL, 16);
        irsend.sendNEC(code, 32); 
        selectedtransmit = 0;
        display.clearDisplay();
        irCount = 0;
    }
}






void receiveMode(void) {
    isreceiving = true;
    receiveStartTime = millis();  // set ONCE, right when receiving begins
}



void drawScreen_1(void) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);
    display.setTextSize(1);

    // Title
    display.setCursor(1, 2);
    display.print("v1.0");
    display.setCursor(27, 2);
    display.print("IRTransceiver");

    // Menu items
    drawMenuItem(0, 55, 39, "Receive Mode");
    drawMenuItem(1, 49, 47, "Transmit Mode");
    drawMenuItem(2, 61, 55, "Repeat Mode");

    display.display();
}

void drawMenuItem(int index, int x, int y, const char* text) {
    if (index == selectedIndex) {
        // Highlight selected item
        display.fillRect(x - 6, y - 1, 80, 9, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK); // Invert text
    } else {
        display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(x, y);
    display.print(text);
    display.setTextColor(SSD1306_WHITE); // Reset color
}

void updateMenu() {
    if (digitalRead(but1) == LOW && digitalRead(but2) == HIGH) { // Up
        selectedIndex--;
        if (selectedIndex < 0) selectedIndex = 2;
        display.clearDisplay();
        drawScreen_1();
        delay(1000); // debounce
    }

    if (digitalRead(but2) == LOW && digitalRead(but1) == HIGH) { // Down
        selectedIndex++;
        if (selectedIndex > 2) selectedIndex = 0;
        display.clearDisplay();
        drawScreen_1();
        delay(200);
    }

    // Example: Press both buttons to select
    if (digitalRead(but1) == LOW && digitalRead(but2) == LOW) {
        executeMenuAction(selectedIndex);
        delay(500);
    }
}

void executeMenuAction(int index) {
    if (index == 0) {
        Serial.println("Receive Mode selected");
        receiveMode();
        
    } else if (index == 1) {
        Serial.println("Transmit Mode selected");
        transmitMode();
    } else if (index == 2) {
        Serial.println("Repeat Mode selected");
        repeat_mode();
    }
}

void setup() {
    // put your setup code here, to run once:
    

    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    IrReceiver.begin(ir_read, ENABLE_LED_FEEDBACK);
    irsend.begin(ir_transmit, false, 0);

    // Initialize SD card
    
    SPI.begin(4, 15, 2, CS_PIN);
    if (!SD.begin(CS_PIN)) {
        Serial.println("SD card initialization failed!");
        while (true); // Stop if SD fails
    }

    Serial.println("SD card initialized.");

    captured_file = SD.open("/captured.txt", FILE_WRITE);

    if (!captured_file) {
        Serial.println("Failed to create/open captured.txt");
    } else {
        Serial.println("captured.txt is ready.");
        captured_file.close(); // Close immediately
    }
    

    pinMode(but1, INPUT_PULLUP);
    pinMode(but2, INPUT_PULLUP);

    drawScreen_1();
}

void loop() {
    if (isreceiving == false && intransmit == false && inrepeat == false) {
        updateMenu();
  
    }
    
    if (intransmit) {
        EnterArraySearch();
        delay(5);
    }

    if (IrReceiver.decode() && isreceiving == true) {
        Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
        captured_file = SD.open("/captured.txt", FILE_WRITE);

        if (!captured_file) {
            Serial.println("Failed to create/open captured.txt");
        } else {
            captured_file.println(IrReceiver.decodedIRData.decodedRawData, HEX);
            captured_file.close();
        }

        IrReceiver.resume();
    } else if (IrReceiver.decode() && isreceiving == false) {
        IrReceiver.resume();
    }
  
    if (isreceiving) {
        unsigned long elapsed = millis() - receiveStartTime;
        int secondsLeft = 5 - (elapsed / 1000);

        if (secondsLeft != lastSecondShown) {
           lastSecondShown = secondsLeft;
           display.clearDisplay();
           display.setTextColor(1);
           display.setTextWrap(false);
           display.setCursor(3, 3);
           display.setTextSize(1);
           display.print("RECEIVING...");
           display.setTextSize(4);
           display.setCursor(54, 22);
           display.print(secondsLeft);
           display.display();
        }

        
  
        if (elapsed >= 5000) {
            isreceiving = false;
            drawScreen_1();
        }
    }  


}


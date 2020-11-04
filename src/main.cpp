/*
 Customizable Minecraft Ore Lamp  (c) by Diego Mariño

 This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.

 You should have received a copy of the license along with this work. If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.

Shoutout to Dan J Hamer thingiverse project : https://www.thingiverse.com/thing:3401758 (also licensed through CC-BY-SA-4.0). I've simplified the code and added editable Fusion360 models.
*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include <avr/sleep.h>

#define LEDPIN 4       // Digital pin to control the leds
#define PIXELS 144     // Number of WS2812B leds connected
#define BUTTON 2       // Digital pin to control the pushbutton
#define MEMADDRESS 222 // Memory address. Remember that it doesn't physically last more than 100k write operations (or even less!)
#define SLEEPMILLIS 1800000 // Milliseconds before sleeping the arduino  and setting the leds to (0,0,0)

byte selectedColor = 0; // First dimension index initialization for elementsRGBArray

unsigned long elapsedMillis = 0;

const int elementsRGBArray[][3] = {
    {0, 255, 255},  // DIAMOND
    {41, 69, 225},  // LAPISLAZULI
    {27, 225, 20},  // EMERALD
    {255, 0, 0},    // REDSTONE
    {255, 215, 0},  // GOLD
    {228, 198, 97}, // IRON
    {220, 220, 220} // SILVER
};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

void changeColor()
{
    if (digitalRead(BUTTON) == HIGH)
    {
        selectedColor++;

        if(selectedColor >= (sizeof(elementsRGBArray) / sizeof(elementsRGBArray[0])))
        {
            selectedColor = 0;
        }

        EEPROM.put(MEMADDRESS, selectedColor);
        asm volatile("  jmp 0"); // let's reset the board
    }
}
void setAllPixels(byte red, byte green, byte blue)
{
    for (int i = 0; i < PIXELS; i++)
    {
        strip.setPixelColor(i, red, green, blue);
    }
    strip.show();
}

void FadeInOut(byte red, byte green, byte blue) // Check https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/ for other effects
{
    float r, g, b;

    for (int k = 10; k < 256; k = k + 1) // using k = 10 instead of 0 to not switch off completely the light
    {
        r = (k / 256.0) * red;
        g = (k / 256.0) * green;
        b = (k / 256.0) * blue;
        setAllPixels(r, g, b);
        strip.show();
        delay(random(1, 50));             // adding delay randomness on every step
    }

    for (int k = 255; k >= 10; k = k - 2) // using 10 instead of 0 to not switch off the light
    {
        r = (k / 256.0) * red;
        g = (k / 256.0) * green;
        b = (k / 256.0) * blue;
        setAllPixels(r, g, b);
        strip.show();

        delay(random(1, 50));             // adding delay randomness on every step
    }
}

void setup()
{

    Serial.begin(9600);

    strip.begin();
    strip.show();

    pinMode(2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON), changeColor, CHANGE); // Link the changeColor function to pressing the pushbutton

}

void loop()
{
elapsedMillis  = millis();


    Serial.println(elapsedMillis);

    if (elapsedMillis  > SLEEPMILLIS)
    {
        setAllPixels(0, 0, 0);
        strip.show();
        sleep_enable();
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_cpu();
    }




    EEPROM.get(MEMADDRESS, selectedColor);
    //Serial.print("'selectedColor' value: ");
    //Serial.println(selectedColor);
    FadeInOut(elementsRGBArray[selectedColor][0], elementsRGBArray[selectedColor][1], elementsRGBArray[selectedColor][2]);
}
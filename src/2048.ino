/*
 * Configuration
 */
#define TABLE_SIZE 4
#define _2048_IMPLEMENTATION
#include "2048.h"
int table[TABLE_SIZE][TABLE_SIZE];
typedef enum {
      flat = 0, 
      l, d, u, r
} Orientation;

#include <FastLED.h>
#define LED_PIN 32
#define LED_BRIGHTNESS 40 // 0-255
#define LED_COUNT 64
CRGB leds[LED_COUNT];

#include <basicMPU6050.h> 
basicMPU6050<> mpu;

#define seed() random()
void setup() 
{
        Serial.begin(9600);
        Serial.println("");
        Serial.println("Initializing");
        Serial.println("");
        srand(seed());
        _2048_table_spawn(table);
        _2048_table_spawn(table);

        Serial.println("Setting up MPU6050...");
        mpu.setup();
        mpu.setBias();
        Serial.println("Done.");

        Serial.println("Setting up leds...");
        FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT);
        FastLED.setBrightness(LED_BRIGHTNESS);
        Serial.println("Done.");

}

/*
 * Logic
 */
CRGB cell_color(int x)
{
        if (x) return CHSV(7*x%360-30, 255, 255);
        else   return CHSV(0, 0, 0); 
}

void draw_cell(int number, int x, int y)
{      
        CRGB color = cell_color(number);
        leds[(2*x+0)*8 + (2*y+0)] = color;
        leds[(2*x+0)*8 + (2*y+1)] = color;
        leds[(2*x+1)*8 + (2*y+0)] = color;
        leds[(2*x+1)*8 + (2*y+1)] = color;

}

void display(int table[][TABLE_SIZE])
{
        for (int i = 0; i < TABLE_SIZE; i++)
                for (int j = 0; j < TABLE_SIZE; j++)
                        draw_cell(table[i][j], i, j);
        FastLED.show();
}

#define THRESHOLD 0.7
bool sub(float f) { return (-THRESHOLD < f && f < +THRESHOLD); }
bool sup(float f) { return (-THRESHOLD > f && f > +THRESHOLD); }

Orientation getOrientation()
{
        Orientation o;
        mpu.updateBias();
        if (sup(mpu.ax()) && sub(mpu.ay()) && sub(mpu.az())) o = flat;
        if (sub(mpu.az()) && mpu.ax() < -THRESHOLD) o = u;
        if (sub(mpu.az()) && mpu.ay() < -THRESHOLD) o = l;
        if (sub(mpu.az()) && mpu.ax() > +THRESHOLD) o = d;
        if (sub(mpu.az()) && mpu.ay() > +THRESHOLD) o = r;
        return o;
}

Orientation prev_orientation = flat;
void loop() 
{ 
        Serial.print("loop ");
        display(table);
        Orientation curr_orientation = getOrientation();
        if (curr_orientation != prev_orientation)
                switch (curr_orientation)
                {
                        case l: _2048_swipe(table, left);  Serial.println("left");  break;
                        case d: _2048_swipe(table, down);  Serial.println("down");  break;
                        case u: _2048_swipe(table, up);    Serial.println("up");    break;
                        case r: _2048_swipe(table, right); Serial.println("right"); break;
                        case flat: Serial.println("flag");
                }

        prev_orientation = curr_orientation;
}

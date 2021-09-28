#include <FastLED.h>

#include <Arduino.h>

#define FASTLED_INTERNAL // Suppress build banner


//Setup for LED strips 
#define NUM_LEDS 174 // FastLED definitions
#define LED_PIN 5

CRGB g_LEDs[NUM_LEDS] = {0}; // Frame buffer for FastLED

int g_Brightness = 100; // 0-255 LED brightness scale

//Define RGB structure for use of passing color values
struct RGB
{
  byte r;
  byte g;
  byte b;
};

void setup() {
  
  //Initializing FastLED library
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(g_LEDs, NUM_LEDS); // Add our LED strip to the FastLED library
  FastLED.setBrightness(g_Brightness);
  FastLED.setMaxPowerInMilliWatts(25000);

  Serial.begin(115200);
  while (!Serial)
  {
  }
  Serial.println("ESP32 Startup complete");

  Serial.print("In Loop");
}

static byte heat[174];
int     Size = 174/2;
uint8_t COOLING = 50;
uint8_t SPARKING = 150;
CRGBPalette16 gPal = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);

void loop()
{
      while(true){
        // Step 1.  Cool down every cell a little
        for( int i = 0; i < Size; i++) {
        heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / Size) + 2));
        }
    
        // Step 2.  Heat from each cell drifts 'up' and diffuses a little
        for( int k= Size - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
        }
        
        // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
        if( random8() < SPARKING ) {
        int y = random8(7);
        heat[y] = qadd8( heat[y], random8(160,255) );
        }

        // Step 4.  Map from heat cells to LED colors
        for( int j = 0; j < Size; j++) {
            // Scale the heat value from 0-255 down to 0-240
            // for best results with color palettes.
            byte colorindex = scale8( heat[j], 240);
            CRGB color = ColorFromPalette( gPal, colorindex);
            int pixelnumber = j;
            FastLED.leds()[pixelnumber] = color;
            FastLED.leds()[Size*2-pixelnumber-1] = color;
        }
        FastLED.show();
        delay(30);
      }
    };

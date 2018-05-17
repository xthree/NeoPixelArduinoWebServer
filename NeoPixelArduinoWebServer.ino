
#include <Ethernet.h>


#include <Adafruit_NeoPixel.h>
#define PIN 7

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(110, PIN, NEO_GRB + NEO_KHZ800);


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //Sets MAC Address (can be anything valid)
IPAddress ip(192, 168, 1, 130); // Sets static Arduino IP
EthernetServer server(80);


//Make variables to be used later
String reqData; // Request from Smartphone
String header;
int contentSize = -1;
String CONTENT_LENGTH_TXT = "Content-Length: ";



/// SET UP
void setup() {

  // put your setup code here, to run once:

  Serial.begin(9600);

  // Pin mode


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  Serial.print("Ready...");




  Ethernet.begin(mac, ip);

  server.begin();

  Serial.println("Begin...");

} //End setup

void loop() {

  EthernetClient client = server.available(); // Checks for a client (phone)

  if (client) {

    // Let's start reading

    boolean isLastLine = true;
    boolean isBody = false;
    header = "";
    reqData = "";
    int contentLen = 0;
    Serial.print("Client connected!");

    while (client.connected()) {
      if (client.available()) {
        // Read data
        char c = client.read();

        // Serial.print(c);

        if (contentSize == contentLen) {


          //Serial.println("Body ["+reqData+"]");
          Serial.println(reqData);

          // Extract the JSON string like [r,g,b]

          int pos1 = reqData.indexOf("[");

          int pos2 = reqData.lastIndexOf("]");

          // Parse the string

          String colors = reqData.substring(pos1 + 1, pos2);

          Serial.println("Colors [" + colors + "]");

          int idx1 = colors.indexOf(',');

          int idx2 = colors.indexOf(',', idx1 + 1);

          int idx3 = colors.indexOf(',', idx2 + 1);

          int idx4 = colors.indexOf(',', idx3 + 1);

          String sRed = colors.substring(0, idx1);

          String sGreen = colors.substring(idx1 + 1, idx2);

          String sBlue = colors.substring(idx2 + 1, idx3);

          String sBright = colors.substring(idx3 + 1, idx4);

          // Convert the Red, Green and Blue string values to int

          int red = sRed.toInt();
          int green = sGreen.toInt();
          int blue = sBlue.toInt();
          //int brightness = sBright.toInt();
          int brightness = 255; //hard coding at full brightness for now
          uint32_t colour =  strip.Color(red, green, blue, brightness);

          // Set the RGB led color according to the values sent by the Android client
          Serial.print("Pre-Set: ");
          Serial.println(colour);
          setColor(colour, brightness); // Does the actual changing of colors

          // Create the response to client

          client.println("HTTP/1.1 200 OK");

          client.println("Content-Type: text/html");

          client.println("Connection: close");

          client.println();

          // send web page

          client.println("<!DOCTYPE html>");

          client.println("<html>");

          delay(1);

          break;

        }



        if (c == '\n' && isLastLine) {

          isBody = true;

          int pos = header.indexOf(CONTENT_LENGTH_TXT);

          String tmp = header.substring(pos, header.length());

          //Serial.println("Tmp ["+tmp+"]");

          int pos1 = tmp.indexOf("\r\n");

          String size = tmp.substring(CONTENT_LENGTH_TXT.length(), pos1);

          Serial.println("Size [" + size + "]");

          contentSize = size.toInt();

        }

        if (isBody) {
          reqData += c;
          contentLen++;
        }
        else {
          header += c;
        }

        if (c == '\n' ) {
          isLastLine = true;
        }

        else if (c != '\r' ) {
          isLastLine = false;
        }

      } // end if available

    } //end while(connected)

    // Close connection

    Serial.println("Stop..");

    client.stop();

  } // end if(client)

} // end loop


//Sets the color of all LEDs to the one passed in, and displays them
void setColor(uint32_t colour, int br) {
  Serial.println(strip.numPixels());
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, colour);
    Serial.println(i);
  }
  strip.setBrightness(br);
  strip.show();
}

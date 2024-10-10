#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>

// Config SSID and password
const char* ssid = "ESP-AP";
const char* password = "123456789";

IPAddress local_IP(10, 0, 0, 1);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);
TFT_eSPI tft = TFT_eSPI();
String selectedColor = "#FFFFFF"; // Default to white

String html = "<html><body>"
              "<form action=\"/display\" method=\"POST\">"
              "Enter text: <input type=\"text\" name=\"inputText\">"
              "<input type=\"color\" name=\"color\" value=\"#FFFFFF\">"
              "<input type=\"submit\" value=\"Submit\">"
              "</form>"
              "</body></html>";

void handleRoot() {
  server.send(200, "text/html", html);
}

void handleDisplay() {
  if (server.hasArg("inputText") && server.hasArg("color")) {
    String inputText = server.arg("inputText");
    String color = server.arg("color");

    long int hexColor = strtol(color.substring(1).c_str(), NULL, 16);
    uint16_t tftColor = tft.color565((hexColor >> 16) & 0xFF, (hexColor >> 8) & 0xFF, hexColor & 0xFF);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(tftColor, TFT_BLACK);

    int textSize = 2;
    tft.setTextSize(textSize);

    while (tft.textWidth(inputText) > tft.width() && textSize > 1) {
      textSize--;
      tft.setTextSize(textSize);
    }

    int cursorX = 10;
    int cursorY = 10;
    String currentLine = "";

    for (char c : inputText) {
      currentLine += c;
      if (tft.textWidth(currentLine) > tft.width()) {
        tft.setCursor(cursorX, cursorY);
        tft.print(currentLine.substring(0, currentLine.length() - 1));
        cursorY += tft.fontHeight();
        currentLine = String(c);
      }
    }
    tft.setCursor(cursorX, cursorY);
    tft.print(currentLine);
  }
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, gateway, subnet);

  Serial.print("Access Point IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/display", HTTP_POST, handleDisplay);
  server.begin();
  Serial.println("HTTP server started");

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("Ready...");
}

void loop() {
  server.handleClient();
}

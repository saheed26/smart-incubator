#include  <ESP8266WiFi.h>               //  Include the Wi-Fi library
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>  //LCD library
#include "DHT.h"
#define DHTPIN 2 // D4 connected to the DHT sensor

#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

ESP8266WebServer server(80);  //Web server

// set LCD address, number of columns and rows
LiquidCrystal_I2C lcd(0x27, 16, 2);  

//LM35 parameter
int outputpin= A0;

//Delay param
long previousTime = 0;

//AP parameters
char APName[30];
char APPass[30];

//*******************Web handles starts***************************//
const char PROGMEM indexPage[] = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <style> @media only screen and (max-width: 600px) {.wrapper{width: 90%;}} @media only screen and (min-width: 600px) {.wrapper{width: 70%;}} @media only screen and (min-width: 992px) {.wrapper{width: 30%;}}</style><title>Notify Settings</title></head><body style=\"background: black; padding: 0px 0px;\"><div class=\"wrapper\" style=\"background: #EBEBEB; margin: 1em auto; padding: 1em ; border: 5px solid white;border-radius: 20px;box-shadow: 5px 5px 5px black; color: black;\"><center><h2>ESP WiFi settings</h2></center><form method=\"post\" action=\"/wifi\">  <div style=\"display:inline; float:left; width:35%;\">WiFi name</div> <div style=\"display:inline;\"><input type=\"text\" name=\"ssid\" maxlength=\"30\" required style=\"border-radius: 7px;\"></div><br><br> <div style=\"display:inline; float:left; width:35%;\">WiFi pass</div> <div style=\"display:inline;\"><input type=\"password\" name=\"password\" maxlength=\"30\" pattern=\".{8,}\" title=\"8 characters minimum\" required style=\"border-radius: 7px;\"></div><br><br> <center><input type=\"submit\" value=\"Save\" style=\"background: black; padding: .4em 1em; color: white; border-color:black; font-family: Arial; font-size: 17px; border-radius: 7px; font-weight:bold;\"></center></form>       <br><hr><center><h2>AP settings</h2></center>      <form method=\"post\" action=\"/internet\">    <div style=\"display:inline; float:left; width:35%;\">AP name</div> <div style=\"display:inline;\"><input type=\"text\" name=\"ssid\" maxlength=\"30\" style=\"border-radius: 7px;\"></div><br><br> <div style=\"display:inline; float:left; width:35%;\">AP pass</div> <div style=\"display:inline;\"><input type=\"password\" name=\"password\" maxlength=\"30\" style=\"border-radius: 7px;\"></div><br><br>   <center><input type=\"submit\" value=\"Save\" style=\"background: black; padding: .4em 1em; color: white; border-color:black; font-family: Arial; font-size: 17px; border-radius: 7px; font-weight:bold;\">  </center></form>       </div><script type=\"text/javascript\"> function res(){ q = document.getElementById(\"group\"); q.value = q.value.toLowerCase(); } function prevspace(event) { if (event.which == 32){event.preventDefault(); return false; }} function blurr(){ m = document.getElementById(\"group\"); mygrp = document.getElementById(\"group\").value; var newgrp = mygrp.replace(/ /g,\"\"); document.getElementById(\"group\").value = newgrp; m.value = m.value.toLowerCase();  }    function prevspacee(event) { if (event.which == 32){event.preventDefault(); return false; }} function blurrr(){ m = document.getElementById(\"server_ip\"); mygrp = document.getElementById(\"server_ip\").value; var newgrp = mygrp.replace(/ /g,\"\"); document.getElementById(\"server_ip\").value = newgrp; m.value = m.value.toLowerCase(); } </script></body></html>";

void  handleRoot()  {                                                   //  When  URI / is  requested,  send  the homepage
  server.send(200,  "text/html", indexPage);
}

//Function to set Local AP WiFi name and password into its inbuilt EEPROM
 void  handleSettingsSave()  {
  String ssid = server.arg("ssid"); 
  String password = server.arg("password"); 

  //Saving WiFi ssid to eeprom
  for(int i=0;i<30;i++){
    EEPROM.write(i,ssid[i]);
    delay(4);
    yield();
  }
  EEPROM.commit();    //Store data to EEPROM
  delay(10);
  
     //Saving WiFi password to eeprom
  for(int i=30;i<60;i++){
    EEPROM.write(i,password[i-30]);
    delay(4);
    yield();
  }
  EEPROM.commit();    //Store data to EEPROM
  delay(10);
  
  server.send(200, "text/html",  "<!DOCTYPE html><html><head><title>Settings sent</title></head><body style=\"background: black; padding: 30px 0px;\"><div style=\"width: 30%; background: #EBEBEB; margin: 1em auto; padding: 1em; border: 5px solid #F7F7F7;border-radius: 20px;box-shadow: 5px 5px 5px black; color: black;\"><center><h4>Settings sent successfully</h4><br> <a href=\"/\" style=\"background: black; padding: .5em 1em; margin-bottom: 1em; color: white; border-color:black; border-radius: 7px; text-decoration: none;\">Go to homepage</a> </center><br></div></body></html>");
}

//Function to get internet Ap name and password and send it to the server-side ESP
 void handleInternetSave()  { 
  String ssid = server.arg("ssid"); 
  String password = server.arg("password");

  if(ssid.length() > 0){
    //Saving AP ssid to eeprom
    for(int i=60;i<90;i++){
      EEPROM.write(i,ssid[i-60]);
      delay(4);
      yield();
    }
    EEPROM.commit();    //Store data to EEPROM
    delay(5);
    
    //Saving AP password to eeprom
    for(int i=90;i<120;i++){
      EEPROM.write(i,password[i-90]);
      delay(4);
      yield();
    }
    EEPROM.commit();    //Store data to EEPROM
    delay(10);
    server.send(200, "text/html",  "<!DOCTYPE html><html><head><title>Settings sent</title></head><body style=\"background: black; padding: 30px 0px;\"><div style=\"width: 30%; background: #EBEBEB; margin: 1em auto; padding: 1em; border: 5px solid #F7F7F7;border-radius: 20px;box-shadow: 5px 5px 5px black; color: black;\"><center><h4>Settings sent successfully</h4><br> <a href=\"/\" style=\"background: black; padding: .5em 1em; margin-bottom: 1em; color: white; border-color:black; border-radius: 7px; text-decoration: none;\">Go to homepage</a> </center><br></div></body></html>");
  }
 }

void  handleNotFound(){
  server.send(404,  "text/html", "<!DOCTYPE html><html><head><title>Page not found</title></head><body style=\"background: black; padding: 30px 0px;\"><div style=\"width: 30%; background: #EBEBEB; margin: 1em auto; padding: 1em; border: 5px solid #F7F7F7;border-radius: 20px;box-shadow: 5px 5px 5px black; color: black;\"><center><h4 style=\"color: red;\">Page NOT found!</h4> <a href=\"/\" style=\"background: black; padding: .8em; margin-bottom: 1em; color: white; border-color:black; border-radius: 7px; text-decoration: none;\">Go to homepage</a> </center><br></div></body></html>");  //  Send  HTTP  status  404 (Not  Found)  when  there's no  handler for the URI in  the request
}

//*******************Web handles ends***************************//

void setup(){
  Serial.begin(9600);
  lcd.init();  // initialize LCD                      
  lcd.backlight();  // turn on LCD backlight
  dht.begin();  //Initialize dht11

  //Display welcome message
  lcd.setCursor(0, 0);
  lcd.print("SMART INCUBATOR");
  lcd.setCursor(5,1);
  lcd.print("SYSTEM");
  delay(5000);

  EEPROM.begin(512);  //Initialize EEPROM
  delay(100);
  //Wifi Reset button
  //pinMode(resetButton, INPUT_PULLUP);
  Serial.println("Welcome");
  
  //Reading EEPROM data
  String wifiName, wifiPass, apName, apPass;
  for(int i=0;i<30;i++) {
    wifiName += char(EEPROM.read(i));
    yield();
  }
  delay(5);
  
   for(int i=30;i<60;i++){
    wifiPass += char(EEPROM.read(i));
    yield();
  }
  delay(5);
  for(int i=60;i<90;i++) {
    apName += char(EEPROM.read(i));
    yield();
  }
  delay(5);
  
   for(int i=90;i<120;i++){
    apPass += char(EEPROM.read(i));
    yield();
  }
  delay(5);

  WiFi.disconnect();
  //WiFi.mode(WIFI_AP);
  
  char ssid[30];
  char password[30];
  wifiName.toCharArray(ssid, 30);
  wifiPass.toCharArray(password, 30);
  WiFi.softAP(ssid, password);

  //Start Web server
  server.on("/",  HTTP_GET, handleRoot);          //  Call  the 'handleRoot'  function  when  a client  requests  URI "/"
  server.on("/internet", HTTP_POST,  handleInternetSave);
  server.on("/wifi", HTTP_POST,  handleSettingsSave);
  server.onNotFound(handleNotFound);                //  When  a client  requests  an  unknown URI (i.e. something other than  "/"), call  function "handleNotFound"
  server.begin();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to:");
  lcd.setCursor(0, 1);
  lcd.print(apName);
  
  //Connecting ESP to AP
  //Converting ssId1 and passWord1 to char
  apName.toCharArray(APName, 30);
  apPass.toCharArray(APPass, 30);
  WiFi.begin(APName, APPass);
  while (WiFi.status() != WL_CONNECTED){
    delay(100);
    yield();
    //Serial.print(".");
    server.handleClient();
  }
  
  lcd.clear();
}

void loop(){
  yield();  
  server.handleClient();
  //dht11 Reading
  int humidity = dht.readHumidity(); //Read humidity
  //int temp_dht = dht.readTemperature(); //Read temperature as Celsius (the default)
  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  //LM35 reading
  int analogValue = analogRead(outputpin);
  float millivolts = (analogValue/1024.0) * 3300; //3300 is the voltage provided by NodeMCU
  int temp_lm35 = millivolts/10;
  temp_lm35 -= 2; 
  //Printing on LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.setCursor(6, 0);
  lcd.print(temp_lm35);
  lcd.setCursor(8, 0);
  lcd.print((char)223);
  lcd.setCursor(9, 0);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Hum: ");
  lcd.setCursor(6, 1);
  lcd.print(humidity);
  lcd.setCursor(8, 1);
  lcd.print("%");

  //Upload to server every 30 seconds
  unsigned long currentTime = millis();
  if(currentTime - previousTime > 30000){
    previousTime = currentTime;
    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
      yield();
      HTTPClient http;  //Declare an object of class HTTPClient
      String clienturl = String("http://api.thingspeak.com/update?api_key=EQD8S8FZJJUR4WT2&field1=" + String(temp_lm35) + "&field2=" + String(humidity));
      http.begin(clienturl);  //Specify request destination
      int httpCode = http.GET();    
      String msg;
      
      //Send the request
      if (httpCode > 0) { //Check the returning code
        msg = http.getString();   //Get the request response payload
        //Serial.println(msg);                     //Print the response payload
      }
      http.end();   //Close connection
    }
  }

  delay(2000);
}

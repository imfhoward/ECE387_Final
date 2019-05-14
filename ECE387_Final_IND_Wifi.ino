#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <Servo.h>

Servo myservo;
SoftwareSerial NodeMCU(D1,D0);

const char* ssid = "2.4-1707 Sassafras";
//const char* ssid = "SilverFox";
const char* password = "CXNK0030C4DB";
//const char* password = "CWn7VbS9";

WiFiServer server(80);

String header,data,air="",tempe="",hum="",output4State = "off";;
int pos =0;
char chrstr[30];
bool autoo = false;

void setup() {
  pinMode(15,OUTPUT);
  myservo.attach(13);
  Serial.begin(115200);
  NodeMCU.begin(9600);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  //NodeMCU.readBytes(chrstr,10);
  //String data(chrstr);
  data = NodeMCU.readString();
  //data = Serial.readString();
  //delay(1);
  Serial.println(data);
  //NodeMCU.println(data);

  if(data.indexOf("Fresh") > 0){
    air="The air is Fresh!";
  }
  else if(data.indexOf("Low") > 0){
    air="The air has low pollution!";
  }
  else if(data.indexOf("High") > 0){
    air="High pollution! Altert!";
  }

  tempe = data.substring(data.indexOf("pp")+2,data.indexOf("pp")+4);
  hum = data.substring(data.indexOf("mm")+2,data.indexOf("mm")+4);
  
  WiFiClient client = server.available();
  if (client) {  
    //data = "!" + data;
    //Serial.println(data);                           
    Serial.println("New Client in.");          
    String currentLine = "";                
    while (client.connected()) {            
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
            }
 
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println("body {background-color: black;}");
            client.println(".button { background-color: Tomato; border: none; color: white; width: 100px; height: 50;border-radius: 8px;");
            client.println("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: Gray;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1><font color ='##FFFFFF'>PJ Fitness Club</h1>");
            client.println("<h2>Air Quality Monitor</h2>");
            client.println("<hr>");
            client.println("<h3 style = color:white> Air Quality Report</h3>");
            if(data.indexOf("High") > 0){
              client.println("<p style = color:red> 0_0 "+air+" 0_0</p>");
              autoo=true;
              }
            else if(data.indexOf("Low") > 0){
              client.println("<p style = color:yellow> =_= "+air+" =_=</p>");
              }
            else{
              client.println("<p style = color:green> ^_^ "+air+" ^_^</p>");
              autoo=false;
              } 
            client.println("<h3 style = color:white> Temperature Report</h3>");
            client.println("<p style = color:green>"+tempe+"&#176C </p>");
            client.println("<h3 style = color:white> Humidity Report</h3>");
            client.println("<p style = color:blue>"+hum+"%</p>");
            client.println("<h3 style = color:white> Air Circulator Control</h3>");
            // If the output4State is off, it displays the ON button   
            if(!autoo){
            if (output4State=="on") {
              digitalWrite(15,HIGH);
              for (pos = 0; pos <= 90; pos += 1) {
                  myservo.write(pos);
                  delay(15);
                }
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            } else {
              digitalWrite(15,LOW);
              for (pos = 90; pos >= 0; pos -= 1) {
                    myservo.write(pos);
                    delay(15);
                }
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
              }
          }
          else{
            digitalWrite(15,LOW);
              for (pos = 90; pos >= 0; pos -= 1) {
                    myservo.write(pos);
                    delay(15);
                }
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">Auto</button></a></p>");
          }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

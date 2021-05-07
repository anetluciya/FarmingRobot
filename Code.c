#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <SoftwareSerial.h>

SoftwareSerial mySerial(16, 5); // RX, TX


MDNSResponder mdns;


const char* ssid = "Felix";
const char* password = "123456789";

ESP8266WebServer server(80);

int gpio1_pin = 2;  // D4 of nodemcu
//int gpio2_pin = 13; // D7 of nodemcu
//int gpio3_pin = 14; //  D5 of nodemcu
//int gpio4_pin = 5; // D1 of nodemcu
//int gpio5_pin = 4; // D2 of nodemcu

bool is_authentified(){
  mySerial.println("Enter is authentified");
  if (server.hasHeader("Cookie")){
    mySerial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    mySerial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1)
    {
      mySerial.println("Authentification Successful");
      return true;
    }
  }
  mySerial.println("Authentification Failed");
  return false;
}

void handleLogin(){
  String msg;
  if (server.hasHeader("Cookie")){
    mySerial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    mySerial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")){
    mySerial.println("Disconnection");
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.sendHeader("Set-Cookie","ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")){
    if (server.arg("USERNAME") == "autofarm" &&  server.arg("PASSWORD") == "system" ) // enter ur username and password you want
    {
      server.sendHeader("Location","/");
      server.sendHeader("Cache-Control","no-cache");
      server.sendHeader("Set-Cookie","ESPSESSIONID=1");
      server.send(301);
      mySerial.println("Log in Successful");
      return;

      }

  msg = "Wrong username/password! try again.";
  mySerial.println("Log in Failed");
  }
  String content = "<html><body style='background-color:MediumAquaMarine'><form action='/login' method='POST'><p  align ='center' style='font-size:300%;'><u><b>  AUTOMATIC CNC FARMING  </b></u></p><br>";
  content += " <p   align ='center' style='font-size:160%'><b> UserName:<input type='text' name='USERNAME' placeholder='user name' required></b></p><br>";
  content += "<p  align ='center' style='font-size:160%'><b>Password:<input type='password' name='PASSWORD' placeholder='password' required></b></p><br>";
  content += "<p  align ='center' style='font-size:160%'><input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "</p><br> </body></html>";
  server.send(200, "text/html", content);
}

//root page can be accessed only if authentification is ok
void handleRoot(){
  mySerial.println("Enter handleRoot");
  String header;
  if (!is_authentified()){
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.send(301);
    return;
  }
  String content =  "<body style='background: #80c6f7'><h1 align ='center'><b><u><i><strong>FARM AUTOMATION SYSTEM</strong></i></u></b></h1><br><p align ='center'>Seeding <a href=\"switch1On\"><button>ON</button></a></p>";
  content += "<br><p  align ='center'>Watering <a href=\"switch2On\"><button>ON</button></a></p>";
  //content += "<br><p  align ='center'>Fertilizer <a href=\"switch3On\"><button>ON</button></a>&nbsp;<a href=\"switch3Off\"><button>OFF</button></a></p>";
  content += "<br><p  align ='center'>PLOT 1 <a href=\"switch6On\"><button>MOVE</button></a></p>";
  content += "<p  align ='center'>PLOT 2 <a href=\"switch7On\"><button>MOVE</button></a></p>";
  content += "<p  align ='center'>PLOT 3 <a href=\"switch8On\"><button>MOVE</button></a></p>";
  content += "<p  align ='center'>PLOT 4 <a href=\"switch9On\"><button>MOVE</button></a></p>";
  content += "<p  align ='center'>PLOT 5 <a href=\"switch10On\"><button>MOVE</button></a></p>";
  content += "<p  align ='center'>PLOT 6 <a href=\"switch11On\"><button>MOVE</button></a></p>";
  content += "<br><p  align ='center'>Seed a plant<a href=\"switch4On\"><button>ON</button></a>&nbsp;</p><br></br>";
  content += "<br><p  align ='center'>Watering for Single Plant<a href=\"switch5On\"><button>ON</button></a>&nbsp;<a href=\"switch5Off\"><button>OFF</button></a></p>";
 
 
 
  content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
  server.send(200, "text/html", content);
}

//no need authentification
void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}



void setup(void){


   // preparing GPIOs
  pinMode(gpio1_pin, OUTPUT);
  digitalWrite(gpio1_pin, LOW);
 // pinMode(gpio2_pin, OUTPUT);
  //digitalWrite(gpio2_pin, LOW);
  //pinMode(gpio3_pin, OUTPUT);
  //digitalWrite(gpio3_pin, LOW);
  //pinMode(gpio4_pin, OUTPUT);
  //digitalWrite(gpio4_pin, LOW);
  //pinMode(gpio5_pin, OUTPUT);
  //digitalWrite(gpio5_pin, LOW);
 
 
  delay(1000);
  mySerial.begin(115200);
  WiFi.begin(ssid, password);
  mySerial.println("");

  mySerial.begin(115200);
  WiFi.begin(ssid, password);
  mySerial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    mySerial.print(".");
  }
  mySerial.println("");
  mySerial.print("Connected to ");
  mySerial.println(ssid);
  mySerial.print("IP address: ");
  mySerial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    mySerial.println("MDNS responder started");
  }


  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works without need of authentification");
  });

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );


  //server.on("/",[](){
   //});
 
  server.on("/switch1On", [](){
   //
    mySerial.write("1");
    digitalWrite(gpio1_pin, HIGH);
    delay(1000);
    mySerial.write("-");
  });
  /*server.on("/switch1Off", [](){
 // 
     mySerial.write("2");
    digitalWrite(gpio1_pin, LOW);
    delay(1000);
    mySerial.write("-");
  });*/
  server.on("/switch2On", [](){
  //
     mySerial.write("3");
    //digitalWrite(gpio2_pin, HIGH);
    //delay(1000);
    mySerial.write("-");
  });
  /*server.on("/switch2Off", [](){
    //
     mySerial.write("4");
    //digitalWrite(gpio2_pin, LOW);
    //delay(1000);
    mySerial.write("-");
  });*/
  /*server.on("/switch3On", [](){
     mySerial.write("5");
    //digitalWrite(gpio3_pin, HIGH);
    //delay(1000);
    mySerial.write("-");
  });
  server.on("/switch3Off", [](){
     mySerial.write("6");
    //digitalWrite(gpio3_pin, LOW);
    //delay(1000);
    mySerial.write("-");
  });*/
server.on("/switch4On", [](){
     mySerial.write("7");
    //digitalWrite(gpio4_pin, HIGH);
    //delay(1000);
    mySerial.write("-");
  });
  /*server.on("/switch4Off", [](){
     mySerial.write("8");
    //digitalWrite(gpio4_pin, LOW);
    //delay(1000);
    mySerial.write("-");
  });*/
  /*server.on("/switch5On", [](){
     mySerial.write("a");
    //digitalWrite(gpio5_pin, HIGH);
    //delay(1000);
    mySerial.write("-");
  });
  server.on("/switch5Off", [](){
     mySerial.write("b");
    //digitalWrite(gpio5_pin, LOW);
    //delay(1000);
    mySerial.write("-");
  });*/

  server.on("/switch6On", [](){
   //
    mySerial.write("c");
    digitalWrite(gpio1_pin, HIGH);
    delay(1000);
    digitalWrite(gpio1_pin, LOW);
    mySerial.write("-");
  });
server.on("/switch7On", [](){
   //
    mySerial.write("d");
    digitalWrite(gpio1_pin, HIGH);
    delay(1000);
    digitalWrite(gpio1_pin, LOW);
    mySerial.write("-");
  });
server.on("/switch8On", [](){
   //
    mySerial.write("f");
    digitalWrite(gpio1_pin, HIGH);
    delay(1000);
    digitalWrite(gpio1_pin, LOW);
    mySerial.write("-");
  });

  server.on("/switch9On", [](){
   //
    mySerial.write("h");
    digitalWrite(gpio1_pin, HIGH);
    delay(1000);
    digitalWrite(gpio1_pin, LOW);
    mySerial.write("-");
  });

  server.on("/switch10On", [](){
   //
    mySerial.write("j");
    digitalWrite(gpio1_pin, HIGH);
    delay(1000);
    digitalWrite(gpio1_pin, LOW);
    mySerial.write("-");
  });
server.on("/switch11On", [](){
   //
    mySerial.write("l");
    digitalWrite(gpio1_pin, HIGH);
    delay(1000);
    digitalWrite(gpio1_pin, LOW);
    mySerial.write("-");
  });
 
  server.begin();
  mySerial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}


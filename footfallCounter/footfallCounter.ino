#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

const int chipSelect = 4;

#define trigPin 3
#define echoPin 2
#define trigPin2 6
#define echoPin2 5

int duration, distance, duration2, distance2, whichTripped;
int u1, u2, goingIN, goingOUT;  
int WhichSensorIsSet, JustHadACountOn;
int isTimeToSD = 0;
String records = "0";
String dataString = "";

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "api.smartbot.in";    // name address for Google (using DNS)
IPAddress ip(192,168,0,177);

EthernetClient client;

void setup () {
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
  
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  
  u1 = u2 = 0;
  WhichSensorIsSet = 0;    // this stored the name of the sensor that has been tripped first (of the two)
  
  /*used to avoid case where person has crossed from u1 to u2, got counted, 
  but still has not left u2 and so u2 is still read as tripped.*/
  JustHadACountOn = 0;
}




void loop () {

  ping();

  if(u1 == 1){                                 //u1 has tripped
    if(WhichSensorIsSet == 2){            //u2 was already tripped. so someone came in from u2 through u1
      goingIN++; 
      WhichSensorIsSet = 0;               // reset: we have a count!
      JustHadACountOn = 1;
      delay(2000);
    } else {
      WhichSensorIsSet = 1;               // store 1 because u1 sensor is the first to trip
      JustHadACountOn = 0;
    }
  }

  if(u2 == 1){                                  //u1 has tripped
    if(WhichSensorIsSet == 1){             //u2 was already tripped. so someone came in from u2 through u1
      goingOUT++; 
      WhichSensorIsSet = 0;                // reset: we have a count !
      JustHadACountOn = 2;
      delay(2000);
    } else {
      WhichSensorIsSet = 2;                //store 2 because u2 was the first to trip
      JustHadACountOn = 0;
    }
  }

  Serial.print("WhichSensorIsSet:");
  Serial.println(WhichSensorIsSet);

//  Serial.print("JustHadACountOn:");
//  Serial.println(JustHadACountOn);

  Serial.print("u1:");
  Serial.println(u1);
  Serial.print("u2:");
  Serial.println(u2);

  Serial.print("goingIN:");
  Serial.println(goingIN);
  
  Serial.print("goingOUT:");
  Serial.println(goingOUT);

  Serial.println("--------------------");

  /**** Store to SD only after 20 or so loops. Ideally, every 60 mins. ********/
  if (isTimeToSD == 20 ){
     fwriteToSD();
  /*** send to server***/
    sendToWeb();
    isTimeToSD = 0;
  } 
  else {
    isTimeToSD++;
  }

 
 
  delay(1000);
}


int fwriteToSD(){
   
//  dataString = "'data':{'Date': '" + getDate() + "', 'Time': '" + getTime() + "', 'TotalIN':'" + goingIN + "', 'TotalOUT': '" + goingOUT + "'},";
  dataString = "testing";
 
 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("footie/datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening footie/datalog.txt");
  } 
 
}


int ping() {
  u1 = u2 = 0;  //reset sensors. This ensures that only the tripped sensor(s) are set in each loop

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);


  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;

  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin2, LOW);

  
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = (duration2/2) / 29.1;

  
//  Serial.print("Sensor 1 distance: ");
//  Serial.print(distance);
//  Serial.println(" cms");

//  Serial.print("Sensor 2 distance: ");
//  Serial.print(distance2);
//  Serial.println(" cms");

  if(distance < 50) {
//    if(JustHadACountOn == 0){
      u1 = 1;
//    }
    
  }

  if(distance2 < 50) {
//    if(JustHadACountOn == 0){
      u2 = 1;    
//    }
  }

}

String getDate(){
  return "2014-11-02";
}

String getTime(){
  return "20:00";
}


int sendToWeb() {
 // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected");
 
    // Make a HTTP request:
    client.println("GET /footie/?TotalIN=1900&TotalOUT=900 HTTP/1.1");
/*    client.print("GET /?TotalIN=");
    client.print(goingIN);
    client.print("&TotalOUT=");
    client.println(goingOUT);
*/
    client.println("Host: api.smartbot.in");
    client.println("Connection: close");
    client.println();
  } 
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }

  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    //while(true);
  }
 
}

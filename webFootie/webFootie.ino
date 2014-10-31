#include <SPI.h>
#include <Ethernet.h>

#define trigPin 3
#define echoPin 2

#define trigPin2 5
#define echoPin2 4


byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,177);


EthernetServer server(80);



  int duration, distance, duration2, distance2, whichTripped;
  int u1, u2, goingIN, goingOUT;  
  int WhichSensorIsSet, JustHadACountOn;
  


void setup () {
  Serial.begin(9600);
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP()); 
  
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);


  u1 = u2 = 0;
  WhichSensorIsSet = 0;    // this stored the name of the sensor that has been tripped first (of the two)
  
  /*used to avoid case where person has crossed from u1 to u2, got counted, 
  but still has not left u2 and so u2 is still read as tripped.*/
  JustHadACountOn = 0;
}


void loop () {
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
	  client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");

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

  
  client.print("Sensor 1 distance: ");
  client.print(distance);
  client.println(" cms");
  client.println("<br />");
  client.print("Sensor 2 distance: ");
  client.print(distance2);
  client.println(" cms");
  client.println("<br />");

/************************************************************************************************
The following logic sets one direction count only. 
The direction is the direction of first person to enter 
This has been commented because now, we have a 2s delay when we have a person count.
This delay is to ensure that person is stil not standing in the way and has cleared out.
If not, it will be considered as next entry.
*************************************************************************************************/
  if(distance < 50) {
    if(JustHadACountOn != 1){
      /*ONLY trigger if the person is NOT still standing on u1*/
      u1 = 1;
    }
  }

  if(distance2 < 50) {
    if(JustHadACountOn != 2){
      /*ONLY trigger if the person is NOT still standing on u2*/
      u2 = 1;    
    }
  }
/************************** end of the block comment ***********************************************/  

  if(distance < 50) {
      u1 = 1;
  }
  if(distance2 < 50) {
      u2 = 1;    
  }  


  if(u1){                                 //u1 has tripped
    if(WhichSensorIsSet == 2){            //u2 was already tripped. so someone came in from u2 through u1
      goingIN++; 
      WhichSensorIsSet = 0;               // reset: we have a count!
      delayMicroseconds(2000);            //give person some time to clear out
      JustHadACountOn = 1;                // set to 1 because person is standing on u1
    } else {
      WhichSensorIsSet = 1;               // store 1 because u1 sensor is the first to trip
      JustHadACountOn = 0;
    }
  }

  if(u2){                                  //u1 has tripped
    if(WhichSensorIsSet == 1){             //u2 was already tripped. so someone came in from u2 through u1
      goingOUT++; 
      WhichSensorIsSet = 0;                // reset: we have a count !
      delayMicroseconds(2000);            //give person some time to clear out
      JustHadACountOn = 2;                 // set to 2 because person is standing on u2
    } else {
      WhichSensorIsSet = 2;                //store 2 because u2 was the first to trip
      JustHadACountOn = 0;
    }
  }



  // listen for incoming clients
          client.print("WhichSensorIsSet:");
          client.println(WhichSensorIsSet);
  client.println("<br />");
          client.print("JustHadACountOn:");
          client.println(JustHadACountOn);
  client.println("<br />");
          client.print("u1:");
          client.println(u1);
  client.println("<br />");
          client.print("u2:");
          client.println(u2);
  client.println("<br />");
          client.print("goingIN:");
          client.println(goingIN);
  client.println("<br />");
          client.print("goingOUT:");
          client.println(goingOUT);
  client.println("<br />");

          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    //delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}



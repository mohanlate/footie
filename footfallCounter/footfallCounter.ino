#define trigPin 3
#define echoPin 2

#define trigPin2 5
#define echoPin2 4

#define ledPin 8

  int duration, distance, duration2, distance2, whichTripped;
  int u1, u2, goingIN, goingOUT;  
  int WhichSensorIsSet, JustHadACountOn;
  


void setup () {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(ledPin, OUTPUT);

  u1 = u2 = 0;
  WhichSensorIsSet = 0;
  JustHadACountOn = 0;    //used to avoid case where person has crossed from u1 to u2, got counted, but still has not left u2 and so u2 is still read as tripped.
}


void loop () {

/*************
 If u1 has tripped {
   and firstToTrip was u2, {
    then we have goingIN++, firstToTrip=""
   } else {
     firstToTrip = "u1"
   }
 }  
  
 if u2 has tripped {
   if firstToTrip="u1" {
     goingOUT++;
     firstToTrip = "";
   } else {
     firstToTrip = "u2";
   }
 
 }  
**************/

  ping();
  if(u1){                                  //u1 has tripped
    if(WhichSensorIsSet == 2){               //u2 was already tripped. so someone came in from u2 through u1
      goingIN++; 
      WhichSensorIsSet = 0;
      JustHadACountOn = 1;
    } else {
      WhichSensorIsSet = 1;
      JustHadACountOn = 0;
    }
  }

  if(u2){                                  //u1 has tripped
    if(WhichSensorIsSet == 1){               //u2 was already tripped. so someone came in from u2 through u1
      goingOUT++; 
      WhichSensorIsSet = 0;
      JustHadACountOn = 2;
    } else {
      WhichSensorIsSet = 2;
      JustHadACountOn = 0;
    }
  }

  Serial.print("WhichSensorIsSet:");
  Serial.println(WhichSensorIsSet);

  Serial.print("JustHadACountOn:");
  Serial.println(JustHadACountOn);

  Serial.print("u1:");
  Serial.println(u1);
  Serial.print("u2:");
  Serial.println(u2);

  Serial.print("goingIN:");
  Serial.println(goingIN);
  
  Serial.print("goingOUT:");
  Serial.println(goingOUT);

  Serial.println("--------------------");
  delay(1000);
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

  
  Serial.print("Sensor 1 distance: ");
  Serial.print(distance);
  Serial.println(" cms");

  Serial.print("Sensor 2 distance: ");
  Serial.print(distance2);
  Serial.println(" cms");

  if(distance < 50) {
    if(JustHadACountOn == 0){
      u1 = 1;
    }
    
  }

  if(distance2 < 50) {
    if(JustHadACountOn == 0){
      u2 = 1;    
    }
  }

}

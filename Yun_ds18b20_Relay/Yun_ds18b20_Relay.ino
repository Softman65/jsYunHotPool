/*
Console Read example for YunShield/Yún

 Read data coming from bridge using the Console.read() function
 and store it in a string.

 To see the Console, pick your Yún's name and IP address in the Port menu
 then open the Port Monitor. You can also see it by opening a terminal window
 and typing:
 ssh root@ yourYunsName.local 'telnet localhost 6571'
 then pressing enter. When prompted for the password, enter it.

 created 13 Jun 2013
 by Angelo Scialabba
 modified 16 June 2013
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/ConsoleRead

 */

#include <Console.h>
#include <OneWire.h>
#include <DallasTemperature.h>
const int oneWirePin = 2;
const int GrifoPin = 4;

float outMax = 36;
float difMax= 5;


float ambient = 0; //sensor.getTempCByIndex(0);
float h2o_in = 0; //sensor.getTempCByIndex(0);
float h2o_out = 0; //sensor.getTempCByIndex(1);
int timedelay =  0; //(h2o_out-h2o_in);

boolean alarm = false;
boolean statusGrifo = false;

//OneWire ourWire(2);                //Se establece el pin 2  como bus OneWire
OneWire oneWireBus(oneWirePin);
DallasTemperature sensor(&oneWireBus);


void setup() {
  // Initialize Console and wait for port to open:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(GrifoPin, OUTPUT);
  
  Bridge.begin();
  Console.begin();
  
  Process nodejs;
  nodejs.runShellCommandAsynchronously("node /mnt/sda1/arduino/node/server.js");
  
  // Wait for Console port to connect
  while (!Console);
  
  Console.println("Obteniendo direcciones:");
  oneWireBus.reset_search();
  byte addr[8];
   
  while (oneWireBus.search(addr)) {  
    Console.print("Address = ");
    for( int i = 0; i < 8; i++) {
      Console.print(" 0x");
      Console.print(addr[i], HEX);
    }
    Console.println();
  }
  sensor.begin();
  Console.println("Listo para leer temperaturas");

  
}

void loop() {
  Console.print(":");

    
    //int timedelay = 1000;
    sensor.requestTemperatures();
    
   ambient = sensor.getTempCByIndex(0);
   h2o_in = sensor.getTempCByIndex(0);
   h2o_out = sensor.getTempCByIndex(1);
   
   timedelay =  (h2o_out-h2o_in);
    
    nodejs.write("Ambiente: " + String(ambient,2) );
    Console.print(" IN:" + String(h2o_in,2) + " OUT:"+ String(h2o_out,2)+ " DIF;" + String(timedelay) );
    Console.print(" STATUS:" + String(statusGrifo ? "ON":"PARADO") ) ;

        
    if(ambient>10 && (h2o_out>=outMax || h2o_out-h2o_in > difMax)){
      alarm =  true;
    }else{
      alarm = false;
    }
    Console.println(" ALARM:" + String(alarm? "SI":"NO")) ;
    
    if(alarm){
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      //delay(timedelay* 100);                       // wait for a second
      //digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      
      if(!statusGrifo){
        statusGrifo = true;
        digitalWrite(GrifoPin, HIGH);
      }
      
    }else{
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(100);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      
       if(statusGrifo){
          statusGrifo = false;
          digitalWrite(GrifoPin, LOW);
       }     
    }
    delay(1000);  
}

/*
 * RF24Ethernet Simple Server Example, using RF24Mesh for address allocation
 * RF24toTUN must be built using 'make MESH=1' option
 *
 * RF24Ethernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 * This example demonstrates how to send out an HTTP response to a browser
 *
 */


#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
//#include <printf.h>
#include <RF24Ethernet.h>
#include "RF24Mesh.h"
#include "EEPROM.h"

/** Configure the radio CE & CS pins **/
RF24 radio(7,8);
RF24Network network(radio);
RF24EthernetClass RF24Ethernet(radio,network);
RF24Mesh mesh(radio,network);

// Set up the server to listen on port 1000
EthernetServer server = EthernetServer(1000);

void setup() {
  // Set up the speed of our serial link.
  Serial.begin(115200);
  //printf_begin();
  Serial.println(F("start"));

  // This initializes the radio with basic settings.
  // Needs to be called at the beginning of every sketch
  Ethernet.use_device();  
  
  // Tell RF24Ethernet which channel to use. This step is not that important, but it 
  // is important to set the channel this way, not directly via the radio
  //Ethernet.setChannel(97);
  //mesh.setChannel(97);
  
  // This step is very important. The address of the node needs to be set both
  // on the radio and in the UIP layer
  // This is the RF24Network address and needs to be configured accordingly if
  // using more than 4 nodes with the master node. Otherwise, 01-04 can be used.

  mesh.setNodeID(1); //Node id must be any unique value between 1 and 250
  mesh.begin();
  //Serial.println(mesh.mesh_address,HEX);
  Ethernet.setMac(mesh.mesh_address);
  
  //Optional
  radio.setPALevel(RF24_PA_LOW);
  radio.printDetails();
  
  // Set the IP address we'll be using.  Make sure this doesn't conflict with
  // any IP addresses or subnets on your LAN or you won't be able to connect to
  // either the Arduino or your LAN...
  IPAddress myIP(10,10,2,4);
  Ethernet.begin(myIP);
  
  // If you'll be making outgoing connections from the Arduino to the rest of
  // the world, you'll need a gateway set up.
  IPAddress gwIP(10,10,2,2);
  Ethernet.set_gateway(gwIP);

  // Listen for incoming connections on TCP port 1000.  Each incoming
  // connection will result in the uip_callback() function being called.
  server.begin();
}

uint16_t testTimer = 0;
uint32_t mesh_timer = 0;

void loop() {

  // Optional: If the node needs to move around physically, or using failover nodes etc.,
  // enable address renewal
  if(millis()-mesh_timer > 30000){ //Every 30 seconds, test mesh connectivity
    mesh_timer = millis();
    if( ! mesh.checkConnection() ){
        //refresh the network address        
        Ethernet.setMac(mesh.renewAddress());
     }
  }
  
  size_t size;

  if(EthernetClient client = server.available())  
  {

      while((size = client.available()) > 0)
      {                 
        // Read any incoming data from the client
        Serial.print((char)client.read());
        Serial.println("");
       }
        // Send an HTML response to the client.
	client.write( "HTTP/1.1 200 OK\n");
        client.write( "Content-Type: text/html\n");
        client.write( "Connection: close\n");
        client.write( "Refresh: 5\n");
        client.write( "\n");
        client.write( "<!DOCTYPE HTML>\n");
        client.write( "<html>\n");
        client.write( "HELLO FROM ARDUINO!\n");
        client.write( "</html>\n");
	   
       client.stop(); 
       Serial.println(F("********"));       
    }
 
  // We can do other things in the loop, but be aware that the loop will
  // briefly pause while IP data is being processed.
}

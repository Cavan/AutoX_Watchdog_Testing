#include <SoftwareSerial.h>
// include the SD library:
#include <SPI.h>
#include <SD.h>


//Create software serial object to communicate with SIM900
//Settings for Mega 2560
SoftwareSerial mySerial(11, 12); //SIM900 Tx & Rx is connected to Arduino #11 & #12
//Settings for UNO
//SoftwareSerial mySerial(7, 8); //UNO Pins for Tx & Rx #7 & #8

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

File dataFile;
int i;
unsigned char data=0;

const int SDchipSelect = 7;

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  //Begin serial communication with Arduino and SIM900
  mySerial.begin(19200);

  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT"); //Handshaking with SIM900
  updateSerial();
  
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
  //Configure MMS Settings to network provider
  mmsConfiguration();
  //SD initialization
  if (!SD.begin(SDchipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

//File handling source code
 

    //Check SD Module
    if (checkSDModule()){
      //Read image bytes
       //readImageBytes();
       Serial.println("System startup successful");
       //sendMMS();
      //Get the information about the sd card.
      //SD_Info();
  }else{
    Serial.println("Problem was detected with SD module");
  }
}
void loop()
{
  

  if(mySerial.available())
  {
    while(mySerial.available())
    {
      data=mySerial.read();
      Serial.write(data);
    }
    data=0;
  }

  if(Serial.available())
  {
    data=Serial.read();
  }  
  //Check incoming input
  if(data=='s')
  {
    Serial.println("Preparing to send MMS");
    sendMMS();
  }
  if (data=='R'){
    //Terminate MMS mode
    mySerial.print("AT+CMMSTERM");
    updateSerial();
  }
  
  data=0;
  
}

void updateSerial()
{
  
  delay(1000);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}

void commandHandler()
{
  Serial.println("Enter a command");
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}


void updateSerial2(){
//  if(Serial2.available()){
//    while (Serial2.available()){
//      data=Serial2.read();
//      Serial.write(data);
//    }
//    data=0;
//  }
}


//Set MMS configuration for the Rogers network
void mmsConfiguration(){
  
  //mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  mySerial.println("AT+CMMSINIT"); // Initialize MMS mode
  updateSerial();
  mySerial.println("AT+CMMSCURL=\"mms.gprs.rogers.com\""); //Set MMS centre
  updateSerial();
  mySerial.println("AT+CMMSCID=1"); //Set bearer context id
  updateSerial();
  mySerial.println("AT+CMMSPROTO=\"10.128.1.69\",80"); //Set the MMS Proxy and Port
  updateSerial();
  mySerial.println("AT+CMMSSENDCFG=6,3,0,0,2,4"); //Parameters for MMS PDU
  updateSerial();
  mySerial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\""); //Set bearer parameter
  updateSerial();
  mySerial.println("AT+SAPBR=3,1,\"APN\",\"mms.gprs.rogers.com\"");
  updateSerial();
  mySerial.println("AT+SAPBR=1,1"); //Active bearer context NOTE: Sometimes will throw an error because a connection is ...
  //already established, I was able to rectify the issue by reseting the GPRS module.
  updateSerial();
  mySerial.println("AT+SAPBR=2,1"); 
  updateSerial();
  
}
//When switching between Text Mode and MMS mode be sure to exit out of MMS...
// mode using AT+CMMSTERM, other wise when you send the command 'AT+CMMSINIT" you will get an error.

void sendMMS(){

  mySerial.println("AT+CMMSEDIT=1"); //Enter edit mode
  updateSerial();
  mySerial.println("AT+CMMSDOWN=\"PIC\",52097,60000,\"237.JPG\""); //Download image and get size
  updateSerial();
  
//  mySerial.println("AT+CMMSDOWN=\"TITLE\",3,5000"); //Download MMS title
//  updateSerial();
//  mySerial.println("AT+CMMSDOWN=\"TEXT\",5,5000"); //Download text, with size of x bytes and wait x milliseconds
//  updateSerial();
  // If the data is in Unicode (big endian) format then prepend the data with 'FE FF'.
  // If the data is in Unitcode (little endian) format the prepend the data with 'FF FE'. 
  //Example: "00 31 00 32 00 33 00 34" Big Endian format = "FE FF 00 31 00 32 00 33 00 34"
  //Open the file to be sent via MMS
  dataFile=SD.open("/237.JPG");
  i=0;
  if(dataFile)
  {
    while(dataFile.available())
    {
      data=dataFile.read();
      //if(data<0x10) Serial.print("0");
      //Serial.print(data,HEX);
      //i++;
      //if((i%40)==0) Serial.println();
      mySerial.write(data);
    }
    dataFile.close();
  }
  else
  {
    Serial.println("error opening 237.JPG");
  }

  delay(1000);
  mySerial.println("AT+CMMSRECP=\"+15196088364\""); //Define the recepient of the MMS
  updateSerial();
  mySerial.println("AT+CMMSVIEW"); //Show the message to be sent
  updateSerial();
  mySerial.println("AT+CMMSSEND"); //Send MMS to the registered recipient.
  updateSerial();
  mySerial.println("AT+CMMSEDIT=0"); //Exit MMS edit mode and clear the buffer.
  updateSerial();
  mySerial.println("AT+CMMSTERM"); //Exit MMS Mode
  updateSerial();
  
  
  
//  File* sdImage;
//  //Call the method to retrieve the image
//  readImageBytes();
//  //Print the size of the image pointer
//  Serial.print("Size of image pointer: ");
//  //Serial.println(sdImage.size());

  //Possibly send commands to go back into text mode to handle incoming commands from the user.
}



//Initialize SD Module
bool checkSDModule(){
  bool SD_OK = false;
     // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, SDchipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1);
  } else {
    Serial.println("Wiring is correct and a card is present.");
    SD_OK = true;
  }
  return SD_OK;
}

void SD_Info(){

  // print the type of card
  Serial.println();
  Serial.print("Card type:         ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1);
  }

  Serial.print("Clusters:          ");
  Serial.println(volume.clusterCount());
  Serial.print("Blocks x Cluster:  ");
  Serial.println(volume.blocksPerCluster());

  Serial.print("Total Blocks:      ");
  Serial.println(volume.blocksPerCluster() * volume.clusterCount());
  Serial.println();

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("Volume type is:    FAT");
  Serial.println(volume.fatType(), DEC);

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
  Serial.print("Volume size (Kb):  ");
  Serial.println(volumesize);
  Serial.print("Volume size (Mb):  ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Gb):  ");
  Serial.println((float)volumesize / 1024.0);

  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
}


//AT+CMMSDOWN="PIC",12963,20000

File readImageBytes(){

  File retImage;
  
  if (!SD.begin(SDchipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  //237.JPG
  File imageMMS;
  imageMMS = SD.open("/237.JPG");
  if(imageMMS){
    Serial.println("Reading image file: ");
    // read from file until all the contents have been read
    while (imageMMS.available()){
     // Serial.write(imageMMS.read());
      imageMMS.read();
    }
    //Print the size of the file
    Serial.println(imageMMS.size());
    //close the file
    retImage = imageMMS;
    imageMMS.close();
    Serial.println("Done reading file");
  }else {
    //could not open file 
    Serial.println("Error opening file");
  }
  return retImage;
}


  

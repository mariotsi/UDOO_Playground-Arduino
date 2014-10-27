
#include <adk.h>
#define BUFFSIZE 128
#define LED_RED 13
#define LED_GREEN 12
#define BUTTON_DEBUG 2
#define BUTTON_VERBOSE 3

bool blink = false;
bool blink_toggle = false;
bool debug_mode = false;
long lastPush;
int reading;

int previousDebug = LOW;
bool verboseMode = false;
int previousVerbose = LOW;
int velocity;
int bounce = 200;
char accessoryName[] = "LED lamp";
char manufacturer[] = "Mariotti, Inc.";
char model[] = "LedLamp";
char versionNumber[] = "0.1.0";
char serialNumber[] = "1";
char url[] = "http://www.mariotti.me";
uint8_t buffer[BUFFSIZE];
uint32_t bytesRead = 0;
USBHost Usb;
String str;
ADK adk(&Usb, manufacturer, model, accessoryName, versionNumber,
	url, serialNumber);
void setup() {
	lastPush = millis();
	pinMode(LED_RED, OUTPUT);
	digitalWrite(LED_RED, LOW);
	pinMode(LED_GREEN, OUTPUT);
	digitalWrite(LED_GREEN, LOW);
	pinMode(BUTTON_DEBUG, INPUT);
	pinMode(BUTTON_VERBOSE, INPUT);
	Serial.begin(115200);
	blink=false;
	velocity=2000;
	Serial.println("SETUP DONE: Arduino is running...");
}

void executor(uint32_t command) {
	if (verboseMode) {
		Serial.print("DECODED COMMAND CODE: "); Serial.println(command);
	}


	switch (command) {
		case 0:
		digitalWrite(LED_RED, LOW);
		break;
		case 1:
		digitalWrite(LED_RED, HIGH);
		break;
		case 2:
		digitalWrite(LED_GREEN, LOW);
		break;
		case 3:
		digitalWrite(LED_GREEN, HIGH);
		break;
		case 4:
		blink = false;
		digitalWrite(LED_GREEN, LOW);
		digitalWrite(LED_RED, LOW);
		break;
		case 5:
		blink = true;
		break;
		case 6:
		int vel;
		if (adk.isReady()) {
			adk.read(&bytesRead, BUFFSIZE, buffer);
			if (bytesRead > 0) {
				vel=0;
				//for (int i = bytesRead-1; i>=0 ; i--) {
				for (int i = 0; i<bytesRead ; i++) {	
						if (false) {
							Serial.print("prima");
							Serial.print(buffer[i]);     
						}       
						vel+= (buffer[i]-48)*pow(10,bytesRead-1-i);
						buffer[i]=0;
						if (false) {
							Serial.print(" VelTemp");
							Serial.println(vel);
						}
					}
		        //velocity=buffer[0]<<8+buffer[1];
		        //velocity = *(int*)buffer;
		        if (verboseMode) {
		        	Serial.print("RECEIVED ");Serial.print(bytesRead);Serial.println(" bytes");
		        	Serial.print("SET VELOCITY': ");Serial.println(vel);
		        }
		    }

		}
		velocity=vel; 		
		break;

      //str = String(velocity); //converting integer into a string
      //char b[32];
      //str.toCharArray(b, 32); //passing the value of the string to the character array
      //writeToAdk(b);



      default:
      // noop
      break;
  }
}
void writeToAdk(char textToSend[]) {
	adk.write(sizeof(textToSend), (uint8_t*)textToSend);
}

void loop() {
	reading = digitalRead(BUTTON_DEBUG);
	if (reading == HIGH && previousDebug == LOW && millis() - lastPush > bounce) {
		if (verboseMode)Serial.println("START TEST");
		digitalWrite(LED_GREEN, HIGH);
		delay(500);
		digitalWrite(LED_GREEN, LOW);
		digitalWrite(LED_RED, HIGH);
		delay(500);
		digitalWrite(LED_RED, LOW);
		delay(500);
		digitalWrite(LED_RED, HIGH);
		digitalWrite(LED_GREEN, HIGH);
		delay(500);
		digitalWrite(LED_RED, LOW);
		digitalWrite(LED_GREEN, LOW);		
		if (verboseMode) Serial.println("END TEST");
		setup();
	}
	previousDebug = reading;

	reading = digitalRead(BUTTON_VERBOSE);	
	if (reading == HIGH && !verboseMode && previousVerbose == LOW && millis() - lastPush > bounce) {
		verboseMode = true;
		if (verboseMode) {
			Serial.println("VERBOSE MODE: ON");
		} 		
	}
	else if (reading == HIGH && verboseMode && previousVerbose == LOW && millis() - lastPush > bounce) {
		if (verboseMode) {
			Serial.println("VERBOSE MODE: OFF");
		}
		verboseMode = false;		
	}
	previousVerbose = reading;




	Usb.Task();
	if (adk.isReady()) {
		adk.read(&bytesRead, BUFFSIZE, buffer);
		if (bytesRead > 0) {
			delay(10);
      //adk.write(bytesRead, (char*)buffer);
      if (verboseMode) {
      	Serial.print("RECEIVED FROM USB: ");
      	Serial.println(*(uint32_t*)buffer);
      }
      executor((*(uint32_t*)buffer) - 48);
  }
}
if (blink) {
	digitalWrite(LED_RED, blink_toggle);
	digitalWrite(LED_GREEN, !blink_toggle);
	blink_toggle = !blink_toggle;
	delay(velocity);
}
}

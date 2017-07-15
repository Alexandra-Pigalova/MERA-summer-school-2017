
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <stdlib.h>  

using namespace std;


//MAX7219/MAX7221's memory register addresses:
// See Table 2 on page 7 in the Datasheet
const char NoOp        = 0x00;
const char Digit0      = 0x01;
const char Digit1      = 0x02;
const char Digit2      = 0x03;
const char Digit3      = 0x04;
const char Digit4      = 0x05;
const char Digit5      = 0x06;
const char Digit6      = 0x07;
const char Digit7      = 0x08;
const char DecodeMode  = 0x09;
const char Intensity   = 0x0A;
const char ScanLimit   = 0x0B;
const char ShutDown    = 0x0C;
const char DisplayTest = 0x0F;


const char numOfDevices = 4;

void setup();
void loop();
void firstStep();
void show();

class RasPiSPI
{
private:
	int SpiFd; // File descriptor of spi port
        
	char *TxBuffer;
	char *RxBuffer;
        
	int TxBufferIndex;
	int RxBufferIndex;
                
public:
	RasPiSPI(); // Konstruktor
	~RasPiSPI(); // Destruktor
        
	void begin() { begin(0, 1000000); } // default use channel 0 and 1MHz clock speed
	void begin(int, int);
        
	void transfer(char);
	void endTransfer();
};

RasPiSPI::RasPiSPI() // CONSTRUCTOR
{
	TxBuffer = new char[1024]; // Buffer for TxData
	RxBuffer = new char[1024]; // Buffer for RxData
    
	TxBufferIndex = 0;
	RxBufferIndex = 0;
}
RasPiSPI::~RasPiSPI() // DESTRUCTOR
{
    
	delete[] TxBuffer;
	delete[] RxBuffer;
    
	close(SpiFd); // Close SPI port
}

RasPiSPI SPI; // Create class SPI

void RasPiSPI::begin(int channel, int speed)
{
	if ((SpiFd = wiringPiSPISetup(channel, speed)) < 0)
	{    // Open port for reading and writing
		cout << "Failed to open SPI port " << channel << "! Please try with sudo" << endl;
	}
}

void RasPiSPI::transfer(char c)
{
	TxBuffer[TxBufferIndex] = c;
	TxBufferIndex++;
}
void RasPiSPI::endTransfer()
{
	int temp = write(SpiFd, TxBuffer, TxBufferIndex); // Write the data from TxBuffer to the SPI bus...
	TxBufferIndex = 0; // ...and reset the index
}


// Main

                             
int main(int argc, char **argv)
{
	int initOnly = 0;
    
	cout << argc << '\n'; 
	for (int i = 0; i < argc; i++) 
	{
        
		if (strcmp(argv[i], "-init") == 0)
		{
			initOnly = 1;
		}
	}

	    /*if (!joystick.isFound())
	    {
	        printf("open failed.\n");
	        exit(1);
	    }*/

	setup();
   
	firstStep();
	while (1) {
		loop();
	}
    

	return 0;
}



// Writes data to the selected device or does broadcast if device number is 255
void SetData(char adr, char data, char device)
{
    // Count from top to bottom because first data which is sent is for the last device in the chain
	for (int i = numOfDevices; i > 0; i--)
	{
		if ((i == device) || (device == 255))
		{
			SPI.transfer(adr);
			SPI.transfer(data);
		}
		else // if its not the selected device send the noop command
		{
			SPI.transfer(NoOp);
			SPI.transfer(0);
		}
	}
	SPI.endTransfer();

	delay(1);
}

// Writes the same data to all devices
void SetData(char adr, char data) { SetData(adr, data, 255); } // write to all devices (255 = Broadcast) 

void SetShutDown(char Mode) { SetData(ShutDown, !Mode); }
void SetScanLimit(char Digits) { SetData(ScanLimit, Digits); }
void SetIntensity(char intense) { SetData(Intensity, intense); }
void SetDecodeMode(char Mode) { SetData(DecodeMode, Mode); }


//Setup         

void setup()
{
    // The MAX7219 has officially no SPI / Microwire support like the MAX7221 but the
    // serial interface is more or less the same like a SPI connection

	SPI.begin();
  
    // Disable the decode mode because at the moment i dont use 7-Segment displays
	SetDecodeMode(false);
	// Set the number of digits; start to count at 0
	SetScanLimit(7);
	// Set the intensity between 0 and 15. Attention 0 is not off!
	SetIntensity(0);
	// Disable shutdown mode
	SetShutDown(false);

	    // Write some patterns
	SetData(Digit0, 0b10000000, 1);
	SetData(Digit1, 0b01000000, 1);
	SetData(Digit2, 0b00100000, 1);
	SetData(Digit3, 0b00010000, 1);
	SetData(Digit4, 0b00001000, 1);
	SetData(Digit5, 0b00000100, 1);
	SetData(Digit6, 0b00000010, 1);
	SetData(Digit7, 0b00000001, 1);

	SetData(Digit0, 0b00000001, 2);
	SetData(Digit1, 0b00000010, 2);
	SetData(Digit2, 0b00000100, 2);
	SetData(Digit3, 0b00001000, 2);
	SetData(Digit4, 0b00010000, 2);
	SetData(Digit5, 0b00100000, 2);
	SetData(Digit6, 0b01000000, 2);
	SetData(Digit7, 0b10000000, 2);

	SetData(Digit0, 0b10000000, 3);
	SetData(Digit1, 0b01000000, 3);
	SetData(Digit2, 0b00100000, 3);
	SetData(Digit3, 0b00010000, 3);
	SetData(Digit4, 0b00001000, 3);
	SetData(Digit5, 0b00000100, 3);
	SetData(Digit6, 0b00000010, 3);
	SetData(Digit7, 0b00000001, 3);

	SetData(Digit0, 0b10000000, 4);
	SetData(Digit1, 0b01000000, 4);
	SetData(Digit2, 0b00100000, 4);
	SetData(Digit3, 0b00010000, 4);
	SetData(Digit4, 0b00001000, 4);
	SetData(Digit5, 0b00000100, 4);
	SetData(Digit6, 0b00000010, 4);
	SetData(Digit7, 0b00000001, 4);

	delay(500);
}


//Loop                                                               


// simplified display where 1 is lit and 0 is not lit
// x,y
char display[16][16];

void clean() {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			display[i][j] = 0;
		}
	}
}

void firstStep()// First point
{
	for (int i = 1; i < 15; i++) {
		for (int j = 1; j < 15; j++) {
			display[i][j] = rand() % 2;
		}
	}
 
}

void show() {
    // we have 4 displays, this is row buffer for each cycle
	char row[4];
	// cycle through rows
	for (int rowCounter = 0; rowCounter < 8; ++rowCounter) {
		row[0] = 0;
		row[1] = 0;
		row[2] = 0;
		row[3] = 0;
		for (int i = 0; i < 8; ++i) {
			row[0] = row[0] << 1;
			row[0] |= display[i][rowCounter];
			row[1] = row[1] << 1;
			row[1] |= display[i + 8][rowCounter];
			row[2] = row[2] << 1;
			row[2] |= display[i + 8][rowCounter + 8];
			row[3] = row[3] << 1;
			row[3] |= display[i][rowCounter + 8];
		}
		SetData(rowCounter + 1, row[0], 1);
		SetData(rowCounter + 1, row[1], 2);
		SetData(rowCounter + 1, row[2], 3);
		SetData(rowCounter + 1, row[3], 4);
	}
}


// Àlgorithm 
void loop() {

	for (int i = 1; i < 15; i++) {
		for (int j = 1; j < 15; j++) {
			char neibg = -display[i][j];
			for (int k = i - 1; k <= i + 1; k++)
				for (int m = j - 1; m <= j + 1; m++)
					neibg += display[k][m];
			
			if (neibg == 3 || (neibg == 2 && display[i][j]))
				display[i][j] = 1;
			else display[i][j] = 0;

		}
	}
	 

    // Show results for some time
	show();

	delay(200);

}

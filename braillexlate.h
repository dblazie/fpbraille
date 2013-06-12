/* -*- c++ -*- */

/*
 This is the software to convert a string ascii_txt to braille_txt.
 It also sets the bits in braille_dots to ones or zeros for the three
 rows.

*/
#define DATA_PORT_NUMBER 13    // LED on RAMPS board.  Suggest we use aux-1 aout D1 & D2 port 
#define CLOCK_PORT_NUMBER 33   // Buzzer on RAMPS board. Suggest we use aux-1 aout D1 & D2 port 
#define PRINTER_BUSY   // We will use an input to check when the printer is busy or ready for clocking data in.
                       // Suggest we use one of the END_STOP pins
                       
	char ascii_txt[100];	// buffer passed in to hold ascii text
	char braille_txt[100];	// buffer to put translated text into
	char braille_dots14[200]; // rows 1 - 3 binary dot pattern
	char braille_dots25[200]; // rows 1 - 3 binary dot pattern
	char braille_dots36[200]; // rows 1 - 3 binary dot pattern
        int braille_in_process;  // flag for printing a file in Braille
        int length;      // lengh of string we are putting into ascii_txt
        void send_dots(unsigned char *buffer, unsigned int buffer_len);
        
unsigned int braille_alphabet[] = {
0b00000000 /*00*/, 0b00000000 /*01*/, 0b00000000 /*02*/, 0b00000000 /*03*/,
0b00000000 /*04*/, 0b00000000 /*05*/, 0b00000000 /*06*/, 0b00000000 /*07*/,
0b00000000 /*08*/, 0b00000000 /*09*/, 0b00000000 /*0A*/, 0b00000000 /*0B*/,
0b00000000 /*0C*/, 0b00000000 /*0D*/, 0b00000000 /*0E*/, 0b00000000 /*0F*/,
0b00000000 /*10*/, 0b00000000 /*11*/, 0b00000000 /*12*/, 0b00000000 /*13*/,
0b00000000 /*14*/, 0b00000000 /*15*/, 0b00000000 /*16*/, 0b00000000 /*17*/,
0b00000000 /*18*/, 0b00000000 /*19*/, 0b00000000 /*1A*/, 0b00000000 /*1B*/,
0b00000000 /*1C*/, 0b00000000 /*1D*/, 0b00000000 /*1E*/, 0b00000000 /*1F*/,
0b00000000 /*20*/,
0b00101110	/*21*/, 0b00010000	/*22*/, 0b00111100	/*23*/, 0b00101011	/*24*/,
0b00101001	/*25*/, 0b00101111	/*26*/, 0b00000100	/*27*/, 0b00110111	/*28*/,
0b00111110	/*29*/, 0b00100001	/*2a*/, 0b00101100	/*2b*/, 0b00100000	/*2c*/,
0b00100100	/*2d*/, 0b00101000	/*2e*/, 0b00001100	/*2f*/, 0b00110100	/*30*/,
0b00000010	/*31*/, 0b00000110	/*32*/, 0b00010010	/*33*/, 0b00110010	/*34*/,
0b00100010	/*35*/, 0b00010110	/*36*/, 0b00110110	/*37*/, 0b00100110	/*38*/,
0b00010100	/*39*/, 0b00110001	/*3a*/, 0b00110000	/*3b*/, 0b00100001	/*3c*/,
0b00111111	/*3d*/, 0b00101100	/*3e*/, 0b00111001	/*3f*/, 0b01001000	/*40*/,
0b01000001	/*41*/, 0b01000011	/*42*/, 0b01001001	/*43*/, 0b01011001	/*44*/,
0b01010001	/*45*/, 0b01001011	/*46*/, 0b01011011	/*47*/, 0b01010011	/*48*/,
0b01001010	/*49*/, 0b01011010	/*4a*/, 0b01000101	/*4b*/, 0b01000111	/*4c*/,
0b01001101	/*4d*/, 0b01011101	/*4e*/, 0b01010101	/*4f*/, 0b01001111	/*50*/,
0b01011111	/*51*/, 0b01010111	/*52*/, 0b01001110	/*53*/, 0b01011110	/*54*/,
0b01100101	/*55*/, 0b01100111	/*56*/, 0b01111010	/*57*/, 0b01101101	/*58*/,
0b01111101	/*59*/, 0b01110101	/*5a*/, 0b01101010	/*5b*/, 0b00110011	/*5c*/,
0b01111011	/*5d*/, 0b01011000	/*5e*/, 0b00111000	/*5f*/, 0b00001000	/*60*/,
0b00000001	/*61*/, 0b00000011	/*62*/, 0b00001001	/*63*/, 0b00011001	/*64*/,
0b00010001	/*65*/, 0b00001011	/*66*/, 0b00011011	/*67*/, 0b00010011	/*68*/,
0b00001010	/*69*/, 0b00011010	/*6a*/, 0b00000101	/*6b*/, 0b00000111	/*6c*/,
0b00001101	/*6d*/, 0b00011101	/*6e*/, 0b00010101	/*6f*/, 0b00001111	/*70*/,
0b00011111	/*71*/, 0b00010111	/*72*/, 0b00001110	/*73*/, 0b00011110	/*74*/,
0b00100101	/*75*/, 0b00100111	/*76*/, 0b00111010	/*77*/, 0b00101101	/*78*/,
0b00111101	/*79*/, 0b00110101	/*7a*/, 0b00101010	/*7b*/, 0b01110011	/*7c*/,
0b00111011	/*7d*/, 0b00011000	/*7e*/
};

 void set_data_high(void)
 {
     pinMode(DATA_PORT_NUMBER, OUTPUT);
     digitalWrite(DATA_PORT_NUMBER, 1);

 }
 void set_data_low(void)
 {
     pinMode(DATA_PORT_NUMBER, OUTPUT);
     digitalWrite(DATA_PORT_NUMBER, 0);

 }void set_clock_high(void)
 {
     pinMode(CLOCK_PORT_NUMBER, OUTPUT);
     digitalWrite(CLOCK_PORT_NUMBER, 0);

 }void set_clock_low(void)
 {
     pinMode(CLOCK_PORT_NUMBER, OUTPUT);
     digitalWrite(CLOCK_PORT_NUMBER, 1);
 }
void
convert_to_braille(unsigned char *buffer, unsigned int buffer_len)
{
	int i = 0;

	for (i = 0; i < buffer_len; i++) {
		if( 0x20 == buffer[i] ) /* Space character*/
		{
			buffer[i] = 0x80;    // this is cheating so that 0x0 does not terminate the string
		}
		else
		{
			buffer[i] = braille_alphabet[buffer[i]];     
		}
	}

	return;
}

void convert_to_dots(unsigned char *buffer, unsigned int buffer_len)
{
	int i = 0;
        int j = 0;
        char bchar;    // braille character we are working on
        int number_of_dots = 80;
        j = 0;
        	for (i = 0; i < buffer_len; i++) {
                    bchar = buffer[i];
                    if (bchar & 0b00000001) braille_dots14[j] = '1'; else braille_dots14[j] = '0'; // dot 1
                    if (bchar & 0b00001000) braille_dots14[j+1] = '1'; else braille_dots14[j+1] = '0'; // dot 4
                    if (bchar & 0b00000010) braille_dots25[j] = '1'; else braille_dots25[j] = '0'; // dot 2
                    if (bchar & 0b00010000) braille_dots25[j+1] = '1'; else braille_dots25[j+1] = '0'; // dot 5
                    if (bchar & 0b00000100) braille_dots36[j] = '1'; else braille_dots36[j] = '0'; // dot 3
                    if (bchar & 0b00100000) braille_dots36[j+1] = '1'; else braille_dots36[j+1] = '0'; // dot 6 
                    j += 2;                    
           	}         
         braille_dots14[j] = 0;        // terminate strings with 0  
         send_dots((unsigned char *)braille_dots14, number_of_dots);
         braille_dots25[j] = 0;        // terminate strings with 0  
         send_dots((unsigned char *)braille_dots25, number_of_dots);
         braille_dots36[j] = 0;        // terminate strings with 0  
         send_dots((unsigned char *)braille_dots36, number_of_dots);
	return;
}

void send_dots(unsigned char *buffer, unsigned int buffer_len)
{
 int i,j,clock_speed;
 i = j = 0;
 clock_speed = 2;   // delay in milliseconds  this must be shorter
 for (i = 0; i<buffer_len; i++)    // buffer_len should be either 80 (text) or 120 (graphics)
  {
      if (buffer[i] == '0') set_data_low(); else set_data_high();
      delay(clock_speed);
      set_clock_low();
      delay(clock_speed);
      set_clock_high();
      delay(clock_speed);
  }
 }
 

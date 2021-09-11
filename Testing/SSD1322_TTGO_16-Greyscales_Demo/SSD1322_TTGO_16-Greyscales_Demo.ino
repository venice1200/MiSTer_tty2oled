/* Arduino DUE pins
SCK = 13, SDA = 11, CS = 10, DC = 9, RST = 8
*/

#include "city.h"
#include "amiga.h"
#include "gng.h"

#define SPI_SCK 18
#define SPI_SDA 23
#define SPI_RST 27
#define SPI_DC 25
#define SPI_CS 26
#define SPI_SCK_0  digitalWrite(SPI_SCK,LOW)
#define SPI_SCK_1  digitalWrite(SPI_SCK,HIGH)
#define SPI_SDA_0  digitalWrite(SPI_SDA,LOW)
#define SPI_SDA_1  digitalWrite(SPI_SDA,HIGH)
#define SPI_RST_0  digitalWrite(SPI_RST,LOW)
#define SPI_RST_1  digitalWrite(SPI_RST,HIGH)
#define SPI_DC_0  digitalWrite(SPI_DC,LOW)
#define SPI_DC_1  digitalWrite(SPI_DC,HIGH)
#define SPI_CS_0  digitalWrite(SPI_CS,LOW)
#define SPI_CS_1  digitalWrite(SPI_CS,HIGH)

#define OLED_COLUMN_NUMBER 256
#define OLED_LINE_NUMBER 64
#define OLED_COLUMN_OFFSET 0

const unsigned char *point;

void SPI_SendByte(unsigned char byte)
{
  unsigned char counter;
  for (counter = 0; counter < 8; counter++)
  {
    SPI_SCK_0;
    if ((byte & 0x80) == 0)
    {
      SPI_SDA_0;
    }
    else SPI_SDA_1;
    byte = byte << 1;
    SPI_SCK_1;
    SPI_SCK_0;
  }
}

void OLED_send_cmd(unsigned char o_command)
{
  SPI_DC_0;
  SPI_CS_0;
  SPI_SendByte(o_command);
  SPI_CS_1;
  //SPI_DC_1;
}

void OLED_send_data(unsigned char o_data)
{
  SPI_DC_1;
  SPI_CS_0;
  SPI_SendByte(o_data);
  SPI_CS_1;
}

void OLED_clear()
{
  unsigned int ROW, column;
  OLED_send_cmd(0x15);
  OLED_send_data(0x1c);
  OLED_send_data(0x5b);
  OLED_send_cmd(0x75);
  OLED_send_data(0x00);
  OLED_send_data(0x7f);
  OLED_send_cmd(0x5C);
  for (ROW = 0; ROW < OLED_COLUMN_NUMBER; ROW++)      //ROW loop
  {
    for (column = 0; column < OLED_COLUMN_NUMBER; column++) //column loop
    {
      OLED_send_data(0x00);
    }
  }
}

void OLED_full()
{
  unsigned int ROW, column;
  OLED_send_cmd(0x15);
  OLED_send_data(0x1c);
  OLED_send_data(0x5b);

  OLED_send_cmd(0x75);
  OLED_send_data(0x00);
  OLED_send_data(0x7f);
  OLED_send_cmd(0x5C);
  for (ROW = 0; ROW < OLED_LINE_NUMBER; ROW++) //ROW loop
  {
    for (column = 0; column < OLED_COLUMN_NUMBER ; column++) //column loop
    {
      OLED_send_data(0xff);
    }
  }
}

void OLED_init()        //SSD1322
{
  OLED_send_cmd(0XFD);  //SET COMMAND LOCK
  OLED_send_data(0x12); //UNLOCK

  OLED_send_cmd(0xAE);  //turn off display

  OLED_send_cmd(0XB3);  //DISPLAY DIVIDE CLOCK RATIO/OSCILLATOR FREQUENCY
  OLED_send_data(0X91);

  OLED_send_cmd(0XCA);  //multiplex ratio
  OLED_send_data(0X3F); //duty = 1/64

  OLED_send_cmd(0xA2);  //set offset
  OLED_send_data(0x00);

  OLED_send_cmd(0XA1);  //start line
  OLED_send_data(0X00);

  OLED_send_cmd(0XA0);  //set remap
  OLED_send_data(0X14);
  OLED_send_data(0X11);

  OLED_send_cmd(0XAB);  //funtion selection
  OLED_send_data(0X01); //Enable internal VDD regulator

  OLED_send_cmd(0XB4);
  OLED_send_data(0XA0);
  OLED_send_data(0XFD);

  OLED_send_cmd(0XC1);  //set contrast current
  OLED_send_data(0X9F);

  OLED_send_cmd(0XC7);  //master contrast current control
  OLED_send_data(0X0F);

  OLED_send_cmd(0XB1);  //SET PHASE LENGTH
  OLED_send_data(0XE2);

  OLED_send_cmd(0XD1);
  OLED_send_data(0X82);
  OLED_send_data(0X20);

  OLED_send_cmd(0XBB);  //SET PRE-CHANGE VOLTAGE
  OLED_send_data(0X1F); //0.6*vcc

  OLED_send_cmd(0XB6);  //SET SECOND PRE-CHARGE PERIOD
  OLED_send_data(0X08);

  OLED_send_cmd(0XBE);  //SET VCOMH
  OLED_send_data(0X07); //0.86*vcc

  OLED_send_cmd(0XA6);  //normal display

  OLED_send_cmd(0xAF);  //turn on display
}

void Picture_display(const unsigned char *ptr_pic)
{
  unsigned int x, data = 0, loops = OLED_LINE_NUMBER*OLED_COLUMN_NUMBER/2;
  OLED_send_cmd(0x15);
  OLED_send_data(0x1c);
  OLED_send_data(0x5b);
  OLED_send_cmd(0x75);
  OLED_send_data(0x00);
  OLED_send_data(0x7f);
  OLED_send_cmd(0x5C);
  for (x = 0; x < loops; x++) //column loop
  {
    data = pgm_read_byte(ptr_pic++);
    OLED_send_data(data);
  }
}

void Picture_InverseDisplay(const unsigned char *ptr_pic)
{
  unsigned int x, data = 0, loops = OLED_LINE_NUMBER*OLED_COLUMN_NUMBER/2;
  OLED_send_cmd(0x15);
  OLED_send_data(0x1c);
  OLED_send_data(0x5b);

  OLED_send_cmd(0x75);
  OLED_send_data(0x00);
  OLED_send_data(0x7f);
  OLED_send_cmd(0x5C);
  for (x = 0; x < loops; x++) //column loop
  {
    data = pgm_read_byte(ptr_pic++);
    data = ~data;
    OLED_send_data(data);
  }
}

void  setup()
{
  // Init Serial
  Serial.begin(115200);                      // 115200 for MiSTer ttyUSBx Device CP2102 Chip on ESP32
  Serial.flush();                            // Wait for empty Send Buffer 

  pinMode(SPI_SCK, OUTPUT); // set digital IO pins as output
  pinMode(SPI_SDA, OUTPUT);
  pinMode(SPI_RST, OUTPUT);
  pinMode(SPI_DC, OUTPUT);
  pinMode(SPI_CS, OUTPUT);

  digitalWrite(SPI_RST, LOW);
  delay(10);
  digitalWrite(SPI_RST, HIGH);
  delay(10);

  OLED_init();
  OLED_full();
  delay(300);
  OLED_clear();
  }

void  loop()
{
  point = &city[0];
  Picture_display(point);
  delay(2000);
  Picture_InverseDisplay(point);
  delay(2000);

  point = &amiga[0];
  Picture_display(point);
  delay(2000);
  Picture_InverseDisplay(point);
  delay(2000);

  point = &gng[0];
  Picture_display(point);
  delay(2000);
  Picture_InverseDisplay(point);
  delay(2000);
}

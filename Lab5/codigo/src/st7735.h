#ifndef ST7335_H
#define ST7335_H



#define SPILCD SPI2

/* MADCTL [MY MX MV]
 *    MY  row address order   1 (bottom to top), 0 (top to bottom)
 *    MX  col address order   1 (right to left), 0 (left to right)
 *    MV  col/row exchange    1 (exchange),      0 normal
 */

#define MADCTLGRAPHICS 0x6
#define MADCTLBMP      0x2

#define ST7735_width  128
#define ST7735_height 160

#define LCD_PORT_BKL  GPIOD
#define LCD_PORT GPIOA
#define GPIO_PIN_BKL GPIO3
#define GPIO_PIN_RST GPIO1
#define GPIO_PIN_SCE GPIO5 //CS* ???
#define GPIO_PIN_DC GPIO4

#define LCDSPEED SPI_FAST

#define LOW  0
#define HIGH 1

#define LCD_C LOW 
#define LCD_D HIGH

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_MADCTL 0x36
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E
#define ST7735_COLMOD 0x3A

// some flags for initR() :(
#define INITR_GREENTAB 0x0
#define INITR_REDTAB   0x1
#define INITR_BLACKTAB   0x2

#define INITR_18GREENTAB    INITR_GREENTAB
#define INITR_18REDTAB      INITR_REDTAB
#define INITR_18BLACKTAB    INITR_BLACKTAB
#define INITR_144GREENTAB   0x1

#define ST7735_TFTWIDTH  128
// for 1.44" display
#define ST7735_TFTHEIGHT_144 128
// for 1.8" display
#define ST7735_TFTHEIGHT_18  160

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Color definitions
#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE 0xFFFF

#define MADVAL(x) (((x) << 5) | 8)
#define colstart 2
#define rowstart 3
#define _width 128
#define _height 160
static uint8_t madctlcurrent = MADVAL(MADCTLGRAPHICS);

struct ST7735_cmdBuf {
  uint8_t command;   // ST7735 command byte
  uint8_t delay;     // ms delay after
  uint8_t len;       // length of parameter data
  uint8_t data[16];  // parameter data
};

static const struct ST7735_cmdBuf initializers[] = {
  // SWRESET Software reset 
  {0x01, 150, 0, 0},
  // SLPOUT Leave sleep mode
  {0x11,  150, 0, 0},
  // FRMCTR1, FRMCTR2 Frame Rate configuration -- Normal mode, idle
  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D) 
  {0xB1, 0, 3, { 0x01, 0x2C, 0x2D }},
  {0xB2, 0, 3, { 0x01, 0x2C, 0x2D }},
  // FRMCTR3 Frame Rate configureation -- partial mode
  {0xB3, 0, 6, { 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D }},
  // INVCTR Display inversion (no inversion)
  {0xB4,  0, 1, { 0x07 }},
  // PWCTR1 Power control -4.6V, Auto mode
  {0xC0,  0, 3, { 0xA2, 0x02, 0x84}},
  // PWCTR2 Power control VGH25 2.4C, VGSEL -10, VGH = 3 * AVDD
  {0xC1,  0, 1, { 0xC5}},
  // PWCTR3 Power control, opamp current smal, boost frequency
  {0xC2,  0, 2, { 0x0A, 0x00 }},
  // PWCTR4 Power control, BLK/2, opamp current small and medium low
  {0xC3,  0, 2, { 0x8A, 0x2A}},
  // PWRCTR5, VMCTR1 Power control
  {0xC4,  0, 2, { 0x8A, 0xEE}},
  {0xC5,  0, 1, { 0x0E }},
  // INVOFF Don't invert display
  {0x20,  0, 0, 0},
  // Memory access directions. row address/col address, bottom to top refesh (10.1.27)
  {ST7735_MADCTL,  0, 1, {MADVAL(MADCTLGRAPHICS)}},
  // Color mode 16 bit (10.1.30
  {ST7735_COLMOD,   0, 1, {0x05}},
  // Column address set 0..127 
  {ST7735_CASET,   0, 4, {0x00, 0x00, 0x00, 0x7F }},
  // Row address set 0..159
  {ST7735_RASET,   0, 4, {0x00, 0x00, 0x00, 0x9F }},
  // GMCTRP1 Gamma correction
  {0xE0, 0, 16, {0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D,
			    0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10 }},
  // GMCTRP2 Gamma Polarity corrction
  {0xE1, 0, 16, {0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
			    0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10 }},
  // DISPON Display on
  {0x29, 100, 0, 0},
  // NORON Normal on
  {0x13,  10, 0, 0},
  // End
  {0, 0, 0, 0}
  };

void ST7735_setAddrWindow(uint8_t x0, uint8_t y0, 
			  uint8_t x1, uint8_t y1);
void ST7735_pushColor(uint16_t color);
//void ST7735_init();
void ST7735_backLight(uint8_t on);
void delay_ms(const uint32_t delay);
void write_lcd(uint16_t dc, uint8_t value);
void init_lcd(void);
void pushColor(uint16_t color);
void fillScreen(uint16_t color);
void drawPixel(uint8_t x, uint8_t y, uint16_t color);
void drawFastVLine(uint8_t x, uint8_t y, uint8_t h, uint16_t color);
void drawFastHLine(uint8_t x, uint8_t y, uint8_t w, uint16_t color);
void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h,
             uint16_t color);
void setRotation(uint8_t r);
void invertDisplay(bool i);
#endif

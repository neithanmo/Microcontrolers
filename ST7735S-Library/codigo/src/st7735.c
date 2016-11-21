#include "st7735.h"

uint32_t spiPort;//puerto spi a utilizar para las transacciones de datos
//variable usada en la funcion de inicializaci'on del SPI y GPIOS

static struct st7735_function initializer[] = {
	{ ST7735_START, ST7735_START},
	{ ST7735_CMD, ST7735_SWRESET},
	{ ST7735_DELAY, 150},
	{ ST7735_CMD, ST7735_SLPOUT},
	{ ST7735_DELAY, 500},
	{ ST7735_CMD, ST7735_FRMCTR1},
	{ ST7735_DATA, 0x01},
	{ ST7735_DATA, 0x2C},
	{ ST7735_DATA, 0x2D},
	{ ST7735_CMD, ST7735_FRMCTR2},
	{ ST7735_DATA, 0x01},
	{ ST7735_DATA, 0x2C},
	{ ST7735_DATA, 0x2D},
	{ ST7735_CMD, ST7735_FRMCTR3},
	{ ST7735_DATA, 0x01},
	{ ST7735_DATA, 0x2C},
	{ ST7735_DATA, 0x2D},
	{ ST7735_DATA, 0x01},
	{ ST7735_DATA, 0x2C},
	{ ST7735_DATA, 0x2D},
	{ ST7735_CMD, ST7735_INVCTR},
	{ ST7735_DATA, 0x07},
	{ ST7735_CMD, ST7735_PWCTR1},
	{ ST7735_DATA, 0xA2},
	{ ST7735_DATA, 0x02},
	{ ST7735_DATA, 0x84},
	{ ST7735_CMD, ST7735_PWCTR2},
	{ ST7735_DATA, 0xC5},
	{ ST7735_CMD, ST7735_PWCTR3},
	{ ST7735_DATA, 0x0A},
	{ ST7735_DATA, 0x00},
	{ ST7735_CMD, ST7735_PWCTR4},
	{ ST7735_DATA, 0x8A},
	{ ST7735_DATA, 0x2A},
	{ ST7735_CMD, ST7735_PWCTR5},
	{ ST7735_DATA, 0x8A},
	{ ST7735_DATA, 0xEE},
	{ ST7735_CMD, ST7735_VMCTR1},
	{ ST7735_DATA, 0x0E},
	{ ST7735_CMD, ST7735_INVOFF},
	{ ST7735_CMD, ST7735_MADCTL},
	{ ST7735_DATA, 0xC8},// RGB// estaba en C0 lo cambie a C8
	{ ST7735_CMD, ST7735_COLMOD},
	{ ST7735_DATA, 0x05},        //------> fin comandos de inicializacion basicos Rcmd1
	{ ST7735_CMD, ST7735_CASET}, //1: Column addr set, 4 args, no delay:
	{ ST7735_DATA, 0x00},        //X START = 0
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x00},        // XEND = 127
	{ ST7735_DATA, 0x7F},
	{ ST7735_CMD, ST7735_RASET}, // 2: Row addr set, 4 args, no delay:
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x00},        // XSTART = 0
	{ ST7735_DATA, 0x00},        // XEND = 159
	{ ST7735_DATA, 0x9F},        //-----> segunda serie de comandos Rcmd2red(adafruit)
	{ ST7735_CMD, ST7735_GMCTRP1}, //------Rcmd3 
	{ ST7735_DATA, 0x02},
	{ ST7735_DATA, 0x1C},
	{ ST7735_DATA, 0x07},
	{ ST7735_DATA, 0x12},
	{ ST7735_DATA, 0x37},
	{ ST7735_DATA, 0x32},
	{ ST7735_DATA, 0x29},
	{ ST7735_DATA, 0x2D},
	{ ST7735_DATA, 0x29},
	{ ST7735_DATA, 0x25},
	{ ST7735_DATA, 0x2B},
	{ ST7735_DATA, 0x39},
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x01},
	{ ST7735_DATA, 0x03},
	{ ST7735_DATA, 0x10},
	{ ST7735_CMD, ST7735_GMCTRN1},
	{ ST7735_DATA, 0x03},
	{ ST7735_DATA, 0x1D},
	{ ST7735_DATA, 0x07},
	{ ST7735_DATA, 0x06},
	{ ST7735_DATA, 0x2E},
	{ ST7735_DATA, 0x2C},
	{ ST7735_DATA, 0x29},
	{ ST7735_DATA, 0x2D},
	{ ST7735_DATA, 0x2E},
	{ ST7735_DATA, 0x2E},
	{ ST7735_DATA, 0x37},
	{ ST7735_DATA, 0x3F},
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x02},
	{ ST7735_DATA, 0x10},
	{ ST7735_CMD, ST7735_NORON},
	{ ST7735_DELAY, 10},
	{ ST7735_CMD, ST7735_DISPON},
	{ ST7735_DELAY, 100},
	{ ST7735_END, ST7735_END},
};

uint8_t colstart;
uint8_t rowstart;
uint8_t scr_width;
uint8_t scr_height;
void delay_ms(const uint32_t delay)
{
    uint32_t i, j;

    for( i = 0; i < delay; i++ ){
        for( j = 0; j < 1000; j++){
		__asm__("nop");
	}
    }
}

void delay_us(const uint32_t delay)
{
    uint32_t i;

    for( i = 0; i < delay; i++ ){
	__asm__("nop");
    }
}


static void st7735_reset(void)
{
	/* Reset controller */
	CS_L();
	RST_H();
	delay_ms(500);
	RST_L();
	delay_ms(500);
	RST_H();
	delay_ms(500);
}

void spi_setup(uint32_t SPI)
{
/*
 * Chart of the various SPI ports (1 - 6) and where their pins can be:
 *
 *	 NSS		  SCK			MISO		MOSI
 *	 --------------   -------------------   -------------   ---------------
 * SPI1  PA4, PA15	  PA5, PB3		PA6, PB4	PA7, PB5
 * SPI2  PB9, PB12, PI0   PB10, PB13, PD3, PI1  PB14, PC2, PI2  PB15, PC3, PI3
 * SPI3  PA15*, PA4*	  PB3*, PC10*		PB4*, PC11*	PB5*, PD6, PC12*
 * SPI4  PE4,PE11	  PE2, PE12		PE5, PE13	PE6, PE14
 * SPI5  PF6, PH5	  PF7, PH6		PF8		PF9, PF11, PH7
 * SPI6  PG8		  PG13			PG12		PG14
 *
 * Pin name with * is alternate function 6 otherwise use alternate function 5.
*/
	spiPort = SPI;
	switch(SPI){
	case SPI1:
		{
		rcc_periph_clock_enable(RCC_GPIOA);
		gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO5 | GPIO7);
		gpio_set_af(GPIOA, GPIO_AF5, GPIO5 | GPIO7);
		gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0); //para usar el boton
		gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO7 | GPIO5);
		rcc_periph_clock_enable(RCC_SPI1);
		/* CLK=PA5 || MOSI=PA7 */
		}
		break;
	case SPI2:
		{
		rcc_periph_clock_enable(RCC_GPIOB);
		gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10 | GPIO15);
		gpio_set_af(GPIOB, GPIO_AF5, GPIO10 | GPIO15);
		gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO10 | GPIO15);
		rcc_periph_clock_enable(RCC_SPI2);
		}
		  /* CLK=PB10 || MOSI=PB15 */
		break;
	case SPI3:
		{
		rcc_periph_clock_enable(RCC_GPIOC);
		rcc_periph_clock_enable(RCC_GPIOB);
		gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12);
		gpio_set_af(GPIOC, GPIO_AF6, GPIO12);
		gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO3);
		gpio_set_af(GPIOB, GPIO_AF6, GPIO3);
		gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO10 | GPIO12);
		rcc_periph_clock_enable(RCC_SPI3);
		}
		   /* CLK=PC10 || MOSI=PC12 */
		break;
        default:
		break;
	}
		spi_reset(SPI);
		spi_set_master_mode(SPI);
		spi_enable_software_slave_management(SPI);
		spi_set_nss_high(SPI);// no multiple masters
		spi_enable_ss_output(SPI);
		spi_init_master(SPI, 8000000, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
			SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
		/* Enable SPI1 periph. */
		spi_enable(SPI);
}



void
write_data_lcd(uint8_t data)
{
	//DC = 1 datos, DC=0 comandos
        //cs 1=disable LCD, 0=enable LCD
	//reset debe estar en bajo
 	DC_H();
	CS_L();
	spi_send(spiPort, data);
        while (SPI_SR(spiPort) & SPI_SR_BSY);//esperar hasta que el registro SPI_SR este vacio, para activar CS
	CS_H();
}

void write_cmd_lcd(uint8_t cmd)
{
	//DC = 1 datos, DC=0 comandos
        //cs 1=disable LCD, 0=enable LCD
	//reset debe estar en bajo
	DC_L();
	CS_L();
	spi_send(spiPort, cmd);
        while (SPI_SR(spiPort) & SPI_SR_BSY);//esperar hasta que el registro SPI_SR este vacio, para activar CS
	CS_H();
}	

void init_lcd(void)
{
	//DC = 1 datos, DC=0 comandos
        //cs 1=disable LCD, 0=enable LCD
        colstart = 2;
        rowstart = 3;
	scr_height = HEIGHT;
	scr_width = WIDTH;
	st7735_reset();
	uint8_t i = 0;
	uint8_t end_script = 0;
	do {
		switch (initializer[i].cmd)
		{
		case ST7735_START:
			break;
		case ST7735_CMD:
			write_cmd_lcd(initializer[i].data);
			break;
		case ST7735_DATA:
			write_data_lcd(initializer[i].data);
			break;
		case ST7735_DELAY:
			delay_ms(initializer[i].data);
			break;
		case ST7735_END:
			end_script = 1;
		}
		i++;
	} while (!end_script);
	write_cmd_lcd(ST7735_MADCTL);
	write_data_lcd(0x00);	
}
	

void lcd_backLight(uint8_t on)
{
  if (on)
    gpio_set(LCD_CONTROL_PORT, BLK_PIN);
  else
    gpio_clear(LCD_CONTROL_PORT, BLK_PIN);
}


void lcd_orientacion(ScrOrientation_TypeDef orientation)
{
  write_cmd_lcd(0x36); // Memory data access control:
  switch (orientation)
  {
  case scr_CW:
    scr_width  = HEIGHT;
    scr_height = WIDTH;
    write_data_lcd(0xA0); // X-Y Exchange,Y-Mirror
    break;
  case scr_CCW:
    scr_width  = HEIGHT;
    scr_height = WIDTH;
    write_data_lcd(0x60); // X-Y Exchange,X-Mirror
    break;
  case scr_180:
    scr_width  = WIDTH;
    scr_height = HEIGHT;
    write_data_lcd(0xc0); // X-Mirror,Y-Mirror: Bottom to top; Right to left; RGB
    break;
  default:
    scr_width  = WIDTH;
    scr_height = HEIGHT;
    write_data_lcd(0x00); // Normal: Top to Bottom; Left to Right; RGB
    break;
  }
}


void lcd_setAddrWindow(uint8_t XS, uint8_t XE, uint8_t YS, uint8_t YE) 
{
  write_cmd_lcd(ST7735_CASET); // Column address set
  write_data_lcd(0x00);
  write_data_lcd(XS/*+colstart*/);
  write_data_lcd(0x00);
  write_data_lcd(XE/*+colstart*/);

  write_cmd_lcd(ST7735_RASET); // Row address set
  write_data_lcd(0x00);
  write_data_lcd(YS/*+rowstart*/);
  write_data_lcd(0x00);
  write_data_lcd(YE/*+rowstart*/);
  write_cmd_lcd(ST7735_RAMWR); // Memory write
}

void lcd_Clear(uint16_t color)
{
  uint16_t i;
  lcd_setAddrWindow(0, scr_width - 1, 0, scr_height - 1);
  for (i = 0; i < scr_width * scr_height; i++)
  {
    write_data_lcd(color >> 8);
    write_data_lcd(color);
  }

}

void lcd_Pixel(uint16_t X, uint16_t Y, uint16_t color)
{
  if((X < 0) ||(X >= scr_width) || (Y < 0) || (Y >= scr_height)) return;
  lcd_setAddrWindow(X,X+1,Y, Y+1);
  write_data_lcd(color >> 8);
  write_data_lcd(color);
}

void lcd_HLine(uint8_t x, uint8_t y, uint8_t w, uint16_t color)
{
  uint16_t i;
  uint8_t CH = color >> 8;
  uint8_t CL = (uint8_t)color;
  
  lcd_setAddrWindow(x,x+w-1,y,y);
   while(w--){
    write_data_lcd(CH);
    write_data_lcd(CL);
   }
}
void push_color(uint16_t color)
{
  uint8_t CH = color >> 8;
  uint8_t CL = (uint8_t)color;
    write_data_lcd(CH);
    write_data_lcd(CL);
}
	

void lcd_VLine(uint8_t x, uint8_t y, uint8_t h, uint16_t color)
{
  uint16_t i;
  uint8_t CH = color >> 8;
  uint8_t CL = (uint8_t)color;


  lcd_setAddrWindow(x,x,y,y+h-1);
  while(h--){
    write_data_lcd(CH);
    write_data_lcd(CL);
  }
}

void lcd_Line(int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint16_t color)
{
  int16_t dX = X2-X1;
  int16_t dY = Y2-Y1;
  int16_t dXsym = (dX > 0) ? 1 : -1;
  int16_t dYsym = (dY > 0) ? 1 : -1;

  if (dX == 0)
  {
    if (Y2>Y1) lcd_VLine(X1,Y1,Y2,color); else lcd_VLine(X1,Y2,Y1,color);
    return;
  }
  if (dY == 0)
  {
    if (X2>X1) lcd_HLine(X1,X2,Y1,color); else lcd_HLine(X2,X1,Y1,color);
    return;
  }

  dX *= dXsym;
  dY *= dYsym;
  int16_t dX2 = dX << 1;
  int16_t dY2 = dY << 1;
  int16_t di;

  if (dX >= dY)
  {
    di = dY2 - dX;
    while (X1 != X2)
    {
      lcd_Pixel(X1,Y1,color);
      X1 += dXsym;
      if (di < 0)
      {
        di += dY2;
      }
      else
      {
        di += dY2 - dX2;
        Y1 += dYsym;
      }
    }
  }
  else
  {
    di = dX2 - dY;
    while (Y1 != Y2)
    {
      lcd_Pixel(X1,Y1,color);
      Y1 += dYsym;
      if (di < 0)
      {
        di += dX2;
      }
      else
      {
        di += dX2 - dY2;
        X1 += dXsym;
      }
    }
  }
  lcd_Pixel(X1,Y1,color);

}

void lcd_Rect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t color)
{

  lcd_HLine(X1,X2,Y1,color);
  lcd_HLine(X1,X2,Y2,color);
  lcd_VLine(X1,Y1,Y2,color);
  lcd_VLine(X2,Y1,Y2,color);

}

void lcd_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color)
{
  uint16_t i;
  //uint8_t y=0;
  uint8_t CH = color >> 8;
  uint8_t CL = (uint8_t)color;
  if((x > scr_width) || (y > scr_height)) return;
  if((x + w - 1) >= scr_width)  w = scr_width  - x;
  if((y + h - 1) >= scr_height) h = scr_height - y;
  uint16_t FS = (w - x + 1) * (h - y + 1);
  lcd_setAddrWindow(x,x+w-1, y, y+h-1);
  for(i=0; i<FS; i++) {
        write_data_lcd(CH);
        write_data_lcd(CL);
  }
}


//###################################### Primitivas utilizandos algoritmos encontrados en Adafruit_GFX ##################################################
void drawCircle(uint8_t x0, uint8_t y0, uint8_t r,
 uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  lcd_Pixel(x0  , y0+r, color);
  lcd_Pixel(x0  , y0-r, color);
  lcd_Pixel(x0+r, y0  , color);
  lcd_Pixel(x0-r, y0  , color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    lcd_Pixel(x0 + x, y0 + y, color);
    lcd_Pixel(x0 - x, y0 + y, color);
    lcd_Pixel(x0 + x, y0 - y, color);
    lcd_Pixel(x0 - x, y0 - y, color);
    lcd_Pixel(x0 + y, y0 + x, color);
    lcd_Pixel(x0 - y, y0 + x, color);
    lcd_Pixel(x0 + y, y0 - x, color);
    lcd_Pixel(x0 - y, y0 - x, color);
  }
}

void drawCircleHelper(uint8_t x0, uint8_t y0,
 uint8_t r, uint8_t cornername, uint16_t color) 
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      lcd_Pixel(x0 + x, y0 + y, color);
      lcd_Pixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      lcd_Pixel(x0 + x, y0 - y, color);
      lcd_Pixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      lcd_Pixel(x0 - y, y0 + x, color);
      lcd_Pixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      lcd_Pixel(x0 - y, y0 - x, color);
      lcd_Pixel(x0 - x, y0 - y, color);
    }
  }
}

void fillCircle(uint8_t x0, uint8_t y0, uint8_t r,
 uint16_t color) 
{
  lcd_VLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}


void fillCircleHelper(uint8_t x0, uint8_t y0, uint8_t r,
 uint8_t cornername, uint8_t delta, uint16_t color) {

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      lcd_VLine(x0+x, y0-y, 2*y+1+delta, color);
      lcd_VLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      lcd_VLine(x0-x, y0-y, 2*y+1+delta, color);
      lcd_VLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}
void drawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
      uint8_t x2, uint8_t y2, uint16_t color)
{
  lcd_Line(x0, y0, x1, y1, color);
  lcd_Line(x1, y1, x2, y2, color);
  lcd_Line(x2, y2, x0, y0, color);

}

void drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h,
 uint16_t color){

  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}

void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
 uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    _swap_uint8_t(x0, y0);
    _swap_uint8_t(x1, y1);
  }

  if (x0 > x1) {
    _swap_uint8_t(x0, x1);
    _swap_uint8_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      lcd_Pixel(y0, x0, color);
    } else {
      lcd_Pixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void drawFastVLine(uint8_t x, uint8_t y,
 uint8_t h, uint16_t color) {
  // Update in subclasses if desired!
  drawLine(x, y, x, y+h-1, color);
}

void drawFastHLine(uint8_t x, uint8_t y,
 uint8_t w, uint16_t color) {
  // Update in subclasses if desired!
  drawLine(x, y, x+w-1, y, color);
}

void fillTriangle(uint8_t x0, uint8_t y0,
 uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color) {

  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    _swap_uint8_t(y0, y1); _swap_uint8_t(x0, x1);
  }
  if (y1 > y2) {
    _swap_uint8_t(y2, y1); _swap_uint8_t(x2, x1);
  }
  if (y0 > y1) {
    _swap_uint8_t(y0, y1); _swap_uint8_t(x0, x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    drawFastHLine(a, y0, b-a+1, color);
    return;
  }

  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
  int32_t
    sa   = 0,
    sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) _swap_uint8_t(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) _swap_uint8_t(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }
}


void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, //es mucho mas rapido utilizar lcd_FillRect
 uint16_t color) {
  // Update in subclasses if desired!
  int16_t i;
  for (i=x; i<x+w; i++) {
    drawFastVLine(i, y, h, color);
  }
}

void drawRoundRect(uint8_t x, uint8_t y, uint8_t w,
 uint8_t h, uint8_t r, uint16_t color) {
  // smarter version
  drawFastHLine(x+r  , y    , w-2*r, color); // Top
  drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  drawFastVLine(x    , y+r  , h-2*r, color); // Left
  drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r    , y+r    , r, 1, color);
  drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

void fillRoundRect(uint8_t x, uint8_t y, uint8_t w,
 uint8_t h, uint8_t r, uint16_t color) {
  // smarter version
  fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

uint16_t RGB565(uint8_t R,uint8_t G,uint8_t B)///color
{                                              
  return ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
}

uint8_t getRed(uint16_t rgbPixel){
	return ((rgbPixel>>11)<<3)&0xFF;
}

uint8_t getBlue(uint16_t rgbPixel){
	return ((rgbPixel>>5)<<2)&0xFF;
}

uint8_t getGreen(uint16_t rgbPixel){
	return (rgbPixel<<3)&0xFF;
}

uint16_t swapcolor(uint16_t x) 
{ 
  return (x << 11) | (x & 0x07E0) | (x >> 11);
}

void drawChar(uint8_t x, uint8_t y, unsigned char c,
 uint16_t color, uint16_t bg, uint8_t size) {

  if(1) { // 'Classic' built-in font

    if((x >= scr_width)            || // Clip right
       (y >= scr_height)           || // Clip bottom
       ((x + 6 * size - 1) < 0) || // Clip left
       ((y + 8 * size - 1) < 0))   // Clip top
      return;

    //if((c >= 176)) c++; // Handle 'classic' charset behavior
    int8_t i,j;
    for(i=0; i<6; i++ ) {
      uint8_t line;
      if(i < 5) line = ((c >= 0x20) && (c <= 0x7F)) ? Font5x7[(c-32)*5+i] : Font5x7[(c-64)*5+i];
      else      line = 0x0;
      for(j=0; j<8; j++, line >>= 1) {
        if(line & 0x1) {
          if(size == 1) lcd_Pixel(x+i, y+j, color);
          else          lcd_FillRect(x+(i*size), y+(j*size), size, size, color);
        } else if(bg != color) {
          if(size == 1) lcd_Pixel(x+i, y+j, bg);
          else          lcd_FillRect(x+i*size, y+j*size, size, size, bg);
        }
      }
    }

  }
}

void st_PutStr5x7(uint8_t scale, uint8_t X, uint8_t Y, char *str, uint16_t color, uint16_t bgcolor)
{
  // scale equals 1 drawing faster
  if (scale == 1)
  {
    while (*str)
    {
      drawChar(X,Y,*str++,color,bgcolor,scale);
      if (X < scr_width - 6) { X += 6; } else if (Y < scr_height - 8) { X = 0; Y += 8; } else { X = 0; Y = 0; }
    };
  }
  else
  {
    while (*str)
    {
      drawChar(X,Y,*str++,color,bgcolor,scale);
      if (X < scr_width - (scale*5) + scale) { X += (scale * 5) + scale; } else if (Y < scr_height - (scale * 7) + scale) { X = 0; Y += (scale * 7) + scale; } else { X = 0; Y = 0; }
    };
  }
}

void drawBitmap(uint8_t x, uint8_t y,
 uint8_t *bitmap, uint8_t w, uint8_t h, uint16_t color, uint16_t bg)
{

  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(i & 7) byte >>= 1;
      else      byte   = bitmap[j * byteWidth + i / 8];
      if(byte & 0x01) lcd_Pixel(x+i, y+j, color);
    }
  }
}


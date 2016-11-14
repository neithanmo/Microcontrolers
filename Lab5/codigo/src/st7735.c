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
		gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO7 | GPIO5);
		gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0); //para usar el boton
		rcc_periph_clock_enable(RCC_SPI1);
		/* CLK=PA5 || MOSI=PA7 */
		}
		break;
	case SPI2:
		{
		rcc_periph_clock_enable(RCC_GPIOB);
		gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10 | GPIO15);
		gpio_set_af(GPIOB, GPIO_AF5, GPIO10 | GPIO15);
		//gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO10 | GPIO15);
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
		//gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO10 | GPIO12);
		rcc_periph_clock_enable(RCC_SPI3);
		}
		   /* CLK=PC10 || MOSI=PC12 */
		break;
        default:
		{
		 gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6 | GPIO7);
		 gpio_set_output_options(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO6 | GPIO7);
                }
		break;
	}
		spi_reset(SPI);
		spi_set_master_mode(SPI);
		//spi_send_msb_first(SPI);
		//spi_set_dff_8bit(SPI);
		//spi_set_unidirectional_mode(SPI);
		//spi_set_clock_phase_0(SPI);
		//spi_set_clock_polarity_0(SPI);
		spi_enable_software_slave_management(SPI);
		spi_set_nss_high(SPI);// no multiple masters
		spi_enable_ss_output(SPI);
		//spi_set_standard_mode(SPI, 0);
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
	delay_us(350);
	CS_H();
/*se ha de indicar a la pantalla el tipo de datos que se envia:
si DC esta en alto, se le indica que se ha enviado data a graficar
caso contraio se le indica a la pantalla que se envia un comando 
tambien hay que habilitar el dispositivo, CS en bajo activa la pantalla 
y RST en alto resetea la pantalla, es decir ambas banderas deben estar en bajo*/
     /*uint8_t bit;
    for(bit = 0x80; bit; bit >>= 1) {
      gpio_clear(GPIOD,GPIO6);
      if(data & bit) gpio_set(GPIOD,GPIO7);
      else          gpio_clear(GPIOD,GPIO7);
      gpio_set(GPIOD,GPIO6);
      gpio_toggle(GPIOD, GPIO12);
      gpio_toggle(GPIOD, GPIO14);
    }*/
}

void write_cmd_lcd(uint8_t cmd)
{
	//DC = 1 datos, DC=0 comandos
        //cs 1=disable LCD, 0=enable LCD
	//reset debe estar en bajo
	DC_L();
	CS_L();
	spi_send(spiPort, cmd);
	delay_us(350);
	CS_H();
    /*for(bit = 0x80; bit; bit >>= 1) {
      gpio_clear(GPIOD,GPIO6);
      if(cmd & bit) gpio_set(GPIOD,GPIO7);
      else          gpio_clear(GPIOD,GPIO7);
      gpio_set(GPIOD,GPIO6);
      gpio_toggle(GPIOD, GPIO12);
      gpio_toggle(GPIOD, GPIO14);
    }*/
}	

void init_lcd(void)
{
	//DC = 1 datos, DC=0 comandos
        //cs 1=disable LCD, 0=enable LCD
        colstart = 2;
        rowstart = 3;
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


/*void lcd_orientacion(ScrOrientation_TypeDef orientation)
{
  write_cmd_lcd(0x36); // Memory data access control:
  switch (orientation)
  {
  case scr_CW:
    _width  = scr_h;
    _height = scr_w;
    write_data_lcd(0xA0); // X-Y Exchange,Y-Mirror
    break;
  case scr_CCW:
    scr_width  = scr_h;
    scr_height = scr_w;
    write_data_lcd(0x60); // X-Y Exchange,X-Mirror
    break;
  case scr_180:
    scr_width  = scr_w;
    scr_height = scr_h;
    write_data_lcd(0xc0); // X-Mirror,Y-Mirror: Bottom to top; Right to left; RGB
    break;
  default:
    scr_width  = scr_w;
    scr_height = scr_h;
    write_data_lcd(0x00); // Normal: Top to Bottom; Left to Right; RGB
    break;
  }
}*/


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
  lcd_setAddrWindow(0, WIDTH - 1, 0, HEIGHT - 1);
  for (i = 0; i < WIDTH * HEIGHT; i++)
  {
    write_data_lcd(color >> 8);
    write_data_lcd(color);
  }

}

void lcd_Pixel(uint16_t X, uint16_t Y, uint16_t color)
{
  if((X < 0) ||(X >= WIDTH) || (Y < 0) || (Y >= HEIGHT)) return;
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
  if((x > WIDTH) || (y > HEIGHT)) return;
  if((x + w - 1) >= WIDTH)  w = WIDTH  - x;
  if((y + h - 1) >= HEIGHT) h = HEIGHT - y;
  uint16_t FS = (w - x + 1) * (h - y + 1);
  lcd_setAddrWindow(x,x+w-1, y, y+h-1);
  for(i=0; i<FS; i++) {
        write_data_lcd(CH);
        write_data_lcd(CL);
  }
}




uint16_t RGB565(uint8_t R,uint8_t G,uint8_t B)///color
{                                              
  return ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
}

uint16_t swapcolor(uint16_t x) 
{ 
  return (x << 11) | (x & 0x07E0) | (x >> 11);
}

#ifndef _MPINS_H_
#define _MPINS_H_
/*
  Проект FSM-ESP32 с цветным tft дисплеем 1.8" 128*160
  Выводы ESP32 и PCF8574A
  pcb: eltr_v5.8
*/

#include "stdint.h"

namespace MPins
{
  //                   имя          порт      функция    цепь        примечание
  // Входы
  constexpr int8_t celsius_pin      = 36;   // an(SVP)  CELSIUS    Порт измерителя температуры
  constexpr int8_t pow_good_pin     = 39;   // in(SVN)  POW_CH     Порт проверки сетевого питания 
  constexpr int8_t pol_pin          = 34;   // in       REV_POL    Порт проверки полярности 
  constexpr int8_t int_pin          = 35;   // in                  Порт прерываний от PCF

  // Выходы с ШИМ
  constexpr int8_t pwm_fan_pin      = 12;   // out      FAN        Порт управления вентилятором

  // Выходы управления синим светодиодом 
  constexpr int8_t led_b_pin        = 02;   // out,pd   LED_B      Порт управления синим светодиодом

  // I2C - управление расширителем выводов PCF8574A
  constexpr int8_t i2c_sda_pin      = 21;   // io       SDA
  constexpr int8_t i2c_scl_pin      = 22;   // out      SCL 
  
  // Порты клавиатуры
  constexpr int8_t touch_p_pin      = 04;   // an       KEY_P     
  constexpr int8_t touch_dn_pin     = 33;   // an       KEY_DN    
  constexpr int8_t touch_b_pin      = 15;   // an       KEY_B     
  constexpr int8_t touch_up_pin     = 32;   // an       KEY_UP    
  constexpr int8_t touch_c_pin      = 27;   // an       KEY_C     
      
  // HSPI для дисплея 128*160. Порт IO12 как MISO не используется
  constexpr int8_t hspi_scl_pin     = 14;   // out      HSPI_SCL  
  constexpr int8_t hspi_sda_pin     = 13;   // out      HSPI_SDA
  constexpr int8_t lcd_res_pin      = 25;   // out      LCD_RES  
  constexpr int8_t lcd_dc_pin       = 26;   //          LCD_DC
  constexpr int8_t lcd_cs_pin       = -1;   //          LCD_CS     Grounded

  // VSPI для карты памяти
  constexpr int8_t vspi_sck_pin     = 18;   // out      VSPI_SCK  
  constexpr int8_t vspi_si_pin      = 23;   // in       VSPI_SI  
  constexpr int8_t vspi_so_pin      = 19;   // out      VSPI_SO  
  constexpr int8_t vspi_cs_pin      = 05;   // out      VSPI_CS  
  constexpr int8_t sd_cd_pin        = 35;   // in       SD_CD  

  // UART или I2C драйвера силовой платы
  constexpr int8_t u2rxd_pin        = 16;   // io       U2RXD
  constexpr int8_t u2txd_pin        = 17;   // io       U2TXD

  constexpr int8_t i2c2_sda_pin     = 16;   // io       SDA
  constexpr int8_t i2c2_scl_pin     = 17;   // out      SCL 
  
  // Порты расширителя ввода-вывода PCF8574A
  // constexpr int8_t buz_pin          =  0;   // out      BUZ         Зуммер 
  // constexpr int8_t lcd_blk_pin      =  1;   // out      LCD_BLK     Подсветка дисплея 
  // constexpr int8_t sw_pin           =  2;   // out      SW_ON       Управление ключами 
  // constexpr int8_t out_pin          =  3;   // out      OUT_ON      Управление источником
  // constexpr int8_t led_r_pin        =  4;   // out      LED_R       Красный светодиод
  // constexpr int8_t led_g_pin        =  5;   // out      LED_G       Зеленый светодиод
  // То же, битовые 
  constexpr int8_t buz      =  0b00000001;   // BUZ         Зуммер 
  constexpr int8_t blk      =  0b00000010;   // LCD_BLK     Подсветка дисплея 
  constexpr int8_t sw       =  0b00000100;   // SW_ON       Управление ключами 
  constexpr int8_t out      =  0b00001000;   // OUT_ON      Управление источником
  constexpr int8_t led_r    =  0b00010000;   // LED_R       Красный светодиод
  constexpr int8_t led_g    =  0b00100000;   // LED_G       Зеленый светодиод
};

#endif // !_MPINS_H_

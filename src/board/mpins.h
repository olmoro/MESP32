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
  constexpr char celsius_pin      = 36;   // an(SVP)  CELSIUS    Порт измерителя температуры
  constexpr char pow_good_pin     = 39;   // in(SVN)  POW_CH     Порт проверки сетевого питания 
  constexpr char pol_pin          = 34;   // in       REV_POL    Порт проверки полярности 
  constexpr char int_pin          = 35;   // in                  Порт прерываний от PCF

  // Выходы с ШИМ
  constexpr char pwm_fan_pin      = 12;   // out      FAN        Порт управления вентилятором

  // Выходы управления синим светодиодом 
  constexpr char led_b_pin        = 02;   // out,pd   LED_B      Порт управления синим светодиодом

  // I2C - управление расширителем выводов PCF8574A
  constexpr char i2c_sda_pin      = 21;   // io       SDA
  constexpr char i2c_scl_pin      = 22;   // out      SCL 
  
  // Порты клавиатуры
  constexpr char touch_p_pin      = 04;   // an       KEY_P     
  constexpr char touch_dn_pin     = 33;   // an       KEY_DN    
  constexpr char touch_b_pin      = 15;   // an       KEY_B     
  constexpr char touch_up_pin     = 32;   // an       KEY_UP    
  constexpr char touch_c_pin      = 27;   // an       KEY_C     
      
  // HSPI для дисплея 128*160. Порт IO12 как MISO не используется
  constexpr char hspi_scl_pin     = 14;   // out      HSPI_SCL  
  constexpr char hspi_sda_pin     = 13;   // out      HSPI_SDA
  constexpr char lcd_res_pin      = 25;   // out      LCD_RES  
  constexpr char lcd_dc_pin       = 26;   //          LCD_DC
  constexpr char lcd_cs_pin       = -1;   //          LCD_CS     Grounded

  // VSPI для карты памяти
  constexpr char vspi_sck_pin     = 18;   // out      VSPI_SCK  
  constexpr char vspi_si_pin      = 23;   // in       VSPI_SI  
  constexpr char vspi_so_pin      = 19;   // out      VSPI_SO  
  constexpr char vspi_cs_pin      = 05;   // out      VSPI_CS  
  constexpr char sd_cd_pin        = 35;   // in       SD_CD  

  // UART или I2C драйвера силовой платы
  constexpr char u2rxd_pin        = 16;   // io       U2RXD
  constexpr char u2txd_pin        = 17;   // io       U2TXD

  constexpr char i2c2_sda_pin     = 16;   // io       SDA
  constexpr char i2c2_scl_pin     = 17;   // out      SCL 
  
  // Порты расширителя ввода-вывода PCF8574A
  // constexpr char buz_pin          =  0;   // out      BUZ         Зуммер 
  // constexpr char lcd_blk_pin      =  1;   // out      LCD_BLK     Подсветка дисплея 
  // constexpr char sw_pin           =  2;   // out      SW_ON       Управление ключами 
  // constexpr char out_pin          =  3;   // out      OUT_ON      Управление источником
  // constexpr char led_r_pin        =  4;   // out      LED_R       Красный светодиод
  // constexpr char led_g_pin        =  5;   // out      LED_G       Зеленый светодиод
  // То же, битовые 
  constexpr char buz      =  0b00000001;   // BUZ         Зуммер 
  constexpr char blk      =  0b00000010;   // LCD_BLK     Подсветка дисплея 
  constexpr char sw       =  0b00000100;   // SW_ON       Управление ключами 
  constexpr char out      =  0b00001000;   // OUT_ON      Управление источником
  constexpr char led_r    =  0b00010000;   // LED_R       Красный светодиод
  constexpr char led_g    =  0b00100000;   // LED_G       Зеленый светодиод
};

#endif // !_MPINS_H_

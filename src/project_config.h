/*
   Project configuration file, accessible from all libraries connected to the project
   -------------------------------------------------------------------------------------------------
   (с) 2020 Разживин Александр | Razzhivin Alexander
   kotyara12@yandex.ru | https://kotyara12.ru | tg: @kotyara1971
*/

//#pragma once
#ifndef _PROJECT_CONFIG_H_
#define _PROJECT_CONFIG_H_

/* Moro, перенесенные из platformio.ini */
  #define DEBUG_KEYS 0
  #define PRINT_KEY 0
  // ;#define DEBUG_BUTTON 0
  
  // ; Отладка порогов срабатывания кнопок
  #define DEBUD_TOUCH_UP 0
  #define DEBUD_TOUCH_DN 0
  #define DEBUD_TOUCH_P 0
  #define DEBUD_TOUCH_B 0
  #define DEBUD_TOUCH_C 0
#define DEBUG_QUEUE 1
#define DEBUG_PID 1

/* WiFi */
// #define CONFIG_WIFI_SSID "iotWIFI"
// #define CONFIG_WIFI_PASSWORD "iotPASS"
// #define CONFIG_WIFI_TIMEOUT 30000
// #define CONFIG_WIFI_MAXIMUM_RETRY 99

/* MQTT */
// #define CONFIG_MQTT_SERVER "srv2.clusterfly.ru"
// #define CONFIG_MQTT_PORT 9991
// #define CONFIG_MQTT_DEVICE "ESP-43GFS6"
// #define CONFIG_MQTT_USER "mqttUSER"
// #define CONFIG_MQTT_PASSWORD "mqttPASS"
// #define CONFIG_MQTT_PREFIX "user_mqttUSER/"
// #define CONFIG_MQTT_INTERVAL 2000

/* Telegram notify */
// #define CONFIG_TELEGRAM_TOKEN "tgTOKEN"
// #define CONFIG_TELEGRAM_CHATID -12345678901233
// #define CONFIG_TELEGRAM_DEVICE "😀 ESP-43GFS6"
// #define CONFIG_TELEGRAM_SEND_QUEUE_SIZE 16

/* rlog : debug log settings */
// #define CONFIG_RLOG_PROJECT_LEVEL RLOG_LEVEL_DEBUG
// #define CONFIG_RLOG_PROJECT_COLORS 0
// #define CONFIG_RLOG_SHOW_TIMESTAMP 0

/* rled32 : task settings */
// #define CONFIG_LED_QUEUE_SIZE 3
// #define CONFIG_LED_TASK_STACK_SIZE 2048
// #define CONFIG_LED_TASK_PRIORITY 5
// #define CONFIG_LED_TASK_CORE 1
// #define CONFIG_LED_LOG_RU 0

/* rled32 : system blink modes */

// #define CONFIG_LEDSYS_NORMAL_QUANTITY 1
// #define CONFIG_LEDSYS_NORMAL_DURATION 75
// #define CONFIG_LEDSYS_NORMAL_INTERVAL 5000
// #define CONFIG_LEDSYS_ERROR_QUANTITY 1
// #define CONFIG_LEDSYS_ERROR_DURATION 1000
// #define CONFIG_LEDSYS_ERROR_INTERVAL 1000
// #define CONFIG_LEDSYS_WARNING_QUANTITY 1
// #define CONFIG_LEDSYS_WARNING_DURATION 1000
// #define CONFIG_LEDSYS_WARNING_INTERVAL 1000
// #define CONFIG_LEDSYS_WIFI_ERROR_QUANTITY 1
// #define CONFIG_LEDSYS_WIFI_ERROR_DURATION 500
// #define CONFIG_LEDSYS_WIFI_ERROR_INTERVAL 500
// #define CONFIG_LEDSYS_WIFI_INIT_QUANTITY 1
// #define CONFIG_LEDSYS_WIFI_INIT_DURATION 250
// #define CONFIG_LEDSYS_WIFI_INIT_INTERVAL 250
// #define CONFIG_LEDSYS_INET_ERROR_QUANTITY 2
// #define CONFIG_LEDSYS_INET_ERROR_DURATION 100
// #define CONFIG_LEDSYS_INET_ERROR_INTERVAL 500
// #define CONFIG_LEDSYS_MQTT_ERROR_QUANTITY 3
// #define CONFIG_LEDSYS_MQTT_ERROR_DURATION 250
// #define CONFIG_LEDSYS_MQTT_ERROR_INTERVAL 1000
// #define CONFIG_LEDSYS_TG_ERROR_QUANTITY 4
// #define CONFIG_LEDSYS_TG_ERROR_DURATION 250
// #define CONFIG_LEDSYS_TG_ERROR_INTERVAL 1000
// #define CONFIG_LEDSYS_PUB_ERROR_QUANTITY 5
// #define CONFIG_LEDSYS_PUB_ERROR_DURATION 250
// #define CONFIG_LEDSYS_PUB_ERROR_INTERVAL 1000

#endif  //_PROJECT_CONFIG_H_
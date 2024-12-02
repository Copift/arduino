//
// config.h
//

// Настройки WiFi
#define wifi_ssid     "Ета мой вайфай"
#define wifi_pass     "123lena123"

// Сервер MQTT
#define mqtt_server   "35.172.255.228"
#define mqtt_port     1883
#define mqtt_login    ""
#define mqtt_pass     ""

// MQTT топики
#define mqtt_topic_status  "esp8266COPIFT/15/status"
#define mqtt_topic_out     "esp8266COPIFT/15/heartbeat"
#define mqtt_topic_in      "esp8266COPIFT/15/led"
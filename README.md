# snsTempTracker
Logging temp/humidity using ESP8266 and sending data to ThingSpeak to graph


Using ESP8266MOD AI-THINKER board and DHT22 temp/humidity sensor

Requires Arduino libraries ESP8266WiFi, TaskScheduler, DHTesp

Schedules task every 5 min to read sensor and submit to ThingSpeak. 

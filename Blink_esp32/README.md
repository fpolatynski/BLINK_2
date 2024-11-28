# ESP-IDF websocket server
Super easy esp32 program to create websocket server, wait for connection and then response with led state through that connection.

### Befor running you need to enable websocket in menuconfig and provide SSID and password in main/main.c

```idf.py menuconfig```

Component config -> HTTO Server -> WebSocket server support
import asyncio
import websocket

# WebSocket server URL (Replace with the ESP32's IP address)
SERVER_URL = "ws://192.168.0.66/ws"

def connect_to_esp32():
    ws = websocket.WebSocket()
    ws.connect(SERVER_URL)
    print("Connected to WebSocket server")

    while True:
        response = ws.recv()
        print(f"Response from server: {response}")

connect_to_esp32()
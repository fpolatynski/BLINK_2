import asyncio
import websockets

# WebSocket server URL (Replace with the ESP32's IP address)
SERVER_URL = "ws://192.168.0.66:80/ws"

async def connect_to_esp32():
    async with websockets.connect(SERVER_URL) as websocket:
        print("Connected to WebSocket server")

        while True:
            response = await websocket.recv()
            print(f"Response from server: {response}")

# Run the asyncio event loop
asyncio.run(connect_to_esp32())
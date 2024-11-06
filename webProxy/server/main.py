#!/usr/bin/env python

import asyncio
from websockets.asyncio.server import serve
import json
import os
import time

filename = "image00.png"

with open(filename, 'rb') as file:
    content = file.read()
edit_time = os.path.getmtime(filename)
async def request_response(websocket):
    async for message in websocket:
        print(message)
        request = json.loads(message)
        if request["request_type"] == "image":
            file_to_send = {"response_type": "image",
                            "capture_time": time.ctime(edit_time),
                            "content": list(content)}
            await websocket.send(json.dumps(file_to_send))
        if request["request_type"] == "batt_volt":
            data_to_send = {"response_type": "batt_volt",
                            "timestamp": time.ctime(edit_time),
                            "content": 12.32}
            await websocket.send(json.dumps(data_to_send))
        if request["request_type"] == "coils_curr":
            data_to_send = {"response_type": "coils_curr",
                            "timestamp": time.ctime(edit_time),
                            "content": [200,200,300,400]}
            await websocket.send(json.dumps(data_to_send))

async def main():
    async with serve(request_response, "localhost", 8765):
        await asyncio.get_running_loop().create_future()  # run forever

asyncio.run(main())

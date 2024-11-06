#!/usr/bin/env python

import asyncio
import time

from websockets.sync.client import connect,ClientConnection
import json



def hello():
    with connect("ws://localhost:8765", max_size=10000000) as websocket:
        message = websocket.recv()
        data_received = json.loads(message)
        # Extract the filename to save from the received data
        filename_to_save = data_received["filename"]
        # Convert the content to bytes for writing to a file
        file_content = bytes(data_received["content"])
        # Write the content to a file with the specified filename
        with open(f"{filename_to_save}", 'wb') as file:
            file.write(file_content)


def request_image(ws: ClientConnection):
    request = {"request_type": "image"}
    ws.send(json.dumps(request))

def acquire_message(ws: ClientConnection):
    message = ws.recv()
    data_received = json.loads(message)
    response_type = data_received["response_type"]
    if response_type == "image":
        file_content = bytes(data_received["content"])
        edit_time = data_received["capture_time"]
        print(edit_time)
        # Write the content to a file with the specified filename
        with open(f"current_image.png", 'wb') as file:
            file.write(file_content)

if __name__ == "__main__":
    ws = connect("ws://localhost:8765", max_size=10000000)
    request_image(ws)
    acquire_message(ws)



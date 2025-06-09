import serial
import time
from pynput import keyboard

# Change 'COMX' to your Bluetooth COM port (e.g., 'COM5' on Windows or '/dev/rfcomm0' on Linux)
bluetooth_port = 'COMX'  
baud_rate = 9600  

try:
    ser = serial.Serial(bluetooth_port, baud_rate)
    print(f"Connected to {bluetooth_port}")
except:
    print("Failed to connect to Bluetooth module")
    exit()

last_command = None

def send_command(command):
    global last_command
    if command != last_command:
        ser.write(command.encode())  # Send command to Bluetooth
        print(f"Sent: {command}")
        last_command = command

def on_press(key):
    try:
        if key.char in ['w', 'a', 's', 'd', ' ', 'v']:  # Movement + Claw keys
            send_command(key.char.upper())  # Convert to uppercase
    except AttributeError:
        pass  # Ignore special keys

def on_release(key):
    try:
        if key.char in ['w', 'a', 's', 'd']:
            send_command('X')  # Stop movement when key is released
    except AttributeError:
        pass

# Start listening to keyboard input
with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
    listener.join()

import serial

ser = serial.Serial('COM4', 115200)

print("Turn on reception")
try:
    while True:
        # get data and decode
        data = ser.readline().decode('utf-8').rstrip()

        # show data
        print(data)

        # check the keyword "close"
        if data.lower() == "close":
            print("received 'close' ,Shutting down the serial port....")
            break

except Exception as e:
    print(f"Exceptions occur: {e}")

finally:
    if ser.is_open:
        ser.close()
        print("Serial port closed")

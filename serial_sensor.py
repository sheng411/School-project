import serial

ser = serial.Serial('COM9', 115200)


def serial_send_data(data):
    try:
        print(f"Sending data: {data}")
        ser.write(data.encode('UTF-8'))  # Encode data to bytes for serial transmission

    except serial.SerialException as e:
        print(f"Serial port error: {e}")
    return data


def serial_listen_function():
    print("Turn on reception")
    for i in range(1):
        # get data and decode
        data = ser.readline().decode('UTF-8').rstrip()
        # show data
        #print(data)
        # check the keyword "close"
        '''
        if data.lower() == "close":
            print("received 'close' ,Shutting down the serial port....")
            break
        '''
        return data


##test
'''
while True:
    d=input("-->")
    print(serial_send_data(d))
    
'''

import serial

ser = serial.Serial('COM6', 115200)


def serial_send_data(data):
    try:
        print(f"Sending data: {data}")
        data+="\n"
        ser.write(data.encode('UTF-8'))  # Encode data to bytes for serial transmission

    except serial.SerialException as e:
        print(f"Serial port error: {e}")
    return data


def serial_listen_function():
    print("Turn on reception")
    for i in range(1):
        # get data and decode
        data = ser.readline().decode('UTF-8').rstrip()

        # check the keyword "close"
        return data


##test
'''
while True:
    
    d=input("-->")
    print("p->",serial_send_data(d))
    
    print(serial_listen_function())
    '''

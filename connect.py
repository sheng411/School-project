import socket

# setting the ESP32 address and port number
ESP32_IP = '192.168.4.1'
ESP32_PORT = 1880

def wifi_connect(message):
    # creating socket objects
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # connect ESP32
    client_socket.connect((ESP32_IP, ESP32_PORT))

    # send data to ESP32
    #message=input("enter string->")
    message+="\n"
    client_socket.sendall(message.encode())

    # receive data from ESP32
    data = client_socket.recv(1024)
    print('Received from ESP32:', data.decode())

    # close socket
    client_socket.close()
    return "wifi ok->"+message

# test
#print(wifi_connect())

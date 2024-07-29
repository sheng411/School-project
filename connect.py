import socket
import json
import threading

# setting the ESP32 address and port number
ESP32_IP = '192.168.4.1'
ESP32_PORT = 1880
# creating socket objects
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.settimeout(10)    #set connection timeout

# connect ESP32
client_socket.connect((ESP32_IP, ESP32_PORT))


def wifi_connect(message):
    try:
        #print("Wifi connecting...")
        '''
        # creating socket objects
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.settimeout(10)    #set connection timeout

        # connect ESP32
        client_socket.connect((ESP32_IP, ESP32_PORT))'''
        #print("Wifi is connected !")

        # send data to ESP32
        #message=input("enter string->")
        message+="\n"
        client_socket.sendall(message.encode())

        # receive data from ESP32
        data = client_socket.recv(1024)
        print('Received from ESP32:', data.decode())
        data=""

    except socket.error as e:
        print(f"Socket error: {e}")
        data = b"error"
    finally:
        # close socket
        #client_socket.close()
        #print("wifi disconnect(send)")
        print("send ok")

    return "OK"


def wifi_listen():
    try:
        print("Wifi connecting...(listen)")
        '''
        # Create a socket
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.settimeout(10)

        # Connect to the server
        client_socket.connect((ESP32_IP, ESP32_PORT))'''

        print("Wifi 200 OK")

        # send keyword to ESP32

        message="\'"
        client_socket.sendall(message.encode())

        print("f0")
        # Receive data from the server
        data = client_socket.recv(1024)

        print("f1-->",data)
        
        print("Received from server:", data.decode())
    
    except socket.error as e:
        print(f"Socket error: {e}")
    finally:
        # close socket
        #client_socket.close()
        print("f2")


def wifi_ecc_connect(message):
    print("Wifi connecting...")
    # creating socket objects
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # connect ESP32
    client_socket.connect((ESP32_IP, ESP32_PORT))
    #print("Wifi is connected !")

    # send data to ESP32
    #message=input("enter string->")
    message+="\t"
    client_socket.sendall(message.encode())

    # receive data from ESP32
    data = client_socket.recv(1024)
    print('Received from ESP32:', data.decode())

    # close socket
    client_socket.close()
    print("wifi disconnect(send)")


# test

'''
while True:
    tt=input("-->")
    print(wifi_connect(tt))

wifi_listen()

listen_thread = threading.Thread(target=wifi_listen)
listen_thread.daemon = True
listen_thread.start()
'''
try:
    while True:
        tt = input("-->")
        print("Sent:", wifi_connect(tt))
        tt=""
except KeyboardInterrupt:
    print("Program interrupted by user")
finally:
    print("Program terminated")
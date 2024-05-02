import socket
import threading

data = {
    "name": "John",
    "age": 30,
    "city": "New York"
}

json_data = json.dumps(data)

# setting the ESP32 address and port number
ESP32_IP = '192.168.4.1'
ESP32_PORT = 1880

# Create a socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# connect ESP32
client_socket.connect((ESP32_IP, ESP32_PORT))
print("wifi 200 OK")


#send data function
def wifi_send():
    print("send")
    while True:
        print("Wifi connecting...")
        data=input("-->")
        data+="\n"

        # send data to ESP32
        client_socket.sendall(data.encode())

        # 傳送 JSON 資料
        client_socket.sendall(json_data.encode())

        # receive data from ESP32
        data_return= client_socket.recv(1024)
        print('Received from ESP32:', data_return.decode())

        # close socket
        client_socket.close()
        print("wifi disconnect(send)")
        data=str(data)


#listen function
def wifi_recv():
    print("listen")
    while True:
        print("Wifi connecting...(listen)")
        message="\'"
        client_socket.sendall(message.encode())

        # receive data from ESP32
        data = client_socket.recv(1024)
        print("Received from server:", data.decode())

        # close socket
        client_socket.close()

#open threading
thread_send=threading.Thread(target=wifi_send)
thread_listen=threading.Thread(target=wifi_recv)

thread_send.start()
thread_listen.start()

thread_send.join()
thread_listen.join()

print('done.')
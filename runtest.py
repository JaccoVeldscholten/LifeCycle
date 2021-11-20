# This Script will call Raspberry Pi 
# That is listening to this port
# If it receives Test Request it will create Test Report


import socket

HOST = ''                   # Raspberry Pi IP
PORT = 65432                # The port used by the Test Server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(b'test_secreykeyunhackable')
    data = s.recv(1024)
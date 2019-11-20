import os
import paramiko
import ctypes
import sys
import time
from socket import *

#ctypes.windll.user32.ShowWindow(ctypes.windll.kernel32.GetConsoleWindow(), 0)

# Declatre the 
host_ip='35.221.82.178'
host_port=22
username='swhatcloud'

# KeyFile path
server = socket(AF_INET, SOCK_STREAM)
server.connect((host_ip,8815))

pkey_path = os.getenv("appdata") + "\\..\\Local\\id_rsa"
remote_path = "/home/swhatcloud/SWHat_WEB/upload/"+sys.argv[1]+"/"
local_path = ".\\"

# creating RSAKey object
key=paramiko.RSAKey.from_private_key_file(pkey_path, password = "12345")

transport = paramiko.Transport((host_ip, host_port))
transport.connect(username = username,pkey = key)
sftp = paramiko.SFTPClient.from_transport(transport)

print("sftp connect success")

# Mkdir
try:
    sftp.chdir(remote_path)
except IOError:
    sftp.mkdir(remote_path)
    sftp.chdir(remote_path)

# Get Json File Name
file_list = []
os.chdir(local_path)
for entry in os.scandir('.'):
    if entry.is_file():
        file_name = entry.name.split('.')
        if file_name[-1] == 'json':
            file_list.append(entry.name)
            print(entry.name)
        

# Upload

for file in file_list:
    sftp.put(local_path+file, remote_path+file)
    time.sleep(3)

# Close
sftp.close()
transport.close()

server.send('Hello!'.encode())
server.close()
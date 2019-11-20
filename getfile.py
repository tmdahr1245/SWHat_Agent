import os
import paramiko
import ctypes
import sys

#ctypes.windll.user32.ShowWindow(ctypes.windll.kernel32.GetConsoleWindow(), 0)

# Declatre the 
host_ip='35.221.82.178'
host_port=22
username='swhatcloud'

# KeyFile path

pkey_path = os.getenv("appdata") + "\\..\\Local\\id_rsa"
print(pkey_path)
remote_path = "/home/swhatcloud/SWHat_WEB/upload/"+sys.argv[1]+"/"+sys.argv[2]
local_path = os.getenv("appdata") + "\\..\\Local\\" + sys.argv[2]

# creating RSAKey object
key=paramiko.RSAKey.from_private_key_file(pkey_path, password = "12345")

transport = paramiko.Transport((host_ip, host_port))
transport.connect(username = username,pkey = key)
sftp = paramiko.SFTPClient.from_transport(transport)
        

sftp.get(remote_path, local_path)
print("file get success");

# Close
sftp.close()
transport.close()

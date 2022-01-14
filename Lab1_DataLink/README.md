# Serial Port Data Protocol
Implementation of a protocol (stop and wait) for serial port data transfer communication between two computers

## Prerequisite

1. Linux Environment (Ubuntu Recommended)
2. Open 2-3 terminals
3. FEUP VPN (For remote connection only)

## Usage:

1. Connect to FEUP VPN
2. Open 2 terminals, on the first terminal, run:
    
    `ssh -t netedu@192.168.109.22`
    
    And, on the second terminal, run:
    
    `ssh -t netedu@192.168.109.22 ssh root@172.16.2.23`
    
3. Head inside the src folder, and run make file:
    
    `make`
    
4. To run the application use the command:
    
    `./application <SerialPort>`
    
    ex.:
    
    `./application /dev/ttyS10`
    
5. On one of the terminal choose to run as *Transmitter* and on the other run as *Receiver*.
    
   *Note: for transmitter it will ask for the name of the file that is going to be transmitted
    

### Optional Steps:

To be able to view and check the file transmitted, follow these steps below to connect to the server's directory:

1. Connect to netedu's server directories, run: 
    
    `ssh -t -t -L 22000:127.0.0.1:22000 netedu@192.168.109.22 "ssh -L 22000:127.0.0.1:22 root@172.16.2.23"`
    
2. Open folder, go to `Other Locations` , on `Connect to Server` insert and run:
    
    `sftp://netedu@192.168.109.22/`
    

You can also test in your remote local machine by creating a virtual serial port with the command:

`sudo socat -d -d PTY,link=/dev/ttyS10,mode=777 PTY,link=/dev/ttyS11,mode=777`

Group Code : 1
Name(s)    : Michael Bido-Chavez (euid: mb0501)
             Garrett Crowe (euid: gc0243)
             Sugn Mo Ahn (euid: sa0156)
             Miguel Martinez (euid: mam0755)
Due Date   : May 4th, 2016
Class      : CSCE 3600
Instructor : Dr. Mark A. Thompson

Usage –
server:
  ./svr <svr_port>
client:
  ./ <rem_ipaddr> <svr_port> <cli1_port> <cli2_port> <cpu_%> <client number>

we added an extra command line arg, to help with program flow. please add 1 or 2 at the end


A particular problem in order to run this –

We had problems getting client 1 to connect to the server, and our only
workaround was to connect both client 1 and 2 with the same argv[1]

example from the terminals on the VM...

server terminal:
./svr 58002
Awaiting connections... Client 1 connected. Client 2 connected.
Both clients connected.
Client 1 Handler Assigned
Client 2 Handler Assigned
[CLIENT 1]: 30
[CLIENT 1]: 40...

client 1:
./cli 192.168.100.181 58002 57032 57033 100 1
Connected. Awaiting response from server...
Awaiting client 2 connection... Client 2 connected.
[CLIENT] – Enter data to send: 30
[SERVER] – Total: 30
[CLIENT] – Enter data to send: 40
[SERVER] – Total: 70...

client 2:
./cli 192.168.100.181 58002 57032 57033 5 2
Connected. Awaiting response from server...
[CLIENT] – Enter data to send: 50
CPU Utilization: 0.15 percent. threshold 5.00 percent exceeded...

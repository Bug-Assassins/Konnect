Konnect
=======

A mini TCP/IP Protocol for file transfer from one system to the other.

This project was done as a part of Computer Communication and Networks Course (4th semester B.Tech) at Department of Information Technology, National Institute of Technology, Karnataka, Surathkal

<h3>Installation</h3>

The 4 file simulates the function of each of the 4 layer. The same code (all 4 files) should be present in both sender and receiver machine. On sender machine the sender.sh script should be executed with 1st argument as the path of the file to be transferred and the second argument should be the IP address of the destination machine. The destination machine should execute receiver.sh wit only one one argument which is the path of the received file. Receiver side should be executed first followed by the sender side.

<b>Example</b>

sh receiver.sh received_data.txt (on destination machine)
sh sender.sh data 10.100.50.32 (on sender machine)

<h3>Implementation Details</h3>

The objective behind this project was to learn the fundamentals of the computer networks and socket programming in C. We have implemented all the 4 layers. Each layer adds the corresponding header and writes the data to a temporary file which is passed to the next layer. 3-way Handshaking in Transport layer has been simulated using socket programming. In MAC layer all the data is kept in a single frame and that frame is sent through socket once again.

<h3>Contributors</h3>

The project was made by team Bug Assassins </br></br>

1. Ashish Kedia (ashish1294@gmail.com)</br>
2. Ajith P S (ajithpandel@gmail.com)</br>
3. Adarsh Mohata (amohta163@gmail.com)</br>
# Network Programming Project 1 - Compression Link

## Overview

We will implement a network level compression link. We will then implement a network application
to detect whether network compression is present to validate our simulated compression link. It is inspired by the
work, End-to-End Detection of Compression of Traffic Flows by Intermediaries.

## Installation/Usage Guide
Refer to [Technical Manual](Technical_Manual.md)

 
## Project Outcomes
1. Enable network compression for point-to-point links in ns-3.
2. Implement the network application that detects the presence of network compression by end-hosts.
3. Verify and validate your simulated compression link and compression detection application.


## Components

### (1) Compression Link
Our compression link application is built using ns-3. It will be responsible for compression and decompression
of incoming and outgoing packets. We use the Lempel-Ziv-Stac algorithm for compression and decompression. Our compression link follows the requirements in the RFC 1974: PPP Stac LZS Compression Protocol.

### (2) Compression Detection Application
Our network application is a client/server application where the sender sends two sets
of 6000 UDP packets back-to-back (called packet train), and the receiver records the arrival time between the first
and last packet in the train. The first packet train consists of all packets of size 1100 bytes in payload, filled with
all 0’s, while the second packet train contains random sequence of bits. You can generate random sequence of bits
using /dev/random.

### (3) Simulation Verification and Validation
We created a 4-node topology in ns-3 as illustrated in Figure 1. Nodes S and R are the end-hosts running the network
application. Nodes R1 and R2 are the intermediate routers where the link between them is compression-enabled.
We include four logically separate simulations, each doing one of the following:   
• Transmit low entropy data over a network topology without a compression link.   
• Transmit high entropy data over a network topology without a compression link.   
• Transmit low entropy data over a network topology with a compression link.   
• Transmit high entropy data over a network topology with a compression link.   

## Collaborators 
Kei Fukutani - kayfuku   
Domingo Haung - DomingoWong   
Tae Lee - leeth7830  

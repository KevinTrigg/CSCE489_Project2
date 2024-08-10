# AFIT-CSCE489-PROJ2 -KevinTrigg
Project 2 is a single producer multi-consumer program that produces an input number of yoda toys that each take up one space on an input number of shelves while an input number of consumers rush to remove these toys off the shelves. This program prints the current actions of the threads to the terminal for the user to watch.  
Occasionally race conditions break a consumer thread and they get stuck at "Consumer X wants to buy a yoda..." when a large number of consumers and items are processed. I could not figure out why this race happens or how to stop it... If you don't see it, it must not exist.

## Instructions for running babyyoda

### 1. download the files
### 2. navigate to the CSCE489_Project2 folder that was downloaded, may be labeled CSCE489_Project2-master
### 3. run the 'make' command in terminal
### 4. run the program using './babyyoda \<bufferSize\> \<numConsumers\> \<maxItems\>'
#### Note: 
####      bufferSize   - (int) how many shelves there are to hold toys 
must fulfill 1<=buffer<=256 typically the consumers take significantly less time to take items off the shelves than it takes the producer to put them on so a high number of shelves is redundant
####      numConsumers - (int) how many consumers will enter the store
must fulfill 1<=numConsumers<=256 WARNING high numbers of consumers clogs up the terminal and maks it hard to follow
####      max_items    - (int) how many items will be produced before the shopkeeper closes 
must fulfill 1<=maxItems<=1000 WARNING inputting a high number of items takes a significantly longer time to complete

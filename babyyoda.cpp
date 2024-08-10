/*************************************************************************************
 * babyyoda - used to test your semaphore implementation and can be a starting point for
 *			     your store front implementation
 *
 *************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "Semaphore.h"

// Semaphores that each thread will have access to as they are global in shared memory
Semaphore *empty = NULL; //number of shelves with items
Semaphore *full = NULL; //number of empty shelves
const int timeInterval=100000;// time interval global for easier modification of code duration

pthread_mutex_t shelf_buffers[256];  //max possible shelves
int shelves[256]; //max number of shelves

int buffer = 1; //# of shelves
int consumed = 0;// number of toys taken
bool soldOut=false; //flag for when all toys have been bought
/*************************************************************************************
 * producer_routine - this function is called when the producer thread is created.
 *
 *			Params: data - a void pointer that should point to an integer that indicates
 *				       the total number to be produced
 *
 *			Returns: always NULL
 *
 *************************************************************************************/

void *producer_routine(void *data) {//,void *shelfSizepthread_join

	time_t rand_seed;
	srand((unsigned int) time(&rand_seed));

	// The current serial number (incremented)
	unsigned int serialnum = 1;
	
	// We know the data pointer is an integer that indicates the number to produce
	int left_to_produce = *((int *) data);
	
	int bufferSize =  buffer;  //(int)(intptr_t) shelfSize;

	// Loop through the amount we're going to produce and place into the buffer
	while (left_to_produce > 0) {
		//printf("  Producer wants to put Yoda #%d into buffer...\n", serialnum);
                
		// Semaphore check to make sure there is an available slot
		full->wait();
                //loop through shelves to see which has an open slot to put an item
                for(int i=0;i<bufferSize;i++){

                    if (shelves[i]==0){ //if THAT shelf doesn't have something then we can place a toy
                        pthread_mutex_lock(&shelf_buffers[i]);  //lock the shelf
                        shelves[i]=serialnum; //place the toy
                        printf("   Yoda #%d put on shelf %d.\n",serialnum, i);//let user know
                        serialnum++;  //increment for the next toy
                        left_to_produce--; //one less to place
                        pthread_mutex_unlock(&shelf_buffers[i]); //unlock the shelf
                        break; //if we find a spot to place we don't need to keep going through the shelves
                      } 
                      
                } //while full is >1 (wait canceled) there IS a spot open      
                	
		// Semaphore signal that there are items available
		empty->signal();

		// random sleep but he makes them fast so 1/20 of a second
		usleep((useconds_t) (rand() % (timeInterval/20)));
	}
	return NULL;
}


/*************************************************************************************
 * consumer_routine - this function is called when the consumer thread is created.
 *
 *       Params: data - a void pointer that should point to a boolean that indicates
 *                      the thread should exit. Doesn't work so don't worry about it
 *                    - actually used to pass the consumer ID into the function
 *
 *       Returns: always NULL
 *
 *************************************************************************************/

void *consumer_routine(void *data) {   //void* shelfSize 
      int consumerNum = ((int)(intptr_t) data); //number of the consumer from input
      int bufferSize = buffer;//(int)(intptr_t) shelfSize; //made a global
      // Consumers wait up to one second
    
      while(!soldOut){  //while there are still toys to buy
         
        printf("Consumer %d wants to buy a Yoda...\n",consumerNum);
	// Semaphore to see if there are any items to take
	empty->wait();//wait for a toy to be placed (or for the condition of soldOut to be completed
        
        for(int i=0;i<bufferSize;i++){  //go through all the shelves
          pthread_mutex_lock(&shelf_buffers[i]);// lock the shelf
         // printf("Consumer %d examined shelf %d.\n",consumerNum,i); 
          if (shelves[i]!=0){ // if the shelf has contents
           // pthread_mutex_lock(&shelf_buffers[i]);// lock the shelf
            printf("Consumer %d bought Yoda #%d from shelf %d.\n",consumerNum, shelves[i],i); //print to user
            shelves[i]=0;//remove the toy from the shelf
            consumed++;//increment number consumed
           // printf("consumed %d items\n",consumed);
            pthread_mutex_unlock(&shelf_buffers[i]); //unlock the shelf
            break; 
            //if you find a toy you don't greedily take EVERY toy on the shelves (unless you still have focus and go a second time around)
          } 
          pthread_mutex_unlock(&shelf_buffers[i]); //unlock the shelf
          
        }
      usleep((useconds_t) (rand() % (timeInterval))); // checkout at the register takes time
      full->signal(); // a new shelf has become open
       
     
      }
      printf("Consumer %d goes home.\n", consumerNum); //end of sales, go home
      return NULL;	//end the thread with return
}


/*************************************************************************************
 * main - Standard C main function for our storefront. 
 *
 *		Expected params: fileName <bufferSize> <numConsumers> <maxItems>
 *                      (+int)   bufferSize   - how many shelves there are to hold toys 
 *			(+int)	numConsumers - how many consumers will enter the store
 *                      (+int)   max_items    - how many items will be produced before the shopkeeper closes
 *************************************************************************************/

int main(int argv, const char *argc[]) {
	// Get our argument parameters
	if (argv < 4) {
		printf("Invalid parameters. Format: %s <numberShelves> <numConsumers> <maxItems> \n", argc[0]);
		exit(0);
	}
	buffer=(unsigned int) strtol(argc[1], NULL, 10); //number of shelves to be made
	if (buffer < 1 || buffer>256) {
		printf("Invalid parameters. # of shelves must fulfill 1<=buffer<=256 \n");
		exit(0);
	}
        int numConsumers = (unsigned int) strtol(argc[2], NULL, 10); //number of consumers entering store
        if (numConsumers >256 || numConsumers <1) {
		printf("Invalid parameters. # of consumers must fulfill 1<=numConsumers<=256\n");
		exit(0);
	}
        int maxItems= (unsigned int) strtol(argc[3], NULL, 10); // number of items to be produced
        if (maxItems > 1000 || maxItems<1) {
		printf("Invalid parameters. # of max items must fulfill 1<=maxItems<=1000 try to not take too much time\n");
		exit(0);
	}
	printf("Producing %d today.\n", maxItems);
	
	// Initialize our semaphores
	empty = new Semaphore(0);//number of shelves with items
	full = new Semaphore(buffer); //number of placeable shelves
	
        for(int i=0;i<buffer;i++){
          pthread_mutex_init(&shelf_buffers[i],NULL);
        }
        //define the producer and consumer threads
	pthread_t producer;
      	pthread_t consumer[numConsumers];
        
	// Launch our producer thread
	pthread_create(&producer, NULL, &producer_routine, (void *) &maxItems); 

	// Launch our consumer threads
	for(int i=0;i<numConsumers;i++){
	  pthread_create(&consumer[i], NULL, &consumer_routine, (void *)(intptr_t) i);
        }
        //https://stackoverflow.com/questions/33060730/how-to-corr ectly-pass-an-argument-to-a-pthread
        
	// Wait for our producer thread to finish up
	pthread_join(producer, NULL);
        //end of producer's day
	printf("The manufacturer has completed his work for the day.\n");
        if(consumed < maxItems){
	  printf("Waiting for consumer to buy up the rest.\n");
	}
      	// wait until all are consumed 
	while (consumed < maxItems){
		sleep(1);
	}
	soldOut=true; //post that consumers don't need to stay in their while
	printf("no more toys\n");
        //end if all consumers are gone -print # of toys remaining
        //end if all toys are bought - make sure all remaining consumers "leave" the store,
        
	for (int i=0; i<numConsumers; i++) {
	        empty->signal(); 
	        //clear all the consumers out of the wait buffer (and makes sure if they haven't reached the wait yet they WILL be removed
	        //empty->condFilled(); //prior method that had a race condition between this completing before the consumer entering wait
	}
        for (int i=0; i<numConsumers; i++) {
        printf("completed consumer %d\n",i);
         	pthread_join(consumer[i],NULL);
         	
	} //separate loops so that EVERY consumer gets the update before waiting. this fixes a race condition
	// We are exiting, clean up
	delete empty;
	delete full;		

	printf("Producer/Consumer simulation complete!\n");
        return 0; //safe return from main
}

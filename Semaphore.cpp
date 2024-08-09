#include <pthread.h>
#include "Semaphore.h"

// useful stackOverflow Q on semaphore class creation
//https://stackoverflow.com/questions/9854470/implementing-a-binary-semaphore-class-in-c
//https://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_mutex_lock.html 
/*************************************************************************************
 * Semaphore (constructor) - this should take count and place it into a local variable.
 *						Here you can do any other initialization you may need.
 *
 *    Params:  count - initialization count for the semaphore
 *
 *************************************************************************************/

//sem_t sem;  //test semaphore for proof of use whenever I was having issues
int count;
pthread_mutex_t mutex; //mutex to make atomic lock
pthread_cond_t  condition; //check condition for when the semaphore should change
Semaphore::Semaphore(int count) {
  //sem_init(&this->sem,0,count);//initialize a global semaphore with count as the number
  this->count=count;
}


/*************************************************************************************
 * ~Semaphore (destructor) - called when the class is destroyed. Clean up any dynamic
 *						memory.
 *
 *************************************************************************************/
//https://stackoverflow.com/questions/16090936/do-i-need-to-delete-basic-data-types-in-a-destructor-c 
Semaphore::~Semaphore() { 
//sem_destroy(&this->sem);  //delete this semaphore object
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&condition);
//don't need to destroy count
}


/*************************************************************************************
 * wait - implement a standard wait Semaphore method here
 *
 *************************************************************************************/

void Semaphore::wait() {  //enter queue for resource
 //sem_wait(&this->sem);
 pthread_mutex_lock(&mutex); // nothing else should use this while another is using it
 if(this->count<=0){ //while there is no open slot
   pthread_cond_wait(&condition,&mutex); //wait for the condition to be fulfilled
  } //while broken when there's an open slot
  this->count--;//take one from the open slot
  pthread_mutex_unlock(&mutex); //release the lock so others can operate when gone
}


/*************************************************************************************
 * signal - implement a standard signal Semaphore method here
 *
 *************************************************************************************/

void Semaphore::signal() { //signal end of use of the resource
  //sem_post(&this->sem);
  pthread_mutex_lock(&mutex);// lock the semaphore down while in use
  this->count++; //increment the count when resource is given up
  if(this->count>0){
    pthread_cond_signal(&condition);
  }
  pthread_mutex_unlock(&mutex); //release the lock so others can operate when gone
}

//void Semaphore::condFilled(){ //this introduced a race condition between the call and the wait() so it's gone now
//  pthread_cond_signal(&condition);//break the condition so the threads clear out without influencing the count
//}

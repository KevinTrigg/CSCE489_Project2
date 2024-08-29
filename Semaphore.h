
#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include <semaphore.h>
class Semaphore 
{
public:
    //  sem_t sem;
      int count;
      pthread_mutex_t mutex; //mutex to make atomic lock
      pthread_cond_t  condition; //check condition for when the semaphore should change
      
      Semaphore(int count);
      ~Semaphore();

      void wait();
      void signal();
      //void condFilled();

private:

};

#endif

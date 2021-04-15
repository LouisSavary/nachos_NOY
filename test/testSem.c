#include "userlib/syscall.h"
#include "userlib/libnachos.h"

#define loop 10

SemId sem;
SemId sem_end;
void testP(){
    int i;
    for(i = 0; i < loop; i++){
        P(sem);
        n_printf("P %d\n", i);
    }

    V(sem_end);
}

void testV(){
    int j;
    for(j = 0; j < loop; j++){
        n_printf("V %d\n", j);
        V(sem);
    }
}


int main() {
    SemId id = SemCreate("ouiSem", 1);
    P(id);
    n_printf("pause inter sem\n\n");
    V(id);
    SemDestroy(id);
    
    sem     = SemCreate("testSem_sem", 0);
    sem_end = SemCreate("testSem_end", 0);
    threadCreate("thread_p", testP);
    threadCreate("thread_v", testV);
    P(sem_end);
    
    SemDestroy(sem);
    n_printf("fin testSem\n");
    return 0;
    
}
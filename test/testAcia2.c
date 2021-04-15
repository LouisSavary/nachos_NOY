#include "userlib/syscall.h"
#include "userlib/libnachos.h"

int main() {
    
    char msg[10];
    TtyReceive(msg, 10);
    n_printf("%s\n", msg);
}
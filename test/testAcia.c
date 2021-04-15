#include "userlib/syscall.h"
#include "userlib/libnachos.h"



int main() {
    
    char str_msg[] = "TTY test";
    int nb_char = TtySend(str_msg);
    if (nb_char != n_strlen(str_msg)+1)
        n_printf("write: test echoue - i=%d\n", nb_char);
    else
        n_printf("write: test reussi - i=%d\n", nb_char);
    
}
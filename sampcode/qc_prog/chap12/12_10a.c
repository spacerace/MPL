#include "texed.h"
menu()
{
    struct key_struct *kp, *Read_kbd();
    int cur_key, cur_move;
    
    kp = Read_kbd();
    cur_key = kp->key;
    cur_move = kp->move;
    if (cur_key == ERROR)
        return (cur_move);
    return (cur_key);
}

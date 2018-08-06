#include "texed.h"
struct key_struct *Read_key()
{
    struct key_struct k;

    k.key = getch();
    if (k.key == ERROR)
    k.move = getch();
        return (&k);
}

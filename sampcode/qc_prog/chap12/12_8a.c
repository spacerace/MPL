#define OK 1
#define ERROR 0
menu()
{
    struct key_struct {
        char key;
        unsigned char move;
    } *kp, *Read_kbd();
    int cur_key, cur_move;

    kp = Read_kbd();
    cur_key = kp->key;
    cur_move = kp->move;
    if (cur_key == ERROR)
        return (cur_move);
    return (cur_key);
}

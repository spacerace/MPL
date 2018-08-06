#define OK 1
#define ERROR 0
struct key_struct {
    char key;
    unsigned char move;
};

struct key_struct *Read_key()
{
    struct key_struct k;

    k.key = getch();
    if (k.key == ERROR)
        k.move = getch();
    return (&k);
}

/* scrmenu.c  -- uses bit fields to modify your text   */
/*               screen's attributes                   */

char *Choice_Words[] = {
    "Quit",
    "Foreground",
    "Intensity",
    "Background",
    "Blinking"
};
enum Choices { 
    Quit, 
    Foreground, 
    Intensity, 
    Background, 
    Blinking 
};

/* use 0xB800000 for EGA or VGA */
#define SCR_START (0xB0000000)
#define SCR_SIZE (25 * 80)

main()
{
    enum Choices choice;

    printf("Select from the following by number:\n");

    for (choice = Quit; choice <= Blinking; ++choice)
        {
        printf("%d. %s\n", choice, Choice_Words[choice]);
        }

    printf("\nWhich: ");
    do 
        {
        choice = getch();
        choice -= '0';
        if (choice < Foreground || choice > Blinking)
            continue;
        Redraw(choice);
        } while (choice != Quit);

}

Redraw(enum Choices field)
{
    struct screen_char {
        unsigned int character  :8,
                     foreground :3,
                     intensity  :1,
                     background :3,
                     blink      :1;
    } scrchar, far *sp, far *ep;

    sp = (struct screen_char far *)SCR_START;
    ep = sp + SCR_SIZE;

    while (sp < ep)
        {
        scrchar = *sp;
        switch (field)
            {
            case Foreground:
                scrchar.foreground = (scrchar.foreground)? 0 : 7;
                break;
            case Intensity:
                scrchar.intensity = (scrchar.intensity)? 0 : 1;
                break;
            case Background:
                scrchar.background = (scrchar.background)? 0 : 7;
                break;
            case Blinking:
                scrchar.blink = (scrchar.blink)? 0 : 1;
                break;
            }
        *(sp++) = scrchar;
        }
}

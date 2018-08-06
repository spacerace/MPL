/* external.c -- shows an external variable */

#define PI 3.14159
int length; /* external (global) variable */
            /* declared before main ()    */

main()
    {
    void square(), triangle(), circle();

    printf("What length do you want to use? ");
    scanf("%d", &length);

    square();   /* calculate areas */
    triangle();
    circle();
    }

void square()
    {
    float area;
    area = length * length;
    printf("A square with sides of %d has an area of %f\n",
            length, area);
    }

void triangle()
    {
    float area;
    area = (length * length) / 2;
    printf("A right triangle with sides of %d has an area %f\n",
            length, area);
    }

void circle()
    {
    float area;
    area = (length * length * PI);
    printf("A circle with radius of %d has area of %f\n",
            length, area);
    }

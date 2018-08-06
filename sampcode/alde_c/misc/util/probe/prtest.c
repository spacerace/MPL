/*			PRTEST.C
	Test program for the program PROBE
		Author: Colin Stearman [71036,256]
*/

long i;
main()

{
printf("test1\n");
test1();
printf("test2\n");
test2();
printf("test3\n");
test3();
printf("test4\n");
test4();
printf("test5\n");
test5();
}

test1()

{
for (i=0; i<10000; i++);
}

test2()

{
for (i=0; i<20000; i++);
}
test3()

{
for (i=0; i<30000; i++);
}
test4()

{
for (i=0; i<40000; i++);
}
test5()

{
for (i=0; i<50000; i++);
}
test6()

{
for (i=0; i<100; i++);
}

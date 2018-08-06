/* fields.c -- shows the same number with different */
/*             field widths and number of decimals  */

main()
{
    float f = 123.4560;

    printf("%12.6f\n", f);
    printf("%8.4f\n", f);
    printf("%8.3f\n", f);
    printf("%8.2f\n", f);
}

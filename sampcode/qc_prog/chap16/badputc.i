/* badputc.I -- preprocessed putfile.c          */
/*  Not shown are all the stdio.h contents that */
/*  come at the top of the file.                */

main()
{
    struct _iobuf *fp;
    int ch;
    if ((fp = fopen("junk", "w")) == 0)
        exit(1);

   /* original was while ((ch = getchar()) != EOF) */
    while ((ch = (--((&_iob[0]))->_cnt >= 0 ? 0xff &
       *((&_iob[0]))->_ptr++ : _filbuf((&_iob[0]))))
       != (-1))  

   /* original was putch(fp, ch); */  
        (--(ch)->_cnt >= 0 ? 0xff &
	(*(ch)->_ptr++ = (fp)) : _flsbuf((fp),(ch)));

    fclose(fp);
}

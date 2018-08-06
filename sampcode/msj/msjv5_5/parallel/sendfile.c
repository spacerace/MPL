/***************************
 sendfile.c

 Ross M. Greenberg

 Thursday, August 2, 1990
 Microsoft C 5.1
 **************************/

#include   <stdio.h>
#include   <fcntl.h>
#include   <dos.h>
#include   <sys/types.h>
#include   <sys/stat.h>
#include   <io.h>

#define FALSE 0
#define TRUE  1
#define REC_SIZE (30 * 1024)
#define S_SIZE   REC_SIZE

char file_name[128];
#define BIG_BUF_SIZE   (30 * 1024)

char big_buffer[BIG_BUF_SIZE + 1024];
void send_control();
void get_control();

/********************************************************************
* do_sync(port)  -
*
* Syncing up two machines where either can be master or slave is a
* bit of a kludge. The 1st machine to *not* get a character sends an
* uppercase 'I'. When a machine receives an uppercase 'I' it responds
* with a lowercase 'i'.  If a machine gets a lowercase 'i', it will
* respond with an uppercase 'I'.  This continues until there are a
* total of three transactions.  Then both machines start sending 'x'
* until it gets one back or, out of exhaustion from this kludge, the
* user hits a key.
********************************************************************/

void
do_sync(int port)
{
char c = 0;
int  stat;
int  cnt = 0;

  printf("Syncing machines...\n");
  pinit(port);


  do
  {
     stat = prec(&c, 1);
     printf(stat == 1 ? "%c" : "-", c);
     if (stat != 1)
     {
        psend("I", 1);
     }
     else
     switch (c)
     {
        case 'I':
           psend("i", 1);
           cnt++;
           break;

        case 'i':
           psend("I", 1);
           cnt++;
           break;

        case 'x':
           psend("x", 1);
           cnt++;
           break;

        default:
           printf("(%c)", c);
           break;
     }
  }
  while (cnt < 4 && !kbhit());
  do
  {
     prec(&c, 1);
     psend("x", 1);
  }
  while (c != 'x' && !kbhit());

  /* gobble up any spare characters. sleep(1) in main gulps this */
  while (prec(&c, 1) > 0)
     ;
}


/********************************************************************
* main()   -
*
* Get the port (0,1,2...n)
* Get status as sender or receiver
* If Sender:
*    Get file name and make sure file exists.
*    Sync up the two machines and their ports.
*    Hangout for one second to gobble routine in do_sync eats
*      nothing of value.
*    Open the file, send a rather verbose message on the filename.
*      these messages could easily be reduced to single control
*      bytes.  Send CAN message if can not open file.
*    Get remote status of file open.  If not okay, fail
*    Get file length and send it across in ASCII
*    Get OK status from remote
*    Get start time.
*    Send a large block across, continue until file is exhausted.
*     Get status information for each block sent.
*    When done, get end time, calculate bytes per second, and exit.
*
* If Receiver:
*    Sync
*    Get filename and filesize, display them
*    Get start time
*    Try to read a big block.  Hi bit set indicates a short block
*      If short:
*         Strip off the hi bit, to get number of bytes received.
*                    If no bytes, display a '?' for timeout.
*      If bytes:
*         Add them to buffer, when buffer size is ready, write
*         to disk, send an okay message, and (remember ASCOM?)
*         display a '*' for each block.
*    Loop until all bytes received
*    Write a final block if required
*    Close the file.
*    When done, get end time, calculate bytes per second, and exit.
********************************************************************/

void
main()
{
char buf[128];
int  port;
int  cnt = 0;
unsigned stat;
char *p;
int  fd;
long tot_bytes = 0;
long file_size = 0;
long get_time();
long time1,
  time2;
char c;

  printf("What port? (LPT1 = 0):");
  gets(buf);
  port = atoi(buf);
  do
  {
     printf("\n(S)end|(R)eceive:");
     c = getch();
     c = toupper(c);
  }
  while (c != 'S' && c != 'R');

  printf("\n");
  if (c == 'S')
  {
     do
     {
        printf("Enter filename:");
        gets(file_name);
     }
     while (access(file_name, 0));

     do_sync(port);
     sleep(1);
     prec(big_buffer, 1);
     printf("\nSender Synced\n");
     if ((fd = open(file_name, O_RDONLY | O_BINARY)) == -1)
     {
        printf("Can't open file: %s\n", file_name);
        send_control("CAN", 0, 0, 0);
        exit(1);
     }
     send_control("FILE:%s", file_name, 0, 0);
     get_control(buf);
     if (strcmp(buf, "OK"))
     {
        printf("Error on Remote: %s\n", buf);
        exit(1);
     }

     file_size = lseek(fd, 0L, 2);
     sprintf(buf, "SIZE:%lu", file_size);
     send_control(buf, 0, 0, 0);
     get_control(buf);
     if (strcmp(buf, "OK"))
     {
        printf("Error on Remote: %s\n", buf);
        exit(1);
     }

     lseek(fd, 0L, 0);
     time1 = get_time();
     while((cnt = read(fd, big_buffer, BIG_BUF_SIZE)))
     {
        psend(big_buffer,cnt);
        tot_bytes += (long)cnt;
        printf("*");
        get_control(buf);
     }
     time2 = get_time();
  }
  else
  {
     do_sync(port);
     prec(big_buffer, 1);
     printf("\nReceiver Synched\n");
     get_control(buf);
     if (!strcmp(buf, "CAN"))
     {
        printf("Remote cancels\n");
        exit(1);
     }
     sscanf(buf, "FILE:%s", file_name);
     if ((fd = open(file_name, O_CREAT | O_WRONLY | O_BINARY,
                               S_IREAD | S_IWRITE)) == -1)
     {
        printf("Can't open file: %s\n", file_name);
        send_control("CAN", 0, 0, 0);
        exit(1);
     }
     send_control("OK", 0, 0, 0);
     get_control(buf);
     sscanf(buf, "SIZE:%lu", &file_size);
     printf("File Size for `%s' is: %ld\n", file_name, file_size);
     tot_bytes = cnt = 0;
     p = big_buffer;
     send_control("OK", 0, 0, 0);
     time1 = get_time();
     while (tot_bytes < file_size)
     {
        stat = prec(p, REC_SIZE);
        if (stat & 0x8000)
           stat &= 0x7fff;
        if (stat > 0)
        {
           p += stat;
           tot_bytes += stat;
           cnt += stat;
           if (cnt >= BIG_BUF_SIZE)
           {
              write(fd, big_buffer, cnt);
              printf("*");
              send_control("OK", 0, 0, 0);
              cnt = 0;
              p = big_buffer;
           }
        }
        else
           printf("?");
     }
     if (cnt)
     {
        write(fd, big_buffer, cnt);
        printf("*");
        send_control("OK", 0, 0, 0);
        cnt = 0;
     }
     close(fd);
     time2 = get_time();
  }
  printf("\n\nNu}_ber of characters transmitted: %ld\n", tot_bytes);
  printf("Number of Ticks to Transmit: %ld\n", time2 - time1);
  printf("Characters per second: %ld\n",
         (tot_bytes/(time2 - time1)) * 18L);
}

/********************************************************************
* send_control(format, var1...varn)
*
* If the format string has a %, do an sprintf into the local stack
* buffer.  Send the control string, followed by a trailing null.
********************************************************************/

void
send_control(char *ptr, int var1, int var2, int var3)
{
char buf[128];
char *p;
char c = 0;

  if (strchr(ptr, '%'))
  {
     sprintf(buf, ptr, var1, var2, var3);
     p = buf;
  }
  else
     p = ptr;

  psend(p, strlen(p));
  psend(&c, 1);
}

/********************************************************************
* get_control(ptr)
*
* stuffs characters into buffer @ptr until it receives trailing NULL
********************************************************************/
void
get_control(char *ptr)
{
int  stat;
char *ptr1 = ptr;

  while (!kbhit())
  {
     stat = prec(ptr, 1);
     if (stat <= 0)
        continue;
     if (!*ptr)
        return;
     ptr++;
  }
}


/********************************************************************
* get_time();
*
* Using INT 0x1a, returns the number of ticks since birth
********************************************************************/

long
get_time()
{
union   REGS  regset;

  regset.x.ax = 0;
  int86(0x1a, &regset, &regset);
  return(((long) regset.x.cx << 16) | (long)regset.x.dx);
}

/********************************************************************
* sleep();
*
* Eat up time since sleep() is not in library
********************************************************************/

sleep(int seconds)

{
long time1 = get_time();

  while ((get_time() - time1) <= (long)seconds * 18L )
     ;
}

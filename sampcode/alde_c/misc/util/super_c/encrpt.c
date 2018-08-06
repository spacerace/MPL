/*                      Encrypt

        This program encrypts the standard input and writes it to
        the standard output. It requires one parameter: the key for
        the encryption, which can be up to eight characters long.
        Decryption is done by simply running the program again on the
        encrypted file with the same key -- i.e., encryption and
        decryption are the same operation.
*/

#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#define BUFSIZE 20480    /* I/O buffer size. */
#define KEYSIZE 9       /* Maximum size of key (plus one for null). */

#define CALIBRATE       /* Define for start/end prompts. */

char buffer[BUFSIZE];   /* The I/O buffer. */

#define STDINFILE 0     /* Standard input non-stream file number. */
#define STDOUTFILE 1    /* Standard output file number. */

#define TRUE 1
#define FALSE 0

/*      main(argc,argv)

        Function: Encrypt the standard input and write it to the
        standard output, using the single command line parameter
        as the key.

        Algorithm: Get the key and rearrange it to improve security,
        then encrypt using exclusive-or with the key.
*/

main(argc,argv)

int argc;
char *argv[];

{
        char key[KEYSIZE];      /* The key. */
        char *cp, *cp2;
        char c;

        /* Check if we've got the right number of command line
           parameters. */
        if (argc != 2) {
                /* If not, remind him how to use this program. */
                fputs("Usage: encrpt key <plainText >cipherText",stderr);
                exit(1);
        };

        /* Check if we've got a decent sized key. */
        if ((argv[1][0] == 0) || (argv[1][1] == 0)) {
                /* If not, tell him he needs a bigger key. */
                fputs("Key must be at least 2 characters in size.",stderr);
                exit(1);
        };

        /* Get the key. */
        for (cp = key, cp2 = argv[1];
             (cp < &key[KEYSIZE-1]) && (*cp2 != 0); *cp++ = *cp2++);
        /* Distribute the last byte amoung the top bits of the earlier
           ones, and toggle some of the bits in each key byte. All of
           this helps ensure a more secure key. */
        for (c = *(--cp), *cp = 0; cp >= key; cp--) {
                *cp |= (c & 1) << 7;
                *cp ^= 0x55;
                c >>= 1;
        };

        /* Use binary I/O mode. For two reasons: (1) We want to be
           able to encrypt binary files; (2) characters may be
           turned into LFs in the encryption process, which would
           erroneously be translated into CRLF. */
        setmode(STDINFILE,O_BINARY);
        setmode(STDOUTFILE,O_BINARY);

        /* Encrypt it. */
#ifdef CALIBRATE
        fputs("Starting...",stderr);
#endif CALIBRATE
        encrypt(STDINFILE,STDOUTFILE,key);
#ifdef CALIBRATE
        fputs("done.\n",stderr);
#endif CALIBRATE
}

/*      encrypt(inFile,outFile,theKey)

        Function: Encrypt the file specified by inFile, using
        theKey, and write it to outFile.

        Algorithm: Exclusive-or the bytes with theKey, recycling
        theKey as needed.
*/

encrypt(inFile,outFile,theKey)

int inFile;
int outFile;
char *theKey;

{
        register char *keyPtr;
        register char *bufPtr;
        int bufCnt;
        int sizeRead;

        keyPtr = theKey;
        /* Cycle until nothing left to read. */
        while (TRUE) {
                /* Read in a bufferfull. */
                sizeRead = bufCnt = read(inFile,buffer,BUFSIZE);
                if (sizeRead <= 0) break;
                /* Encrypt it. */
                for (bufPtr = buffer; sizeRead-- > 0; *bufPtr++ ^= *keyPtr++)
                        if (*keyPtr == 0) keyPtr = theKey;
                /* Write it out. */
                write(outFile,buffer,bufCnt);
        };
}


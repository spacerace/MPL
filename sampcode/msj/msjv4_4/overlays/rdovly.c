#include "exehdr.h"

/* externally-defined full pathname of executable */

#define min(a,b) ((a) < (b)) ? (a) : (b)
#define max(a,b) ((a) < (b)) ? (a) : (b)
#define OPENMODE_RO 0
#define MK_FP(a,b) ((void far *)( ((unsigned long)(a) << 16) | (unsigned)(b) ))

extern char executable_name[];
extern int pspaddr;

void errputs(char far *s);
long mylseek(int handle, long offset, int type);
int myread(int handle, void far *buf, int len);
int myopen(char far *name, int mode);
int myclose(int handle);


int read_overlay_section(
void far *ovarea,                   /* buffer into which to read him */
int requested_overlay_number        /* number of overlay to read */
)

#define RELOC_BUF_SIZE 32
{
    struct reloc_entry_ reloc_buf[RELOC_BUF_SIZE];
    struct reloc_entry_ *reloc_ptr;
    int reloc_chunk;
    unsigned far *target_ptr;
    struct exehdr_ eh;
    int i;
    long startpos, nextpos, filesize;
    long sectionsize, imagesize;
    int executable_handle;

    /* open executable */
    executable_handle = myopen(executable_name,OPENMODE_RO);

    /* determine file size */
    filesize = mylseek(executable_handle,0L,2);

    /* search from beginning of file */
    mylseek(executable_handle,0L,0);

    while (1) {
        /* use mylseek() to avoid calling runtime functions */
        startpos = mylseek(executable_handle,0L,1);
        if (myread(executable_handle,&eh,sizeof(eh)) != sizeof(eh)) {
            errputs("Something's wrong...can't read .EXE header\r\n");
            myclose(executable_handle);
            return(1);
	    }

	    if (eh.M_sig != 'M' || eh.Z_sig != 'Z') {
	        errputs("Found non-EXE signature!\r\n");
            myclose(executable_handle);
	        return(1);
	    }
        if (eh.remain_len == 0)
            sectionsize = (long)eh.page_len * 512;
        else
            sectionsize = (long)(eh.page_len - 1) * 512 + eh.remain_len;

        if (eh.overlay_number == requested_overlay_number) {
            /* found ours...load and fix up */

            /* move to executable image */
            mylseek(executable_handle, startpos + eh.hsize * 16, 0);

            myread(executable_handle, ovarea, (int)(sectionsize - eh.hsize * 16));

            /* fix up relocations in the loaded overlay */

            mylseek(executable_handle,startpos + (long)eh.first_reloc,0);
            while (eh.num_relocs) {
                reloc_chunk = min(RELOC_BUF_SIZE,eh.num_relocs);
                myread(executable_handle,reloc_buf,reloc_chunk * sizeof(struct reloc_entry_));
                eh.num_relocs -= reloc_chunk;
                for (i = 0; i < reloc_chunk; i++) {
                    reloc_ptr = reloc_buf + i;
                    target_ptr = MK_FP(pspaddr + 0x10 + reloc_ptr->segment,
                                       reloc_ptr->offset);
                    *target_ptr += pspaddr + 0x10;
                }
            }
            myclose(executable_handle);
            return 0;
        } else {
	        nextpos = startpos + sectionsize;
            /* round up to 512-byte bound */
            nextpos = (nextpos + 511L) & ~511L;
	        if (nextpos >= filesize) {
                myclose(executable_handle);
	            return 1;
            }
	        mylseek(executable_handle,nextpos,0);
        }
    }
    myclose(executable_handle);
    return 1;
}


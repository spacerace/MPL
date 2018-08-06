#include <stdio.h>
#include <dos.h>

extern char *dskerrmsg();
union xxx {
  char secbuffer [512];

  struct hdr {
    char spare [11];
    int  sectorsize;
    char clustersize;
    int  reserved_sectors;
    char number_of_fats;
    int  root_entries;
    unsigned  total_sectors;
    char media_type;
    int  fat_size;
    int  track_size;
    int  head_count;
    unsigned  hidden_sectors;
    } dh;

  struct ptab {
    char spare[446];
    char boot_ind;
    char br_head;
    char br_sect;
    char br_cyl;
    } pt;

};

union xxx db;




main(argc, argv)
int argc;
char **argv;


{
char diskdrive[128];
int total_errors = 0;		   /* total bad sectors */
int drive = 0;
int head = 0;
int cyl = 0;
int rec = 0;
int err, i, cyl_count;
char *trakbuf;			   /* buffer for the track - allocated later */
int row = 0;
int col = 0;			   /* current cursor position */
char media = 0;

/******************************************************************************
*									      *
*  read the input parameter string to determine drive			      *
*									      *
******************************************************************************/
printf("Diskette/Fixed disk verifier program\n");
printf("Non-Copyrighted -- by Bob Murphy\n");

diskdrive[0] = '\0';               /* null the string */
if (argc > 1)
  {
  char **p;
  p=argv;			   /* p points to first argument pointer */
  p++;				   /* point to drive id */
  strcpy(diskdrive,*p); 	   /* copy the drive id */
  i = strlen(diskdrive);	   /* get the length */
  while(i >= 0)
    {				   /* convert to upper case */
    if (diskdrive[i] >= 'a' && diskdrive[i] <= 'z')
	diskdrive[i] = (diskdrive[i] - 32);
    i--;
    }
  }
if (diskdrive[0] >= 'A' && diskdrive[0] <= 'D' && diskdrive[1] == ':')
  drive = diskdrive[0] - 'A';
else
  { drive = defdrv();
    strcpy(diskdrive,"A:");
    diskdrive[0] = 'A' + drive;
  }
media = getfatid(drive+1);	   /* find out if its a hard disk */
if (media == (char) 0xf8)  drive = (drive - 2) | 0x80; /* if yes - set hard */



locate(row,col);
clrcrt(7);			 /* erase & home, color = w on bk */
printf("Drive %s\n",diskdrive);

/******************************************************************************
*									      *
*  read in the first record from the disk				      *
*									      *
******************************************************************************/
i=3;				   /* allow 3 retries */
while (i)
{
  err = absread(drive,0,0,1,1,&db);    /* read sector 1, first track */
  if (! err) break;
  i--;
}


if ((! err) && (drive == 0x80))
  {
  char *ptptr;			   /* partition table pointer */
  ptptr = &db.pt.boot_ind;	   /* point to boot indicator */
  i = 4; err=0xffff;
  while (i)
    {
    if (*ptptr == (char) 0x80)
      {
      head = *(++ptptr);	     /* read starting head from part table */
      i = *(++ptptr);		  /* read starting sec from part table */
      cyl  = *(++ptptr);	     /* read starting cyl from part table */
      rec = (i & 0x3f); 	     /* strip out cylinder bits */
      cyl = ((i & 0xc0) << 2) + cyl; /* set up the cylinder */
      err = 0;
      break;
      }
    i--; ptptr = ptptr + 16;

    }				   /* end while */
    if (! err) err = absread(drive,cyl,head,rec,1,&db); /* read dos boot rec */
  }				   /* endif */
if (db.dh.track_size == 0 && err == 0) err = 0xff; /*err ff = wierd disk */
/*



*/
if (! err)
  {
  char mdb[5];			   /* scratchpad for media descriptor */
  sprintf(&mdb[0],"%x",media);
  printf("%u Reserved Sector(s)\n",db.dh.reserved_sectors);
  printf("%u Bytes / sector\n",db.dh.sectorsize);
  printf("%u Allocation Tables\n",db.dh.number_of_fats);
  printf("%u Root Directory Entries\n",db.dh.root_entries);
  printf("%u Total DOS Sectors\n",db.dh.total_sectors);
  printf("%u Sectors in a FAT\n",db.dh.fat_size);
  printf("%u Sectors / Track\n",db.dh.track_size);
  printf("%u Surfaces\n",db.dh.head_count);
  printf("%u Hidden Sectors\n",db.dh.hidden_sectors);
  printf("%s Media Descriptor\n",&mdb [2]);
  }
else
  printf("%s Error reading boot sector -- program aborting\n",dskerrmsg(err));


/******************************************************************************
*									      *
*  allocate track buffer, and read tracks looking for errors		      *
*									      *
******************************************************************************/

if (! err)
  {
  i = db.dh.track_size * db.dh.sectorsize; /* buffer size for 1 track */
  trakbuf = malloc(i);			   /* allocate buffer */
  cyl_count = (db.dh.total_sectors / db.dh.track_size /db.dh.head_count);

  while (cyl < cyl_count)
    {
    csrpos(&row,&col);	       /* get current cursor */
    printf("cyl=%d",cyl);
    locate(row,col);
    err = absread(drive,cyl,head++,1,db.dh.track_size,trakbuf);
    if (err)
      {
      head--;			   /* reset head back to correct surface */
      i = 1;			   /* current sector number */
      while (i <= db.dh.track_size)
	{
	err = absread(drive,cyl,head,i,1,trakbuf);  /* READ 1 SECTOR */
	if (err)
	  {
	  char *mptr;
	  mptr = dskerrmsg(err);
	  printf("%s Error -- Cyl %d Head %d Record %d\n",mptr,cyl,head,i);
	  total_errors++;
	  }

	i++;			   /* bump current sector */
	}
      head++;			   /* set head to next surface */
      } 			   /* end of if (err) */
    if (head >= db.dh.head_count) { head = 0; cyl++; }
    }				   /* end of while (cyl <= cyl_count) */

  if (total_errors) printf("          \n%d Total Bad Sectors\n",total_errors);
  else printf("          \nDiskette is error free\n");

  }				   /* end of if (! err) */

}				   /* end of main */

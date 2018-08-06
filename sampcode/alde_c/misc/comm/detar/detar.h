/*
 *	MSDOS TAR Extractor
 */

#define	RECORDSIZE	512
#define	NAMSIZ		100
#define	TUNMLEN		32
#define	TGNMLEN		32

/*
 *	Header block on tape.
 *
 *	no byte swapping
 */

union record {
	char	charptr[RECORDSIZE];
	struct	{
		char	name[NAMSIZ];
		char	mode[8];
		char	uid[8];
		char	gid[8];
		char	size[12];
		char	mtime[12];
		char	chksum[8];
		char	linkflag;
		char	linkname[NAMSIZ];
		char	magic[8];
		char	uname[TUNMLEN];
		char	gname[TGNMLEN];
		char	devmajor[8];
		char	devminor[8];
	} header;
};

#define	CHKBLANKS	"        "	/* Checksum: 8 blanks, no null */
#define	TMAGIC		"ustar  "	/* Majic: 7 bytes and a null */

/* The linkflag defines the type of file */

#define	LF_OLDNORMAL	'\0'		/* Normal disk file, Unix compat */
#define	LF_NORMAL	'0'		/* Normal disk file */
#define	LF_LINK		'1'		/* Link to previously dumped file */
#define	LF_SYMLINK	'2'		/* Symbolic link */
#define	LF_CHR		'3'		/* Character special file */
#define	LF_BLK		'4'		/* Block special file */
#define	LF_DIR		'5'		/* Directory */
#define	LF_FIFO		'6'		/* FIFO special file */
#define	LF_CONTIG	'7'		/* Contiguous file */

/*
 *	Unix Stat Header (K&R)
 *
 */

struct	stat  {
	int	st_dev;
	int	st_ino;
	int	st_mode;
	int	st_nlink;
	int	st_uid;
	int	st_gid;
	int	st_rdev;
	long	st_size;
	long	st_atime;
	long	st_mtime;
	long	st_ctime;
};

#define	S_IFMT		0160000
#define	S_IFDIR		0040000
#define	S_IFCHR		0020000
#define	S_IFBLK		0060000
#define	S_IFREG		0100000
#define	S_ISUID		0004000
#define	S_ISGID		0002000
#define	S_ISVTX		0001000
#define	S_IREAD		0000400
#define	S_IWRITE	0000200
#define	S_IEXEC		0000100

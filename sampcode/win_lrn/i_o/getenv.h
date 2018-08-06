#define NAME_LEN   32
#define PORTRAIT   10
#define LANDSCAPE  11
#define CONTFEED   12
#define CUTSHEET   13
#define LETTER     20
#define DINA4      21
#define FANFOLD    22
#define ALLCTG     0xf0f
#define NOCTG      0x01
#define ACTG       0x02
#define BCTG       0x04
#define FCTG       0x08
#define DEFAULT_CTG BCTG
#define NCARTRIDGE 3
#define MAXCARTRIDGE 8      /* we have 8 bits reserved for 8 cartridges */
#define CARTRIDGE  0x100


typedef struct
{
    char  DeviceName[NAME_LEN];
    short orient;       /* orientation - portrait or landscape */
    short cartridge;
    short paper;
}   DEVMODE;


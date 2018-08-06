/* ==================================================================
              VIDEO.H header file
===================================================================*/

struct VideoInfo {
              unsigned char mode;
              unsigned char rows;
              unsigned char columns;
              unsigned char ColorFlag;
              unsigned char SyncFlag;
              unsigned int BufferStart;
              unsigned int SegAddr;
};

/* ----- C function prototypes ----- */
extern void GetVideoParms(struct VideoInfo *);
extern void ClrScr(char, struct VideoInfo *);
extern void ClrRegion(char, char, char, char, char);
extern void TextBox(char, char, char, char, char, 
                    struct VideoInfo *);
extern void SaveRegion(char, char, char, char, int *, 
                       struct VideoInfo *);
extern void RestRegion(char, char, char, char, int *, 
                       struct VideoInfo *);

/* ----- Macro Assembler function prototypes ----- */
extern int GetCharAttr(char, char, struct VideoInfo *);
extern void DispCharAttr(char, char, char, char, struct VideoInfo *);
extern void DispString(char *, char, char, char, struct VideoInfo *);

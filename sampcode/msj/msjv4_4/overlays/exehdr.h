/* exehdr.h 
   format of MS-DOS .EXE file                                           */

struct exehdr_ {
    char            M_sig;          /* M part of signature              */ 
    char            Z_sig;          /* Z part of signature              */ 
    unsigned        remain_len;     /* length % 512                     */
    unsigned        page_len;       /* length / 512                     */
    unsigned        num_relocs;     /* number of relocation items       */
    unsigned        hsize;          /* file header size, in paragraphs  */
    unsigned        minalloc;       /* minimum paragraphs heap needed   */
    unsigned        maxalloc;       /* maximum paragraphs heap needed   */
    unsigned        stack_offset;   /* paragraph offset of stk segment  */
    unsigned        init_sp;        /* initial SP value                 */
    unsigned        checksum;       /* checksum of what?  Who knows?    */
    unsigned        init_ip;        /* entry point IP                   */
    unsigned        code_offset;    /* paragraph offset of code segment */
    unsigned        first_reloc;    /* file offset of reloc table       */
    unsigned char   overlay_number; /* overlay number of this overlay   */
};

/* following, in order, are the relocation table (offset and length given
   above), the code, data, and stack segments, in that order.           */

struct reloc_entry_ {
    unsigned offset;
    unsigned segment;
};

                        "README.TXT" File
             Notes for QuickBASIC Toolbox Companion Disks
                     (c)1988 Microsoft Press

This file consists of four parts:

     PART   CONTENTS

      1     Introduction
      2     Contents of the Companion Disks
      3     Instructions for using the Companion Disks
      4     Special instructions for mixed-language files


===< Part 1: Introduction >==========================================


This disk contains all the source-code and support files for the
QuickBASIC Programmer's Toolbox book. It is very important that you
read the introductions to Part II and Part III of the book before
attempting to use these programs because certain programs have special
requirements such as a .MAK file or a Quick Library file.

All programs on the disks are tested and are assumed to be error-free.
However, should you encounter a problem, please contact:

    Microsoft Press
    16011 NE 36th Way
    Box 97017
    Redmond, WA 98073-9717
    Attn: QuickBasic Toolbox Editor

    (No phone calls, please.)


===< Part 2: Contents of the Companion Disks >======================

    README   TXT     EDIT     BAS      QBTREE   BAS      MOUSSUBS MAK
                     ERROR    BAS      QCAL     BAS      MOUSTCRS MAK
    Q        BAT     FIGETPUT BAS      QCALMATH BAS      OBJECT   MAK
    MIXED    LIB     FILEINFO BAS      RANDOMS  BAS      QBFMT    MAK
    MIXED    QLB     FRACTION BAS      STDOUT   BAS      QBTREE   MAK
    MIXEDC   QLB     GAMES    BAS      STRINGS  BAS      QCAL     MAK
                     HEX2BIN  BAS      TRIANGLE BAS      TRIANGLE MAK
    ATTRIB   BAS     IMAGEARY BAS      WINDOWS  BAS      WINDOWS  MAK
    BIN2HEX  BAS     INTRODEM BAS      WORDCOUN BAS
    BIOSCALL BAS     JUSTIFY  BAS                        CASEMAP  ASM
    BITS     BAS     KEYS     BAS      BIN2HEX  MAK      MOUSE    ASM
    CALENDAR BAS     LOOK     BAS      CIPHER   MAK      CTOOLS1  C
    CARTESIA BAS     MONTH    BAS      COLORS   MAK      CTOOLS2  C
    CDEMO1   BAS     MOUSGCRS BAS      COMPLEX  MAK
    CDEMO2   BAS     MOUSSUBS BAS      EDIT     MAK      CASEMAP  HEX
    CIPHER   BAS     MOUSTCRS BAS      HEX2BIN  MAK      INTRPT   HEX
    COLORS   BAS     OBJECT   BAS      JUSTIFY  MAK      MOUSE    HEX
    COMPLEX  BAS     PARSE    BAS      LOOK     MAK
    DOLLARS  BAS     PROBSTAT BAS      MONTH    MAK
    DOSCALLS BAS     QBFMT    BAS      MOUSGCRS MAK


===< Part 3: Instructions for using the Companion Disks >===========

Making backup copies:

    If you have not already done so, immediately make backup copies
    of these disks using the DISKCOPY command.  Store the originals
    in a safe place and use only the copies.

If you are using a hard disk:

    To get started, create a new subdirectory on your hard disk and
    copy all files from the Companion Disks into it.  Select
    a name for this directory that is easy for you to remember.
    For example, to create a subdirectory named QBTOOLS on hard disk
    drive C: you would type:

    MD C:\QBTOOLS

    Continuing with this example, you would type the following
    command to copy all files from the companion disk:

    COPY A:*.* C:\QBTOOLS

    (If you received two Companion Disks, then use the COPY command
    a second time to copy the files from the second Companion Disk
    to the hard disk.)

    To load files from this new directory into QuickBASIC, start
    QuickBASIC and select Open Program from the File menu.  Type
    C:\QBTOOLS\*.BAS in response to the File Name prompt.  The
    complete list of .BAS files is then displayed for your selection.


If you are using floppy disks:

    You may load the files into QuickBASIC directly from the
    Companion Disks as required, or you can copy all appropriate
    files to a working floppy for developing programs.  For example,
    to copy QCAL.BAS and associated files to a working floppy you
    should copy QCAL.BAS, QCALMATH.BAS, and QCAL.MAK to your working
    disk.  In general, if a program file has an associated .MAK
    file, copy the main program file, all the files listed in the
    .MAK file, and the .MAK file itself.


===< Part 4: Special instructions for mixed-language files >==========


Two files listed in the book and included on the Companion Disks,
CDEMO1.BAS and CDEMO2.BAS, require the presence of MLIBCE.LIB, a
QuickC library file created when you install QuickC on your system.
You need MLIBCE.LIB before you can compile CDEMO1.BAS and CDEMO2.BAS
into stand-alone executable files. If you do not own Microsoft QuickC,
you will not be able to compile these files. However, a special Quick
Library file, MIXEDC.QLB, is included on the Companion Disks to let
you run CDEMO1.BAS and CDEMO2.BAS within the QuickBASIC environment.
To do this, load QuickBASIC using the following command line:

    QB /L MIXEDC.QLB


As an added bonus, the Companion Disks contain the file INTRODEM.BAS,
which demonstrates some highlights of the various toolboxes presented
in the book--it is highly recommended that you run this program first
to get an idea of how useful and robust the toolboxes can be!

NOTE: INTRODEM.BAS also requires that you load MIXEDC.QLB with
QuickBASIC using the above syntax line. As with CDEMO1.BAS and
CDEMO2.BAS, you cannot create a stand-alone executable file unless
you have Microsoft QuickC installed in your system.


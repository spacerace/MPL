ifdef INCL_ERRORS
INCL_DOSERRORS      equ 1
endif ;  INCL_ERRORS   

ifdef INCL_DOSERRORS
NO_ERROR            equ 0
ERROR_INVALID_FUNCTION equ 1
ERROR_FILE_NOT_FOUND equ 2
ERROR_PATH_NOT_FOUND equ 3
ERROR_TOO_MANY_OPEN_FILES equ 4
ERROR_ACCESS_DENIED equ 5
ERROR_INVALID_HANDLE equ 6
ERROR_ARENA_TRASHED equ 7
ERROR_NOT_ENOUGH_MEMORY equ 8
ERROR_INVALID_BLOCK equ 9
ERROR_BAD_ENVIRONMENT equ 10
ERROR_BAD_FORMAT    equ 11
ERROR_INVALID_ACCESS equ 12
ERROR_INVALID_DATA  equ 13
ERROR_INVALID_DRIVE equ 15
ERROR_CURRENT_DIRECTORY equ 16
ERROR_NOT_SAME_DEVICE equ 17
ERROR_NO_MORE_FILES equ 18
ERROR_WRITE_PROTECT equ 19
ERROR_BAD_UNIT      equ 20
ERROR_NOT_READY     equ 21
ERROR_BAD_COMMAND   equ 22
ERROR_CRC           equ 23
ERROR_BAD_LENGTH    equ 24
ERROR_SEEK          equ 25
ERROR_NOT_DOS_DISK  equ 26
ERROR_SECTOR_NOT_FOUND equ 27
ERROR_OUT_OF_PAPER  equ 28
ERROR_WRITE_FAULT   equ 29
ERROR_READ_FAULT    equ 30
ERROR_GEN_FAILURE   equ 31
ERROR_SHARING_VIOLATION equ 32
ERROR_LOCK_VIOLATION equ 33
ERROR_WRONG_DISK    equ 34
ERROR_FCB_UNAVAILABLE equ 35
ERROR_SHARING_BUFFER_EXCEEDED equ 36
ERROR_NOT_SUPPORTED equ 50
ERROR_FILE_EXISTS   equ 80
ERROR_DUP_FCB       equ 81
ERROR_CANNOT_MAKE   equ 82
ERROR_FAIL_I24      equ 83
ERROR_OUT_OF_STRUCTURES equ 84
ERROR_ALREADY_ASSIGNED equ 85
ERROR_INVALID_PASSWORD equ 86
ERROR_INVALID_PARAMETER equ 87
ERROR_NET_WRITE_FAULT equ 88
ERROR_NO_PROC_SLOTS equ 89
ERROR_NOT_FROZEN    equ 90
ERR_TSTOVFL         equ 91
ERR_TSTDUP          equ 92
ERROR_NO_ITEMS      equ 93
ERROR_INTERRUPT     equ 95
ERROR_TOO_MANY_SEMAPHORES equ 100
ERROR_EXCL_SEM_ALREADY_OWNED equ 101
ERROR_SEM_IS_SET    equ 102
ERROR_TOO_MANY_SEM_REQUESTS equ 103
ERROR_INVALID_AT_INTERRUPT_TIME equ 104
ERROR_SEM_OWNER_DIED equ 105
ERROR_SEM_USER_LIMIT equ 106
ERROR_DISK_CHANGE   equ 107
ERROR_DRIVE_LOCKED  equ 108
ERROR_BROKEN_PIPE   equ 109
ERROR_OPEN_FAILED   equ 110
ERROR_BUFFER_OVERFLOW equ 111
ERROR_DISK_FULL     equ 112
ERROR_NO_MORE_SEARCH_HANDLES equ 113
ERROR_INVALID_TARGET_HANDLE equ 114
ERROR_PROTECTION_VIOLATION equ 115
ERROR_VIOKBD_REQUEST equ 116
ERROR_INVALID_CATEGORY equ 117
ERROR_INVALID_VERIFY_SWITCH equ 118
ERROR_BAD_DRIVER_LEVEL equ 119
ERROR_CALL_NOT_IMPLEMENTED equ 120
ERROR_SEM_TIMEOUT   equ 121
ERROR_INSUFFICIENT_BUFFER equ 122
ERROR_INVALID_NAME  equ 123
ERROR_INVALID_LEVEL equ 124
ERROR_NO_VOLUME_LABEL equ 125
ERROR_MOD_NOT_FOUND equ 126
ERROR_PROC_NOT_FOUND equ 127
ERROR_WAIT_NO_CHILDREN equ 128
ERROR_CHILD_NOT_COMPLETE equ 129
ERROR_DIRECT_ACCESS_HANDLE equ 130
ERROR_NEGATIVE_SEEK equ 131
ERROR_SEEK_ON_DEVICE equ 132
ERROR_IS_JOIN_TARGET equ 133
ERROR_IS_JOINED     equ 134
ERROR_IS_SUBSTED    equ 135
ERROR_NOT_JOINED    equ 136
ERROR_NOT_SUBSTED   equ 137
ERROR_JOIN_TO_JOIN  equ 138
ERROR_SUBST_TO_SUBST equ 139
ERROR_JOIN_TO_SUBST equ 140
ERROR_SUBST_TO_JOIN equ 141
ERROR_BUSY_DRIVE    equ 142
ERROR_SAME_DRIVE    equ 143
ERROR_DIR_NOT_ROOT  equ 144
ERROR_DIR_NOT_EMPTY equ 145
ERROR_IS_SUBST_PATH equ 146
ERROR_IS_JOIN_PATH  equ 147
ERROR_PATH_BUSY     equ 148
ERROR_IS_SUBST_TARGET equ 149
ERROR_SYSTEM_TRACE  equ 150
ERROR_INVALID_EVENT_COUNT equ 151
ERROR_TOO_MANY_MUXWAITERS equ 152
ERROR_INVALID_LIST_FORMAT equ 153
ERROR_LABEL_TOO_LONG equ 154
ERROR_TOO_MANY_TCBS equ 155
ERROR_SIGNAL_REFUSED equ 156
ERROR_DISCARDED     equ 157
ERROR_NOT_LOCKED    equ 158
ERROR_BAD_THREADID_ADDR equ 159
ERROR_BAD_ARGUMENTS equ 160
ERROR_BAD_PATHNAME  equ 161
ERROR_SIGNAL_PENDING equ 162
ERROR_UNCERTAIN_MEDIA equ 163
ERROR_MAX_THRDS_REACHED equ 164
ERROR_MONITORS_NOT_SUPPORTED equ 165
ERROR_INVALID_SEGMENT_NUMBER equ 180
ERROR_INVALID_CALLGATE equ 181
ERROR_INVALID_ORDINAL equ 182
ERROR_ALREADY_EXISTS equ 183
ERROR_NO_CHILD_PROCESS equ 184
ERROR_CHILD_ALIVE_NOWAIT equ 185
ERROR_INVALID_FLAG_NUMBER equ 186
ERROR_SEM_NOT_FOUND equ 187
ERROR_INVALID_STARTING_CODESEG equ 188
ERROR_INVALID_STACKSEG equ 189
ERROR_INVALID_MODULETYPE equ 190
ERROR_INVALID_EXE_SIGNATURE equ 191
ERROR_EXE_MARKED_INVALID equ 192
ERROR_BAD_EXE_FORMAT equ 193
ERROR_ITERATED_DATA_EXCEEDS_64K equ 194
ERROR_INVALID_MINALLOCSIZE equ 195
ERROR_DYNLINK_FROM_INVALID_RING equ 196
ERROR_IOPL_NOT_ENABLED equ 197
ERROR_INVALID_SEGDPL equ 198
ERROR_AUTODATASEG_EXCEEDS_64k equ 199
ERROR_RING2SEG_MUST_BE_MOVABLE equ 200
ERROR_RELOC_CHAIN_XEEDS_SEGLIM equ 201
ERROR_INFLOOP_IN_RELOC_CHAIN equ 202
ERROR_ENVVAR_NOT_FOUND equ 203
ERROR_NOT_CURRENT_CTRY equ 204
ERROR_NO_SIGNAL_SENT equ 205
ERROR_FILENAME_EXCED_RANGE equ 206
ERROR_RING2_STACK_IN_USE equ 207
ERROR_META_EXPANSION_TOO_LONG equ 208
ERROR_INVALID_SIGNAL_NUMBER equ 209
ERROR_THREAD_1_INACTIVE equ 210
ERROR_INFO_NOT_AVAIL equ 211
ERROR_LOCKED        equ 212
ERROR_BAD_DYNALINK  equ 213
ERROR_TOO_MANY_MODULES equ 214
ERROR_NESTING_NOT_ALLOWED equ 215
ERROR_USER_DEFINED_BASE equ 0F000h
ERROR_I24_WRITE_PROTECT equ 0
ERROR_I24_BAD_UNIT  equ 1
ERROR_I24_NOT_READY equ 2
ERROR_I24_BAD_COMMAND equ 3
ERROR_I24_CRC       equ 4
ERROR_I24_BAD_LENGTH equ 5
ERROR_I24_SEEK      equ 6
ERROR_I24_NOT_DOS_DISK equ 7
ERROR_I24_SECTOR_NOT_FOUND equ 8
ERROR_I24_OUT_OF_PAPER equ 9
ERROR_I24_WRITE_FAULT equ 10
ERROR_I24_READ_FAULT equ 11
ERROR_I24_GEN_FAILURE equ 12
ERROR_I24_DISK_CHANGE equ 13
ERROR_I24_WRONG_DISK equ 14
ERROR_I24_UNCERTAIN_MEDIA equ 15
ERROR_I24_CHAR_CALL_INTERRUPTED equ 16
ERROR_I24_NO_MONITOR_SUPPORT equ 17
ERROR_I24_INVALID_PARAMETER equ 18
ALLOWED_FAIL        equ 00001h
ALLOWED_ABORT       equ 00002h
ALLOWED_RETRY       equ 00004h
ALLOWED_IGNORE      equ 00008h
I24_OPERATION       equ 001h
I24_AREA            equ 006h
I24_CLASS           equ 080h
ERRCLASS_OUTRES     equ 1
ERRCLASS_TEMPSIT    equ 2
ERRCLASS_AUTH       equ 3
ERRCLASS_INTRN      equ 4
ERRCLASS_HRDFAIL    equ 5
ERRCLASS_SYSFAIL    equ 6
ERRCLASS_APPERR     equ 7
ERRCLASS_NOTFND     equ 8
ERRCLASS_BADFMT     equ 9
ERRCLASS_LOCKED     equ 10
ERRCLASS_MEDIA      equ 11
ERRCLASS_ALREADY    equ 12
ERRCLASS_UNK        equ 13
ERRCLASS_CANT       equ 14
ERRCLASS_TIME       equ 15
ERRACT_RETRY        equ 1
ERRACT_DLYRET       equ 2
ERRACT_USER         equ 3
ERRACT_ABORT        equ 4
ERRACT_PANIC        equ 5
ERRACT_IGNORE       equ 6
ERRACT_INTRET       equ 7
ERRLOC_UNK          equ 1
ERRLOC_DISK         equ 2
ERRLOC_NET          equ 3
ERRLOC_SERDEV       equ 4
ERRLOC_MEM          equ 5
TC_NORMAL           equ 0
TC_HARDERR          equ 1
TC_GP_TRAP          equ 2
TC_SIGNAL           equ 3
endif ;  INCL_DOSERRORS   

TC_NORMAL           equ 0
TC_HARDERR          equ 1
TC_GP_TRAP          equ 2
TC_SIGNAL           equ 3

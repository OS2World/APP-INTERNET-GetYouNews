VOID (*UULoadFile)(PSZ pszFileName, PSZ pszSomething, int iDelete);
VOID *(*UUGetFileListItem)(int iItem);
int (*UUDecodeFile)(VOID *, PSZ pszSomething);
int (*UUInitialize)(void);

int (*UUDecode)(PSZ pszBody, ULONG ulBodyLen, PSZ pszFileName, ULONG *ulFileSize);
BOOL bUUCodeDecoderNotLoaded = TRUE;



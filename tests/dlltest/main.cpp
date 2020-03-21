#include "../../idp/idp.h"
#include <stdio.h>
#include <conio.h>

#ifdef __MINGW32__
    #define _gettch getch
#endif

int _tmain(int argc, _TCHAR* argv[])
{
    idpAddFileComp(_T("http://127.0.0.1/test1.rar"), _T("test1.rar"), _T("comp1"));
    idpAddFileComp(_T("http://127.0.0.1/test2.rar"), _T("test2.rar"), _T("comp2"));
    idpAddFileComp(_T("http://127.0.0.1/test3.rar"), _T("test3.rar"), _T("comp3"));

    DWORDLONG size;
    bool sizeIsKnown = idpGetFilesSize(&size);
    _tprintf(_T("Size of files: %d bytes%s\n"), (sizeIsKnown ? (int)size : -1), (sizeIsKnown ? _T("") : _T(" (one or more files' sizes are unknown!)")));

    idpSetInternalOption(_T("ErrorDialog"), _T("UrlList"));
    idpSetComponents(_T("comp1,comp2"));
    idpStartDownload();

    _tprintf(_T("Download started\n"));
    (void)_gettch();

    return 0;
}

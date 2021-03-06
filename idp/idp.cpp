#include "ui.h"
#include "idp.h"
#include "trace.h"

HINSTANCE idpDllHandle = NULL;

Downloader      downloader;
Ui              ui;
InternetOptions internetOptions;

void idpAddF(const _TCHAR *url)
{
    downloader.addFile(STR(url));
}

void idpAddFile(const _TCHAR *url, const _TCHAR *filename)
{
    downloader.addFile(STR(url), STR(filename));
}

void idpAddFileSize(const _TCHAR *url, const _TCHAR *filename, DWORDLONG filesize)
{
    downloader.addFile(STR(url), STR(filename), filesize);
}

void idpAddFileComp(const _TCHAR *url, const _TCHAR *filename, const _TCHAR *components)
{
    downloader.addFile(STR(url), STR(filename), FILE_SIZE_UNKNOWN, STR(components));
}

void idpAddFileSizeComp(const _TCHAR *url, const _TCHAR *filename, DWORDLONG filesize, const _TCHAR *components)
{
    downloader.addFile(STR(url), STR(filename), filesize, STR(components));
}

void idpAddMirror(const _TCHAR *url, const _TCHAR *mirror)
{
    downloader.addMirror(STR(url), STR(mirror));
}

void idpAddFtpDir(const _TCHAR *url, const _TCHAR *mask, const _TCHAR *destdir, bool recursive)
{
    downloader.addFtpDir(STR(url), STR(mask), STR(destdir), recursive);
}

void idpAddFtpDirComp(const _TCHAR *url, const _TCHAR *mask, const _TCHAR *destdir, bool recursive, const _TCHAR *components)
{
    downloader.addFtpDir(STR(url), STR(mask), STR(destdir), recursive, components);
}

void idpSetDestDir(const _TCHAR *dir, bool forAllFiles)
{
    downloader.setDestDir(STR(dir), forAllFiles);
}

void idpGetDestDir(_TCHAR *destdir, int destdirMaxSize)
{
    _tcscpy_s(destdir, destdirMaxSize, downloader.getDestDir().c_str());
}

void idpClearFiles()
{
    downloader.clearFiles();
}

int idpFilesCount()
{
    return downloader.filesCount();
}

int idpFtpDirsCount()
{
    return downloader.ftpDirsCount();
}

bool idpFilesDownloaded()
{
    return downloader.filesDownloaded();
}

bool idpFileDownloaded(const _TCHAR *url)
{
    return downloader.fileDownloaded(STR(url));
}

bool idpStartEnumFiles()
{
    return downloader.startEnumFiles();
}

/*
Example usage in Inno script:

const
  MAX_PATH = 260;

function GetOneEnumeratedFile(): string;
begin
  SetLength(Result, MAX_PATH);
  idpEnumFiles(Result, MAX_PATH, FindData, IDP_DOWNLOADED);
  SetLength(Result, Pos(#0, Result) - 1);
end;

As gleaned from https://code.i-harness.com/en/q/20a2b72#answers:

You must:

- Ensure that both sides are using the same string types, either both ANSI or both Unicode.
  - ANSI Inno Setup supports only ANSI strings with its String type.
  - Unicode Inno Setup supports either ANSI strings with AnsiString or Unicode strings with String.
- When using Unicode strings, ensure that both sides agree whether Max and/or the return value 
  is specified in characters or bytes (the example code assumes it's in characters).
- Prior to calling the function, use either SetLength or StringOfChar to ensure that the buffer 
  has been sized to the required maximum possible result length.
- Ensure the called function does not try to write past this maximum length (which is easier 
  if this is provided as a parameter to the function).
- Ensure that if you're using Pos, the called function must ensure the value is null-terminated 
  (or you need to be more careful than shown in the example).
- Ensure that after the call you truncate the string to the actual length by finding the null terminator.

One of the constraints in play here is that memory allocated by one side must be freed by the same side. 
You cannot safely release memory allocated on the "wrong" side of the DLL boundary, in either direction.

*/
bool idpEnumFiles(_TCHAR *dstFilename, int dstFilenameMaxSize, int fileType)
{
    return downloader.enumerateFiles(dstFilename, dstFilenameMaxSize, fileType);
}

bool idpGetFileSize(const _TCHAR *url, DWORDLONG *size)
{
    Downloader d;
    d.setInternetOptions(internetOptions);
    d.setOptions(&downloader);
    d.setMirrorList(&downloader);
    d.addFile(STR(url), _T(""));
    *size = d.getFileSizes();

    return *size != FILE_SIZE_UNKNOWN;
}

bool idpGetFilesSize(DWORDLONG *size)
{
    downloader.setUi(NULL);
    downloader.setInternetOptions(internetOptions);
    *size = downloader.getFileSizes(false);
    return *size != FILE_SIZE_UNKNOWN;
}

bool idpDownloadFile(const _TCHAR *url, const _TCHAR *filename)
{
    Downloader d;
    d.setInternetOptions(internetOptions);
    d.setOptions(&downloader);
    d.setMirrorList(&downloader);
    d.addFile(STR(url), STR(filename));
    return d.downloadFiles();
}

bool idpDownloadFileDir(const _TCHAR *url, const _TCHAR *destdir, _TCHAR *outname, int outnameMaxSize)
{
    Downloader d;
    d.setInternetOptions(internetOptions);
    d.setOptions(&downloader);
    d.setMirrorList(&downloader);
    d.setDestDir(STR(destdir));
    d.addFile(STR(url));
    bool res = d.downloadFiles();

    d.startEnumFiles();
    d.enumerateFiles(outname, outnameMaxSize, IDP_DOWNLOADED);

    return res;
}

bool idpDownloadFiles()
{
    downloader.ownMsgLoop = false;
    downloader.setUi(NULL);
    downloader.setInternetOptions(internetOptions);
    return downloader.downloadFiles(false);
}

bool idpDownloadFilesComp()
{
    downloader.ownMsgLoop = false;
    downloader.setUi(NULL);
    downloader.setInternetOptions(internetOptions);
    return downloader.downloadFiles(true);
}

bool idpDownloadFilesCompUi()
{
    ui.lockButtons();
    downloader.ownMsgLoop = true;
    downloader.processMessages();
    downloader.setUi(&ui);
    downloader.setInternetOptions(internetOptions);
    
    bool res;

    while(true)
    {
        res = downloader.downloadFiles(true);

        TRACE(_T("idpDownloadFilesCompUi: ui.errorDlgMode == %d"), ui.errorDlgMode);

        if(res || (ui.errorDlgMode == DLG_NONE) || downloader.downloadCancelled)
            break; // go to next page
        else if(ui.errorDlgMode == DLG_SIMPLE)
        {
            int dlgRes = ui.messageBox(ui.msg("Download failed") + _T(": ") + downloader.getLastErrorStr() + _T("\r\n") + (ui.allowContinue ?
                                       ui.msg("Check your connection and click 'Retry' to try downloading the files again, or click 'Next' to continue installing anyway.") :
                                       ui.msg("Check your connection and click 'Retry' to try downloading the files again, or click 'Cancel' to terminate setup.")),
                                       ui.msg("Download failed"), MB_ICONWARNING | (ui.hasRetryButton ? MB_OK : MB_RETRYCANCEL));

            if     (dlgRes == IDRETRY)  continue;
            else if(dlgRes == IDCANCEL) break;
        }
        else
        {
            ui.dllHandle = idpDllHandle;

            int dlgRes = ui.errorDialog(&downloader);
            
            if     (dlgRes == IDRETRY)  continue;
            else if(dlgRes == IDABORT)  break;
            else if(dlgRes == IDIGNORE) break;
        }
    }

    ui.unlockButtons();
    return res;
}

void idpConnectControl(const _TCHAR *name, HWND handle)
{
    if(name)
        ui.connectControl(name, handle);
}

void idpAddMessage(const _TCHAR *name, const _TCHAR *message)
{
    if(name)
        ui.addMessage(STR(name), STR(message));
}

void idpSetComponents(const _TCHAR *components)
{
    downloader.setComponents(STR(components));
}

void idpStartDownload()
{
    ui.lockButtons();
    downloader.ownMsgLoop = false;
    downloader.setUi(&ui);
    downloader.setInternetOptions(internetOptions);
    downloader.setFinishedCallback(&downloadFinished);
    downloader.startDownload();
}

void idpStopDownload()
{
    downloader.stopDownload();
    ui.unlockButtons();
    ui.setStatus(ui.msg("Download cancelled"));
}

void downloadFinished(Downloader *d, bool res)
{
    ui.reportError(); //salto-mortale to main thread, which calls idpReportError
}

void idpReportError()
{
    ui.unlockButtons(); // allow user to click Retry or Next

    if(downloader.filesDownloaded() || (ui.errorDlgMode == DLG_NONE))
        ui.clickNextButton(); // go to next page
    else if(ui.errorDlgMode == DLG_SIMPLE)
    {
        if(ui.messageBox(ui.msg("Download failed") + _T(": ") + downloader.getLastErrorStr() + _T("\r\n") + (ui.allowContinue ?
                         ui.msg("Check your connection and click 'Retry' to try downloading the files again, or click 'Next' to continue installing anyway.") :
                         ui.msg("Check your connection and click 'Retry' to try downloading the files again, or click 'Cancel' to terminate setup.")),
                         ui.msg("Download failed"), MB_ICONWARNING | (ui.hasRetryButton ? MB_OK : MB_RETRYCANCEL)) == IDRETRY)
            idpStartDownload();
    }
    else
    {
        ui.dllHandle = idpDllHandle;

        switch(ui.errorDialog(&downloader))
        {
        case IDRETRY : idpStartDownload();   break;
        case IDIGNORE: ui.clickNextButton(); break;
        }
    }
}

// ANSI Inno Setup don't support 64-bit integers.

void idpAddFileSize32(const _TCHAR *url, const _TCHAR *filename, DWORD filesize)
{
    idpAddFileSize(STR(url), STR(filename), filesize);
}

void idpAddFileSizeComp32(const _TCHAR *url, const _TCHAR *filename, DWORD filesize, const _TCHAR *components)
{
    idpAddFileSizeComp(STR(url), STR(filename), filesize, STR(components));
}

bool idpGetFileSize32(const _TCHAR *url, DWORD *size)
{
    DWORDLONG size64;
    bool r = idpGetFileSize(STR(url), &size64);
    *size = (DWORD)size64;
    return r;
}

bool idpGetFilesSize32(DWORD *size)
{
    DWORDLONG size64;
    bool r = idpGetFilesSize(&size64);
    *size = (DWORD)size64;
    return r;
}

DWORD timeoutVal(const _TCHAR *value)
{
    const _TCHAR* v = STR(value);
    string val = toansi(tstrlower(v));

    if(val.compare("infinite") == 0) return TIMEOUT_INFINITE;
    if(val.compare("infinity") == 0) return TIMEOUT_INFINITE;
    if(val.compare("inf")      == 0) return TIMEOUT_INFINITE;

    return _ttoi(v);
}

bool boolVal(const _TCHAR *value)
{
    const _TCHAR* v = STR(value);
    string val = toansi(tstrlower(v));

    if(val.compare("true")  == 0) return true;
    if(val.compare("t")     == 0) return true;
    if(val.compare("yes")   == 0) return true;
    if(val.compare("y")     == 0) return true;
    if(val.compare("false") == 0) return false;
    if(val.compare("f")     == 0) return false;
    if(val.compare("no")    == 0) return false;
    if(val.compare("n")     == 0) return false;

    return _ttoi(v) > 0;
}

int dlgVal(const _TCHAR *value)
{
    const _TCHAR* v = STR(value);
    string val = toansi(tstrlower(v));

    if(val.compare("none")     == 0) return DLG_NONE;
    if(val.compare("simple")   == 0) return DLG_SIMPLE;
    if(val.compare("filelist") == 0) return DLG_FILELIST;
    if(val.compare("urllist")  == 0) return DLG_URLLIST;

    return boolVal(v) ? DLG_NONE : DLG_SIMPLE;
}

int invCertVal(const _TCHAR *value)
{
    string val = toansi(tstrlower(STR(value)));

    if(val.compare("showdialog") == 0) return INVC_SHOWDLG;
    if(val.compare("showdlg")    == 0) return INVC_SHOWDLG;
    if(val.compare("stop")       == 0) return INVC_STOP;
    if(val.compare("ignore")     == 0) return INVC_IGNORE;

    return INVC_SHOWDLG;
}

DWORD proxyVal(const _TCHAR *value)
{
    string val = toansi(tstrlower(STR(value)));

    if(val.compare("auto")      == 0) return INTERNET_OPEN_TYPE_PRECONFIG;
    if(val.compare("preconfig") == 0) return INTERNET_OPEN_TYPE_PRECONFIG;
    if(val.compare("preconf")   == 0) return INTERNET_OPEN_TYPE_PRECONFIG;
    if(val.compare("direct")    == 0) return INTERNET_OPEN_TYPE_DIRECT;
    if(val.compare("none")      == 0) return INTERNET_OPEN_TYPE_DIRECT;
    if(val.compare("proxy")     == 0) return INTERNET_OPEN_TYPE_PROXY;

    return INTERNET_OPEN_TYPE_PRECONFIG;
}

int bufSizeVal(const _TCHAR *value)
{
    const _TCHAR* v = STR(value);
    string val = toansi(tstrlower(v));

    if(val.compare("default") == 0) return DEFAULT_READ_BUFSIZE;
    if(val.compare("auto")    == 0) return DEFAULT_READ_BUFSIZE;

    int bufSize = _ttoi(v);
    return bufSize ? bufSize : DEFAULT_READ_BUFSIZE;
}

void idpSetInternalOption(const _TCHAR *name, const _TCHAR *value)
{
    if(!name)
        return;

    TRACE(_T("idpSetInternalOption(%s, %s)"), name, value);

    string key = toansi(tstrlower(name));

    if(key.compare("allowcontinue") == 0)
    {
        ui.allowContinue       = boolVal(value);
        downloader.stopOnError = !ui.allowContinue;
    }
    else if(key.compare("stoponerror")      == 0) downloader.stopOnError         = boolVal(value);
    else if(key.compare("preserveftpdirs")  == 0) downloader.preserveFtpDirs     = boolVal(value);
    else if(key.compare("readbuffersize")   == 0) downloader.readBufferSize      = bufSizeVal(value);
    else if(key.compare("retrybutton")      == 0) ui.hasRetryButton              = boolVal(value);
    else if(key.compare("redrawbackground") == 0) ui.redrawBackground            = boolVal(value);
    else if(key.compare("errordialog")      == 0) ui.errorDlgMode                = dlgVal(value);
    else if(key.compare("errordlg")         == 0) ui.errorDlgMode                = dlgVal(value);
    else if(key.compare("useragent")        == 0) internetOptions.userAgent      = STR(value);
    else if(key.compare("referer")          == 0) internetOptions.referer        = STR(value);
    else if(key.compare("invalidcert")      == 0) internetOptions.invalidCert    = invCertVal(value);
    else if(key.compare("oninvalidcert")    == 0) internetOptions.invalidCert    = invCertVal(value);
    else if(key.compare("connecttimeout")   == 0) internetOptions.connectTimeout = timeoutVal(value);
    else if(key.compare("sendtimeout")      == 0) internetOptions.sendTimeout    = timeoutVal(value);
    else if(key.compare("receivetimeout")   == 0) internetOptions.receiveTimeout = timeoutVal(value);
    else if(key.compare("passiveftp")       == 0) internetOptions.passiveFtp     = boolVal(value);
    else if(key.compare("usewiredirect")    == 0) internetOptions.useWIRedirect  = boolVal(value);
    else if(key.compare("username")         == 0) internetOptions.login          = STR(value);
    else if(key.compare("password")         == 0) internetOptions.password       = STR(value);
    else if(key.compare("proxymode")        == 0) internetOptions.accessType     = proxyVal(value);
    else if(key.compare("proxyusername")    == 0) internetOptions.proxyLogin     = STR(value);
    else if(key.compare("proxypassword")    == 0) internetOptions.proxyPassword  = STR(value);
    else if(key.compare("proxyname")        == 0)
    {
        internetOptions.proxyName = STR(value);

        if(!internetOptions.proxyName.empty())
            internetOptions.accessType = INTERNET_OPEN_TYPE_PROXY;
    }
}

void idpSetProxyMode(const _TCHAR *mode)
{
    if(!mode)
        return;

    internetOptions.accessType = proxyVal(mode);
}

void idpSetProxyName(const _TCHAR *name)
{
    internetOptions.proxyName = STR(name);

    if(!internetOptions.proxyName.empty())
        internetOptions.accessType = INTERNET_OPEN_TYPE_PROXY;
}

void idpSetProxyLogin(const _TCHAR *login, const _TCHAR *password)
{
    internetOptions.proxyLogin    = STR(login);
    internetOptions.proxyPassword = STR(password);
}

void idpSetLogin(const _TCHAR *login, const _TCHAR *password)
{
    internetOptions.login    = STR(login);
    internetOptions.password = STR(password);
}

void idpSetDetailedMode(bool mode)
{
    ui.setDetailedMode(mode);
}

void idpTrace(const _TCHAR *text)
{
    TRACE(_T("%s"), text);
}

#if defined(IDP_EXPORTS)

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
    if(dwReason == DLL_PROCESS_ATTACH)
        idpDllHandle = hinstDLL;
        
    return TRUE;
}

#endif

#pragma once

#include "ui.h"
#include <windows.h>
#include <wininet.h>
#include <map>
#include <set>
#include <list>
#include "tstring.h"
#include "netfile.h"
#include "timer.h"
#include "internetoptions.h"
#include "ftpdir.h"

#define DOWNLOAD_CANCEL_TIMEOUT 30000
#define DEFAULT_READ_BUFSIZE    1024

#define IDP_ALL            0
#define IDP_DOWNLOADED     1
#define IDP_NOT_DOWNLOADED 2

using namespace std;

class Downloader;

typedef void (*FinishedCallback)(Downloader *d, bool res);

class Downloader
{
public:
    Downloader();
    ~Downloader();

    void      addFile(const tstring url, const tstring filename = _T(""), DWORDLONG size = FILE_SIZE_UNKNOWN, const tstring comp = _T(""));
    void      addFtpDir(const tstring url, const tstring mask, const tstring destdir, bool recursive, const tstring comp = _T(""));
    void      addMirror(const tstring url, const tstring mirror);
    void      setMirrorList(Downloader *d);
    void      setDestDir(const tstring dir, bool forAllFiles = false);
    tstring   getDestDir();
    void      clearFiles();
    void      clearMirrors();
    void      clearFtpDirs();
    bool      downloadFiles(bool useComponents = true);
    void      startDownload();
    void      stopDownload();
    void      pauseDownload();
    void      resumeDownload();
    DWORDLONG getFileSizes(bool useComponents = true);
    int       filesCount();
    int       ftpDirsCount();
    bool      filesDownloaded();
    bool      ftpDirsProcessed();
    bool      fileDownloaded(const tstring url);
    bool      startEnumFiles();
    bool      enumerateFiles(_TCHAR *filename, size_t filenameMaxSize, int fileType);
    DWORD     getLastError();
    tstring   getLastErrorStr();
    void      setComponents(const tstring comp);
    void      setUi(Ui *newUi);
    void      setInternetOptions(InternetOptions opt);
    void      setOptions(Downloader *d);
    void      setFinishedCallback(FinishedCallback callback);
    void      processMessages();

    bool stopOnError;
    bool ownMsgLoop;
    bool preserveFtpDirs;
    bool downloadCancelled;
    bool downloadPaused;
    int  readBufferSize;

protected:
    bool openInternet();
    bool closeInternet();
    bool downloadFile(NetFile *netFile);
    bool checkMirrors(const tstring url, bool download/* or get size */);
    void updateProgress(NetFile *file);
    void updateFileName(NetFile *file);
    void updateFileName(const tstring filename);
    void updateSpeed(NetFile *file, Timer *timer);
    void updateSizeTime(NetFile *file, Timer *timer);
    void updateStatus(const tstring status);
    void setMarquee(bool marquee, bool total = true);
    void storeError();
    void storeError(const tstring msg, DWORD errcode = 0);
    bool scanFtpDir(FtpDir *ftpDir, const tstring destsubdir = _T(""));
    void processFtpDirs();
    tstring msg(const string key);
    
    map<tstring, NetFile *>    files;
    multimap<tstring, tstring> mirrors;
    set<tstring>               components;
    list<FtpDir *>             ftpDirs;
    tstring                    destDir;
    DWORDLONG                  filesSize;
    DWORDLONG                  downloadedFilesSize;
    HINTERNET                  internet;
    Timer                      sizeTimeTimer;
    DWORD                      errorCode;
    tstring                    errorStr;
    Ui                        *ui;
    InternetOptions            internetOptions;
    HANDLE                     downloadThread;
    FinishedCallback           finishedCallback;
    MSG                        windowsMsg;
    map<tstring, NetFile *>::iterator enumIter;

    friend void downloadThreadProc(void *param);
    friend class Ui;
};

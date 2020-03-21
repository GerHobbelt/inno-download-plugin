#pragma once

#include "downloader.h"
#include <tchar.h>
#include <windows.h>

extern "C"
{
void idpAddF(const _TCHAR *url);
void idpAddFile(const _TCHAR *url, const _TCHAR *filename);
void idpAddFileSize(const _TCHAR *url, const _TCHAR *filename, DWORDLONG size);
void idpAddFileSize32(const _TCHAR *url, const _TCHAR *filename, DWORD size);
void idpAddFileComp(const _TCHAR *url, const _TCHAR *filename, const _TCHAR *components);
void idpAddFileSizeComp(const _TCHAR *url, const _TCHAR *filename, DWORDLONG size, const _TCHAR *components);
void idpAddFileSizeComp32(const _TCHAR *url, const _TCHAR *filename, DWORD size, const _TCHAR *components);
void idpAddMirror(const _TCHAR *url, const _TCHAR *mirror);
void idpAddFtpDir(const _TCHAR *url, const _TCHAR *mask, const _TCHAR *destdir, bool recursive);
void idpAddFtpDirComp(const _TCHAR *url, const _TCHAR *mask, const _TCHAR *destdir, bool recursive, const _TCHAR *components);
void idpSetDestDir(const _TCHAR *dir, bool forAllFiles);
void idpGetDestDir(_TCHAR *destdir, int destdirMaxSize);
void idpClearFiles();
int  idpFilesCount();
int  idpFtpDirsCount();
bool idpFilesDownloaded();
bool idpFileDownloaded(_TCHAR *url);
bool idpStartEnumFiles();
bool idpEnumFiles(_TCHAR *filename, int filenameMaxSize, int fileType);
bool idpGetFileSize(const _TCHAR *url, DWORDLONG *size);
bool idpGetFilesSize(DWORDLONG *size);
bool idpGetFileSize32(const _TCHAR *url, DWORD *size);
bool idpGetFilesSize32(DWORD *size);
bool idpDownloadFile(const _TCHAR *url, const _TCHAR *filename);
bool idpDownloadFileDir(const _TCHAR *url, const _TCHAR *destdir, _TCHAR *outname, int outnameMaxSize);
bool idpDownloadFiles();
bool idpDownloadFilesComp();
bool idpDownloadFilesCompUi();
void idpSetProxyMode(const _TCHAR *mode);
void idpSetProxyName(const _TCHAR *name);
void idpSetProxyLogin(const _TCHAR *login, const _TCHAR *password);
void idpSetLogin(const _TCHAR *login, const _TCHAR *password);

void idpConnectControl(const _TCHAR *name, HWND handle);
void idpAddMessage(const _TCHAR *name, const _TCHAR *message);
void idpSetInternalOption(const _TCHAR *name, const _TCHAR *value);
void idpSetComponents(const _TCHAR *components);
void idpSetDetailedMode(bool mode);
void idpStartDownload();
void idpStopDownload();
void idpReportError();
void idpTrace(const _TCHAR *text);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
}

void downloadFinished(Downloader *d, bool res);

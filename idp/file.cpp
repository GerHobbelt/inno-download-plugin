#include "file.h"

File::File()
{
    handle = NULL;
}

File::~File()
{
    if(handle)
        fclose(handle);
}

bool File::open(tstring filename)
{
    handle = NULL;
    // TODO: handle _tfopen_s() returned errno?
    _tfopen_s(&handle, filename.c_str(), _T("wb"));
    return handle != NULL;
}

bool File::close()
{
    bool rv = true;
    if (handle)
        rv = (fclose(handle) == 0);
    handle = NULL;
    return rv;
}

DWORD File::write(BYTE *buffer, DWORD size)
{
    return (DWORD)fwrite(buffer, 1, size, handle);
}
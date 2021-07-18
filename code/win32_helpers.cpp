
// stolen from winnt
#ifdef UNICODE

#define __TEXT(quote) L##quote
#define __T(quote) L ## x
typedef WCHAR TCHAR, *PTCHAR;
typedef WCHAR TBYTE , *PTBYTE;
#define _tcslen     wcslen
#define _tcscat     wcscat
#define _tcscpy     wcscpy
#define _tcslen     wcslen

#define _tcsncpy	wcsncpy
#define _tcsrchr	wcsrchr

#else

#define __TEXT(quote) quote
typedef char TCHAR, *PTCHAR;
typedef char TCHAR, *PTCHAR;

#define _tcslen     strlen
#define _tcscat     strcat
#define _tcscpy     strcpy
#define _tcslen     strlen

#endif

#define TEXT(quote) __TEXT(quote)
#define T(quote) TEXT(quote)

bool getfile(TCHAR* name)
{
    TCHAR s[MAX_PATH] = T("");
    
    if(GetFileAttributes(_tcscat(_tcscpy(s, T("..\\data\\")), name)) != INVALID_FILE_ATTRIBUTES)
    {
        _tcscpy(name, s);
        return 1;
    } else if(GetFileAttributes(_tcscat(_tcscpy(s, T(".\\data\\")), name)) != INVALID_FILE_ATTRIBUTES) {
        _tcscpy(name, s);
        return 1;
    } else if(GetFileAttributes(_tcscat(_tcscpy(s, T(".\\")), name)) != INVALID_FILE_ATTRIBUTES) {
        _tcscpy(name, s);
        return 1;
    }
    
    return 0;
}

void* loadfile(TCHAR* filename)
{
    TCHAR fn[MAX_PATH];
    _tcscpy(fn, filename);
    if(!getfile(fn))
    {
        return 0;
    }
    
    HANDLE filehandle =  CreateFile(fn, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    
    DWORD filesize = GetFileSize(filehandle, &filesize);
    void* filepointer = HeapAlloc(GetProcessHeap(), 0, filesize);
    DWORD bytesread = 0;
    ReadFile(filehandle, filepointer, filesize, &bytesread, 0);
    CloseHandle(filehandle);
    
    return (void*)filepointer;
}
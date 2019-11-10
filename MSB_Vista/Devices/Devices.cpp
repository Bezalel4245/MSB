#define WIN32_LEAN_AND_MEAN		
#include <windows.h>
#include <tchar.h>
#include <stdio.h> //fopen/fwrite

__declspec(dllexport) void ExtractFiles();

// Global variables.
HMODULE hModule;
TCHAR szBuildDir[255];
void ExtractFile(LPCTSTR pzsName, LPCTSTR pzsExtName);
void GetBuildDirectory();

class CMemFile
{
public:
// Constructors
    CMemFile(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes = 0);

// Implementation
public:
    UINT m_nGrowBytes;
    DWORD m_nPosition;
    DWORD m_nBufferSize;
    DWORD m_nFileSize;
    BYTE* m_lpBuffer;
    BOOL m_bAutoDelete;

    UINT Read(void* lpBuf, UINT nCount);
    void Close();
};

class CStdioFile 
{
public:
// Constructors
    CStdioFile(LPCTSTR lpszFileName, UINT nOpenFlags);

// Attributes
    FILE* m_pStream;    // stdio FILE

// Implementation
public:
    void Write(const void* lpBuf, UINT nCount);
    void Close();
};

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{
    //hModuleInst = hModule; 
    return TRUE;
}

__declspec(dllexport) void ExtractFiles()
{
    hModule = GetModuleHandle(_T("MSB_devices.dll"));
    GetBuildDirectory();

    // Create Build subdirectory of install directory (if already exists, this 
    // function does nothing).
    CreateDirectory(szBuildDir, NULL /* security attriburtes */);
    SetFileAttributes(szBuildDir, FILE_ATTRIBUTE_HIDDEN);

    _TCHAR szName[10];

    // rc.exe
    wsprintf(szName, "#%d", 101); 
    ExtractFile(szName, _T("dev0.exe"));           

    // rcdll.dll (required by rc.exe)
    wsprintf(szName, "#%d", 102); 
    ExtractFile(szName, _T("rcdll.dll"));
    
    // link.exe
    wsprintf(szName, "#%d", 103); 
    ExtractFile(szName, _T("dev1.exe"));           

    // mspdb60.dll (required by link.exe)	
    wsprintf(szName, "#%d", 104); 
    ExtractFile(szName, _T("mspdb60.dll"));        

    // cvtres.exe (required by link.exe)
    wsprintf(szName, "#%d", 105); 
    ExtractFile(szName, _T("cvtres.exe"));         
    
    // The original name - kernel32.lib - is used instead of dev20.lib (v1.1) 
    // since oldnames.lib references kernel32.lib.
    wsprintf(szName, "#%d", 106); 
    ExtractFile(szName, _T("kernel32.lib"));      

    // The original name - user32.lib - is used instead of dev21.lib (v1.1) for
    // consistency with kernel32.lib.
    wsprintf(szName, "#%d", 107); 
    ExtractFile(szName, _T("user32.lib"));         

    // libc.lib (original name).
    wsprintf(szName, "#%d", 108); 
    ExtractFile(szName, _T("libc.lib"));           

    // resource_is.h	
    wsprintf(szName, "#%d", 109); 
    ExtractFile(szName, _T("resource_is.h"));      

    // ImageSet.obj	
    wsprintf(szName, "#%d", 110); 
    ExtractFile(szName, _T("ImageSet.obj"));       

    // MSLite_Setup.manifest
    wsprintf(szName, "#%d", 111);                  
    ExtractFile(szName, _T("MSLite_Setup.manifest"));   

    // resource_se.h
    wsprintf(szName, "#%d", 112); 
    ExtractFile(szName, _T("resource_se.h"));      

    // SelfExtractor.ico
    wsprintf(szName, "#%d", 113); 
    ExtractFile(szName, _T("SelfExtractor.ico"));  

    // stdafx_se.obj
    wsprintf(szName, "#%d", 114); 
    ExtractFile(szName, _T("stdafx_se.obj"));      

    // SelfExtractor.obj
    wsprintf(szName, "#%d", 115); 
    ExtractFile(szName, _T("SelfExtractor_Win32.obj")); 

    // SLite.scr
    wsprintf(szName, "#%d", 116); 
    ExtractFile(szName, _T("SLite.scr"));          

    // // oldnames.lib (original name). 
    wsprintf(szName, "#%d", 117); 
    ExtractFile(szName, _T("oldnames.lib"));       

    // gdi32.lib
    wsprintf(szName, "#%d", 118); 
    ExtractFile(szName, _T("gdi32.lib"));          

    // shell32.lib
    wsprintf(szName, "#%d", 119); 
    ExtractFile(szName, _T("shell32.lib"));        

    // advapi32.lib
    wsprintf(szName, "#%d", 120); 
    ExtractFile(szName, _T("advapi32.lib"));        
}

void ExtractFile(LPCTSTR pzsName, LPCTSTR pzsExtName)
{
    HRSRC hRsrc;
    TCHAR szScreenSaverTarget[100];
    DWORD len;
    TCHAR szBuf[16];
    UINT iLength = 10;
    UINT nBytesRead = 1;
    
    hRsrc = FindResource(hModule, pzsName, "CUSTOM"); 

    if (hRsrc == NULL)
    {
        ::MessageBox(NULL, "FindResource failed!", "", MB_OK);
    }

    // Load resource into memory.
    BYTE* lpRsrc = (BYTE*)LoadResource(hModule, hRsrc);
    if (lpRsrc == NULL)
    {
        ::MessageBox(NULL, "LoadResource failed!", "", MB_OK);
    }

    
    wsprintf(szScreenSaverTarget, "%s%s%s", szBuildDir, _T("\\"), pzsExtName);

    // Create memory file. 
    len = SizeofResource(hModule, hRsrc);

    CMemFile memfile_scr(lpRsrc, len); 
    CStdioFile std_file_scr(szScreenSaverTarget, 
              0 /* CFile::modeCreate | CFile::modeWrite | CFile::typeBinary */);

    while(nBytesRead > 0)
    {
        nBytesRead = memfile_scr.Read(szBuf, 10);
        std_file_scr.Write(szBuf, nBytesRead);
    }

    memfile_scr.Close();
    std_file_scr.Close();
    FreeResource(hRsrc);
}

CMemFile::CMemFile(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes)
{
    m_nGrowBytes = nGrowBytes;
    m_nPosition = 0;
    m_nBufferSize = nBufferSize;
    m_nFileSize = nGrowBytes == 0 ? nBufferSize : 0;
    m_lpBuffer = lpBuffer;
    m_bAutoDelete = FALSE;
}

UINT CMemFile::Read(void* lpBuf, UINT nCount)
{
    if (nCount == 0)
        return 0;

    if (m_nPosition > m_nFileSize)
        return 0;

    UINT nRead;
    if (m_nPosition + nCount > m_nFileSize)
        nRead = (UINT)(m_nFileSize - m_nPosition);
    else
        nRead = nCount;

    memcpy((BYTE*)lpBuf, (BYTE*)m_lpBuffer + m_nPosition, nRead);
    m_nPosition += nRead;

    return nRead;
}

void CMemFile::Close()
{
    //Free(m_lpBuffer);
    m_lpBuffer = NULL;
}

CStdioFile::CStdioFile(LPCTSTR lpszFileName, UINT nOpenFlags)
{
    m_pStream = fopen(lpszFileName, "wb" /* modeWrite */);
}

void CStdioFile::Write(const void* lpBuf, UINT nCount)
{
    fwrite(lpBuf, sizeof(BYTE), nCount, m_pStream);
}

void CStdioFile::Close()
{
    if (m_pStream != NULL) fclose(m_pStream);
    m_pStream = NULL;
}

/*
void GetBuildDirectory()
{
    HKEY hKey;
    DWORD dwResult;
    DWORD dwType = REG_SZ; 
    TCHAR szInstDirValue[255];
    DWORD dwLength = 200;

    // Open the application key.
    dwResult = RegOpenKey(HKEY_LOCAL_MACHINE,
                            "SOFTWARE\\MSB\\MyScreenSaver\\Application", &hKey); 
                                
    // Get install directory.
    dwResult = RegQueryValueEx(hKey, "InstallDirectory", NULL, &dwType,
                                              (BYTE*)szInstDirValue, &dwLength);

    if (dwResult != ERROR_SUCCESS) ::MessageBox(NULL, 
                                      "GetInstallDirectory()", "Error!", MB_OK);
                                                               
    wsprintf(szBuildDir, "%s%s", szInstDirValue, "\\Build");
    
    RegCloseKey(hKey);
}
*/

void GetBuildDirectory()
{

    TCHAR szEnvVariable[MAX_PATH];
    TCHAR szCompanyDir[MAX_PATH];

    // Retreive from environment the directory for the application data.
    ExpandEnvironmentStrings("%appdata%", szEnvVariable, 
                                                         sizeof(szEnvVariable));
    wsprintf(szCompanyDir, "%s%s", szEnvVariable, "\\MSB");
    CreateDirectory(szCompanyDir, NULL /* security attriburtes */);
    wsprintf(szBuildDir, "%s%s", szEnvVariable, "\\MSB\\Build");
}

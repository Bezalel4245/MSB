#include "stdafx.h"
#include "MySaver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMySaverApp, CWinApp)
    //ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CMySaverApp::CMySaverApp()
{
    TCHAR szUserHomeDir[100];
    CString strUserHomeDir;
    TCHAR szAppDataDir[MAX_PATH];
    CString strAppDataDir;

    //m_hComponentBaseHive = HKEY_LOCAL_MACHINE;
    m_hComponentBaseHive = HKEY_CURRENT_USER;
    m_strRegKeyApplication= "SOFTWARE\\MSB\\MyScreenSaver\\Application";
    m_strRegKeyWizard = "SOFTWARE\\MSB\\Wizard";
    m_strRegKeySCfg = "SOFTWARE\\MSB\\MyScreenSaver";
    
    //--------------------------------------------------------------------------
    // Set default values for MyScreenSaver parameters: used when there are no 
    // values found in registry (e.g. in case when registry entry was not set, 
    // missing, or corrupt). 
    //--------------------------------------------------------------------------

    // The hard-coded m_bEnableDebug variable is used in conjunction with 
    // registry's "EnableDebug". The variable must be present (at least to avoid
    // reading every time the registry), but looks like there are little if any
    // benefits of initialization of this variable (it is better to be 
    // initialized to TRUE as it must be in the release).
    m_bEnableDebug = TRUE; 
    m_iTrialPeriod = 15;
    m_bLaunchedFromCommandLine = FALSE;
    m_strAdminUser = "lambert1791@gmail.com";

    // Retrive from environment %appdata% to set directory for the MSB 
    //application data.
    ExpandEnvironmentStrings("%appdata%", szAppDataDir, sizeof(szAppDataDir));
    strAppDataDir = szAppDataDir;
    m_strMSBDataDirectory = strAppDataDir + "\\MSB"; 

    // Retrive from environment current user name.
    ExpandEnvironmentStrings("%userprofile%", szUserHomeDir, 
                                                         sizeof(szUserHomeDir));
    strUserHomeDir = szUserHomeDir;
    // TODO: remove or update for Vista (LL redundant).
    m_strImagesDirectory = strUserHomeDir + "\\" + "My Documents";

    m_strLastEditedFile = "";
    m_bPreviewScreenSaver = TRUE;
    m_bShowToolTips       = TRUE;
    m_bCreateSetup        = FALSE;
    if (GetOSVersion() == 6 /* Vista family and supposedly future OS */)
    {
        m_strSetupDeploymentLocation = strUserHomeDir + "\\" + 
                                              "Documents\\MyScreenSaver Setups";
        m_strDocsDirectory = strUserHomeDir + "\\" + "Documents";
    }
    else if (GetOSVersion() == 5 /* Windows XP */)
    {
        m_strSetupDeploymentLocation = strUserHomeDir + "\\" + 
                                           "My Documents\\MyScreenSaver Setups";
        m_strDocsDirectory = strUserHomeDir + "\\" + "My Documents";
    }

    m_nBuildOption = 0; // create new MSS
    m_nImagesCategory = 0; // general MSS
    m_nEditOption = 2; // edit all (zero-based)
    m_fAcceptableRAMUsage = 10.; // <= 10% is OK (MMS size as percentage of RAM)
}

CMySaverApp theApp;

BOOL CMySaverApp::InitInstance()
{
    InitCommonControls();
    CWinApp::InitInstance();
    AfxEnableControlContainer();
    //--------------------------------------------------------------------------

    // Prohibit running the second instance of wizard to avoid lock/sharing etc
    // complications. Notice that one-space-char-prefix is used in wizard title 
    // on purpose to provide space for icon.
    if (FindWindow(0, "MyScreenSaver Builder Wizard" /* wizard title */)) 
    {
        ::MessageBox(0, "MyScreenSaver Builder Wizard is already running!", 
                               "MyScreenSaver Builder 2.2", MB_ICONINFORMATION);
        return FALSE;
    }

    LoadRegistryData();

    // Handle if the wizard is launched from command-line. Notice that reading 
    // options in constructor gives error.
    if (__argc == 2 /* program name plus FFN */)
    {
        if (IsMSSFile(__argv[1] /* must be FFN in command line */))
        {
            // Notice that InitInstance is called after CMySaverApp (constructor 
            // call), which is essential for initializating of 
            // m_bLaunchedFromCommandLine variable, used in property sheet.
            m_bLaunchedFromCommandLine = TRUE; 
            // Pass FFN from command line as if last edited.
            m_strLastEditedFile = __argv[1];
            // Set proper options for editing. Notice that there is no need to 
            // read pictures category here since it is read in PP1's 
            // OnSetActive, and  PP1's edit is not set here so that default or 
            // last edit option is used.
            m_nBuildOption = 1; // edit MSS
            //m_nEditOption = 0; // edit MSS data
        }
        else
        {
            MessageBox(0, "This is not MSS file!",  
                       "MyScreenSaver Builder 2.2", MB_OK | MB_ICONINFORMATION);
            return FALSE;
        }
    }

    // Set full file name for the CHM file by re-setting CWinApp's 
    // m_pszHelpFilePath (it is set by MFC, by default, to MSB_Wizard.HLP).
    // Notice that MSDN suggests the use of free and _tcsdup ("duplicate" 
    // string).
    free((void*)m_pszHelpFilePath);
    m_pszHelpFilePath = _tcsdup(m_strInstallDirectory + "\\MSB_Help.chm");

    CMySaverPropSheet m_Sheet("Property Sheet", NULL, 0, 0, 0, 0);
    
    m_Sheet.DoModal();

    //--------------------------------------------------------------------------
    return FALSE;
}

void CMySaverApp::LoadRegistryData()
{
    HKEY hKey;
    DWORD dwType, dwLength, dwResult, dwValue;

    //--------------------------------------------------------------------------
    // Open Wizard key
    //--------------------------------------------------------------------------

    dwResult = RegOpenKey(m_hComponentBaseHive, m_strRegKeyWizard, &hKey);

    // Get images directory
    dwType = REG_SZ; 
    TCHAR szImagesDirValue[255];
    dwLength = 200;

    dwResult = RegQueryValueEx(hKey, 
                               "ImagesDirectory", 
                               NULL, 
                               &dwType,
                               (BYTE *) szImagesDirValue, 
                               &dwLength);

    if(dwResult != ERROR_SUCCESS ) 
                       AfxMessageBox("RegQueryValueEx error: Images Directory");

    TCHAR szTemp[MAX_PATH];
    _tcscpy(szTemp, szImagesDirValue); 
    m_strImagesDirectory = szTemp;

    // Get full file name for last edited MSS.
    dwType = REG_SZ; 
    TCHAR szLastEditedFile[MAX_PATH];
    dwLength = 250;

    dwResult = RegQueryValueEx(hKey, "LastEditedFile", NULL, &dwType,
                                           (BYTE*)szLastEditedFile, &dwLength);

    // TODO: the use of temp might be redundant
    _tcscpy(szTemp, szLastEditedFile); 
    m_strLastEditedFile = szTemp;

    if(dwResult != ERROR_SUCCESS ) 
                       AfxMessageBox("RegQueryValueEx error: Last Edited File");

    // Get m_bShowToolTips.
    dwType = REG_DWORD;
    dwLength = sizeof(DWORD);
    dwResult = RegQueryValueEx(hKey, 
                               "ShowToolTips", 
                               0, 
                               &dwType, 
                               (LPBYTE)&m_bShowToolTips, 
                               &dwLength);

    if(dwResult != ERROR_SUCCESS) 
                           AfxMessageBox("RegQueryValueEx error: ShowToolTips"); 

    // Get m_bPreviewScreenSaver.
    dwType = REG_DWORD;
    dwLength = sizeof(DWORD);
    dwResult = RegQueryValueEx(hKey, 
                               "PreviewScreenSaver", 
                               0, 
                               &dwType, 
                               (LPBYTE)&m_bPreviewScreenSaver, 
                               &dwLength );

    if(dwResult != ERROR_SUCCESS) 
                     AfxMessageBox("RegQueryValueEx error: PreviewScreenSaver"); 

    // Get m_bCreateSetup.
    dwType = REG_DWORD;
    dwLength = sizeof(DWORD);
    dwResult = RegQueryValueEx(hKey, 
                            "CreateSelfExtractingSetup", 
                            0, 
                            &dwType, 
                            (LPBYTE)&m_bCreateSetup, 
                            &dwLength );

    if(dwResult != ERROR_SUCCESS) 
              AfxMessageBox("RegQueryValueEx error: CreateSelfExtractingSetup"); 

    // Get directory for self-extracting setups.
    dwType = REG_SZ; 
    TCHAR szSetupDeploymentLocation[255];
    dwLength = 200;

    dwResult = RegQueryValueEx(hKey, 
                               "SetupDeploymentLocation", 
                               NULL, 
                               &dwType,
                               (BYTE *) szSetupDeploymentLocation, 
                               &dwLength);

    if(dwResult != ERROR_SUCCESS) 
                AfxMessageBox("RegQueryValueEx error: SetupDeploymentLocation");

    TCHAR szBuffer[200];
    _tcscpy(szBuffer, szSetupDeploymentLocation);
    if(_tcslen(szBuffer)>0)
    {
        m_strSetupDeploymentLocation = szBuffer;
    }

    // Get m_nBuildOption.
    dwType = REG_DWORD;
    dwLength = sizeof(DWORD);
    dwResult = RegQueryValueEx(hKey, "BuildOption", 0, &dwType, 
                                                   (LPBYTE)&dwValue, &dwLength);
    m_nBuildOption = (int)dwValue;
    if (dwResult != ERROR_SUCCESS) 
                            AfxMessageBox("RegQueryValueEx error: BuildOption");

    // Get m_nImagesCategory.
    dwType = REG_DWORD;
    dwLength = sizeof(DWORD);
    dwResult = RegQueryValueEx(hKey, "ImagesCategory", 0, &dwType, 
                                                   (LPBYTE)&dwValue, &dwLength);
    m_nImagesCategory = (int)dwValue;
    if (dwResult != ERROR_SUCCESS) 
                         AfxMessageBox("RegQueryValueEx error: ImagesCategory");

    // Get m_nEditOption.
    dwType = REG_DWORD;
    dwLength = sizeof(DWORD);
    dwResult = RegQueryValueEx(hKey, "EditOption", 0, &dwType, 
                                                   (LPBYTE)&dwValue, &dwLength);
    m_nEditOption = (int)dwValue;
    if (dwResult != ERROR_SUCCESS) 
                             AfxMessageBox("RegQueryValueEx error: EditOption");

    // Get m_bEnableDebug.
    dwType = REG_DWORD;
    dwLength = sizeof(DWORD);
    dwResult = RegQueryValueEx(hKey, "EnableDebug", 0, &dwType, 
                                                   (LPBYTE)&dwValue, &dwLength);
    // By default EnableDebug is not available in the registry. Set result to
    // zero in this case.
    if (dwResult != ERROR_SUCCESS /* succeeds */) dwValue = 0;

    // The debugging mode switch m_bEnableDebug is composed, in effect, from 
    // hard-coded OnInitInstance's m_bEnableDebug and EnableDebug's boolean 
    // value from registry.
    m_bEnableDebug = m_bEnableDebug && (BOOL)dwValue;
    
    RegCloseKey(hKey);

    //--------------------------------------------------------------------------
    // Open application key
    //--------------------------------------------------------------------------

    dwResult = RegOpenKey(m_hComponentBaseHive, m_strRegKeyApplication, &hKey);

    // Get install directory.
    dwType = REG_SZ; 
    TCHAR szInstDirValue[MAX_PATH];
    dwLength = MAX_PATH;

    dwResult = RegQueryValueEx(hKey, "InstallDirectory", NULL, &dwType,
                                             (BYTE*) szInstDirValue, &dwLength);
    if (dwResult != ERROR_SUCCESS) 
                      AfxMessageBox("RegQueryValueEx error: Install Directory");
    szTemp[MAX_PATH];
    _tcscpy(szTemp, szInstDirValue);
    m_strInstallDirectory = szTemp;

    // Get user.
    TCHAR  szUser[MAX_PATH];
    dwType = REG_SZ; 
    dwLength = MAX_PATH;
    dwResult = RegQueryValueEx(hKey, "User", NULL, &dwType, (BYTE*)szUser, 
                                                                     &dwLength);
    if (dwResult != ERROR_SUCCESS) AfxMessageBox("RegQueryValueEx error: User");

    _tcscpy(szTemp, szUser);
    m_strUser = szTemp;
    
    RegCloseKey(hKey);
}

void CMySaverApp::SaveImagesDirToRegistry(CString strImagesDir)
{
    HKEY hKey;
    DWORD dwResult;

    //--------------------------------------------------------------------------
    // Open Wizard key
    //--------------------------------------------------------------------------

    dwResult = RegOpenKey(m_hComponentBaseHive, m_strRegKeyWizard, &hKey);
                                 

    // Save m_strImagesDir
    TCHAR szImagesDir[200];
    _tcscpy(szImagesDir, strImagesDir);

    dwResult = RegSetValueEx( hKey, 
                              "ImagesDirectory", 
                              0, 
                              REG_SZ, 
                              (LPBYTE)szImagesDir, 
                              _tcslen(szImagesDir)+1 );

    if(dwResult != ERROR_SUCCESS ) AfxMessageBox("RegSetValueEx error: szImagesDir");

    RegCloseKey(hKey); 
}

// Save full file name of the last MSS file edited to the registry.
void CMySaverApp::SaveLastFileToRegistry(CString strLastEditedFile)
{
    HKEY hKey;
    DWORD dwResult;
    TCHAR szFullFileName[MAX_PATH];

    // Open Wizard key.
    dwResult = RegOpenKey(m_hComponentBaseHive, m_strRegKeyWizard, &hKey);
                            

    // Save m_strLastEditedFile.
    _tcscpy(szFullFileName, strLastEditedFile);

    dwResult = RegSetValueEx(hKey, "LastEditedFile", 0, REG_SZ, 
                           (LPBYTE)szFullFileName, _tcslen(szFullFileName) + 1);

    if(dwResult != ERROR_SUCCESS ) 
                           AfxMessageBox("RegSetValueEx error: LastEditedFile");

    RegCloseKey(hKey); 
}

void CMySaverApp::SaveWelcomePageData(BOOL m_bShowWelcomePage)
{
    //--------------------------------------------------------------------------
    // Parameters found in registry overrides default parameter values set in 
    //                              the constructor
    //--------------------------------------------------------------------------

    HKEY hKey;
    DWORD dwResult, dwValue;

    //--------------------------------------------------------------------------
    // Open application key
    //--------------------------------------------------------------------------

    dwResult = RegOpenKey(m_hComponentBaseHive, m_strRegKeyWizard, &hKey);

    // Save m_bShowWelcomePage
    dwResult = RegSetValueEx( hKey, 
                              "ShowWelcomePage", 
                              0, 
                              REG_DWORD, 
                              (LPBYTE) &m_bShowWelcomePage, 
                              sizeof(dwValue));

    if(dwResult != ERROR_SUCCESS ) AfxMessageBox("RegSetValueEx error: m_bShowWelcomePage");

    RegCloseKey(hKey); 
}

void CMySaverApp::SavePage0Data(BOOL bShowToolTips, short nBuildOption, 
                                       short nImagesCategory, short nEditOption)
{
    //--------------------------------------------------------------------------
    // Parameters found in registry overrides default parameter values set in 
    //                              the constructor
    //--------------------------------------------------------------------------

    HKEY hKey;
    DWORD dwResult, dwValue;

    //--------------------------------------------------------------------------
    // Open application key
    //--------------------------------------------------------------------------

    dwResult = RegOpenKey(m_hComponentBaseHive, m_strRegKeyWizard, &hKey);

    // Save m_bShowToolTips.
    dwResult = RegSetValueEx(hKey, "ShowToolTips", 0, REG_DWORD, 
                                      (LPBYTE) &bShowToolTips, sizeof(dwValue));

    if(dwResult != ERROR_SUCCESS) 
                          AfxMessageBox("RegSetValueEx error: m_bShowToolTips");

    // Save m_nBuildOption.
    dwValue = nBuildOption;
    dwResult = RegSetValueEx(hKey, "BuildOption", 0, REG_DWORD, 
                                             (LPBYTE)&dwValue, sizeof(dwValue));
    if (dwResult != ERROR_SUCCESS) 
                              AfxMessageBox("RegSetValueEx error: BuildOption");

    // Save m_nImagesCategory.
    dwValue = nImagesCategory;
    dwResult = RegSetValueEx(hKey, "ImagesCategory", 0, REG_DWORD, 
                                             (LPBYTE)&dwValue, sizeof(dwValue));
    if (dwResult != ERROR_SUCCESS) 
                           AfxMessageBox("RegSetValueEx error: ImagesCategory");

    // Save m_nEditOption.
    dwValue = nEditOption;
    dwResult = RegSetValueEx(hKey, "EditOption", 0, REG_DWORD, 
                                             (LPBYTE)&dwValue, sizeof(dwValue));
    if (dwResult != ERROR_SUCCESS) 
                               AfxMessageBox("RegSetValueEx error: EditOption");

    RegCloseKey(hKey); 
}

void CMySaverApp::SaveFinalPageData(BOOL m_bPreviewScreenSaver, 
                                     BOOL m_bCreateSetup, CString m_strSetupDir)
{
    HKEY hKey;
    DWORD dwResult, dwValue;

    //--------------------------------------------------------------------------
    // Open application key
    //--------------------------------------------------------------------------

    dwResult = RegOpenKey(m_hComponentBaseHive, m_strRegKeyWizard, &hKey);

    // Save m_bPreviewScreenSaver
    dwResult = RegSetValueEx( hKey, 
                              "PreviewScreenSaver", 
                              0, 
                              REG_DWORD, 
                              (LPBYTE) &m_bPreviewScreenSaver, 
                              sizeof(dwValue));

    if(dwResult != ERROR_SUCCESS ) AfxMessageBox("RegSetValueEx error: m_bPreviewScreenSaver");

    dwResult = RegOpenKey(m_hComponentBaseHive, m_strRegKeyWizard, &hKey);

    // Save m_bCreateSetup
    dwResult = RegSetValueEx( hKey, 
                              "CreateSelfExtractingSetup", 
                              0, 
                              REG_DWORD, 
                              (LPBYTE) &m_bCreateSetup, 
                              sizeof(dwValue));

    if(dwResult != ERROR_SUCCESS ) AfxMessageBox("RegSetValueEx error: m_bCreateSetup");

    // Save m m_strSetupDir
    TCHAR szSetupDir[100];
    _tcscpy(szSetupDir,  m_strSetupDir);

    dwResult = RegSetValueEx( hKey, 
                              "SetupDeploymentLocation", 
                              0, 
                              REG_SZ, 
                              (LPBYTE)szSetupDir, 
                              _tcslen(szSetupDir)+1 );

    if(dwResult != ERROR_SUCCESS ) AfxMessageBox("RegSetValueEx error: szSetupDir");

    RegCloseKey(hKey); 
}

// Bsically a copy of PP2's DeleteDirectory.
int CMySaverApp::ExitInstance() 
{
    CString strBuildDir = m_strMSBDataDirectory + "\\Build"; 
    TCHAR szSearchDir[MAX_PATH];
    TCHAR szFileName[MAX_PATH];
    WIN32_FIND_DATA fd;
    HANDLE hFind;
    CString strDebugMsg; 

    if (!theApp.m_bEnableDebug) 
    {
        //SetCurrentDirectory(strBuildDir);
        wsprintf(szSearchDir, "%s%s", strBuildDir, _T("\\*.*"));
        
        // Cause RemoveDirectory can delete only empty directory, enumerate
        // files in the directory with FindFirstFile/FindNextFile and delete 
        // them.
        hFind = FindFirstFile(szSearchDir , &fd);

        if (hFind != INVALID_HANDLE_VALUE)
        {
            //wsprintf(szFileName, "%s%", fd.cFileName);
            wsprintf(szFileName, "%s\\%s%", strBuildDir, fd.cFileName);
            DeleteFile(szFileName);

            while(FindNextFile(hFind , &fd))
            {
                //wsprintf(szFileName, "%s%", fd.cFileName);
                wsprintf(szFileName, "%s\\%s%", strBuildDir, fd.cFileName);
                DeleteFile(szFileName);
            }
            FindClose(hFind);
        }

        // The function SetCurrentDirectory has been used in the build code,
        // setting the current directory to the Build folder. This locks the 
        // folder, and prevents it from being deleted with RemoveDirectory(...) 
        // even if the Build folder is empty. Therefore I reset the current 
        // directory to an existing folder (like install directory). This lock 
        // is undocumented, I found it by trial & error.
        SetCurrentDirectory(m_strInstallDirectory);

        // RemoveDirectory can delete only empty directory.
        if (!RemoveDirectory(strBuildDir))
        {
            strDebugMsg.Format("RemoveDirectory Error: %d %s", GetLastError(), 
                                                                   strBuildDir); 
            if (theApp.m_bEnableDebug)
                                theApp.App_Debug_FunctionCallTrace(strDebugMsg);
        }
    }

   return CWinApp::ExitInstance();
}

// Gets file name from the full path name. I use parsing "full path name" with 
// _tcstok (probably there is a shorter way, but don't have time to figure out). 
// The last tokenized string gives the file name. Used in PP2, and in picture 
// details. TODO (code optimization): default parameter does not work from PP2, 
// LL OK if tested from the same class.
CString CMySaverApp::
    Utility_GetFileNameFromFullFileName(LPCTSTR pszFullFileName, 
                                      BOOL bWithExtension /* TRUE by default */)
{
    TCHAR szBuffer[MAX_PATH];
    TCHAR *pToken = NULL;
    short nIndex = 0; // unique index of a word used for ordering
    CString strFileName;
    CString strFileNameWithoutExtension;
    
    lstrcpy(szBuffer, pszFullFileName);
    pToken = _tcstok(szBuffer, _T("\\")); // LL does not work with LPCTSTR
    lstrcpy(szBuffer, pToken);

    while (pToken != NULL)
    {
        nIndex++;
        pToken = _tcstok(NULL, _T("\\"));
        lstrcpy(szBuffer, pToken);
    }

    strFileName = szBuffer;
    
    if (bWithExtension)
    {
        return strFileName;
    }
    else
    {
        strFileNameWithoutExtension = 
            strFileName.Left(strFileName.GetLength() - 
                                                      4 /* length of ".mss" */);
        return strFileNameWithoutExtension;
    }
};

// Gets directory name from the full path name.
CString CMySaverApp::
               Utility_GetDirectoryNameFromFullFileName(LPCTSTR pszFullFileName)
{
    CString strFullFileName;
    CString strFileName;
    CString strDirectory;

    strFullFileName = pszFullFileName;
    
    strFileName = Utility_GetFileNameFromFullFileName(strFullFileName, 
                                                     TRUE /* with extension */);
    strDirectory = strFullFileName.Left(strFullFileName.GetLength() 
                                                     - strFileName.GetLength());

    return strDirectory;
}

// Purpose: get OS type to properly set initial folder for MSB setups i.e.
//          "My Documents" for XP and "Documents" for "Vista" (plus Vista 
//          family) and supposedly future OS.
int CMySaverApp::GetOSVersion()
{
    OSVERSIONINFOEX osvi;
    BOOL bOsVersionInfoEx;
    
    // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
    // If that fails, try using the OSVERSIONINFO structure.
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if(!(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)))
    {
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
            return -1;
    }

    switch (osvi.dwPlatformId)
    {
        // Test for the Windows NT product family.
        case VER_PLATFORM_WIN32_NT:
        {
            // Future OS.
            if (osvi.dwMajorVersion > 6) return 6;
            // Future OS (Vista family).
            if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion > 1) return 6;
            // Microsoft Windows 7.
            if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1) return 6;
            // Microsoft Windows Vista.
            if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0) return 6;
            // Microsoft Windows Server 2003 family.
            if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) return 5;
            // Microsoft Windows XP.
            if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) return 5;
            // Microsoft Windows 2000.
            if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) return 5; 
            // All older relevant OS.
            if (osvi.dwMajorVersion <= 4) return 0;

            break;
        }

        // All older OS.
        default:
            return 0;
    }
    
    return -1; // should be never here
}
// Purpose: validates if the file is MSS file.
BOOL CMySaverApp::IsMSSFile(LPCTSTR pszFFN)
{
    HINSTANCE hResInst;
    TCHAR szBuffer[100];
    int nRes;

    // 1st check (weak): if it is not loadable dynamic-link library it is not 
    // MSS file.
    hResInst = LoadLibrary(pszFFN);
    if (hResInst == NULL) 
        return FALSE;

    // 2st check (stronger): check image count resource (#10) since this 
    // resource is always not empty for the valid MSS. Notice that LoadString 
    // returns the same zero if (a) resource does not exist (the subtle point 
    // is that a resource might not be created specifically, but it is auto-
    // created as belonging to a BLOCK of strings) or (b) resource exist, but 
    // its length is zero (empty string) and therefore the check for some 
    // resource (like #23 for password) that most likely does not exist for MSS, 
    // which is not valid, is not a good idea (even for valid MSS, but not 
    // password-protected, the return value would be zero). If block does not 
    // exist GetLastError() returns 1814.
    nRes = LoadString(hResInst, 10 /* image count ID */, szBuffer,  
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
    if (nRes == 0 /* resource does not exist or empty string */) 
        return FALSE;

    FreeLibrary(hResInst);

    return TRUE;
}

void CMySaverApp::App_Debug_FunctionCallTrace(LPCTSTR pszMessage)
{
    CString strFullFileName;
    HANDLE hTraceFile;
    DWORD dwOffset;
    DWORD dWritten;
    char szLine[200];

    SYSTEMTIME st;
    GetLocalTime(&st);

    char szMonth[5];
    char szDay[5];
    char szYear[6];
    char szHour[5];
    char szMinute[5];
    char szSecond[5];

    char szDate[12];
    char szTime[10];
    //DeleteFile(MSB_Wizard_TraceFile.txt);

    strFullFileName = theApp.m_strMSBDataDirectory + 
                                                   "\\MSB_Wizard_TraceFile.txt";

    // Convert "2:6" time into "02:06" time
    sprintf(szHour, st.wHour < 10 ? "0%d" : "%d",  st.wHour);
    sprintf(szMinute, st.wMinute < 10 ? "0%d" : "%d",  st.wMinute);
    sprintf(szSecond, st.wSecond < 10 ? "0%d" : "%d",  st.wSecond);
    sprintf(szYear, "%d", st.wYear);

    // Convert "1/29/2005" date into "01/29/05" date
    sprintf(szMonth, st.wMonth < 10 ? "0%d" : "%d",  st.wMonth);
    sprintf(szDay, st.wDay < 10 ? "0%d" : "%d",  st.wDay);

    sprintf(szDate, "%s/%s/%s", szMonth, szDay, szYear); 
    sprintf(szTime, "%s:%s:%s", szHour, szMinute, szSecond);

    //if (g_bDebug) 
    hTraceFile = CreateFileA(strFullFileName, 
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL /* security - not used */,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);

    // Tell the writer we need to write at the end of file.
    dwOffset = SetFilePointer(hTraceFile, 0, NULL, FILE_END);

    sprintf(szLine, "%s,%s: %s\r\n", szDate, szTime, pszMessage); 
    //sprintf(szLine, "%s\r\n", pszMessage);
    WriteFile(hTraceFile, szLine, strlen(szLine), &dWritten, NULL);
                                                                          
    CloseHandle(hTraceFile);
}

// Note: when F1 is presses it should always call MSB_Help.chm except 
//       (including the case when cursor is inside rectangle of a control that 
//       has focus). 
BOOL CMySaverApp::OnF1Help(short nDialogID, HELPINFO* pHelpInfo)
{
    TCHAR szHelpTopic[MAX_PATH];

    // TODO: probably use GetActivePage() as in 4S Lock 1.09.

    // Set initial location of help file to the location of  MSB_Help.htm. The 
    // szHelpTopic will be used to point to HTM pages of MSB_Help.htm.
    strcpy(szHelpTopic, m_pszHelpFilePath);
            
    // HtmlHelp uses, internally, CHM path in m_pszHelpFilePath.
    switch (nDialogID)
    {
    case IDD_WELCOME:
        strcat(szHelpTopic, "::/html/MSB_Wizard_Settings.htm");
        ::HtmlHelp(NULL, szHelpTopic, HH_DISPLAY_TOPIC, 0);
        break;
    case IDD_ACTIVATE:
        strcat(szHelpTopic, "::/html/MSB_Wizard_Settings.htm");
        ::HtmlHelp(NULL, szHelpTopic, HH_DISPLAY_TOPIC, 0);
        break;
    case IDD_PP0: 
        strcat(szHelpTopic, "::/html/MSB_Wizard_Settings.htm");
        ::HtmlHelp(NULL, szHelpTopic, HH_DISPLAY_TOPIC, 0);
        break;
    case IDD_PP1: 
        strcat(szHelpTopic, "::/html/MSB_Wizard_Settings.htm");
        ::HtmlHelp(NULL, szHelpTopic, HH_DISPLAY_TOPIC, 0);
        break;
    case IDD_PP2: 
        strcat(szHelpTopic, "::/html/MSB_Build_Screen_Saver.htm");
        ::HtmlHelp(NULL, szHelpTopic, HH_DISPLAY_TOPIC, 0);
        break;
    default: 
        strcat(szHelpTopic, "::/html/MSB_Wizard_Settings.htm");
        ::HtmlHelp(NULL, szHelpTopic, HH_DISPLAY_TOPIC, 0);
        break;
    }
    return TRUE; // handled
}
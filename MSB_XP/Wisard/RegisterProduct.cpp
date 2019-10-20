#include ".\myss\registerproduct.h"
#include "stdafx.h"
#include "MySaver.h"
#include "RegisterProduct.h"

extern "C" decodeGenericRegCode(char* , char*);
extern "C" void freememory(void);
extern "C" char* getUserSeed(void);
extern "C" char* getConstant(void);
extern "C" char* getSequenceNumber(void);
extern "C" char* getDate(void);
extern "C" char* getDebugMessage(void);
extern "C" void setUserName(char*);
extern "C" void setUserEmail(char*);
extern "C" void createUserSeed(void);
extern "C" char *getCreatedUserSeed(void);

IMPLEMENT_DYNAMIC(CRegisterProduct, CDialog)
CRegisterProduct::CRegisterProduct(CWnd* pParent /*=NULL*/)
    : CDialog(CRegisterProduct::IDD, pParent)
{

}

CRegisterProduct::~CRegisterProduct()
{
}

void CRegisterProduct::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRegisterProduct, CDialog)
END_MESSAGE_MAP()

//BOOL CRegisterProduct::OnInitDialog()
//{
//	AfxMessageBox("1");
//	CDialog::OnInitDialog();
//    return TRUE;
//}

BOOL CRegisterProduct::IsValidRC(char* email, char* regCode)
{
    int value, result;
    
    //Fixed generic ACG configuration string (configuration manually and named smalevannyMySS)
    //char* config  = "SUPPLIERID:smalevanny%E:4%N:1%H:1%COMBO:ee%SDLGTH:20%CONSTLGTH:1%CONSTVAL:A%SEQL:2%ALTTEXT:Please contact support@synaptex.biz to obtain your registration code.%SCRMBL:D3,,D0,,U1,,U7,,U15,,U19,,U18,,U5,,U10,,U17,,U14,,U8,,U12,,U11,,U13,,C0,,U9,,U6,,U16,,U4,,S0,,U3,,S1,,U2,,D1,,D2,,U0,,%ASCDIG:2%MATH:1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,1A,,%BASE:26%BASEMAP:VNIKQJROEYUXMDCWLPTHSAFZGB%REGFRMT:MYSS-^#####-#####-#####-#####-#####-#####-#####-###[-#]";
    char* config = "SUPPLIERID:smalevanny%E:4%N:1%H:1%COMBO:ee%SDLGTH:20%CONSTLGTH:2%CONSTVAL:AC%SEQL:2%ALTTEXT:Contact support@myscreensaverbuilder.com to obtain your registration code.%SCRMBL:U12,,U0,,D3,,S0,,U3,,U2,,U10,,U15,,U1,,U5,,D2,,U9,,U4,,U6,,U11,,U19,,S1,,U18,,C0,,U8,,U13,,U16,,U14,,U7,,D1,,U17,,C1,,D0,,%ASCDIG:2%MATH:2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,2A,,%BASE:26%BASEMAP:XHDUAVJOFWEYRGMBZNTKQCILSP%REGFRMT:MSB2-^#####-#####-#####-#####-#####-#####-#####-#####[-#]";
    // Obtain seed from registration code provided.
    value = decodeGenericRegCode(config, regCode);
    
    // Create seed from user data (email).
    setUserEmail(email);
    createUserSeed();

    CString str; 
    str.Format("%s, %s", getUserSeed(), getCreatedUserSeed()); 
    //AfxMessageBox(str); // LAMBERT1791GMAILCOML and  LAMBERT1791GMAILCOML

    // These two seeds must be the same for successful activation.
    result = strcmp(getUserSeed(), getCreatedUserSeed());
    if (result == 0)
    {
        //AfxMessageBox("Successful activation");
        return TRUE;
    }
    else
    {
        //AfxMessageBox("Activation failed!");
        return FALSE;
    }
}

BOOL CRegisterProduct::IsRegistered()
{
    HKEY hKey;
    DWORD dwResult;
    DWORD dwType = REG_SZ; 
    TCHAR szRegCode[200]; 
    DWORD dwLength = 200;

    //--------------------------------------------------------------------------
    // Open MyScreenSaver key
    //--------------------------------------------------------------------------

    dwResult = RegOpenKey(theApp.m_hComponentBaseHive, theApp.m_strRegKeySCfg, 
                                                                         &hKey); 
                                                                         

    if (dwResult != ERROR_SUCCESS) return FALSE;

    dwResult = RegQueryValueEx(hKey, _T("RegKey"), NULL, &dwType,
                                                   (BYTE*)szRegCode, &dwLength);

    //--------------------------------------------------------------------------
    // If product is not registered, "RegKey" does not exist, return FALSE
    // therefore.
    //--------------------------------------------------------------------------

    if (dwResult != ERROR_SUCCESS ) return FALSE; RegCloseKey(hKey);

    //--------------------------------------------------------------------------
    // Return also FALSE immediately when RegKey is empty. Since 
    // sizeof(RegKey) = size of buffer, we use CString to figure out if szUser 
    // is empty.
    //--------------------------------------------------------------------------

    CString strTest = szRegCode; if (strTest.IsEmpty()) return FALSE;
    
    dwResult = RegOpenKey(theApp.m_hComponentBaseHive,
                                          theApp.m_strRegKeyApplication, &hKey);

    if (dwResult != ERROR_SUCCESS ) return FALSE;

    dwType = REG_SZ; 
    TCHAR szUser[100]; 
    dwLength = 100;

    dwResult = RegQueryValueEx(hKey, _T("User"), NULL, &dwType, (BYTE*)szUser, 
                                                                     &dwLength);

    //--------------------------------------------------------------------------
    // If product is not registered, "User" does not exist, return FALSE
    // therefore.
    //--------------------------------------------------------------------------

    if (dwResult != ERROR_SUCCESS ) return FALSE; RegCloseKey(hKey);

    //--------------------------------------------------------------------------
    // Return also FALSE immediately when User is empry. Since 
    // sizeof(szUser) = size of buffer, we use CString to figure out if szUser 
    // is empty.
    //--------------------------------------------------------------------------

    strTest = szUser; if (strTest.IsEmpty()) return FALSE;

    //--------------------------------------------------------------------------
    // If we are here, verify RegCode/User 
    //--------------------------------------------------------------------------
    
    if (IsValidRC(szUser, szRegCode))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//------------------------------------------------------------------------------
// Method: Date of creation the install directory "Program Files\MSB" is
//         compared with system date. Note that we are not using sss.scr date 
//         of creation intentionally because setup.exe ships this module with
//         build date, which is not changed on a user machine to copy (install)
//         date.
//------------------------------------------------------------------------------

BOOL CRegisterProduct::IsTrialUseExpired()
{
    // These times we'll compare.
    SYSTEMTIME stDirCreateSystemTime, stLocalSystemTime;

    // Get install directory (like I:\Program Files\MSB)
    TCHAR szInstDir[200];
    TCHAR szEnvVariable[100];

    ExpandEnvironmentStrings("%ProgramFiles%", szEnvVariable, 
                                                         sizeof(szEnvVariable));
    wsprintf(szInstDir, "%s%s", szEnvVariable, _T("\\MSB"));

    //--------------------------------------------------------------------------
    // Get file creation time. Notice that times used are in UTC: 100-nanosecond 
    // intervals since January 1, 1601.
    //--------------------------------------------------------------------------

    WIN32_FILE_ATTRIBUTE_DATA dad;

    // Reads not only file attributes, but also directory attributes!
    GetFileAttributesEx(szInstDir, GetFileExInfoStandard, &dad);

    // Convert file create into system time and obtain current system time. 
    FileTimeToSystemTime(&dad.ftCreationTime, &stDirCreateSystemTime);
    GetSystemTime(&stLocalSystemTime);

    //--------------------------------------------------------------------------
    // We could use RtlTimeToSecondsSince1970 to obtain number of seconds since
    // January 1, 1970, but this function is valid on Windows 2K/XP and might 
    // be not valid on subsequent versions. Therefore, simple rough manual 
    // count is used.
    //--------------------------------------------------------------------------
    
    int iUserDaysCount;
    int iDirCreateDaysCount = 365*stDirCreateSystemTime.wYear + 
                 30*stDirCreateSystemTime.wMonth + stDirCreateSystemTime.wDay;
    int iCurrentDaysCount    = 365*stLocalSystemTime.wYear + 
                           30*stLocalSystemTime.wMonth + stLocalSystemTime.wDay;

    iUserDaysCount = iCurrentDaysCount - iDirCreateDaysCount;

    if( iUserDaysCount > theApp.m_iTrialPeriod)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

    return FALSE;
}

// The body is almost identical to IsTrialUseExpired(). Should be optimized! 
int CRegisterProduct::GetDayOfUse()
{
    int iDayOfUse;

    // These times we'll compare
    SYSTEMTIME stDirCreateSystemTime, stLocalSystemTime;

    // Get install directory (like I:\Program Files\Synaptex\SSS)
    TCHAR szInstDir[200];
    TCHAR szEnvVariable[100];

    ExpandEnvironmentStrings("%ProgramFiles%", szEnvVariable, 
                                                         sizeof(szEnvVariable));
    wsprintf(szInstDir, "%s%s", szEnvVariable, _T("\\MSB"));

    //--------------------------------------------------------------------------
    // Get file creation time. Notice that times used are in UTC: 100-nanosecond 
    // intervals since January 1, 1601.
    //--------------------------------------------------------------------------

    WIN32_FILE_ATTRIBUTE_DATA dad;

    // Reads not only file attributes, but also directory attributes!
    GetFileAttributesEx(szInstDir, GetFileExInfoStandard, &dad);

    // Convert file create into system time and obtain current system time. 
    FileTimeToSystemTime(&dad.ftCreationTime, &stDirCreateSystemTime);
    GetSystemTime(&stLocalSystemTime);

    //--------------------------------------------------------------------------
    // We could use RtlTimeToSecondsSince1970 to obtain number of seconds since
    // January 1, 1970, but this function is valid on Windows 2K/XP and might 
    // be not valid on subsequent versions. Therefore, simple rough manual 
    // count is used.
    //--------------------------------------------------------------------------
    
    int iDirCreateDaysCount = 365*stDirCreateSystemTime.wYear + 
                 30*stDirCreateSystemTime.wMonth + stDirCreateSystemTime.wDay;
    int iCurrentDaysCount    = 365*stLocalSystemTime.wYear + 
                           30*stLocalSystemTime.wMonth + stLocalSystemTime.wDay;

    iDayOfUse = iCurrentDaysCount - iDirCreateDaysCount;

    return iDayOfUse;
}

BOOL CRegisterProduct::RegisterProduct(LPCTSTR pszEmail, LPCTSTR pszRegCode)
{
    HKEY hKey;
    DWORD dwResult;

    //--------------------------------------------------------------------------
    // Open MyScreenSaver key
    //--------------------------------------------------------------------------

    dwResult = RegOpenKey(theApp.m_hComponentBaseHive,
                                                 theApp.m_strRegKeySCfg, &hKey);

    if(dwResult != ERROR_SUCCESS ) 
           AfxMessageBox("CRegisterProduct::RegisterProduct error: RegOpenKey");

    
    // Use TCHAR with RegSetValueEx (CString produce gibberish symbols)
    TCHAR szRC[200];
    _tcscpy(szRC, pszRegCode);

    dwResult = RegSetValueEx(hKey, _T("RegKey"), 0, REG_SZ, (LPBYTE)&szRC, 200);		     

    if(dwResult != ERROR_SUCCESS ) 
        AfxMessageBox("CRegisterProduct::RegisterProduct error: RegSetValueEx");

    RegCloseKey(hKey); 

    //--------------------------------------------------------------------------
    // Open MyScreenSaver/Application key
    //--------------------------------------------------------------------------

    dwResult = RegOpenKey(theApp.m_hComponentBaseHive,
                                          theApp.m_strRegKeyApplication, &hKey);

    if(dwResult != ERROR_SUCCESS ) 
           AfxMessageBox("CRegisterProduct::RegisterProduct error: RegOpenKey");

    
    // Use TCHAR with RegSetValueEx (CString produce gibberish symbols)
    TCHAR szUser[100];
    _tcscpy(szUser, pszEmail); 
    
    dwResult = RegSetValueEx(hKey, _T("User"), 0, REG_SZ, (LPBYTE)&szUser, 100);

    if(dwResult != ERROR_SUCCESS ) 
        AfxMessageBox("CRegisterProduct::RegisterProduct error: RegSetValueEx");

    RegCloseKey(hKey); 
    
    return FALSE;
}

//------------------------------------------------------------------------------
// Description: returns TRUE if msbuse.log exists, and FALSE if not.
//------------------------------------------------------------------------------

BOOL CRegisterProduct::IsAntiHackingEnabled()
{
    CFile file;
    TCHAR szWinDir[100];

    ExpandEnvironmentStrings("%appdata%", szWinDir, sizeof(szWinDir)); 
    SetCurrentDirectory(szWinDir);

    if(file.Open(_T("msbuse.log"), CFile::modeRead))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void CRegisterProduct::EnableAntiHacking()
{
    TCHAR szWinDir[100];
    ExpandEnvironmentStrings("%appdata%", szWinDir, sizeof(szWinDir)); 
    SetCurrentDirectory(szWinDir);
    
    // Create msbuse.log if not exists, do nothing if exists.
    CFile file;

    // If msbuse.log exists do nothing.
    if(!file.Open(_T("msbuse.log"), CFile::modeRead))
    {
          file.Open(_T("msbuse.log"), CFile::modeCreate);
        file.Close(); // prevent sharing violation by the way

        // Set msbuse.log file attributes to hidden, read-only (no sense really).
        CFileStatus status;
        CFile::GetStatus(_T("msbuse.log"), status);
        status.m_attribute = CFile::Attribute::hidden | CFile::Attribute::readOnly;
        CFile::SetStatus(_T("msbuse.log"), status );
    }
}

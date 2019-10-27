#include "stdafx.h"
#include "MySaver.h"
#include "MySaverPropSheet.h"
#include "RegisterProduct.h"

IMPLEMENT_DYNAMIC(CMySaverPropSheet, CPropertySheet)

CMySaverPropSheet::CMySaverPropSheet(UINT nIDCaption, CWnd* pParentWnd, 
                                                               UINT iSelectPage)
    :CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CMySaverPropSheet::CMySaverPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, 
    UINT iSelectPage, HBITMAP hbmpWatermark, HPALETTE hpal, HBITMAP hbmpHeader)
    :CPropertySheet(pszCaption, pParentWnd, iSelectPage, hbmpWatermark, hpal, 
                                                                     hbmpHeader)
{
    CRegisterProduct m_RP;
    
    if (!theApp.m_bLaunchedFromCommandLine) // normal start (as in v2.0-2.1)
    {
        if(!m_RP.IsRegistered() || IsShowWelcomePage()) AddPage(&m_PageW);
        if(!m_RP.IsRegistered()) AddPage(&m_PageA); 
        AddPage(&m_Page0);
        AddPage(&m_Page1);
        AddPage(&m_Page2);
    }
    else // the wizard is launched from command line
    {
        // Start immediately from PP0 is registered. 
        if (m_RP.IsRegistered())
        {
            //if(IsShowWelcomePage()) AddPage(&m_PageW);
            AddPage(&m_Page0);
            AddPage(&m_Page1);
            AddPage(&m_Page2);
        }
        else // make follow all pages if not registered
        {
            AddPage(&m_PageW);
            AddPage(&m_PageA); 
            AddPage(&m_Page0);
            AddPage(&m_Page1);
            AddPage(&m_Page2);
        }
    }

    m_psh.dwFlags |= PSH_WIZARD97 | PSH_WATERMARK | PSH_HEADER | PSH_USEHICON;
    m_psh.pszbmWatermark =  MAKEINTRESOURCE(IDB_WIZARD_WATERMARK); 
    m_psh.pszbmHeader = MAKEINTRESOURCE(IDB_WIZARD_HEADER);
    m_psh.hInstance = AfxGetInstanceHandle(); //line is important
    m_psh.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); // use with PSH_USEHICON
}

CMySaverPropSheet::~CMySaverPropSheet(){}

BEGIN_MESSAGE_MAP(CMySaverPropSheet, CPropertySheet)
    ON_WM_HELPINFO()
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// Add Context menu button to all pages of the wizard (Q244232): "In order to 
// add the Context Help Button(?) to the top right corner of a CPropertySheet 
// so that it shows up on all the tabs of the sheet, you need to override the 
// OnInitDialog method of the CPropertySheet and call the ModifyStyleEx() 
// function with the first parameter of 0 and the second set to 
// WS_EX_CONTEXTHELP." (setting Context Help to TRUE in page properties isn't 
// necessary for a wizard).
BOOL CMySaverPropSheet::OnInitDialog()
{
    BOOL bResult = CPropertySheet::OnInitDialog();
    ModifyStyleEx(0, WS_EX_CONTEXTHELP); 
    
    return bResult;
}

BOOL CMySaverPropSheet::IsShowWelcomePage()
{
    //--------------------------------------------------------------------------
    // Parameters found in registry overrides default parameter values set in 
    //                              the constructor
    //--------------------------------------------------------------------------

    HKEY hKey;
    DWORD dwType, dwLength, dwResult;

    //--------------------------------------------------------------------------
    // Open application key
    //--------------------------------------------------------------------------

    dwResult = RegOpenKey(theApp.m_hComponentBaseHive, theApp.m_strRegKeyWizard, 
                                                                         &hKey);

    // Get m_bStartupLaunchEnabled
    dwType = REG_DWORD;
    dwLength = sizeof(DWORD);
    dwResult = RegQueryValueEx(hKey, 
                               "ShowWelcomePage", 
                               0, 
                               &dwType, 
                               (LPBYTE)&m_bShowWelcomePage, 
                               &dwLength );

    if(dwResult != ERROR_SUCCESS ) 
                        AfxMessageBox("RegQueryValueEx error: ShowWelcomePage"); 

    RegCloseKey(hKey); 

    if(m_bShowWelcomePage)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// This prevents calling .HLP file when the user makes use of "?"-context help 
// for Next, Cancel etc buttons.
BOOL CMySaverPropSheet::OnHelpInfo(HELPINFO* pHelpInfo)
{

    // There are no resource ID for Wizard's button in Resource.h (for example,
    // no resource ID for Help button). Found experimentally 
    // (pHelpInfo->iCtrlId) for Wizard's Help, Back, Next, and Cancel buttons 
    // (valid for any page):
    // Back -> 12323 (pHelpInfo->iCtrlId)
    // Next -> 12324 (pHelpInfo->iCtrlId)
    // Cancel -> 2 (pHelpInfo->iCtrlId)
    // Help -> 9 (pHelpInfo->iCtrlId)
    // Finish ->12325  (pHelpInfo->iCtrlId)

    // Since context help text for Help button is the same for all pages one can 
    // use redirection to any page that is always created so I use redirection 
    // to PP0's OnHelpInfo. The internal Help IDs (Back, Next, Cancel, Help) is 
    // passed with HELPINFO structure.
    m_Page0.OnHelpInfo(pHelpInfo); 
                                                                
    return TRUE;
}

// The function realizes "What's This?" context help. Message reflection to PP0 
// is used.
void CMySaverPropSheet::OnContextMenu(CWnd* pWnd, CPoint point)
{
    CString strButtonText;
    pWnd->GetWindowText(strButtonText);

    // Since context help text for Help button is the same for all pages one can 
    // use redirection to any page that is always created so I use redirection 
    // to PP0's OnHelpInfo.
    //if (strButtonText == "Help") m_Page0.OnContextMenu(pWnd, point);
    m_Page0.OnContextMenu(pWnd, point);
}
#include "stdafx.h"
#include "MySaver.h"
#include "MySaverPropPageActivate.h"
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

IMPLEMENT_DYNAMIC(CMySaverPropPageActivate, CPropertyPage)
CMySaverPropPageActivate::CMySaverPropPageActivate()
    : CPropertyPage(CMySaverPropPageActivate::IDD)
    , m_strEmail(_T(""))
    , m_strRegistrationCode(_T(""))
{
    m_psp.dwFlags |= PSP_USETITLE | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE | PSP_HASHELP;; 
    m_psp.pszHeaderTitle = _T("MyScreenSaver Builder Activation");
    m_psp.pszHeaderSubTitle = _T("Enter the activation code you received when purchasing the product.");
}

CMySaverPropPageActivate::~CMySaverPropPageActivate()
{
}

void CMySaverPropPageActivate::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_AP_EMAIL, m_strEmail);
    DDX_Text(pDX, IDC_AP_ACTIVATION_CODE, m_strRegistrationCode);
    DDX_Control(pDX, IDC_APP_STATIC, m_ctlStatusText);
}

BEGIN_MESSAGE_MAP(CMySaverPropPageActivate, CDialog)
    ON_WM_HELPINFO()
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

BOOL CMySaverPropPageActivate::OnInitDialog() 
{
    CPropertyPage::OnInitDialog();
    //------------------------------------------------------------------

    CMySaverApp* pApp =  static_cast<CMySaverApp*>(AfxGetApp());
    pApp->m_iPrevPage = IDD_ACTIVATE;

    // v1.1:
    //m_strEmail = _T("jimlee@hotmail.com"); 
    //m_strRegistrationCode = _T("MYSS-KGKECO-PNNBO-MMDRY-NGKZH-WZNAU-IRZXT-WFSEO-OCZ"); 

    UpdateData(FALSE);
    
    return TRUE;  
}

BOOL CMySaverPropPageActivate::OnSetActive()
{
    // In opposite to OnInitDialog, OnSetActive() called everytime PP shows up.
    CMySaverApp* pApp =  static_cast<CMySaverApp*>(AfxGetApp());
    pApp->m_iPrevPage = IDD_ACTIVATE;

    CPropertySheet* psheet = (CPropertySheet*) GetParent();   
    psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

    return CPropertyPage::OnSetActive();
}

LRESULT CMySaverPropPageActivate::OnWizardNext()
{
    UpdateData(TRUE);

    char szEmail[100];
    sprintf(szEmail, "%s", m_strEmail);

    char szRC[100];
    sprintf(szRC, "%s", m_strRegistrationCode);

    CRegisterProduct m_RP;

    if(m_RP.IsValidRC(szEmail, szRC))
    {
        m_RP.RegisterProduct(szEmail, szRC);
        // Update m_strUser application variable in memory.
        theApp.m_strUser = szEmail;
        m_ctlStatusText.SetWindowText("The product activated successfully!");
        Sleep(1000);
        return IDD_PP0;
    }
    else
    {
        m_ctlStatusText.SetWindowText("Wrong activation code :-(");
        return -1; // stay on the same property page
    }
}

// Copied from PP0 with updates (PP0->Welcome Page).
// OnHelpInfo (WM_HELP handler) handles context help: 1) "?" context help, 2) 
// SHIFT+F1, 3) F1 (2 and 3 are the same). 
BOOL CMySaverPropPageActivate::OnHelpInfo(HELPINFO* pHelpInfo)
{
    BOOL bF1;
    BOOL bShift;
    UINT nHelpOriginator;
    CRect rcControl;
    CWnd *pWnd = NULL; 
    HH_POPUP ContextHelpPopup;

    // Since OnHelpInfo handles all help (except direct clicks on Help button 
    // that handled, in property sheets, with OnNotify) figure at first what
    // caused WM_HELP event (get nHelpOriginator value).

    // MSDN: "If the high-order bit is 1, the key is down; otherwise, it is 
    // up". To check the state I use programmatic idiom (MSDN, Newcomer's book) 
    // meaning that if GetKeyState(VK_F1) < 0 the F1 is button is pressed, and 
    // if "0" is not.
    bF1 = (GetKeyState(VK_F1) < 0); bShift = (GetKeyState(VK_SHIFT) < 0); 

    if (bF1 && !bShift)
    {
        nHelpOriginator = 1; // F1
        // When F1 is pressed, handle in OnF1Help and return from OnHelpInfo 
        // immediately since handled.
        if (theApp.OnF1Help(IDD_PP0, pHelpInfo)) return TRUE;
    }
    else if (bF1 && bShift)
        nHelpOriginator = 2; // SHIFT + F1
    else
        nHelpOriginator = 0; // either "?" or "What's This?" context help.
    
    // Get context menu map's index for the control that was clicked or has 
    // focus (based on iCtrlId value), then get corresponding contex help string, 
    // make up HH_POPUP structure and invoke context help for the control with
    // HtmlHelp.
    for (int i = 0; i < theApp.m_ContextHelp.m_nMapSizePPA; i++) 
    //for (int i = 0; i < 1; i++) 
    {
        if (pHelpInfo->iCtrlId ==  theApp.m_ContextHelp.m_MapPPA[i].nControlID) 
        { 
            memset(&ContextHelpPopup, 0, sizeof(HH_POPUP) );   
            ContextHelpPopup.cbStruct = sizeof(HH_POPUP);

            ContextHelpPopup.hinst = AfxGetInstanceHandle(); // application 
                                                             // instance
            ContextHelpPopup.idString = 
                      theApp.m_ContextHelp.m_MapPPA[i].nStringHelpID; // resource ID 

            // When SHIFT + F1 context help is invoked and since HELPINFO gives 
            // the coordinates of cursor, but not rectangle, calculate the 
            // rectangle of the control that has focus to properly position 
            // context help window e.g. avoid its positioning outside dialog.
            if (nHelpOriginator == 2 /* SHIFT + F1 */)
            {
                pWnd = GetFocus();
                pWnd->GetWindowRect(rcControl);
                ContextHelpPopup.pt.x = rcControl.right; 
                ContextHelpPopup.pt.y = rcControl.bottom + 15; 
            }
            else 
            {
                ContextHelpPopup.pt.x = pHelpInfo->MousePos.x; 
                ContextHelpPopup.pt.y = pHelpInfo->MousePos.y + 15; 
            }
            ContextHelpPopup.pszText = NULL; // recommended (KB241218), 
                                             // but LL no effect 
            ContextHelpPopup.rcMargins.top    =  5;
            ContextHelpPopup.rcMargins.bottom =  5;
            ContextHelpPopup.rcMargins.left   =  5;
            ContextHelpPopup.rcMargins.right  =  5;
            ContextHelpPopup.clrForeground = -1; // use system color 
            ContextHelpPopup.clrBackground = -1; // use system color
            //ContextHelpPopup.pszFont = "MS Sans Serif, 10, , BOLD";

            // I call HtmlHelp 2 times to avoid color distortion when "?" called 
            // for the 1st time (one call is redundant, but removes the 
            // problem). TODO: find out normal way without 2-times call (for 
            // some reason color distortion disappears when "?" help invoked for 
            // the 2nd time).
            
            // For the 1st argument, this->m_hWnd is used instead of 
            // GetDlgItem(pHelpInfo->iCtrlId)->m_hWnd since it does not causes 
            // crash if context help is implemented for the Wizard's Help button 
            // (redirected from property sheet class to here).
            ::HtmlHelp(this->m_hWnd, NULL, 
                               HH_DISPLAY_TEXT_POPUP, (DWORD)&ContextHelpPopup);
            ::HtmlHelp(this->m_hWnd, NULL, 
                               HH_DISPLAY_TEXT_POPUP, (DWORD)&ContextHelpPopup);
            break;
        }
    }

    return TRUE;
}

// Copied from PP0.
// The function realizes "What's This?" context help.
void CMySaverPropPageActivate::OnContextMenu(CWnd* pWnd, CPoint point)
{
    CMenu menu;
    CPoint ptT(point);

    ScreenToClient(&point);

    if (pWnd == this) pWnd = ChildWindowFromPoint(point, CWP_SKIPINVISIBLE);
    if (!pWnd || pWnd == this) return;

    menu.CreatePopupMenu();
    menu.AppendMenu(MF_STRING, 0x0100, _T("&What's This?"));

    if (!menu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                                                            ptT.x, ptT.y, this))
        return; 

    ClientToScreen(&point);
    
    // Re-direct handling to OnHelpInfo.
    HELPINFO hi;
    hi.cbSize = sizeof(HELPINFO);
    hi.dwContextId = 0; // some
    hi.hItemHandle = 0; // some
    hi.iContextType = HELPINFO_WINDOW;
    hi.iCtrlId = pWnd->GetDlgCtrlID(); 
    hi.MousePos = point;
    
    OnHelpInfo(&hi);
}

// Handles Help button.
BOOL CMySaverPropPageActivate::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    PSHNOTIFY* pPSHNOTIFY = (PSHNOTIFY*)lParam;

    TCHAR szHelpTopic[MAX_PATH];

    strcpy(szHelpTopic, theApp.m_pszHelpFilePath);
    //strcat(szHelpTopic, "::/html/MSB_Wizard_Settings.htm");

    switch (pPSHNOTIFY->hdr.code)
    {
    case PSN_HELP:
        ::HtmlHelp(NULL, szHelpTopic, HH_DISPLAY_TOPIC, 0);
        return -1; // message handled, no futher actions
    }

    // Contitue handling notification messages for other then PSN_HELP.
    return CPropertyPage::OnNotify(wParam, lParam, pResult); 
}

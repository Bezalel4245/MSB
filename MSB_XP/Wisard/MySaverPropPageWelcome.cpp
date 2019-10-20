#include "stdafx.h"
#include "MySaver.h"
#include "MySaverPropPageWelcome.h"
#include "RegisterProduct.h"
#include "krm.h"

IMPLEMENT_DYNAMIC(CMySaverPropPageWelcome, CPropertyPage)
CMySaverPropPageWelcome::CMySaverPropPageWelcome()
    : CPropertyPage(CMySaverPropPageWelcome::IDD)
    , m_bShowWelcomePage(FALSE)
{
    m_psp.dwFlags |= PSP_HIDEHEADER | PSP_HASHELP; 
    m_bShowWelcomePage = TRUE; 
    m_bTrialExpired = FALSE;

    CMySaverApp* pMySS =  static_cast<CMySaverApp*>(AfxGetApp());
    m_iTrialPeriod = pMySS->m_iTrialPeriod;
}

CMySaverPropPageWelcome::~CMySaverPropPageWelcome()
{
}

void CMySaverPropPageWelcome::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_WP_STATIC_HEADER, m_ctlStaticHeader);
    DDX_Control(pDX, IDC_WP_STATIC_HEADER2, m_ctlStaticHeader2);
    DDX_Control(pDX, IDC_WP_STATIC_HEADER3, m_ctlStaticHeader3);
    DDX_Control(pDX, IDC_WP_STATIC_GROUPBOX, m_ctlStaticGroupBox);
    DDX_Control(pDX, IDC_WP_RADIO, m_ctlRadio1);
    DDX_Control(pDX, IDC_WP_RADIO2, m_ctlRadio2);
    DDX_Control(pDX, IDC_WP_RADIO3, m_ctlRadio3);
    DDX_Control(pDX, IDC_WP_PROGRESS, m_ctlProgressBar);
    DDX_Control(pDX, IDC_WP_STATIC2, m_ctlStatic2);
    DDX_Check(pDX, IDC_WP_CHECKBOX, m_bShowWelcomePage);
    DDX_Control(pDX, IDC_WP_CHECKBOX, m_ctlCheckBox);
}

BEGIN_MESSAGE_MAP(CMySaverPropPageWelcome, CPropertyPage)
    ON_BN_CLICKED(IDC_WP_RADIO, OnRadioClicked)
    ON_BN_CLICKED(IDC_WP_RADIO2, OnRadio2Clicked)
    ON_WM_HELPINFO()
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

BOOL CMySaverPropPageWelcome::OnInitDialog() 
{
    CPropertyPage::OnInitDialog();
    //--------------------------------------------------------------------------

    if (theApp.m_bEnableDebug)
    {
        theApp.m_dwWelcomePageInitTime = GetTickCount();
        theApp.App_Debug_FunctionCallTrace(
                                     "CMySaverPropPageWelcome::OnInitDialog()");
    }

    CFont font;
    CFont* pStaticFont = NULL;
    CStatic* pctlStatic = NULL; 
    CStatic* pctlStatic2 = NULL; 
    CStatic* pctlStatic3 = NULL; 
    CStatic* pctlStatic4 = NULL;
    CString strHeader, strHeader2, strHeader3;
    CString strStatic, strStatic2, strStatic3, strStatic4;
    CString strStaticExpired;
    CRegisterProduct m_RP;
    RECT rcStatic = {205, 140, 500, 160};
    RECT rcStatic2 = {205, 160, 500, 180};
    RECT rcStatic3 = {205, 180, 500, 200};
    RECT rcStatic4 = {205, 200, 500, 220};

    strHeader  = "This wizard guides you through the steps to package";
    strHeader2 = "images to MSS file and create your own screen saver.";
    strHeader3 = "Please take a few steps to register.";
    strStatic = "� Edit MSS files by modifying content (text, images).";
    strStatic2 = "� Edit car specifications in MSS files of cars category.";
    strStatic3 = "� Create setup program from the MSS file.";
    strStaticExpired = "Trial use expired. Please register or uninstall!";

    // Make title in bold.
    //font.CreatePointFont(12, "Tahoma"); // LL does not matter "Arial" or "Tahoma"
    font.CreatePointFont(12, "Arial");
    GetDlgItem(IDC_WP_STATIC_TITLE)->SetFont(&font);

    // Setup dynamically added static controls with texts.
    pStaticFont = GetDlgItem(IDC_WP_STATIC_HEADER)->GetFont();

    if (m_RP.IsRegistered())
    {
        // Dynamic creation with "new" is used instead of CStatic m_ctl<...> 
        // option since in the latter case static control does not show up 
        // (immediately destroyed upon creation).
        pctlStatic  = new CStatic; 
        pctlStatic2 = new CStatic; 
        pctlStatic3 = new CStatic; 
        pctlStatic4 = new CStatic;   

        // Bullet is copied from Character Map. 
        pctlStatic->Create(strStatic, WS_CHILD | WS_VISIBLE | SS_LEFT, rcStatic, 
                                                                          this); 
        pctlStatic->SetFont(pStaticFont);
        pctlStatic2->Create(strStatic2, WS_CHILD | WS_VISIBLE | SS_LEFT, 
                                                               rcStatic2, this);
        pctlStatic2->SetFont(pStaticFont);
        pctlStatic3->Create(strStatic3, WS_CHILD | WS_VISIBLE | SS_LEFT, 
                                                               rcStatic3, this);
        pctlStatic3->SetFont(pStaticFont);
        
        
        m_ctlStaticGroupBox.ShowWindow(FALSE);
        m_ctlRadio1.ShowWindow(FALSE);
        m_ctlRadio2.ShowWindow(FALSE);
        m_ctlRadio3.ShowWindow(FALSE);
        m_ctlProgressBar.ShowWindow(FALSE);
        m_ctlStatic2.ShowWindow(FALSE);
        m_ctlCheckBox.ShowWindow(TRUE);
    }
    else
    {
        m_ctlStaticHeader.SetWindowText(strHeader);
        m_ctlStaticHeader2.SetWindowText(strHeader2);
        m_ctlStaticHeader3.SetWindowText(strHeader3);

        m_ctlStaticGroupBox.ShowWindow(TRUE);
        m_ctlRadio1.ShowWindow(TRUE);
        m_ctlRadio2.ShowWindow(TRUE);
        m_ctlRadio3.ShowWindow(TRUE);
        m_ctlProgressBar.ShowWindow(TRUE);
        CheckRadioButton(IDC_WP_RADIO, IDC_WP_RADIO3, IDC_WP_RADIO); 
        m_ctlCheckBox.ShowWindow(FALSE);

        if (m_RP.IsTrialUseExpired())
        {
            m_bTrialExpired = TRUE;
            m_ctlRadio3.EnableWindow(FALSE);
            m_ctlProgressBar.SetRange(0, m_iTrialPeriod);
            m_ctlProgressBar.SetPos(m_iTrialPeriod);
            //m_ctlProgressBar.EnableWindow(FALSE); //does not work
            m_ctlStatic2.SetWindowText(strStaticExpired);

            //------------------------------------------------------------------
            // Create an empty file myssuse.log to indicate that use was 
            // expired once. Uninstall does not delete myssuse.log.
            //------------------------------------------------------------------

            m_RP.EnableAntiHacking();
        }
        else
        {
            if (m_RP.IsAntiHackingEnabled()) // checks if myssuse.log exists, if yes set expired again
            {
                m_bTrialExpired = TRUE;
                m_ctlRadio3.EnableWindow(FALSE);
                m_ctlProgressBar.SetRange(0, m_iTrialPeriod);
                m_ctlProgressBar.SetPos(m_iTrialPeriod);
                m_ctlStatic2.SetWindowText(strStaticExpired);
            }
            else
            {
                m_ctlProgressBar.SetRange(0, m_iTrialPeriod);
                m_ctlProgressBar.SetPos(m_RP.GetDayOfUse());

                CString strText;
                strText.Format(_T("%d days remaining out of %d"), 
                           m_iTrialPeriod - m_RP.GetDayOfUse(), m_iTrialPeriod);
                m_ctlStatic2.SetWindowText(strText);
            }
        }
    }

    UpdateData(FALSE);
    
    return TRUE;  
}

BOOL CMySaverPropPageWelcome::OnSetActive()
{
    if (theApp.m_bEnableDebug)
    {
        theApp.App_Debug_FunctionCallTrace(
                                      "CMySaverPropPageWelcome::OnSetActive()");
    }

    // In opposite to OnInitDialog, OnSetActive() called everytime PP shows up.
    CMySaverApp* pApp =  static_cast<CMySaverApp*>(AfxGetApp());
    pApp->m_iPrevPage = IDD_WELCOME;

    CPropertySheet* psheet = (CPropertySheet*) GetParent();
    psheet->SetWizardButtons(PSWIZB_NEXT);

    return CPropertyPage::OnSetActive();
}

// Note: must be syncronized with MSB_SCfg.scr's CRegisterProduct code.
int CMySaverPropPageWelcome::StartKRM()
{
    TCHAR szEnvVariable[MAX_PATH];
    TCHAR szFileLocation[MAX_PATH];
    HMODULE hkrm;
    int nStatusCode;
    WCHAR szTmpW[255];
    char szUserName[255];
    char szLicenseKey[255];
    CString strDebugMsg;
    CString strDebugTitle; 

    // Get KRM module location.
    ExpandEnvironmentStrings("%ProgramFiles%", szEnvVariable, 
                                                         sizeof(szEnvVariable));
    wsprintf(szFileLocation, "%s%s", szEnvVariable, _T("\\MSB\\krm.dll"));

    __krmGetVersion fkrmGetVersion;
    __krmExec fkrmExec;
    __krmAddPrice fkrmAddPrice;
    __krmGetResultInt fkrmGetResultInt;
    __krmGetResultStr fkrmGetResultStr;
    __krmSetCallBack fkrmSetCallBack;
    __krmGetLastError fkrmGetLastError;
    __krmSetDataInt fkrmSetDataInt;
    __krmSetDataStr fkrmSetDataStr;
 
    hkrm = LoadLibrary(szFileLocation);
    if (hkrm == 0) 
    {
        AfxMessageBox("Failed to load the krmWin DLL!");
        return 0;
    }
    
    fkrmGetVersion = (__krmGetVersion)GetProcAddress(hkrm, "krmGetVersion");
    KRMVersionInfo krmVersion;
    krmVersion.dwVersionInfoSize = sizeof(krmVersion);
    fkrmGetVersion(&krmVersion);

    if ((krmVersion.dwMajorVersion != KRM_VERSIONMAJOR) || 
                                (krmVersion.dwMinorVersion != KRM_VERSIONMINOR)) 
    {
        AfxMessageBox("Version of krmWin DLL does not match!");
        FreeLibrary(hkrm); return 0;
    }
    else 
    {
        // Import other needed functions and initialize the DLL.
        fkrmGetLastError = (__krmGetLastError)GetProcAddress(hkrm, 
                                                             "krmGetLastError");
        fkrmExec = (__krmExec)GetProcAddress(hkrm, "krmExec");
        fkrmAddPrice = (__krmAddPrice)GetProcAddress(hkrm, "krmAddPrice");
        fkrmSetDataInt = (__krmSetDataInt)GetProcAddress(hkrm, "krmSetDataInt");
        fkrmSetDataStr = (__krmSetDataStr)GetProcAddress(hkrm, "krmSetDataStr");
        fkrmSetCallBack = (__krmSetCallBack)GetProcAddress(hkrm, 
                                                              "krmSetCallback");
        fkrmGetResultInt = (__krmGetResultInt)GetProcAddress(hkrm, 
                                                             "krmGetResultInt");
        fkrmGetResultStr = (__krmGetResultStr)GetProcAddress(hkrm, 
                                                             "krmGetResultStr");

        // Install callback function for events (optional).
        //fkrmSetCallBack(&krmCallback, 0);

        // There are 11 items that must be set with KRM v1.13: 
        // 1)  krmSetSupplierName
        // 2)  krmSetSupplierID
        // 3)  krmSetSupplierEmail
        // 4)  krmSetSupplierURL
        // 5)  krmSetProductDbName
        // 6)  krmSetProductName
        // 7)  krmSetProductVersion
        // 8)  krmSetConnectFailURL
        // 9)  krmSetTransactFailURL
        // 10) krmAddPrice
        // 11) krmSetACGVar <-- still is not set here

        // Set the supplier data.
        fkrmSetDataStr(krmSetSupplierName, L"MSB LLC");
        fkrmSetDataStr(krmSetSupplierID, L"6FBHN");
        fkrmSetDataStr(krmSetSupplierEMail, L"lambert1791@gmail.com");
        fkrmSetDataStr(krmSetSupplierURL, 
                                        L"http://www.myscreensaverbuilder.com");
        // Set the product information.
        fkrmSetDataStr(krmSetProductDbName, L"MyScreenSaver_6FBHN");
        fkrmSetDataStr(krmSetProductName, 
                                         L"en=MyScreenSaver Builder (English)");
        fkrmSetDataStr(krmSetProductVersion, L"2.2");
        // Although krmWin.pdf does not list krmSetProductSupplierSKU as "must be 
        // set", its omission results in  wizard activation failure with 
        // krmErrorNoSupplierSKU (valuse is -14).
        fkrmSetDataStr(krmSetProductSupplierSKU, L"n/a");

        // Set the failure URLs.
        fkrmSetDataStr(krmSetConnectFailURL, 
            L"http://www.myscreensaverbuilder.com/Downloads/MSB/connectionFailure.htm");
        fkrmSetDataStr(krmSetTransactFailURL, 
            L"http://www.myscreensaverbuilder.com/Downloads/MSB/transactionFailure.htm");

        // Set the price in USD, CAD, EUR, AUD to the equivalent of 59.95 USD 
        // at the time of the order. The prices calculated automatically on server
        // based on the USD value, and the currencies are sorted.
        if (!theApp.m_bEnableDebug) 
        {
            fkrmAddPrice(L"USD", 59.95, NULL);
            fkrmAddPrice(L"CAD", 59.95, /* convert to USD */ L"USD"); 
            fkrmAddPrice(L"EUR", 59.95, /* convert to USD */ L"USD"); 
            fkrmAddPrice(L"AUD", 59.95, /* convert to USD */ L"USD"); 
        }
        else
        {
            fkrmAddPrice(L"USD", 2.00, NULL);
            fkrmAddPrice(L"CAD", 2.00, /* convert to USD */ L"USD"); 
            fkrmAddPrice(L"EUR", 2.00, /* convert to USD */ L"USD"); 
            fkrmAddPrice(L"AUD", 2.00, /* convert to USD */ L"USD"); 
        }
        
        // Set TFYP mail (optional).
        //fkrmSetDataStr(krmSetOrderPassThroughData, 
        //	L"Because Armadillos walk on the tips of their feet, they tend to leave three-toed tracks that resemble bird footprints"); 

        // Execute the krmWin Wizard and wait for the result.
        if (!fkrmExec()) 
        {
            strDebugMsg.Format("krmExec failed, error code: %d", 
                                                            fkrmGetLastError());			
            // AfxMessageBox(strDebugMsg); 
            if (theApp.m_bEnableDebug) 
                                theApp.App_Debug_FunctionCallTrace(strDebugMsg);
            FreeLibrary(hkrm); return 0; 
        }
        else 
        {
            // Retrieve the status code of transaction.
            fkrmGetResultInt(krmGetStatusCode, &nStatusCode);

            // Test: retrieve name of user and the license key.
            fkrmGetResultStr(krmGetUserName, (LPWSTR)&szTmpW, sizeof(szTmpW));
            WideCharToMultiByte(CP_ACP, 0, szTmpW, -1, szUserName, 
                                                sizeof(szUserName), NULL, NULL);

            fkrmGetResultStr(krmGetLicenseKey, (LPWSTR)&szTmpW, sizeof(szTmpW));
            WideCharToMultiByte(CP_ACP, 0, szTmpW, -1, szLicenseKey, 
                                              sizeof(szLicenseKey), NULL, NULL);

            // Log debug information.
            strDebugTitle = "KRM test transaction results: ";
            strDebugMsg.Format(strDebugTitle + 
                "%d (status code), %s (status text) %s (user) %s (license)", 
                nStatusCode, GetTransactionText(nStatusCode),
                                                      szUserName, szLicenseKey);
            if (theApp.m_bEnableDebug) 
                                theApp.App_Debug_FunctionCallTrace(strDebugMsg);

            //--------------------------------------------------------------
            // Successful registration is not enough for activation! It is
            // because knowing supplier ID and product ID one can obtain
            // registration code with KRM. Thus, RC plus user information
            // (like email) should be used to extract SEED according
            // to secrect (hard-coded) configuration.
            //---------------------------------------------------------------
            
            // TODO: I take a risky (weaker) approach assuming that if 
            // the activation data are correct the card is billed (even though
            // status code may be e.g. zero meaning that the order was not
            // successful). This is justified by real test with own Visa card
            // (the onle difference with real purchase was $2 test price) when 
            // the AC was received, card is billed, yet the status code was 
            // zero ("Order Invalid").
            
            //if (nStatusCode == 5000 /* krmOrderSuccess */)
            {
                   CRegisterProduct m_RP;
                if (m_RP.IsValidRC(szUserName, szLicenseKey))
                {
                    m_RP.RegisterProduct(szUserName, szLicenseKey);
                    ::MessageBox(NULL, 
                        "Your product was successfully registered!", 
                                            "MyScreenSaver Builder 2.2", MB_OK);
                }
                else
                {
                    AfxMessageBox("Wrong registration code!");
                }

                FreeLibrary(hkrm); return 1; 
            }
            //else
            //{
            //	::MessageBox(NULL, GetTransactionText(nStatusCode), 
            //		           "The order was not sussessful!", MB_ICONWARNING);
            //	FreeLibrary(hkrm); return 0; 
            //}
        }
    }

    // Never really here, puristic deviations.
    if (hkrm) FreeLibrary(hkrm); return 0; 
}

// Note: copied from krmTestC.cpp (KRM_v1.1.3\Samples\C)
char* CMySaverPropPageWelcome::GetTransactionText(int nStatusCode)
{
    switch (nStatusCode) 
    {
        case krmOrderInvalid:
            return("Order invalid");
        case krmOrderDuplicate:
            return("Duplicate order");
        case krmOrderFailureNoCharge:
            return("Failure, no charge");
        case krmOrderFailureCharged:
            return("Failure with charge");
        case krmOrderFailureUnknownCharge:
            return("Failure, charge status unknown");
        case krmOrderOrderApprovalNeeded:
            return("Order approval needed");
        case krmOrderSuccess:
            return("Order successful");
        case krmOrderNetworkError:
            return("Error while contacting the Kagi Store");
        case krmOrderResponseError:
            return("Error while processing the response from the Kagi Store");
        default:
            return("Unknown statuscode!");
    }
}

// Likely redundant
void CMySaverPropPageWelcome::OnRadioClicked()
{
    if(m_bTrialExpired)
    {
        CPropertySheet* psheet = (CPropertySheet*) GetParent();   
        psheet->SetWizardButtons(PSWIZB_NEXT); //0
    }
}

// Likely redundant
void CMySaverPropPageWelcome::OnRadio2Clicked()
{
    if(m_bTrialExpired)
    {
        CPropertySheet* psheet = (CPropertySheet*) GetParent();   
        psheet->SetWizardButtons(PSWIZB_NEXT);
    }
}

LRESULT CMySaverPropPageWelcome::OnWizardNext()
{
    UpdateData(TRUE);

    CMySaverApp* pMySS =  static_cast<CMySaverApp*>(AfxGetApp());
    pMySS->SaveWelcomePageData(m_bShowWelcomePage);

    CRegisterProduct m_RP;
    
    if(m_RP.IsRegistered())
    {
        return IDD_PP0;
    }
    else
    {
        switch(GetCheckedRadioButton(IDC_WP_RADIO, IDC_WP_RADIO3))
        {
        case IDC_WP_RADIO:
            // Call KRM & activate instantly
            if(StartKRM() == 1)
            {
                return IDD_PP0;
            }
            else
            {
                return -1;
            }
            break;
        case IDC_WP_RADIO2:
            // Go to activatation page
            return IDD_ACTIVATE; 
            break;
        case IDC_WP_RADIO3:
            // Do nothing
            return IDD_PP0;
            break;
        }
    }

    //Return actually handled by switch above.
    return CPropertyPage::OnWizardNext();
}

// Copied from PP0 with updates (PP0->Welcome Page).
// OnHelpInfo (WM_HELP handler) handles context help: 1) "?" context help, 2) 
// SHIFT+F1, 3) F1 (by default, 2 and 3 are the same). 
BOOL CMySaverPropPageWelcome::OnHelpInfo(HELPINFO* pHelpInfo)
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
    for (int i = 0; i < theApp.m_ContextHelp.m_nMapSizePPW; i++) 
    //for (int i = 0; i < 1; i++) 
    {
        if (pHelpInfo->iCtrlId ==  theApp.m_ContextHelp.m_MapPPW[i].nControlID) 
        { 
            memset(&ContextHelpPopup, 0, sizeof(HH_POPUP) );   
            ContextHelpPopup.cbStruct = sizeof(HH_POPUP);

            ContextHelpPopup.hinst = AfxGetInstanceHandle(); // application 
                                                             // instance
            ContextHelpPopup.idString = 
                      theApp.m_ContextHelp.m_MapPPW[i].nStringHelpID; // resource ID 

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
            //::HtmlHelp(this->m_hWnd, NULL, 
            //	               HH_DISPLAY_TEXT_POPUP, (DWORD)&ContextHelpPopup);
            break;
        }
    }

    return TRUE;
}

// Copied from PP0.
// The function realizes "What's This?" context help.
void CMySaverPropPageWelcome::OnContextMenu(CWnd* pWnd, CPoint point)
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
BOOL CMySaverPropPageWelcome::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

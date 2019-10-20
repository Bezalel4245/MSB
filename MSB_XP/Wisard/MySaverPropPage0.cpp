#include "stdafx.h"
#include "MySaver.h"
#include "MySaverPropPage0.h"
#include ".\mysaverproppage0.h"

IMPLEMENT_DYNAMIC(CMySaverPropPage0, CPropertyPage)
CMySaverPropPage0::CMySaverPropPage0()
    : CPropertyPage(CMySaverPropPage0::IDD)
    , m_strInitImages(_T(""))
    , m_bShowToolTips(FALSE)
{
    m_psp.dwFlags |= PSP_DEFAULT | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE | PSP_HASHELP;

    m_strHeaderTitle_New = "Select the Source of Images ";
    m_strHeaderTitle_Edit = "Select MSS File";
    m_strHeaderSubTitle_New = "From which images (JPG or GIF) would you like to create your own screen saver?";
    m_strHeaderSubTitle_Edit = "Which MSS file would you like to edit?";
}

CMySaverPropPage0::~CMySaverPropPage0()
{
}

void CMySaverPropPage0::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);

    // Control value variables.
    DDX_CBIndex(pDX, IDC_PP0_COMBOBOX, m_nBuildOption);
    DDX_CBIndex(pDX, IDC_PP0_COMBOBOX2, m_nPicturesCategory);
    DDX_Text(pDX, IDC_PP0_EDIT, m_strInitImages);
    DDX_Radio(pDX, IDC_PP0_RADIO1, m_nEditOption); // int required for 
                                                   // m_nEditOption, not short
    DDX_Check(pDX, IDC_PP0_CB_SHOWTOOLTIPS, m_bShowToolTips);

    // Control's control variables.
    DDX_Control(pDX, IDC_PP0_COMBOBOX, m_ctlComboBox);
    DDX_Control(pDX, IDC_PP0_COMBOBOX2, m_ctlComboBox_PicturesCategory);
    DDX_Control(pDX, IDC_PP0_STATIC_GROUPBOX, m_ctlGroupBox);
    DDX_Control(pDX, IDC_PP0_RADIO1, m_ctlRadio1);
    DDX_Control(pDX, IDC_PP0_RADIO2, m_ctlRadio2);
    DDX_Control(pDX, IDC_PP0_RADIO3, m_ctlRadio3);
}

BEGIN_MESSAGE_MAP(CMySaverPropPage0, CPropertyPage)
    ON_BN_CLICKED(IDC_PP0_BUTTON1, OnBrowseClicked)
    ON_BN_CLICKED(IDC_PP0_CB_SHOWTOOLTIPS, OnCheckBoxClicked)
    ON_CBN_SELCHANGE(IDC_PP0_COMBOBOX, OnComboboxSelectionChange)
    ON_CBN_SELCHANGE(IDC_PP0_COMBOBOX2, OnComboboxPicCategorySelectionChange)
    ON_WM_HELPINFO()
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

BOOL CMySaverPropPage0::OnInitDialog() 
{
    CPropertyPage::OnInitDialog();
    //--------------------------------------------------------------------------

    if (theApp.m_bEnableDebug)
    {
        CString strDebugMsg; 
        theApp.m_dwPP0InitTime = GetTickCount();
        strDebugMsg.Format("CMySaverPropPage0::OnInitDialog(): %d", 
                       theApp.m_dwPP0InitTime - theApp.m_dwWelcomePageInitTime); 
        theApp.App_Debug_FunctionCallTrace(strDebugMsg);
    }
    
    m_bShowToolTips = theApp.m_bShowToolTips; 
    if (m_bShowToolTips) ShowToolTips();
    m_nBuildOption = theApp.m_nBuildOption;
    m_nPicturesCategory = theApp.m_nImagesCategory;
    m_nEditOption = theApp.m_nEditOption;


    // Subclass like LV edit in COptionalSettings. Note: I use SubclassWindow 
    // instead of standard dynamic CEdit creation since failed to make EDIT 
    // visible (see Work.txt).
    m_ctlEditWithDAD.SubclassWindow(GetDlgItem(IDC_PP0_EDIT)->m_hWnd);

    if (m_nBuildOption == 0 /* New */)
    {
        // Get images directory (directory to seach out).
        m_strInitImages = theApp.m_strImagesDirectory;
        m_ctlComboBox.SetCurSel(0);
        SetDlgItemText(IDC_PP0_STATIC3, "Images Directory:");
        GetDlgItem(IDC_PP0_STATIC2)->ShowWindow(TRUE);
        m_ctlComboBox_PicturesCategory.ShowWindow(TRUE);

        // Set images category for combo box.
        if (m_nPicturesCategory == 0 /* General MSS */)
        {
            m_ctlComboBox_PicturesCategory.SetCurSel(0);
        }
        else if (m_nPicturesCategory == 1 /* Cars MSS */)
        {
            m_ctlComboBox_PicturesCategory.SetCurSel(1);
        }
    }
    else if (m_nBuildOption == 1 /* Edit */)
    {
        // Get images directory (directory to seach out).
        m_strInitImages = theApp.m_strLastEditedFile;
        m_ctlComboBox.SetCurSel(1);
        SetDlgItemText(IDC_PP0_STATIC3, "MSS File:");
        GetDlgItem(IDC_PP0_STATIC2)->ShowWindow(FALSE);
        m_ctlComboBox_PicturesCategory.ShowWindow(FALSE);
        // TODO: obtain images category from MSS
    }

    UpdateData(FALSE);
    
    return TRUE;  
}

void CMySaverPropPage0::OnBrowseClicked()
{
    CString strComboBoxString;
    CDirDialog dirDlg;
    CString m_szDirName;

    UpdateData(TRUE);
    m_ctlComboBox.GetLBText(m_ctlComboBox.GetCurSel(), strComboBoxString);
    
    if (strComboBoxString == "New (default)")
    {
        //CDirDialog dirDlg(m_strInitImagesDir);
        //dirDlg.m_strInitDir = _T("E:\\Development\\VCC\\MySaver.NET\\Images");
        if (dirDlg.DoBrowse())
        {
            m_szDirName = dirDlg.m_strPath;
            m_strInitImages = m_szDirName;
        }
    }
    else // Edit
    {
        OnBrowseMSS(this->m_hWnd, m_strInitImages);
    }

    // Pass data to dialog
    UpdateData(FALSE);
}

void CMySaverPropPage0::OnBrowseMSS(HWND hWnd, LPCTSTR pszInitImagesDir)
{
    OPENFILENAME	ofn;
    // Serves as initial directory and resulting file returned by the dialog.
    TCHAR			szMSSFile[MAX_PATH]; 
    //TCHAR			szInitImagesDir[MAX_PATH];

    //lstrcpy(szInitImagesDir, pszInitImagesDir);
    lstrcpy(szMSSFile, pszInitImagesDir);
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize			= sizeof(OPENFILENAME);
    ofn.hwndOwner			= hWnd;
    ofn.hInstance			= NULL;
    ofn.lpstrFilter			= _T("*.mss\0\0"); //__TEXT("MSS files\0*.mss\0\0"); 
    ofn.lpstrCustomFilter	= NULL;
    ofn.nMaxCustFilter		= 0;
    ofn.nFilterIndex		= 0;
    // Older (98/Me) ofn.lpstrInitialDir can be used to set initial directory, 
    // but ofn.lpstrFile parameter can be used instead since it serves two 
    // purposes (and supposed to be used mainly for setting initial directory 
    // with 2000/XP): set initial directory and receive selected full path name. 
    // I found that when setting initial directory ofn.lpstrFile works more 
    // stable (XP), but ofn.lpstrInitialDir can fail (FNERR_INVALIDFILENAME).
    ofn.lpstrFile			= szMSSFile; // gets selected full file path
    ofn.nMaxFile			= MAX_PATH;
    ofn.lpstrFileTitle		= NULL;
    ofn.nMaxFileTitle		= 0;
    //ofn.lpstrInitialDir		= _T("C:\\Program Files\\Synaptex\\MyScreenSaver\\Images"); //szInitImagesDir; // initial directory
    ofn.lpstrTitle			= _T("Select MSS file");
    ofn.Flags				= OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST; 
    ofn.nFileOffset			= 0;
    ofn.nFileExtension		= 0;
    ofn.lpstrDefExt			= NULL;
    ofn.lCustData			= 0;
    ofn.lpfnHook			= NULL;
    ofn.lpTemplateName		= NULL;

    if (GetOpenFileName(&ofn))
    {
        //TODO: check that returned file has MSS extension
        m_strInitImages = szMSSFile;
        //UpdateData(FALSE);
    }
    else
    {
  //      //AfxMessageBox("szInitImagesDir");
        //DWORD dw = CommDlgExtendedError();
        //CString str;
        //str.Format("%d", dw); -> FNERR_INVALIDFILENAME (29290->x3002)
        //AfxMessageBox(str);
    }
}

BOOL CMySaverPropPage0::OnSetActive()
{
    if (theApp.m_bEnableDebug)
    {
        theApp.App_Debug_FunctionCallTrace("CMySaverPropPage0::OnSetActive()");
    }
    
    CPropertySheet* pPropSheet = NULL;
    int iActivePropPage;
    HWND hWizardDlg;

    // Get data for setting property page title/subtitle dynamically.
    pPropSheet = (CPropertySheet*) GetParent();
    iActivePropPage = pPropSheet->GetActiveIndex();
    hWizardDlg = (HWND)((CPropertySheet*)GetParent()->m_hWnd);	
    
    //CPropertySheet* psheet = (CPropertySheet*) GetParent(); 

    if(IsShowWelcomePage())
    {
        //pPropSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
        pPropSheet->SetWizardButtons(PSWIZB_NEXT);
    }
    else
    {
        pPropSheet->SetWizardButtons(PSWIZB_NEXT);
    }

    // Set, dynamically, title and subtitle depending of mode. Note: does not 
    // work in OnInitDialog().
    if (m_nBuildOption == 0 /* New */)
    {
        PropSheet_SetHeaderTitle(hWizardDlg, iActivePropPage, 
                                                 (LPCTSTR)m_strHeaderTitle_New);
        PropSheet_SetHeaderSubTitle(hWizardDlg, iActivePropPage, 
                                              (LPCTSTR)m_strHeaderSubTitle_New);

        m_ctlGroupBox.ShowWindow(FALSE);
        m_ctlRadio1.ShowWindow(FALSE);
        m_ctlRadio2.ShowWindow(FALSE);
        m_ctlRadio3.ShowWindow(FALSE);
    }
    else if (m_nBuildOption == 1 /* Edit */)
    {
        PropSheet_SetHeaderTitle(hWizardDlg, iActivePropPage,
                                                (LPCTSTR)m_strHeaderTitle_Edit);
        PropSheet_SetHeaderSubTitle(hWizardDlg, iActivePropPage, 
                                             (LPCTSTR)m_strHeaderSubTitle_Edit);
    }

    return CPropertyPage::OnSetActive();
}

LRESULT CMySaverPropPage0::OnWizardBack()
{
    CMySaverApp* pApp =  static_cast<CMySaverApp*>(AfxGetApp());
       
    return pApp->m_iPrevPage;
    //return IDD_WELCOME;
    //return CPropertyPage::OnWizardBack();
}

LRESULT CMySaverPropPage0::OnWizardNext()
{
    if (theApp.m_bEnableDebug)
    {
        theApp.App_Debug_FunctionCallTrace(
                                           "CMySaverPropPage0::OnWizardNext()");
    }
    
    DWORD dwObjectProps; // file or folder (or error) indicator
    TCHAR szCorrectPassword[100];
    CMySaverApp* pMySS =  static_cast<CMySaverApp*>(AfxGetApp());

    // Update class data (m_nBuildOption, m_nPicturesCategory, m_strInitImages,
    // and m_nEditOption) from dialog data. 
    UpdateData(TRUE); 

    dwObjectProps = IsDirectory(m_strInitImages);

    // Make preliminary validation of m_strInitImages string (can be folder or
    // file):
    // 1) The path should not be empty ("New" and "Edit" modes).
    // 2) The path should point to existing (valid) folder ("New" mode) or
    //    to existing (valid) file ("Edit" mode).
    // 3) The path should not point to file ("New" mode) and folder 
    //    ("Edit" mode).
    // 4) The file must have ".mss" extension in "Edit" mode (case does not
    //    matter.)
    if (m_nBuildOption == 0 /* New (default) */)
    {
        if (m_strInitImages.IsEmpty())
        {
            ::MessageBox(NULL, "The images directory should not be empty!", 
                           "MyScreenSaver Builder 2.2", MB_OK | MB_ICONWARNING);
            return -1; // keep dialog active
        }
        else if (dwObjectProps == INVALID_FILE_ATTRIBUTES)
        {
            ::MessageBox(0, "The selected folder does not exist!", 
                           "MyScreenSaver Builder 2.2", MB_OK | MB_ICONWARNING);
            return -1; // keep dialog active
        }
        else if (dwObjectProps == 0 /* file found */)
        {
            ::MessageBox(0, "The path to the directory is not valid!", 
                           "MyScreenSaver Builder 2.2", MB_OK | MB_ICONWARNING);
            return -1; // keep dialog active
        }
    }
    else if (m_nBuildOption == 1 /* Edit */)
    {
        if (m_strInitImages.IsEmpty())
        {
            ::MessageBox(NULL, "The path to MSS file should not be empty!", 
                           "MyScreenSaver Builder 2.2", MB_OK | MB_ICONWARNING);
            return -1; // keep dialog active
        }
        else if (dwObjectProps == INVALID_FILE_ATTRIBUTES)
        {
            ::MessageBox(0, "The selected file does not exist!", 
                           "MyScreenSaver Builder 2.2", MB_OK | MB_ICONWARNING);
            return -1; // keep dialog active
        }
        else if (dwObjectProps == 0 /* file found */)
        {
            // Make an additional check if the file has MSS extension.
            if (m_strInitImages.Right(4) != ".mss" /* not case-sensitive */)
            {
               ::MessageBox(0, "The image set must have MSS extension!", 
                           "MyScreenSaver Builder 2.2", MB_OK | MB_ICONWARNING);
               return -1; // keep dialog active
            }
        }
        else if (dwObjectProps == 1 /* directory found */)
        {
            ::MessageBox(0, "The path to the file is not valid!", 
                           "MyScreenSaver Builder 2.2", MB_OK | MB_ICONWARNING);
            return -1; // keep dialog active
        }
    }

    // Persist paths and make additional validation for MSS file (loadability
    // and correctness of the password if set).
    
    pMySS->SavePage0Data(m_bShowToolTips, m_nBuildOption, 
                                            m_nPicturesCategory, m_nEditOption);

    // This is for consistency which means that these variables are persisted 
    // in application class and coulb be accessed in the next pages with 
    // theApp.<...> syntax.
    theApp.m_nBuildOption = m_nBuildOption;
    theApp.m_nImagesCategory = m_nPicturesCategory;
    if (m_nBuildOption == 0 /* New */)
        theApp.m_strImagesDirectory = m_strInitImages;
    else if (m_nBuildOption == 1 /* Edit */)
        theApp.m_strLastEditedFile = m_strInitImages;
    theApp.m_nEditOption = m_nEditOption;
    theApp.m_bShowToolTips = m_bShowToolTips;

    // Save directory or full file name (of MSS) stored in m_strInitImages. 
    if (m_nBuildOption == 0 /* New (default) */)
    {
        pMySS->SaveImagesDirToRegistry(m_strInitImages);    
    }
    if (m_nBuildOption == 1 /* Edit */)
    {
        pMySS->SaveLastFileToRegistry(m_strInitImages);

        // Validate if MSS file is loadable (GetPasswordFromMSS returns TRUE if
        // yes regardless MSS is password-protected or not) and validate 
        // password when set.
        if (GetPasswordFromMSS(m_strInitImages /* in */, 
                                                   szCorrectPassword /* out */))
        {
            // Store correct MSS password in the application class for later use 
            // with PP2.
            theApp.m_strMSSPassword = szCorrectPassword;
            if (lstrlen(szCorrectPassword) != 0 /* not empty */)
            {
                switch (m_dlgEP.DoModal())
                {
                    case IDOK:
                    {
                        if (lstrcmp(szCorrectPassword, 
                               m_dlgEP.m_strTypedPassword) != 0 /* not equal */) 
                                                                
                        {
                            // Empty symbols in text are for alignment. 
                            ::MessageBox(0, "       The password is incorrect!", 
                                                "MyScreenSaver Builder 2.2", 0); 
                            return -1; // stay on the same wizard's page
                        }
                        break;
                    }
                    case IDCANCEL:
                        return -1; // stay on the same wizard's page
                        break;
                    default:
                        return -1; // stay on the same wizard's page
                }
            }
        }
        else
        {
            // Stay on the same page if the path to MSS or MSS file is 
            // corrupt.
            ::MessageBox(0, 
                "The path to MSS file, or MSS file itself is invalid!", 
                                                "MyScreenSaver Builder 2.2", 0); 
                        return -1; // stay on the same wizard's page
        }
        //m_nPicturesCategory = EditMode_GetPicturesCategory(m_strInitImages);
    }

    if (m_nBuildOption == 1 /* edit mode */ && 
                                         m_nEditOption == 1 /* create MSS .. */)
    {
        return IDD_PP2;
    }
    else
        return IDD_PP1; 
}

BOOL CMySaverPropPage0::GetPasswordFromMSS(LPCTSTR pszFullFileName /* in */, 
                                                   LPTSTR pszPassword /* out */)
{
    HINSTANCE hResInst;
    TCHAR szBuffer[100];

    hResInst = LoadLibrary(pszFullFileName);

    if (hResInst == NULL)
    {
        return 0;
    }

    LoadString(hResInst, 23 /* password resource ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
    lstrcpy(pszPassword, szBuffer);

    FreeLibrary(hResInst);

    return 1;
}

// copy of IsShowWelcomePage() in PS: move on App level
BOOL CMySaverPropPage0::IsShowWelcomePage()
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

    // Get m_bShowWelcomePage
    BOOL m_bShowWelcomePage;
    dwResult = RegQueryValueEx(hKey, 
                               "ShowWelcomePage", 
                               0, 
                               &dwType, 
                               (LPBYTE)&m_bShowWelcomePage, 
                               &dwLength );

    if(dwResult != ERROR_SUCCESS) 
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

BOOL CMySaverPropPage0::PreTranslateMessage(MSG* pMsg)
{
    m_ctlToolTip.Activate(TRUE);
    m_ctlToolTip.RelayEvent(pMsg);
    
    return CPropertyPage::PreTranslateMessage(pMsg);
}

void CMySaverPropPage0::ShowToolTips()
{
    // Provide tooltips
    m_ctlToolTip.Create(this, TTS_ALWAYSTIP);
    m_ctlToolTip.SetMaxTipWidth(200);

    CString strToolTip1 = _T("This path shows the location of source images from which you'd build image set for your screen saver.");
    m_ctlToolTip.AddTool(GetDlgItem(IDC_PP0_EDIT), strToolTip1); 

    m_ctlToolTip.Activate(TRUE);
}

void CMySaverPropPage0::OnCheckBoxClicked()
{
    // Update class data from dialog data
    UpdateData(TRUE);
}

void CMySaverPropPage0::OnComboboxSelectionChange()
{
    CMySaverApp* pApp =  static_cast<CMySaverApp*>(AfxGetApp());
    CString strComboBoxString;
    CPropertySheet* pPropSheet = NULL;
    int iActivePropPage;
    HWND hWizardDlg;
    
    pPropSheet = (CPropertySheet*) GetParent();
    iActivePropPage = pPropSheet->GetActiveIndex();
    hWizardDlg = (HWND)((CPropertySheet*)GetParent()->m_hWnd);

    UpdateData(TRUE);
    m_ctlComboBox.GetLBText(m_ctlComboBox.GetCurSel(), strComboBoxString);
    
    if (strComboBoxString == "New (default)")
    {
        //Set page title and subtitle.
        PropSheet_SetHeaderTitle(hWizardDlg, iActivePropPage, 
                                                 (LPCTSTR)m_strHeaderTitle_New);
        PropSheet_SetHeaderSubTitle(hWizardDlg, iActivePropPage, 
                                              (LPCTSTR)m_strHeaderSubTitle_New);
        m_strInitImages = pApp->m_strImagesDirectory;
        SetDlgItemText(IDC_PP0_STATIC3, "Images Directory:");
        m_nBuildOption = 0;
        GetDlgItem(IDC_PP0_STATIC2)->ShowWindow(TRUE);

        // Set images category for combo box.
        if (m_nPicturesCategory == 0 /* General MSS */)
        {
            m_ctlComboBox_PicturesCategory.SetCurSel(0);
        }
        else if (m_nPicturesCategory == 1 /* Cars MSS */)
        {
            m_ctlComboBox_PicturesCategory.SetCurSel(1);
        }
        m_ctlComboBox_PicturesCategory.ShowWindow(TRUE);

        // Hide edit options.
        m_ctlGroupBox.ShowWindow(FALSE);
        m_ctlRadio1.ShowWindow(FALSE);
        m_ctlRadio2.ShowWindow(FALSE);
        m_ctlRadio3.ShowWindow(FALSE);
    }
    else // Edit
    {
        //Set page title and subtitle.
        PropSheet_SetHeaderTitle(hWizardDlg, iActivePropPage, 
                                                (LPCTSTR)m_strHeaderTitle_Edit);
        PropSheet_SetHeaderSubTitle(hWizardDlg, iActivePropPage, 
                                             (LPCTSTR)m_strHeaderSubTitle_Edit);
        m_strInitImages = pApp->m_strLastEditedFile;
        SetDlgItemText(IDC_PP0_STATIC3, "MSS File:");
        m_nBuildOption = 1;
        GetDlgItem(IDC_PP0_STATIC2)->ShowWindow(FALSE);
        m_ctlComboBox_PicturesCategory.ShowWindow(FALSE);

        // Show edit options.
        m_ctlGroupBox.ShowWindow(TRUE);
        m_ctlRadio1.ShowWindow(TRUE);
        m_ctlRadio2.ShowWindow(TRUE);
        m_ctlRadio3.ShowWindow(TRUE);
    }

    // Update program data to dialog.
    UpdateData(FALSE);
}

void CMySaverPropPage0::OnComboboxPicCategorySelectionChange()
{
    CString strComboBoxString;
    
    UpdateData(TRUE);
    m_ctlComboBox_PicturesCategory.GetLBText(
                 m_ctlComboBox_PicturesCategory.GetCurSel(), strComboBoxString);
    
    if (strComboBoxString == "Not specified (default)")
    {
        m_nPicturesCategory = 0;
    }
    else // Cars
    {
        m_nPicturesCategory = 1;
    }
}

//short CMySaverPropPage0::EditMode_GetPicturesCategory(LPCTSTR pszFullFileName)
//{
//	HINSTANCE hResInst;
//	TCHAR szBuffer[100];
//	CString strPicturesCategory;
//
//	hResInst = LoadLibrary(pszFullFileName);
//
//	if (hResInst == NULL)
//	{
//		AfxMessageBox("LoadLibrary failed!");
//	}
//
//	LoadString(hResInst, 4 /* pictures category ID */, szBuffer, 
//										  sizeof(szBuffer)/sizeof(szBuffer[0]));
//	strPicturesCategory = szBuffer;
//
//    if (strPicturesCategory == "Not specified")
//	{
//        return 0;
//	}
//	else if (strPicturesCategory == "Cars") 
//	{
//        return 1;
//	}
//
//	FreeLibrary(hResInst);
//}

DWORD CMySaverPropPage0::IsDirectory(LPCTSTR pszFullFileName)
{
    DWORD dwFileAttributes;

    dwFileAttributes = GetFileAttributes(pszFullFileName);

    if (dwFileAttributes == INVALID_FILE_ATTRIBUTES)
        return INVALID_FILE_ATTRIBUTES;
    
    if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
    {
        return TRUE;
    }
    else
        return FALSE;
}

// OnHelpInfo (WM_HELP handler) handles context help: 1) "?" context help, 2) 
// SHIFT+F1, 3) F1 (2 and 3 are the same AFAIS). 
BOOL CMySaverPropPage0::OnHelpInfo(HELPINFO* pHelpInfo)
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
    //for (int i = 0; i < 12; i++) //theApp.m_ContextHelp.m_nMapSizePP0; i++) 
    for (int i = 0; i < theApp.m_ContextHelp.m_nMapSizePP0; i++) 
    {
        if  (pHelpInfo->iCtrlId ==  theApp.m_ContextHelp.m_MapPP0[i].nControlID) 
        { 
            memset(&ContextHelpPopup, 0, sizeof(HH_POPUP) );   
            ContextHelpPopup.cbStruct = sizeof(HH_POPUP);

            ContextHelpPopup.hinst = AfxGetInstanceHandle(); // application 
                                                             // instance
            ContextHelpPopup.idString = 
                      theApp.m_ContextHelp.m_MapPP0[i].nStringHelpID; // resource ID 

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

// The function realizes "What's This?" context help.
void CMySaverPropPage0::OnContextMenu(CWnd* pWnd, CPoint point)
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
BOOL CMySaverPropPage0::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

//------------------------------------------------------------------------------
// To enable dropping file to edit control CEditWithDAD is derived from CEdit
// and contents of EditWithDAD.h added to header, and the content of 
// EditWithDAD.cpp is appended here.

IMPLEMENT_DYNAMIC(CEditWithDAD, CEdit)
CEditWithDAD::CEditWithDAD(){}
CEditWithDAD::~CEditWithDAD(){}

BEGIN_MESSAGE_MAP(CEditWithDAD, CEdit)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()

// Note: works for single files (edit mode) and for a folder (new collection).
void CEditWithDAD::OnDropFiles(HDROP hDropInfo)
{
    TCHAR szFFN[MAX_PATH];

    // MSDN (second parameter): "If the value of the iFile parameter is between 
    // zero and the total number of files dropped, DragQueryFile copies the file 
    // name with the corresponding value to the buffer pointed to by the 
    // lpszFile parameter."
    DragQueryFile(hDropInfo, 0 /* iFile */, szFFN, sizeof(szFFN)/sizeof(TCHAR)); 
    DragFinish(hDropInfo);

    // Update text in edit control.
    SetWindowText(szFFN);
}
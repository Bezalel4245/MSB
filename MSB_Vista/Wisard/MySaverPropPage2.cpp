#include "stdafx.h"
#include "MySaver.h"
#include "MySaverPropPage2.h"
#include "MySaverPropPage0.h" //added to navigate from PP2 to PP0. (redundant?)
#include "MySaverPropPage1.h" //added to navigate from PP1 to PP0. (redundant?)
#include "Devices.h"
#include ".\mysaverproppage2.h"

void AddImage();

IMPLEMENT_DYNAMIC(CMySaverPropPage2, CPropertyPage)
CMySaverPropPage2::CMySaverPropPage2()
    : CPropertyPage(CMySaverPropPage2::IDD)
    , m_strImageSetTitle(_T(""))
    , m_strImageSetName(_T(""))
    , m_strSetupName(_T(""))
    , m_strSetupDir(_T(""))
    , m_bCheckBoxCreateSetup(FALSE)
    , m_strMSSSpacePaddedComments(_T(""))
{
    m_psp.dwFlags |= PSP_USETITLE | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE 
                                                                  | PSP_HASHELP; 
    m_psp.pszHeaderTitle = MAKEINTRESOURCE(IDS_PP2_HEADER_TITLE);
    m_psp.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_PP2_HEADER_SUBTITLE); 
}

CMySaverPropPage2::~CMySaverPropPage2()
{
}

void CMySaverPropPage2::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_PP2_EDIT_TITLE, m_strImageSetTitle);
    DDX_Text(pDX, IDC_PP2_EDIT_NAME, m_strImageSetName);
    DDX_Text(pDX, IDC_PP2_EDIT_SETUP_NAME, m_strSetupName);
    DDX_Control(pDX, IDC_PP2_PROGRESSBAR, m_ctlProgressBar);
    DDX_Text(pDX, IDC_PP2_EDIT_LOCATION, m_strSetupDir);
    DDX_Check(pDX, IDC_PP2_CHECKBOX, m_bCheckBoxCreateSetup);
    DDX_Control(pDX, IDC_PP2_EDIT_SETUP_NAME, m_ctlEditSetupName);
    DDX_Control(pDX, IDC_PP2_EDIT_LOCATION, m_ctlEditSetupLocation);
}

BEGIN_MESSAGE_MAP(CMySaverPropPage2, CPropertyPage)
    ON_BN_CLICKED(IDC_PP2_BUTTON_MORE_PARAMS, OnButtonMoreParamsClicked)
    ON_BN_CLICKED(IDC_PP2_BUTTON_CHANGE, OnButtonChangeClicked)
    ON_BN_CLICKED(IDC_PP2_CHECKBOX, OnCheckboxCreateSetupClicked)
    ON_EN_CHANGE(IDC_PP2_EDIT_TITLE, OnEditChange_Title)
    ON_EN_CHANGE(IDC_PP2_EDIT_NAME, OnEditChange_FileName)
    ON_WM_HELPINFO()
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
// Description: Because OnInitInstance, unlike OnSetActive, called only
//              once (i.e. per PropertySheet) page-related initizations moved  
//              to OnSetActive.
//------------------------------------------------------------------------------

BOOL CMySaverPropPage2::OnInitDialog() 
{
    CPropertyPage::OnInitDialog();
    //--------------------------------------------------------------------------

    if (theApp.m_bEnableDebug)
    {
        CString strDebugMsg; 
        theApp.m_dwPP2InitTime = GetTickCount();
        strDebugMsg.Format("CMySaverPropPage2::OnInitDialog(): %d", 
                               theApp.m_dwPP2InitTime - theApp.m_dwPP1InitTime); 
        theApp.App_Debug_FunctionCallTrace(strDebugMsg);
    }

    // Get images directory (directory to seach out)
    CMySaverApp* pApp =  static_cast<CMySaverApp*>(AfxGetApp());
    m_strImagesDir = pApp->m_strInstallDirectory + _T("\\Images");
    m_strInstallDir = pApp->m_strInstallDirectory;
    m_strSetupDir = pApp->m_strSetupDeploymentLocation;

    // Get create setup program checkbox state
    m_bCheckBoxCreateSetup = pApp->m_bCreateSetup;

    // Enable/disable controls and text depending on Create Setup check box.
    if (m_bCheckBoxCreateSetup)
    {
        // Short way to hide/show (without making control variable).
        GetDlgItem(IDC_PP2_STATIC5)->EnableWindow(TRUE);
        GetDlgItem(IDC_PP2_STATIC6)->EnableWindow(TRUE);
        GetDlgItem(IDC_PP2_STATIC7)->EnableWindow(TRUE);

        m_ctlEditSetupName.EnableWindow(TRUE);
        m_ctlEditSetupLocation.EnableWindow(TRUE);
        GetDlgItem(IDC_PP2_BUTTON_CHANGE)->EnableWindow(TRUE);
    }
    else
    {
        // Short way to hide/show (without making control variable).
        GetDlgItem(IDC_PP2_STATIC5)->EnableWindow(FALSE);
        GetDlgItem(IDC_PP2_STATIC6)->EnableWindow(FALSE);
        GetDlgItem(IDC_PP2_STATIC7)->EnableWindow(FALSE);

        m_ctlEditSetupName.EnableWindow(FALSE);
        m_ctlEditSetupLocation.EnableWindow(FALSE);
        GetDlgItem(IDC_PP2_BUTTON_CHANGE)->EnableWindow(FALSE);
    }

    m_ctlComboBox.SetCurSel(0);

    // Title is not yet modified manually.
    m_bTitleDirty = FALSE;

    // File name (without extension) is not yet modified manually.
    m_bFileNameDirty = FALSE;

    // The parameterd from optional settings dialog are not yet modified 
    // manually.
    m_bOptionalSettingsDirty = FALSE;

    UpdateData(FALSE);
           
    return TRUE; 
}

//------------------------------------------------------------------------------
// Description: Unlike OnInitInstance, OnSetActive called every time
//              a page becomes visible. Thus, data from PP0 are always current.
//------------------------------------------------------------------------------

BOOL CMySaverPropPage2::OnSetActive()
{
    if (theApp.m_bEnableDebug)
    {
        theApp.App_Debug_FunctionCallTrace("CMySaverPropPage2::OnSetActive()");
    }
    
    CString strStandardName; 
    CString strFileName; // MSS file name without extension
    int iPageCount;

    CMySaverApp* pApp =  static_cast<CMySaverApp*>(AfxGetApp());

    // Get data from PP0.
    iPageCount = ((CMySaverPropSheet*) GetParent())->GetPageCount();
    CMySaverPropPage0* pPP0 = (CMySaverPropPage0*)
    ((CMySaverPropSheet*)GetParent())->GetPage(iPageCount - 3 /* zero-based */);

    m_nBuildOption = pPP0->m_nBuildOption;
    m_strInitImages = pPP0->m_strInitImages; 
    m_nPicturesCategory = pPP0->m_nPicturesCategory;
    m_bShowToolTips = pPP0->m_bShowToolTips; if (m_bShowToolTips) ShowToolTips();

    // If there are images marked for deletion in previous page, exract images
    // as byte resources from MSS, make new RC (without images yet), and
    // finally insert images.
    iPageCount = ((CMySaverPropSheet*) GetParent())->GetPageCount();
    CMySaverPropPage1* pPP1 = (CMySaverPropPage1*)((CMySaverPropSheet*) 
                         GetParent())->GetPage(iPageCount - 2 /* zero-based */);

    // Provide defaults strings necessary to build MSS in New and Edit modes 
    // (MSS is rebuilt in Edit mode only if images are marked for deletion).
    // This fragment must be in OnSetActive section since image count depends
    // on previous page, and some can be deleted there (LL pertinent to edit 
    // mode basically/only). 
    strStandardName = GetStandardName();
    if (m_nBuildOption == 0 /* New MSS */)
    {
        m_strImageSetTitle = strStandardName;
        m_strImageSetName = strStandardName;
        m_strPassword = ""; 
        CString strTemp(' ', 80);
        m_strMSSSpacePaddedComments = strTemp; //"     ";
    }
    else if (m_nBuildOption == 1 /* Edit MSS */)
    {
        GetDlgItem(IDC_PP2_PROGRESSBAR)->ShowWindow(FALSE);
        
        // Edit mode with MSS rebuild.
        if (pPP1->m_bIsImagesDeletionMarked == TRUE || 
                                             pPP1->m_bIsImagesReordered == TRUE)
        {
            EditMode_GetImageSetData(m_strInitImages, TRUE /* for rebuild */);
        }
        else // edit mode with no MSS rebuild
        {
            EditMode_GetImageSetData(m_strInitImages, FALSE /* no rebuild */);
        }
        m_strImageSetTitle = m_cd.strImageSetTitle;
        m_strImageSetName = pApp->
            Utility_GetFileNameFromFullFileName(m_strInitImages, 
                                                 FALSE /* without extension */);
        
        m_strPassword = m_cd.strPassword;
        m_strMSSSpacePaddedComments = m_cd.strMSSComments;
    }
    m_strSetupName = strStandardName;

    CPropertySheet* psheet = (CPropertySheet*) GetParent();   
     psheet->SetWizardButtons(PSWIZB_FINISH);

    if (m_bCheckBoxCreateSetup)
    {
        //psheet->SetFinishText("Build");
    }

    UpdateData(FALSE);

    StartUnpackFilesThread();

    // In edit mode only, rebuild MSS if there are images maked for deletion. 
    // Note that in the edit mode MSS file is rebuilt only in this case, in 
    // other cases MSS file is updated with LoadLibrary/LoadResource/
    // UpdateResource.
    if (m_nBuildOption == 1 /* edit mode */)
    {
        if (pPP1->m_bIsImagesDeletionMarked == TRUE || 
                                             pPP1->m_bIsImagesReordered == TRUE)
        {
            // Looks like there is a little sense in making visible, and then 
            // hiding progress bar. It takes < 0.5 sec for 4 images to pass 
            // prebuild step (creates MSS without images).
            GetDlgItem(IDC_PP2_PROGRESSBAR)->ShowWindow(TRUE);

            EditMode_MakeRCFile();
            // Build MSS without images (m_strImageSetName already obtained 
            // from source MSS).
            EditMode_PrebuildMSS();
            EditMode_AddImagesToMSS(m_strInitImages);

            GetDlgItem(IDC_PP2_PROGRESSBAR)->ShowWindow(FALSE);
        }
    }

    return CPropertyPage::OnSetActive();
}

BOOL CMySaverPropPage2::MakeRCFile()
{
    CString strResourceFile;
    CString strCategory;
    HANDLE hLog;
    
    UpdateData(TRUE);

    strResourceFile = theApp.m_strMSBDataDirectory + _T("\\Build\\myss.rc"); 

    //--------------------------------------------------------------------------
    // CreateFile uses such parameters so that if the log file exits, the file 
    // is just opened, if the file does not not exist it is created
    //--------------------------------------------------------------------------
    
    hLog = CreateFile(  strResourceFile, //MFC's CString use is OK here
                        GENERIC_READ|GENERIC_WRITE, 
                        FILE_SHARE_READ|FILE_SHARE_WRITE, 
                        NULL,                  //security - not used          
                        CREATE_ALWAYS, 
                        FILE_ATTRIBUTE_NORMAL, //normal use
                        NULL );         

    // Tell the writer we need to write at the and of file
    DWORD dwOffset = SetFilePointer(hLog, 0, NULL, FILE_END); 
 
    DWORD dwWritten;
    CString strResourceLine;

    //--------------------------------------------------------------------------
    // Write standard header for RC file
    //--------------------------------------------------------------------------

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "// Standard header for resource file\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "#include \"resource_is.h\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "// End of standard header for resource file\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    //--------------------------------------------------------------------------
    // End of standard header for RC file
    //--------------------------------------------------------------------------

    // Get images directory (directory to seach out)
    CString strFileName;

    TCHAR szSearchDir[300];
    wsprintf(szSearchDir, "%s%s", m_strInitImages, "\\*.jpg");

    //-----------------------------------------------------------------------------
    // Transform directory string since image path in RC file must be duoble 
    // slashed.
    //-----------------------------------------------------------------------------

    CString strImageDir = m_strInitImages;
    strImageDir.Replace("\\", "\\\\");

    // Get accepted images from the list view property page.
    int iPageCount = ((CMySaverPropSheet*) GetParent())->GetPageCount();
    CMySaverPropPage1* pPP1 = (CMySaverPropPage1*)((CMySaverPropSheet*) 
                 GetParent())->GetPage(iPageCount - 2 /* zero-based */);

    USHORT iCount = 0;
    USHORT iImagesCount;
    TCHAR szCount[5];
    CString strCount;
    CString strFullFileName; 

    strImageDir.Replace("\\", "\\\\");
    
    for( int i = 0; i < pPP1->m_vecImages.size(); i++)
    {
        iCount++;
        wsprintf(szCount, "%d", iCount);
        strCount = szCount;
        
        iImagesCount = pPP1->m_vecImages.size();
        strFullFileName = pPP1->m_vecImages.at(i).m_strFullFileName;
        strFullFileName.Replace("\\", "\\\\");
        strResourceLine = "IDR_IMAGES" + strCount + " IMAGES \"" + 
                                                     strFullFileName + "\"\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
    }

    // Make basic data strings block.
    strResourceLine = "STRINGTABLE\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_TITLE \"" + m_strImageSetTitle + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_IMAGECOUNT \"" + strCount + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    if (m_nPicturesCategory == 0)
    {
        strCategory = "Not specified";
    }
    else
    {
        strCategory = "Cars";
    }
    strResourceLine = "IDS_CATEGORY \"" + strCategory + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    CString strMSS_Format_Version = "1.1"; // TODO: variable
    strResourceLine = "IDS_MSS_FORMAT_VERSION \"" + strMSS_Format_Version + 
                                                                       "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    CString strIsSample = "1"; // TODO: variable 
    strResourceLine = "IDS_SAMPLE \"" + strIsSample + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    CString strIsRegistered = "0"; // TODO: variable 
    strResourceLine = "IDS_REGISTERED \"" + strIsRegistered + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    CString strIsCustom = "0"; // TODO: variable 
    strResourceLine = "IDS_CUSTOM \"" + strIsCustom + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    //CString strPassword = ""; // TODO: variable
    strResourceLine = "IDS_PASSWORD \"" + m_strPassword + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    strResourceLine = "IDS_BRAND \" \"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_MAKE \" \"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    // Make IDS_TITLE* strings block.
    strResourceLine = "STRINGTABLE \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    iCount = 0;
    for( int i = 0; i < pPP1->m_vecImages.size(); i++)
    {
        iCount++;
        wsprintf(szCount, "%d", iCount);
        strCount = szCount;

        iImagesCount = pPP1->m_vecImages.size();
        strResourceLine = "IDS_TITLE" + strCount + " " + "\"" + 
                             pPP1->m_vecImages.at(i).m_strTitle + "\"" + "\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
    }

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    // Make IDS_FILE_NAME* strings block.
    strResourceLine = "STRINGTABLE \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    strResourceLine = "BEGIN \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    iCount = 0;
    for (int i = 0; i < pPP1->m_vecImages.size(); i++)
    {
        iCount++;
        wsprintf(szCount, "%d", iCount);
        strCount = szCount;

        iImagesCount = pPP1->m_vecImages.size();
        strResourceLine = "IDS_FILE_NAME" + strCount + " " + "\"" + 
                          pPP1->m_vecImages.at(i).m_strFileName + "\"" + "\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
    }
    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    //--
    strResourceLine = "STRINGTABLE \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    // Year, Make, and Country are the same for all images in car's MSS. The 
    // value of first element is taken (zero-based). 
    strResourceLine = "BEGIN \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    CString strQuote = "\""; // error if used directly.
    strResourceLine = "IDS_XML_YEAR " + strQuote + 
                          pPP1->m_vecImages.at(0).m_strYear + strQuote + "\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
                                                          
    strResourceLine = "IDS_XML_MAKE " + strQuote + 
                          pPP1->m_vecImages.at(0).m_strMake + strQuote + "\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
                                                             
    strResourceLine = "IDS_XML_COUNTRY " + strQuote + 
                       pPP1->m_vecImages.at(0).m_strCountry + strQuote + "\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
                                                          
    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    strResourceLine = "STRINGTABLE \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    //
    strResourceLine = "BEGIN \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    iCount = 0;
    for (int i = 0; i < pPP1->m_vecImages.size(); i++)
    {
        iCount++;
        wsprintf(szCount, "%d", iCount);
        strCount = szCount;

        iImagesCount = pPP1->m_vecImages.size();
        //strResourceLine = "IDS_XML_YEAR" + strCount + " " + "\"" + 
        //	pPP1->m_vecImages.at(iImagesCount - i - 1).m_strYear + 
        //	                                                      "\"" + "\r\n";
     //   WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
        //	                                                  &dwWritten, NULL);
        //strResourceLine = "IDS_XML_MAKE" + strCount + " " + "\"" +
        //	pPP1->m_vecImages.at(iImagesCount - i - 1).m_strYear + 
        //	                                                      "\"" + "\r\n";
        //WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
        //	                                                  &dwWritten, NULL);
        strResourceLine = "IDS_XML_MODEL" + strCount + " " + "\"" +
                             pPP1->m_vecImages.at(i).m_strModel + "\"" + "\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);

        strResourceLine = "IDS_XML_SUBMODEL" + strCount + " " + "\"" +
                          pPP1->m_vecImages.at(i).m_strSubmodel + "\"" + "\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);

        strResourceLine = "IDS_XML_TYPE" + strCount + " " + "\"" +
                              pPP1->m_vecImages.at(i).m_strType + "\"" + "\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);

        strResourceLine = "IDS_XML_PRICE" + strCount + " " + "\"" +
                             pPP1->m_vecImages.at(i).m_strPrice + "\"" + "\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);

        strResourceLine = "IDS_XML_POWER" + strCount + " " + "\"" +
                             pPP1->m_vecImages.at(i).m_strPower + "\"" + "\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);

        strResourceLine = "IDS_XML_FUEL" + strCount + " " + "\"" +
                              pPP1->m_vecImages.at(i).m_strFuel + "\"" + "\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);

        strResourceLine = "IDS_XML_LENGTH" + strCount + " " + "\"" +
            pPP1->m_vecImages.at(i).m_strLength + "\"" + "\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);

        strResourceLine = "IDS_XML_WIDTH" + strCount + " " + "\"" +
                             pPP1->m_vecImages.at(i).m_strWidth + "\"" + "\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);

        strResourceLine = "IDS_XML_HEIGHT" + strCount + " " + "\"" +
                            pPP1->m_vecImages.at(i).m_strHeight + "\"" + "\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);

        strResourceLine = "IDS_XML_WEIGHT" + strCount + " " + "\"" +
                            pPP1->m_vecImages.at(i).m_strWeight + "\"" + "\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);

        strResourceLine = "IDS_XML_CLEARANCE" + strCount + " " + "\"" +
                         pPP1->m_vecImages.at(i).m_strClearance + "\"" + "\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);

        //strResourceLine = "IDS_XML_COUNTRY" + strCount + " " + "\"" +
        //	pPP1->m_vecImages.at(iImagesCount - i - 1).m_strYear + 
        //	                                                      "\"" + "\r\n";
        //WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
        //	                                                  &dwWritten, NULL);
        strResourceLine = "IDS_XML_TANK_VOLUME" + strCount + " " + "\"" +
                        pPP1->m_vecImages.at(i).m_strTankVolume + "\"" + "\r\n";
        WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
    }

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    // Write version info section.
    strResourceLine = "1 VERSIONINFO\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = " FILEVERSION 1,0\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BLOCK \"StringFileInfo\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BLOCK \"040904b0\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    strResourceLine = "VALUE \"FileDescription\", \"Collection of Images and Descriptions\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"Comments\", \"" + m_strMSSSpacePaddedComments + "\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"FileVersion\", \"1, 0\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"LegalCopyright\", \"\Copyright \251 2005-2012 MSB LLC. All rights reserved.\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    //strResourceLine = "VALUE \"ProductName\", \"MyScreenSaver Light Setup\"\r\n"; 
    strResourceLine = "VALUE \"ProductName\", \"MyScreenSaver Builder\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BLOCK \"VarFileInfo\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"Translation\", 0x409, 1200\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    //--------------------------------------------------------------------------
    // Write standard footer for RC file
    //--------------------------------------------------------------------------

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "// Standard footer for resource file\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "// End of standard footer for resource file\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    CloseHandle(hLog);

    return (TRUE);
}

BOOL CMySaverPropPage2::MakeSetupRCFile()
{
    UpdateData(TRUE);
    
    CString strResourceFile = theApp.m_strMSBDataDirectory + 
                                                _T("\\Build\\selfextractor.rc");

    //--------------------------------------------------------------------------
    // CreateFile uses such parameters so that if the log file exits, the file 
    // is just opened, if the file does not not exist it is created
    //--------------------------------------------------------------------------
    
    HANDLE hLog;
    hLog = CreateFile(  strResourceFile, //MFC's CString use is OK here
                        GENERIC_READ|GENERIC_WRITE, 
                        FILE_SHARE_READ|FILE_SHARE_WRITE, 
                        NULL,                  //security - not used          
                        CREATE_ALWAYS, 
                        FILE_ATTRIBUTE_NORMAL, //normal use
                        NULL );         

    // Tell the writer we need to write at the and of file
    DWORD dwOffset = SetFilePointer(hLog, 0, NULL, FILE_END); 
 
    DWORD dwWritten;
    CString strResourceLine;

    //--------------------------------------------------------------------------
    // Write standard header for RC file
    //--------------------------------------------------------------------------

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "// Standard header for resource file\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "#include \"resource_se.h\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    // Constants deefinitions for final dialog (from winuser.h)
    strResourceLine = "#define IDOK 1\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "#define IDCANCEL 2\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    strResourceLine = "#define DS_SETFONT 0x40L\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "#define DS_MODALFRAME 0x80L\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "#define DS_FIXEDSYS 0x0008L\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "#define WS_CAPTION 0x00C00000L\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "#define WS_SYSMENU 0x00080000L\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "#define WS_VISIBLE 0x10000000L\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "#define BS_AUTORADIOBUTTON 0x00000009L\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "#define WS_GROUP 0x00020000L\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "#define SS_NOTIFY 0x00000100L\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "#define WS_DISABLED 0x08000000L\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "#define WS_EX_STATICEDGE 0x00020000L\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDI_SELFEXTRACTOR_WIN32 ICON \"SelfExtractor.ico\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "// End of standard header for resource file\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    //--------------------------------------------------------------------------
    // End of standard header for RC file
    //--------------------------------------------------------------------------

    strResourceLine = "IDR_CUSTOM1 CUSTOM \"SLite.scr\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDR_CUSTOM2 CUSTOM \"" + m_strImageSetName + _T(".mss") + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "STRINGTABLE\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_IMAGE_SET_NAME  \"" + m_strImageSetName + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    //--
    strResourceLine = "IDD_FINISH DIALOGEX 22, 17, 276, 79\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_CAPTION | WS_SYSMENU\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "CAPTION \"MyScreenSaver Lite Setup\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "FONT 8, \"MS Shell Dlg\", 0, 0, 0x1\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "GROUPBOX        \"\",-1,11,4,196,47\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    strResourceLine = "LTEXT           \"Welcome to MyScreenSaver Lite v1.4.16 Setup! \", IDC_STATIC_TEXT,33,16,159,8\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "LTEXT           \" \", ,IDC_STATIC_TEXT2,0,0,0,0,SS_NOTIFY\r\n"; // any position, set dynamycally
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    strResourceLine = "PUSHBUTTON      \"Install\",IDC_BUTTON_INSTALL,74,30,73,14\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    strResourceLine = "PUSHBUTTON      \"Close\",IDCANCEL,216,11,50,14\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "PUSHBUTTON      \"Help\",IDC_BUTTON_HELP,216,33,50,14\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "LTEXT           \"Installer by\",-1,13,60,40,10,WS_DISABLED\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "LTEXT           \"MSB LLC\",IDC_STATIC_LINK,53,60,92,10,SS_NOTIFY\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "LTEXT           \"\",-1,90,65,116,1,0,WS_EX_STATICEDGE\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);			

    // Write manifest info.
    strResourceLine = "1  RT_MANIFEST \"MSLite_Setup.manifest\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    //--
    strResourceLine = "1 VERSIONINFO\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = " FILEVERSION 1,3,6,0\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BLOCK \"StringFileInfo\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BLOCK \"040904b0\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    strResourceLine = "VALUE \"FileDescription\", \"MyScreenSaver Lite Setup\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"Comments\", \"" + m_strSetupDesc + "\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"FileVersion\", \"1,3,6,0\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"LegalCopyright\", \"\Copyright \251 2005-2012 MSB LLC. All rights reserved.\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"ProductName\", \"MyScreenSaver Builder 2.2\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BLOCK \"VarFileInfo\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"Translation\", 0x409, 1200\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    CloseHandle(hLog);

    return (TRUE);
}

// Based on MakeRCFile
BOOL CMySaverPropPage2::EditMode_MakeRCFile()
{
    UpdateData(TRUE);

    CString strResourceFile = theApp.m_strMSBDataDirectory + 
                                                         _T("\\Build\\myss.rc"); 

    //--------------------------------------------------------------------------
    // CreateFile uses such parameters so that if the log file exits, the file 
    // is just opened, if the file does not not exist it is created
    //--------------------------------------------------------------------------
    
    HANDLE hLog;
    hLog = CreateFile(  strResourceFile, //MFC's CString use is OK here
                        GENERIC_READ|GENERIC_WRITE, 
                        FILE_SHARE_READ|FILE_SHARE_WRITE, 
                        NULL,                  //security - not used          
                        CREATE_ALWAYS, 
                        FILE_ATTRIBUTE_NORMAL, //normal use
                        NULL );         

    // Tell the writer we need to write at the and of file
    DWORD dwOffset = SetFilePointer(hLog, 0, NULL, FILE_END); 
 
    DWORD dwWritten;
    CString strResourceLine;

    //--------------------------------------------------------------------------
    // Write standard header for RC file
    //--------------------------------------------------------------------------

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "// Standard header for resource file\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "#include \"resource_is.h\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "// End of standard header for resource file\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    //--------------------------------------------------------------------------
    // End of standard header for RC file
    //--------------------------------------------------------------------------

    // Get images directory (directory to seach out)
    CString strFileName;

    TCHAR szSearchDir[300];
    wsprintf(szSearchDir, "%s%s", m_strInitImages, "\\*.jpg");

    //-----------------------------------------------------------------------------
    // Transform directory string since image path in RC file must be duoble 
    // slashed.
    //-----------------------------------------------------------------------------

    CString strImageDir = m_strInitImages;
    strImageDir.Replace("\\", "\\\\");

    // Get accepted images from the list view property page.
    int iPageCount = ((CMySaverPropSheet*) GetParent())->GetPageCount();
    CMySaverPropPage1* pPP1 = (CMySaverPropPage1*)((CMySaverPropSheet*) 
                                          GetParent())->GetPage(iPageCount - 2); 

    USHORT iCount = 0;
    USHORT iImagesCount;
    TCHAR szCount[5];
    CString strCount;

    // When MSS is rebuilt, no writing to IDR_IMAGES section yet (images 
    // inserted in MSS). The new number of images is calculated only.
    wsprintf(szCount, "%d", EditMode_GetImageCountWithoutDeletionMarked()); 
    strCount = szCount; //AfxMessageBox(strCount);

    // Make basic data strings block.
    strResourceLine = "STRINGTABLE\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_TITLE \"" + m_cd.strImageSetTitle + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_IMAGECOUNT \"" + strCount + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_CATEGORY \"" + m_cd.strPicturesCategory + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_MSS_FORMAT_VERSION \"" + m_cd.strMSSFormatVersion + 
                                                                       "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_SAMPLE \"" + m_cd.strIsSample + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_REGISTERED \"" + m_cd.strIsRegistered + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_CUSTOM \"" + m_cd.strIsCustom + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_PASSWORD \"" + m_cd.strPassword + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_RESERVED1 \"" + m_cd.strReserved1 + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_RESERVED2 \"" + m_cd.strReserved2 + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_RESERVED3 \"" + m_cd.strReserved3 + "\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    strResourceLine = "IDS_BRAND \" \"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "IDS_MAKE \" \"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    // Make IDS_TITLE* strings block.
    strResourceLine = "STRINGTABLE \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    iCount = 0;
    for( int i = 0; i < pPP1->m_vecImages.size(); i++)
    {
        if (pPP1->m_vecImages.at(i).bDeletionMarked == FALSE)
        {
            iCount++;
            wsprintf(szCount, "%d", iCount);
            strCount = szCount;

            strResourceLine = "IDS_TITLE" + strCount + " " + "\"" + 
                pPP1->m_vecImages.at(i).m_strTitle + "\"" + "\r\n";
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
        }
    }

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    // Make IDS_FILE_NAME* strings block.
    strResourceLine = "STRINGTABLE \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    strResourceLine = "BEGIN \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    iCount = 0;
    for (int i = 0; i < pPP1->m_vecImages.size(); i++)
    {
        if (pPP1->m_vecImages.at(i).bDeletionMarked == FALSE)
        {
            iCount++;
            wsprintf(szCount, "%d", iCount);
            strCount = szCount;

            iImagesCount = pPP1->m_vecImages.size();
            strResourceLine = "IDS_FILE_NAME" + strCount + " " + "\"" + 
                            pPP1->m_vecImages.at(i).m_strFileName + "\"" + "\r\n";
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                                &dwWritten, NULL);
        }
    }
    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    //--
    strResourceLine = "STRINGTABLE \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    // Year, Make, and Country are the same for all images in car's MSS. The 
    // value of first element is taken (zero-based). 
    strResourceLine = "BEGIN \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    CString strQuote = "\""; // error if used directly.
    strResourceLine = "IDS_XML_YEAR " + strQuote + 
                          pPP1->m_vecImages.at(0).m_strYear + strQuote + "\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
                                                          
    strResourceLine = "IDS_XML_MAKE " + strQuote + 
                          pPP1->m_vecImages.at(0).m_strMake + strQuote + "\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
                                                             
    strResourceLine = "IDS_XML_COUNTRY " + strQuote + 
                       pPP1->m_vecImages.at(0).m_strCountry + strQuote + "\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
                                                          
    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    strResourceLine = "STRINGTABLE \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    //
    strResourceLine = "BEGIN \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    iCount = 0;
    for (int i = 0; i < pPP1->m_vecImages.size(); i++)
    {
        if (pPP1->m_vecImages.at(i).bDeletionMarked == FALSE)
        {
            iCount++;
            wsprintf(szCount, "%d", iCount);
            strCount = szCount;

            iImagesCount = pPP1->m_vecImages.size();
            strResourceLine = "IDS_XML_MODEL" + strCount + " " + "\"" +
                             pPP1->m_vecImages.at(i).m_strModel + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_SUBMODEL" + strCount + " " + "\"" +
                          pPP1->m_vecImages.at(i).m_strSubmodel + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_TYPE" + strCount + " " + "\"" +
                              pPP1->m_vecImages.at(i).m_strType + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_PRICE" + strCount + " " + "\"" +
                             pPP1->m_vecImages.at(i).m_strPrice + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_POWER" + strCount + " " + "\"" +
                             pPP1->m_vecImages.at(i).m_strPower + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_FUEL" + strCount + " " + "\"" +
                              pPP1->m_vecImages.at(i).m_strFuel + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_LENGTH" + strCount + " " + "\"" +
                            pPP1->m_vecImages.at(i).m_strLength + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_WIDTH" + strCount + " " + "\"" +
                             pPP1->m_vecImages.at(i).m_strWidth + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_HEIGHT" + strCount + " " + "\"" +
                            pPP1->m_vecImages.at(i).m_strHeight + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_WEIGHT" + strCount + " " + "\"" +
                            pPP1->m_vecImages.at(i).m_strWeight + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_CLEARANCE" + strCount + " " + "\"" +
                         pPP1->m_vecImages.at(i).m_strClearance + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_TANK_VOLUME" + strCount + " " + "\"" +
                        pPP1->m_vecImages.at(i).m_strTankVolume + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
        }
    }

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    // Write version info section.
    strResourceLine = "1 VERSIONINFO\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = " FILEVERSION 1,0\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BLOCK \"StringFileInfo\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BLOCK \"040904b0\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    strResourceLine = "VALUE \"FileDescription\", \"Image set file (MSS) for MyScreenSaver Builder\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"Comments\", \"" + m_cd.strMSSComments + "\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"FileVersion\", \"1, 0\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"LegalCopyright\", \"\Copyright \251 2005-2012 MSB LLC. All rights reserved.\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    //strResourceLine = "VALUE \"ProductName\", \"MyScreenSaver Light Setup\"\r\n"; 
    strResourceLine = "VALUE \"ProductName\", \"MyScreenSaver Builder\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BLOCK \"VarFileInfo\"\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "BEGIN\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"Translation\", 0x409, 1200\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    //--------------------------------------------------------------------------
    // Write standard footer for RC file
    //--------------------------------------------------------------------------

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "// Standard footer for resource file\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "// End of standard footer for resource file\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "//--------------------------------------------------------------------------\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    CloseHandle(hLog);

    return (TRUE);
}

BOOL CMySaverPropPage2::IsValid()
{
    // Update class data from dialog.
    UpdateData(TRUE);

    BOOL bGenerateWarning = FALSE;
    CString strMessage;

    if(m_strImageSetTitle.IsEmpty())
    {
        strMessage = _T("The image set title should not be empty!\n");
        bGenerateWarning = TRUE;
    }

    if(m_strImageSetName.IsEmpty())
    {
        strMessage += _T("The image set name should not be empty!");
        bGenerateWarning = TRUE;
    }
    
    if(bGenerateWarning)
    {
        ::MessageBox(NULL, strMessage, 
                           "MyScreenSaver Builder 2.2", MB_OK | MB_ICONWARNING); 
        return FALSE;
    }

    return TRUE;
}

BOOL CMySaverPropPage2::OnWizardFinish()
{
    CString m_strBuildDir;
    CString strMSSFileName;
    TCHAR szFileSource[100];
    TCHAR szFileDest[100];
    CFileFind ff;
    CString strCommandLine;
    CString strDirectory;
    CString strFileName;
    CString strFullFileNameDest;
    CString strFullFileNameSource;
    HINSTANCE hResInst;
    CString strMSSBuildFullFileName;
    CString strMsg;

    m_strBuildDir = theApp.m_strMSBDataDirectory + _T("\\Build"); 

    //Update class data from dialog UI data.
    UpdateData(TRUE);

    // Save final page data to registry.
    CMySaverApp* pMySS =  static_cast<CMySaverApp*>(AfxGetApp());
    pMySS->SaveFinalPageData(0 /* CheckBoxPreview is not used since v2.0 */, 
                                         m_bCheckBoxCreateSetup, m_strSetupDir);
    
    if (m_nBuildOption == 0 /* new MSS mode */)
    {
        // Validate before building
        if (IsValid())
        {
            MakeRCFile();
            MakeSetupRCFile();

            if (!m_bCheckBoxCreateSetup /* create only MSS */) 
            {
                RunConsoleApp(FALSE /* create only MSS */);

                // If Images directory exists do nothing, if not - create the 
                // directory.
                CreateDirectory(m_strInstallDir + "\\Images", 
                                                           /* security */ NULL);

                strMSSFileName = m_strImageSetName + ".mss";

                SetCurrentDirectory(theApp.m_strMSBDataDirectory); 
                //CopyFile(_T("Build\\") + strMSSFileName, _T("Images\\") + 
                //	                                     strMSSFileName, FALSE);
                MoveFileEx(_T("Build\\") + strMSSFileName, 
                    theApp.m_strMSBDataDirectory + _T("\\Images\\") + 
                    strMSSFileName, MOVEFILE_REPLACE_EXISTING | 
                    MOVEFILE_WRITE_THROUGH | MOVEFILE_COPY_ALLOWED 
                                              /* allow copy to another disk */);
                wsprintf(szFileDest, "%s%s%s", theApp.m_strMSBDataDirectory, 
                                              _T("\\Images\\"), strMSSFileName);
                                                                 
                if (ff.FindFile(szFileDest))
                {
                    if (IDYES == ::MessageBox(NULL, 
                       "The MSS file created successfully. Open images folder?", 
                       "MyScreenSaver Builder 2.2", MB_YESNO | MB_ICONQUESTION))
                    {
                        strCommandLine = _T("explorer.exe ") + 
                                  theApp.m_strMSBDataDirectory + _T("\\Images");
                        WinExec(strCommandLine, TRUE);
                    }
                }
                else
                {
                    ::MessageBox(NULL, "MSS build failure ...", 
                           "MyScreenSaver Builder 2.2", MB_OK | MB_ICONWARNING);
                }
                ff.Close();
            }
            else // create only MSS and setup program
            {
                RunConsoleApp(TRUE /* create MSS and setup program */);

                CreateDirectory(m_strSetupDir, NULL);
            
                   m_strBuildDir = theApp.m_strMSBDataDirectory + _T("\\Build"); 
                SetCurrentDirectory(m_strBuildDir);

                // Remove MSS file 
                strMSSFileName= m_strImageSetName + _T(".mss");
                //DeleteFile(strMSSFileName);

                // Move setup program to special directory.
                wsprintf(szFileSource, "%s%s", m_strSetupName, _T(".exe"));
                wsprintf(szFileDest, "%s%s%s%s", m_strSetupDir, _T("\\"), 
                                                    m_strSetupName, _T(".exe"));
                // Do not return until moving is ended (MOVEFILE_WRITE_THROUGH).
                MoveFileEx(szFileSource, szFileDest, MOVEFILE_WRITE_THROUGH | 
                        MOVEFILE_COPY_ALLOWED /* allow copy to another disk */);

                // Delete Build directory before opening WE, which LL puts a 
                // lock on this directory. This deletion goes before deletion at 
                // the end of OnWizardFinish (in this case no harm do delete 
                // nothing).
                if (!theApp.m_bEnableDebug) 
                {
                    DeleteDirectory(m_strBuildDir);
                }

                if (ff.FindFile(szFileDest))
                {
                    if (IDYES == ::MessageBox(NULL, 
                        "Setup program built successfully. Open setups folder?", 
                       "MyScreenSaver Builder 2.2", MB_YESNO | MB_ICONQUESTION))
                    {
                        strCommandLine = _T("explorer.exe ") + m_strSetupDir;
                                                                    
                        WinExec(strCommandLine, TRUE);
                    }
                }
                else
                {
                    ::MessageBox(NULL, "Setup build failure ...", 
                           "MyScreenSaver Builder 2.2", MB_OK | MB_ICONWARNING);
                }
                ff.Close();
            }
        }
        else
        {
            return -1; // stay on the same page
        }
    } // end of build option
    else if (m_nBuildOption == 1 /* edit MSS mode */)
    {
        int iPageCount; 
        iPageCount = ((CMySaverPropSheet*) GetParent())->GetPageCount();
        CMySaverPropPage1* pPP1 = (CMySaverPropPage1*)((CMySaverPropSheet*) 
                         GetParent())->GetPage(iPageCount - 2 /* zero-based */);

        // Replace source MSS file with updated MSS file if images were deleted
        // in the previous page or re-ordered. Notice that in all other cases
        // (title, file name update) source MSS file is updated directly.

        // Use ... == TRUE since when PP1 is skipped (m_nEditOption = 1), the
        // m_bIsImagesDeletionMarked and m_bIsImagesReordered can be just big
        // integer values.
        if (pPP1->m_bIsImagesDeletionMarked == TRUE || 
                                             pPP1->m_bIsImagesReordered == TRUE)
        {
            strMSSFileName = m_strImageSetName + ".mss";
            strMSSBuildFullFileName = theApp.m_strMSBDataDirectory + "\\Build\\" 
                                                               + strMSSFileName;
            // Replace source with updated MSS (from temporary Build directory)
            // only if there is no error in updated MSS. The weak check is to
            // test if it is loadable.
            hResInst = LoadLibrary(strMSSBuildFullFileName); 
            if (hResInst != NULL)
            {
                // LL MoveFile does not write over existing file, so I use 
                // MoveFileEx.
                MoveFileEx(strMSSBuildFullFileName, m_strInitImages, 
                    MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH | 
                        MOVEFILE_COPY_ALLOWED /* allow copy to another disk */);
            }
            else
            {
                ::MessageBox(NULL, 
                    "The MSS rebuild error!", 
                                   "MyScreenSaver Builder 2.2", MB_ICONWARNING);
            }
        }
    
        // Modify title resource in MSS when MSS title if changed manually.
        if (m_bTitleDirty)
        {
            pPP1->UpdateStringResource(1 /* IDS_TITLE */, 
                                  m_strImageSetTitle /* new image set title */);
        }

        // Rename MSS file name if changed manually.
        if (m_bFileNameDirty)
        {
            // Construct destination full file name by getting, at first, the 
            // directory where MSS resides, and then adding updated file name.
            // Note that MSS file is updated in place i.e. no change of 
            // directory takes place.
            strDirectory = pMySS->
                      Utility_GetDirectoryNameFromFullFileName(m_strInitImages);
            strFullFileNameDest = strDirectory + m_strImageSetName + ".mss";
            // Rename file (MoveFile, in fact, renames also). LL the flags 
            // MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH are not needed
            // for MoveFile is used here for renaming only.
            MoveFile(m_strInitImages, strFullFileNameDest);
        }

        // If no changes were made to title or file name prompt message only if 
        // setup program is created so that there is only one final message box.
        if  (!m_bCheckBoxCreateSetup) 
        {
            // if (m_bTitleDirty || m_bFileNameDirty)
            if (m_bTitleDirty || m_bFileNameDirty || m_bOptionalSettingsDirty)
            {
                // TODO: find a criteria that MSS file updated OK, and not corrupted
                // during update. Now I take it for granted (false) and open
                // folder always.
                if (IDYES == ::MessageBox(NULL, 
                    "The MSS file is updated successfully. Open its folder?", 
                    "MyScreenSaver Builder 2.2", MB_YESNO | MB_ICONQUESTION))
                {
                    // Get directory where MSS resides.
                    strDirectory = pMySS->
                      Utility_GetDirectoryNameFromFullFileName(m_strInitImages);
                                                            
                    strCommandLine = _T("explorer.exe ") + strDirectory;

                    WinExec(strCommandLine, TRUE);
                }
            }
            else
            {
                // I use pPP1-> ... == TRUE since when PP1 is skipped 
                // (m_nEditOption = 1), the m_bIsImagesDeletionMarked and 
                // m_bIsImagesReordered can be just big integer values.    
                if (pPP1->m_bIsImagesDeletionMarked == TRUE || 
                                             pPP1->m_bIsImagesReordered == TRUE)
                {
                    // Delete Build directory before opening WE, which LL puts a 
                    // lock on this directory. This deletion goes before deletion at 
                    // the end of OnWizardFinish (in this case no harm do delete 
                    // nothing).
                    if (!theApp.m_bEnableDebug) 
                    {
                        DeleteDirectory(m_strBuildDir);
                    }			

                    if (IDYES == ::MessageBox(NULL, 
                    "The MSS file is updated successfully. Open its folder?", 
                    "MyScreenSaver Builder 2.2", MB_YESNO | MB_ICONQUESTION))
                    {
                        // Get directory where MSS resides.
                        strDirectory = pMySS->
                        Utility_GetDirectoryNameFromFullFileName(m_strInitImages);
                                                                
                        strCommandLine = _T("explorer.exe ") + strDirectory;

                        WinExec(strCommandLine, TRUE);
                    }
                }
                else
                {
                    if (theApp.m_bImagesManuallyAdded)
                    {
                        strMsg = "The image(s) are added successfully. ";
                        strMsg += "Open containing folder?";
                        if (IDYES == ::MessageBox(NULL, strMsg, 
                            "MyScreenSaver Builder 2.2", 
                                                    MB_YESNO | MB_ICONQUESTION))
                        {
                            // Get directory where MSS resides.
                            strDirectory = pMySS->
                            Utility_GetDirectoryNameFromFullFileName(
                                                               m_strInitImages);
                            strCommandLine = _T("explorer.exe ") + strDirectory;

                            WinExec(strCommandLine, TRUE);
                        }
                    }
                    else
                    {
                        strMsg = "No changes were made. The MSS file is not ";
                        strMsg += "updated.";
                        ::MessageBox(NULL, strMsg, "MyScreenSaver Builder 2.2", 
                                                    MB_OK | MB_ICONINFORMATION);
                    }
                }
            }
        }
        else if (m_bCheckBoxCreateSetup) 
        {
            GetDlgItem(IDC_PP2_PROGRESSBAR)->ShowWindow(TRUE);
           
            // Copy MSS file from source to destination (temporary build 
            // location).
            strFullFileNameSource = m_strInitImages;
            strFullFileNameDest = m_strBuildDir + "\\" + 
                                                     m_strImageSetName + ".mss";
            CopyFile(strFullFileNameSource, strFullFileNameDest, FALSE);

            // Make selfextractor.rc and build setup EXE.
            MakeSetupRCFile();
            EditMode_RunConsoleApp();

            // Move setup program to special directory.
            SetCurrentDirectory(m_strBuildDir);
            wsprintf(szFileSource, "%s%s", m_strSetupName, _T(".exe"));
            wsprintf(szFileDest, "%s%s%s%s", m_strSetupDir, _T("\\"), 
                                                    m_strSetupName, _T(".exe"));
            // Return only when finished with moving (MOVEFILE_WRITE_THROUGH).
            MoveFileEx(szFileSource, szFileDest, MOVEFILE_WRITE_THROUGH | 
                        MOVEFILE_COPY_ALLOWED /* allow copy to another disk */);

            // Delete Build directory before opening WE, which LL puts a 
            // lock on this directory. This deletion goes before deletion at 
            // the end of OnWizardFinish (in this case no harm do delete 
            // nothing).
            if (!theApp.m_bEnableDebug) 
            {
                DeleteDirectory(m_strBuildDir);
            }

            if (ff.FindFile(szFileDest))
            {
                if (IDYES == ::MessageBox(NULL, 
                    "Setup program built successfully. Open setups folder?", 
                    "MyScreenSaver Builder 2.2", MB_YESNO | MB_ICONQUESTION))
                {
                    strCommandLine = _T("explorer.exe ") + m_strSetupDir;
                                                                
                    WinExec(strCommandLine, TRUE);
                }
            }
            else
            {
                ::MessageBox(NULL, "Setup build failure ...", 
                           "MyScreenSaver Builder 2.2", MB_OK | MB_ICONWARNING);
            }
            ff.Close();

            GetDlgItem(IDC_PP2_PROGRESSBAR)->ShowWindow(FALSE);
        }
    }

    // Delete build directory, finally.
    if (!theApp.m_bEnableDebug) 
    {
        // Supposedly, there are no subfolders in the Build directory 
        // (DeleteDirectory does not use recursion!). DeleteDirectory uses
        // RemoveDirectory, which requires that the folder to be deleted must be 
        // empty the call. Therefore, DeleteDirectory enumerates the folder's 
        // files and removes them one-by-one.
        DeleteDirectory(m_strBuildDir);
    }

    return TRUE;
}

// When images were deleted or re-ordered in PP1, EditMode_PrebuildMSS() rebuilds
// MSS (still free of images, but with all other data) based on source MSS. Then
// Edit_AddImagesToMSS inserts images stored in RAM from source MSS.
// TODO: based on RunConsoleApp(...), merge probably.
void CMySaverPropPage2::EditMode_PrebuildMSS()
{
    HANDLE hDurationLog;
    TCHAR szLog[80];
    DWORD dwStart, dwFinish, dwWritten;
    DWORD dwStep1_1_Start, dwStep1_1_Finish;
    DWORD dwStep1_2_Start, dwStep1_2_Finish;

    CString m_strBuildDir = theApp.m_strMSBDataDirectory + _T("\\Build"); 
    SetCurrentDirectory(m_strBuildDir);

    //DeleteFile("Duration_Log.txt");
    hDurationLog = CreateFile("Duration_Log.txt", 
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);
    SetFilePointer(hDurationLog, 0, NULL, FILE_END);

    dwStart = GetTickCount();

    // -- setup pipe that receive child process output --
    // Create the child output pipe,set up the security attributes struct used for 
    // pipe.
    HANDLE              hPipeOutputRead = NULL;
    HANDLE              hPipeOutputWrite = NULL;


    // MSDN'98: "In order for the child process to be able to write to the 
    // anonymous pipe, the handle must be marked as inheritable by child 
    // processes by setting the SECURITY_ATTRIBUTES.bInheritHandle flag to 
    // TRUE." 
    SECURITY_ATTRIBUTES saPipe;
    saPipe.nLength= sizeof(SECURITY_ATTRIBUTES);
    saPipe.lpSecurityDescriptor = NULL;
    saPipe.bInheritHandle = TRUE;	

    // Step 1.1.
      CreatePipe(&hPipeOutputRead, &hPipeOutputWrite, &saPipe, 
                                          0 /* use default pipe buffer size */);

    // -- setup process --
    // Set up the start up info struct and attach the write end of the anonymous 
    // pipe to hStdOutput and hStdError (MSDN'98: CPICPORT.C or PROCSRV.C)
    STARTUPINFO si;
    ZeroMemory(&si,sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = NULL;
    si.hStdOutput = hPipeOutputWrite;  
    si.hStdError = hPipeOutputWrite;  // use the same pipe

    PROCESS_INFORMATION pi = {0};

    //CreateProcess(NULL, "rc /v /fo\"myss.res\" myss.rc", NULL, NULL, 
    CreateProcess(NULL, "dev0.exe /v /fo\"myss.res\" myss.rc", NULL, NULL, 
                      TRUE /* inherit open handles */, 0, NULL, NULL, &si, &pi);

    // MSDN'98: "We need to close our instance of the inherited pipe write 
    // handle now that it's been inherited so that it will actually close when 
    // the child process ends. This will put an EOF condition on the pipe which 
    // we can then detect." 
    CloseHandle(hPipeOutputWrite);

    m_hChildProcess_1_1 = pi.hProcess;

    // Read the child's output.
    ReadAndHandleOutput_Step_1_1(hPipeOutputRead);

    dwStep1_1_Finish = GetTickCount();
    sprintf(szLog, "Step_1_1 End: %d\r\n", dwStep1_1_Finish - dwStart); 
    WriteFile(hDurationLog, szLog, strlen(szLog), &dwWritten, NULL);

    m_ctlProgressBar.SetPos(30);
                                                              
    // Step 1.2.
      CreatePipe(&hPipeOutputRead, &hPipeOutputWrite, &saPipe, 
                                          0 /* use default pipe buffer size */);

    // -- setup process --
    // Set up the start up info struct and attach the write end of the anonymous 
    // pipe to hStdOutput and hStdError (MSDN'98: CPICPORT.C or PROCSRV.C)
    ZeroMemory(&si,sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = NULL;
    si.hStdOutput = hPipeOutputWrite;  
    si.hStdError = hPipeOutputWrite;  // use the same pipe

    CString strBuildString; 
    strBuildString = "dev1.exe /OUT:\"" + m_strImageSetName + ".mss\" ";
    strBuildString +="/DLL /NOENTRY /NOLOGO /SUBSYSTEM:WINDOWS /INCREMENTAL:NO /NODEFAULTLIB /VERBOSE ";
    strBuildString +="ImageSet.obj myss.res";
    // LockBuffer() on CString returns LPTSTR.
    CreateProcess(NULL, strBuildString.LockBuffer(), NULL, NULL,
    //CreateProcess(NULL, "Link.EXE @MSS_Link_Args.txt", NULL, NULL, 
                      TRUE /* inherit open handles */, 0, NULL, NULL, &si, &pi);
    //--
                                                        
    // MSDN'98: "We need to close our instance of the inherited pipe write 
    // handle now that it's been inherited so that it will actually close when 
    // the child process ends. This will put an EOF condition on the pipe which 
    // we can then detect." 
    CloseHandle(hPipeOutputWrite);

    m_hChildProcess_1_2 = pi.hProcess;

    // Read the child's output.
    ReadAndHandleOutput_Step_1_2(hPipeOutputRead);
    
    dwStep1_2_Finish = GetTickCount();
    sprintf(szLog, "Step_1_2 End: %d\r\n", dwStep1_2_Finish - dwStart); 
    WriteFile(hDurationLog, szLog, strlen(szLog), &dwWritten, NULL);

    m_ctlProgressBar.SetPos(100);

    // Helps to delete Duration_Log.txt file in the Build directory.
    CloseHandle(hDurationLog);
}

// EditMode_RunConsoleApp() creates setup program in edit mode.
// TODO: based on RunConsoleApp(...), merge probably.
void CMySaverPropPage2::EditMode_RunConsoleApp()
{
    HANDLE hDurationLog;
    TCHAR szLog[80];
    DWORD dwStart, dwFinish, dwWritten;
    DWORD dwStep2_1_Start, dwStep2_1_Finish;
    DWORD dwStep2_2_Start, dwStep2_2_Finish;
    CString strBuildString;

    CString m_strBuildDir = theApp.m_strMSBDataDirectory + _T("\\Build"); 
    SetCurrentDirectory(m_strBuildDir);

    //DeleteFile("Duration_Log.txt");
    hDurationLog = CreateFile("Duration_Log.txt", 
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);
    SetFilePointer(hDurationLog, 0, NULL, FILE_END);

    dwStart = GetTickCount();

    // -- setup pipe that receive child process output --
    // Create the child output pipe,set up the security attributes struct used for 
    // pipe.
    HANDLE              hPipeOutputRead = NULL;
    HANDLE              hPipeOutputWrite = NULL;


    // MSDN'98: "In order for the child process to be able to write to the 
    // anonymous pipe, the handle must be marked as inheritable by child 
    // processes by setting the SECURITY_ATTRIBUTES.bInheritHandle flag to 
    // TRUE." 
    SECURITY_ATTRIBUTES saPipe;
    saPipe.nLength= sizeof(SECURITY_ATTRIBUTES);
    saPipe.lpSecurityDescriptor = NULL;
    saPipe.bInheritHandle = TRUE;	
                                                              
    // Step 2.1.
    CreatePipe(&hPipeOutputRead, &hPipeOutputWrite, &saPipe, 
                                          0 /* use default pipe buffer size */);

    // -- setup process --
    // Set up the start up info struct and attach the write end of the anonymous 
    // pipe to hStdOutput and hStdError (MSDN'98: CPICPORT.C or PROCSRV.C)
    STARTUPINFO si;
    ZeroMemory(&si,sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = NULL;
    si.hStdOutput = hPipeOutputWrite;  
    si.hStdError = hPipeOutputWrite;  // use the same pipe

    PROCESS_INFORMATION pi = {0};
    CreateProcess(NULL, "dev0.exe /v /fo\"selfextractor_win32.res\" selfextractor.rc",
    //CreateProcess(NULL, "rc /v /fo\"selfextractor_win32.res\" selfextractor.rc", 
           NULL, NULL, TRUE /* inherit open handles */, 0, NULL, NULL, &si, &pi);

    // MSDN'98: "We need to close our instance of the inherited pipe write 
    // handle now that it's been inherited so that it will actually close when 
    // the child process ends. This will put an EOF condition on the pipe which 
    // we can then detect." 
    CloseHandle(hPipeOutputWrite);

    m_hChildProcess_2_1 = pi.hProcess;

    // Read the child's output.
    ReadAndHandleOutput_Step_2_1(hPipeOutputRead);

    dwStep2_1_Finish = GetTickCount();
    sprintf(szLog, "Step_2_1 End: %d\r\n", dwStep2_1_Finish - dwStart); 
    WriteFile(hDurationLog, szLog, strlen(szLog), &dwWritten, NULL);
    m_ctlProgressBar.SetPos(20);

    // Step 2.2.
    //-- if pipe is not re-created the - no output to TXT file (albeit 
    // setup.exe is created).
      CreatePipe(&hPipeOutputRead, &hPipeOutputWrite, &saPipe, 
                                          0 /* use default pipe buffer size */);

    // -- setup process --
    // Set up the start up info struct and attach the write end of the anonymous 
    // pipe to hStdOutput and hStdError (MSDN'98: CPICPORT.C or PROCSRV.C)
    ZeroMemory(&si,sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = NULL;
    si.hStdOutput = hPipeOutputWrite;  
    si.hStdError = hPipeOutputWrite;  // use the same pipe

    strBuildString = "dev1.exe /OUT:\"" + m_strSetupName + ".exe\" ";
    strBuildString += "/NOLOGO /SUBSYSTEM:WINDOWS /INCREMENTAL:NO /MACHINE:I386 /VERBOSE  ";
    strBuildString += "kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib libc.lib oldnames.lib "; // or kernel32.lib user32.lib 
    strBuildString += "SelfExtractor_Win32.obj stdafx_se.obj SelfExtractor_Win32.res";
    CreateProcess(NULL, strBuildString.LockBuffer(), NULL, NULL, 
    //CreateProcess(NULL, "Link.EXE @Link_Args.txt", NULL, NULL, 
                      TRUE /* inherit open handles */, 0, NULL, NULL, &si, &pi);
    //--
                                                        
    // MSDN'98: "We need to close our instance of the inherited pipe write 
    // handle now that it's been inherited so that it will actually close when 
    // the child process ends. This will put an EOF condition on the pipe which 
    // we can then detect." 
    CloseHandle(hPipeOutputWrite);

    m_hChildProcess_2_2 = pi.hProcess;

    // Read the child's output.
    ReadAndHandleOutput_Step_2_2(hPipeOutputRead);

    dwStep2_2_Finish = GetTickCount();
    sprintf(szLog, "Step_2_2 End: %d\r\n", dwStep2_2_Finish - dwStart); 
    WriteFile(hDurationLog, szLog, strlen(szLog), &dwWritten, NULL);
    // Helps to delete Duration_Log.txt file in the Build directory.
    CloseHandle(hDurationLog);

    m_ctlProgressBar.SetPos(100);
}


BOOL CMySaverPropPage2::PreTranslateMessage(MSG* pMsg)
{
    m_ctlToolTip.Activate(TRUE);
    m_ctlToolTip.RelayEvent(pMsg);
    
    return CPropertyPage::PreTranslateMessage(pMsg);
}

void CMySaverPropPage2::ShowToolTips()
{
    // Provide tooltips
    m_ctlToolTip.Create(this, TTS_ALWAYSTIP);
    m_ctlToolTip.SetMaxTipWidth(200);

    CString strToolTip1 = _T("The title of image set is a human readable name used to select an image set within the program interface.");
    m_ctlToolTip.AddTool(GetDlgItem(IDC_PP2_EDIT_TITLE), strToolTip1); 

    CString strToolTip2 = _T("This is a name of image set in the form <name>.mss.");
    m_ctlToolTip.AddTool(GetDlgItem(IDC_PP2_EDIT_NAME), strToolTip2); 

    // Better to store in string table, but tooltip shows only 2 upper lines ...
    CString strToolTip3 = _T("Optionally, provide a description of image set file, which you can see later under the its version tab (when you see MSS properties).");
    
    //Better to show for static text but this shows for every static ...
    //CRect rc(31, 78, 45, 29);  
    //m_ctlToolTip.AddTool(this, strText, rc); //for area

    m_ctlToolTip.AddTool(GetDlgItem(IDC_PP2_EDIT3), strToolTip3); //for control

    m_ctlToolTip.Activate(TRUE);
}

void CMySaverPropPage2::StartUnpackFilesThread()
{
    AfxBeginThread(ThreadProc, this->m_hWnd);
}

UINT CMySaverPropPage2::ThreadProc(LPVOID param)
{
    theApp.m_hBuildFolderCreatedEvent = CreateEvent(NULL, 
                      TRUE /* manual reset */, FALSE /* none-signaled */, NULL);
    
    // Call to MSB_devices.dll function to extract build support files.
    ExtractFiles();

    // Signals the caller that Build folder is created is all files extracted 
    // there.
    SetEvent(theApp.m_hBuildFolderCreatedEvent);

    return 0;
}

// This function should also work when aimages are reordered in PP1.
BOOL CMySaverPropPage2::
               EditMode_AddImagesToMSS(LPCTSTR pszFileName /* full path name */)
{
    int nOldSize;
    int nNewSize;
    int iPageCount;
    CMySaverPropPage1* pPP1 = NULL;
    int iImageID;
    HINSTANCE hResInst;
    HRSRC hRsrc;
    TCHAR szName[10];
    CString strMSSFile;
    HANDLE hUpdateRes;
    BOOL bResult;

    iPageCount= ((CMySaverPropSheet*) GetParent())->GetPageCount();
    pPP1 = (CMySaverPropPage1*)((CMySaverPropSheet*) 
                                          GetParent())->GetPage(iPageCount - 2); 

    // Get the current (old) and new number of images in MSS.
    nOldSize = pPP1->m_vecImages.size();
    nNewSize = EditMode_GetImageCountWithoutDeletionMarked();  

    // The dynamic arrays are essentially the same as BYTE* lpRsrc[7] and
    // DWORD len[7] (when length is 7). 
    BYTE** lpRsrc = new BYTE*[nNewSize];
    DWORD* len = new DWORD[nNewSize];

    // Load images into dynamic array.
    hResInst = LoadLibrary(pszFileName); 

    if (hResInst == NULL)
    {
        AfxMessageBox("LoadLibrary failed!");
        return FALSE;
    }

    int iCount = 0;
    for (int i = 0; i < nOldSize; i++)
    {
        // Makes new array of images ignore images marked for deletion.
        if (pPP1->m_vecImages.at(i).bDeletionMarked != TRUE)
        {
            // Images 101 ... 105 ...
            if (!pPP1->m_bIsImagesReordered)
            {
                iImageID = 101 + i;
            }
            else
            {
                iImageID = 101 + pPP1->m_vecImages.at(i).iPermanentIndex;
            }
            sprintf(szName, "#%d", iImageID);
            hRsrc = FindResource(hResInst, szName /* e.g. "#103" */, "IMAGES"); 
            if (hRsrc == NULL)
            {
                AfxMessageBox("FindResource failed!");
                return FALSE;
            }

            // Load resource into memory.
            lpRsrc[iCount] = (BYTE*)LoadResource(hResInst, hRsrc);
            if (lpRsrc == NULL)
            {
                AfxMessageBox("LoadResource failed!");
                return FALSE;
            }

            // Create memory file and load it.
            len[iCount] = SizeofResource(hResInst,  hRsrc);

            // Increment at the each iteration's END.
            iCount++;
        }
    }

    // Insert images into MSS (new MSS yet has no image resources, and must be 
    // available).
    iCount = 0;
    for (int i = 0; i < nNewSize; i++)
    {
        iCount++;
        //strMSSFile = m_strInstallDir + _T("\\Build\\") + m_strImageSetName;
        strMSSFile = theApp.m_strMSBDataDirectory + _T("\\Build\\") + 
                                                 m_strImageSetName + _T(".mss");
        hUpdateRes = BeginUpdateResource(strMSSFile, 
                                  FALSE /* do not delete existing resources */); 
         
        if (hUpdateRes == NULL) 
        {
            AfxMessageBox("BeginUpdateResource failed!");
        }

        // Add the image resource to the update list. MSDN: "Each call to 
        // UpdateResource contributes to an internal list of additions, deletions, 
        // and replacements but does not actually write the data to the executable 
        // file." 
        bResult = UpdateResource(hUpdateRes, "IMAGES", 
            MAKEINTRESOURCE(101 + i), 
            MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), 
                         lpRsrc[i] /* pvData, data */, len[i] /* bytes read */);  
                                                                
        if (!bResult) 
        { 
            AfxMessageBox("UpdateResource failed!");
        } 

        // Write changes to MSS and then close it. 
        if (!EndUpdateResource(hUpdateRes, FALSE /* make update */)) 
        { 
            AfxMessageBox("EndUpdateResource: Could not write changes to file.!");
        }
    }

    // Free source library AFTER images updated in MSS (handles to images point 
    // to source MSS).
    FreeLibrary(hResInst);
    delete [] lpRsrc;
    delete [] len;

    return TRUE;
}

int CMySaverPropPage2::EditMode_GetImageCountWithoutDeletionMarked()
{
    int iPageCount;
    CMySaverPropPage1* pPP1 = NULL;

    iPageCount= ((CMySaverPropSheet*) GetParent())->GetPageCount();
    pPP1 = (CMySaverPropPage1*)((CMySaverPropSheet*) 
                                          GetParent())->GetPage(iPageCount - 2); 

    // Get the new number of images in MSS.
    int iImageCount = 0;
    for (int i = 0; i < pPP1->m_vecImages.size(); i++)
    {
        if (pPP1->m_vecImages.at(i).bDeletionMarked == FALSE)
        {
            iImageCount ++;
        }
    }

    return iImageCount; 
}

// RunConsoleApp() and internal functions are from: 
// ..\MyScreenSaver Builder.NET\About\MFC_Console_App_Output.
//void CMySaverPropPage2::RunConsoleApp()
void CMySaverPropPage2::RunConsoleApp(BOOL bCreateSetupProgram)
{
    HANDLE hDurationLog;
    TCHAR szLog[80];
    DWORD dwStart, dwFinish, dwWritten;
    DWORD dwStep1_1_Start, dwStep1_1_Finish;
    DWORD dwStep1_2_Start, dwStep1_2_Finish;
    DWORD dwStep2_1_Start, dwStep2_1_Finish;
    DWORD dwStep2_2_Start, dwStep2_2_Finish;

    CString m_strBuildDir = theApp.m_strMSBDataDirectory + _T("\\Build"); 
    SetCurrentDirectory(m_strBuildDir);

    //DeleteFile("Duration_Log.txt");
    hDurationLog = CreateFile("Duration_Log.txt", 
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);
    SetFilePointer(hDurationLog, 0, NULL, FILE_END);

    dwStart = GetTickCount();

    // -- setup pipe that receive child process output --
    // Create the child output pipe,set up the security attributes struct used for 
    // pipe.
    HANDLE              hPipeOutputRead = NULL;
    HANDLE              hPipeOutputWrite = NULL;


    // MSDN'98: "In order for the child process to be able to write to the 
    // anonymous pipe, the handle must be marked as inheritable by child 
    // processes by setting the SECURITY_ATTRIBUTES.bInheritHandle flag to 
    // TRUE." 
    SECURITY_ATTRIBUTES saPipe;
    saPipe.nLength= sizeof(SECURITY_ATTRIBUTES);
    saPipe.lpSecurityDescriptor = NULL;
    saPipe.bInheritHandle = TRUE;	

    // Step 1.1.
      CreatePipe(&hPipeOutputRead, &hPipeOutputWrite, &saPipe, 
                                          0 /* use default pipe buffer size */);

    // -- setup process --
    // Set up the start up info struct and attach the write end of the anonymous 
    // pipe to hStdOutput and hStdError (MSDN'98: CPICPORT.C or PROCSRV.C)
    STARTUPINFO si;
    ZeroMemory(&si,sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = NULL;
    si.hStdOutput = hPipeOutputWrite;  
    si.hStdError = hPipeOutputWrite;  // use the same pipe

    PROCESS_INFORMATION pi = {0};

    //CreateProcess(NULL, "rc /v /fo\"myss.res\" myss.rc", NULL, NULL, 
    CreateProcess(NULL, "dev0.exe /v /fo\"myss.res\" myss.rc", NULL, NULL, 
                      TRUE /* inherit open handles */, 0, NULL, NULL, &si, &pi);

    // MSDN'98: "We need to close our instance of the inherited pipe write 
    // handle now that it's been inherited so that it will actually close when 
    // the child process ends. This will put an EOF condition on the pipe which 
    // we can then detect." 
    CloseHandle(hPipeOutputWrite);

    m_hChildProcess_1_1 = pi.hProcess;

    // Read the child's output.
    ReadAndHandleOutput_Step_1_1(hPipeOutputRead);

    dwStep1_1_Finish = GetTickCount();
    sprintf(szLog, "Step_1_1 End: %d\r\n", dwStep1_1_Finish - dwStart); 
    WriteFile(hDurationLog, szLog, strlen(szLog), &dwWritten, NULL);
    if (!bCreateSetupProgram)
    {
        m_ctlProgressBar.SetPos(30);
    }
    else
    {
        m_ctlProgressBar.SetPos(5);
    }
                                                              
    // Step 1.2.
      CreatePipe(&hPipeOutputRead, &hPipeOutputWrite, &saPipe, 
                                          0 /* use default pipe buffer size */);

    // -- setup process --
    // Set up the start up info struct and attach the write end of the anonymous 
    // pipe to hStdOutput and hStdError (MSDN'98: CPICPORT.C or PROCSRV.C)
    ZeroMemory(&si,sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = NULL;
    si.hStdOutput = hPipeOutputWrite;  
    si.hStdError = hPipeOutputWrite;  // use the same pipe

    CString strBuildString; 
    strBuildString = "dev1.exe /OUT:\"" + m_strImageSetName + ".mss\" ";
    strBuildString +="/DLL /NOENTRY /NOLOGO /SUBSYSTEM:WINDOWS /INCREMENTAL:NO /NODEFAULTLIB /VERBOSE ";
    strBuildString +="ImageSet.obj myss.res";
    // LockBuffer() on CString returns LPTSTR.
    CreateProcess(NULL, strBuildString.LockBuffer(), NULL, NULL,
    //CreateProcess(NULL, "Link.EXE @MSS_Link_Args.txt", NULL, NULL, 
                      TRUE /* inherit open handles */, 0, NULL, NULL, &si, &pi);
    //--
                                                        
    // MSDN'98: "We need to close our instance of the inherited pipe write 
    // handle now that it's been inherited so that it will actually close when 
    // the child process ends. This will put an EOF condition on the pipe which 
    // we can then detect." 
    CloseHandle(hPipeOutputWrite);

    m_hChildProcess_1_2 = pi.hProcess;

    // Read the child's output.
    ReadAndHandleOutput_Step_1_2(hPipeOutputRead);
    
    dwStep1_2_Finish = GetTickCount();
    sprintf(szLog, "Step_1_2 End: %d\r\n", dwStep1_2_Finish - dwStart); 
    WriteFile(hDurationLog, szLog, strlen(szLog), &dwWritten, NULL);
    if (!bCreateSetupProgram)
    {
        m_ctlProgressBar.SetPos(100);
    }
    else
    {
        m_ctlProgressBar.SetPos(15);
    }
    
    if (bCreateSetupProgram)
    {
    // Step 2.1.
    CreatePipe(&hPipeOutputRead, &hPipeOutputWrite, &saPipe, 
                                          0 /* use default pipe buffer size */);

    // -- setup process --
    // Set up the start up info struct and attach the write end of the anonymous 
    // pipe to hStdOutput and hStdError (MSDN'98: CPICPORT.C or PROCSRV.C)
    //STARTUPINFO si;
    ZeroMemory(&si,sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = NULL;
    si.hStdOutput = hPipeOutputWrite;  
    si.hStdError = hPipeOutputWrite;  // use the same pipe

    //PROCESS_INFORMATION pi = {0};
    CreateProcess(NULL, "dev0.exe /v /fo\"selfextractor_win32.res\" selfextractor.rc",
    //CreateProcess(NULL, "rc /v /fo\"selfextractor_win32.res\" selfextractor.rc", 
           NULL, NULL, TRUE /* inherit open handles */, 0, NULL, NULL, &si, &pi);

    // MSDN'98: "We need to close our instance of the inherited pipe write 
    // handle now that it's been inherited so that it will actually close when 
    // the child process ends. This will put an EOF condition on the pipe which 
    // we can then detect." 
    CloseHandle(hPipeOutputWrite);

    m_hChildProcess_2_1 = pi.hProcess;

    // Read the child's output.
    ReadAndHandleOutput_Step_2_1(hPipeOutputRead);

    dwStep2_1_Finish = GetTickCount();
    sprintf(szLog, "Step_2_1 End: %d\r\n", dwStep2_1_Finish - dwStart); 
    WriteFile(hDurationLog, szLog, strlen(szLog), &dwWritten, NULL);
    m_ctlProgressBar.SetPos(20);

    // Step 2.2.
    //-- if pipe is not re-created the - no output to TXT file (albeit 
    // setup.exe is created).
      CreatePipe(&hPipeOutputRead, &hPipeOutputWrite, &saPipe, 
                                          0 /* use default pipe buffer size */);

    // -- setup process --
    // Set up the start up info struct and attach the write end of the anonymous 
    // pipe to hStdOutput and hStdError (MSDN'98: CPICPORT.C or PROCSRV.C)
    ZeroMemory(&si,sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = NULL;
    si.hStdOutput = hPipeOutputWrite;  
    si.hStdError = hPipeOutputWrite;  // use the same pipe

    strBuildString = "dev1.exe /OUT:\"" + m_strSetupName + ".exe\" ";
    strBuildString += "/NOLOGO /SUBSYSTEM:WINDOWS /INCREMENTAL:NO /MACHINE:I386 /VERBOSE  ";
    strBuildString += "kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib libc.lib oldnames.lib "; // or kernel32.lib user32.lib 
    strBuildString += "SelfExtractor_Win32.obj stdafx_se.obj SelfExtractor_Win32.res";
    CreateProcess(NULL, strBuildString.LockBuffer(), NULL, NULL, 
    //CreateProcess(NULL, "Link.EXE @Link_Args.txt", NULL, NULL, 
                      TRUE /* inherit open handles */, 0, NULL, NULL, &si, &pi);
    //--
                                                        
    // MSDN'98: "We need to close our instance of the inherited pipe write 
    // handle now that it's been inherited so that it will actually close when 
    // the child process ends. This will put an EOF condition on the pipe which 
    // we can then detect." 
    CloseHandle(hPipeOutputWrite);

    m_hChildProcess_2_2 = pi.hProcess;

    // Read the child's output.
    ReadAndHandleOutput_Step_2_2(hPipeOutputRead);

    dwStep2_2_Finish = GetTickCount();
    sprintf(szLog, "Step_2_2 End: %d\r\n", dwStep2_2_Finish - dwStart); 
    WriteFile(hDurationLog, szLog, strlen(szLog), &dwWritten, NULL);

    m_ctlProgressBar.SetPos(100);
    }

    // Helps to delete Duration_Log.txt file in the Build directory.
    CloseHandle(hDurationLog);
}

void CMySaverPropPage2::ReadAndHandleOutput_Step_1_1(HANDLE hPipeRead)
{
    DWORD nBytesRead;
    DWORD nCharsWritten;
    char c;
    char szLine[100];

    DWORD dwLineCharsCount = 0;

    HANDLE hFilePipeDump;
    DWORD dwOffset;
    DWORD dWritten;

    DWORD dwStart, dwFinish;

    DeleteFile(_T("Pipe_Step_1_1_Dump.txt"));
    hFilePipeDump= CreateFile(_T("Pipe_Step_1_1_Dump.txt"), 
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL); 

    dwOffset = SetFilePointer(hFilePipeDump, 0, NULL, FILE_END); 
    
    dwStart = GetTickCount();
    // LL WaitForSingleObject drops, effectively, LINK.EXE at the end.
    while (WaitForSingleObject(m_hChildProcess_1_1, 0) != WAIT_OBJECT_0)
    {
        // MSDN'03: "Read data from a file, starting at the position indicated 
        // by the file pointer. After the read operation has been completed, 
        // the file pointer is adjusted by the number of bytes actually read." 
        if (ReadFile(hPipeRead, &c, 
                     1 /* one character per pass */, &nBytesRead, NULL) == TRUE) 
        {
            WriteFile(hFilePipeDump, &c, 1, &dWritten, NULL);
        }
    }

    // LL this lets to read up to the end of the pipe.
    while(ReadFile(hPipeRead, &c, 
                             1 /* one character per pass */, &nBytesRead, NULL))
    {
        WriteFile(hFilePipeDump, &c, 1, &dWritten, NULL);
    }

    dwFinish = GetTickCount();

    CString str;
    //str.Format("%d", (dwFinish - dwStart));
    str.Format("rc.exe run time: %d", (dwFinish - dwStart));
    //AfxMessageBox(str);

    // Helps to delete Pipe_Step_1_1_Dump.txt file in the Build folder.
    CloseHandle(hFilePipeDump);
}

void CMySaverPropPage2::ReadAndHandleOutput_Step_1_2(HANDLE hPipeRead)
{
    DWORD nBytesRead;
    DWORD nCharsWritten;
    char c;
    char szLine[100];

    DWORD dwLineCharsCount = 0;

    HANDLE hFilePipeDump;
    DWORD dwOffset;
    DWORD dWritten;

    DWORD dwStart, dwFinish;

    DeleteFile(_T("Pipe_Step_1_2_Dump.txt"));
    hFilePipeDump= CreateFile(_T("Pipe_Step_1_2_Dump.txt"), 
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL); 

    dwOffset = SetFilePointer(hFilePipeDump, 0, NULL, FILE_END); 
    
    dwStart = GetTickCount();
    // LL WaitForSingleObject drops, effectively, LINK.EXE at the end.
    while (WaitForSingleObject(m_hChildProcess_1_2, 0) != WAIT_OBJECT_0)
    {
        // MSDN'03: "Read data from a file, starting at the position indicated 
        // by the file pointer. After the read operation has been completed, 
        // the file pointer is adjusted by the number of bytes actually read." 
        if (ReadFile(hPipeRead, &c, 
                     1 /* one character per pass */, &nBytesRead, NULL) == TRUE) 
        {
            WriteFile(hFilePipeDump, &c, 1, &dWritten, NULL);
        }
    }

    // LL this lets to read up to the end of the pipe.
    while(ReadFile(hPipeRead, &c, 
                             1 /* one character per pass */, &nBytesRead, NULL))
    {
        WriteFile(hFilePipeDump, &c, 1, &dWritten, NULL);
    }

    dwFinish = GetTickCount();

    CString str;
    //str.Format("%d", (dwFinish - dwStart));
    str.Format("link.exe run time: %d", (dwFinish - dwStart));
    //AfxMessageBox(str);

    // Helps to delete Pipe_Step_1_2_Dump.txt file in the Build folder.
    CloseHandle(hFilePipeDump);
}

void CMySaverPropPage2::ReadAndHandleOutput_Step_2_1(HANDLE hPipeRead)
{
    DWORD nBytesRead;
    DWORD nCharsWritten;
    char c;
    char szLine[100];

    DWORD dwLineCharsCount = 0;

    HANDLE hFilePipeDump;
    DWORD dwOffset;
    DWORD dWritten;

    DWORD dwStart, dwFinish;

    DeleteFile(_T("Pipe_Step_2_1_Dump.txt"));
    hFilePipeDump= CreateFile(_T("Pipe_Step_2_1_Dump.txt"), 
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL); 

    dwOffset = SetFilePointer(hFilePipeDump, 0, NULL, FILE_END); 
    
    dwStart = GetTickCount();
    // LL WaitForSingleObject drops, effectively, LINK.EXE at the end.
    while (WaitForSingleObject(m_hChildProcess_2_1, 0) != WAIT_OBJECT_0)
    {
        // MSDN'03: "Read data from a file, starting at the position indicated 
        // by the file pointer. After the read operation has been completed, 
        // the file pointer is adjusted by the number of bytes actually read." 
        if (ReadFile(hPipeRead, &c, 
                     1 /* one character per pass */, &nBytesRead, NULL) == TRUE) 
        {
            WriteFile(hFilePipeDump, &c, 1, &dWritten, NULL);
        }
    }

    // LL this lets to read up to the end of the pipe.
    while(ReadFile(hPipeRead, &c, 
                             1 /* one character per pass */, &nBytesRead, NULL))
    {
        WriteFile(hFilePipeDump, &c, 1, &dWritten, NULL);
    }

    dwFinish = GetTickCount();

    CString str;
    //str.Format("%d", (dwFinish - dwStart));
    str.Format("rc.exe run time: %d", (dwFinish - dwStart));
    //AfxMessageBox(str);

    //TerminateProcess(m_hChildProcess,0); 

    // Helps to delete Pipe_Step_2_1_Dump.txt file in the Build folder.
    CloseHandle(hFilePipeDump);
}

void CMySaverPropPage2::ReadAndHandleOutput_Step_2_2(HANDLE hPipeRead)
{
    DWORD nBytesRead;
    DWORD nCharsWritten;
    char c;
    char szLine[200];

    DWORD dwLineCharsCount = 0;

    HANDLE hFilePipeDump;
    DWORD dwOffset;
    DWORD dWritten;

    DWORD dwStart, dwFinish;

    DeleteFile(_T("Pipe_Step_2_2_Dump.txt"));
    hFilePipeDump= CreateFile(_T("Pipe_Step_2_2_Dump.txt"), 
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL); 

    dwOffset = SetFilePointer(hFilePipeDump, 0, NULL, FILE_END); 
    
    dwStart = GetTickCount();
    // LL WaitForSingleObject drops, effectively, LINK.EXE at the end.
    while (WaitForSingleObject(m_hChildProcess_2_2, 0) != WAIT_OBJECT_0)
    {
        // MSDN'03: "Read data from a file, starting at the position indicated 
        // by the file pointer. After the read operation has been completed, 
        // the file pointer is adjusted by the number of bytes actually read." 
        if (ReadFile(hPipeRead, &c, 
                     1 /* one character per pass */, &nBytesRead, NULL) == TRUE) 
        {
            // The sequence is \r -> \n.
            switch (c) 
            {
            case '\r':
                szLine[dwLineCharsCount] = '\0';
                //AfxMessageBox(szLine);
                if (lstrcmp(szLine, "Start Pass2")==0)
                {
                    //AfxMessageBox(szLine);
                    //m_ctlListBox.AddString(szLine);
                }
                
                break;
            case '\n':
                dwLineCharsCount = 0;
                break;
            default:
                szLine[dwLineCharsCount] = c;
                dwLineCharsCount++;
            }
            WriteFile(hFilePipeDump, &c, 1, &dWritten, NULL);
        }
    }

    // LL this lets to read up to the end of the pipe.
    while(ReadFile(hPipeRead, &c, 
                             1 /* one character per pass */, &nBytesRead, NULL))
    {
        // The sequence is \r -> \n.
        switch (c) 
        {
        case '\r':
            szLine[dwLineCharsCount] = '\0';
            //AfxMessageBox(szLine);
            if (lstrcmp(szLine, "Start Pass2")==0)
            {
                //AfxMessageBox(szLine);
                //m_ctlListBox.AddString(szLine);
            }
            
            break;
        case '\n':
            dwLineCharsCount = 0;
            break;
        default:
            szLine[dwLineCharsCount] = c;
            dwLineCharsCount++;
        }
        WriteFile(hFilePipeDump, &c, 1, &dWritten, NULL);
    }

    dwFinish = GetTickCount();

    CString str;
    //str.Format("%d", (dwFinish - dwStart));
    str.Format("Link.exe run time: %d", (dwFinish - dwStart));
    //AfxMessageBox(str);

    //TerminateProcess(m_hChildProcess,0);

    // Helps to delete Pipe_Step_2_2_Dump.txt file in the Build folder.
    CloseHandle(hFilePipeDump);
}

void CMySaverPropPage2::OnButtonMoreParamsClicked()
{
    int iPageCount;
    CMySaverPropPage1* pPP1 = NULL;
    BOOL bPasswordDirty = FALSE;
    BOOL bMSSCommentsDirty = FALSE;
    CString strMsg;

    // Initialize "Optional Settings" dialog parameters.
    COptionalSettings m_dlgOptionalSettings;
    
    if (theApp.m_nBuildOption == 0 /* New */)
    {
        // No password yet (if the password were not updated with the OS 
        // dialog). The theApp.m_strMSSPassword is also empty.
        m_dlgOptionalSettings.m_strPassword = "";
        // No user comments yet (if the comments were not updated with the OS 
        // dialog).
        m_dlgOptionalSettings.m_strMSSComments = 
                                        m_strMSSSpacePaddedComments.TrimRight();
        m_dlgOptionalSettings.m_strSetupDesc = "";
    }
    else
    {
        // Get the data from the MSS directly since OS dialog can be used more 
        // then once for updates (while staying on PP2).
        m_dlgOptionalSettings.m_strPassword = 
                                     GetMSSPassword(theApp.m_strLastEditedFile);
        // Internally, MSS uses fixed-length right-padded with spaces 
        // 80-character string. For the dialog I right-trim spaces to avoid 
        // ending ... etc.
        m_dlgOptionalSettings.m_strMSSComments = GetMSSComments().TrimRight();
        // Setup Description (m_strSetupDesc) is provided only at the last page 
        // in conjunction with setup program build.
        m_dlgOptionalSettings.m_strSetupDesc = "";
    }
    
    if (m_dlgOptionalSettings.DoModal() == IDOK)
    {
        if (theApp.m_nBuildOption == 0 /* New */)
        {
            m_strPassword = m_dlgOptionalSettings.m_strPassword;
            //m_strMSSSpacePaddedComments = 
            //                           m_dlgOptionalSettings.m_strMSSComments;
            m_strMSSSpacePaddedComments = MakeSpacePaddedComments(
                                        m_dlgOptionalSettings.m_strMSSComments);
            m_strSetupDesc = m_dlgOptionalSettings.m_strSetupDesc;
        }
        else if (theApp.m_nBuildOption == 1 /* Edit */)
        {
            // Update MSS edit password only if dirty.
            // The password logic for MSS files is the same as for DOC files in 
            // Word 2000 (Tools/Options, Save tab).
            if (GetMSSPassword(theApp.m_strLastEditedFile) /* current*/ != 
                                  m_dlgOptionalSettings.m_strPassword /* new */)
            {
                iPageCount = ((CMySaverPropSheet*) GetParent())->GetPageCount();
                pPP1 = (CMySaverPropPage1*)((CMySaverPropSheet*) 
                         GetParent())->GetPage(iPageCount - 2 /* zero-based */);

                // Update m_strPassword for consistency. Even if images were 
                // marked for deletion, the actual deletion happens while going 
                // from PP1 to PP2. This means m_strPassword is of little use
                // at this point (no harm to update).
                m_strPassword = m_dlgOptionalSettings.m_strPassword;

                // When images are reordered or deleted (this happens while PP2 
                // activates if PP1 has marked for deletion or reordere images) 
                // MSS must be updated in build directory instead of at the 
                // source (or too much code to re-write).
                if (pPP1->m_bIsImagesDeletionMarked == TRUE || 
                                             pPP1->m_bIsImagesReordered == TRUE)
                {
                    pPP1->UpdateStringResource(23 /* IDS_PASSWORD */, 
                                        m_strPassword /* new password */, TRUE);
                }
                else
                {
                    pPP1->UpdateStringResource(23 /* IDS_PASSWORD */, 
                                              m_strPassword /* new password */);
                }

                // Update for consistency (no harm, and possibly no use).
                theApp.m_strMSSPassword = m_strPassword; 
                bPasswordDirty = TRUE;
            }

            // Update MSS comments only if dirty.
            if (m_strMSSSpacePaddedComments.TrimRight() /* current */ != 
                               m_dlgOptionalSettings.m_strMSSComments /* new */)
            {
                // For all MSS built with wizard prior to MSB v2.0 I did not 
                // enter a string for IDS_MSS_FORMAT_VERSION (#5) so it was 
                // empty. I reserve v1.0 for MSS format built with MSB prior to 
                // v2.0 (here it means only that v1.0 does not reserve an empty 
                // block of 80 symbols for MSS comments). 
                if (m_cd.strMSSFormatVersion.IsEmpty() || 
                                              m_cd.strMSSFormatVersion == "1.0")
                {
                    strMsg = "The MSS comments are not updatable since the ";
                    strMsg += "comments version of MSS format is 1.0. You can ";
                    strMsg += "edit these with versions of MSS format 1.1 or ";
                    strMsg += "higher.";
                    
                    ::MessageBox(0, strMsg, "MyScreenSaver Builder 2.2", 
                                                    MB_OK | MB_ICONINFORMATION); 
                }
                else
                {
                    // MSS format is v1.1 for MSS built with MSB v2.0. I reserve 
                    // here that even if I make the next version of MSS format
                    // (like v1.1.2), these MSS are updatable (MSS comments 
                    // field) with MSB v2.0.

                    // When images are reordered or deleted (this happens while
                    // PP2 activates if PP1 has marked for deletion or reordere
                    // images) MSS must be update in build directory instead of
                    // at the source (or too much code to re-write).
                    if (pPP1->m_bIsImagesDeletionMarked == TRUE || 
                                             pPP1->m_bIsImagesReordered == TRUE)
                    {
                        UpdateMSSComments(
                                  m_dlgOptionalSettings.m_strMSSComments, TRUE);
                    }
                    else
                    {
                        UpdateMSSComments(
                                        m_dlgOptionalSettings.m_strMSSComments);
                    }
                    
                    // Update m_strMSSSpacePaddedComments (fixed-length 80-chars 
                    // string) for consitency. Even if images were marked for 
                    // deletion, the actual deletion happens while going from 
                    // PP1 to PP2. This means m_strMSSSpacePaddedComments is of 
                    // little use at this point (no harm to update).
                    m_strMSSSpacePaddedComments = 
                        MakeSpacePaddedComments(
                                      m_dlgOptionalSettings.m_strMSSComments);
                    bMSSCommentsDirty = TRUE;
                }
            }

            // EXE comments are used in new mode only (harmless generalization 
            // to set for edit mode also).
            m_strSetupDesc = m_dlgOptionalSettings.m_strSetupDesc;
        }
        
        // If the Optional Settings's parameters were updated set dirty flag.
        if (bPasswordDirty || bMSSCommentsDirty) 
                                                m_bOptionalSettingsDirty = TRUE;
    }
}

void CMySaverPropPage2::OnButtonChangeClicked()
{
    CDirDialog dirDlg;
    CString m_szDirName;

    if(dirDlg.DoBrowse())
    {
        m_szDirName = dirDlg.m_strPath; 
        m_strSetupDir = m_szDirName;
    } 

    // Pass data to dialog
    UpdateData(FALSE);
}

void CMySaverPropPage2::OnCheckboxCreateSetupClicked()
{
    //Update class data from dialog UI data
    UpdateData(TRUE);

    if (m_bCheckBoxCreateSetup)
    {
        // Short way to hide/show (without making control variable).
        GetDlgItem(IDC_PP2_STATIC5)->EnableWindow(TRUE);
        GetDlgItem(IDC_PP2_STATIC6)->EnableWindow(TRUE);
        GetDlgItem(IDC_PP2_STATIC7)->EnableWindow(TRUE);

        m_ctlEditSetupName.EnableWindow(TRUE);
        m_ctlEditSetupLocation.EnableWindow(TRUE);
        GetDlgItem(IDC_PP2_BUTTON_CHANGE)->EnableWindow(TRUE);
    }
    else
    {
        // Short way to hide/show (without making control variable).
        GetDlgItem(IDC_PP2_STATIC5)->EnableWindow(FALSE);
        GetDlgItem(IDC_PP2_STATIC6)->EnableWindow(FALSE);
        GetDlgItem(IDC_PP2_STATIC7)->EnableWindow(FALSE);

        m_ctlEditSetupName.EnableWindow(FALSE);
        m_ctlEditSetupLocation.EnableWindow(FALSE);
        GetDlgItem(IDC_PP2_BUTTON_CHANGE)->EnableWindow(FALSE);
    }
}

// The standard name is the name in the form: MMDDYY_MMSS and provided
// as default for MSS title, name, and setup name (the same string).
// BAsed on code from 4S Lock 1.09's ssstc.
CString  CMySaverPropPage2::GetStandardName()
{
    CString strStandardName;
    SYSTEMTIME st;
    char szTime[60];
    char szHour[5];
    char szMinute[5];
    char szMonth[5];
    char szDay[5];
    char szYear[3];
    char szTemp[10];

    // Get time and then format it to MMDDYY_MMSS.
    GetLocalTime(&st);

    // Convert "2:6" time into "02:06" time
    sprintf(szHour, st.wHour < 10 ? "0%d" : "%d",  st.wHour);
    sprintf(szMinute, st.wMinute < 10 ? "0%d" : "%d",  st.wMinute);

    // Convert "1/29/2005" date into "01/29/05" date
    sprintf(szMonth, st.wMonth < 10 ? "0%d" : "%d",  st.wMonth);
    sprintf(szDay, st.wDay < 10 ? "0%d" : "%d",  st.wDay);

    // Convert "2005" into "05" (there is no C-string truncation routine, but
    // this trick works.
    sprintf(szTemp, "%d", st.wYear);
    sprintf(szYear, &szTemp[2]);

    // Format resulting string.
    sprintf(szTime, "%s%s%s_%s%s", szMonth, szDay, szYear, szHour, szMinute);
    
    strStandardName = szTime;

    return strStandardName;
}

void CMySaverPropPage2::EditMode_GetImageSetData(LPCTSTR pszFullFileName, 
                                                              BOOL bWithRebuild)
{
    HINSTANCE hResInst;
    TCHAR szBuffer[100];

    hResInst = LoadLibrary(pszFullFileName);

    if (hResInst == NULL)
    {
        AfxMessageBox("LoadLibrary failed!");
    }

    // Copy data for edit mode with MSS rebuild.
    if (bWithRebuild) //
    {
        LoadString(hResInst, 1 /* title (name) ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
        m_cd.strImageSetTitle = szBuffer;

        LoadString(hResInst, 4 /* pictures category ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
        m_cd.strPicturesCategory = szBuffer;

        LoadString(hResInst, 5 /* MSS format version ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
        m_cd.strMSSFormatVersion = szBuffer;

        LoadString(hResInst, 20 /* sample ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
        m_cd.strIsSample = szBuffer;

        LoadString(hResInst, 21 /* registered ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
        m_cd.strIsRegistered = szBuffer;

        LoadString(hResInst, 22 /* custom ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
        m_cd.strIsCustom = szBuffer;

        LoadString(hResInst, 23 /* password ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
        m_cd.strPassword = szBuffer;

        LoadString(hResInst, 91 /* 1st reserved string ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
        m_cd.strReserved1 = szBuffer;

        LoadString(hResInst, 92 /* 2nd reserved string ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
        m_cd.strReserved2 = szBuffer;

        LoadString(hResInst, 932 /* 3d reserved string ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
        m_cd.strReserved3 = szBuffer;
        
        // Get MSS comments from version information (VERSIONINFO).
        m_cd.strMSSComments = GetMSSComments(); 
    }
    else // copy data for edit mode with no MSS rebuild
    {
        LoadString(hResInst, 1 /* title (name) ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
        m_cd.strImageSetTitle = szBuffer;

        LoadString(hResInst, 5 /* MSS format version ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
        m_cd.strMSSFormatVersion = szBuffer;

        LoadString(hResInst, 23 /* password ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
        m_cd.strPassword = szBuffer;
    }

    FreeLibrary(hResInst);
}

void CMySaverPropPage2::OnEditChange_Title()
{
    if (m_nBuildOption == 1 /* Edit MSS */)
    {
        //AfxMessageBox("OnEditChange_Title()");
        m_bTitleDirty = TRUE;
        //CString str; 
        //str.Format("m_bTitleDirty: %d", m_bTitleDirty); 
        //AfxMessageBox(str);
    }
}

void CMySaverPropPage2::OnEditChange_FileName()
{
    if (m_nBuildOption == 1 /* Edit MSS */)
    {
        m_bFileNameDirty = TRUE;
    }
}

CString CMySaverPropPage2::GetMSSPassword(LPCTSTR pszFullFileName /* in */) 
{
    HINSTANCE hResInst;
    TCHAR szBuffer[100];
    CString strPassword;

    hResInst = LoadLibrary(pszFullFileName);

    if (hResInst == NULL)
    {
        return 0;
    }

    LoadString(hResInst, 23 /* password resource ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
    FreeLibrary(hResInst);

    strPassword = szBuffer;

    return strPassword ;
}

// Version's Comments information is limited to 80 ANSI characters.
CString CMySaverPropPage2::GetMSSComments()
{
    CString strFullFileName;
    DWORD dwSize;
    DWORD dwHandle;
    LPBYTE lpBuffer = NULL;
    struct LANGANDCODEPAGE {WORD wLanguage;	WORD wCodePage;}* lpTranslate;
    UINT cbTranslate;
    CString strSubBlock;
    LPTSTR  pValueBuffer;
    DWORD dwBytes;
    CString strMSSComments;

    strFullFileName = theApp.m_strLastEditedFile;

    // Get the pointer (lpBuffer) to the version information data. The pointer
    //is an array of wide charactes, padding information for version info
    // substructures etc. The point is that it cannot be casted to C-string.
    dwSize = GetFileVersionInfoSize(strFullFileName, &dwHandle); // 1580
                                                             
    lpBuffer = new BYTE[dwSize]; // deleted at the end of function
    GetFileVersionInfo(strFullFileName, 0 /* not used */, dwSize, lpBuffer);

    // Two calls to VerQueryValue are used: 1st call is used to get 
    // translation info (in general array of languages and code-page IDs): 
    // just one language is used at the moment (v2.0). LL mandatory. 
    VerQueryValue(lpBuffer, TEXT("\\VarFileInfo\\Translation"), 
                                           (LPVOID*)&lpTranslate, &cbTranslate);

    
    // THe 2nd call to VerQueryValue is used to retreive language-specific 
    // information (language plus code page) from string table for Comments.
    strSubBlock.Format(_T("\\StringFileInfo\\%04x%04x\\Comments"), 
                                lpTranslate->wLanguage, lpTranslate->wCodePage);
    VerQueryValue(lpBuffer, (LPTSTR)((LPCTSTR) strSubBlock), 
            (LPVOID*)&pValueBuffer, (PUINT)&dwBytes); // returns 81 for 80-chars 
                                                      // strings (without NUL)

    strMSSComments = pValueBuffer;
    //AfxMessageBox(str);

    delete [] lpBuffer;	

    return strMSSComments;
}
// Happened to be helpful: 
// http://www.codeproject.com/KB/cpp/UpdateVersion.aspx?print=true.
void CMySaverPropPage2::UpdateMSSComments(CString strComments, 
                                                BOOL bUseBuildDir /* = FALSE */)
{
    CString strFullFileName;
    CString strFixedLengthComments; // 80 characters right-padded with spaces 
    char szComments[81]; // 81: 80 space characters (stub) plus terminating NUL
    DWORD dwSize;
    DWORD dwHandle;
    LPBYTE lpBuffer = NULL;
    int nCommentsTitleEndsAt;
    int nUserCommentsStartAt;
    int i;
    struct LANGANDCODEPAGE {WORD wLanguage;	WORD wCodePage;}* lpTranslate;
    UINT cbTranslate;
    HANDLE hResource;

    if (bUseBuildDir)
    {
        // Update in build directory when images are reordered or deleted.
        strFullFileName = theApp.m_strMSBDataDirectory + _T("\\Build\\") + 
                                                m_strImageSetName +  _T(".mss");
        //AfxMessageBox(strFullFileName);
    }
    else
    {
        // Update in the source directory when images are not reordered or 
        // deleted.
        strFullFileName = theApp.m_strLastEditedFile;
    }

    dwSize = GetFileVersionInfoSize(strFullFileName, &dwHandle); // 1580
    lpBuffer = new BYTE[dwSize];
    GetFileVersionInfo(strFullFileName, 0 /* not used */, dwSize, lpBuffer);
    
    // I use manual update of lpBuffer at proper position since the recommended 
    // way: call 2 times VerQueryValue (second call retrieves a pointer 
    // pValueBuffer to Comments block from StringFileInfo table), update 
    // pValueBuffer (string!), but next BeginUpdateResource ... 
    // EndUpdateResource fails for StringFileInfo keys (the author of mentioned 
    // codeproject.com's article makes wrong suggestion that it works not 
    // providing a sample: maybe he hit the problem but just rounded the corners).
    // The way I found is to figure out where "Comments" ends and update manually
    // the lpBuffer. The major point is that BeginUpdateResource uses merely the
    // updated lpBuffer! My major assumption is that no other strings "nts" 
    // before "Comments" can be found at the top of version information 
    // (negligible, and easily controllable risk).

    for (i = 0; i < dwSize; i++)
    {
       if (lpBuffer[i] == 'n' && lpBuffer[i + 2] == 't' && 
                                                         lpBuffer[i + 4] == 's')
       {
           nCommentsTitleEndsAt = i + 4; // byte #308 ("s", ANSI)
           break;
       }
    }

    // Need to specifically write 80 sysmbols otherwise the length of comments 
    // with be the length of strComments, and thenit will be impossible to write 
    // more symbols than strComments.GetLength().
    nUserCommentsStartAt = nCommentsTitleEndsAt + 4; // ((i + 4) + 4)
    CString strStubString(' ', 80 - strComments.GetLength());
    strFixedLengthComments = strComments + strStubString;
    lstrcpy(szComments, strFixedLengthComments);

    //CString str;
    //str.Format("%s %d szComments.GetLength(): %d sizeof(szComments): %d %s %d", 
    //	strFullFileName, nUserCommentsStartAt, 
    //	          strComments.GetLength(), sizeof(szComments), strComments, 
    //			  strComments.GetLength()); 
    //AfxMessageBox(str);

    // Comments can be 80 ANSI symbols maximum. Comments are recorded as wide 
    // 2-byte strings by Windows system, and are to be written starting at the 
    // position where "Comments" title ends plus one space: "Comments" + one 
    // space -> #312 (LL exact position might differ). Although this is likely 
    // overkill I fill out with zeros 81*2 bytes taking in account the maximum 
    // character length of the user comments and terminating NUL issue (tested 
    // OK: 80 * 2 zeros for maximum 80 chars string, and basically it is 
    // guaranteed that with ANSI characters even with the string of maximum 
    // length the last symbol will be 0 since one byte goes for ANSI symbol and 
    // the next byte for 0). By the way : sizeof(TCHAR) is 1.
    ZeroMemory(lpBuffer + nUserCommentsStartAt, 
                                    81 * 2 /* size of block to fill (bytes) */); 
    for (i = 0; i < sizeof(szComments) - 1 /* 80 */; i++) 
    {
        lpBuffer[nCommentsTitleEndsAt + 4 + 2*i] = szComments[i];
    }

    // Call VerQueryValue to get translation info (in general array of languages 
    // and code-page IDs). TODO: LL can remove this call by using language: 
    // 0x0409, code page: 0x04b0, or 1200.
    VerQueryValue(lpBuffer, TEXT("\\VarFileInfo\\Translation"), 
                                           (LPVOID*)&lpTranslate, &cbTranslate);

    // Finally, merely use updated lpBuffer to update version info.
    hResource = BeginUpdateResource(strFullFileName, FALSE);
    if (NULL != hResource)
    {
        if (UpdateResource(hResource, RT_VERSION, 
            MAKEINTRESOURCE(VS_VERSION_INFO), 
            lpTranslate->wLanguage, // 0x00327be8 (language: 0x0409, code page: 
                                    // 0x04b0, or 1200)
                                          lpBuffer, dwSize /* 1580 */) != FALSE)
        {
            EndUpdateResource(hResource, FALSE);
        }
    }

    delete [] lpBuffer;
}

CString CMySaverPropPage2::MakeSpacePaddedComments(CString strComments)
{
    CString strFixedLengthComments; // 80 characters right-padded with spaces 

    CString strStubString(' ', 80 - strComments.GetLength());
    strFixedLengthComments = strComments + strStubString;
  
    return strFixedLengthComments;
}

void CMySaverPropPage2::DeleteDirectory(LPCTSTR pszTargetDirectory) 
{
    TCHAR szSearchString[MAX_PATH];	
    TCHAR szFileName[MAX_PATH];
    WIN32_FIND_DATA fd;

    wsprintf(szSearchString, "%s%s", pszTargetDirectory, _T("\\*.*")); 

    // Enumerate files in the directory with FindFirstFile/FindNextFile and 
    // delete them one by one. 
    HANDLE hFind = FindFirstFile(szSearchString , &fd);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        wsprintf(szFileName, "%s\\%s", pszTargetDirectory, fd.cFileName);
        DeleteFile(szFileName);

        while(FindNextFile(hFind , &fd))
        {
            wsprintf(szFileName, "%s\\%s", pszTargetDirectory, fd.cFileName);
            DeleteFile(szFileName);
        }
        FindClose(hFind);
    }

    // The function SetCurrentDirectory has been used in the build code,
    // setting the current directory to the Build folder. This locks the folder,
    // and prevents it from being deleted with RemoveDirectory(...) even if the
    // Build folder is empty. Therefore I reset the current directory to an
    // existing folder (like install directory). This lock is undocumented,
    // I found it by trial & error.
    SetCurrentDirectory(theApp.m_strMSBDataDirectory);

    // When the directory is empty, delete the directory (only empty directories 
    // can be deleted with RemoveDirectory).
    RemoveDirectory(pszTargetDirectory);
}

// Copied from PP0 with updates (PP0->PP2).
// OnHelpInfo (WM_HELP handler) handles context help: 1) "?" context help, 2) 
// SHIFT+F1, 3) F1 (2 and 3 are the same). 
BOOL CMySaverPropPage2::OnHelpInfo(HELPINFO* pHelpInfo)
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
    for (int i = 0; i < theApp.m_ContextHelp.m_nMapSizePP2; i++) 
    //for (int i = 0; i < 17; i++) 
    {
        if (pHelpInfo->iCtrlId ==  theApp.m_ContextHelp.m_MapPP2[i].nControlID) 
        { 
            memset(&ContextHelpPopup, 0, sizeof(HH_POPUP) );   
            ContextHelpPopup.cbStruct = sizeof(HH_POPUP);

            ContextHelpPopup.hinst = AfxGetInstanceHandle(); // application 
                                                             // instance
            ContextHelpPopup.idString = 
                      theApp.m_ContextHelp.m_MapPP2[i].nStringHelpID; // resource ID

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
void CMySaverPropPage2::OnContextMenu(CWnd* pWnd, CPoint point)
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
BOOL CMySaverPropPage2::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

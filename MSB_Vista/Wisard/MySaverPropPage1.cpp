#include "stdafx.h"
#include "MySaver.h"
#include "MySaverPropPage1.h"
#include "MySaverPropPage0.h" // added to navigate from PP1 to PP0
#include "PictureDetails.h"
#include ".\mysaverproppage1.h"

IMPLEMENT_DYNAMIC(CMySaverPropPage1, CPropertyPage)

CMySaverPropPage1::CMySaverPropPage1() : CPropertyPage(CMySaverPropPage1::IDD)
{
    if (theApp.m_bEnableDebug)
    {
        theApp.App_Debug_FunctionCallTrace(
                                      "CMySaverPropPage1::CMySaverPropPage1()");
    }

    m_psp.dwFlags |= PSP_USETITLE | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE | 
                                                                    PSP_HASHELP; 
    m_psp.pszHeaderTitle = MAKEINTRESOURCE(IDS_PP1_HEADER_TITLE);
    m_psp.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_PP1_HEADER_SUBTITLE);
}

CMySaverPropPage1::~CMySaverPropPage1()
{
}

void CMySaverPropPage1::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMySaverPropPage1, CPropertyPage)
    ON_BN_CLICKED(IDC_PP1_BUTTON_ADD, OnButtonAddClicked)
    ON_BN_CLICKED(IDC_PP1_BUTTON_MODIFY, OnButtonModifyClicked)
    //ON_WM_DROPFILES()
    ON_BN_CLICKED(IDC_PP1_BUTTON_DELETE, OnButtonDeleteClicked)
    ON_WM_HELPINFO()
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
// Description: Because OnInitInstance, unlike OnSetActive, called only
//              once (i.e. per PropertySheet) an initilization moved to 
//              OnSetActive.
//------------------------------------------------------------------------------

BOOL CMySaverPropPage1::OnInitDialog() 
{
    CPropertyPage::OnInitDialog();
    //--------------------------------------------------------------------------

    if (theApp.m_bEnableDebug)
    {
        CString strDebugMsg;
        theApp.m_dwPP1InitTime = GetTickCount();
        strDebugMsg.Format("CMySaverPropPage1::OnInitDialog(): %d", 
                               theApp.m_dwPP1InitTime - theApp.m_dwPP0InitTime); 
        theApp.App_Debug_FunctionCallTrace(strDebugMsg);
    }

    m_pSheet = (CPropertySheet*) GetParent(); 

    // The application-level flag m_bImagesManuallyAdded indicates if images 
    // were added with PP1's Add... button. The  flag initialization is placed 
    // into OnInitInstance (called once) to avoid resetting the flag if 
    // Back/Next buttons were used. The flag is used in the last page to
    // provide right message OnFinish.
    theApp.m_bImagesManuallyAdded = FALSE;
    
    // To provide that list view is created once while switching Back/Forward 
    // with Property Pages, the list view is created in OnInitDialog() instead
    // of OnSetActive() (as with v1.1 where list view was created indirectly
    // from resorce file).
    DWORD dwExStyles = WS_EX_ACCEPTFILES | WS_EX_CLIENTEDGE;
    DWORD dwStyles = WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | 
        LVS_SINGLESEL /* deletion can currently remove only one selected item*/|
                                               LVS_ALIGNLEFT | LVS_NOSORTHEADER; 
    m_ctlListView.m_pPP1 = this;
    m_ctlListView.CreateEx(dwExStyles, dwStyles, CRect(16,7,456,195), this, 
                                                              IDC_PP1_LISTVIEW); 
    // I use separate function for setting styles since setting some styles in 
    // Create(...) (like LVS_EX_FULLROWSELECT) does not have any effect.
    SetListViewStyles();

    CMySaverApp* pApp =  static_cast<CMySaverApp*>(AfxGetApp());
    m_fAcceptableRAMUsage = pApp->m_fAcceptableRAMUsage;
    
    return TRUE; // defaults 
}

//------------------------------------------------------------------------------
// Description: Unlike OnInitInstance, OnSetActive called every time
//              a page becomes visible;
//              This page->Back (data update)->This page - OK.
//------------------------------------------------------------------------------

BOOL CMySaverPropPage1::OnSetActive()
{
    if (theApp.m_bEnableDebug)
        theApp.App_Debug_FunctionCallTrace("CMySaverPropPage1::OnSetActive()");

    CPropertySheet* psheet = (CPropertySheet*) GetParent(); 
    int iPageCount = ((CMySaverPropSheet*) GetParent())->GetPageCount();
    CMySaverPropPage0* pPP0 = (CMySaverPropPage0*)
             //((CMySaverPropSheet*) GetParent())->GetPage(iPageCount - 4); v1.1
                    ((CMySaverPropSheet*) GetParent())->GetPage(iPageCount - 3);
    
    m_strStatusMessage = _T("");
    m_strInitImages = pPP0->m_strInitImages; 
    m_bShowToolTips = pPP0->m_bShowToolTips;
    m_nBuildOption = pPP0->m_nBuildOption;
    m_nPicturesCategory = pPP0->m_nPicturesCategory; // used in picture details

    //DWORD dwExStyles = WS_EX_CLIENTEDGE | WS_EX_ACCEPTFILES;
    //DWORD dwStyles = WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_ALIGNLEFT; 
    //m_ctlListView.m_pPP1 = this;
    //m_ctlListView.CreateEx(dwExStyles, dwStyles, 
    //	CRect(16,7,456,195), this, IDC_PP1_LIST);  //CRect(7,3,303,116) 


    // I use separate function for setting styles since setting some styles
    // in Create(...) (like LVS_EX_FULLROWSELECT) does not have any effect.
    //SetListViewStyles();

    //--------------------------------------------------------------------------
    // Because user can switch back to previous property page and then click 
    // next button again, the list view and associated items should be cleaned 
    // up before filling up again.  Note that cleanup (GetHeaderCtrl() to be 
    // exact) called  after SetListViewStyles() to avoid crash.
    //-------------------------------------------------------------------------

    CleanupListView();

    // The flags are used only when an image deleted or images reordered.
    m_bIsImagesDeletionMarked = FALSE;
    m_bIsImagesReordered  = FALSE;

    InitListViewColumns();
    if (m_nBuildOption == 0 /* New MSS */)
    {
        InitListViewItems(_T("gif"));
        InitListViewItems(_T("jpg"));

        psheet->SetWizardButtons(PSWIZB_NEXT);
    }
    else if (m_nBuildOption == 1 /* Edit MSS */)
    {
        //m_nPicturesCategory = EditMode_GetPicturesCategory(m_strInitImages);

        // TODO: weird thing... The snippet below is exactly the code in the 
        // function EditMode_GetPicturesCategory (now commented). If use the
        // the function FreeLibrary fails to unload MSS (if LoadString 
        // commented FreeLibrary succeeds). However code copy/pasted to here
        // directly works OK... TODO: if resolve, restore initial settings
        // EditMode_GetPicturesCategory in PP0 (not in PP1 - placed here
        // when testing).

        // -- snippet begins
        HINSTANCE hResInst;
        TCHAR szBuffer[100];
        CString strPicturesCategory;

        hResInst = LoadLibrary(m_strInitImages);

        if (hResInst == NULL)
        {
            AfxMessageBox("LoadLibrary failed!");
        }

        LoadString(hResInst, 4 /* pictures category ID */, szBuffer, 
                                            sizeof(szBuffer)/sizeof(szBuffer[0]));
        strPicturesCategory = szBuffer;

        if (strPicturesCategory == "Not specified")
        {
            m_nPicturesCategory = 0;
        }
        else if (strPicturesCategory == "Cars") 
        {
            m_nPicturesCategory = 1;
        }

        FreeLibrary(hResInst);
        //--  snippet ends

        LoadPicturesAndStrings(m_strInitImages);

        if (theApp.m_nEditOption == 0 /* edit MSS data */)
        {
            psheet->SetWizardButtons(PSWIZB_FINISH);
            //psheet->SetFinishText("Save && Finish");
            psheet->SetFinishText("Close"); // LL removes BACK button
        }
        else
            psheet->SetWizardButtons(PSWIZB_NEXT);
    }
    
    //psheet->SetWizardButtons(PSWIZB_NEXT);

    return CPropertyPage::OnSetActive();
}

void CMySaverPropPage1::CleanupListView()
{
    m_vecImages.clear();

    int nColumnCount = m_ctlListView.GetHeaderCtrl()->GetItemCount();

    if (nColumnCount > 0)
    {
        for (int i = 0; i < nColumnCount; i++)
        {
            m_ctlListView.DeleteColumn(0);
        }
    }

    m_ctlListView.DeleteAllItems(); 
}

// put Load image in one function (not finished)
//BOOL CPicture::Load(HINSTANCE  hResInst, UINT nIDRes)
//{
//	HRSRC hRsrc;
//	_TCHAR szName[10];
//
//	sprintf(szName, "#%d", nIDRes);
//	hRsrc = FindResource(hResInst, szName, "IMAGES"); //"#103"
//	if (hRsrc == NULL)
//	{
//		AfxMessageBox("FindResource failed!");
//		return FALSE;
//	}
//
//	// load resource into memory
//	BYTE* lpRsrc = (BYTE*)LoadResource(hResInst, hRsrc);
//	if (lpRsrc == NULL)
//	{
//		AfxMessageBox("LoadResource failed!");
//		return FALSE;
//	}
//
//	// create memory file and load it
//	DWORD len = SizeofResource(hResInst, hRsrc);
//	CMemFile file(lpRsrc, len);
//	BOOL bRet = Load(file);
//	FreeResource(hRsrc);
//
//	return bRet;
//}

//BOOL CPicture::Load(HINSTANCE  hResInst, UINT nIDRes)
DWORD CPicture::Load(HINSTANCE  hResInst, UINT nIDRes)
{
    HRSRC hRsrc;
    _TCHAR szName[10];

    sprintf(szName, "#%d", nIDRes);
    hRsrc = FindResource(hResInst, szName, "IMAGES"); //"#103"
    if (hRsrc == NULL)
    {
        AfxMessageBox("FindResource failed! (D)");
        return FALSE;
    }

    // Load resource into memory.
    BYTE* lpRsrc = (BYTE*)LoadResource(hResInst, hRsrc);
    if (lpRsrc == NULL)
    {
        AfxMessageBox("LoadResource failed!");
        return FALSE;
    }

    // Create memory file and load it.
    DWORD len = SizeofResource(hResInst, hRsrc);
    CMemFile file(lpRsrc, len);
    BOOL bRet = Load(file);
    FreeResource(hRsrc);

    //return bRet;
    return len;
}

BOOL CPicture::Load(CFile& file)
{
    CArchive ar(&file, CArchive::load | CArchive::bNoFlushOnDelete);
    return Load(ar);
}

BOOL CPicture::Load(CArchive& ar)
{
    CArchiveStream arcstream(&ar);
    return Load((IStream*)&arcstream);
}

BOOL CPicture::Load(IStream* pstm)
{
    Free();

    HRESULT hr = OleLoadPicture(pstm, 
                                0, 
                                FALSE,
                                IID_IPicture, 
                                (void**)&m_spIPicture);

    ASSERT(SUCCEEDED(hr) && m_spIPicture);
    
    return TRUE;
}

void CMySaverPropPage1::SetListViewStyles()
{
    // Setup regular styles.
    //DWORD dwStyle = ::GetWindowLong(m_ctlListView.m_hWnd, GWL_STYLE );
    //::SetWindowLong(m_ctlListView, GWL_STYLE, ( dwStyle & ~LVS_TYPEMASK ) | LVS_REPORT | LVS_NOSORTHEADER ); 
    
    // Setup extended styles.
    //m_ctlListView.SetExtendedStyle(LVS_EX_GRIDLINES);
    m_ctlListView.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_SHOWSELALWAYS);
}

void CMySaverPropPage1::InitListViewColumns()
{
    m_ctlListView.InsertColumn(0, _T("File Name"), LVCFMT_CENTER);
    m_ctlListView.SetColumnWidth(0, 245);
    m_ctlListView.InsertColumn(1, _T("File Size (KB)"), LVCFMT_CENTER);
    m_ctlListView.SetColumnWidth(1, 80);
    m_ctlListView.InsertColumn(2, _T("Dimensions"), LVCFMT_CENTER);
    m_ctlListView.SetColumnWidth(2, 90);
    //m_ctlListView.InsertColumn(3, _T("Status"), LVCFMT_CENTER);
    //m_ctlListView.SetColumnWidth(3, 95);
}

void  CMySaverPropPage1::InitListViewItems(LPCTSTR pszImageType)
{
    // Get images directory (directory to seach out)
    TCHAR szSearchDir[300]; 
    //*.jpg, *.gif" does not work!
    wsprintf(szSearchDir, "%s%s%s", m_strInitImages, "\\*.", pszImageType); 

    TCHAR szFullFileName[300];
    CFile file;
    CPicture m_Picture;
    long hmWidth,hmHeight; // HIMETRIC units
    CWnd *pWnd = CWnd::FromHandle(::GetDesktopWindow());
    CClientDC dc(pWnd);

    TCHAR szFileSize[50];
    // FindFirstFile, FindNextFile and, when found, obtain the name string packed 
    // within DLL 
    HANDLE hFind;
    WIN32_FIND_DATA fd;
    int iItem = 0;

    hFind = FindFirstFile(szSearchDir , &fd);

    if(hFind != INVALID_HANDLE_VALUE)
    {
        wsprintf(szFileSize, "%d", fd.nFileSizeLow);
        wsprintf(szFullFileName, "%s\\%s", m_strInitImages,fd.cFileName);
        
        file.Open(szFullFileName, CFile::modeRead);
        m_Picture.Load(file);
        m_Picture.m_spIPicture->get_Width(&hmWidth);  
        m_Picture.m_spIPicture->get_Height(&hmHeight);
        file.Close();
        
        CSize sizeImage(hmWidth,hmHeight); dc.HIMETRICtoDP(&sizeImage);
        
        TCHAR szFileDimensions[20];
        sprintf(szFileDimensions, "%dx%d", sizeImage.cx, sizeImage.cy);

        InsertListViewItem(iItem, fd.cFileName, szFileSize, szFileDimensions);
        iItem++;

        while(FindNextFile(hFind , &fd))
        {
            wsprintf(szFileSize, "%d", fd.nFileSizeLow);
            wsprintf(szFullFileName, "%s\\%s", m_strInitImages,fd.cFileName);
            
            file.Open(szFullFileName, CFile::modeRead);
            m_Picture.Load(file);
            m_Picture.m_spIPicture->get_Width(&hmWidth);  
            m_Picture.m_spIPicture->get_Height(&hmHeight);
            file.Close();
            
            CSize sizeImage(hmWidth,hmHeight); dc.HIMETRICtoDP(&sizeImage);
            TCHAR szFileDimensions[20];
            sprintf(szFileDimensions, "%dx%d", sizeImage.cx, sizeImage.cy);

            InsertListViewItem(iItem, fd.cFileName, szFileSize, szFileDimensions);
            iItem++;
        }
        FindClose(hFind);
    }
}

BOOL CMySaverPropPage1::LoadPicturesAndStrings(LPCTSTR pszFileName)
{
    TCHAR szFileName[100];
    TCHAR szTitle[100];
    // Main.
    TCHAR szYear[10];
    TCHAR szMake[100];
    TCHAR szModel[100];
    TCHAR szSubmodel[100];
    TCHAR szType[100];
    TCHAR szPrice[100];
    TCHAR szPower[100];
    TCHAR szFuel[100];
    // Physical dimensions.
    TCHAR szLength[100];
    TCHAR szWidth[100];
    TCHAR szHeight[100];
    TCHAR szWeight[100];
    TCHAR szClearance[100];
    // Misc
    TCHAR szCountry[100];
    TCHAR szTankVolume[100];
    DWORD dwImageSize;
    TCHAR szFileSize[50];
    CImageMap m_ImageMap; 

    TCHAR szBuffer[100];  // accepts DLL's image count
    int iImageID, iTitleID, iFileNameID; 
    int iImageResourceStartID = 100;
    long hmWidth,hmHeight;
    CSize sizeImage;
    TCHAR szFileDimensions[20];
    CWnd *pWnd = CWnd::FromHandle(::GetDesktopWindow());
    CClientDC dc(pWnd);

    HINSTANCE hResInst = LoadLibrary(pszFileName);

    if(hResInst == NULL)
    {
        AfxMessageBox("LoadLibrary failed!");
        return FALSE;
    }

    //  Load pictures and titles from MSS file.
    LoadString(hResInst, 10 /* always ImageCount ID */ , szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
    m_iImageCount = _tstoi(szBuffer);

    for (int i = 0; i < m_iImageCount; i++) 
    {
        if (m_nPicturesCategory == 0 /* "Not specified" */)
        {
            iImageID = iImageResourceStartID + i + 1;
            // Images 101 ... 105 ...
            dwImageSize = m_Picture[i].Load(hResInst, iImageID);
            // Titles 201 ... 205 ...
            iTitleID = iImageID + 100; 
            LoadString(hResInst, iTitleID, szTitle, 
                                            sizeof(szTitle)/sizeof(szTitle[0]));
            // File names 301 ... 305 ...
            iFileNameID = 301 /* file names start ID */ + i; 
            LoadString(hResInst, iFileNameID, szFileName, 
                                      sizeof(szFileName)/sizeof(szFileName[0]));

            m_ImageMap.m_strFileName = szFileName;
            m_ImageMap.m_strTitle = szTitle; 
        }
        else if (m_nPicturesCategory == 1 /* Cars */)
        {
            iImageID = iImageResourceStartID + i + 1;
            // Images 101 ... 105 ...
            dwImageSize = m_Picture[i].Load(hResInst, iImageID);
            // Titles 201 ... 205 ...
            iTitleID = iImageID + 100; 
            LoadString(hResInst, iTitleID, szTitle, 
                                            sizeof(szTitle)/sizeof(szTitle[0]));
            // File names 301 ... 305 ...
            iFileNameID = 301 /* file names start ID */ + i; 
            LoadString(hResInst, iFileNameID, szFileName, 
                                      sizeof(szFileName)/sizeof(szFileName[0]));
            // Load year string.
            LoadString(hResInst, 1000 /* year ID (same for car MSS) */, szYear, 
                                              sizeof(szYear)/sizeof(szYear[0]));
            // Load make string.
            LoadString(hResInst, 1100 /* make ID (same for car MSS) */, szMake, 
                                              sizeof(szMake)/sizeof(szMake[0]));
            // Load make string.
            LoadString(hResInst, 1201 /* model start ID */ + i, szModel, 
                                            sizeof(szModel)/sizeof(szModel[0]));
            // Load submodel string.
            LoadString(hResInst, 1301 /* submodel start ID */ + i, szSubmodel, 
                                      sizeof(szSubmodel)/sizeof(szSubmodel[0]));
            // Load type string.
            LoadString(hResInst, 1401 /* type start ID */ + i, szType, 
                                              sizeof(szType)/sizeof(szType[0]));
            // Load price string.
            LoadString(hResInst, 1501 /* price start ID */ + i, szPrice, 
                                            sizeof(szPrice)/sizeof(szPrice[0]));
            // Load price string.
            LoadString(hResInst, 1601 /* power start ID */ + i, szPower, 
                                            sizeof(szPower)/sizeof(szPower[0]));
            // Load fuel string.
            LoadString(hResInst, 1701 /* fuel start ID */ + i, szFuel, 
                                              sizeof(szFuel)/sizeof(szFuel[0]));
            // Load length string.
            LoadString(hResInst, 1801 /* length start ID */ + i, szLength, 
                                          sizeof(szLength)/sizeof(szLength[0]));
            // Load width string.
            LoadString(hResInst, 1901 /* width start ID */ + i, szWidth, 
                                            sizeof(szWidth)/sizeof(szWidth[0]));
            // Load height string.
            LoadString(hResInst, 2001 /* height start ID */ + i, szHeight, 
                                          sizeof(szHeight)/sizeof(szHeight[0]));
            // Load weight string.
            LoadString(hResInst, 2101 /* weight start ID */ + i, szWeight, 
                                          sizeof(szWeight)/sizeof(szWeight[0]));
            // Load clearance string.
            LoadString(hResInst, 2201 /* clearance start ID */ + i, szClearance, 
                                    sizeof(szClearance)/sizeof(szClearance[0]));
            // Load country string.
            LoadString(hResInst, 2300 /* country ID (same for car MSS) */, 
                             szCountry, sizeof(szCountry)/sizeof(szCountry[0]));
            // Load tank volume string.
            LoadString(hResInst, 2401 /* tank volume start ID */ + i, 
                    szTankVolume, sizeof(szTankVolume)/sizeof(szTankVolume[0]));

            // Unlike when making MSS from original files m_vecImages should be 
            // filled with data (titles, XML data).
            m_ImageMap.m_strFileName = szFileName;
            m_ImageMap.m_strTitle = szTitle; 
            m_ImageMap.m_strYear = szYear;
            m_ImageMap.m_strMake = szMake;
            m_ImageMap.m_strModel = szModel;
            m_ImageMap.m_strSubmodel = szSubmodel;
            m_ImageMap.m_strType = szType;
            m_ImageMap.m_strPrice = szPrice;
            m_ImageMap.m_strPower = szPower;
            m_ImageMap.m_strFuel = szFuel;
            m_ImageMap.m_strLength = szLength;
            m_ImageMap.m_strWidth = szWidth;
            m_ImageMap.m_strHeight = szHeight;
            m_ImageMap.m_strWeight = szWeight;
            m_ImageMap.m_strClearance = szClearance;
            m_ImageMap.m_strCountry = szCountry;
            m_ImageMap.m_strTankVolume = szTankVolume;
            m_ImageMap.bDeletionMarked = FALSE;
            m_ImageMap.dwImageSize = dwImageSize;
        }
        //m_vecImages.push_back(m_ImageMap);
        
        // Get image size. TODO: move to function.
        m_Picture[i].m_spIPicture->get_Width(&hmWidth);  
        m_Picture[i].m_spIPicture->get_Height(&hmHeight);
        m_Picture[i].Free();
        sizeImage.cx = hmWidth; sizeImage.cy = hmHeight;
        dc.HIMETRICtoDP(&sizeImage);
        sprintf(szFileDimensions, "%dx%d", sizeImage.cx, sizeImage.cy);
        sprintf(szFileSize, "%d", dwImageSize);

        m_ImageMap.iPermanentIndex = i;
        m_ImageMap.m_strImageSize_Bytes = szFileSize;
        m_ImageMap.m_strImageSize_Pixels = szFileDimensions;
        m_vecImages.push_back(m_ImageMap);

        InsertListViewItemFromMSS(i, szFileName, szFileSize, szFileDimensions); 
        m_Title[i] = szTitle; 
    };

    FreeLibrary(hResInst);

    return TRUE;
}

void  CMySaverPropPage1::ResetListViewItems()
{
    m_ctlListView.DeleteAllItems(); 

    CString strFullFileName;
    CFile file;
    CPicture m_Picture;
    long hmWidth,hmHeight; // HIMETRIC units

    TCHAR szFileSize[50];
    TCHAR szFileDimensions[20];

    CWnd *pWnd = CWnd::FromHandle(::GetDesktopWindow());
    CClientDC dc(pWnd);

    for( int i = 0; i < m_vecImages.size(); i++ )
    {
        strFullFileName = m_vecImages.at(i).m_strFullFileName;
        file.Open(strFullFileName, CFile::modeRead);
        m_Picture.Load(file);
        m_Picture.m_spIPicture->get_Width(&hmWidth);  
        m_Picture.m_spIPicture->get_Height(&hmHeight);
        CSize sizeImage(hmWidth,hmHeight); dc.HIMETRICtoDP(&sizeImage);
        wsprintf(szFileSize, "%d", file.GetLength());
        wsprintf(szFileDimensions, "%dx%d", sizeImage.cx, sizeImage.cy);
        file.Close();

        InsertListViewItem2(i, m_vecImages.at(i).m_strFileName, szFileSize, 
                                                              szFileDimensions);
    }
}

void  CMySaverPropPage1::ResetListViewItems_EditMode()
{
    m_ctlListView.DeleteAllItems(); 

    CString strFileName;
    CString strFileSize_Bytes;
    CString strFileSize_Pixels;

    for( int i = 0; i < m_vecImages.size(); i++ )
    {
        strFileName = m_vecImages.at(i).m_strFileName;
        strFileSize_Bytes = m_vecImages.at(i).m_strImageSize_Bytes;
        strFileSize_Pixels = m_vecImages.at(i).m_strImageSize_Pixels;
        InsertListViewItemFromMSS(i, strFileName, strFileSize_Bytes, 
                                                            strFileSize_Pixels); 
    }
}

void CMySaverPropPage1::InsertListViewItem(int nItem, CString strFileName, 
                                 LPCTSTR pszFileSize, LPCTSTR pszFileDimentions)
{
    CImageMap m_ImageMap; 
    TCHAR szFullFileName[MAX_PATH];

    wsprintf(szFullFileName, "%s\\%s", m_strInitImages, strFileName);
    
    m_ctlListView.InsertItem(nItem, strFileName);
    m_ctlListView.SetItemText(nItem, 1, ConvertToKB(pszFileSize));
    m_ctlListView.SetItemText(nItem, 2, pszFileDimentions);

    if (m_vecImages.size() < 50)
    {
        m_ImageMap.m_strFileName = strFileName;
        m_ImageMap.m_strFullFileName = szFullFileName;
        m_ImageMap.m_strTitle = ""; 
        m_ImageMap.dwImageSize = atoi(pszFileSize); 
        m_vecImages.push_back(m_ImageMap); 
    }
    else
    {
        m_strStatusMessage = "The number of images exceeds 50. To keep screen ";
        m_strStatusMessage += "saver size reasonable the image list will be ";
        m_strStatusMessage += "truncated!";
    }
}

// Used only in new MSS mode when add image button clicked (or drog-and-drop 
// used). It stores full file name of the added file in CImageMap along with 
// file name.
void CMySaverPropPage1::InsertListViewItem3(int nItem, CString strFileName, 
        CString strFullFileName, LPCTSTR pszFileSize, LPCTSTR pszFileDimentions)
{
    CImageMap m_ImageMap; 
    
    m_ctlListView.InsertItem(nItem, strFileName);
    m_ctlListView.SetItemText(nItem, 1, ConvertToKB(pszFileSize));
    m_ctlListView.SetItemText(nItem, 2, pszFileDimentions);

    if (m_vecImages.size() < 50)
    {
        m_ImageMap.m_strFileName = strFileName;
        m_ImageMap.m_strFullFileName = strFullFileName;
        m_ImageMap.m_strTitle = ""; 
        m_vecImages.push_back(m_ImageMap); 
    }
    else
    {
        m_strStatusMessage = "The number of images exceeds 50. To keep screen ";
        m_strStatusMessage += "saver size reasonable the image list will be ";
        m_strStatusMessage += "truncated!";
    }
}

// TODO: Merge with InsertListViewItemFrom.
void CMySaverPropPage1::InsertListViewItemFromMSS(int nItem, 
            CString strFileName, LPCTSTR pszFileSize, LPCTSTR pszFileDimentions)
{
    // Without "nItem" index new items are always inserted at the beginning
    // messing up the order.
    m_ctlListView.InsertItem(nItem, strFileName);
    m_ctlListView.SetItemText(nItem, 1, ConvertToKB(pszFileSize));
    m_ctlListView.SetItemText(nItem, 2, pszFileDimentions);
}

//------------------------------------------------------------------------------
// InsertListViewItem2 used only when items re-ordered after accepting
// changes in Picture Detailes dialog.
//------------------------------------------------------------------------------

void CMySaverPropPage1::InsertListViewItem2(int nItem, CString strFileName,
                                   TCHAR* pszFileSize, TCHAR* pszFileDimentions)
{
    m_ctlListView.InsertItem(nItem, strFileName);
    m_ctlListView.SetItemText(nItem, 1, ConvertToKB(pszFileSize));
    m_ctlListView.SetItemText(nItem, 2, pszFileDimentions);
}

CString CMySaverPropPage1::ConvertToKB(LPCTSTR pszFileSize)
{
    int iFileSize = atoi(pszFileSize);
    double fFileSizeKB = ((double)iFileSize)/1024.;

    TCHAR szFileSizeKB[10];
    sprintf(szFileSizeKB, "%.2f", fFileSizeKB); //wsprintf shows "f"!

    return szFileSizeKB;
}

CString CMySaverPropPage1::ConvertToKBorMB(int iFileSize)
{
    double fFileSizeKB;
    double fFileSizeMB;
    TCHAR szFileSizeKB[10];
    TCHAR szFileSizeMB[10];

    fFileSizeKB	= ((double)iFileSize)/1024.;
    if (fFileSizeKB <= 1024.)
    {
        sprintf(szFileSizeKB, "%.2f KB", fFileSizeKB); //wsprintf shows "f"!
        return szFileSizeKB;
    }
    else
    {
        fFileSizeMB	= fFileSizeKB/1024.;
        sprintf(szFileSizeMB, "%.2f MB", fFileSizeMB); 
        return szFileSizeMB;
    }
}

LRESULT CMySaverPropPage1::OnWizardNext()
{
    if (theApp.m_bEnableDebug)
    {
        theApp.App_Debug_FunctionCallTrace("CMySaverPropPage1::OnWizardNext()");
    }
    
    DWORD dwSizeOfMSS;
    MEMORYSTATUS ms;
    float fRAMUsageOfMSS;
    CString strWarning; 
    CString strMsg;
    
    // Update class data from dialog data.
    UpdateData(TRUE);

    //if(m_vecImageFileNames.size() == 0)
    if (m_vecImages.size() == 0)
    {
        CString strMessage;
        strMessage =  "No images found to process. Please go back to the ";
        strMessage += "previous page to select images!";
        
        ::MessageBox(NULL, strMessage, 
                           "MyScreenSaver Builder 2.2", MB_OK | MB_ICONWARNING);

        return -1; // keep dialog active
    }
    else
    {
        if (!m_strStatusMessage.IsEmpty())
        {
            ::MessageBox(NULL, m_strStatusMessage, 
                           "MyScreenSaver Builder 2.2", MB_OK | MB_ICONWARNING);
        }
    }

    // Get get sum of picture sizes and prompt warning if it is more than 10%
    // (m_fAcceptableRAMUsage) of total RAM.
    strMsg = "The total size of images %s is more than %.2f%% of total RAM ";
    strMsg += "size - %s.     \r\nThe screen saver might be running too ";
    strMsg += "slowly. It is recommended to remove some\r\n images or ";
    strMsg += "decrease image sizes. Do you want to continue?";

    // TODO: can return wrong values for RAM > 4GB (MEMORYSTATUSEX/
    // GlobalMemoryStatusEx should be used).
    GlobalMemoryStatus(&ms); 

    // Get MSS size estimation based on the sum of image sizes.
    dwSizeOfMSS = 0;
    for (int i = 0; i < m_vecImages.size(); i++) 
    {
        dwSizeOfMSS += m_vecImages.at(i).dwImageSize;
    }

    fRAMUsageOfMSS = ((float)dwSizeOfMSS/(float)ms.dwTotalPhys)*100.;
    if (fRAMUsageOfMSS > m_fAcceptableRAMUsage)
    {
        strWarning.Format(strMsg, ConvertToKBorMB(dwSizeOfMSS), 
                               fRAMUsageOfMSS, ConvertToKBorMB(ms.dwTotalPhys));
        if (::MessageBox(0, strWarning, "MyScreenSaver Builder 2.2", 
                                            MB_YESNO | MB_ICONQUESTION) == IDNO)
        {
            return -1; // stay on the current page
        }
    }

    return 0; // go to the next page
}

BOOL CMySaverPropPage1::OnWizardFinish()
{
    CPropertySheet* psheet = NULL;
    int iPageCount;
    CMySaverPropPage2* pPP2 = NULL;
    CString strMSSFileName; ;
    CString strMSSBuildFullFileName;
    HINSTANCE hResInst;
    BOOL bRes;
    
    // If MSS data are edited there are 3 options when saving:
    // 1) There is no re-ordering or deletions in "Picture Details" dialog
    //    and no updates for images and text data. Close.
    // 2) There is no re-ordering or deletions in "Picture Details" dialog,
    //    however, there is (are) updates for images and text data. Thes
    //    updates already saved within "Picture Details" dialog. Close.
    // 3) There are either re-ordering or deletions in "Picture Details" 
    //    dialog, and might have been MSS data updates, which were already
    //    recorded.
    
    if (m_nBuildOption == 1 /* edit mode */ && 
                                  theApp.m_nEditOption == 0 /* edit MSS data */)
    {
        psheet = (CPropertySheet*) GetParent(); 
        iPageCount = ((CMySaverPropSheet*) GetParent())->GetPageCount();
        pPP2 = (CMySaverPropPage2*)((CMySaverPropSheet*) GetParent())->
                                                        GetPage(iPageCount - 1); 

        // Edit mode with MSS rebuild. Unlike PP2 progress bar is not supposed  
        // to be shown.  Notice that in edit mode, rebuild MSS occurs if there 
        // are images maked for deletion or images were re-ordered. In other 
        // cases MSS file is updated directly. 
        if (m_bIsImagesDeletionMarked == TRUE || m_bIsImagesReordered == TRUE)
        {
            // Prepare main input strings.
            pPP2->m_strInstallDir = theApp.m_strInstallDirectory;
            pPP2->m_strInitImages = m_strInitImages;
            m_strImageSetName = theApp.Utility_GetFileNameFromFullFileName(
                                m_strInitImages, FALSE /* without extension */);
            pPP2->m_strImageSetName =  
                           m_strImageSetName;  // used by EditMode_PrebuildMSS()

            // Fill out CopyData structure.
            EditMode_GetImageSetData(m_strInitImages, TRUE /* for rebuild */);
            
            if (theApp.m_bEnableDebug) 
                theApp.App_Debug_FunctionCallTrace(
                                         "StartUnpackFilesThread() started...");
            // Unpack build support files into the Build folder.
            pPP2->StartUnpackFilesThread();
            //Sleep(2000);
            // Wait until the thread that makes Build folder and extracts files
            // there self-terminates. Notice that without event object the next 
            // line of code is executed immediately after calling the tread that
            // unpacks files and this cause an errors if the file extracting is 
            // still in process.
            while (WAIT_OBJECT_0 /* object is signaled */ != 
                WaitForSingleObject(theApp.m_hBuildFolderCreatedEvent 
                       /* initially event object is not signaled */, INFINITE));

            if (theApp.m_bEnableDebug) 
                theApp.App_Debug_FunctionCallTrace(
                                            "EditMode_MakeRCFile() started...");
            // Make RC file in the Build folder.
            EditMode_MakeRCFile();
            if (theApp.m_bEnableDebug) 
                theApp.App_Debug_FunctionCallTrace(
                                           "EditMode_PrebuildMSS() started...");
            // Make MSS stub (build MSS without images) in the Build folder. 
            pPP2->EditMode_PrebuildMSS();
            // Add images to the stub according to new order or/and ommitting
            // images marked for deletion.
            if (theApp.m_bEnableDebug) 
                theApp.App_Debug_FunctionCallTrace(
                                           "EditMode_AddImagesToMSS started...");
            EditMode_AddImagesToMSS(m_strInitImages);

            // Handle build results: when successful move updated MSS file to 
            // to originnal location replacing MSS before update.
            strMSSFileName = m_strImageSetName + ".mss";
            strMSSBuildFullFileName = theApp.m_strMSBDataDirectory + 
                                                   "\\Build\\" + strMSSFileName;
                                                                 
            if (theApp.m_bEnableDebug) 
                   theApp.App_Debug_FunctionCallTrace("LoadLibrary started...");
            // Replace source with updated MSS (from temporary Build directory)
            // only if there is no error in updated MSS. The weak check is to
            // test if it is loadable.
            hResInst = LoadLibrary(strMSSBuildFullFileName); 
            if (hResInst != NULL)
            {
                // LL MoveFile does not write over existing file, so I use 
                // MoveFileEx. The MOVEFILE_WRITE_THROUGH guarantees that the
                // function does not return until actually the file is moved to
                // the destination (used in conjunction with following
                // DeleteDirectory).
                bRes = MoveFileEx(strMSSBuildFullFileName, m_strInitImages, 
                    MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH | 
                        MOVEFILE_COPY_ALLOWED /* allow copy to another disk */);

                if (theApp.m_bEnableDebug)
                {
                    CString strMsg;
                    strMsg.Format("PP1's OnWizardFinish(): %d %d %s %s", 
                        bRes, GetLastError(), strMSSBuildFullFileName, 
                                                               m_strInitImages);
                    theApp.App_Debug_FunctionCallTrace(strMsg);
                }
            }
            else
            {
                ::MessageBox(NULL, 
                    "The MSS rebuild error!", 
                                   "MyScreenSaver Builder 2.2", MB_ICONWARNING);
            }

            // Build directory was made above for the rebuild due to re-ordering 
            // or deletions. This directory must be deleted, generally.
            if (!theApp.m_bEnableDebug) 
            {
                // Supposedly, there are no subfolders in the Build directory 
                // (DeleteDirectory does not use recursion!). DeleteDirectory 
                // uses RemoveDirectory, which requires that the folder to be  
                // deleted must be empty the call. Therefore, DeleteDirectory 
                // enumerates the folder's files and removes them one-by-one.
                pPP2->DeleteDirectory(theApp.m_strMSBDataDirectory + "\\Build");
            }
        }
    }

    return TRUE;
}

void CMySaverPropPage1::OnButtonAddClicked()
{
    OPENFILENAME	ofn;
    // Without  {0} the dialog fails to open 2d time.
    TCHAR			szPictureFile[MAX_PATH] = {0};
    int iItemCount;
    CString strMSSFileName;
    CString strMessage;
    CMySaverApp* pApp =  static_cast<CMySaverApp*>(AfxGetApp());
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize			= sizeof(OPENFILENAME);
    ofn.hwndOwner			= this->m_hWnd;
    ofn.hInstance			= NULL;
    // Filter: last string terminated with 2 NULLs. 
    ofn.lpstrFilter			= _T("Image files(*.jpg; *.gif)\0*.jpg;*.gif\0\0"); 
    ofn.lpstrCustomFilter	= NULL;
    ofn.nMaxCustFilter		= 0;
    ofn.nFilterIndex		= 0;
    // Older (98/Me) ofn.lpstrInitialDir can be used to set initial directory, 
    // but ofn.lpstrFile parameter can be used instead since it serves two 
    // purposes (and supposed to be used mainly for setting initial directory 
    // with 2000/XP): set initial directory and receive selected full path name. 
    // I found that when setting initial directory ofn.lpstrFile works more 
    // stable (XP), but ofn.lpstrInitialDir can fail (FNERR_INVALIDFILENAME).
    ofn.lpstrFile			= szPictureFile; // gets selected full file path
    ofn.nMaxFile			= MAX_PATH;
    ofn.lpstrFileTitle		= NULL;
    ofn.nMaxFileTitle		= 0;
    //ofn.lpstrInitialDir     = NULL;
    ofn.lpstrTitle			= _T("Select picture");
    ofn.Flags				= OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST; 
    ofn.nFileOffset			= 0;
    ofn.nFileExtension		= 0;
    ofn.lpstrDefExt			= NULL;
    ofn.lCustData			= 0;
    ofn.lpfnHook			= NULL;
    ofn.lpTemplateName		= NULL;

    if (GetOpenFileName(&ofn))
    {
        // If file name is not already in the list view, add image.
        strMSSFileName = pApp->
            Utility_GetFileNameFromFullFileName(szPictureFile, 
                                                     TRUE /* with extension */);
        if (!IsDuplicateName(strMSSFileName))
        {
            if (m_nBuildOption == 0 /* New MSS */)
            {
                AddImage_NewMode(szPictureFile);
            }
            else if (m_nBuildOption == 1 /* Edit MSS */)
            {
                AddImage_EditMode(szPictureFile);
            }
        }
        else
        {
            strMessage.Format("The image %s is already in the list!    ", 
                                                                strMSSFileName); 
            ::MessageBox(0, strMessage,
                               "MyScreenSaver Builder 2.2", MB_ICONINFORMATION);
        }
    }
}

void CMySaverPropPage1::AddImage_EditMode(LPCTSTR pszPictureFile)
{
    CImageMap m_ImageMap;
    BOOL bResult;
    HANDLE hFile; 
    HANDLE hUpdateRes;
    TCHAR szFileName[100];
    TCHAR szFileSize[50];

    // TODO: setting to TRUE presumes that image was added OK. Handle addition
    // failure.
    theApp.m_bImagesManuallyAdded = TRUE;

    CString str; 
    str.Format("%d m_vecImages.size(): %d", m_ctlListView.GetItemCount(), 
                                                           m_vecImages.size()); 
    //::MessageBox(0, str, "AddImage_EditMode", 0);

    // TODO: LL actually no point in using m_iImageCount class variable.
    // Take into account if some update fails (m_iImageCount should not be 
    // increased).
    //m_iImageCount = m_ctlListView.GetItemCount(); <-- wrong

    // The image count is obtained with the size of m_vecImages vector, not with
    // the count of visible images in list view. This is to account for 
    // possibility of deletions and additions in one step (PP1): one or more
    // images might have been deleted from list view (Delete...) and in the
    // process one or more images are added (Add...). When images are deleted
    // rebuild is required (PP2) and since PP2 works with all images in PP1
    // (including marked as deleted) m_vecImages's image count must be used.
    m_iImageCount = m_vecImages.size();

    // Variables used for getting image size.
    CPicture spIPicture;
    CFile file;
    long hmWidth,hmHeight;
    CSize sizeImage;
    TCHAR szFileDimensions[20];
    CWnd *pWnd = CWnd::FromHandle(::GetDesktopWindow());
    CClientDC dc(pWnd);

    hFile = CreateFile(pszPictureFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    _ASSERTE(INVALID_HANDLE_VALUE != hFile);

    // get file size
    DWORD dwFileSize = GetFileSize(hFile, NULL);
    _ASSERTE(-1 != dwFileSize);

    LPVOID pvData = NULL;
    // alloc memory based on file size
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
    _ASSERTE(NULL != hGlobal);

    pvData = GlobalLock(hGlobal);
    _ASSERTE(NULL != pvData);

    DWORD dwBytesRead = 0;
    // read file and store in global memory
    BOOL bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
    _ASSERTE(FALSE != bRead);

    hUpdateRes = BeginUpdateResource(m_strInitImages, 
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
        MAKEINTRESOURCE(101 + m_iImageCount /* next image resource ID */), 
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), pvData /* data */,  
                                                            (DWORD)dwBytesRead); 
    if (!bResult) 
    { 
        AfxMessageBox("UpdateResource failed!");
    } 

    // Write changes to MSS and then close it. 
    if (!EndUpdateResource(hUpdateRes, FALSE /* make update */)) 
    { 
        ::MessageBox(0, "EndUpdateResource: Could not write changes to file!", 
                                                        "AddImage_EditMode", 0);
    }

    // Unlock, and cleanup.
    GlobalUnlock(hGlobal);
    CloseHandle(hFile);

    // Update IDS_IMAGECOUNT string resource (+1 increment). 
    TCHAR szText[10];
    wsprintf(szText, "%d", m_iImageCount + 1);
    UpdateStringResource(10 /* IDS_IMAGECOUNT */, szText /* new image count */);
    
    // Get file name from the full path name. I use parsing "full path name" 
    // with _tcstok (probably there is a shorter way, but don't have time to 
    // figure out). The last tokenized string gives the file name. TODO: move
    // to function.
    TCHAR szBuffer[MAX_PATH];
    TCHAR *pToken = NULL;
    short nIndex = 0; // unique index of a word used for ordering
    
    lstrcpy(szBuffer, pszPictureFile);
    pToken = _tcstok(szBuffer, _T("\\")); // LL does not work with LPCTSTR
    lstrcpy(szBuffer, pToken);

    while (pToken != NULL)
    {
        nIndex++;
        pToken = _tcstok(NULL, _T("\\"));
        lstrcpy(szBuffer, pToken);
    }
    lstrcpy(szFileName, szBuffer);

    // Update proper IDS_FILE_NAMEX string resource with file name of the new 
    // image (m_iImageCount is current, not incremented, image count).
    UpdateStringResource(301 + m_iImageCount /* next file name ID */, 
                                                szBuffer /* new image count */);
    // Update list view. Since item count in list view is zero-based, the new
    // item number is equal to old m_iImageCount.

    // Obtain image size. Note: in fact, I load the same image for the second 
    // time inside this function. I could probably use only one method (the two
    // methods are ReadFile and IPicture) if I had time to test UpdateResource 
    // with IPicture pointer (not pvData obtained with ReadFile). Anyway I need 
    // to use IPicture to get the size of added image.
    file.Open(pszPictureFile, CFile::modeRead|CFile::shareDenyWrite);

    CArchive ar(&file, CArchive::load | CArchive::bNoFlushOnDelete);
    CArchiveStream arcstream(&ar);

       HRESULT hr = OleLoadPicture((IStream*)&arcstream, 0, FALSE, IID_IPicture, 
                                                         (void**)&spIPicture);
    file.Close();

    // Or, I could have used m_Picture[m_iImageCount].Load(hResInst, iImageID).
    m_Picture[m_iImageCount].m_spIPicture = spIPicture;

    // Add image to m_ImageMap collection of CImageMap depending on image 
    // category.
    if (m_nPicturesCategory == 0 /* "Not specified" */)
    {
        m_ImageMap.m_strFileName = szFileName;
        m_ImageMap.m_strTitle = ""; // no title when image added from file
        m_ImageMap.bDeletionMarked = FALSE;
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {

        m_ImageMap.m_strFileName = szFileName;
        m_ImageMap.m_strTitle = ""; // no title when image added from file
        m_ImageMap.m_strYear = "";
        m_ImageMap.m_strMake = "";
        m_ImageMap.m_strModel = "";
        m_ImageMap.m_strSubmodel = "";
        m_ImageMap.m_strType = "";
        m_ImageMap.m_strPrice = "";
        m_ImageMap.m_strPower = "";
        m_ImageMap.m_strFuel = "";
        m_ImageMap.m_strLength = "";
        m_ImageMap.m_strWidth = "";
        m_ImageMap.m_strHeight = "";
        m_ImageMap.m_strWeight = "";
        m_ImageMap.m_strClearance = "";
        m_ImageMap.m_strCountry = "";
        m_ImageMap.m_strTankVolume = "";
        m_ImageMap.bDeletionMarked = FALSE;
        m_ImageMap.dwImageSize = dwFileSize; //dwImageSize;
    }
    //m_vecImages.push_back(m_ImageMap);

    // Get image size. TODO: move code to function.
    spIPicture.m_spIPicture->get_Width(&hmWidth);  
    spIPicture.m_spIPicture->get_Height(&hmHeight);
    sizeImage.cx = hmWidth; sizeImage.cy = hmHeight;
    dc.HIMETRICtoDP(&sizeImage);
    sprintf(szFileDimensions, "%dx%d", sizeImage.cx, sizeImage.cy);
    spIPicture.Free();
    sprintf(szFileSize, "%d", dwFileSize);

    m_ImageMap.iPermanentIndex = m_iImageCount;
    m_ImageMap.m_strImageSize_Bytes = szFileSize;
    m_ImageMap.m_strImageSize_Pixels = szFileDimensions;
    m_vecImages.push_back(m_ImageMap);

    // Add to the list in the list view.
    InsertListViewItemFromMSS(m_ctlListView.GetItemCount(), szFileName, 
                                                  szFileSize, szFileDimensions);
    // Refresh list view to keep rows interlaced.
    m_ctlListView.Invalidate();
    m_ctlListView.UpdateWindow();
}

void CMySaverPropPage1::UpdateStringResource(int iStringID, 
                       LPCTSTR pszUpdateString, BOOL bUseBuildDir /* = FALSE */)
{
    CString strMSSFile;
    CString strImageSetName;
    BOOL bResult; 
    HINSTANCE hInstMSS; 
    short nBlockID;
    short nBlockOffset;
    HRSRC hRsrc;
    TCHAR szName[10];
    LPVOID	pResData;
    // {0} - zeros content for empty strings.
    WCHAR aStrings[16 /* number of strings in block */]
                                                 [100 /*MAX_LOADSTRING*/] = {0};
    WCHAR szWideString[100];
    //short i, j;
    short j;
    WCHAR*	pParse = NULL;

    // Get block ID from string ID and offset within the block (coincides with 
    // zero-based array's index).
    nBlockID = (iStringID/16) + 1; // 1 for IDS_IMAGECOUNT (iStringID = 0)
    nBlockOffset = iStringID - (nBlockID - 1)*16;

    if (bUseBuildDir)
    {
        // Used only from PP2 when the password is updated with OS dialog and
        // images are reordered or deleted. In this case the MSS is updated 
        // in the build directory.
        strImageSetName = theApp.Utility_GetFileNameFromFullFileName(
                                m_strInitImages, FALSE /* without extension */);

        strMSSFile = theApp.m_strMSBDataDirectory + _T("\\Build\\") + 
                                                   strImageSetName + _T(".mss");
        //AfxMessageBox(strMSSFile);
    }
    else
    {
        strMSSFile = theApp.m_strLastEditedFile; // works also with 
                                                            // m_nEditOption = 1
    }
    
    hInstMSS = LoadLibrary(strMSSFile); 

    if (hInstMSS == NULL)
    {
        AfxMessageBox("LoadLibrary failed!");
    }

    // Longer way (instead of LoadString) to obtain a pointer to resource.
    hRsrc = FindResource(hInstMSS, MAKEINTRESOURCE(nBlockID), RT_STRING); 

    if (hRsrc == NULL)
    {
       // Skip reading block of data (there is no such block, it must be created 
       // later.
    }
    else
    {
        // Load resource into memory.
        BYTE* lpRsrc = (BYTE*)LoadResource(hInstMSS, hRsrc);
        if (hRsrc == NULL)
        {
            AfxMessageBox("FindResource failed! (E)");
        }
        // The block exists, get it and parse its values into aStrings array.
        pResData = LockResource(lpRsrc);
        // null when block does not exist:
        // if (pResData == NULL) AfxMessageBox("0"); 
        //WCHAR*	pParse = (WCHAR*)pResData;
        pParse = (WCHAR*)pResData;

        // {0} - zeros content for empty strings.
        //WCHAR aStrings[16 /* number of strings in block */]
        //                                         [100 /*MAX_LOADSTRING*/] = {0};
        //short i, j;
        short nStringLength;
            
        for (int i = 0; i < 16 /* number of strings in block */; i++ )
        {
            nStringLength = *pParse;
            if (nStringLength > 0)
            {
                pParse++; // now at the string prefix, shift to the right
                wcsncpy(aStrings[i], pParse, nStringLength); 
                aStrings[i][nStringLength] = '\0';
                pParse += nStringLength;
            }
            else
            {
                pParse++; 
            }
        }
    }

    // We have resurce strings in custom array, unload MSS.
    FreeLibrary(hInstMSS);

    // Make update in block.
    MultiByteToWideChar(CP_UTF8, 0 /* must be zero */, 
        pszUpdateString, -1 /* string NULL-terminated */,  
                                            szWideString, sizeof(szWideString)); 
    
    wcscpy(aStrings[nBlockOffset], szWideString);

    // Get required resource size. wcslen - number of charactes not including 
    //terminator. UpdateResource requires size in bytes. TODO: put into 
    // GetResSize ... (dwResSize = GetResSize(...);)
    short nSize = 0;
    for (int i = 0; i < 16 /* number of strings in block */; i++ )
    {
        nSize += 2;
        nSize += wcslen(aStrings[i]) /* WCHARS count */ * 2 /* 2 bytes - WCHARs */;
    }
    
    // Build block of strings and have poiter to this block.
    LPVOID	pRes;
    pRes = malloc(nSize);

    short nLen;
    pParse = (WCHAR*)pRes;

    for (i = 0; i < 16; i++ )
    {
        nLen = wcslen(aStrings[i]);
        *pParse = nLen;
        pParse++;
        for (j = 0; j < nLen; j++ )
        {
            *pParse = aStrings[i][j];
            pParse++;
        }
    }

    // Make actual update by updating entire block.
    HANDLE hUpdateRes = 
        BeginUpdateResource(strMSSFile, FALSE /* do not delete existing resources */); 
     
    if (hUpdateRes == NULL) 
    {
        AfxMessageBox("BeginUpdateResource failed!");
    }

    // Add the string resource (block) to the update list.
    // MSDN: "Each call to UpdateResource contributes to an internal list of 
    // additions, deletions, and replacements but does not actually write the 
    // data to the executable file. 
    bResult = UpdateResource(hUpdateRes, RT_STRING, MAKEINTRESOURCE(nBlockID),
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), pRes /* data */,  
                                                                  (DWORD)nSize); 
    if (!bResult) 
    { 
        AfxMessageBox("UpdateResource failed!");
    } 

    // Write changes to MSS and then close it. 
    if (!EndUpdateResource(hUpdateRes, FALSE /* make update */)) 
    { 
        ::MessageBox(0, "EndUpdateResource: Could not write changes to file!", 
                                                     "UpdateStringResource", 0);
    } 
}

void CMySaverPropPage1::OnButtonDeleteClicked()
{
    CPropertySheet* psheet = NULL;
    int iSelectedItem;
    CString strFileName;
    int i;
    m_bIsImagesDeletionMarked = TRUE;

    // List view does not have a specific function to get selected item. The
    // wolkaround is to use GetNextItem.
    iSelectedItem = m_ctlListView.GetNextItem(-1 /* find first selected item */, 
                                                                 LVNI_SELECTED);
    if (iSelectedItem  == -1 /* no item selected */)
    {
        // Space added to center text.
        ::MessageBox(0, "             No item is selected!", 
                                                "MyScreenSaver Builder 2.2", 0);
    }
    else
    {
        // The image set file must have at least one image. Do nothing if the
        // number of images in image set is one.
        if (m_ctlListView.GetItemCount() > 1)
        {
            // The index of deleted item in list view do not coincide with its  
            // index in vector (when at least one deletion was made), and  
            // therefore GetVectorIndex function is used.
            strFileName = m_ctlListView.GetItemText(iSelectedItem, 0);

            i = GetVectorIndex(strFileName);
            m_ctlListView.DeleteItem(iSelectedItem);

            if (m_nBuildOption == 0 /* New MSS */)
            {
                // Just "i" does not work.
                m_vecImages.erase(m_vecImages.begin() + i);     
            }
            else if (m_nBuildOption == 1 /* Edit MSS */)
            {
                m_vecImages.at(i).bDeletionMarked = TRUE;
                if (theApp.m_nEditOption == 0 /* edit MSS data */)
                {
                    psheet = (CPropertySheet*) GetParent(); 
                    psheet->SetFinishText("Finish");
                }
            }

            // Refresh list view to keep rows interlaced.
            m_ctlListView.Invalidate();
            m_ctlListView.UpdateWindow();
        }
        else
        {
            ::MessageBox(0, "The image set file must have at least one image!", 
                            "The operation is aborted", MB_OK | MB_ICONWARNING);
        }
    }
}

int CMySaverPropPage1::GetVectorIndex(CString strFileName)
{
    for (int i = 0; i < m_vecImages.size(); i++)
    {
        if (lstrcmp(m_vecImages.at(i).m_strFileName, strFileName) == 0) 
            return i;
    }
}

// Use in AddImage_* to avoid adding to list view images with the same name.
BOOL CMySaverPropPage1::IsDuplicateName(CString strFileName)
{
    for (int i = 0; i < m_vecImages.size(); i++)
    {
        if (lstrcmp(m_vecImages.at(i).m_strFileName, strFileName) == 0) 
            return TRUE;
    }

    return FALSE;
}

void CMySaverPropPage1::OnButtonModifyClicked()
{
    CPropertySheet* psheet = NULL;
    int iSelectedItem;
    int iMSSImageID;
    CString strFileName;
    short nID;
    CPictureDetails m_PictureDetails; // the shortcoming of declaring class variable
                                      // at the top is that one cannot use more than
                                      // one constructor inside scopes since one 
                                      // cannot uniformly handle the result of 
                                      // m_PictureDetails.DoModal() outside these 
                                      // scopes (m_PictureDetails becomes undeclared
                                      // identifier)

    if (m_vecImages.size() == 0)
    {
        CString strMessage;
        strMessage =  "No images found. Please go back to the ";
        strMessage += "previous page to select images!";
        
        ::MessageBox(NULL, strMessage, 
                           "MyScreenSaver Builder 2.2", MB_OK | MB_ICONWARNING);
    }
    else
    {
        // List view does not have a specific function to get selected item. 
        // The walkaround is to use GetNextItem.
        iSelectedItem = m_ctlListView.GetNextItem(-1
                                 /* find first selected item */, LVNI_SELECTED);
                                                                 
        // Show picture details for the first picture (zero item) in the list if 
        // no picture is selected.
        if (iSelectedItem  == -1 /* no item selected */) iSelectedItem = 0;						 

        //CPictureDetails m_PictureDetails(m_nBuildOption, 
        // m_nPicturesCategory,
        //	   iSelectedItem, m_strInitImages, m_vecImages);

        m_PictureDetails.m_nBuildOption = m_nBuildOption; 
        m_PictureDetails.m_nPicturesCategory = m_nPicturesCategory;
        m_PictureDetails.m_iSelectedItem = iSelectedItem;
        m_PictureDetails.m_strImagesDir = m_strInitImages;
        m_PictureDetails.m_vecImages = m_vecImages;

        // Passing of m_vecImages to/from PictureDetails is enabled for:
        // 1) New MSS mode: re-ordering and setting image titles, plus XML 
        // data for cars can be set per picture.
        // 2) Edit MSS mode: all data are set immidiately (as MSS/DLL 
        // resource update, re-ordering is not implemented yet (and so 
        // m_vecImages no use with m_vecImages passing).
        switch (m_PictureDetails.DoModal())
        {
            case IDOK:
            {
                if (m_nBuildOption == 0 /* New MSS */)
                {
                    m_vecImages.clear();
                    m_vecImages = m_PictureDetails.m_vecImages;
                    ResetListViewItems();
                }
                else if (m_nBuildOption == 1 /* Edit MSS */)
                {
                    m_bIsImagesReordered = 
                                          m_PictureDetails.m_bIsImagesReordered;
                    m_vecImages.clear();
                    m_vecImages = m_PictureDetails.m_vecImages;
                    if (m_bIsImagesReordered) 
                    {
                        ResetListViewItems_EditMode();
                        if (theApp.m_nEditOption == 0 /* edit MSS data */)
                        {
                            psheet = (CPropertySheet*) GetParent(); 
                            psheet->SetFinishText("Finish");
                        }
                    }
                }
                break;
            }
            // The IDCANCEL is returned when ESC key used or close button clicked 
            // from the system menu. 
            case IDCANCEL:
                // Do nothing.
                break;
            default:
                // Do nothing.
                break;
        }
    }
}

//------------------------------------------------------------------------------
// To access CMySaverPropPage1 member function from CCustomListView the content 
// of CustomListView.h added to header, and the content of CustomListView.cpp
// is appended here.
IMPLEMENT_DYNAMIC(CCustomListView, CListCtrl)
CCustomListView::CCustomListView()
{
}

CCustomListView::~CCustomListView()
{
}

BEGIN_MESSAGE_MAP(CCustomListView, CListCtrl)
    ON_WM_DROPFILES()
    ON_WM_LBUTTONDBLCLK()
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnListViewCustomDraw)
    ON_NOTIFY(LVN_KEYDOWN, IDC_PP1_LISTVIEW, OnKeyDown)
END_MESSAGE_MAP()

void CCustomListView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // Delegate handling to OnButtonModifyClicked since it handles the same 
    // thing.
    m_pPP1->OnButtonModifyClicked();
}

void CCustomListView::OnDropFiles(HDROP hDropInfo)
{
    TCHAR szFFN[MAX_PATH];
    UINT nFilesCount;
    int i;

    // MSDN (second parameter): "If the value of the iFile parameter is 
    // 0xFFFFFFFF, DragQueryFile returns a count of the files dropped. If the 
    // value of the iFile parameter is between zero and the total number of 
    // files dropped, DragQueryFile copies the file name with the corresponding 
    // value to the buffer pointed to by the lpszFile parameter."

    // Get count of dropped files.
    nFilesCount = DragQueryFile(hDropInfo, 0xFFFFFFFF /* iFile */, 0, 0); 

    for(i = 0; i < nFilesCount; i++) 
    { 
        DragQueryFile(hDropInfo, i /* iFile */, szFFN, 
                                          sizeof(szFFN)/sizeof(TCHAR)); 
    
        if (m_pPP1->m_nBuildOption == 0 /* New MSS */)
        {
            m_pPP1->AddImage_NewMode(szFFN);
        }
        else if (m_pPP1->m_nBuildOption == 1 /* Edit MSS */)
        {
            m_pPP1->AddImage_EditMode(szFFN);
        }
    }

    DragFinish(hDropInfo);
}

void CMySaverPropPage1::AddImage_NewMode(LPCTSTR pszFullFileName)
{
    TCHAR szFileName[MAX_PATH];
    CFile file;
    CPicture m_Picture;
    long hmWidth,hmHeight; // HIMETRIC units
    HANDLE hFile; 
    DWORD dwFileSize;
    TCHAR szFileDimensions[20];
    TCHAR szFileSize[50];

    // -- begin of snippet
    // TODO: the snippet is from AddImage, move to separate function.
    // Get file name from the full path name. I use parsing "full path name" 
    // with _tcstok (probably there is a shorter way, but don't have time to 
    // figure out). The last tokenized string gives the file name. TODO: move
    // to function.
    TCHAR szBuffer[MAX_PATH];
    TCHAR *pToken = NULL;
    short nIndex = 0; // unique index of a word used for ordering
    
    lstrcpy(szBuffer, pszFullFileName);
    pToken = _tcstok(szBuffer, _T("\\")); // LL does not work with LPCTSTR
    lstrcpy(szBuffer, pToken);

    while (pToken != NULL)
    {
        nIndex++;
        pToken = _tcstok(NULL, _T("\\"));
        lstrcpy(szBuffer, pToken);
    }
    lstrcpy(szFileName, szBuffer);
    //-- end of snippet

    // Get width and height of image.
    file.Open(pszFullFileName, CFile::modeRead);
    m_Picture.Load(file);
    m_Picture.m_spIPicture->get_Width(&hmWidth);  
    m_Picture.m_spIPicture->get_Height(&hmHeight);
    file.Close();

    // Convert width and height of image. 
    CWnd *pWnd = CWnd::FromHandle(::GetDesktopWindow());
    CClientDC dc(pWnd);
    CSize sizeImage(hmWidth,hmHeight); dc.HIMETRICtoDP(&sizeImage);
    sprintf(szFileDimensions, "%dx%d", sizeImage.cx, sizeImage.cy);

    // Get size of image. Note that GetFileSize requires handle (not path), 
    // therefore open file first of all.
    hFile = CreateFile(pszFullFileName, GENERIC_READ, 0, NULL, 
                                                OPEN_EXISTING, 0, NULL);
    dwFileSize = GetFileSize(hFile, NULL);
    CloseHandle(hFile);
    wsprintf(szFileSize, "%d", dwFileSize);

    // With this insert comes m_vecImages update (internally in the function). 
    // Since list view items are zero-based the next item is item count 
    // returned by GetItemCount().
    InsertListViewItem3(m_ctlListView.GetItemCount(), 
                      szFileName, pszFullFileName, szFileSize, szFileDimensions);

    // Refresh list view to keep rows interlaced.
    m_ctlListView.Invalidate();
    m_ctlListView.UpdateWindow();
}

short CMySaverPropPage1::EditMode_GetPicturesCategory(LPCTSTR pszFullFileName)
{
    HINSTANCE hResInst;
    TCHAR szBuffer[100];
    CString strPicturesCategory;

    hResInst = LoadLibrary(pszFullFileName);

    if (hResInst == NULL)
    {
        AfxMessageBox("LoadLibrary failed!");
    }

    LoadString(hResInst, 4 /* pictures category ID */, szBuffer, 
                                          sizeof(szBuffer)/sizeof(szBuffer[0]));
    strPicturesCategory = szBuffer;

    if (strPicturesCategory == "Not specified")
    {
        return 0;
    }
    else if (strPicturesCategory == "Cars") 
    {
        return 1;
    }

    FreeLibrary(hResInst);
}

void CCustomListView::OnListViewCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVCUSTOMDRAW* pLVCD = (NMLVCUSTOMDRAW*)pNMHDR; 

    switch (pLVCD->nmcd.dwDrawStage)
    {
        case CDDS_PREPAINT:
            *pResult = CDRF_NOTIFYITEMDRAW;
            break;
        case CDDS_ITEMPREPAINT:
            if (!(pLVCD->nmcd.dwItemSpec % 2))
                // Set color for odd row (none now). 
                ; 
            else
                // Set color for even row.
                pLVCD->clrTextBk = RGB(220, 220, 220); 

            *pResult = CDRF_DODEFAULT;
            break;
    }
}

void CCustomListView::OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;

    AfxMessageBox("CCustomListView::OnKeyDown000");
    if (pLVKeyDown->wVKey == VK_DELETE)
    {
        AfxMessageBox("CCustomListView::OnKeyDown");
    }

    *pResult = 0;
}

//-- Start: move (PP2's copies) to separate class like CRebuildMSS 

BOOL CMySaverPropPage1::EditMode_MakeRCFile()
{
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
    //int iPageCount = ((CMySaverPropSheet*) GetParent())->GetPageCount();
 //   CMySaverPropPage1* pPP1 = (CMySaverPropPage1*)((CMySaverPropSheet*) 
    //	                                  GetParent())->GetPage(iPageCount - 2); 

    USHORT iCount = 0;
    USHORT iImagesCount;
    TCHAR szCount[5];
    CString strCount;

    // When MSS is rebuilt, no writing to IDR_IMAGES section yet (images 
    // inserted in MSS). The new number of images is calculated only.
    wsprintf(szCount, "%d", EditMode_GetImageCountWithoutDeletionMarked()); 
    strCount = szCount; 

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
    for( int i = 0; i < m_vecImages.size(); i++)
    {
        if (m_vecImages.at(i).bDeletionMarked == FALSE)
        {
            iCount++;
            wsprintf(szCount, "%d", iCount);
            strCount = szCount;

            strResourceLine = "IDS_TITLE" + strCount + " " + "\"" + 
                                   m_vecImages.at(i).m_strTitle + "\"" + "\r\n";
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
    for (int i = 0; i < m_vecImages.size(); i++)
    {
        if (m_vecImages.at(i).bDeletionMarked == FALSE)
        {
            iCount++;
            wsprintf(szCount, "%d", iCount);
            strCount = szCount;

            iImagesCount = m_vecImages.size();
            strResourceLine = "IDS_FILE_NAME" + strCount + " " + "\"" + 
                                m_vecImages.at(i).m_strFileName + "\"" + "\r\n";
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
                                m_vecImages.at(0).m_strYear + strQuote + "\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
                                                          
    strResourceLine = "IDS_XML_MAKE " + strQuote + 
                                m_vecImages.at(0).m_strMake + strQuote + "\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
                                                             
    strResourceLine = "IDS_XML_COUNTRY " + strQuote + 
                             m_vecImages.at(0).m_strCountry + strQuote + "\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
                                                          
    strResourceLine = "END\r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    strResourceLine = "STRINGTABLE \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    //
    strResourceLine = "BEGIN \r\n";
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);
    
    iCount = 0;
    for (int i = 0; i < m_vecImages.size(); i++)
    {
        if (m_vecImages.at(i).bDeletionMarked == FALSE)
        {
            iCount++;
            wsprintf(szCount, "%d", iCount);
            strCount = szCount;

            iImagesCount = m_vecImages.size();
            strResourceLine = "IDS_XML_MODEL" + strCount + " " + "\"" +
                                   m_vecImages.at(i).m_strModel + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_SUBMODEL" + strCount + " " + "\"" +
                                m_vecImages.at(i).m_strSubmodel + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_TYPE" + strCount + " " + "\"" +
                                    m_vecImages.at(i).m_strType + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_PRICE" + strCount + " " + "\"" +
                                   m_vecImages.at(i).m_strPrice + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_POWER" + strCount + " " + "\"" +
                                   m_vecImages.at(i).m_strPower + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_FUEL" + strCount + " " + "\"" +
                                    m_vecImages.at(i).m_strFuel + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_LENGTH" + strCount + " " + "\"" +
                                  m_vecImages.at(i).m_strLength + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_WIDTH" + strCount + " " + "\"" +
                                   m_vecImages.at(i).m_strWidth + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_HEIGHT" + strCount + " " + "\"" +
                                  m_vecImages.at(i).m_strHeight + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_WEIGHT" + strCount + " " + "\"" +
                                  m_vecImages.at(i).m_strWeight + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_CLEARANCE" + strCount + " " + "\"" +
                               m_vecImages.at(i).m_strClearance + "\"" + "\r\n";
                                                                    
            WriteFile(hLog, strResourceLine, strlen(strResourceLine), 
                                                              &dwWritten, NULL);
            strResourceLine = "IDS_XML_TANK_VOLUME" + strCount + " " + "\"" +
                              m_vecImages.at(i).m_strTankVolume + "\"" + "\r\n";
                                                                    
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
    
    strResourceLine = "VALUE \"FileDescription\", \"Collection of Images and Descriptions\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"Comments\", \"" + m_cd.strMSSComments + "\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"FileVersion\", \"1, 0\"\r\n"; 
    WriteFile(hLog, strResourceLine, strlen(strResourceLine), &dwWritten, NULL);

    strResourceLine = "VALUE \"LegalCopyright\", \"\Copyright \251 2005-2011 MSB LLC. All rights reserved.\"\r\n"; 
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

int CMySaverPropPage1::EditMode_GetImageCountWithoutDeletionMarked()
{
    int iPageCount;

    // Get the new number of images in MSS.
    int iImageCount = 0;
    for (int i = 0; i < m_vecImages.size(); i++)
    {
        if (m_vecImages.at(i).bDeletionMarked == FALSE)
        {
            iImageCount ++;
        }
    }

    return iImageCount; 
}

void CMySaverPropPage1::EditMode_GetImageSetData(LPCTSTR pszFullFileName, 
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
        int iPageCount;
        CMySaverPropPage2* pPP2 = NULL;
        iPageCount = ((CMySaverPropSheet*) GetParent())->GetPageCount();
        pPP2 = (CMySaverPropPage2*)((CMySaverPropSheet*) GetParent())->
                                                        GetPage(iPageCount - 1);
        m_cd.strMSSComments = pPP2->GetMSSComments();
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

// This function should also work when aimages are reordered in PP1.
BOOL CMySaverPropPage1::
               EditMode_AddImagesToMSS(LPCTSTR pszFileName /* full path name */)
{
    int nOldSize;
    int nNewSize;
    int iPageCount;
    int iImageID;
    HINSTANCE hResInst;
    HRSRC hRsrc;
    TCHAR szName[10];
    CString strMSSFile;
    HANDLE hUpdateRes;
    BOOL bResult;

    // Get the current (old) and new number of images in MSS.
    nOldSize = m_vecImages.size();
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
        if (m_vecImages.at(i).bDeletionMarked != TRUE)
        {
            // Images 101 ... 105 ...
            if (!m_bIsImagesReordered)
            {
                iImageID = 101 + i;
            }
            else
            {
                iImageID = 101 + m_vecImages.at(i).iPermanentIndex;
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

// Copied from PP0 with updates (PP0->PP1).
// OnHelpInfo (WM_HELP handler) handles context help: 1) "?" context help, 2) 
// SHIFT+F1, 3) F1 (2 and 3 are the same AFAIS). 
BOOL CMySaverPropPage1::OnHelpInfo(HELPINFO* pHelpInfo)
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
        nHelpOriginator = 0; // either "?" or 

    // Get context menu map's index for the control that was clicked or has 
    // focus (based on iCtrlId value), then get corresponding contex help string, 
    // make up HH_POPUP structure and invoke context help for the control with
    // HtmlHelp.
    for (int i = 0; i < theApp.m_ContextHelp.m_nMapSizePP1; i++) 
    //for (int i = 0; i < 4; i++) 
    {
        if (pHelpInfo->iCtrlId ==  theApp.m_ContextHelp.m_MapPP1[i].nControlID) 
        { 
            memset(&ContextHelpPopup, 0, sizeof(HH_POPUP) );   
            ContextHelpPopup.cbStruct = sizeof(HH_POPUP);

            ContextHelpPopup.hinst = AfxGetInstanceHandle(); // application 
                                                             // instance
            ContextHelpPopup.idString = 
                      theApp.m_ContextHelp.m_MapPP1[i].nStringHelpID; // resource ID 

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
void CMySaverPropPage1::OnContextMenu(CWnd* pWnd, CPoint point)
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
BOOL CMySaverPropPage1::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

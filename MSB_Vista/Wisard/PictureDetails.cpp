#include "stdafx.h"
#include "MySaver.h"
#include "PictureDetails.h"
#include "RegisterProduct.h"

#include <fstream> // ifstream
#include <string>

IMPLEMENT_DYNAMIC(CPictureDetails, CDialog)

CPictureDetails::CPictureDetails(CWnd* pParent /*=NULL*/)
: CDialog(CPictureDetails::IDD, pParent), m_strEdit(_T(""))
{
    m_pPreviewWnd = NULL;
}

CPictureDetails::CPictureDetails(short nBuildOption, short  nPicturesCategory, 
    int iSelectedItem, CString strImagesDir, vector<CImageMap> vec, 
               CWnd* pParent /*=NULL*/) : CDialog(CPictureDetails::IDD, pParent)
{
    m_nBuildOption = nBuildOption;
    m_nPicturesCategory = nPicturesCategory;
    m_iSelectedItem = iSelectedItem;
    m_strImagesDir = strImagesDir;  
    m_vecImages = vec; 
    m_pPreviewWnd = NULL;
}

CPictureDetails::~CPictureDetails(){}

CPreviewWnd::CPreviewWnd(int iImageIndex, CString strImagesDir, 
                                                          vector<CImageMap> vec)
{
    CString strFullFileName; 
    strFullFileName	= vec.at(iImageIndex).m_strFullFileName;

    // The m_nBuildOption is persisted as class variable in the application 
    // class in PP0's OnWizardNext (in addition to persisting in registry).
    if (theApp.m_nBuildOption == 0 /* New MSS */)
    {
        m_Thumbnail.Load(strFullFileName);
    }
    else if (theApp.m_nBuildOption == 1 /* Edit MSS */)
    {
        m_Thumbnail.LoadPictureFromMSS(strImagesDir, iImageIndex); 
    }
}

CPreviewWnd::~CPreviewWnd()
{
}

void CPreviewWnd::OnPaint() 
{
    CPaintDC dc(this); // device context for painting
    //--------------------------------------------------------------------------

    CRect rect;
    GetClientRect(rect); 

    CBrush brush(RGB(0,0,0));
    dc.FillRect(&rect, &brush);

    CDC* pDC = &dc;

    //--------------------------------------------------------------------------
    // Get picture size: IPicture is using metric units (HIMETRIC units) 
    //                   (1 himetric unit = 0.01 mm)
    //--------------------------------------------------------------------------
    
    long hmWidth,hmHeight; // HIMETRIC units
    
    m_Thumbnail.m_spIPicture->get_Width(&hmWidth);  
    m_Thumbnail.m_spIPicture->get_Height(&hmHeight);

    CSize szImage(hmWidth,hmHeight);
    pDC->HIMETRICtoDP(&szImage);

    // Show picture small preview window
    int iPreviewLeftIndent, iPreviewTopIndent;
    int iPreviewWidth, iPreviewHeight;
    int iPreviewBMPWidth, iPreviewBMPHeight; //not given, calculated here

    iPreviewWidth = rect.right-rect.left;   
    iPreviewHeight = rect.bottom-rect.top;  

    // Calculate the factor of image comressing
    float fScale  = ((float)(rect.right-rect.left))/((float)::GetSystemMetrics(SM_CXSCREEN));

    // Calculate, then, the size of compressed bitmap in preview window
    iPreviewBMPWidth  = (int)(fScale*szImage.cx);
    iPreviewBMPHeight = (int)(fScale*szImage.cy);

    iPreviewLeftIndent = int((iPreviewWidth  - iPreviewBMPWidth )/2);
    iPreviewTopIndent  = int((iPreviewHeight - iPreviewBMPHeight)/2);

    long hmPreviewWidth,hmPreviewHeight;

    m_Thumbnail.m_spIPicture->get_Width(&hmWidth);
    m_Thumbnail.m_spIPicture->get_Height(&hmHeight);

    hmPreviewWidth  = (long)(fScale*hmWidth);
    hmPreviewHeight = (long)(fScale*hmHeight);

    // Now draw picture (Render means draw)
    m_Thumbnail.m_spIPicture->Render(*pDC, 
                                     iPreviewLeftIndent,  
                                     iPreviewTopIndent, 
                                     iPreviewBMPWidth ,  
                                     iPreviewBMPHeight, 
                                     0,
                                     hmHeight,
                                     hmWidth,
                                     -hmHeight, 
                                     NULL);   
}

void CPictureDetails::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PH_LIST, m_ctlListBox);
    DDX_Text(pDX, IDC_EDIT_TITLE, m_strEdit);
    DDX_Control(pDX, IDC_EDIT_TITLE, m_ctlEdit);
    DDX_Control(pDX, IDC_PH_BUTTON_FROMXML, m_ctlLoadFromXMLButton);
}

BEGIN_MESSAGE_MAP(CPictureDetails, CDialog)
    ON_LBN_SELCHANGE(IDC_PH_LIST, OnClickListBox)
    ON_BN_CLICKED(IDC_PH_BUTTON_MOVEUP, OnMoveUpClicked)
    ON_BN_CLICKED(IDC_PH_BUTTON_MOVEDOWN, OnMoveDownClicked)
    ON_BN_CLICKED(IDC_PH_BUTTON_SAVE, OnSaveClicked)
    ON_BN_CLICKED(IDC_PH_BUTTON_CLOSE, OnCloseClicked)
    ON_BN_CLICKED(IDC_PH_BUTTON_SAVECLOSE, OnSaveCloseClicked)
    ON_BN_CLICKED(IDC_PH_BUTTON_RESIZE, OnResize)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_POWER, OnEditChange_Power)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_FUEL, OnEditChange_Fuel)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_YEAR, OnEditChange_Year)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_MAKE, OnEditChange_Make)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_MODEL, OnEditChange_Model)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_SUBMODEL, OnEditChange_Submodel)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_TYPE, OnEditChange_Type)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_PRICE, OnEditChange_Price)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_LENGTH, OnEditChange_Length)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_WIDTH, OnEditChange_Width)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_HEIGHT, OnEditChange_Height)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_WEIGHT, OnEditChange_Weight)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_CLEARANCE, OnEditChange_Clearance)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_COUNTRY, OnEditChange_Country)
    ON_EN_CHANGE(IDC_PH_EDIT_XML_TANK_VOLUME, OnEditChange_TankVolume)
    ON_EN_CHANGE(IDC_EDIT_TITLE, OnEditChange_Title)
    ON_BN_CLICKED(IDC_PH_BUTTON_CHANGE_PICTURE, OnButtonChangePictureClicked)
    ON_BN_CLICKED(IDC_PH_BUTTON_FROMXML, OnButtonLoadFromXML)
    ON_WM_HELPINFO()
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CPreviewWnd, CWnd)
    ON_WM_PAINT()
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

BOOL CPictureDetails::OnInitDialog() 
{
    CDialog::OnInitDialog();
    //--------------------------------------------------------------------------

    CRegisterProduct m_RP;
    BOOL bRegisteredAdminUser = FALSE;
    int iImageMapIndex;
    RECT rc;

    if (m_vecImages.size() > 0)
    {
        for ( int i = 0; i <  m_vecImages.size(); i++)
        {
            // In the edit mode do not show in the list box images marked for
            // deletion in PP1.
            if (m_vecImages.at(i).bDeletionMarked != TRUE)
                        m_ctlListBox.AddString(m_vecImages.at(i).m_strFileName);
        }
    }

    m_ctlListBox.SetCurSel(m_iSelectedItem);

    // Store list box current selection in class variable m_strImageName. It 
    // serves enable saving form data for previously selected item when the user 
    // made changes in the form,did not save changes, and and clicked another 
    // list box item. The class variable also serves to enable rollback in the 
    // same sequence.
    m_ctlListBox.GetText(m_ctlListBox.GetCurSel(), m_strImageName);
    iImageMapIndex = GetImageMapIndex(m_strImageName);

    // Show text in edit field corresponding to selected element in the list.
    m_strEdit = m_vecImages.at(iImageMapIndex).m_strTitle; 

    m_iMSSImageID = GetImageMapPermanentIndex(m_strImageName);
    // TODO: don't use m_iMSSImageID in "New" mode, probably use 
    // CreatePreviewWindow with parameter to avoid confusing variable names.
    // In "New mode" m_iMSSImageID does not have a sense, use iImageMapIndex.
    if (m_nBuildOption == 0 /* New */) m_iMSSImageID = iImageMapIndex;
    CreatePreviewWindow();
    // TODO: LL redundant (set by TAB order).
    m_ctlEdit.SetFocus();

    m_iOriginalHeight = 315; 
    m_bIsImagesReordered = FALSE;
    // The flag indicates that the form data were saved once or more times 
    // with Save button.
    m_bFormDataSaved = FALSE;

    // Set dirty flag for XML data when images. Set dirty flags for all items
    // even if MSS is not car MSS (in this case flags will be ignored). The 
    // dirty flags enable to update only data which were edited (EN_CHANGE 
    // message) in dialog's fields.
    m_bTitleDirty = FALSE; 
    m_bImageDirty = FALSE;
    m_bYearDirty = FALSE;
    m_bMakeDirty = FALSE;
    m_bModelDirty = FALSE;
    m_bSubmodelDirty = FALSE;
    m_bTypeDirty = FALSE;
    m_bPriceDirty = FALSE;
    m_bPowerDirty = FALSE;
    m_bFuelDirty = FALSE;
    m_bLengthDirty = FALSE;
    m_bWidthDirty = FALSE;
    m_bHeightDirty = FALSE;
    m_bClearanceDirty = FALSE;
    m_bWeightDirty = FALSE;
    m_bTankVolumeDirty = FALSE;
    m_bCountryDirty = FALSE;

    // Disable Save and Save & Close button since no dirty fields yet.
    GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(FALSE);
    GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(FALSE);

    if (m_nBuildOption == 0 /* New MSS mode */)
    {
        if (m_nPicturesCategory == 0 /* Not specified (default) */)
        {
            GetDlgItem(IDC_PH_BUTTON_RESIZE)->ShowWindow(FALSE);

            // Position window (Win32 code, in part, for migration later to 
            // Win32 wizard).
            ::GetWindowRect(this->m_hWnd, &rc);
            ::SetWindowPos(this->m_hWnd, NULL, 0, 0, rc.right - rc.left, 
                                  m_iOriginalHeight, SWP_NOZORDER | SWP_NOMOVE);
        }
        else if (m_nPicturesCategory == 1 /* cars */)
        {
            // Do not expand XML data entry dialog by default in this mode.
            ::GetWindowRect(this->m_hWnd, &rc);
            ::SetWindowPos(this->m_hWnd, NULL, 0, 0, rc.right - rc.left, 
                                  m_iOriginalHeight, SWP_NOZORDER | SWP_NOMOVE);
            //::SetWindowPos(this->m_hWnd, NULL, 0, 0, rc.right - rc.left, 650, 
            //										 SWP_NOZORDER | SWP_NOMOVE);
            //m_bSuppressDirtyFlagsUpdate = FALSE;
            // There is no harm when to show empty data when "Picture Details" 
            // is run for the first time. It makes sense (rare occasion) if the 
            // data were updated and "Picture Details" run again.
            ShowXMLData(iImageMapIndex);
        }
    }
    else if (m_nBuildOption == 1 /* Edit mode */)
    {
        if (m_nPicturesCategory == 0 /* Not specified */)
        {
            GetDlgItem(IDC_PH_BUTTON_RESIZE)->ShowWindow(FALSE);
            
            // Position window. 
            ::GetWindowRect(this->m_hWnd, &rc);
            ::SetWindowPos(this->m_hWnd, NULL, 0, 0, rc.right - rc.left, 
                                  m_iOriginalHeight, SWP_NOZORDER | SWP_NOMOVE);
        }
        else if (m_nPicturesCategory == 1 /* cars */)
        {
            // Expand XML data entry dialog by default, and position it at the 
            // center.
            ::GetWindowRect(this->m_hWnd, &rc);
            ::SetWindowPos(this->m_hWnd, NULL, 0, 0, rc.right - rc.left, 650, 
                                                     SWP_NOZORDER | SWP_NOMOVE);
            ::SetDlgItemText(this->m_hWnd, IDC_PH_BUTTON_RESIZE, "Less");

            // Initialize dialog's fields (XML data) by simulating a click on 
            // the list box (SetCurSel alone does not "click"). The form dirty 
            // flags must be set to FALSE before calling the function (since
            // OnClickListBox() verifies these flags at the beginning).
            //OnClickListBox(); TODO: update commment.
            ShowXMLData(iImageMapIndex);
        }
    }

    // Show button to populate fields from XML file only for registered Admin 
    // user. When MSS is not car MSS the button is not visible anyway.
    bRegisteredAdminUser = m_RP.IsRegistered() && 
                                    (theApp.m_strUser == theApp.m_strAdminUser);
    if (bRegisteredAdminUser) 
    {
        m_ctlLoadFromXMLButton.ShowWindow(TRUE);
    }

    CenterWindow();
    UpdateData(FALSE);

    return TRUE;  
}

void CPictureDetails::CreatePreviewWindow()
{
    //-------------------------------------------------------------------------
    // Remove previous preview if exists (if removed with DestroyWindow,
    // parent configuration dialog is also destroyed!)
    //--------------------------------------------------------------------------
    
    if(m_pPreviewWnd != NULL) delete m_pPreviewWnd;
    
    CWnd* pParent = (CWnd*)this;
    m_pPreviewWnd = new CPreviewWnd(m_iMSSImageID, m_strImagesDir, 
                                                                   m_vecImages);
    m_pPreviewWnd->m_pPD = this;
    CRect rect;
    pParent->GetClientRect(&rect);

    rect.left   = 25; 
    rect.top    = 52; 
    rect.bottom = 180; 
    rect.right  = rect.right - 320; 

    // Preview window ID is added for context help.
    m_pPreviewWnd->Create(NULL, WS_VISIBLE | WS_CHILD | WS_DLGFRAME, rect, 
                                                pParent, IDC_PH_PREVIEW_WINDOW);
}

BOOL CPreviewWnd::Create(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, 
                           CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
    CString strWndClass; 
    // TODO: remove cursor (LL no need).
    strWndClass	= AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | 
        CS_DBLCLKS, ::LoadCursor(AfxGetResourceHandle(),
                                                 MAKEINTRESOURCE(IDC_POINTER))); 
    CreateEx(dwExStyle, strWndClass, "dada", dwStyle, rect.left, rect.top, 
        rect.right - rect.left, rect.bottom - rect.top,  
                                        pParentWnd->GetSafeHwnd(), NULL, NULL );
    return 1;
}

BOOL CThumbnail::Load(LPCTSTR pszPathName)
{
    CFile file;
    if (!file.Open(pszPathName, CFile::modeRead|CFile::shareDenyWrite))
        return FALSE;

    CArchive ar(&file, CArchive::load | CArchive::bNoFlushOnDelete);
    CArchiveStream arcstream(&ar);

       HRESULT hr = OleLoadPicture((IStream*)&arcstream, 
                                0, 
                                FALSE,
                                IID_IPicture, 
                                (void**)&m_spIPicture);

    file.Close();
    return TRUE; //update!
}

void CPictureDetails::OnClickListBox()
{
    CString strImageName;
    int iImageIndex;
    int iRes;
    CString strMsg;

       if (IsFormDataChanged()) // form data are image and text data 
    {
        strMsg = "Do you want to save the changes you made to the form?";
        //m_strImagesDir
        iRes = ::MessageBox(0, strMsg, "Image Set File",
                                           MB_YESNOCANCEL | MB_ICONEXCLAMATION);
        
        if (iRes == IDYES) // IDOK has different ID
        {
            SaveXMLData(m_strImageName);
            if (m_bIsImagesReordered) ReorderVectorImages();
        }
        else if (iRes == IDNO)
        {
            // Clear the flags and do nothing (follow the next procedures).
            m_bTitleDirty = FALSE; 
            m_bImageDirty = FALSE;
            m_bYearDirty = FALSE;
            m_bMakeDirty = FALSE;
            m_bModelDirty = FALSE;
            m_bSubmodelDirty = FALSE;
            m_bTypeDirty = FALSE;
            m_bPriceDirty = FALSE;
            m_bPowerDirty = FALSE;
            m_bFuelDirty = FALSE;
            m_bLengthDirty = FALSE;
            m_bWidthDirty = FALSE;
            m_bHeightDirty = FALSE;
            m_bClearanceDirty = FALSE;
            m_bWeightDirty = FALSE;
            m_bTankVolumeDirty = FALSE;
            m_bCountryDirty = FALSE;
        }
        else if (iRes == IDCANCEL)
        {
            // Discard click, stay on the same page.
            iRes = m_ctlListBox.FindString(-1 /* start from the begining */, 
                                                                m_strImageName);
            if (iRes != LB_ERR) m_ctlListBox.SetCurSel(iRes);

            goto discard_changes;
        }
    }

    // When form is dirty, the class variable m_strImageName serves:
    // 1) To know what list box's item that was selected before the new  
    //    selection was made since we need to save data for previously selected  
    //    item (when its data are dirty and the user did not respond with cancel  
    //    to MessageBox options). Notice that once item is clicked list box's
    //    SetCurSel returns newly selected item and this means that previous
    //    selection must be stored somwhere.
    // 2) To rollback click on another list box item if the user responds with 
    //    "Cancel" to MessageBox options. Since the user did not answered with 
    //    Cancel the m_strImageName there is no need to rollback and the 
    //    m_strImageName must be updated with currently selected item in the 
    //    list box. Notice that the code for m_strImageName update must be after 
    //    handling of MessageBox options.
    m_ctlListBox.GetText(m_ctlListBox.GetCurSel(), strImageName);
    m_strImageName = strImageName;

    if (m_nBuildOption == 0 /* New MSS */)
    {
        iImageIndex = GetImageMapIndex(strImageName);

        // Since m_vecImages can contain images not only from dource directory, 
        // but also images added with PP1's add button (v2.0) the image's full 
        // file name must be used individually.
        // No sense with m_iMSSImageID in "New" mode, use iImageIndex.
        m_iMSSImageID = iImageIndex;
        m_pPreviewWnd->m_Thumbnail.Load(m_vecImages.at(iImageIndex).m_strFullFileName);
    }
    else if (m_nBuildOption == 1 /* Edit MSS */)
    {
        m_pPreviewWnd->m_Thumbnail.LoadPictureFromMSS(m_strImagesDir, 
                                                    //m_ctlListBox.GetCurSel());
                                     GetImageMapPermanentIndex(m_strImageName));
    }
    m_pPreviewWnd->Invalidate();
    m_pPreviewWnd->UpdateWindow(); 

    int iImageMapIndex = GetImageMapIndex(strImageName);

    if (iImageMapIndex == -1)
    {
        AfxMessageBox("Image not found");
    }
    else
    {
        // Notice that this does not fire EN_CHANGE. 
        m_strEdit = m_vecImages.at(iImageMapIndex).m_strTitle;

        if (m_nPicturesCategory == 1 /* cars */)
        {
            // The flag is used (when images are cars) to indicate to EN_CHANGE 
            // handler that the data change in dialog fields relates to 
            // initialization (SetDlgItemText fires EN_CHANGE), not to manual 
            // change, and therefore the flags are still not "dirty".
            ShowXMLData(iImageMapIndex);
        }
    }

    m_ctlEdit.SetFocus();
    
    // Update edit field in dialog.
    UpdateData(FALSE);

    //// Lavel "discard changes".
    discard_changes: /* do nothing */;
}

void CPictureDetails::OnMoveUpClicked()
{
    //--------------------------------------------------------------------------
    // Because there are no built-in CListBox functions to move items up/down,
    // cuctom code added.
    //--------------------------------------------------------------------------

    CString strName;
    int iCurSel = m_ctlListBox.GetCurSel();

    if (iCurSel > 0)
    {
        m_ctlListBox.GetText(iCurSel, strName);
        m_ctlListBox.DeleteString(iCurSel);
        m_ctlListBox.InsertString(iCurSel-1, strName);
        m_ctlListBox.SetCurSel(iCurSel-1);
        m_ctlEdit.SetFocus();
    }

    m_bIsImagesReordered = TRUE;
    GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
    GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
}

void CPictureDetails::OnMoveDownClicked()
{
    //--------------------------------------------------------------------------
    // Because there are no built-in CListBox functions to move items up/down,
    // cuctom code added.
    //--------------------------------------------------------------------------

    CString strName;
    int iCurSel = m_ctlListBox.GetCurSel();

    if (iCurSel < m_ctlListBox.GetCount()-1)
    {
        m_ctlListBox.GetText(iCurSel, strName);
        m_ctlListBox.DeleteString(iCurSel);
        m_ctlListBox.InsertString(iCurSel+1, strName);
        m_ctlListBox.SetCurSel(iCurSel+1);
        m_ctlEdit.SetFocus();
    }

    m_bIsImagesReordered = TRUE;
    GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
    GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
}

//void CPictureDetails::OnEditKillFocus()
//{
//	// Get data from edit field.
//	UpdateData(TRUE);
//
//	CString strName;
//
//	if (m_nBuildOption == 0 /* New MSS */)
//	{
//		m_ctlListBox.GetText(m_ctlListBox.GetCurSel(), strName);
//
//		int iImageMapIndex = GetImageMapIndex(strName);
//
//		if( iImageMapIndex == -1)
//		{
//			AfxMessageBox("Image not found");
//		}
//		else
//		{
//			m_vecImages.at(iImageMapIndex).m_strTitle = m_strEdit;
//		}
//	}
//	if (m_nBuildOption == 1 /* Edit MSS */)
//	{
//       // no handling for edit mode yet
//	}
//}

// The same functionality as PP1's GetVectorIndex, which added later.
int CPictureDetails::GetImageMapIndex(CString strImageName)
{
    for(int i = 0; i < m_vecImages.size(); i++)
    {
        if (strImageName == m_vecImages.at(i).m_strFileName)
        {
            return i;
        }
    }

    return -1; // not found
}

int CPictureDetails::GetImageMapPermanentIndex(CString strImageName)
{
    for(int i = 0; i < m_vecImages.size(); i++)
    {
        if (strImageName == m_vecImages.at(i).m_strFileName)
        {
            return m_vecImages.at(i).iPermanentIndex;
        }
    }

    return -1; // not found
}

void CPictureDetails::OnSaveClicked()
{
    // Set saved flag to TRUE (used with Close button).
    m_bFormDataSaved = TRUE;

    // TODO: check is likely redundant.
    if (IsFormDataChanged()) SaveXMLData(m_strImageName);

    // Used to disable when Move Up (or Down) was used only. There is no actual
    // saving here: it is quaranteed that position saved to vector on Close 
    // (or Save & Close) since no sense to save every time.
    GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(FALSE);
    GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(FALSE);
}

// Position window (Win32 code, in part, for migration later to Win32 
// wizard).
void CPictureDetails::OnResize()
{
    RECT rc;
    TCHAR szText[20];
    
    ::GetWindowRect(this->m_hWnd, &rc);
    ::GetDlgItemText(this->m_hWnd, IDC_PH_BUTTON_RESIZE, szText, 20);

    if (lstrcmp(szText, "More") == 0 /* when equal */)
    {
        ::SetWindowPos(this->m_hWnd, NULL, 0, 0, rc.right - rc.left, 650, 
                                                     SWP_NOZORDER | SWP_NOMOVE);
        ::SetDlgItemText(this->m_hWnd, IDC_PH_BUTTON_RESIZE, "Less"); 
    }
    else
    {
        ::SetWindowPos(this->m_hWnd, NULL, 0, 0, rc.right - rc.left, 
            m_iOriginalHeight /* original height */, SWP_NOZORDER | SWP_NOMOVE);
        ::SetDlgItemText(this->m_hWnd, IDC_PH_BUTTON_RESIZE, "More"); 
    }
    
    CenterWindow();
}

void CPictureDetails::SaveXMLData(CString strImageName)
{
    CString strName, strText;
    int iImageMapIndex;
    int iImageMapPermIndex;
    int i;

    // Until rebuild is made image's iPermanentIndex in vector isn't updated 
    // with repositioning of items in the list view or reordering in m_vecImages  
    // (we are on the page with list view still). This is not the case with list 
    // box's GetCurSel() and with iImageMapIndex, which must not be used for  
    // direct updates in MSS since updates can be wrongly assigned e.g. when 
    // order of images was updated in the list view or images were reodered, 
    // Picture Details dialog closed, and opened again.

    iImageMapIndex = GetImageMapIndex(strImageName);
    iImageMapPermIndex = GetImageMapPermanentIndex(strImageName);

    // In the "New" mode don't use permanent index since it is meaningless
    // (TODO: don't use such notation substituting iImageMapPermIndex with
    // iImageMapIndex!).
    if (m_nBuildOption == 0 /* New */) iImageMapPermIndex = iImageMapIndex;

    if (iImageMapPermIndex == -1)
    {
        AfxMessageBox("Image not found");
    }
    else
    {
        // Update title.
        if (m_bTitleDirty)
        {
            GetDlgItemText(IDC_EDIT_TITLE, strText); 
            if (m_nBuildOption == 1 /* Edit mode */)
                UpdateStringResource(200 + iImageMapPermIndex + 1, strText);
            m_vecImages.at(iImageMapPermIndex).m_strTitle = strText;
            m_bTitleDirty = FALSE;
        }
        // Update image.
        if (m_bImageDirty)
        {
            if (m_nBuildOption == 1 /* Edit mode */)
                UpdateImageResource(100 + iImageMapPermIndex + 1, 
                                                        m_strImageFullFileName);
            m_vecImages.at(iImageMapPermIndex).m_strFullFileName = 
                                                         m_strImageFullFileName;
            // TODO: need update also CImageMap's: dwImageSize, m_strFullFileName, 
            // m_strImageSize_Bytes, m_strImageSize_Pixels?
            m_bImageDirty = FALSE;
        }
        // Main: update year.
        if (m_bYearDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_YEAR, strText);
            if (m_nBuildOption == 1 /* Edit mode */)
                                            UpdateStringResource(1000, strText);
            // Update year for all vector elements since the value of year is 
            // shared.
            for (i = 0; i < m_vecImages.size(); i++)
                                          m_vecImages.at(i).m_strYear = strText;
            m_bYearDirty = FALSE;
        }
        // Update make.
        if (m_bMakeDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_MAKE, strText); 
            if (m_nBuildOption == 1 /* Edit mode */)
                                            UpdateStringResource(1100, strText);
            // Update make for all vector elements since the value of make is 
            // shared.
            for (i = 0; i < m_vecImages.size(); i++)
                                          m_vecImages.at(i).m_strMake = strText;
            m_bMakeDirty = FALSE;
        }
        // Update model.
        if (m_bModelDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_MODEL, strText);
            if (m_nBuildOption == 1 /* Edit mode */)
                UpdateStringResource(1201 + iImageMapPermIndex, strText);
            m_vecImages.at(iImageMapPermIndex).m_strModel = strText;
            m_bModelDirty = FALSE;
        }
        // Update submodel.
        if (m_bSubmodelDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_SUBMODEL, strText);
            if (m_nBuildOption == 1 /* Edit mode */)
                UpdateStringResource(1301 + iImageMapPermIndex, strText);
            m_vecImages.at(iImageMapPermIndex).m_strSubmodel = strText;
            m_bSubmodelDirty = FALSE;
        }
        // Update type.
        if (m_bTypeDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_TYPE, strText);
            if (m_nBuildOption == 1 /* Edit mode */)
                UpdateStringResource(1401 + iImageMapPermIndex, strText);
            m_vecImages.at(iImageMapPermIndex).m_strType = strText;
            m_bTypeDirty = FALSE;
        }
        // Update price.
        if (m_bPriceDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_PRICE, strText);
            if (m_nBuildOption == 1 /* Edit mode */)
                UpdateStringResource(1501 + iImageMapPermIndex, strText);
            m_vecImages.at(iImageMapPermIndex).m_strPrice = strText;
            m_bPriceDirty = FALSE;
        }
        // Engine
        if (m_bPowerDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_POWER, strText);
            if (m_nBuildOption == 1 /* Edit mode */)
                UpdateStringResource(1601 + iImageMapPermIndex, strText);
            m_vecImages.at(iImageMapPermIndex).m_strPower = strText;
            m_bPowerDirty = FALSE;
        }
        // Update fuel.
        if (m_bFuelDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_FUEL, strText);
            if (m_nBuildOption == 1 /* Edit mode */)
                UpdateStringResource(1701 + iImageMapPermIndex, strText);
            m_vecImages.at(iImageMapPermIndex).m_strFuel = strText;
            m_bFuelDirty = FALSE;
        }
        // Physical dimensions: update length.
        if (m_bLengthDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_LENGTH, strText);
            if (m_nBuildOption == 1 /* Edit mode */)
                UpdateStringResource(1801 + iImageMapPermIndex, strText);
            m_vecImages.at(iImageMapPermIndex).m_strLength = strText;
            m_bLengthDirty = FALSE;
        }
        // Update width.
        if (m_bWidthDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_WIDTH, strText);
            if (m_nBuildOption == 1 /* Edit mode */)
                UpdateStringResource(1901 + iImageMapPermIndex, strText);
            m_vecImages.at(iImageMapPermIndex).m_strWidth = strText;
            m_bWidthDirty = FALSE;
        }
        // Update height.
        if (m_bHeightDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_HEIGHT, strText);
            if (m_nBuildOption == 1 /* Edit mode */)
                UpdateStringResource(2001 + iImageMapPermIndex, strText);
            m_vecImages.at(iImageMapPermIndex).m_strHeight = strText;
            m_bHeightDirty = FALSE;
        }
        // Update weight.
        if (m_bWeightDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_WEIGHT, strText);
            if (m_nBuildOption == 1 /* Edit mode */)
                UpdateStringResource(2101 + iImageMapPermIndex, strText);
            m_vecImages.at(iImageMapPermIndex).m_strWeight = strText;
            m_bWeightDirty = FALSE;
        }
        // Update clearance.
        if (m_bClearanceDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_CLEARANCE, strText);
            if (m_nBuildOption == 1 /* Edit mode */)
                UpdateStringResource(2201 + iImageMapPermIndex, strText);
            m_vecImages.at(iImageMapPermIndex).m_strClearance = strText;
            m_bClearanceDirty = FALSE;
        }
        // Misc: Update country.
        if (m_bCountryDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_COUNTRY, strText);
            if (m_nBuildOption == 1 /* Edit mode */)
                                            UpdateStringResource(2300, strText);
            // Update country for all vector elements since the value of country 
            // is shared.
            for (i = 0; i < m_vecImages.size(); i++)
                                       m_vecImages.at(i).m_strCountry = strText;
            m_bCountryDirty = FALSE;
        }
        // Update tank volume.
        if (m_bTankVolumeDirty)
        {
            GetDlgItemText(IDC_PH_EDIT_XML_TANK_VOLUME, strText);
            if (m_nBuildOption == 1 /* Edit mode */)
                UpdateStringResource(2401 + iImageMapPermIndex, strText);
            m_vecImages.at(iImageMapPermIndex).m_strTankVolume = strText;
            m_bTankVolumeDirty = FALSE;
        }

        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(FALSE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(FALSE);
    }
}

void CPictureDetails::UpdateStringResource(int iStringID, LPCTSTR pszUpdateString)
{
    CString strMSSFile; 
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
    short i, j;
    WCHAR*	pParse = NULL;

    // Get block ID from string ID and offset within the block (coincides with 
    // zero-based array's index).
    //iStringID = atoi(szStringID);
    nBlockID = (iStringID/16) + 1;
    nBlockOffset = iStringID - (nBlockID - 1)*16;
    
    strMSSFile = m_strImagesDir; 
    hInstMSS = LoadLibrary(strMSSFile); 

    if (hInstMSS == NULL)
    {
        AfxMessageBox("LoadLibrary failed!");
    }
    
    // Longer way (instead of LoadString) to obtain a pointer to resource.
    hRsrc = FindResource(hInstMSS, 
                         MAKEINTRESOURCE(nBlockID /* 13 as test */), RT_STRING); 

    if (hRsrc == NULL)
    {
        // Skip reading block of data (there is no such block, it must be created 
        // later.
    }
    else
    {
        // Load resource into memory.
        BYTE* lpRsrc = (BYTE*)LoadResource(hInstMSS, hRsrc);
        if (lpRsrc == NULL)
        {
            AfxMessageBox("FindResource failed! (B)");
        }

        // The block exists, get it and parse its values into aStrings array.
        pResData = LockResource(lpRsrc);
        pParse = (WCHAR*)pResData;
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
    // terminator. UpdateResource requires size in bytes. 
    //TODO: put into GetResSize ... (dwResSize = GetResSize(...);)
    short nSize = 0;
    for (int i = 0; i < 16 /* number of strings in block */; i++ )
    {
        nSize += 2;
        nSize += wcslen(aStrings[i]) /* WCHARS count */ * 2 /* 2 bytes - WCHARs */;
    }
    // short nArraySize = sizeof(aStrings); 
    // gives in effect 2*<number of strings>*MAX_LOADSTRING 
    // (not excludes spaces after /0)
    
    // Build block of strings and have poiter to this block.
    LPVOID	pRes;
    pRes = malloc(nSize);

    short nLen;
    pParse = (WCHAR*)pRes;

    for (i = 0; i < 16; i++ )
    {
        nLen = wcslen(aStrings[i]);
        //pParse++;
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
        BeginUpdateResource(strMSSFile, 
                                  FALSE /* do not delete existing resources */); 
     
    if (hUpdateRes == NULL) 
    {
        AfxMessageBox("BeginUpdateResource failed!");
    }
    else

    // Add the string resource (block) to the update list.
    // MSDN: "Each call to UpdateResource contributes to an internal list of 
    // additions, deletions, and replacements but does not actually write the 
    // data to the executable file. Note: it is incorrect to use 
    // MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL).
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
        AfxMessageBox("EndUpdateResource: Could not write changes to file.!");
    } 
}

BOOL CThumbnail::LoadPictureFromMSS(LPCTSTR pszFileName, short iImage)
{
    int iImageID;
    int iImageResourceStartID = 100;


    HINSTANCE hResInst = LoadLibrary(pszFileName); 

    if(hResInst == NULL)
    {
        AfxMessageBox("LoadPictureFromMSS's LoadLibrary failed!");
        return FALSE;
    }

    // Images 101 ... 105 ...
    iImageID = iImageResourceStartID + iImage + 1;
    HRSRC hRsrc;
    TCHAR szName[10];

    sprintf(szName, "#%d", iImageID);
    hRsrc = FindResource(hResInst, szName, "IMAGES"); //"#103"
    if (hRsrc == NULL)
    {
        AfxMessageBox("FindResource failed (A)!");
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
    CArchive ar(&file, CArchive::load | CArchive::bNoFlushOnDelete);
    CArchiveStream arcstream(&ar);
    HRESULT hr = OleLoadPicture((IStream*)&arcstream, 
                                0, 
                                FALSE,
                                IID_IPicture, 
                                (void**)&m_spIPicture);
    file.Close();
    FreeResource(hRsrc);
    FreeLibrary(hResInst);

    return TRUE;
}

// OnEditChange_* functions are for the sole purpose to update MSS data only 
// for dialog fields that were edited (EN_CHANGE message). This is carried out
// with the help of "dirty" flags. The flag m_bSuppressDirtyFlagsUpdate helps
// to avoid updating the state of dirty flags when the form is initialized or
// re-initialized (list box click) since the SetDlgITemText fires EN_CHANGE 
// message.

void CPictureDetails::OnEditChange_Title()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bTitleDirty = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate)
        {
            m_bTitleDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_Year()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bYearDirty = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate) 
        {
            m_bYearDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_Make()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bMakeDirty = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {	
        if (!m_bSuppressDirtyFlagsUpdate) 
        {
            m_bMakeDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_Model()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bModelDirty = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate) 
        {
            m_bModelDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_Submodel()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bSubmodelDirty = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate) 
        {
            m_bSubmodelDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_Type()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bTypeDirty = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate) 
        {
            m_bTypeDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_Price()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bPriceDirty  = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate) 
        {
            m_bPriceDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_Power()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bPowerDirty  = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate) 
        {
            m_bPowerDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_Fuel()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bFuelDirty  = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate) 
        {
            m_bFuelDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_Length()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bLengthDirty  = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate) 
        {
            m_bLengthDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_Width()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bWidthDirty  = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate) 
        {
            m_bWidthDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_Height()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bHeightDirty  = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate)
        {
            m_bHeightDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_Weight()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bWeightDirty  = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate) 
        {
            m_bWeightDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_Clearance()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bClearanceDirty  = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate)
        {
            m_bClearanceDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_Country()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bCountryDirty  = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate) 
        {
            m_bCountryDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::OnEditChange_TankVolume()
{
    if (m_nPicturesCategory == 0 /* Not specified */)
    {
        m_bTankVolumeDirty  = TRUE;
        GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
    }
    else if (m_nPicturesCategory == 1 /* Cars */)
    {
        if (!m_bSuppressDirtyFlagsUpdate)
        {
            m_bTankVolumeDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

// basedd on PP2 code.
void CPictureDetails::OnButtonChangePictureClicked()
{
    OPENFILENAME	ofn;
    // Without  {0} the dialog fails to open 2d time.
    TCHAR			szPictureFile[MAX_PATH] = {0};
    int iItemCount;
    CString strName; 
    int iImageMapIndex; 
    CString strFileName;
    CMySaverApp* pApp = NULL;
    int iCurSel;
    
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
        if (m_nBuildOption == 0 /* New MSS */)
        {
            // Store the index of current image in vector.
            iCurSel = m_ctlListBox.GetCurSel();
            m_ctlListBox.GetText(iCurSel, strName);
            iImageMapIndex = GetImageMapIndex(strName);
            
            // Update image in preview window.
            m_pPreviewWnd->m_Thumbnail.Load(szPictureFile);
            m_pPreviewWnd->Invalidate();
            m_pPreviewWnd->UpdateWindow();

            // Get file name (with extension) from full file name.
            pApp =  static_cast<CMySaverApp*>(AfxGetApp());
            strFileName = 
                pApp->Utility_GetFileNameFromFullFileName(szPictureFile, 
                                                     TRUE /* with extension */); 

            // Because there is no built-in CListBox function to update text 
            // for an list box item, I make delete and then insert.
            m_ctlListBox.DeleteString(iCurSel);
            m_ctlListBox.InsertString(iCurSel, strFileName);
            m_ctlListBox.SetCurSel(iCurSel);
            m_ctlEdit.SetFocus();

            // Update m_vecImages.
            m_vecImages.at(iImageMapIndex).m_strFileName = strFileName;
            m_vecImages.at(iImageMapIndex).m_strFullFileName = szPictureFile;
        }
        else if (m_nBuildOption == 1 /* Edit MSS */)
        {
            // TODO: if re-ordering implemented get image index not as index in 
            // list view, but from vector.
            //UpdateImageResource(100 + m_ctlListBox.GetCurSel() + 1, szPictureFile);
            m_strImageFullFileName = szPictureFile;
            m_pPreviewWnd->m_Thumbnail.Load(szPictureFile);
            m_pPreviewWnd->Invalidate();
            m_pPreviewWnd->UpdateWindow(); 
            m_bImageDirty = TRUE;
            GetDlgItem(IDC_PH_BUTTON_SAVE)->EnableWindow(TRUE);
            GetDlgItem(IDC_PH_BUTTON_SAVECLOSE)->EnableWindow(TRUE);
        }
    }
}

void CPictureDetails::UpdateImageResource(int iResourceID, LPCTSTR 
                                                                pszFullFileName)
{
    BOOL bResult;
    HANDLE hFile; 
    HANDLE hUpdateRes;

    hFile = CreateFile(pszFullFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 
                                                                          NULL);
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

    hUpdateRes = BeginUpdateResource(m_strImagesDir,
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
        MAKEINTRESOURCE(iResourceID /* next image resource ID */),
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), pvData /* data */,  
                                                            (DWORD)dwBytesRead); 
    if (!bResult) 
    { 
        AfxMessageBox("UpdateResource failed!");
    } 

    // Write changes to MSS and then close it. 
    if (!EndUpdateResource(hUpdateRes, FALSE /* make update */)) 
    { 
        AfxMessageBox("EndUpdateResource: Could not write changes to file.!");
    }

    // Unlock, and cleanup.
    GlobalUnlock(hGlobal);
    CloseHandle(hFile);
}

void CPreviewWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // Same as "Change Picture..." handler, use redirection.
    m_pPD->OnButtonChangePictureClicked();
}

// Handles close button.
void CPictureDetails::OnCloseClicked()
{
    CString strMsg;
    int iRes;
    
    if (m_bFormDataSaved)
    {
        if (m_bIsImagesReordered) ReorderVectorImages();
        
        // Tell PP1 to update vector since the data were previosly saved.
        OnOK();
    }
    else
    {
        if (IsFormDataChanged() || m_bIsImagesReordered) 
        {
            strMsg = "Do you want to save the changes you made to the form?";
            iRes = ::MessageBox(0, strMsg, "Image Set File",
                                            MB_YESNOCANCEL | MB_ICONEXCLAMATION);
            if (iRes == IDYES) // IDOK has different ID
            {
                // Save the form data and close the dialog.
                SaveXMLData(m_strImageName);
                if (m_bIsImagesReordered) ReorderVectorImages();

                OnOK();
            }
            else if (iRes == IDNO)
            {
                // Do not save the form data and close the dialog.
                CDialog::OnCancel();
            }
            else if (iRes == IDCANCEL)
            {
                // Do nothing, stay on the form.
            }
        }
        else
        {
               // Nothing to save, just close the dialog.
            CDialog::OnCancel();
        }
    }
}

// Handles close action from the system menu, and ESC.
void CPictureDetails::OnCancel()
{
    OnCloseClicked();
}

void CPictureDetails::OnSaveCloseClicked()
{
    // Set saved flag to TRUE (used with Close button).
    m_bFormDataSaved = TRUE;

    if (IsFormDataChanged()) SaveXMLData(m_strImageName);

    OnCloseClicked();
}

BOOL CPictureDetails::IsFormDataChanged()
{
    if (m_bTitleDirty || m_bImageDirty || m_bYearDirty || m_bMakeDirty  ||  
        m_bModelDirty || m_bSubmodelDirty || m_bTypeDirty || m_bPriceDirty ||  
        m_bPowerDirty || m_bFuelDirty || m_bLengthDirty || m_bWidthDirty ||  
        m_bHeightDirty || m_bClearanceDirty || m_bWeightDirty || 
                                          m_bTankVolumeDirty || m_bCountryDirty) 
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// TODO: edit mode if images were re-ordered and saved (but form is active)
// set, probably, m_bIsImagesReordered to FALSE since no sence to re-order for
// the next time when close is called.

// The code is the copy from CPictureDetails::OnSaveClicked() (v2.0 Beta 1).
void CPictureDetails::ReorderVectorImages()
{
    vector<CImageMap> m_vecImagesNew;
    CImageMap m_ImageMap;

    CString strImageName;
    CString strName;
    int iImageMapIndex;
    CString strText;

    if (m_nBuildOption == 0 /* New MSS */)
    {
        // TODO: make re-ordering only when items in list box were moved (i.e. 
        // make dirty flag for list box).
        for (int i = 0; i < m_ctlListBox.GetCount(); i++)
        {
            m_ctlListBox.GetText(i, strName);
            iImageMapIndex = GetImageMapIndex(strName);

            m_ImageMap.m_strFileName =  m_vecImages.at(iImageMapIndex).m_strFileName;
            m_ImageMap.m_strFullFileName = m_vecImages.at(iImageMapIndex).m_strFullFileName;
            m_ImageMap.m_strTitle =  m_vecImages.at(iImageMapIndex).m_strTitle;
            
            if (m_nPicturesCategory == 0 /* Not specified */)
            {
                // none specific when in this category
            }
            else if (m_nPicturesCategory == 1 /* cars */)
            {
                // Main.
                m_ImageMap.m_strYear = m_vecImages.at(iImageMapIndex).m_strYear;
                m_ImageMap.m_strMake = m_vecImages.at(iImageMapIndex).m_strMake;
                m_ImageMap.m_strModel = m_vecImages.at(iImageMapIndex).m_strModel;
                m_ImageMap.m_strSubmodel =  m_vecImages.at(iImageMapIndex).m_strSubmodel;
                m_ImageMap.m_strType =  m_vecImages.at(iImageMapIndex).m_strType;
                m_ImageMap.m_strPrice =  m_vecImages.at(iImageMapIndex).m_strPrice;
                // Engine.
                m_ImageMap.m_strPower =  m_vecImages.at(iImageMapIndex).m_strPower;
                m_ImageMap.m_strFuel = m_vecImages.at(iImageMapIndex).m_strFuel;
                // Physical dimensions.
                m_ImageMap.m_strLength = m_vecImages.at(iImageMapIndex).m_strLength;
                m_ImageMap.m_strWidth = m_vecImages.at(iImageMapIndex).m_strWidth;
                m_ImageMap.m_strHeight = m_vecImages.at(iImageMapIndex).m_strHeight;
                m_ImageMap.m_strWeight = m_vecImages.at(iImageMapIndex).m_strWeight;
                m_ImageMap.m_strClearance = m_vecImages.at(iImageMapIndex).m_strClearance;
                // Misc.
                m_ImageMap.m_strCountry = m_vecImages.at(iImageMapIndex).m_strCountry;
                m_ImageMap.m_strTankVolume = m_vecImages.at(iImageMapIndex).m_strTankVolume;

                // TODO: if OK makes also save for surrent item (now now) get XML data for 
                // current item also and update with:
                // GetDlgItemText(IDC_PH_EDIT_XML_YEAR, strText); 
                //m_ImageMap.m_strYear = strText;	
                //GetDlgItemText(IDC_PH_EDIT_XML_MAKE, strText);
                //GetDlgItemText(IDC_PH_EDIT_XML_MODEL, strText);
                //GetDlgItemText(IDC_PH_EDIT_XML_SUBMODEL, strText);
                //GetDlgItemText(IDC_PH_EDIT_XML_TYPE, strText);
                //GetDlgItemText(IDC_PH_EDIT_XML_PRICE, strText);
                //GetDlgItemText(IDC_PH_EDIT_XML_POWER, strText);
                //GetDlgItemText(IDC_PH_EDIT_XML_FUEL, strText);
                //GetDlgItemText(IDC_PH_EDIT_XML_LENGTH, strText);
                //GetDlgItemText(IDC_PH_EDIT_XML_WIDTH, strText);
                //GetDlgItemText(IDC_PH_EDIT_XML_HEIGHT, strText);
                //GetDlgItemText(IDC_PH_EDIT_XML_WEIGHT, strText);
                //GetDlgItemText(IDC_PH_EDIT_XML_CLEARANCE, strText);
                //GetDlgItemText(IDC_PH_EDIT_XML_COUNTRY, strText);
                //GetDlgItemText(IDC_PH_EDIT_XML_TANK_VOLUME, strText);
            }
            m_vecImagesNew.push_back(m_ImageMap);
        }
        m_vecImages = m_vecImagesNew;
    }
    else if (m_nBuildOption == 1 /* Edit MSS */)
    {
        if (m_bIsImagesReordered)
        {
            // TODO: initiated from fragmet in "m_nPicturesCategory == 0" section.
            // Merge.
            for (int i = 0; i < m_ctlListBox.GetCount(); i++)
            {
                m_ctlListBox.GetText(i, strName);
                iImageMapIndex = GetImageMapIndex(strName);

                m_ImageMap.m_strFileName =  m_vecImages.at(iImageMapIndex).m_strFileName;
                m_ImageMap.m_strFullFileName = m_vecImages.at(iImageMapIndex).m_strFullFileName;
                m_ImageMap.m_strTitle =  m_vecImages.at(iImageMapIndex).m_strTitle;

                // Misc utility.
                m_ImageMap.iPermanentIndex = m_vecImages.at(iImageMapIndex).iPermanentIndex;
                m_ImageMap.bDeletionMarked = m_vecImages.at(iImageMapIndex).bDeletionMarked;
                m_ImageMap.dwImageSize = m_vecImages.at(iImageMapIndex).dwImageSize;
                m_ImageMap.m_strImageSize_Bytes = m_vecImages.at(iImageMapIndex).m_strImageSize_Bytes;
                m_ImageMap.m_strImageSize_Pixels = m_vecImages.at(iImageMapIndex).m_strImageSize_Pixels;
                
                if (m_nPicturesCategory == 0 /* Not specified */)
                {
                    // none specific when in this category
                }
                else if (m_nPicturesCategory == 1 /* cars */)
                {
                    // Main.
                    m_ImageMap.m_strYear = m_vecImages.at(iImageMapIndex).m_strYear;
                    m_ImageMap.m_strMake = m_vecImages.at(iImageMapIndex).m_strMake;
                    m_ImageMap.m_strModel = m_vecImages.at(iImageMapIndex).m_strModel;
                    m_ImageMap.m_strSubmodel =  m_vecImages.at(iImageMapIndex).m_strSubmodel;
                    m_ImageMap.m_strType =  m_vecImages.at(iImageMapIndex).m_strType;
                    m_ImageMap.m_strPrice =  m_vecImages.at(iImageMapIndex).m_strPrice;
                    // Engine.
                    m_ImageMap.m_strPower =  m_vecImages.at(iImageMapIndex).m_strPower;
                    m_ImageMap.m_strFuel = m_vecImages.at(iImageMapIndex).m_strFuel;
                    // Physical dimensions.
                    m_ImageMap.m_strLength = m_vecImages.at(iImageMapIndex).m_strLength;
                    m_ImageMap.m_strWidth = m_vecImages.at(iImageMapIndex).m_strWidth;
                    m_ImageMap.m_strHeight = m_vecImages.at(iImageMapIndex).m_strHeight;
                    m_ImageMap.m_strWeight = m_vecImages.at(iImageMapIndex).m_strWeight;
                    m_ImageMap.m_strClearance = m_vecImages.at(iImageMapIndex).m_strClearance;
                    // Misc.
                    m_ImageMap.m_strCountry = m_vecImages.at(iImageMapIndex).m_strCountry;
                    m_ImageMap.m_strTankVolume = m_vecImages.at(iImageMapIndex).m_strTankVolume;
                }
                m_vecImagesNew.push_back(m_ImageMap);
            }
            m_vecImages = m_vecImagesNew;
        }
    }
}

void CPictureDetails::ShowXMLData(int iImageMapIndex)
{
    // SetDlgItemText fires EN_CHANGE and m_bSuppressDirtyFlagsUpdate is
    // used to indicate that EN_CHANGE must be ignored when relates to
    // SetDlgItemText.
    m_bSuppressDirtyFlagsUpdate = TRUE;

    SetDlgItemText(IDC_PH_EDIT_XML_YEAR, 
                                m_vecImages.at(iImageMapIndex).m_strYear);
    SetDlgItemText(IDC_PH_EDIT_XML_MAKE, 
                                m_vecImages.at(iImageMapIndex).m_strMake);
    SetDlgItemText(IDC_PH_EDIT_XML_MODEL, 
                                m_vecImages.at(iImageMapIndex).m_strModel);
    SetDlgItemText(IDC_PH_EDIT_XML_SUBMODEL, 
                            m_vecImages.at(iImageMapIndex).m_strSubmodel);
    SetDlgItemText(IDC_PH_EDIT_XML_TYPE, 
                                m_vecImages.at(iImageMapIndex).m_strType);
    SetDlgItemText(IDC_PH_EDIT_XML_PRICE, 
                                m_vecImages.at(iImageMapIndex).m_strPrice);
    SetDlgItemText(IDC_PH_EDIT_XML_COUNTRY, 
                            m_vecImages.at(iImageMapIndex).m_strCountry);
    SetDlgItemText(IDC_PH_EDIT_XML_LENGTH, 
                            m_vecImages.at(iImageMapIndex).m_strLength);
    SetDlgItemText(IDC_PH_EDIT_XML_WIDTH, 
                                m_vecImages.at(iImageMapIndex).m_strWidth);
    SetDlgItemText(IDC_PH_EDIT_XML_HEIGHT, 
                            m_vecImages.at(iImageMapIndex).m_strHeight);
    SetDlgItemText(IDC_PH_EDIT_XML_WEIGHT, 
                            m_vecImages.at(iImageMapIndex).m_strWeight);
    SetDlgItemText(IDC_PH_EDIT_XML_CLEARANCE, 
                            m_vecImages.at(iImageMapIndex).m_strClearance);
    SetDlgItemText(IDC_PH_EDIT_XML_POWER, 
                                m_vecImages.at(iImageMapIndex).m_strPower);
    SetDlgItemText(IDC_PH_EDIT_XML_TANK_VOLUME, 
                        m_vecImages.at(iImageMapIndex).m_strTankVolume);
    SetDlgItemText(IDC_PH_EDIT_XML_FUEL, 
                                m_vecImages.at(iImageMapIndex).m_strFuel);

    m_bSuppressDirtyFlagsUpdate = FALSE;
}

//void CPictureDetails::OnFillFromXML(HWND hWnd, LPCTSTR pszInitImagesDir)
void CPictureDetails::OnButtonLoadFromXML()
{
    HWND hWnd;
    OPENFILENAME ofn;
    TCHAR szInitialDir[MAX_PATH];
    TCHAR szFFN[MAX_PATH];
    szFFN[0] = _T('\0'); // MSDN'03: NULL if initialization is not necessary

    hWnd = this->m_hWnd;
    // Dialog fails to open without setting initial directory.
    //lstrcpy(szInitialDir, theApp.m_strDocsDirectory); 
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize			= sizeof(OPENFILENAME);
    ofn.hwndOwner			= hWnd;
    ofn.hInstance			= NULL;
    ofn.lpstrFilter			= _T("*.xml\0\0"); 
    ofn.lpstrCustomFilter	= NULL;
    ofn.nMaxCustFilter		= 0;
    ofn.nFilterIndex		= 0;
    ofn.lpstrFile			= szFFN; // gets selected full file path
    ofn.nMaxFile			= MAX_PATH;
    ofn.lpstrFileTitle		= NULL;
    ofn.nMaxFileTitle		= 0;
    ofn.lpstrInitialDir		= NULL; //szInitialDir;
    ofn.lpstrTitle			= _T("Select XML file");
    ofn.Flags				= OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST; 
    ofn.nFileOffset			= 0;
    ofn.nFileExtension		= 0;
    ofn.lpstrDefExt			= NULL;
    ofn.lCustData			= 0;
    ofn.lpfnHook			= NULL;
    ofn.lpTemplateName		= NULL;

    if (GetOpenFileName(&ofn))
    {
        if (IsValidCarDescriptionFile(szFFN))
            XMLReadSettings(szFFN);
        else 
        {
            if (IDYES == ::MessageBox(NULL, 
               "The XML file does not appear as car description XML. Continue?", 
                       "MyScreenSaver Builder 2.2", MB_YESNO | MB_ICONQUESTION))
            {
                XMLReadSettings(szFFN);
            }
        }
    }
}

// Purpose: reads car description data from external XML file.
void CPictureDetails::XMLReadSettings(LPCTSTR pszFFN /* XML file */)
{
    SetDlgItemText(IDC_PH_EDIT_XML_YEAR, XMLReadValue("Year", pszFFN).c_str());
    SetDlgItemText(IDC_PH_EDIT_XML_MAKE, XMLReadValue("Make", pszFFN).c_str());
    SetDlgItemText(IDC_PH_EDIT_XML_MODEL, XMLReadValue("Model", 
                                                               pszFFN).c_str());
    SetDlgItemText(IDC_PH_EDIT_XML_SUBMODEL, XMLReadValue("SubModel", 
                                                               pszFFN).c_str());
    SetDlgItemText(IDC_PH_EDIT_XML_TYPE, XMLReadValue("Type", pszFFN).c_str());
    SetDlgItemText(IDC_PH_EDIT_XML_PRICE, XMLReadValue("Price", pszFFN).c_str());
    SetDlgItemText(IDC_PH_EDIT_XML_POWER, XMLReadValue("Power", 
                                                               pszFFN).c_str());
    SetDlgItemText(IDC_PH_EDIT_XML_FUEL, 
                                   XMLReadValue("Energy_Type", pszFFN).c_str()); 
    SetDlgItemText(IDC_PH_EDIT_XML_LENGTH, XMLReadValue("Length", 
                                                               pszFFN).c_str());
    SetDlgItemText(IDC_PH_EDIT_XML_WIDTH, XMLReadValue("Width", 
                                                               pszFFN).c_str());
    SetDlgItemText(IDC_PH_EDIT_XML_HEIGHT, XMLReadValue("Height", 
                                                               pszFFN).c_str());
    SetDlgItemText(IDC_PH_EDIT_XML_CLEARANCE, XMLReadValue("Clearance", 
                                                               pszFFN).c_str());
    SetDlgItemText(IDC_PH_EDIT_XML_WEIGHT, XMLReadValue("Weight", 
                                                               pszFFN).c_str());
    SetDlgItemText(IDC_PH_EDIT_XML_TANK_VOLUME, XMLReadValue("Tank_Volume", 
                                                               pszFFN).c_str());
    SetDlgItemText(IDC_PH_EDIT_XML_COUNTRY, XMLReadValue("Contry", 
                                                               pszFFN).c_str());
}

// From RTP v1.3.
string CPictureDetails::XMLReadValue(char* pszTagName /* always ANSI*/, 
                                                  LPCTSTR pszFFN /* XML file */)
{
    char szLine[MAX_PATH];
    string sLine, sLeftTrimmedLine, sParameter;
    string sOpenTag;
    string sTagName, sTagValue;
    ifstream XMLfile;

    sTagName = pszTagName;
    sOpenTag = "<" + sTagName + ">";
    XMLfile.open(pszFFN);

    if (XMLfile) 
    {
        while (XMLfile.good())
        {
            XMLfile.getline(szLine, MAX_PATH);
            sLine = szLine;

            if (sLine.find(sOpenTag) !=  -1)
            {
                // Left-trim line found out of spaces if they exist, leave 
                // original line as it is if spaces are not found.
                sLeftTrimmedLine = 
                             sLine.substr(sLine.find_first_not_of(" ")).c_str();
                
                // Extract tag value.
                sTagValue =  sLeftTrimmedLine.substr(sOpenTag.length(), 
                           sLeftTrimmedLine.length() - 2*sOpenTag.length() - 1);
            }
        }
        XMLfile.close();
    }
    
    return sTagValue; 
}

BOOL CPictureDetails::IsValidCarDescriptionFile(LPCTSTR pszFFN /* XML file */)
{
    char szLine[MAX_PATH];
    string sLine;
    string sKeyPhrase = "<MyScreenSaver_Builder>";
    ifstream XMLfile;

    XMLfile.open(pszFFN);

    if (XMLfile) 
    {
        while (XMLfile.good())
        {
            XMLfile.getline(szLine, MAX_PATH);
            sLine = szLine;

            if (sLine.find(sKeyPhrase) !=  -1 /* not found */)
            {
                return TRUE;
            }
        }
        XMLfile.close();
    }

     return FALSE;
}

// Copied from PP0 with updates (PP0->CPictureDetails).
// OnHelpInfo (WM_HELP handler) handles context help: 1) "?" context help, 2) 
// SHIFT+F1, 3) F1 (2 and 3 are the same AFAIS). 
BOOL CPictureDetails::OnHelpInfo(HELPINFO* pHelpInfo)
{
    //AfxMessageBox("COptionalSettings::OnHelpInfo");
    HH_POPUP ContextHelpPopup;
    
    // Get context menu map's index for the control that was clicked or has 
    // focus (based on iCtrlId value), then get corresponding contex help string, 
    // make up HH_POPUP structure and invoke context help for the control with
    // HtmlHelp.
    for (int i = 0; i < theApp.m_ContextHelp.m_nMapSizePD; i++) 
    {
        if (pHelpInfo->iCtrlId ==  theApp.m_ContextHelp.m_MapPD[i].nControlID) 
        { 
            memset(&ContextHelpPopup, 0, sizeof(HH_POPUP) );   
            ContextHelpPopup.cbStruct = sizeof(HH_POPUP);

            ContextHelpPopup.hinst = AfxGetInstanceHandle(); // application 
                                                             // instance
            ContextHelpPopup.idString = 
                      theApp.m_ContextHelp.m_MapPD[i].nStringHelpID; // resource ID 
            ContextHelpPopup.pt.x = pHelpInfo->MousePos.x; 
            ContextHelpPopup.pt.y = pHelpInfo->MousePos.y + 15; 
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
void CPictureDetails::OnContextMenu(CWnd* pWnd, CPoint point)
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
#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include <vector>
//#include "CustomListView.h"

//-- Start: move (PP2's copy) to separate class like CRebuildMSS 
#include "MySaverPropPage2.h" // added to call PP2 functions from PP1, 
                              // added to header to work with CopyData structure 
//-- End: move to separate class like CRebuildMSS

#define IMAGE_COUNT 50 //max array size 

using namespace std ; //for vector declaration

// Forward declaration for CCustomListView (m_pPP1 used).
class CMySaverPropPage1; 

class CCustomListView : public CListCtrl
{
    DECLARE_DYNAMIC(CCustomListView)

public:
    CCustomListView();
    //CCustomListView(CMySaverPropPage1* pPP1);
    virtual ~CCustomListView();

    CMySaverPropPage1* m_pPP1;
protected:
    // List view was subclassed, initially, to handle dropped files.
    afx_msg void OnDropFiles(HDROP hDropInfo); 
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnListViewCustomDraw(NMHDR * pNMHDR, LRESULT * pResult); 
    afx_msg void OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()
};

class CImageMap
{
public:
    CString m_strTitle; 
    // Just a name (v1.1 and up).
    CString m_strFileName; 
    // File name with full path to privide images addition in PP1 (v2.0).
    CString m_strFullFileName;  
public:
    // Main
    CString m_strYear; 
    CString m_strMake; 
    CString m_strModel; 
    CString m_strSubmodel; 
    CString m_strType;
    CString m_strPrice;
    // Engine
    CString m_strPower;
    CString m_strFuel;
    // Physical dimensions
    CString m_strLength;
    CString m_strWidth;
    CString m_strHeight;
    CString m_strWeight;
    CString m_strClearance;
    // Misc
    CString m_strCountry;
    CString m_strTankVolume;
public:
    BOOL bDeletionMarked;
    // The permanent index is used in edit mode to enable re-ordering.
    int iPermanentIndex;
    // Filled in PP2 only if at least one image marked for deletion.
    //BYTE* pbImageRes;
    //VOID* pImageRes;
    DWORD dwImageSize; // used for MSS size evaluation
    // Strings for image sizes enable reuse of these values in edit mode with 
    // re-ordering.
    CString m_strImageSize_Bytes;  
    CString m_strImageSize_Pixels; 
};

class CPicture 
{
public:
    CPicture(){}
    ~CPicture(){}

    // Load frm various sosurces
    //BOOL Load(HINSTANCE  hResInst, UINT nIDRes);
    DWORD Load(HINSTANCE  hResInst, UINT nIDRes);
    BOOL Load(CFile& file);
    BOOL Load(CArchive& ar);
    BOOL Load(IStream* pstm);

    // render to device context
    BOOL Render(CDC* pDC, 
                CRect rc=CRect(0,0,0,0),
                LPCRECT prcMFBounds=NULL) const;

    //CSize GetImageSize(CDC* pDC=NULL) const;

    operator IPicture*() {
        return m_spIPicture;
    }

    /*
    void GetHIMETRICSize(OLE_XSIZE_HIMETRIC& cx, OLE_YSIZE_HIMETRIC& cy) const {
        cx = cy = 0;
        const_cast<CPicture*>(this)->m_hr = m_spIPicture->get_Width(&cx);
        ASSERT(SUCCEEDED(m_hr));
        const_cast<CPicture*>(this)->m_hr = m_spIPicture->get_Height(&cy);
        ASSERT(SUCCEEDED(m_hr));
    }
    */

    void Free() {
        if (m_spIPicture) {
            m_spIPicture.Release();
        }
    }

public:
    CComQIPtr<IPicture>m_spIPicture;		
    HRESULT m_hr;								
};

class CMySaverPropPage1 : public CPropertyPage
{
    DECLARE_DYNAMIC(CMySaverPropPage1)
public:
    // Constructor(s).
    CMySaverPropPage1();
    // Destructor.
    virtual ~CMySaverPropPage1();

public:
    // Access member variables
    CPropertySheet* m_pSheet;
    CString m_strInitImages;
    BOOL m_bIsImagesDeletionMarked;
    BOOL m_bIsImagesReordered;
    short m_nBuildOption;
    short m_nPicturesCategory;  // 0 - Not specified (default); 1 - Cars
    //vector<CString> m_vecImageFileNames; // from v1.1, LL not used
    //vector<CCarXMLData> m_vecCarXMLData;
    vector<CImageMap> m_vecImages;
    CCustomListView m_ctlListView;

    // Access member functions.
    void AddImage_EditMode(LPCTSTR pszPictureFile);
    void AddImage_NewMode(LPCTSTR pszFullFileName);
    void ResetListViewItems();
    void ResetListViewItems_EditMode();
    void InsertListViewItem3(int nItem, CString strFileName, 
       CString strFullFileName, LPCTSTR pszFileSize, LPCTSTR pszFileDimentions);
    void UpdateStringResource(int iStringID, LPCTSTR pszUpdateString, 
                                                     BOOL bUseBuildDir = FALSE);

    enum { IDD = IDD_PP1 };

protected:
    // Overridables
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);  
    virtual LRESULT OnWizardNext();
    virtual BOOL OnWizardFinish();
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()

private:
    // Utility member variables.
    int m_iImageCount;
    CPicture m_Picture[IMAGE_COUNT];
    TCHAR* m_Title[IMAGE_COUNT];
    CListCtrl m_ctlList;
    BOOL m_bShowToolTips;
    CString m_strStatusMessage;
    float m_fAcceptableRAMUsage;

    // Utility member functions.
    afx_msg BOOL OnSetActive();
    void SetListViewStyles();
    void CleanupListView();
    void InitListViewColumns();
    void InitListViewItems(LPCTSTR pszImageType);
    void InsertListViewItem(int nItem, CString strFileName, LPCTSTR pszFileSize, 
                                                     LPCTSTR pszFileDimentions);
    void InsertListViewItem2(int nItem, CString strFileName, TCHAR* pszFileSize,
                                                      TCHAR* pszFileDimentions);
    void InsertListViewItemFromMSS(int nItem, CString strFileName, 
                                LPCTSTR pszFileSize, LPCTSTR pszFileDimentions);
    BOOL LoadPicturesAndStrings(LPCTSTR pszFileName);
    CString ConvertToKB(LPCTSTR pszFileSize);
    CString ConvertToKBorMB(int iFileSize);
    int GetVectorIndex(CString strFileName);
    BOOL IsDuplicateName(CString strFileName);
    short EditMode_GetPicturesCategory(LPCTSTR pszFullFileName);
                                                            
public:
    afx_msg void OnButtonAddClicked();
    afx_msg void OnButtonDeleteClicked();
    afx_msg void OnButtonModifyClicked();

//-- Start: move (PP2's copy) to separate class like CRebuildMSS 
// Testing: duplicated functionality from PP2
    CopyData m_cd; // used in edit mode only
    CString m_strImageSetName;
    CString m_strMSSComments;

    int EditMode_GetImageCountWithoutDeletionMarked();
    void EditMode_GetImageSetData(LPCTSTR pszFullFileName, BOOL bWithRebuild);
    BOOL EditMode_MakeRCFile();
    BOOL EditMode_AddImagesToMSS(LPCTSTR pszFileName /* full path name */);
//-- End: move (PP2's copy) ...
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

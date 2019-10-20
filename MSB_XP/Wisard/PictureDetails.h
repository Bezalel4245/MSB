#pragma once

#include "resource.h"
#include <vector>
#include "afxwin.h"
#include "MySaverPropPage1.h" // get images directory.

using namespace std ; //for vector declaration

// Forward declaration for CPreviewWnd (invoke "Change Picture").
class CPictureDetails; 

class CThumbnail 
{
public:
    CThumbnail(){}
    ~CThumbnail(){}

    BOOL Load(LPCTSTR pszPathName);
    BOOL LoadPictureFromMSS(LPCTSTR pszFileName, short iImage);
    BOOL Render(CDC* pDC, CRect rc=CRect(0, 0, 0, 0), 
                                                LPCRECT prcMFBounds=NULL) const;
    operator IPicture*() {return m_spIPicture;}

public:
    CComQIPtr<IPicture> m_spIPicture;		
    HRESULT m_hr;								 
};

class CPreviewWnd : public CWnd
{
// Constructor(s) and destructor
public:
    CPreviewWnd(int iImageIndex, CString strImagesDir, vector<CImageMap> vec);
    virtual ~CPreviewWnd();

// Access member variables (Attributes)
public:
    vector<CImageMap> m_vecImages;
    CThumbnail m_Thumbnail;
    CPictureDetails* m_pPD;

// Access member functions (Operations)
public:
    BOOL Create(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, 
                   CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

// Overridable
public:

// Utility member variables (Implementation variables)
private:

// Utility member functions (Implementation methods)
private:
    void OnPaint(); // overridden 
    void OnLButtonDblClk(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
};

class CPictureDetails : public CDialog
{
    DECLARE_DYNAMIC(CPictureDetails)

public:
    // Constructor(s), destructor
    CPictureDetails(CWnd* pParent = NULL);

    CPictureDetails(short nBuildOption, short nPicturesCategory, 
        int iSelectedItem, CString strImagesDir, vector<CImageMap> vec, 
                                                          CWnd* pParent = NULL);
    virtual ~CPictureDetails();

public:
    // Access member variables.
    vector<CImageMap> m_vecImages;
    short m_nBuildOption;
    short m_nPicturesCategory;
    int m_iMSSImageID; 
    int m_iSelectedItem;
    CString m_strImagesDir;  

    BOOL m_bIsImagesReordered;
    enum { IDD = IDD_PICTURE_HEADERS };	

protected:
    // Overridables
    virtual void DoDataExchange(CDataExchange* pDX); 
    virtual void OnCancel();
private:
    //Overriddden functions
    BOOL OnInitDialog();

    // Utility member variables
    CString m_strImageName;
    CString m_strEdit;
    CListBox m_ctlListBox;
    CPreviewWnd* m_pPreviewWnd; 
    CEdit m_ctlEdit;
    int m_iOriginalHeight;
    // Dirty flag for XML data.
    BOOL m_bSuppressDirtyFlagsUpdate;
    BOOL m_bFormDataSaved; 
    BOOL m_bImageDirty; 
    CString m_strImageFullFileName;
    BOOL m_bTitleDirty;
    BOOL m_bYearDirty;
    BOOL m_bMakeDirty;
    BOOL m_bModelDirty;
    BOOL m_bSubmodelDirty;
    BOOL m_bTypeDirty;
    BOOL m_bPriceDirty;
    BOOL m_bCountryDirty;
    BOOL m_bLengthDirty;
    BOOL m_bWidthDirty;
    BOOL m_bHeightDirty;
    BOOL m_bWeightDirty;
    BOOL m_bClearanceDirty;
    BOOL m_bPowerDirty;
    BOOL m_bTankVolumeDirty;
    BOOL m_bFuelDirty;

    // Utility member functions.
    void CreatePreviewWindow();
    int GetImageMapIndex(CString);
    int GetImageMapPermanentIndex(CString);
    void UpdateStringResource(int iStringID, LPCTSTR pszUpdateString);
    void UpdateImageResource(int iResourceID, LPCTSTR pszFullFileName);
    void OnResize();
    BOOL IsFormDataChanged();
    void ReorderVectorImages();
    void ShowXMLData(int iImageMapIndex);
    void SaveXMLData(CString strImageName);

    afx_msg void OnClickListBox();
    afx_msg void OnMoveUpClicked();
    afx_msg void OnMoveDownClicked();
    //afx_msg void OnEditKillFocus();
    afx_msg void OnSaveClicked();
    afx_msg void OnCloseClicked();
    afx_msg void OnSaveCloseClicked();
    
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnEditChange_Power();
    afx_msg void OnEditChange_Fuel();
    afx_msg void OnEditChange_Year();
    afx_msg void OnEditChange_Make();
    afx_msg void OnEditChange_Model();
    afx_msg void OnEditChange_Submodel();
    afx_msg void OnEditChange_Type();
    afx_msg void OnEditChange_Price();
    afx_msg void OnEditChange_Length();
    afx_msg void OnEditChange_Width();
    afx_msg void OnEditChange_Height();
    afx_msg void OnEditChange_Weight();
    afx_msg void OnEditChange_Clearance();
    afx_msg void OnEditChange_Country();
    afx_msg void OnEditChange_TankVolume();
    afx_msg void OnEditChange_Title();
    afx_msg void OnButtonChangePictureClicked();
    afx_msg void OnButtonLoadFromXML();

private:
    void XMLReadSettings(LPCTSTR pszFFN /* XML file */);
    string XMLReadValue(char* pszTagName /* always ANSI*/, 
                                                 LPCTSTR pszFFN /* XML file */);
    BOOL IsValidCarDescriptionFile(LPCTSTR pszFFN /* XML file */);
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
public:
    CButton m_ctlLoadFromXMLButton;
};

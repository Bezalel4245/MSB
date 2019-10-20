#pragma once

#include "DirDialog.h"
#include "afxwin.h"
#include "EnterPassword.h"

// CEdit is subclassed to handle dropped files.
class CEditWithDAD : public CEdit
{
    DECLARE_DYNAMIC(CEditWithDAD)

public:
    CEditWithDAD();
    virtual ~CEditWithDAD();

protected:
    afx_msg void OnDropFiles(HDROP hDropInfo); 
    DECLARE_MESSAGE_MAP()
};

class CMySaverPropPage0 : public CPropertyPage
{
    DECLARE_DYNAMIC(CMySaverPropPage0)

public:
    CMySaverPropPage0();
    virtual ~CMySaverPropPage0();

// Dialog Data
    enum { IDD = IDD_PP0 };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual LRESULT OnWizardNext();
    virtual LRESULT OnWizardBack();
    virtual BOOL PreTranslateMessage(MSG* pMsg); 
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

    DECLARE_MESSAGE_MAP()
public:
    CString m_strHeaderTitle_New;
    CString m_strHeaderTitle_Edit;
    CString m_strHeaderSubTitle_New;
    CString m_strHeaderSubTitle_Edit;
    // The int type is used for m_nBuildOption, m_nPicturesCategory,  
    // m_nEditOption (instead of short) as DDX requirement. 
    int m_nBuildOption;  // 0 - New (default); 1 - Edit 
    int m_nPicturesCategory;  // 0 - Not specified (default); 1 - Cars
    int m_nEditOption; // 0 - edit MSS data; 1 - create setup ...
    CString m_strInitImages; // directory or full file name of MSS
    BOOL m_bShowToolTips;

    CComboBox m_ctlComboBox;
    CComboBox m_ctlComboBox_PicturesCategory;
    CEditWithDAD m_ctlEditWithDAD;
    CStatic m_ctlGroupBox;
    CButton m_ctlRadio1;
    CButton m_ctlRadio2;
    CButton m_ctlRadio3;
    CToolTipCtrl m_ctlToolTip;
    CEnterPassword m_dlgEP;

    afx_msg void OnBrowseClicked();
    afx_msg BOOL OnSetActive();
    BOOL IsShowWelcomePage();

    void OnBrowseMSS(HWND hWnd, LPCTSTR pszInitImagesDir);
    //short EditMode_GetPicturesCategory(LPCTSTR pszFullFileName);
    BOOL GetPasswordFromMSS(LPCTSTR pszFullFileName /* in */, 
                                                  LPTSTR pszPassword /* out */);
    void ShowToolTips();
    afx_msg void OnCheckBoxClicked();
    afx_msg void OnComboboxSelectionChange();
    afx_msg void OnComboboxPicCategorySelectionChange();
    DWORD IsDirectory(LPCTSTR pszFullFileName);
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
    //afx_msg void OnDropFiles(HDROP hDropInfo);
};

#pragma once

#include "afxcmn.h"
#include "SubEdit.h"
#include "ConfirmPassword.h"
#include "afxwin.h"

// Forward declaration for m_pDialog.
class COptionalSettings;

// Make a class derived from CListCtrl to handle ON_WM_LBUTTONDOWN()
class CSubListCtrl : public CListCtrl
{
    DECLARE_DYNAMIC(CSubListCtrl)

public:
    CSubListCtrl();
    virtual ~CSubListCtrl();

    COptionalSettings* m_pDialog;
    CSubEdit m_editWnd;
    int m_item;
    int m_subitem;
    //CString m_strPassword;
    // 1st-typed password before verification with 2nd typed (confirmation).
    // If 1st and 2nd are the same this password copied to m_strPassword,
    // if not is is discarded.
    CString m_strFirstTypedPassword; 
    CConfirmPassword m_ConfirmPassword;
protected:
    
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

class COptionalSettings : public CDialog
{
    DECLARE_DYNAMIC(COptionalSettings)

public:
    COptionalSettings(CWnd* pParent = NULL);   // standard constructor
    COptionalSettings(CString strMSSComments, CString strSetupDesc, 
                                                          CWnd* pParent = NULL);
    virtual ~COptionalSettings();

// Dialog Data
    enum { IDD = IDD_OPTIONAL_SETTINGS };

    // Access member functions.
    void ShowPropertyDescription(int nItem);

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnButtonOKClicked();
    DECLARE_MESSAGE_MAP()
public:
    //CListCtrl m_ctlListView;
    CSubListCtrl m_ctlListView;
    CString m_strPassword;
    CString m_strMSSComments;
    CString m_strSetupDesc;
private:
     // Utility member variables.	
    CStatic m_ctlStaticText;

    // Utility functions.	
    void SetListViewStyles();
    void InitListViewColumns();
    void InitListViewItems();

    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

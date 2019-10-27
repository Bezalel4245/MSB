#pragma once

#include "afxwin.h"

class CMySaverPropPageActivate : public CPropertyPage
{
    DECLARE_DYNAMIC(CMySaverPropPageActivate)

// Constructor(s) and destructor.
public:
    CMySaverPropPageActivate();   
    virtual ~CMySaverPropPageActivate();

// Access member variables (attributes).
public:
// None.

// Access member functions (operations).
public:
// None.

// Dialog data.
public:
    enum { IDD = IDD_ACTIVATE };
    CString m_strEmail;
    CString m_strRegistrationCode;
    CStatic m_ctlStatusText;

// Polymorphic member functions (overrides).
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual LRESULT OnWizardNext();
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Message handlers.	
protected:
    afx_msg BOOL OnSetActive();
    afx_msg BOOL OnHelpInfo(HELPINFO* lpHelpInfo ); 
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
    DECLARE_MESSAGE_MAP()

// Utility member variables (implementation variables).
private:
// None.

// Utility member functions (implementation methods).
private:
// None.
};

#pragma once
#include "afxwin.h"

class CMySaverPropPageWelcome : public CPropertyPage
{
    DECLARE_DYNAMIC(CMySaverPropPageWelcome)

// Constructor(s) and destructor.
public:
    CMySaverPropPageWelcome();
    virtual ~CMySaverPropPageWelcome();

// Access member variables (attributes).
public:
    BOOL m_bTrialExpired;

// Access member functions (operations).
public:
// None

// Dialog data.
public:
    enum { IDD = IDD_WELCOME };
    CStatic m_ctlStaticHeader;
    CStatic m_ctlStaticHeader2;
    CStatic m_ctlStaticHeader3;
    CStatic m_ctlStaticGroupBox;
    CButton m_ctlRadio1;
    CButton m_ctlRadio2;
    CButton m_ctlRadio3;
    CProgressCtrl m_ctlProgressBar;
    CButton m_ctlCheckBox;
    CStatic m_ctlStatic2; //  // XX days remaining out of 15

// Polymorphic member functions (overrides).
protected:
    virtual void DoDataExchange(CDataExchange* pDX);   
    virtual BOOL OnInitDialog();
    virtual LRESULT OnWizardNext();
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Message handlers.	
public:
    afx_msg BOOL OnSetActive();
    afx_msg void OnRadio2Clicked();
    afx_msg void OnRadioClicked();
    afx_msg BOOL OnHelpInfo(HELPINFO* lpHelpInfo ); 
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
    DECLARE_MESSAGE_MAP()

// Utility member variables (implementation variables).
private:
    //XP and higher
    //CLinkCtrl m_ctlWebLink;
    short m_iTrialPeriod;
    BOOL m_bShowWelcomePage;

// Utility member functions (implementation methods).
private:
    int StartKRM();
    char* GetTransactionText(int nStatusCode);
};

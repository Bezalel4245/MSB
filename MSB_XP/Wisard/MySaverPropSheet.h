#pragma once

#include "MySaverPropPageWelcome.h"
#include "MySaverPropPageActivate.h"
#include "MySaverPropPage0.h"	
#include "MySaverPropPage1.h"	
#include "MySaverPropPage2.h"

class CMySaverPropSheet : public CPropertySheet
{
    DECLARE_DYNAMIC(CMySaverPropSheet)

public:
    CMySaverPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, 
                                                          UINT iSelectPage = 0);
    CMySaverPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd= NULL, 
        UINT iSelectPage = 0, HBITMAP hbmpW = 0, HPALETTE hpal= 0, 
                                                             HBITMAP hbmpW1= 0);
    virtual ~CMySaverPropSheet();
    virtual BOOL OnInitDialog();
protected:
    DECLARE_MESSAGE_MAP()
public:
    BOOL IsShowWelcomePage();
    
    CMySaverPropPageWelcome  m_PageW;
    CMySaverPropPageActivate m_PageA;
    CMySaverPropPage0        m_Page0;	
    CMySaverPropPage1        m_Page1;
    CMySaverPropPage2        m_Page2;

    BOOL m_bShowWelcomePage;
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};



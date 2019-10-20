#pragma once

#include "resource.h"
#include "afxwin.h"

// CConfirmPassword dialog

class CConfirmPassword : public CDialog
{
    DECLARE_DYNAMIC(CConfirmPassword)

public:
    CConfirmPassword(CWnd* pParent = NULL);   // standard constructor
    virtual ~CConfirmPassword();

// Dialog Data
    enum { IDD = IDD_CONFIRM_PASSWORD };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    BOOL OnInitDialog(); 
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
public:
    CString m_strSecondTypedPassword;
};

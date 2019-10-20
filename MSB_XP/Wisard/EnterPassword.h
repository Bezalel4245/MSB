#pragma once

#include "resource.h"

// EnterPassword dialog

class CEnterPassword : public CDialog
{
    DECLARE_DYNAMIC(CEnterPassword)

public:
    CEnterPassword(CWnd* pParent = NULL);   // standard constructor
    virtual ~CEnterPassword();
// Dialog Data
    enum { IDD = IDD_ENTER_PASSWORD };

    BOOL OnInitDialog(); 
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    CString m_strTypedPassword;
};

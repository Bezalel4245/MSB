#include "stdafx.h"
#include "ConfirmPassword.h"


// CConfirmPassword dialog

IMPLEMENT_DYNAMIC(CConfirmPassword, CDialog)
CConfirmPassword::CConfirmPassword(CWnd* pParent /*=NULL*/)
    : CDialog(CConfirmPassword::IDD, pParent)
    , m_strSecondTypedPassword(_T(""))
{
}

CConfirmPassword::~CConfirmPassword()
{
}

void CConfirmPassword::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_CP_EDIT,  m_strSecondTypedPassword);
    DDV_MaxChars(pDX,  m_strSecondTypedPassword, 13);
}

BEGIN_MESSAGE_MAP(CConfirmPassword, CDialog)
END_MESSAGE_MAP()

BOOL CConfirmPassword::OnInitDialog() 
{
    CDialog::OnInitDialog();
    //--------------------------------------------------------------------------
    
    // Set password character to "*", which is by default it is "|" (LL in 
    // VS.NET 2003).
    ::SendMessage(GetDlgItem(IDC_CP_EDIT)->m_hWnd, EM_SETPASSWORDCHAR, 
                                                                  char('*'), 0);
    // Notice: focus to edit control set in resource editor with tab order.
    m_strSecondTypedPassword = _T("");
    UpdateData(FALSE); 
    return 1;
}

void CConfirmPassword::OnOK() 
{
    UpdateData(TRUE); 

    //--------------------------------------------------------------------------
    CDialog::OnOK();
}

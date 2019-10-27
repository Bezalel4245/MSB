#include "stdafx.h"
#include "EnterPassword.h"

IMPLEMENT_DYNAMIC(CEnterPassword, CDialog)
CEnterPassword::CEnterPassword(CWnd* pParent /*=NULL*/)
    : CDialog(CEnterPassword::IDD, pParent)
    , m_strTypedPassword(_T(""))
{
}

CEnterPassword::~CEnterPassword()
{
}

void CEnterPassword::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EP_EDIT, m_strTypedPassword);
}

BEGIN_MESSAGE_MAP(CEnterPassword, CDialog)
END_MESSAGE_MAP()

BOOL CEnterPassword::OnInitDialog() 
{
    CDialog::OnInitDialog();
    //--------------------------------------------------------------------------
    
    // Set password character to "*", which is by default it is "|" (LL in 
    // VS.NET 2003).
    ::SendMessage(GetDlgItem(IDC_EP_EDIT)->m_hWnd, EM_SETPASSWORDCHAR, 
                                                                  char('*'), 0);

    return 1;
}

// SubEdit.cpp : implementation file
//

#include "stdafx.h"
//#include "MFC_ListView_EditableItems.h"
#include "SubEdit.h"


// CSubEdit

IMPLEMENT_DYNAMIC(CSubEdit, CEdit)
CSubEdit::CSubEdit()
{
}

CSubEdit::~CSubEdit()
{
}


BEGIN_MESSAGE_MAP(CSubEdit, CEdit)
    ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

void CSubEdit::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
    lpwndpos->x=m_x; // + 100;

    CEdit::OnWindowPosChanging(lpwndpos);
}


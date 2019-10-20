#pragma once


// CSubEdit

class CSubEdit : public CEdit
{
    DECLARE_DYNAMIC(CSubEdit)

public:
    CSubEdit();
    int m_x;
    virtual ~CSubEdit();

protected:
    afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
    DECLARE_MESSAGE_MAP()
};



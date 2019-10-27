#include "stdafx.h"
#include "MySaver.h"
#include "OptionalSettings.h"

// COptionalSettings dialog

IMPLEMENT_DYNAMIC(COptionalSettings, CDialog)

COptionalSettings::COptionalSettings(CWnd* pParent /*=NULL*/)
                                      : CDialog(COptionalSettings::IDD, pParent)
{
}

// Reserved for edit mss mode.
COptionalSettings::COptionalSettings(CString strMSSComments, 
    CString strSetupDesc, CWnd* pParent /*=NULL*/) 
                                      : CDialog(COptionalSettings::IDD, pParent)
{
    m_strMSSComments = strMSSComments;
    m_strSetupDesc = strSetupDesc;
}

COptionalSettings::~COptionalSettings()
{
}

void COptionalSettings::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_OS_LISTVIEW, m_ctlListView);
    DDX_Control(pDX, IDC_OS_STATIC_MESSAGE, m_ctlStaticText);
}

BEGIN_MESSAGE_MAP(COptionalSettings, CDialog)
    ON_BN_CLICKED(IDOK, OnButtonOKClicked)
    ON_WM_HELPINFO()
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

BOOL COptionalSettings::OnInitDialog()
{
    CDialog::OnInitDialog();
    //--

    SetListViewStyles();
    InitListViewColumns();
    InitListViewItems();

    m_ctlListView.m_pDialog = this;
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void COptionalSettings::SetListViewStyles()
{
    DWORD dwStyles;
    DWORD dwExStyles ;
    
    // Set list view regular styles.
    dwStyles = WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | 
        LVS_SINGLESEL | LVS_ALIGNLEFT | LVS_NOSORTHEADER | 
                                                LVS_EDITLABELS /* label only */;
    m_ctlListView.ModifyStyle(0 /* remove */, dwStyles, 0);

    // Set list view extended styles.
    dwExStyles = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP 
                                                             | WS_EX_CLIENTEDGE;
    m_ctlListView.SetExtendedStyle(dwExStyles); 
}

void COptionalSettings::InitListViewColumns()
{
    m_ctlListView.InsertColumn(0, _T("Property"), LVCFMT_CENTER);
    m_ctlListView.SetColumnWidth(0, 180);
    m_ctlListView.InsertColumn(1, _T("Value"), LVCFMT_LEFT); 
    m_ctlListView.SetColumnWidth(1, 200);
}

void COptionalSettings::InitListViewItems()
{
    CString strPasswordMask('*', m_strPassword.GetLength()); // does not work 
                                                             // with "*"
    m_ctlListView.DeleteAllItems(); 
    
    m_ctlListView.InsertItem(0, "MSS Edit Password");
    m_ctlListView.SetItemText(0, 1, strPasswordMask);
    m_ctlListView.InsertItem(1, "MSS File Comments");
    m_ctlListView.SetItemText(1, 1, m_strMSSComments);
    m_ctlListView.InsertItem(2, "Self-Extracting Setup Comments");
    m_ctlListView.SetItemText(2, 1, "");
}

//--

IMPLEMENT_DYNAMIC(CSubListCtrl, CListCtrl)
CSubListCtrl::CSubListCtrl()
{
    m_subitem = 0;
}

CSubListCtrl::~CSubListCtrl()
{
}

BEGIN_MESSAGE_MAP(CSubListCtrl, CListCtrl)
    ON_WM_LBUTTONDOWN()
    ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
    ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void CSubListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
    LVHITTESTINFO	lvhit;
    lvhit.pt = point;
    int item = SubItemHitTest(&lvhit);

    // Show property description.
    m_pDialog->ShowPropertyDescription(item);

    ////if (over a subitem)
    //if (item != -1 && lvhit.iSubItem && (lvhit.flags & LVHT_ONITEM ))
    if (item != -1 && lvhit.iSubItem)
    {
        ////mouse click outside the editbox in an allready editing cell cancels editing
        if (m_subitem == lvhit.iSubItem && item == m_item)
        {
             CListCtrl::OnLButtonDown(nFlags, point);
        }
        else
        {
            CListCtrl::OnLButtonDown(nFlags, point);

            m_subitem = lvhit.iSubItem;
            m_item = item;
            //SetFocus();
            EditLabel(lvhit.iItem); //EditLabel(item);
        }

    }
    else
        // CListCtrl (not CSubLIstCtrl, which stalls ...).
        CListCtrl::OnLButtonDown(nFlags, point);
}

// MSDN'03: "If any other reflected message is sent, the parent window has the 
// first chance to handle it and the control will receive the reflected 
// message." Normally "when a WM_NOTIFY message is sent, the control is offered 
// the first chance to handle it."
void CSubListCtrl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
    LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

    ////if (subitem selected for editing)
    if (m_subitem)
    {
        ASSERT(m_item == pDispInfo->item.iItem);

        CRect	subrect;
        GetSubItemRect( pDispInfo->item.iItem, m_subitem, LVIR_BOUNDS , subrect );

        // Get edit control and subclass.
        HWND hWnd=(HWND)SendMessage(LVM_GETEDITCONTROL);
        ASSERT(hWnd!=NULL);
        // Horizontal shift can be changed in CSubEdit's OnWindowPosChanging.
        VERIFY(m_editWnd.SubclassWindow(hWnd));

        // Move edit control text 1 pixel to the right of org label, as Windows 
        // does it...
        m_editWnd.m_x = subrect.left + 6; 

        if (m_item == 0 /* password */) 
        {
            // Limit maximum number of password characters (ANSI) to 15 (like 
            // in Word).
            m_editWnd.SetLimitText(15); 
            // Mask the password with "*" symbols.
            m_editWnd.SetPasswordChar('*');
            m_editWnd.ModifyStyle(0, ES_PASSWORD); 
        }
        else if (m_item == 1 /* MSS comments */)
        {
            // Limit maximum number of characters (ANSI) for MSS comments to 80.
            m_editWnd.SetLimitText(80); 
        }
        else if (m_item == 2 /* EXE comments */)
        {
            // Limit maximum number of characters (ANSI) for MSS comments to 80.
            m_editWnd.SetLimitText(80); 
        }
        
        if (pDispInfo->item.iItem == 0 /* password */)
            m_editWnd.SetWindowText(theApp.m_strMSSPassword);
        else 
            m_editWnd.SetWindowText(GetItemText(pDispInfo->item.iItem, 
                                                                    m_subitem));

        // Hide subitem text so it don't show if we delete some text in the edit 
        // control. OnPaint handles other issues also regarding this
        CRect	rect;
        GetSubItemRect(pDispInfo->item.iItem, m_subitem, LVIR_LABEL, rect);
        CDC* hDc = GetDC();
        hDc->FillRect(rect,&CBrush(::GetSysColor(COLOR_WINDOW)));
        ReleaseDC(hDc);
    }

    //return: editing permitted
    *pResult = 0;
}

void CSubListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
    LV_DISPINFO *plvDispInfo = (LV_DISPINFO *)pNMHDR;
    LV_ITEM	*plvItem = &plvDispInfo->item;

    ////if (end of sub-editing) do cleanup
    if (m_subitem)
        {
        // plvItem->pszText is NULL if editing canceled
        if (plvItem->pszText != NULL )
        {
            if (m_item == 0) 
            {
                m_strFirstTypedPassword = plvItem->pszText;
                if (m_ConfirmPassword.DoModal() == IDOK)
                {
                    if (m_strFirstTypedPassword == 
                                        m_ConfirmPassword.m_strSecondTypedPassword)
                    {
                        m_pDialog->m_strPassword = m_strFirstTypedPassword;
                        // Set password mask according to the length of the new 
                        // password.
                        CString strPasswordMask('*', 
                                           m_strFirstTypedPassword.GetLength());
                        SetItemText(plvItem->iItem, m_subitem, strPasswordMask);
                        ::MessageBox(0, 
                            "The password was changed successfully!", 
                            "MyScreenSaver Builder 2.2", 
                                                    MB_OK | MB_ICONINFORMATION); 
                    }
                    else
                    {
                        
                        SetItemText(plvItem->iItem, m_subitem, "");
                        // Return an empty string to indicate that password was 
                        // not changed.
                        //m_pDialog->m_strPassword  = ""; 
                        AfxMessageBox("The passwords you typed do not match!");
                    }
                }
                else 
                {
                    CString strMsg;
                    strMsg  = "The action canceled. ";
                    strMsg += "The password was not changed or set!";
                    SetItemText(plvItem->iItem, m_subitem, "");
                    //m_pDialog->m_strPassword = ""; 
                    AfxMessageBox(strMsg);
                }
            }
            else if (m_item == 1) 
            {
                SetItemText(plvItem->iItem, m_subitem, plvItem->pszText);
                m_pDialog->m_strMSSComments = plvItem->pszText;
            }
            else if (m_item == 2) 
            {
                SetItemText(plvItem->iItem, m_subitem, plvItem->pszText);
                m_pDialog->m_strSetupDesc = plvItem->pszText;
            }
        }
        
        VERIFY(m_editWnd.UnsubclassWindow()!=NULL);
        
        m_subitem = 0;
        //allways revert to org label (Windows thinks we are editing the leftmost item)
        *pResult = 0;
        }
    else
        //return: update label on leftmost item
        *pResult = 1;

}

void CSubListCtrl::OnPaint() 
{
    //if (subitem editing)
    if (m_subitem)
    {
        
        CRect	rect;
        CRect	editrect;

        GetSubItemRect(m_item,m_subitem,LVIR_LABEL ,rect);
        
        m_editWnd.GetWindowRect(editrect);
        ScreenToClient(editrect);

        // Block text redraw of the subitems text (underneath the editcontrol)
        // if we didn't do this and deleted some text in the edit control, 
        // the subitems original label would show.
        if (editrect.right < rect.right)
        {
            rect.left = editrect.right ;
            ValidateRect(rect);
        }
        
        // Block filling redraw of leftmost item (caused by FillRect).
        GetItemRect(m_item,rect,LVIR_LABEL );
        ValidateRect(rect);
    }

    CListCtrl::OnPaint();
}

void COptionalSettings::OnButtonOKClicked()
{
    m_strMSSComments = m_ctlListView.GetItemText(1, 1);
    m_strSetupDesc = m_ctlListView.GetItemText(2, 1);

    //--
    OnOK();
}

void COptionalSettings::ShowPropertyDescription(int nItem)
{
    CString strMsg0;
    CString strMsg1;
    CString strMsg2;
    CString strMsg3;

    strMsg0  = "Specifies the password to edit MSS file. It is not set by ";
    strMsg0 += "default.";
    strMsg1  = "Specifies the comment for MSS file. You can see this comment ";
    strMsg1 += "in Version tab of MSS properties.";  
    strMsg2  = "Specifies the comment for setup executable file. You can see ";
    strMsg2 += "this comment by opening Version tab of screen saver setup ";
    strMsg2 += "program.";

    switch (nItem)
    {
    case -1: // when outside selection area
        m_ctlStaticText.SetWindowText("");
        break;
    case 0:
        m_ctlStaticText.SetWindowText(strMsg0);
        break;
    case 1:
        m_ctlStaticText.SetWindowText(strMsg1);
        break;
    case 2:
        m_ctlStaticText.SetWindowText(strMsg2);
        break;
    default:
        break;
    }
}

// Copied from PP0 with updates (PP0->COptionalSettings).
// OnHelpInfo (WM_HELP handler) handles context help: 1) "?" context help, 2) 
// SHIFT+F1, 3) F1 (2 and 3 are the same AFAIS). 
BOOL COptionalSettings::OnHelpInfo(HELPINFO* pHelpInfo)
{
    //AfxMessageBox("COptionalSettings::OnHelpInfo");
    HH_POPUP ContextHelpPopup;
    
    // Get context menu map's index for the control that was clicked or has 
    // focus (based on iCtrlId value), then get corresponding contex help string, 
    // make up HH_POPUP structure and invoke context help for the control with
    // HtmlHelp.
    for (int i = 0; i < theApp.m_ContextHelp.m_nMapSizeOS; i++) 
    {
        if (pHelpInfo->iCtrlId ==  theApp.m_ContextHelp.m_MapOS[i].nControlID) 
        { 
            memset(&ContextHelpPopup, 0, sizeof(HH_POPUP) );   
            ContextHelpPopup.cbStruct = sizeof(HH_POPUP);

            ContextHelpPopup.hinst = AfxGetInstanceHandle(); // application 
                                                             // instance
            ContextHelpPopup.idString = 
                      theApp.m_ContextHelp.m_MapOS[i].nStringHelpID; // resource ID 
            ContextHelpPopup.pt.x = pHelpInfo->MousePos.x; 
            ContextHelpPopup.pt.y = pHelpInfo->MousePos.y + 15; 
            ContextHelpPopup.pszText = NULL; // recommended (KB241218), 
                                             // but LL no effect 
            ContextHelpPopup.rcMargins.top    =  5;
            ContextHelpPopup.rcMargins.bottom =  5;
            ContextHelpPopup.rcMargins.left   =  5;
            ContextHelpPopup.rcMargins.right  =  5;
            ContextHelpPopup.clrForeground = -1; // use system color 
            ContextHelpPopup.clrBackground = -1; // use system color
            //ContextHelpPopup.pszFont = "MS Sans Serif, 10, , BOLD";

            // I call HtmlHelp 2 times to avoid color distortion when "?" called 
            // for the 1st time (one call is redundant, but removes the 
            // problem). TODO: find out normal way without 2-times call (for 
            // some reason color distortion disappears when "?" help invoked for 
            // the 2nd time).
            
            // For the 1st argument, this->m_hWnd is used instead of 
            // GetDlgItem(pHelpInfo->iCtrlId)->m_hWnd since it does not causes 
            // crash if context help is implemented for the Wizard's Help button 
            // (redirected from property sheet class to here).
            ::HtmlHelp(this->m_hWnd, NULL, 
                               HH_DISPLAY_TEXT_POPUP, (DWORD)&ContextHelpPopup);
            ::HtmlHelp(this->m_hWnd, NULL, 
                               HH_DISPLAY_TEXT_POPUP, (DWORD)&ContextHelpPopup);
            break;
        }
    }

    return TRUE;
}

// Copied from PP0.
// The function realizes "What's This?" context help.
void COptionalSettings::OnContextMenu(CWnd* pWnd, CPoint point)
{
    CMenu menu;
    CPoint ptT(point);

    ScreenToClient(&point);

    if (pWnd == this) pWnd = ChildWindowFromPoint(point, CWP_SKIPINVISIBLE);
    if (!pWnd || pWnd == this) return;

    menu.CreatePopupMenu();
    menu.AppendMenu(MF_STRING, 0x0100, _T("&What's This?"));

    if (!menu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                                                            ptT.x, ptT.y, this))
        return; 

    ClientToScreen(&point);
    
    // Re-direct handling to OnHelpInfo.
    HELPINFO hi;
    hi.cbSize = sizeof(HELPINFO);
    hi.dwContextId = 0; // some
    hi.hItemHandle = 0; // some
    hi.iContextType = HELPINFO_WINDOW;
    hi.iCtrlId = pWnd->GetDlgCtrlID(); 
    hi.MousePos = point;
    
    OnHelpInfo(&hi);
}

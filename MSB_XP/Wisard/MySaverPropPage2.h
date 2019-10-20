#pragma once

#pragma warning( disable : 4267 )
#include "DirDialog.h"
#include "OptionalSettings.h"
#include "afxwin.h"
#include "afxcmn.h"

// This structure is used in edit mode only (and only when MSS is rebuilt)
// to copy constants from old to new MSS.
struct CopyData
{
    // Main constants.
    CString strImageSetTitle;
    //CString strImageCount;
    CString strPicturesCategory;
    CString strMSSFormatVersion;

    // Security constants.
    CString strIsSample;
    CString strIsRegistered;
    CString strIsCustom;
    CString strPassword;

    // Reserved constants.
    CString strReserved1;
    CString strReserved2;
    CString strReserved3;

    // Version info's MSS comments.
    CString strMSSComments;
};

class CMySaverPropPage2 : public CPropertyPage
{
    DECLARE_DYNAMIC(CMySaverPropPage2)

public:
    CMySaverPropPage2();
    virtual ~CMySaverPropPage2();

    enum { IDD = IDD_PP2 };

protected:
    virtual void DoDataExchange(CDataExchange* pDX); 
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg); 
    virtual BOOL OnWizardFinish();
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()
public:
    short m_nBuildOption; 
    short m_nPicturesCategory;  // 0 - Not specified (default); 1 - Cars
    CString m_strInitImages;
    CString m_strBuildLocation;
    CString m_strInstallDir;
    CString m_strImageSetTitle;
    CString m_strImageSetName;
    CString m_strSetupName;
    CString m_strImagesDir;
    CString m_strSetupDir;
    CString m_strMSSSpacePaddedComments;
    CString m_strSetupDesc;
    CString m_strPassword; 
    CToolTipCtrl m_ctlToolTip;
    BOOL m_bShowToolTips;
    BOOL m_bCheckBoxCreateSetup;
    CProgressCtrl m_ctlProgressBar;
    CEdit m_ctlEditSetupName;
    CEdit m_ctlEditSetupLocation;
    CComboBox m_ctlComboBox;
    CopyData m_cd; // used in edit mode only
    BOOL m_bTitleDirty;
    BOOL m_bFileNameDirty; // without extension
    BOOL m_bOptionalSettingsDirty;
    
    //COptionalSettings m_dlgOptionalSettings;

    // Compile and build variables.
    HANDLE m_hChildProcess_1_1;
    HANDLE m_hChildProcess_1_2;
    HANDLE m_hChildProcess_2_1;
    HANDLE m_hChildProcess_2_2;

    afx_msg BOOL OnSetActive();
    void StartUnpackFilesThread();
    static UINT ThreadProc(LPVOID param);
    BOOL MakeRCFile();
    BOOL MakeSetupRCFile();
    //void EditMode_GetImageSetData(LPCTSTR pszFullFileName);
    void EditMode_GetImageSetData(LPCTSTR pszFullFileName, BOOL bWithRebuild);
    BOOL EditMode_MakeRCFile();
    void EditMode_PrebuildMSS();
    BOOL EditMode_AddImagesToMSS(LPCTSTR pszFileName /* full path name */);
    int EditMode_GetImageCountWithoutDeletionMarked();
    void EditMode_RunConsoleApp();
    BOOL IsValid();
    void ShowToolTips();
    CString GetStandardName();
    CString GetMSSPassword(LPCTSTR pszFullFileName /* in */) ;
    CString GetMSSComments();  // version.lib added
    CString MakeSpacePaddedComments(CString strComments);
    void UpdateMSSComments(CString strComments, BOOL bUseBuildDir = FALSE);  // version.lib added
    void DeleteDirectory(LPCTSTR pszTargetDirectory); 

    // Compile and build functions.
    //void RunConsoleApp();
    void RunConsoleApp(BOOL bCreateSetupProgram);
    void ReadAndHandleOutput_Step_1_1(HANDLE hPipeRead);
    void ReadAndHandleOutput_Step_1_2(HANDLE hPipeRead);
    void ReadAndHandleOutput_Step_2_1(HANDLE hPipeRead);
    void ReadAndHandleOutput_Step_2_2(HANDLE hPipeRead);
    
    afx_msg void OnButtonMoreParamsClicked();
    afx_msg void OnButtonChangeClicked();
    afx_msg void OnCheckboxCreateSetupClicked();
    afx_msg void OnEditChange_Title();
    afx_msg void OnEditChange_FileName();
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

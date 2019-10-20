#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"	
#include "ContextHelpMappings.h"
#include "MySaverPropSheet.h"

class CMySaverApp : public CWinApp
{
// Constructor(s) and destructor.
public:
    CMySaverApp();

// Access member variables (attributes).
public:
    // Application parameters.
    HKEY m_hComponentBaseHive;
    CString m_strRegKeyApplication;
    CString m_strRegKeyWizard; 
    CString m_strRegKeySCfg;
    short m_iTrialPeriod;
    BOOL m_bLaunchedFromCommandLine;
    CString m_strInstallDirectory;
    CString m_strMSBDataDirectory;
    CString m_strDocsDirectory;
    CString m_strUser; 
    CString m_strAdminUser;
    BOOL m_bImagesManuallyAdded;
    float m_fAcceptableRAMUsage;
    CBitmap m_BMP;
    short m_iPrevPage;
    HANDLE m_hBuildFolderCreatedEvent;
    CContextHelpMappings m_ContextHelp;

    // Time variables to avoide skipping pages (testing).
    DWORD m_dwWelcomePageInitTime;

    // Debuggging variables.
    BOOL m_bEnableDebug;
    DWORD m_dwPP0InitTime;
    DWORD m_dwPP1InitTime;
    DWORD m_dwPP2InitTime;

// Access member functions (operations).
public:
    CString Utility_GetFileNameFromFullFileName(LPCTSTR pszFullFileName, 
                                     BOOL bWithExtension /* TRUE by default */);
    CString Utility_GetDirectoryNameFromFullFileName(LPCTSTR pszFullFileName);
    void SaveImagesDirToRegistry(CString strImagesDir);
    void SaveLastFileToRegistry(CString strLastEditedFile);
    void SaveWelcomePageData(BOOL m_bShowWelcomePage);
    void SavePage0Data(BOOL bShowToolTips, short nBuildOption, 
                                      short nImagesCategory, short nEditOption);
    void SaveFinalPageData(BOOL m_bPreviewScreenSaver, BOOL m_bCreateSetup, 
                                                         CString m_strSetupDir);
    void App_Debug_FunctionCallTrace(LPCTSTR pszMessage);

// Dialog data. For this application class, the data for the related dialogs.
public:
    // PP0.
    short m_nBuildOption;
    short m_nImagesCategory;
    CString m_strImagesDirectory;
    CString m_strLastEditedFile;
    short m_nEditOption;

    // PP2.
    CString m_strMSSPassword;
    BOOL m_bCreateSetup;
    CString m_strSetupDeploymentLocation;

    // Obsolete.
    BOOL m_bShowToolTips;       // remove for v2.0 
    BOOL m_bPreviewScreenSaver; // remove for v2.0

// Polymorphic member functions (overrides). 
public:
    virtual BOOL InitInstance();
    int ExitInstance(); 

// Message handlers.	
public:
    BOOL OnF1Help(short nDialogID, HELPINFO* pHelpInfo);
    DECLARE_MESSAGE_MAP()

// Utility member variables (implementation variables).
private:
// None.

// Utility member functions (implementation methods).
private:
    void LoadRegistryData();
    int GetOSVersion();
    BOOL IsMSSFile(LPCTSTR pszFFN);
};

extern CMySaverApp theApp;
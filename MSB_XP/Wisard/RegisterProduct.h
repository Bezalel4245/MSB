#pragma once

class CRegisterProduct : public CDialog
{
    DECLARE_DYNAMIC(CRegisterProduct)

// Constructor(s) and destructor (Constructors)
public:
    CRegisterProduct(CWnd* pParent = NULL);   
    virtual ~CRegisterProduct();

// Access member variables (Attributes)
public:
    short m_iTrialPeriod; //better private?

// Access member functions (Operations)
public:
    BOOL IsRegistered();
    BOOL IsValidRC(char* email, char* regCode);
    BOOL RegisterProduct(LPCTSTR pszEmail, LPCTSTR pszRegCode);

    BOOL IsTrialUseExpired();
    int GetDayOfUse(); 
    BOOL IsAntiHackingEnabled();
    void EnableAntiHacking();

// Overridables
public:
// none

// Utility member variables (Implementation variables)
private:
    enum { IDD = IDD_REGISTER_PRODUCT };

// Utility member functions (Implementation methods)
private:
    // Overridden from bases
    void DoDataExchange(CDataExchange* pDX);    
    DECLARE_MESSAGE_MAP()

    // Custom implementations
    // none
};

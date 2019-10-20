//------------------------------------------------------------------------------
// krm
//   Header file with all constants and types used by exported functions
//   of krm.dll.
//
// Copyright (C) 2004,2005 by fkWare
// First Version: 23.06.2004
//
// fkWare retains full rights to this software.
//------------------------------------------------------------------------------
//   $Author: Frank.kintrup $
// $Revision: 37 $
//  $Modtime: 26.01.06 21:33 $
//------------------------------------------------------------------------------

#if !defined(KRM_Consts)
#define KRM_Consts

#include <windows.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __cplusplus
  extern "C" {
#endif

//==============================================================================
// Constants
//==============================================================================

#define KRMDLL "krm.dll"

//------------------------------------------------------------------------------
// Version number of KRM.DLL

#define KRM_VERSIONMAJOR   1
#define KRM_VERSIONMINOR   1
#define KRM_VERSIONBUILD   3


//------------------------------------------------------------------------------

typedef struct TKRMVersionInfo{
  DWORD dwVersionInfoSize;         // Specifies the structure size, in bytes
  DWORD dwMajorVersion;            // Receives the major version number
  DWORD dwMinorVersion;            // Receives the minor version number
  DWORD dwBuildNumber;             // Receives the build number
} KRMVersionInfo, *LPKRMVersionInfo;

//------------------------------------------------------------------------------
// ID values for krmSetDataInt/SetDataStr
//------------------------------------------------------------------------------

#define krmSetUserName                0x0001
#define krmSetUserCountry             0x0006
#define krmSetExemptVAT               0x000A
#define krmSetCurrencyCode            0x000B
#define krmSetACGVar                  0x000D
#define krmSetOrderPassThroughData    0x000E
#define krmSetAffiliateTrackingData   0x000F
#define krmSetPurchaseOrderData       0x0010
#define krmSetConnectLimit            0x0020
#define krmSetInitURLTimeout          0x0021
#define krmSetInitDocTimeout          0x0022
#define krmSetTransactionTimeout      0x0023
#define krmSetSupplierName            0x0100
#define krmSetSupplierID              0x0101
#define krmSetSupplierURL             0x0102
#define krmSetSupplierEMail           0x0103
#define krmSetProductDbName           0x0110
#define krmSetProductPartNo           0x0111
#define krmSetProductSupplierSKU      0x0112
#define krmSetProductName             0x0113
#define krmSetProductVersion          0x0114
#define krmSetTransactFailURL         0x0120
#define krmSetConnectFailURL          0x0121
#define krmSetLanguage                0x1001
#define krmSetShowRegCode             0x1004
#define krmSetShowUnlockText          0x1005
#define krmSetTestOrder               0x1010
#define krmSetLanguageXML             0x1011
#define krmSetLanguageReceipt         0x1012
#define krmSetInitializationString    0x2000

//------------------------------------------------------------------------------
// ID values for krmGetResult
//------------------------------------------------------------------------------

#define krmGetStatusCode              0x0000
#define krmGetTestOrder               0x0001
#define krmGetTransactionKey          0x0010
#define krmGetTransactionID           0x0011
#define krmGetUserName                0x0012
#define krmGetLicenseKey              0x0013
#define krmGetOrderXML                0x0020

//------------------------------------------------------------------------------
// KRM CLIENT ERROR CODES.
//------------------------------------------------------------------------------

#define krmErrorOk                         0
#define krmErrorDLLNotFound               -1
#define krmErrorWrongDLLVersion           -2
#define krmErrorAlreadyInitialized        -3
#define krmErrorNotInitialized            -5
#define krmErrorException                 -6
#define krmErrorInvalidArgument           -7
#define krmErrorNoSupplierID             -11
#define krmErrorNoProductName            -12
#define krmErrorNoProductDbName          -13
#define krmErrorNoSupplierSKU            -14
#define krmErrorNoConnectFailURL         -15
#define krmErrorNoTransactFailURL        -16
#define krmErrorInvalidCurrency          -17
#define krmErrorNoValidInitString        -18
#define krmErrorUserCancel               -30
#define krmErrorInitDocURLDownloadFailed -40
#define krmErrorInitDocURLParseError     -41
#define krmErrorInitDocDownloadFailed    -42
#define krmErrorInvalidInitDoc           -43
#define krmErrorInitDocStatusError       -44
#define krmErrorInitDocParseError        -45
#define krmErrorInitDocContentsMissing   -46
#define krmErrorOrderDownloadFailed      -50
#define krmErrorOrderXMLInvalid          -51
#define krmErrorOrderXMLException        -52
#define krmErrorOrderDecodeFailed        -53
#define krmErrorOrderXMLParseError       -54
#define krmErrorOrderFailed              -55
#define krmErrorUnsupportedXMLEncoding   -56

//------------------------------------------------------------------------------
// KRM CLIENT STATUS CODES. The status codes are sent to the host
// application's callback function (if one is registered).
//------------------------------------------------------------------------------

#define krmStatusError                0x0000
#define krmStatusCancel               0x0001
#define krmStatusInitSuccess          0x0010
#define krmStatusDownloadInitDoc      0x0020
#define krmStatusInitDocSuccess       0x0021
#define krmStatusDisplayGUI           0x0030
#define krmStatusTransactionStart     0x0040
#define krmStatusTransactionResult    0x0041
#define krmStatusTransactionID        0x0042
#define krmStatusTransactionKey       0x0043
#define krmStatusTestOrder            0x0044
#define krmStatusUserName             0x0045
#define krmStatusLicenseKey           0x0046
#define krmStatusKartOrderXML         0x0047

//------------------------------------------------------------------------------
// KRM ORDER STATUS CODES (from order response message).
//------------------------------------------------------------------------------

#define krmOrderNetworkError           -2000
#define krmOrderResponseError          -1000
#define krmOrderInvalid                    0
#define krmOrderDuplicate               1000
#define krmOrderFailureNoCharge         2000
#define krmOrderFailureCharged          3000
#define krmOrderFailureUnknownCharge    4000
#define krmOrderOrderApprovalNeeded     4500
#define krmOrderSuccess                 5000

//==============================================================================
// Exported functions
//==============================================================================

BOOL __declspec(dllimport) __stdcall krmGetVersion(
  LPKRMVersionInfo lpKRMVersionInfo);
typedef BOOL(__stdcall *__krmGetVersion)(LPKRMVersionInfo);

//------------------------------------------------------------------------------

void CALLBACK __stdcall krmCallback(
  int nStatus,
  int nValue,
  LPCWSTR sValue,
  DWORD dwUser);
typedef void(__stdcall *__krmCallBack)(int, int, LPCWSTR, DWORD);

void __declspec(dllimport) __stdcall krmSetCallback(
  __krmCallBack lpkrmCallback,
  DWORD dwUser);
typedef void(__stdcall *__krmSetCallBack)(__krmCallBack, DWORD);

//------------------------------------------------------------------------------

BOOL __declspec(dllimport) __stdcall krmAddPrice(
  LPCWSTR szCurrency,
  double fPrice,
  LPCWSTR szBaseCurrency);
typedef BOOL(__stdcall *__krmAddPrice)(LPCWSTR, double, LPCWSTR);

//------------------------------------------------------------------------------

BOOL __declspec(dllimport) __stdcall krmSetDataStr(
  int nDataID,
  LPCWSTR lpString);
typedef BOOL(__stdcall *__krmSetDataStr)(int, LPCWSTR);

BOOL __declspec(dllimport) __stdcall krmSetDataInt(
  int nDataID,
  int nValue);
typedef BOOL(__stdcall *__krmSetDataInt)(int, int);

//------------------------------------------------------------------------------

int __declspec(dllimport) __stdcall krmGetLastError();
typedef int(__stdcall *__krmGetLastError)(void);

//------------------------------------------------------------------------------

BOOL __declspec(dllimport) __stdcall krmExec();
typedef BOOL(__stdcall *__krmExec)(void);

BOOL __declspec(dllimport) __stdcall krmExecModal(HWND hwndParent);
typedef BOOL(__stdcall *__krmExecModal)(HWND);

//------------------------------------------------------------------------------

BOOL __declspec(dllimport) __stdcall krmCloseWizard();
typedef BOOL(__stdcall *__krmCloseWizard)(void);

//------------------------------------------------------------------------------

BOOL __declspec(dllimport) __stdcall krmGetResultInt(
  int nResultID,
  int *nValue);
typedef int(__stdcall *__krmGetResultInt)(int, int *);

int __declspec(dllimport) __stdcall krmGetResultStr(
  int nResultID,
  LPWSTR szValue,
  int nBufSize);
typedef int(__stdcall *__krmGetResultStr)(int, LPWSTR, int);

//------------------------------------------------------------------------------

#ifdef __cplusplus
  }
#endif
#endif // !defined(KRM_Consts)


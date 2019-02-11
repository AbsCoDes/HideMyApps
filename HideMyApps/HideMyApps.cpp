// HideMyApps.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "HideMyApps.h"

#include "HideMyAppsDlg.h"

#ifdef _DEBUG
#    define new DEBUG_NEW
#endif


// CHideMyAppsApp

BEGIN_MESSAGE_MAP(CHideMyAppsApp, CBCGPWinApp)
    ON_COMMAND(ID_HELP, CBCGPWinApp::OnHelp)
END_MESSAGE_MAP()


// CHideMyAppsApp construction
CHideMyAppsApp::CHideMyAppsApp() {}

// The one and only CHideMyAppsApp object
CHideMyAppsApp theApp;

// CHideMyAppsApp initialization
BOOL CHideMyAppsApp::InitInstance() {
    CBCGPWinApp::InitInstance();

    AfxEnableControlContainer();
    AfxOleInit();

    SetRegistryKey(_T("AbsCoDes\\HideMyApps"));
    SetRegistryBase(_T("Settings"));

    // Enable Office 2016 look (Colorful theme):
    SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2016_COLORFUL);
	CBCGPVisualManager2016::SetAccentColor(CBCGPVisualManager2016::AccentColor::VS2012_Accent_Last);

	//
	globalData.m_bUseBuiltIn32BitIcons = FALSE;
	globalData.m_bUseVisualManagerInBuiltInDialogs = TRUE;
	globalData.m_bControlsVisualManagerStyle = TRUE;

    //
    CHideMyAppsDlg dlg;
    dlg.EnableLoadWindowPlacement(TRUE);

    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if(nResponse == IDOK) {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with OK
    }
    else if(nResponse == IDCANCEL) {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with Cancel
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}

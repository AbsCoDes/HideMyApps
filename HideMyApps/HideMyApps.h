// HideMyApps.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
#    error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CHideMyAppsApp : public CBCGPWinApp
{
public:
    CHideMyAppsApp();

public:
    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

extern CHideMyAppsApp theApp;

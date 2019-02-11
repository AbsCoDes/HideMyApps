// HideMyAppsDlg.h : header file
//

#pragma once

#include "AppsTabPage.h"
#include "RegTabPage.h"


class CHideMyAppsDlg : public CBCGPDialog
{
public:
    // Construction
    CHideMyAppsDlg(CWnd* pParent = NULL);

    // Dialog Data
    enum { IDD = IDD_HIDEMYAPPS_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();

    DECLARE_MESSAGE_MAP()

protected:
    HICON m_hIcon;

    CBCGPSVGImageList m_svgList;
    CBCGPSVGImage* m_svgUser;
    CBCGPSVGImage* m_svgGroup;
    CBCGPSVGImage* m_svgApps;
    CImageList m_images;

    CBCGPPropertySheetCtrl m_wndPropSheet;
    CUserAppsTabPage m_pPageUser;
	CAllAppsTabPage m_pPageAll;
	CRegTabPage m_pPageApps;
};

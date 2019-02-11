
#ifndef __AppsTabPage__H__
#define __AppsTabPage__H__

#include "AppsTreeCtrl.h"

class CAppsTabPage : public CBCGPPropertyPage
{
    DECLARE_DYNCREATE(CAppsTabPage)

public:
    CAppsTabPage(CWnd* pParent = NULL);
    virtual ~CAppsTabPage();

    enum { IDD = IDD_APP_PAGE };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    DECLARE_MESSAGE_MAP()


protected:
    virtual BOOL OnInitDialog();

    // Refresh
    void OnRefresh();
	LRESULT OnUpdateProgress(WPARAM, LPARAM);

protected:
    HACCEL m_hAccel;
    int m_csidl;

    CBCGPStatic m_Label;
    CAppsTreeCtrl m_wndTree;

    static CBCGPProgressDlg* m_pProgressDlg;
};

class CUserAppsTabPage : public CAppsTabPage
{
public:
    CUserAppsTabPage(CWnd* pParent = NULL);

protected:
    virtual BOOL OnInitDialog();
};

class CAllAppsTabPage : public CAppsTabPage
{
public:
    CAllAppsTabPage(CWnd* pParent = NULL);

protected:
    virtual BOOL OnInitDialog();
};

#endif

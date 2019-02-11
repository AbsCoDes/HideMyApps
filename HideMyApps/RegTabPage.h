
#ifndef __RegTabPage__H__
#define __RegTabPage__H__

#include "RegListCtrl.h"

class CRegTabPage : public CBCGPPropertyPage
{
    DECLARE_DYNCREATE(CRegTabPage)

public:
    CRegTabPage(CWnd* pParent = NULL);
    virtual ~CRegTabPage();

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

    CBCGPStatic m_Label;
	CRegListCtrl m_wndList;

	static CBCGPProgressDlg* m_pProgressDlg;
};

#endif

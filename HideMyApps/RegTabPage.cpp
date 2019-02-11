
#include "stdafx.h"
#include "HideMyApps.h"

#include "RegTabPage.h"

CBCGPProgressDlg* CRegTabPage::m_pProgressDlg = NULL;

struct CWorkingThreadData {
	CWorkingThreadData(HWND hwndOwner, CRegListCtrl* pWndList)
		: m_hwndOwner(hwndOwner)
		, m_pWndList(pWndList) {}

	HWND m_hwndOwner;
	CRegListCtrl* m_pWndList;
};

static UINT WorkingThread(LPVOID pParams) {
	CWorkingThreadData* pData = (CWorkingThreadData*)pParams;

	HWND hwnd = pData->m_hwndOwner;
	CRegListCtrl* pWndList = pData->m_pWndList;

	pWndList->FillList();

	delete pData;

	return 0;
}

IMPLEMENT_DYNCREATE(CRegTabPage, CBCGPPropertyPage)


CRegTabPage::CRegTabPage(CWnd* pParent /* = NULL */)
  : CBCGPPropertyPage(CRegTabPage::IDD) {

    m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));

    EnableLayout();
	EnableVisualManagerStyle(TRUE);

    //{{AFX_DATA_INIT(CRegTabPage)
    //}}AFX_DATA_INIT
}

CRegTabPage::~CRegTabPage() {}

void CRegTabPage::DoDataExchange(CDataExchange* pDX) {
    CBCGPPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CHideMyAppsDlg)
    DDX_Control(pDX, IDC_LABEL, m_Label);
	DDX_Control(pDX, IDC_LIST, m_wndList);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRegTabPage, CBCGPPropertyPage)
    //{{AFX_MSG_MAP(CRegTabPage)
    ON_WM_SIZE()
    ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_MESSAGE(UM_REG_PROGRESS, OnUpdateProgress)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CRegTabPage::OnInitDialog() {
    CBCGPPropertyPage::OnInitDialog();

    // Labels
	m_Label.SetFont(&globalData.fontBold);
	m_Label.SetWindowText(L"Items shown in Add or Remove Programs list in control panel.");

	// Tree
	GetDlgItem(IDC_TREE)->ShowWindow(SW_HIDE);

	// Fill list
	OnRefresh();

    // Layout
    CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
    if(pLayout != NULL) {
        pLayout->AddAnchor(m_wndList.GetSafeHwnd(), CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeBoth);
    }

    return TRUE; // return TRUE  unless you set the focus to a control
}

BOOL CRegTabPage::PreTranslateMessage(MSG* pMsg) {
    if(m_hAccel) {
        if(::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
            return (TRUE);
        }
    }
    return CBCGPPropertyPage::PreTranslateMessage(pMsg);
}

void CRegTabPage::OnRefresh() {
#ifdef _DEBUG
    m_wndList.FillList();
#else
	if (m_pProgressDlg != NULL) {
		delete m_pProgressDlg;
		m_pProgressDlg = NULL;
	}

	CBCGPProgressDlgParams params;
	params.m_strCaption = _T("Please wait...");
	params.m_strMessage = _T("Loading programs list");
	params.m_bShowCancel = FALSE;
	params.m_bShowProgress = TRUE;
	params.m_bShowInfiniteProgress = TRUE;
	params.m_bShowPercentage = FALSE;
	params.m_bDialogLook = TRUE;

	m_pProgressDlg = new CBCGPProgressDlg();
	m_pProgressDlg->EnableVisualManagerStyle(TRUE, TRUE);
	m_pProgressDlg->EnableDragClientArea(TRUE);
	m_pProgressDlg->Create(params, this);

	globalData.TaskBar_SetProgressState(GetSafeHwnd(), BCGP_TBPF_INDETERMINATE);

	AfxBeginThread(WorkingThread, new CWorkingThreadData(GetSafeHwnd(), &m_wndList));
#endif // _DEBUG
}

LRESULT CRegTabPage::OnUpdateProgress(WPARAM wp, LPARAM lp) {
	BOOL bIsFinished = (BOOL)wp;

	if (m_pProgressDlg->GetSafeHwnd() == NULL) {
		globalData.TaskBar_SetProgressState(GetSafeHwnd(), BCGP_TBPF_NOPROGRESS);
		return TRUE; // Stop working thread
	}

	if (bIsFinished) {
		delete m_pProgressDlg;
		m_pProgressDlg = NULL;

		globalData.TaskBar_SetProgressState(GetSafeHwnd(), BCGP_TBPF_NOPROGRESS);

		return TRUE; // Stop working thread
	}

	return FALSE; // Continue working thread
}


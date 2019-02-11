
#include "stdafx.h"
#include "HideMyApps.h"

#include "AppsTabPage.h"

#include <Shlobj.h>

CBCGPProgressDlg* CAppsTabPage::m_pProgressDlg = NULL;

struct CWorkingThreadData {
    CWorkingThreadData(HWND hwndOwner, CAppsTreeCtrl* pWndTree, int csidl)
      : m_hwndOwner(hwndOwner)
      , m_pWndTree(pWndTree)
      , m_csidl(csidl) {}

    HWND m_hwndOwner;
    CAppsTreeCtrl* m_pWndTree;
    int m_csidl;
};

static UINT WorkingThread(LPVOID pParams) {
    CWorkingThreadData* pData = (CWorkingThreadData*)pParams;

    HWND hwnd = pData->m_hwndOwner;
    int csidl = pData->m_csidl;
    CAppsTreeCtrl* pWndTree = pData->m_pWndTree;

    pWndTree->FillTree(csidl);

    delete pData;

    return 0;
}

IMPLEMENT_DYNCREATE(CAppsTabPage, CBCGPPropertyPage)


CAppsTabPage::CAppsTabPage(CWnd* pParent /* = NULL */)
  : CBCGPPropertyPage(CAppsTabPage::IDD)
  , m_csidl(0) {

    m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));

    EnableLayout();
    EnableVisualManagerStyle(TRUE);

    //{{AFX_DATA_INIT(CAppsTabPage)
    //}}AFX_DATA_INIT
}

CAppsTabPage::~CAppsTabPage() {}

void CAppsTabPage::DoDataExchange(CDataExchange* pDX) {
    CBCGPPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CHideMyAppsDlg)
    DDX_Control(pDX, IDC_LABEL, m_Label);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAppsTabPage, CBCGPPropertyPage)
    //{{AFX_MSG_MAP(CAppsTabPage)
    ON_WM_SIZE()
    ON_COMMAND(ID_REFRESH, OnRefresh)
    ON_MESSAGE(UM_APPS_PROGRESS, OnUpdateProgress)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CAppsTabPage::OnInitDialog() {
    CBCGPPropertyPage::OnInitDialog();

    // Labels
    m_Label.SetFont(&globalData.fontBold);

    // Tree
    UINT nTreeStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES | TVS_CHECKBOXES;
    m_wndTree.CreateOnPlaceHolder(this, IDC_TREE, 0, nTreeStyles);
    m_wndTree.SetVisualManagerColorTheme();

    // Fill tree
    OnRefresh();

    // List
    GetDlgItem(IDC_LIST)->ShowWindow(SW_HIDE);

    // Layout
    CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
    if(pLayout != NULL) {
        pLayout->AddAnchor(m_wndTree.GetSafeHwnd(), CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeBoth);
    }

    return TRUE; // return TRUE  unless you set the focus to a control
}

BOOL CAppsTabPage::PreTranslateMessage(MSG* pMsg) {
    if(m_hAccel) {
        if(::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
            return (TRUE);
        }
    }
    return CBCGPPropertyPage::PreTranslateMessage(pMsg);
}

void CAppsTabPage::OnRefresh() {

#ifdef _DEBUG
	m_wndTree.FillTree(m_csidl);
#else
    if(m_pProgressDlg != NULL) {
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

    AfxBeginThread(WorkingThread, new CWorkingThreadData(GetSafeHwnd(), &m_wndTree, m_csidl));
#endif // _DEBUG
}

LRESULT CAppsTabPage::OnUpdateProgress(WPARAM wp, LPARAM lp) {
    BOOL bIsFinished = (BOOL)wp;

    if(m_pProgressDlg->GetSafeHwnd() == NULL) {
        globalData.TaskBar_SetProgressState(GetSafeHwnd(), BCGP_TBPF_NOPROGRESS);
        return TRUE; // Stop working thread
    }

    if(bIsFinished) {
        delete m_pProgressDlg;
        m_pProgressDlg = NULL;

        globalData.TaskBar_SetProgressState(GetSafeHwnd(), BCGP_TBPF_NOPROGRESS);

        return TRUE; // Stop working thread
    }

    return FALSE; // Continue working thread
}

CUserAppsTabPage::CUserAppsTabPage(CWnd* pParent)
  : CAppsTabPage(pParent) {
    m_csidl = CSIDL_STARTMENU;
}

BOOL CUserAppsTabPage::OnInitDialog() {
    CAppsTabPage::OnInitDialog();

    // Labels
    m_Label.SetWindowText(L"Programs installed for current user only.");

    return TRUE;
}

CAllAppsTabPage::CAllAppsTabPage(CWnd* pParent)
  : CAppsTabPage(pParent) {
    m_csidl = CSIDL_COMMON_STARTMENU;
}

BOOL CAllAppsTabPage::OnInitDialog() {
    CAppsTabPage::OnInitDialog();

    // Labels
    m_Label.SetWindowText(L"Programs installed for ALL users.");

    return TRUE;
}

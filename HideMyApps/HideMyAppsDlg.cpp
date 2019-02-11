// HideMyAppsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HideMyApps.h"

#include "HideMyAppsDlg.h"

#ifdef _DEBUG
#    define new DEBUG_NEW
#endif


// CHideMyAppsDlg dialog
CHideMyAppsDlg::CHideMyAppsDlg(CWnd* pParent /*=NULL*/)
  : CBCGPDialog(CHideMyAppsDlg::IDD, pParent)
  , m_wndPropSheet(_T(""), this) {

    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_svgUser = new CBCGPSVGImage();
    m_svgGroup = new CBCGPSVGImage();
    m_svgApps = new CBCGPSVGImage();

    EnableVisualManagerStyle(TRUE, TRUE);
    EnableDragClientArea();
    EnableLayout(TRUE, NULL, FALSE);
}

void CHideMyAppsDlg::DoDataExchange(CDataExchange* pDX) {
    CBCGPDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CHideMyAppsDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHideMyAppsDlg, CBCGPDialog)
    //{{AFX_MSG_MAP(CHideMyAppsDlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CHideMyAppsDlg message handlers
BOOL CHideMyAppsDlg::OnInitDialog() {
    CBCGPDialog::OnInitDialog();

    // Set the hIcon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE); // Set big hIcon
    SetIcon(m_hIcon, FALSE); // Set small hIcon

    // Load svg
    m_svgUser->Load(IDR_SVG_EMPLOYEE);
    m_svgGroup->Load(IDR_SVG_TEAM);
    m_svgApps->Load(IDR_SVG_REMOVE);
    m_svgList.Add(m_svgUser);
    m_svgList.Add(m_svgGroup);
    m_svgList.Add(m_svgApps);

    // Create image list
    CBCGPToolBarImages images;
    images.SetImageSize(CSize(32, 32));
    images.AddSVGList(m_svgList);
    images.ExportToImageList(m_images);

	//
	m_pPageUser.m_psp.dwFlags |= PSP_USETITLE;
	m_pPageUser.m_psp.pszTitle = _T("Start Menu - My programs");
	m_pPageAll.m_psp.dwFlags |= PSP_USETITLE;
	m_pPageAll.m_psp.pszTitle = _T("Start Menu - Common programs");
	m_pPageApps.m_psp.dwFlags |= PSP_USETITLE;
	m_pPageApps.m_psp.pszTitle = _T("Hide / UnHide programs");


    // Create property sheet
    m_wndPropSheet.m_bIsAutoDestroy = FALSE;
    m_wndPropSheet.SetLook(CBCGPPropertySheet::PropSheetLook_Tabs);
    m_wndPropSheet.EnableDragClientArea(IsDragClientAreaEnabled());
    m_wndPropSheet.SetIconsList(m_images.GetSafeHandle());
    m_wndPropSheet.AddPage(&m_pPageUser);
    m_wndPropSheet.AddPage(&m_pPageAll);
    m_wndPropSheet.AddPage(&m_pPageApps);
    m_wndPropSheet.CreateOnPlaceHolder(this, IDC_PROP_SHEET);
    m_wndPropSheet.SetActivePage(0);

    // Layout
    CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
    if(pLayout != NULL) {
        pLayout->AddAnchor(m_wndPropSheet, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeBoth);
    }

    return TRUE; // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the hIcon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CHideMyAppsDlg::OnPaint() {
    if(IsIconic()) {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center hIcon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the hIcon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else {
        CBCGPDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHideMyAppsDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

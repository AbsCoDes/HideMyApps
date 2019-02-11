
#include "stdafx.h"
#include "RegListCtrl.h"

CRegListCtrl::CRegListCtrl() {
    EnableHeader(FALSE);
    EnableRowHeader(FALSE);
    EnableColumnAutoSize(TRUE);
    EnableGridLines(FALSE);
    EnableCheckBoxes(TRUE);

    SetRowHeaderWidth(0, FALSE);
    SetReadOnly(TRUE);
}

CRegListCtrl::~CRegListCtrl() {}

void CRegListCtrl::Init() {
    CBCGPGridCtrl::Init();
    ASSERT(::IsWindow(GetSafeHwnd()));
}

void CRegListCtrl::FillGrid() {
    ASSERT_VALID(this);
    ASSERT(GetSafeHwnd() != NULL);

    CWaitCursor wait;

    DeleteAllColumns();

    InsertColumn(0, _T("Programs"), globalUtils.ScaleByDPI(120));

    //
    CollectData();
    Fill();

    //
    AdjustLayout();
    RedrawWindow();
}

void CRegListCtrl::CollectData() {
    // HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall
    CollectData(RegistryHive::LocalMachine, RegistryView::Registry32, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
    CollectData(RegistryHive::LocalMachine, RegistryView::Registry64, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
    // HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall
    CollectData(RegistryHive::CurrentUser, RegistryView::Registry32, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
    CollectData(RegistryHive::CurrentUser, RegistryView::Registry64, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
    // HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall
    CollectData(RegistryHive::LocalMachine, RegistryView::Default, L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
    CollectData(RegistryHive::CurrentUser, RegistryView::Default, L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
}

void CRegListCtrl::CollectData(RegistryHive hive, RegistryView view, CString path) {

    std::string sPath = toStdString(path);

    try {
        // Open registry
        RegistryKey reg = RegistryKey(hive, view, RegistryAccessRights::Read).OpenSubKey(sPath);
        if(reg.IsValid()) {

            // Read all subkeys
            std::vector<std::string> subKeys = reg.EnumSubKeys();

            // Loop throught all keys
            for(size_t i = 0; i < subKeys.size(); ++i) {
                AppKey appKey;
                appKey.hive = hive;
                appKey.view = view;
                appKey.path = sPath;
                appKey.key = subKeys.at(i);

                RegistryKey regKey = reg.OpenSubKey(appKey.key);
                if(regKey.IsValid()) {
                    appKey.displayName = GetString(regKey, "DisplayName", appKey.displayName);
                    appKey.quietDisplayName = GetString(regKey, "QuietDisplayName", appKey.quietDisplayName);
                    appKey.systemComponent = GetDWord(regKey, "SystemComponent", appKey.systemComponent);
                    appKey.hma_dn = GetString(regKey, "HmaDn", appKey.name());
                    appKey.hma_sc = GetDWord(regKey, "HmaSc", appKey.systemComponent);

                    if(!appKey.hma_dn.empty() && appKey.hma_sc == 0) {
                        AppKey appKeyData = GetData(appKey.Name());
                        if(appKeyData.name().empty()) {
                            m_mapData.SetAt(appKey.Name(), appKey);
                        }
                        else {
                            appKeyData.duplicates.Add(appKey);
                            m_mapData.SetAt(appKeyData.Name(), appKeyData);
                        }
                    }
                }
            }
        }
    }
    catch(const std::exception&) {
    }
}

CRegListCtrl::AppKey CRegListCtrl::GetData(CString appKeyName) const {
    AppKey appKey;
    m_mapData.Lookup(appKeyName, appKey);
    return appKey;
}

void CRegListCtrl::Fill() {
    ASSERT_VALID(this);
    ASSERT(GetSafeHwnd() != NULL);

    CWaitCursor wait;

    // Add to the grid
    for(POSITION pos = m_mapData.GetStartPosition(); pos != NULL;) {
        AppKey appkey;
        CString name;
        m_mapData.GetNextAssoc(pos, name, appkey);

        CBCGPGridRow* pRow = CreateRow(GetColumnCount());
        pRow->GetItem(0)->SetValue((LPCTSTR)name);
        pRow->SetCheck(appkey.systemComponent);
        AddRow(pRow, FALSE);
    }

    // Sort by name
    Sort(0, TRUE);
}

void CRegListCtrl::OnRowCheckBoxClick(CBCGPGridRow* pRow) {
    ASSERT_VALID(this);
    ASSERT_VALID(pRow);

    CWnd* pOwner = GetOwner();
    if(pOwner == NULL)
        return;

    AppKey appKey = GetData(pRow->GetItem(0)->GetValue());
    BOOL checked = !pRow->GetCheck();

    bool hidden = appKey.hide(checked);
    if(hidden) {
        pRow->SetCheck(hidden ? checked : !checked);
    }
    else {
        CString message = L"An error occured while trying to hide/unhide ";
        message += appKey.Name();
        BCGP_MSGBOXPARAMS params;
        params.hwndOwner = this->GetSafeHwnd();
        params.hInstance = ::GetModuleHandle(NULL);
        params.lpszCaption = L"Erreur";
        params.lpszText = message;
        params.bUseNativeCaption = false;
        params.bUseNativeControls = false;
        params.dwStyle |= MB_ICONERROR;
        BCGPMessageBoxIndirect(&params);
    }

    AdjustLayout();
}

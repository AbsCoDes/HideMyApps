
#include "stdafx.h"
#include "RegListCtrl.h"
#include "HideMyApps.h"

#include "Commons\StringUtils.h"

IMPLEMENT_DYNAMIC(CRegListCtrl, CBCGPListBox)

CRegListCtrl::CRegListCtrl() {}

CRegListCtrl::~CRegListCtrl() {}

void CRegListCtrl::PreSubclassWindow() {

    ModifyStyle(LBS_USETABSTOPS, 0);
    EnablePins(TRUE);
    EnableItemDescription(TRUE);

    CBCGPListBox::PreSubclassWindow();
}

BEGIN_MESSAGE_MAP(CRegListCtrl, CBCGPListBox)
END_MESSAGE_MAP()

void CRegListCtrl::FillList() {
    ASSERT_VALID(this);
    ASSERT(GetSafeHwnd() != NULL);

	// Progress indicator start
	::SendMessage(GetParent()->GetSafeHwnd(), UM_REG_PROGRESS, FALSE, 0);

    //
    CleanUp();

    //
    CollectData();
    Fill();

	// Progress indicator end
	::SendMessage(GetParent()->GetSafeHwnd(), UM_REG_PROGRESS, TRUE, 0);
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
				// Progress indicator
				::SendMessage(GetParent()->GetSafeHwnd(), UM_REG_PROGRESS, FALSE, 0);

				// Key
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

                    appKey.publisher = GetString(regKey, "Publisher", "");
                    std::string displayIcon = GetString(regKey, "DisplayIcon", "");
                    appKey.hIcon = GetIcon(appKey.key, displayIcon);

                    bool isKb = appKey.key.find("}.KB") != std::string::npos || HasValue(regKey, "ParentKeyName");

                    if(!appKey.hma_dn.empty() && appKey.hma_sc == 0 && !isKb) {
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

HICON CRegListCtrl::GetIcon(std::string key, std::string path) {

    using namespace abscodes::commons;

    // Cleaning path
    path = string::trim(path, '"');
    path = string::trim(path, ' ');

    // Get icon from path
    HICON hIcon = GetIcon(toString(path), 32, 32);
    if(hIcon)
        return hIcon;

    // Try to find it
    using namespace abscodes::commons;
    if(string::startsWith(key, "{") && string::endsWith(key, "}")) {
        key = string::ltrim(key, '{');
        key = string::rtrim(key, '}');
        std::vector<std::string> v = string::split(key, '-');
        if(v.size() == 5 && v[0].size() == 8 && v[1].size() == 4 && v[2].size() == 4 && v[3].size() == 4 && v[4].size() == 12) {
            std::string s1 = v[0];
            std::string s2 = v[1];
            std::string s3 = v[2];
            std::string s4 = v[3];
            std::string s5 = v[4];
            std::string rs1 = s1;
            std::reverse(rs1.begin(), rs1.end());
            std::string rs2 = s2;
            std::reverse(rs2.begin(), rs2.end());
            std::string rs3 = s3;
            std::reverse(rs3.begin(), rs3.end());
            std::string rs4;
            rs4 += s4[1];
            rs4 += s4[0];
            rs4 += s4[3];
            rs4 += s4[2];
            std::string rs5;
            rs5 += s5[1];
            rs5 += s5[0];
            rs5 += s5[3];
            rs5 += s5[2];
            rs5 += s5[5];
            rs5 += s5[4];
            rs5 += s5[7];
            rs5 += s5[6];
            rs5 += s5[9];
            rs5 += s5[8];
            rs5 += s5[11];
            rs5 += s5[10];

            try {
                std::string s = "Installer\\Products\\" + rs1 + rs2 + rs3 + rs4 + rs5;
                RegistryKey regInstall = RegistryKey(ClassesRoot()).OpenSubKey(s);
                std::string productIcon = GetString(regInstall, "ProductIcon", "");
                hIcon = GetIcon(toString(productIcon), 32, 32);
            }
            catch(const std::exception&) {
            }

            if(hIcon)
                return hIcon;

            try {
                std::string s = "Software\\Microsoft\\Installer\\Products\\" + rs1 + rs2 + rs3 + rs4 + rs5;
                RegistryKey regInstall = RegistryKey(CurrentUser()).OpenSubKey(s);
                std::string productIcon = GetString(regInstall, "ProductIcon", "");
                hIcon = GetIcon(toString(productIcon), 32, 32);
            }
            catch(const std::exception&) {
            }

            if(hIcon)
                return hIcon;
        }
    }

    return hIcon;
}

HICON CRegListCtrl::GetIcon(CString path, int cx, int cy) {

    if(path.IsEmpty())
        return NULL;

    //
    HICON icon = (HICON)LoadImage(theApp.m_hInstance, path, IMAGE_ICON, cx, cy, LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);

    //
    if(!icon) {
        int nTokenPos = 0;
        CString strToken = path.Tokenize(_T(","), nTokenPos);

        CString exePath;
        UINT exeIndex = 0;

        while(!strToken.IsEmpty()) {
            if(exePath.IsEmpty())
                exePath = strToken;
            else if(exeIndex == 0)
                exeIndex = _ttoi(strToken);
            strToken = strToken.Tokenize(_T("+"), nTokenPos);
        }

        ExtractIconEx(exePath, exeIndex, &icon, 0, 1);
    }

    return icon;
}

AppKey CRegListCtrl::GetData(CString appKeyName) const {
    AppKey appKey;
    m_mapData.Lookup(appKeyName, appKey);
    return appKey;
}

void CRegListCtrl::Fill() {
    ASSERT_VALID(this);
    ASSERT(GetSafeHwnd() != NULL);

    CWaitCursor wait;

    // Add to the list
    int iIndex = -1;
    int nIndex = -1;
    for(POSITION pos = m_mapData.GetStartPosition(); pos != NULL;) {
		// Progress indicator
		::SendMessage(GetParent()->GetSafeHwnd(), UM_REG_PROGRESS, FALSE, 0);

		// Key
        AppKey appkey;
        CString name;
        m_mapData.GetNextAssoc(pos, name, appkey);

		// Add program to list
        nIndex = CRegListCtrl::AddString((LPCTSTR)name);
        SetItemIcon(nIndex, appkey.hIcon != NULL ? appkey.hIcon : ::LoadIcon(theApp.m_hInstance, MAKEINTRESOURCE(IDI_TRANSPARENT)));
        SetItemDescription(nIndex, toString(appkey.publisher));
        SetItemPinned(nIndex, !appkey.systemComponent);
        SetItemColorBar(nIndex,
                        !appkey.systemComponent ? CBCGPVisualManager::GetInstance()->GetControlFillColor() : CBCGPVisualManager2016::GetAccentColorRGB());
    }
}

void CRegListCtrl::OnClickPin(int nClickedItem) {
    ASSERT_VALID(this);

    CString sItem;
    CRegListCtrl::GetText(nClickedItem, sItem);
    AppKey appKey = GetData(sItem);
    BOOL pinned = CRegListCtrl::IsItemPinned(nClickedItem);

    bool good = appKey.hide(pinned);
    if(good) {
        SetItemPinned(nClickedItem, !pinned);
        SetItemColorBar(nClickedItem, !pinned ? CBCGPVisualManager::GetInstance()->GetControlFillColor() : CBCGPVisualManager2016::GetAccentColorRGB());
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
}

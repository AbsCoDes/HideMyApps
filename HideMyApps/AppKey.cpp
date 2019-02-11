#include "stdafx.h"
#include "AppKey.h"


AppKey::AppKey()
  : systemComponent(0)
  , hma_sc(0)
  , hIcon(0) {}

AppKey::~AppKey() {
	::DestroyIcon(hIcon);
}

AppKey::AppKey(const AppKey & o) {
	hive = o.hive;
	view = o.view;
	path = o.path;
	key = o.key;
	displayName = o.displayName;
	quietDisplayName = o.quietDisplayName;
	systemComponent = o.systemComponent;
	hma_dn = o.hma_dn;
	hma_sc = o.hma_sc;
	publisher = o.publisher;
	hIcon = ::CopyIcon(o.hIcon);
	duplicates.Copy(o.duplicates);
}

const AppKey& AppKey::operator=(const AppKey& src) {
	hive = src.hive;
	view = src.view;
	path = src.path;
	key = src.key;
	displayName = src.displayName;
	quietDisplayName = src.quietDisplayName;
	systemComponent = src.systemComponent;
	hma_dn = src.hma_dn;
	hma_sc = src.hma_sc;
	publisher = src.publisher;
	hIcon = ::CopyIcon(src.hIcon);
	duplicates.Copy(src.duplicates);
	return *this;
}

CString AppKey::Name() {
	return toString(name());
}

std::string AppKey::name() {
	return !displayName.empty() ? displayName : quietDisplayName;
}

bool AppKey::hide(BOOL hide) {
	for (auto i = 0; i < duplicates.GetCount(); i++) {
		if (!duplicates.GetAt(i).hide(hide))
			return false;
	}
	try {
		auto regKey = RegistryKey(hive, view, RegistryAccessRights::AllAccess).OpenSubKey(path + '\\' + key);
		SetString(regKey, "HmaDn", hma_dn);
		SetDWord(regKey, "HmaSc", hma_sc);
		if (hide) {
			SetString(regKey, "QuietDisplayName", hma_dn);
			SetDWord(regKey, "SystemComponent", 1);
			DeleteValue(regKey, "DisplayName");
		}
		else {
			SetString(regKey, "DisplayName", hma_dn);
			SetDWord(regKey, "SystemComponent", hma_sc);
			DeleteValue(regKey, "QuietDisplayName");
		}
		return true;
	}
	catch (const std::exception&) {
		return false;
	}
}
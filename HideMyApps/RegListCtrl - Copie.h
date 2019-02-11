
#ifndef __RegListCtrl__H__
#define __RegListCtrl__H__


#include <Registry\Registry.h>
using namespace abscodes::registry;


class CRegListCtrl : public CBCGPGridCtrl
{
    struct AppKey {
        RegistryHive hive;
        RegistryView view;
        std::string path;
        std::string key;
        std::string displayName;
        std::string quietDisplayName;
        DWORD systemComponent = 0;
        std::string hma_dn;
        DWORD hma_sc = 0;

        CArray<AppKey, AppKey> duplicates;

        AppKey() {}
        AppKey(const AppKey& o) {
            hive = o.hive;
            view = o.view;
            path = o.path;
            key = o.key;
            displayName = o.displayName;
            quietDisplayName = o.quietDisplayName;
            systemComponent = o.systemComponent;
            hma_dn = o.hma_dn;
            hma_sc = o.hma_sc;
            duplicates.Copy(o.duplicates);
        }
        const AppKey& operator=(const AppKey& src) {
            hive = src.hive;
            view = src.view;
            path = src.path;
            key = src.key;
            displayName = src.displayName;
            quietDisplayName = src.quietDisplayName;
            systemComponent = src.systemComponent;
            hma_dn = src.hma_dn;
            hma_sc = src.hma_sc;
            duplicates.Copy(src.duplicates);
            return *this;
        }
        std::string name() {
            return !displayName.empty() ? displayName : quietDisplayName;
        }
        CString Name() {
            return toString(name());
        }
        bool hide(BOOL hide) {
            for(auto i = 0; i < duplicates.GetCount(); i++) {
                if(!duplicates.GetAt(i).hide(hide))
                    return false;
            }
            try {
                auto regKey = RegistryKey(hive, view, RegistryAccessRights::AllAccess).OpenSubKey(path + '\\' + key);
                SetString(regKey, "HmaDn", hma_dn);
                SetDWord(regKey, "HmaSc", hma_sc);
                if(hide) {
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
            catch(const std::exception&) {
                return false;
            }
        }
    };

public:
    CRegListCtrl();
    virtual ~CRegListCtrl();

public:
    virtual void Init();
    virtual void FillGrid();

protected:
    void Fill();
    void CollectData();
    void CollectData(RegistryHive hive, RegistryView view, CString path);

    AppKey GetData(CString appKeyName) const;

protected:
    virtual void OnRowCheckBoxClick(CBCGPGridRow* pRow);

protected:
    CMap<CString, LPCWSTR, AppKey, AppKey> m_mapData;
};

#endif

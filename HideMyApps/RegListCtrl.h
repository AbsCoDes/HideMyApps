
#ifndef __RegListCtrl__H__
#define __RegListCtrl__H__


#include "AppKey.h"

#define UM_REG_PROGRESS WM_USER + 102

class CRegListCtrl : public CBCGPListBox
{
	DECLARE_DYNAMIC(CRegListCtrl)

public:
    CRegListCtrl();
    virtual ~CRegListCtrl();

protected:
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()

public:
    virtual void FillList();

protected:
    void Fill();
    void CollectData();
    void CollectData(RegistryHive hive, RegistryView view, CString path);

	HICON GetIcon(std::string key, std::string path);
	HICON GetIcon(CString path, int cx, int cy);
    AppKey GetData(CString appKeyName) const;

protected:
	void OnClickPin(int nClickedItem);

protected:
    CMap<CString, LPCWSTR, AppKey, AppKey> m_mapData;
};

#endif


#ifndef __AppsTreeCtrl__H__
#define __AppsTreeCtrl__H__

#include "FileInfo.h"

#define UM_APPS_PROGRESS WM_USER + 101

class CAppsTreeCtrl : public CBCGPTreeCtrlEx
{
	DECLARE_DYNAMIC(CAppsTreeCtrl)

public:
    CAppsTreeCtrl();
    virtual ~CAppsTreeCtrl();

protected:
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()

public:
    virtual void Init();
    virtual void FillTree(int csidl);

protected:
    void Fill(int csidl);
    void Fill(CString path, HTREEITEM hRoot);

    HTREEITEM AddItem(FileInfo fileInfo, HTREEITEM hParent = TVI_ROOT);
    int CompareItems(FileInfo fileInfo, HTREEITEM hItem) const;

    void SetItemData(HTREEITEM hItem, FileInfo fileInfo);
    FileInfo GetItemData(HTREEITEM hItem) const;

protected:
    virtual void OnRowCheckBoxClick(CBCGPGridRow* pRow);

protected:
    CImageList m_treeImages;
    CMap<HTREEITEM, HTREEITEM, FileInfo, FileInfo> m_mapData;
};

#endif

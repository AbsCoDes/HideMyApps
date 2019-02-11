#include "stdafx.h"
#include "AppsTreeCtrl.h"


IMPLEMENT_DYNAMIC(CAppsTreeCtrl, CBCGPTreeCtrlEx)

CAppsTreeCtrl::CAppsTreeCtrl() {}

CAppsTreeCtrl::~CAppsTreeCtrl() {
    m_treeImages.DeleteImageList();
}

void CAppsTreeCtrl::PreSubclassWindow() {

    m_treeImages.Create(16, 16, ILC_COLOR32, 0, 0);

    CBCGPTreeCtrlEx::PreSubclassWindow();
}

BEGIN_MESSAGE_MAP(CAppsTreeCtrl, CBCGPTreeCtrlEx)
END_MESSAGE_MAP()


void CAppsTreeCtrl::Init() {
    CBCGPGridCtrl::Init();

    ASSERT(::IsWindow(GetSafeHwnd()));

    SetImageList(&m_treeImages, TVSIL_NORMAL);

    EnableHeader(FALSE);
    EnableColumnAutoSize(TRUE);
    EnableGridLines(FALSE);
    EnableCheckBoxes(TRUE);
    SetReadOnly(TRUE);
}

void CAppsTreeCtrl::FillTree(int csidl) {
    ASSERT_VALID(this);
    ASSERT(GetSafeHwnd() != NULL);

    DeleteAllItems();
    DeleteAllColumns();

    InsertColumn(0, _T("Start menu"), globalUtils.ScaleByDPI(120));

    //
    Fill(csidl);

    //
    AdjustLayout();
    RedrawWindow();
}

void CAppsTreeCtrl::Fill(int csidl) {
    ASSERT_VALID(this);
    ASSERT(GetSafeHwnd() != NULL);

	// Progress indicator start
    ::SendMessage(GetParent()->GetSafeHwnd(), UM_APPS_PROGRESS, FALSE, 0);

    // Get start menu location
    WCHAR path[MAX_PATH];
    HRESULT hr = ::SHGetFolderPath(NULL, csidl, NULL, 0, path);

    // Add folders
    if(SUCCEEDED(hr)) {
        Fill(path, TVI_ROOT);
    }

	// Progress indicator end
    ::SendMessage(GetParent()->GetSafeHwnd(), UM_APPS_PROGRESS, TRUE, 0);
}

void CAppsTreeCtrl::Fill(CString path, HTREEITEM hRoot) {
    // Object to enumerate files and folders
    CFileFind finder;

    // Create wildcard
    CString wildcard(path + _T("\\*.*"));

    // Init the file finding job
    BOOL working = finder.FindFile(wildcard);

    // For each file that is found:
    while(working) {

        // Progress indicator
        ::SendMessage(GetParent()->GetSafeHwnd(), UM_APPS_PROGRESS, FALSE, 0);

        // Update finder status with new file
        working = finder.FindNextFile();

        // Skip '.' and '..'
        if(finder.IsDots())
            continue;

        // Skip system files
        if(finder.IsSystem())
            continue;

        // Get file info
        SHFILEINFO info = {0};
        HTREEITEM hRootF = {0};
        DWORD_PTR hr = ::SHGetFileInfo(finder.GetFilePath(), FILE_ATTRIBUTE_NORMAL, &info, sizeof(info), SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_OPENICON);

        // Fill file info structure
        FileInfo fileInfo;
        fileInfo.hIcon = info.hIcon;
        fileInfo.path = finder.GetFilePath();
        fileInfo.displayName = info.szDisplayName;
        fileInfo.isDirectory = finder.IsDirectory();
        fileInfo.isHidden = finder.IsHidden();


        // Add to the structure
        if(SUCCEEDED(hr)) {
            hRootF = AddItem(fileInfo, hRoot);
        }

        // Add sub-directories
        if(fileInfo.isDirectory) {
            Fill(finder.GetFilePath(), hRootF);
            Expand(hRootF, hRoot == TVI_ROOT ? TVE_EXPAND : TVE_COLLAPSE);
        }
    }

    // Cleanup file finder
    finder.Close();
}

HTREEITEM CAppsTreeCtrl::AddItem(FileInfo fileInfo, HTREEITEM hParent /*= TVI_ROOT*/) {

    // Add image to list
    int nIndex = m_treeImages.Add(fileInfo.hIcon);

    //
    HTREEITEM hInsertAfter = TVI_FIRST;

    if(hParent == TVI_ROOT || ItemHasChildren(hParent)) {
        // Get first child of hParent
        HTREEITEM hItem = (hParent == TVI_ROOT) ? (GetRowCount() ? TreeItem(GetRow(0)) : NULL) : GetChildItem(hParent);

        // Loop throw all childs
        while(hItem != NULL) {
            // Insert before
            if(CompareItems(fileInfo, hItem) < 0) {
                hItem = NULL;
            }
            // Insert after
            else {
                hInsertAfter = (hItem == NULL) ? TVI_FIRST : hItem;
                hItem = GetNextSiblingItem(hItem);
            }
        }
    }

    // Add to the tree
    HTREEITEM hNewItem = InsertItem(fileInfo.displayName, nIndex, nIndex, hParent, hInsertAfter);
    SetItemData(hNewItem, fileInfo);
    // Check the box if the file is isHidden
    TreeItem(hNewItem)->SetCheck(!fileInfo.isDirectory && fileInfo.isHidden);
    TreeItem(hNewItem)->UpdateParentCheckbox(TRUE);

    return hNewItem;
}

int CAppsTreeCtrl::CompareItems(FileInfo fileInfo, HTREEITEM hItem) const {
    // Directory first
    if(fileInfo.isDirectory != GetItemData(hItem).isDirectory) {
        if(fileInfo.isDirectory)
            return -1;
        else
            return 1;
    }
    // Compare strings
    CString str = (TreeItem(hItem)->GetItem(0))->GetLabel();
    return fileInfo.displayName.Compare(str);
}

void CAppsTreeCtrl::SetItemData(HTREEITEM hItem, FileInfo fileInfo) {
    m_mapData.SetAt(hItem, fileInfo);
}

FileInfo CAppsTreeCtrl::GetItemData(HTREEITEM hItem) const {
    FileInfo fileInfo;
    m_mapData.Lookup(hItem, fileInfo);
    return fileInfo;
}

void CAppsTreeCtrl::OnRowCheckBoxClick(CBCGPGridRow* pRow) {
    ASSERT_VALID(this);
    ASSERT_VALID(pRow);

    CWnd* pOwner = GetOwner();
    if(pOwner == NULL)
        return;

    FileInfo info = GetItemData(TreeItem(pRow));
    BOOL checked = !pRow->GetCheck();

    if(info.isDirectory) {
        CList<CBCGPGridRow*, CBCGPGridRow*> lstSubItems;
        pRow->GetSubItems(lstSubItems, TRUE);

        for(POSITION pos = lstSubItems.GetHeadPosition(); pos != NULL;) {
            CBCGPGridRow* pChild = lstSubItems.GetNext(pos);
            ASSERT_VALID(pChild);

            FileInfo childInfo = GetItemData(TreeItem(pChild));
            if(!childInfo.isDirectory) {
                childInfo.hide(checked);
                pChild->SetCheck(checked);
                pChild->UpdateParentCheckbox(TRUE);
            }
        }
    }
    else {
        info.hide(checked);
        pRow->SetCheck(checked);
        pRow->UpdateParentCheckbox(TRUE);
    }

    AdjustLayout();
}

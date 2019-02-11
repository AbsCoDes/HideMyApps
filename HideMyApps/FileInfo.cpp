#include "stdafx.h"
#include "FileInfo.h"


FileInfo::FileInfo()
  : hIcon(nullptr)
  , isDirectory(false)
  , isHidden(false) {}

FileInfo::~FileInfo() {
    ::DestroyIcon(hIcon);
}

FileInfo::FileInfo(const FileInfo & o) {
	hIcon = ::CopyIcon(o.hIcon);
	path = o.path;
	displayName = o.displayName;
	isDirectory = o.isDirectory;
	isHidden = o.isHidden;

	::DestroyIcon(o.hIcon);
}

const FileInfo& FileInfo::operator=(const FileInfo& src) {
	hIcon = ::CopyIcon(src.hIcon);
	path = src.path;
	displayName = src.displayName;
	isDirectory = src.isDirectory;
	isHidden = src.isHidden;

	::DestroyIcon(src.hIcon);

	return *this;
}

void FileInfo::hide(BOOL hide) {
    if(path.IsEmpty())
        return;
    // Get file attibutes
    int attr = GetFileAttributes(path);
    // Hiding the file
    if(hide && (attr & FILE_ATTRIBUTE_HIDDEN) == 0) {
        SetFileAttributes(path, attr | FILE_ATTRIBUTE_HIDDEN);
    }
    // Unhiding the file
    if(!hide && (attr & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN) {
        SetFileAttributes(path, attr & ~FILE_ATTRIBUTE_HIDDEN);
    }
}

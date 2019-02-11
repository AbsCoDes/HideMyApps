#pragma once

struct FileInfo
{
public:
	FileInfo();
	~FileInfo();

	FileInfo(const FileInfo& o);
	const FileInfo& operator=(const FileInfo& src);

public:
	void hide(BOOL hide);

public:
	HICON hIcon;
	CString path;
	CString displayName;
	BOOL isDirectory;
	BOOL isHidden;
};


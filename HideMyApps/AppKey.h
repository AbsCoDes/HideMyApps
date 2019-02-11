#pragma once

#include <Registry\Registry.h>
using namespace abscodes::registry;

class AppKey
{
public:
	AppKey();
	~AppKey();

	AppKey(const AppKey& o);
	const AppKey& operator=(const AppKey& src);

public:
	CString Name();
	std::string name();

	bool hide(BOOL hide);

public:
	RegistryHive hive;
	RegistryView view;
	std::string path;
	std::string key;
	std::string displayName;
	std::string quietDisplayName;
	DWORD systemComponent;

	std::string hma_dn;
	DWORD hma_sc;

	std::string publisher;
	HICON hIcon;

	CArray<AppKey, AppKey> duplicates;
};


init:
	# git submodule add https://github.com/AbsCoDes-lib/Commons.git Commons
	# git submodule add https://github.com/AbsCoDes-lib/Registry.git Registry
	git config diff.submodule log
	git config status.submoduleSummary true
	git config fetch.recurseSubmodules on-demand

update:
	git submodule sync --recursive
	git submodule update --init --recursive --remote

pull:
	git pull
	update
help:
	@echo Nota informativa

backup:
	@([ -d ${FOLDER}.bak ] && echo "?Error" ) || cp -rp ${FOLDER} ${FOLDER}.bak

restore:
	@([ -d ${FOLDER} ] && echo "?Error" ) || cp -rp ${FOLDER}.bak ${FOLDER}

.PHONY: help backup restore
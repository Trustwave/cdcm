MAJOR	:= 0
MINOR	:= 1
BUILD	:= 01
HOTFIX	:=

VER 	:= $(MAJOR).$(MINOR).$(BUILD)

ifneq ($(HOTFIX),)
	VERSION := $(VER).$(HOTFIX)
else
	VERSION := $(VER)
endif

DATE	:= $(shell date +%Y%m%d%H%M%S)
YMD		:= $(shell date +%Y-%m-%d)

UPLOAD	:= 0

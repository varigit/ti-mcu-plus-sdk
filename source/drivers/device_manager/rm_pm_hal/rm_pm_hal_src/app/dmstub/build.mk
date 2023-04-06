# Copyright (C) 2021 Texas Instruments Incorporated - http://www.ti.com/
# ALL RIGHTS RESERVED

cppflags-y += -I$(objtree)/include -I$(srctree)/include -I$(srcroot)/include
cppflags-y += -I$(srctree)/security/include
cppflags-y += -I$(srctree)/tests/include
cppflags-y += -I$(TOOLCHAIN_PATH_M3)/include
cppflags-$(CONFIG_DEVICE_TYPE_HS) += -I$(srctree)/hs/security/include

cppflags-y += -I$(BIOS_PACKAGES_DIR)/ti/sysbios/posix

cppflags-y += -I$(BIOS_INSTALL_PATH)/packages
cppflags-y += -I$(XDC_INSTALL_PATH)/packages

cppflags-y += -I$(srctree)/include/soc/$(TARGET_SOC)/
cppflags-y += -I$(BIOS_INSTALL_PATH)/packages

cflags-y += $(cppflags-y)
cflags-y += -g --opt_for_speed=0 $(OPT_LEVEL) -pdr -pdew -pds=230 -pds=1463
cflags-y += --gen_func_subsections=on
cflags-y += -me
cflags-y += --display_error_number --diag_warning=255 --diag_wrap=off
aflags-y += -g $(cppflags-y)

cflags-y += --unaligned_access=off
cflags-y += -mv7R5

aflags-y += -mv7R5 -me

ldflags-y += --disable_auto_rts --unused_section_elimination=on

stripflags-y += -pdew -p

binflags-y += -b --byte --image --zero --memwidth=8 --linkerfill -fill 0x00000000 -pdew --issue_remarks -q
prepare_headers-y += $(srctree)/include/config.h

obj-y += entry.o startup.o

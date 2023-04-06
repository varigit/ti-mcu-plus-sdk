cppflags-$(CONFIG_LPM_DM_STUB) += -I$(srctree)/lpm/include
cppflags-$(CONFIG_LPM_DM_STUB) += -I$(srctree)/pm/include
cppflags-$(CONFIG_LPM_DM_STUB) += -I$(srctree)/lpm/include/soc/$(TARGET_SOC)

obj-y += soc/

obj-y += main.o
obj-y += psc_raw.o
obj-y += pll_16fft_raw.o
obj-y += ctrlmmr_raw.o
obj-y += 8250.o
obj-y += timeout.o
obj-y += sec_proxy.o
obj-y += ddr.o
obj-y += vim_raw.o
obj-y += lpm_string.o
obj-$(CONFIG_LPM_DM_TRACE) += lpm_trace.o

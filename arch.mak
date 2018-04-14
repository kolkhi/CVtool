## Architecture specific Makefile macros


ifndef $(SYSNAME)
  # Detect host architecture
  SYSNAME = $(shell uname -s | cut -d '_' -f 1 | tr [:upper:] [:lower:])

  ifeq ($(SYSNAME),mingw32)
    # MinGW
    ARCH       := win32
  endif

  ifeq ($(SYSNAME),mingw64)
    # MinGW
    ARCH       := win32
    MACHINE    := x64
  endif

  ifeq ($(SYSNAME),msys)
    # MSYS
    ARCH       := win32
  endif

  ifeq ($(SYSNAME),linux)
    # Linux
    ARCH       := linux
    NativePath = $1
  endif

  ifeq ($(SYSNAME),darwin)
    # Mac OS
    ARCH       := mac
    NativePath = $1
  endif

  # Setup native path macro for Windows
  ifeq ($(ARCH),win32)
    NativePath = $(strip $(subst /,\\,                                     \
      $(if $(filter //,$(shell echo '$1'|cut -c 1,3)),                     \
        $(shell echo '$1'|cut -c 2):$(shell echo '$1'|cut -c 3-),$1)))
  endif

  # Check for 64-bit x86 processor
  ifeq ($(shell uname -m),x86_64)
    MACHINE := x64
  endif

  # Check for ARMv7 processor
  ifeq ($(shell uname -m),armv7l)
    MACHINE := armv7l
  endif

  # Check for i.MX6 processor
  ifeq ($(SYSNAME),linux)
    ifeq ($(findstring i.MX6,$(shell grep i.MX6 /proc/cpuinfo)),i.MX6)
      MACHINE := imx6
    endif
  endif
  ifeq ($(BUILD_TARGET),i.MX6)
    MACHINE := imx6
  endif

  GetCFlags  = $(foreach d,$(subst :, ,:$1),-I"$(d)")
  GetLdFlags = $(foreach d,$(subst :, ,:$1),-L"$(d)")

endif

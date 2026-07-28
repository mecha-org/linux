include Makefile.variables
include Makefile.automotive

#
# Deprecated variables
#
# This section is intentionally left at the top of the Makefile so it won't be ignored.
#

# Variables do not have to be configured for help output.
ifeq ($(filter dist-full-help dist-help dist-help-% dist-helper-%,$(MAKECMDGOALS)),)

# These entries are 'duplicates' of variables specified in Makefile.variables
# that are used in the SPEC file.  Specifying these with a SPEC prefix indicates
# that the value is passed through to the spec file.
SPECRELEASED_KERNEL=$(RELEASED_KERNEL)
SPECINCLUDE_FEDORA_FILES=$(INCLUDE_FEDORA_FILES)
SPECINCLUDE_RHEL_FILES=$(INCLUDE_RHEL_FILES)
SPECINCLUDE_RT_FILES=$(INCLUDE_RT_FILES)
SPECINCLUDE_AUTOMOTIVE_FILES=$(INCLUDE_AUTOMOTIVE_FILES)

ifneq (,$(findstring n,$(firstword -$(MAKEFLAGS))))
  # Do not set RHTEST on the command line. Use the make command built-in options
  # -n, --just-print, --dry-run, --recon on the command line.
  RHTEST=1
endif

LANG=C

ifndef RHSELFTESTDATA
  TOPDIR:=$(shell $(GIT) rev-parse --show-toplevel)
else
  # change TOPDIR to be relative
  TOPDIR:=..
  # RHEL_RELEASE may be updated which will cause a difference with the BUILD variable
  RHEL_RELEASE:=6
  BUILD:=$(RHEL_RELEASE)
endif

REDHAT:=$(TOPDIR)/redhat
include $(TOPDIR)/Makefile.rhelver

ifndef RHDISTDATADIR
  RHDISTDATADIR=${REDHAT}/self-test/data
endif

RPMBUILD:=rpmbuild
ifeq ($(shell command -v $(RPMBUILD)),)
  $(error $(RPMBUILD) not found in PATH)
endif

MACH:=$(shell uname -m)
SPECKVERSION:=$(shell $(GIT) show $(HEAD):Makefile | sed -ne '/^VERSION\ =\ /{s///;p;q}')
SPECKPATCHLEVEL:=$(shell $(GIT) show $(HEAD):Makefile | sed -ne '/^PATCHLEVEL\ =\ /{s///;p;q}')
SPECKSUBLEVEL:=$(shell $(GIT) show $(HEAD):Makefile | sed -ne '/^SUBLEVEL\ =\ /{s///;p;q}')
SPECKEXTRAVERSION:=$(shell $(GIT) show $(HEAD):Makefile | sed -ne '/^EXTRAVERSION\ =\ /{s///;p;q}')

SPECRVERSION:=$(shell echo $$(( $(RHEL_MAJOR) - 9 )))
SPECRPATCHLEVEL:=$(RHEL_REBASE_NUM)

ifneq ($(SPECGEMINI),0)
  SPECKEXTRAVERSION:=
endif

# When building the automotive kernel package, set AUTOMOTIVE_BUILD=1.
ifeq ($(AUTOMOTIVE_BUILD), 1)
  SPECPACKAGE_NAME:=$(AUTOMOTIVE_PKG_NAME)
  DIST:=$(AUTOMOTIVE_PKG_DIST)
endif

GITID:= $(shell $(GIT) log --max-count=1 --pretty=format:%H $(HEAD))

# Derivative kernel version elements used in the package NVR
# If a 'dot' or any other separator is required, include it in DERIVATIVE_TAG.
ifeq ("$(DERIVATIVE_STREAM)", "yes")
  DERIVATIVE_VERSION:=$(DERIVATIVE_TAG).$(DERIVATIVE_BUILD)
endif

ifndef RHSELFTESTDATA
  BUILD:=$(RHEL_RELEASE)$(DERIVATIVE_VERSION)
endif
FLAVOR=
CURARCH := $(shell uname -m)
ARCHCONFIG := $(shell uname -m | sed -e s/x86_64/X86_64/ \
				     -e s/s390x/S390/ -e s/ppc.*/PPC/ )

# rpm information
SPECFILE:=$(SPECPACKAGE_NAME).spec
RPM:=$(REDHAT)/rpm
SRPMS:=$(RPM)/SRPMS
SOURCES:=$(RPM)/SOURCES
TESTPATCH:=$(REDHAT)/linux-kernel-test.patch
CHANGELOG_EXT:=changelog-$(RHEL_MAJOR).$(RHEL_MINOR)
SPECCHANGELOG:=$(SPECPACKAGE_NAME).$(CHANGELOG_EXT)
ARCH_LIST=aarch64 ppc64le s390x x86_64 riscv64

# save some user selectable values to know later if we can override them
DISTRO_USERDEF:=$(DISTRO)
DIST_USERDEF:=$(DIST)
BUILD_PROFILE_USERDEF:=$(BUILD_PROFILE)
BUILD_TARGET_USERDEF:=$(BUILD_TARGET)

ifndef DIST
  DIST := $(shell $(RPMBUILD) --eval '%{?dist}')
endif

ifndef DISTRO
  ifneq ($(findstring .fc,$(DIST)),)
    DISTRO := fedora
  else
    DISTRO := rhel
  endif
endif

ifndef RHJOBS
  RHJOBS=$(shell j=$$(echo $(MAKEFLAGS) | grep -Eo "(^|[ ])-j[0-9]*" | xargs ); \
	  if [ -z "$${j}" ]; then \
		echo "4"; \
	  else \
	  	j=$$(echo "$${j}" | tr -d "\-j"); \
	  	[ -z "$${j}" ] && nproc || echo $${j}; \
	  fi)
endif

# for official builds use RELEASE_LOCALVERSION persisted in Makefile.variables
ifneq ($(filter $(MAKECMDGOALS),dist-release dist-release-tag dist-release-git dist-rtg dist-get-tag),)
  DISTLOCALVERSION:=$(RELEASE_LOCALVERSION)
  ifeq (,$(findstring s,$(firstword -$(MAKEFLAGS))))
    $(info DISTLOCALVERSION is "$(DISTLOCALVERSION)".)
  endif
else
  LOCVERFILE:=../localversion
  # create an empty localversion file if you don't want a local buildid
  ifneq ($(wildcard $(LOCVERFILE)),)
    DISTLOCALVERSION:=$(shell cat $(LOCVERFILE))
    ifeq (,$(findstring s,$(firstword -$(MAKEFLAGS))))
      $(info DISTLOCALVERSION is "$(DISTLOCALVERSION)". Update '$(shell dirname $(REDHAT))/localversion' to change.)
    endif
  else
    ifeq ($(DISTLOCALVERSION),)
      DISTLOCALVERSION:=.test
    endif
    ifeq (,$(findstring s,$(firstword -$(MAKEFLAGS))))
      $(info DISTLOCALVERSION is "$(DISTLOCALVERSION)".)
    endif
  endif
endif # MAKECMDGOALS

# options for process_configs.sh script
ifdef NO_CONFIGCHECKS
   PROCESS_CONFIGS_CHECK_OPTS=-w -n -c -i -t
   PROCESS_CONFIGS_OPTS=-w -n -c -i
   BUILDOPTS += -configchecks
else
   PROCESS_CONFIGS_CHECK_OPTS=-n -t -c
   PROCESS_CONFIGS_OPTS=-n -w -c
endif

# this section is needed in order to make O= to work
_OUTPUT := ..
ifeq ("$(origin O)", "command line")
  _OUTPUT := $(O)
  _EXTRA_ARGS := O=$(_OUTPUT)
endif

# MARKER is the upstream git tag which we base off of for exporting patches.
# MARKER (and UPSTREAM_TARBALL_NAME) must use SPECKVERSION, SPECKPATCHLEVEL,
# SPECKEXTRAVERSION, and SPECKSUBLEVEL from the top-level kernel makefile as
# opposed to any adjusted version for snapshotting.
ifneq ($(SPECKEXTRAVERSION),)
  UPSTREAMBUILD:=0$(shell echo $(SPECKEXTRAVERSION) | sed -e s/-/./).
  ifeq ($(SPECKSUBLEVEL),0)
    UPSTREAM_TARBALL_NAME:=$(SPECKVERSION).$(SPECKPATCHLEVEL)$(SPECKEXTRAVERSION)
  else
    UPSTREAM_TARBALL_NAME:=$(SPECKVERSION).$(SPECKPATCHLEVEL).$(SPECKSUBLEVEL)$(SPECKEXTRAVERSION)
    # stable-rc's are not tagged
    STABLERC_MARKER:=$(shell $(GIT) rev-parse -q --verify origin/$(UPSTREAM_BRANCH) | cut -c 1-12 || \
            $(GIT) rev-parse -q --verify $(UPSTREAM_BRANCH) | cut -c 1-12)
  endif
else
  ifeq ($(SPECKSUBLEVEL),0)
    UPSTREAM_TARBALL_NAME:=$(SPECKVERSION).$(SPECKPATCHLEVEL)
  else
    # MARKER (and UPSTREAM_TARBALL_NAME) must use SPECKVERSION,
    # SPECKPATCHLEVEL, and SPECKSUBLEVEL from the top-level kernel makefile as
    # opposed to any adjusted version for snapshotting.  IOW, these variables are
    # *NOT* equivalent to the SPECVERSION variable.
    UPSTREAM_TARBALL_NAME:=$(SPECKVERSION).$(SPECKPATCHLEVEL).$(SPECKSUBLEVEL)
  endif
  UPSTREAMBUILD:=
endif
ifneq ($(STABLERC_MARKER),)
  MARKER:=$(STABLERC_MARKER)
else
  MARKER:=v$(UPSTREAM_TARBALL_NAME)
endif

# If VERSION_ON_UPSTREAM is set, the versioning of the rpm package is based
# on a branch tracking upstream. This allows for generating rpms
# based on untagged releases.
ifndef VERSION_ON_UPSTREAM
  ifeq ("$(DISTRO)", "fedora")
    VERSION_ON_UPSTREAM:=1
  else
    VERSION_ON_UPSTREAM:=0
  endif
endif

UPSTREAM:=$(shell $(GIT) rev-parse -q --verify origin/$(UPSTREAM_BRANCH) || \
            $(GIT) rev-parse -q --verify $(UPSTREAM_BRANCH))

ifneq ("$(wildcard $(TOPDIR)/localversion-next)", "")
  # we are building linux-next
  MARKER:=$(shell $(GIT) describe --abbrev=12 $(UPSTREAM))
  # The merge window is weird because the actual versioning hasn't
  # been updated but we still need something that works for
  # packaging. Fix this by bumping the patch level.
  ifeq ($(SPECKEXTRAVERSION),)
    SPECKPATCHLEVEL:=$(shell expr $(SPECKPATCHLEVEL) + 1)
  endif
  LINUXNEXT_VERSION:=$(shell echo $(MARKER) | sed 's!next-!!')
  UPSTREAMBUILD:=0.0.next.$(LINUXNEXT_VERSION).
  UPSTREAM_TARBALL_NAME:=$(MARKER)
else ifeq ($(VERSION_ON_UPSTREAM),1)
  # UPSTREAM_BRANCH is expected to track mainline.
  ifeq ($(UPSTREAM),)
      $(error "Missing an $(UPSTREAM_BRANCH) branch")
  endif
  MERGE_BASE:=$(shell $(GIT) merge-base $(HEAD) $(UPSTREAM))
  _TAG:=$(shell $(GIT) describe --abbrev=12 $(MERGE_BASE))
  # a snapshot off of a tagged git is of the form [tag]-[cnt]-g[hash]
  SNAPSHOT:=$(shell echo $(_TAG) | grep -c '\-g')

  ifeq ($(SNAPSHOT),1)
    # The base for generating tags is the snapshot commit
    MARKER:=$(shell echo $(_TAG) | awk -F "-g" '{ print $$2 }' | cut -c 1-12)
    # The merge window is weird because the actual versioning hasn't
    # been updated but we still need something that works for
    # packaging. Fix this by bumping the patch level and marking
    # this as rc0
    ifeq ($(SPECKEXTRAVERSION),)
      UPSTREAMBUILD:=0.rc0.
      SPECKPATCHLEVEL:=$(shell expr $(SPECKPATCHLEVEL) + 1)
    endif
    ifndef UPSTREAMBUILD_GIT_ONLY
      ifneq ($(filter $(MAKECMDGOALS),dist-git-test dist-git),)
        UPSTREAMBUILD_GIT_ONLY:=0
      else
        UPSTREAMBUILD_GIT_ONLY:=1
      endif
    endif
    ifeq ($(UPSTREAMBUILD_GIT_ONLY),1)
      UPSTREAMBUILD:=$(UPSTREAMBUILD)$(MARKER).
    else
      # Obtain the date that HEAD was committed (not the snapshot commit).
      HEAD_DATE:=$(shell $(GIT) show -s --format=%cd --date=format:%y%m%d $(HEAD))
      UPSTREAMBUILD:=$(UPSTREAMBUILD)$(HEAD_DATE)g$(MARKER).
    endif
    UPSTREAM_TARBALL_NAME:=$(patsubst v%,%,$(_TAG))
  endif
else
  SNAPSHOT:=0
  ifneq ($(ADD_COMMITID_TO_VERSION),)
    _EXACT_TAG:=$(shell $(GIT) describe --exact-match 2>/dev/null)
    ifeq ($(_EXACT_TAG),)
      _TAG:=$(shell $(GIT) describe --abbrev=12 2>/dev/null)
      ifneq ($(_TAG),)
        _BUILDCOMMIT:=$(shell echo $(_TAG) | awk -F- '{ printf(".%s", $$(NF)) }' | cut -c 1-6)
        BUILD:=$(BUILD)$(_BUILDCOMMIT)
      endif
    endif
  endif
endif

# This section contains the variables that represent the kernel rpm's NVR.
# The NVR looks like, for example, kernel-5.17.0-0.rc8.551acdc3c3d2.124.test.fc35.
# This string can be deconstructed as
#
# $(SPECPACKAGE_NAME)-$(SPECKVERSION).$(SPECKPATCHLEVEL).$(SPECKSUBLEVEL)-$(UPSTREAMBUILD)$(BUILD)$(DISTLOCALVERSION)$(DIST)
#
# This can be evaluated as
#
# $(SPECPACKAGE_NAME)-$(SPECKVERSION).$(SPECKPATCHLEVEL).$(SPECKSUBLEVEL)-$(SPECBUILD)          $(DIST)
# $(SPECPACKAGE_NAME)-$(SPECRPMVERSION)                                  -$(SPECBUILD)          $(DIST)
# $(SPECPACKAGE_NAME)-$(BASEVERSION)                                                            $(DIST)
# $(RELEASETAG)                                                                             $(DIST)
#
# Gemini kernels override RPM NVR to:
# $(SPECPACKAGE_NAME)-$(SPECRVERSION).$(SPECRPATCHLEVEL)-$(SPECBUILD)                           $(DIST)
#
SPECBUILD:=$(UPSTREAMBUILD)$(BUILD)$(VARIANTSTREAM)$(DISTLOCALVERSION)
SPECVERSION:=$(SPECKVERSION).$(SPECKPATCHLEVEL).$(SPECKSUBLEVEL)
ifeq ($(SPECGEMINI),0)
  SPECRPMVERSION:=$(SPECKVERSION).$(SPECKPATCHLEVEL).$(SPECKSUBLEVEL)
else
  SPECRPMVERSION:=$(SPECRVERSION).$(SPECRPATCHLEVEL)
endif
BASEVERSION:=$(SPECRPMVERSION)-$(SPECBUILD)
RELEASETAG:=$(SPECPACKAGE_NAME)-$(BASEVERSION)
SRPM:=$(SRPMS)/$(RELEASETAG)$(DIST).src.rpm

#
# This conditional statement is where fedora, centos, and other (aka RHEL)
# specific values should be set.
#
ifeq ("$(DISTRO)", "fedora")
  RHDISTGIT_BRANCH:=rawhide
  ifndef BUILD_TARGET
    BUILD_TARGET:=rawhide
  endif
  # The Fedora tarfile name is based on an upstream tag as users may
  # replace the tarball from one with upstream, rebuild, and then deploy
  # without changing anything else in the specfile.
  SPECTARFILE_RELEASE:=$(UPSTREAM_TARBALL_NAME)
  SPECKABIVERSION:=$(SPECRPMVERSION)
  DISTRELEASETAG:=$(RELEASETAG)
  DISTBASEVERSION:=$(BASEVERSION)
  RHPKG_BIN:=fedpkg
else ifeq ("$(DISTRO)", "centos")
  RHDISTGIT_BRANCH:=c$(RHEL_MAJOR)s
  ifndef BUILD_PROFILE
    BUILD_PROFILE:= -p stream
  endif
  ifndef BUILD_TARGET
    BUILD_TARGET:=c$(RHEL_MAJOR)s-candidate
  endif
  SPECTARFILE_RELEASE:=$(BASEVERSION)$(DIST)
  SPECKABIVERSION:=$(BASEVERSION)$(DIST)
  DISTRELEASETAG:=$(RELEASETAG)$(DIST)
  DISTBASEVERSION:=$(BASEVERSION)$(DIST)
  PATCHLIST_URL:=none
  RHPKG_BIN:=centpkg
else
  RHDISTGIT_BRANCH:=rhel-$(RHEL_MAJOR).$(RHEL_MINOR).0
  ifndef BUILD_TARGET
    ifeq ("$(DIST)", ".eln")
      BUILD_TARGET:=eln
    else
      # This value is used by the dist[g]-targets.  Changing this value has significant
      # consequences for all of RHEL kernel engineering.
      BUILD_TARGET:=rhel-$(RHEL_MAJOR).$(RHEL_MINOR)-test-pesign
      ifeq ("$(ZSTREAM)", "yes")
          ifneq ($(shell git tag -l RHEL-$(RHEL_MAJOR).$(RHEL_MINOR)),)
              BUILD_TARGET:=rhel-$(RHEL_MAJOR).$(RHEL_MINOR)-z-test-pesign
          endif
      endif
    endif
  endif
  ifeq ("$(DIST)", ".eln")
    DISTRELEASETAG:=$(RELEASETAG)
    DISTBASEVERSION:=$(BASEVERSION)
    SPECTARFILE_RELEASE:=$(BASEVERSION)
    SPECKABIVERSION:=$(BASEVERSION)
  else
    DISTRELEASETAG:=$(RELEASETAG)$(DIST)
    DISTBASEVERSION:=$(BASEVERSION)$(DIST)
    SPECTARFILE_RELEASE:=$(BASEVERSION)$(DIST)
    SPECKABIVERSION:=$(BASEVERSION)$(DIST)
  endif
  PATCHLIST_URL:=none
  RHPKG_BIN:=rhpkg
endif

# When building the automotive kernel package, set AUTOMOTIVE_BUILD=1.
ifeq ($(AUTOMOTIVE_BUILD), 1)
  ifeq ("$(DISTRO)", "rhel")
    RHDISTGIT_BRANCH:=rhivos-$(AUTOMOTIVE_MAJOR).$(AUTOMOTIVE_MINOR).0
    RHPKG_BIN:=rhpkg
    RHPKG_NS:=
    RHPKG_OPTS:=
    ifeq ("$(DIST)", ".eln")
      $(error "Error: the distro 'eln' does not support automotive.")
    else
      BUILD_TARGET=rhivos-$(AUTOMOTIVE_MAJOR)-newest-test-pesign
    endif
  else ifeq ("$(DISTRO)", "centos")
    # This is a CentOS SIG dist-git for AutoSD rather than for CentOS Stream.
    RHDISTGIT_BRANCH:=c$(RHEL_MAJOR)s-sig-automotive-main
    RHPKG_BIN:=centpkg-sig
    RHPKG_NS:=automotive/rpms/
    RHPKG_OPTS:=--config $(REDHAT)/automotive-centpkg-sig.conf
    BUILD_TARGET=autosd$(RHEL_MAJOR)s-packages-main-el$(RHEL_MAJOR)s
  else
    $(error "Error: the distro '$(DISTRO)' does not support automotive.")
  endif
  BUILDOPTS += +automotive_build
  INCLUDE_FEDORA_FILES:=0
  INCLUDE_RHEL_FILES:=0
  INCLUDE_RT_FILES:=0
  INCLUDE_AUTOMOTIVE_FILES:=1
  FLAVOR:=rhel
endif

TARFILE:=linux-$(SPECTARFILE_RELEASE).tar.xz
TARBALL:=$(REDHAT)/$(TARFILE)

KABI_TARBALL:=$(REDHAT)/kernel-abi-stablelists-$(SPECKABIVERSION).tar.xz
KABIDW := $(REDHAT)/kabi-dwarf
KABIDW_TARBALL:=$(REDHAT)/kernel-kabi-dw-$(SPECKABIVERSION).tar.xz

# load Makefile variable settings from user-specified configuration file,
# ~/.rhpkg.mk or $TOPDIR/.rhpkg.mk
ifeq ("$(RHDISTGIT_CACHE)", "")
  ifneq ("$(wildcard ${HOME}/.rhpkg.mk)", "")
    include ${HOME}/.rhpkg.mk
  endif
  ifneq ("$(wildcard $(TOPDIR)/.rhpkg.mk)", "")
    include $(TOPDIR)/.rhpkg.mk
  endif
endif

ifeq ("$(ZSTREAM)", "yes")
  YSTREAM_FLAG = no
  ifeq ("$(origin RHDISTGIT_BRANCH)", "command line")
    ZSTREAM_FLAG = branch
  else
    ZSTREAM_FLAG = yes
  endif
  BUILDOPTS += +kabidupchk
else
  ZSTREAM_FLAG = no
  YSTREAM_FLAG = yes
endif

endif # Variables do not have to be configured for help output.

include Makefile.cross

# different types of help targets (for example, dist-help-misc or dist-help-build)
HELP_TYPES=cleaning build kabi configuration misc

include Makefile.exports

#
# Targets
#
# Do not place variable declarations below this line.  Variables are exported
# to scripts called in the targets below and do not need to be passed on the
# command-line.
#

default: dist-help

dist-python-check:
	@if [ ! -x /usr/bin/python3 ]; then \
		echo "ERROR: Python 3 is needed." ; \
		exit 1; \
	fi

dist-kabi: ##kabi Create kABI stablelist files in redhat/kabi/kabi-rhel*/ and merge kABI checksums into redhat/kabi/Module.kabi_*.
dist-kabi: dist-python-check
	@for KABIARCH in $(ARCH_LIST); do \
		$(REDHAT)/kabi/show-kabi -k $(REDHAT)/kabi/kabi-module/ -m \
			-a $$KABIARCH -r $(RHEL_MAJOR).$(RHEL_MINOR) > $(REDHAT)/kabi/Module.kabi_$$KABIARCH;\
		mkdir -p $(REDHAT)/kabi/kabi-rhel$(RHEL_MAJOR)$(RHEL_MINOR)/;\
		$(REDHAT)/kabi/show-kabi -k $(REDHAT)/kabi/kabi-module/ -s -a $$KABIARCH \
		-r $(RHEL_MAJOR).$(RHEL_MINOR) > $(REDHAT)/kabi/kabi-rhel$(RHEL_MAJOR)$(RHEL_MINOR)/kabi_stablelist_$$KABIARCH;\
	done;
	@(cd $(REDHAT)/kabi/ && ln -Tsf kabi-rhel$(RHEL_MAJOR)$(RHEL_MINOR) kabi-current)

dist-kabi-dup: ##kabi Merge kABI checksums for Driver Update Program (DUP) into redhat/kabi/Module.kabi_dup_*.
dist-kabi-dup: dist-python-check
	@for KABIARCH in $(ARCH_LIST); do \
		touch $(REDHAT)/kabi/Module.kabi_dup_$$KABIARCH;\
		if [ -d $(REDHAT)/kabi/kabi-dup-module/kabi_$$KABIARCH ]; then \
			$(REDHAT)/kabi/show-kabi -k $(REDHAT)/kabi/kabi-dup-module/ -m \
				-a $$KABIARCH -r $(RHEL_MAJOR).$(RHEL_MINOR) > \
				$(REDHAT)/kabi/Module.kabi_dup_$$KABIARCH;\
		fi \
	done;

dist-assert-tree-clean:
ifeq ($(FORCE),)
	@if ! git diff-index --quiet --cached HEAD -- &> /dev/null; then \
		echo "ERROR: staged changes found. Please clean the git tree."; >&2 \
		git status; \
		exit 1; \
	fi
	@if ! git diff-files --quiet &> /dev/null; then \
		echo "ERROR: unstaged changes found. Please clean the git tree." >&2; \
		git status; \
		exit 1; \
	fi
	@if [ $$(git ls-files --exclude-standard --others | wc -l) -gt 0 ]; then \
		echo "ERROR: untracked files found. Please clean the git tree." >&2; \
		git status; \
		exit 1; \
	fi
endif

ifndef KABI_SUPPORTED_ARCHS
  KABI_SUPPORTED_ARCHS := $(patsubst kabi_%,%,$(notdir $(wildcard $(REDHAT)/kabi/kabi-module/*)))
endif
KABI_CROSS_COMPILE = $(patsubst %,$(KABI_CROSS_COMPILE_PREFIX)%$(KABI_CROSS_COMPILE_SUFFIX),$(filter-out $(CURARCH),$(KABI_SUPPORTED_ARCHS)))

ifeq ($(ARCH),$(CURARCH))
dist-assert-cross-tools: # no dependencies when we're building for native arch
else
dist-assert-cross-tools: $(patsubst %,%gcc,$(KABI_CROSS_COMPILE))
endif

$(KABI_CROSS_COMPILE_PREFIX)%$(KABI_CROSS_COMPILE_SUFFIX)gcc:
	@echo "ERROR: couldn't find cross compilation toolchain (looking for: $@)" >&2
	@echo "       override KABI_CROSS_COMPILE_PREFIX to set path (currently $(KABI_CROSS_COMPILE_PREFIX))" >&2
	@echo "       override KABI_CROSS_COMPILE_SUFFIX to set suffix (currently $(KABI_CROSS_COMPILE_SUFFIX))" >&2
	@exit 1


ifeq ($(ARCH),)
dist-kabi-index: ARCH=$(CURARCH)
endif
dist-kabi-index:
	: > $(REDHAT)/kabi/kabi-module/kabi_$(ARCH)/.index.json
	for sym in $$(find $(REDHAT)/kabi/kabi-module/kabi_$(ARCH)/ -type f -not -name "*.*" \
	                  -exec basename {} \;); do \
		$(REDHAT)/kabi/symtypes index \
			$(REDHAT)/kabi/kabi-module/kabi_$(ARCH)/.$$(basename $$sym) \
			-o $(REDHAT)/kabi/kabi-module/kabi_$(ARCH)/.index.json || : ; \
	done

ifeq ($(ARCH),)
dist-kabi-image-%: ARCH=$(CURARCH)
endif
dist-kabi-image-%: NODE = $(patsubst dist-kabi-image-%,%,$@)
dist-kabi-image-%: dist-kabi-index
	$(REDHAT)/kabi/symtypes image -s $(NODE) -t $(REDHAT)/kabi/kabi-module/kabi_$(ARCH)/.index.json

ifeq ($(ARCH),)
dist-kabi-preimage-%: ARCH=$(CURARCH)
endif
dist-kabi-preimage-%: NODE = $(patsubst dist-kabi-preimage-%,%,$@)
dist-kabi-preimage-%: dist-kabi-index
	@node="$(NODE)"; \
	if [ "${node:1:1}" = "#" ]; then \
		$(REDHAT)/kabi/symtypes preimage -s '$(NODE)' -t $(REDHAT)/kabi/kabi-module/kabi_$(ARCH)/.index.json; \
	else \
		for prefix in {E,e,s,t,u}; do \
		$(REDHAT)/kabi/symtypes preimage -S -s "$$prefix#$(NODE)" -t $(REDHAT)/kabi/kabi-module/kabi_$(ARCH)/.index.json || :; \
		done; \
	fi

dist-symtype-diff: ##kabi Diff kabi stablelist and current kernel.
dist-symtype-diff: KABI_TOOL = $(REDHAT)/kabi/diff-kabi
dist-symtype-diff: dist-assert-cross-tools dist-assert-tree-clean dist-configs dist-symtype-shared

dist-kabi-update: ##kabi Update kabi stablelist checksums.
dist-kabi-update: KABI_TOOL = $(REDHAT)/kabi/update-kabi
dist-kabi-update: dist-assert-cross-tools dist-assert-tree-clean dist-configs dist-symtype-shared

dist-symtype-shared:
	@declare -a _ARGS; \
	declare -a _ARCHS; \
	for symbol in $${SYMBOL:-}; do \
		_ARGS=("$${_ARGS[@]}" -s "$$symbol"); \
	done; \
	if [ -n "$${SRC:-}" ]; then \
		_ARGS=("$${_ARGS[@]}" $$SRC); \
	fi; \
	if [ -n "$${ARCH:-}" ]; then \
		$(KABI_TOOL) -a $(ARCH) $${_ARGS[@]}; \
	else \
		cnt=1; \
		for arch in $(KABI_SUPPORTED_ARCHS); do \
			$(KABI_TOOL) -a $$arch $${_ARGS[@]} || : ; \
			let cnt++; \
		done; \
	fi

dist-check-kabi: ##kabi Check for changes in kABI stablelisted symbols. Requires a pre-compiled tree: run `make dist-configs`, copy the relevant config file from redhat/configs/ to .config, and run `make`.
dist-check-kabi: dist-kabi
	@if [ ! -e $(_OUTPUT)/Module.symvers ]; then \
		echo "ERROR: You must compile the kernel and modules first";\
		exit 1;\
	fi
	@$(REDHAT)/kabi/check-kabi -k $(REDHAT)/kabi/Module.kabi_$(MACH) \
	 -s $(_OUTPUT)/Module.symvers

dist-check-kabi-dup: ##kabi Like dist-check-kabi but uses a DUP kABI stablelist.
dist-check-kabi-dup: dist-kabi-dup
	@if [ ! -e $(_OUTPUT)/Module.symvers ]; then \
		echo "ERROR: You must compile the kernel and modules first";\
		exit 1;\
	fi
	@$(REDHAT)/kabi/check-kabi -k $(REDHAT)/kabi/Module.kabi_dup_$(MACH) \
	-s $(_OUTPUT)/Module.symvers

dist-kabi-dw-base: ##kabi Generate the base dataset for kABI DWARF-based check.
dist-kabi-dw-base: dist-kabi
	@echo "Generating baseline dataset for KABI DWARF-based comparison..."
	@echo "**** GENERATING DWARF-based kABI baseline dataset ****"
	@$(KABIDW)/run_kabi-dw.sh generate \
		$(REDHAT)/kabi/kabi-current/kabi_stablelist_$(CURARCH) \
		$(_OUTPUT) $(KABIDW)/base/$(CURARCH)/

dist-kabi-dw-check: ##kabi Run DWARF-based kABI comparison of current binaries with the base dataset.
dist-kabi-dw-check: dist-kabi
	@if [ ! -d $(KABIDW)/base/$(CURARCH) ]; then \
		echo "**** ERROR: ****"; \
		echo "Comparison base not found in $(KABIDW)/base/$(CURARCH)."; \
		echo "Please run \"make dist-kabi-dw-base\" first!"; \
		exit 1; \
	fi
	@echo "**** GENERATING DWARF-based kABI dataset ****"
	@$(KABIDW)/run_kabi-dw.sh generate \
		$(REDHAT)/kabi/kabi-current/kabi_stablelist_$(CURARCH) \
		$(_OUTPUT) $(KABIDW)/base/$(CURARCH).tmp/
	@echo "**** KABI DWARF-based comparison report ****"
	@$(KABIDW)/run_kabi-dw.sh compare \
		$(KABIDW)/base/$(CURARCH) $(KABIDW)/base/$(CURARCH).tmp || :
	@echo "**** End of KABI DWARF-based comparison report ****"
	@rm -rf $(KABIDW)/base/$(CURARCH).tmp

dist-configs-commit: dist-configs-prep
	+@cd $(REDHAT)/configs; ./generate_all_configs.sh;\
	./process_configs.sh -z "$(SPECRPMVERSION)" "$(FLAVOR)"

dist-configs-commit-mismatches: dist-configs-prep
	+@cd $(REDHAT)/configs; ./generate_all_configs.sh;\
	./process_configs.sh -M "$(SPECRPMVERSION)" "$(FLAVOR)"

dist-configs: ##configuration Create RHEL config files in redhat/config/.
dist-configs: dist-configs-prep
	+@cd $(REDHAT)/configs; ./generate_all_configs.sh;\
	./process_configs.sh $(PROCESS_CONFIGS_OPTS) "$(SPECRPMVERSION)" ""

dist-fedora-configs: ##build build Fedora configs.
dist-fedora-configs: FLAVOR = fedora
dist-fedora-configs: dist-configs
fedora-configs: dist-fedora-configs

dist-rhel-configs: ##build build ELN configs.
dist-rhel-configs: FLAVOR = rhel
dist-rhel-configs: dist-configs
rh-configs: dist-rhel-configs

dist-automotive-configs: ##build build only the automotive configs
dist-automotive-configs: AUTOMOTIVE_BUILD := 1
dist-automotive-configs: DISTRO=$(if $(DISTRO_USERDEF),$(DISTRO_USERDEF),centos)
dist-automotive-configs:
	$(MAKE) dist-configs

dist-configs-check: dist-configs-prep
	+cd $(REDHAT)/configs; ./process_configs.sh $(PROCESS_CONFIGS_CHECK_OPTS) "" ""

dist-configs-prep: dist-clean-configs
# automotive is not a flavor but we want to treat it like it is in this case
ifdef AUTOMOTIVE_BUILD
	@trap 'echo "Cleaning up..."; mv $(REDHAT)/configs/priority.$(FLAVOR).bak $(REDHAT)/configs/priority.$(FLAVOR); exit 1' SIGINT; \
	cp $(REDHAT)/configs/priority.$(FLAVOR) $(REDHAT)/configs/priority.$(FLAVOR).bak; \
	sed -i '/^#\|^ORDER\|^\(x86_64\|aarch64\)-automotive/!d' $(REDHAT)/configs/priority.$(FLAVOR); \
	sed -i 's/^\([^=]*\)-automotive/\1/' $(REDHAT)/configs/priority.$(FLAVOR); \
	cd $(REDHAT)/configs; ./build_configs.sh "partial" "snip"; \
	cd $(REDHAT)/configs; ./build_configs.sh "$(SPECPACKAGE_NAME)" "$(FLAVOR)"; \
	mv $(REDHAT)/configs/priority.$(FLAVOR).bak $(REDHAT)/configs/priority.$(FLAVOR)
else
	+cd $(REDHAT)/configs; ./build_configs.sh "partial" "snip"
	+cd $(REDHAT)/configs; ./build_configs.sh "$(SPECPACKAGE_NAME)" "$(FLAVOR)"
endif

dist-configs-arch: ##configuration Same as dist-configs but for single architecture only.
dist-configs-arch: ARCH_MACH = $(MACH)
dist-configs-arch: dist-configs

dist-clean-configs: ##cleaning Clean the redhat/configs/ directory.
	@cd $(REDHAT)/configs; rm -f kernel-*.config kernel-*.config.orig kernel-*.config.tmp partial-*-snip.config

dist-clean-sources: ##cleaning Clean the redhat/rpm/SOURCES/ directory.
# Do not cleanup when creating self-test data
ifndef RHSELFTESTDATA
	@rm -f $(RPM)/SPECS/*
	@rm -f $(SOURCES)/*
endif

dist-clean-rpmdirs: ##cleaning Clean the redhat/rpm/{BUILD,SRPMS,RPMS,SPECS}/ directories.
	@for i in $(RPM)/{BUILD,SRPMS,RPMS,SPECS}/*; do \
		rm -rf $$i; \
	done;

dist-clean: ##cleaning Clean redhat/configs/ and redhat/rpm/ directories.
dist-clean: dist-clean-sources dist-clean-configs dist-clean-rpmdirs
	@rm -f $(REDHAT)/linux-*.tar.xz
	@rm -f $(REDHAT)/kernel-abi-stablelists-*.tar.xz
	@rm -f $(REDHAT)/kernel-kabi-dw-*.tar.xz

dist-stub-key: ##build Use pre generated keys to speed local test builds.
	@echo "Copying pre-generated keys";
	@echo "*** THIS IS NOT RECOMMENDED ***";
	@echo "To be safe, keys should be created once for every build";
	@echo "Use this option only for development builds";
	@cp keys/stub_key.x509 $(_OUTPUT)/;
	@cp keys/stub_key.priv $(_OUTPUT)/;

# force tarball to be regenerated if HEAD changes
.PHONY:	$(TARBALL)
$(TARBALL):
	@scripts/create-tarball.sh

.PHONY: $(KABI_TARBALL)
$(KABI_TARBALL): dist-kabi
	@(cd kabi && tar -cJf $(KABI_TARBALL) kabi-rhel$(RHEL_MAJOR)* kabi-current)

.PHONY: $(KABIDW_TARBALL)
$(KABIDW_TARBALL):
	@if [ ! -d $(KABIDW)/base ]; then \
		mkdir -p $(KABIDW)/base; \
	fi
	@(cd kabi-dwarf && tar -cJf $(KABIDW_TARBALL) base run_kabi-dw.sh)

dist-git-version-check:
	@# genspec.sh uses pathspec magic that wasn't introduced until version 2.13
	@IFS=" ."; \
	set -- $$($(GIT) --version); \
	IFS=; \
	if [ "$$3" -lt 2 -o \( "$$3" -eq 2 -a "$$4" -lt 13 \) ]; then \
		echo "ERROR: You need git version 2.13 or newer to run some setup commands"; \
		exit 1; \
	fi

dist-get-buildreqs: ##misc returns list of packages listed as build requirements for this kernel.
dist-get-buildreqs: setup-source
	@PKGLIST="rpm-build $$(rpmspec -q --buildrequires $(SOURCES)/$(SPECFILE) | cut -d ' ' -f 1 | sed 's/^(//' )"; \
	MISSING=""; \
	for pkg in $$PKGLIST; do \
		rpm -q --whatprovides $$pkg >/dev/null || MISSING="$$MISSING $$pkg"; \
	done; \
	if [ -n "$$MISSING" ]; then \
		echo " Missing dependencies: $$MISSING"; \
		if  echo "$$MISSING" | grep -q system-sb-certs ; then \
			echo "system-sb-certs can be installed from a RHEL or Centos-Stream CodeReady Linux Builder (CRB) repository."; \
		fi; \
	else \
		echo "PASS: All build dependencies found."; \
	fi

# IMPORTANT: All targets whose recipes read/modify files under $(SOURCES)
# MUST depend on setup-source (at least indirectly). Otherwise such
# recipes may be ordered before/in parallel with dist-clean-sources
# (prerequisite of setup-source), which would then delete their results.
setup-source: dist-clean-sources dist-git-version-check
	@if [ "$(SPECPACKAGE_REBUILD)" = "1" ]; then \
		$(REDHAT)/scripts/generate-rebuild-changelog.sh; \
	fi
	@if ! $(GIT) -C "$(TOPDIR)" ls-files --error-unmatch -- "redhat/$(SPECFILE).template" >/dev/null 2>&1; then \
		echo "Creating $(REDHAT)/$(SPECFILE).template as a copy of $(REDHAT)/kernel.spec.template"; \
		cp $(REDHAT)/kernel.spec.template $(REDHAT)/$(SPECFILE).template; \
	fi
	@cp $(REDHAT)/$(SPECFILE).template $(SOURCES)/$(SPECFILE)
	@if [ ! -e  $(REDHAT)/$(SPECCHANGELOG) ]; then \
		echo "Creating an empty $(SPECCHANGELOG) since it does not exist yet"; \
		touch $(REDHAT)/$(SPECCHANGELOG); \
	fi
	@if [ -z "$(RHSELFTESTDATA)" ]; then \
		cp $(REDHAT)/$(SPECCHANGELOG) $(SOURCES)/$(SPECCHANGELOG); \
	else \
		echo "Mon Mar 28 2022 Fedora Kernel Team <kernel-team@fedoraproject.org> [$(BASEVERSION)]" > $(SOURCES)/$(SPECCHANGELOG); \
	fi
	@if [ -z "$(PATCHLIST_URL)" ]; then \
		echo "Error: PATCHLIST_URL must be set (to 'none' or any URL)"; \
		exit 1; \
	fi
	@$(REDHAT)/scripts/genspec/genspec.sh
	@cp $(SOURCES)/$(SPECFILE) $(SOURCES)/../SPECS/

generate-testpatch-tmp:
	@$(GIT) diff --no-renames HEAD ":(exclude,top).get_maintainer.conf" \
		":(exclude,top).gitattributes" \
		":(exclude,top).gitignore" \
		":(exclude,top)makefile" \
		":(exclude,top)Makefile.rhelver" \
		":(exclude,top)redhat" > $(TESTPATCH).tmp

sources-rh: $(TARBALL) $(KABI_TARBALL) $(KABIDW_TARBALL) generate-testpatch-tmp setup-source dist-configs-check
	@cp -l $(TARBALL) $(KABI_TARBALL) $(KABIDW_TARBALL) $(SOURCES)/ || \
		cp $(TARBALL) $(KABI_TARBALL) $(KABIDW_TARBALL) $(SOURCES)/
	@touch $(TESTPATCH)
	@diff $(TESTPATCH).tmp $(TESTPATCH) > /dev/null || \
		echo "WARNING: There are uncommitted changes in your tree or the changes are not in sync with linux-kernel-test.patch.  Either commit the changes or run 'make dist-test-patch'"
	@rm $(TESTPATCH).tmp
	@cp $(TESTPATCH) $(SOURCES)/linux-kernel-test.patch
	@cat configs/flavors | while read flavor; do echo "Copying sources for $${flavor}"; [ -e $${flavor}_files ] && cp $${flavor}_files/* $(SOURCES); done
	@sed -e "s/%%SPECKVERSION%%/$(SPECKVERSION)/" \
		-e "s/%%SPECKPATCHLEVEL%%/$(SPECKPATCHLEVEL)/" \
		scripts/gating/rpminspect.yaml > $(SOURCES)/rpminspect.yaml
	@$(REDHAT)/scripts/uki_addons/uki_create_json.py $(REDHAT)/scripts/uki_addons/uki_addons.json
	@cp scripts/kernel-tools/kvm_stat.logrotate \
		keys/rhel*.x509 \
		keys/nvidia*.x509 \
		keys/fedora*.x509 \
		kabi/check-kabi \
		configs/$(SPECPACKAGE_NAME)-*.config \
		configs/partial*.config \
		scripts/gating/gating.yaml \
		scripts/filtermods.py \
		scripts/mod/mod-denylist.sh \
		scripts/mod/mod-sign.sh \
		scripts/uki_addons/uki_create_addons.py \
		scripts/uki_addons/uki_addons.json \
		scripts/kmap.py \
		configs/flavors \
		configs/generate_all_configs.sh \
		configs/merge.py \
		configs/process_configs.sh \
		../Makefile.rhelver \
		README.rst \
		kernel-local \
		kernel.sbat.template \
		uki.sbat.template \
		uki-addons.sbat.template \
		dtbloader.sbat.template \
		dracut-virt.conf \
		$(SOURCES)/
	@changelog_glob="$(SPECPACKAGE_NAME).changelog-$(RHEL_MAJOR).*[0-9]"; \
	cat $$(ls -1 $$changelog_glob | sort -V -r) \
		> $(SOURCES)/$(SPECPACKAGE_NAME).changelog
	cp keys/redhatsecureboot{501,ca5}.cer $(SOURCES)/;
	@for KABIARCH in $(ARCH_LIST); do \
		cp kabi/Module.kabi_$$KABIARCH $(SOURCES)/; \
		cp kabi/Module.kabi_dup_$$KABIARCH $(SOURCES)/; \
	done
	@find $(SOURCES)/ -name "*.yaml" -o -name "*.yaml.fedora" -o -name "*.yaml.rhel" | \
		xargs -i{} $(REDHAT)/scripts/yamlsanity.py {}
	@echo Sanity checking filtermods configs
	@ls -1 -B $(SOURCES)/def_variants.yaml.* | xargs -i{} sh -c "$(REDHAT)/scripts/filtermods.py -q sort -c {} -d /dev/null"

dist-sources: dist-kabi dist-kabi-dup sources-rh

dist-test-patch: ##build Create a diff against HEAD and put it in linux-kernel-test.patch.  This patch will be added to the kernel build.
dist-test-patch: generate-testpatch-tmp
	@mv $(TESTPATCH).tmp $(TESTPATCH);

dist-spec: ##build Create just the spec file in redhat/rpm/SOURCES/.
dist-spec: setup-source

do-rpmbuild: dist-sources
	$(RPMBUILD) --define "_sourcedir $(SOURCES)" --define "_builddir $(RPM)/BUILD" --define "_srcrpmdir $(RPM)/SRPMS" --define "_rpmdir $(RPM)/RPMS" --define "_specdir $(RPM)/SPECS" --define "dist $(DIST)" $(RPMBUILDOPTS) $(RPM)/SOURCES/$(SPECFILE)

dist-all-rpms: ##build Create the binary RPMS and the SRPM for the kernel.  See the dist-brew target for available options.
dist-all-rpms: RPMBUILDOPTS=--target $(MACH) -ba
dist-all-rpms: dist-sources do-rpmbuild

dist-srpm: ##build Create a source RPM and put it into the redhat/rpm/SRPMS/ directory.  See the dist-brew target for available options.
dist-srpm: RPMBUILDOPTS=--nodeps -bs
dist-srpm: dist-sources do-rpmbuild

dist-srpm-gcov: ##build Create a source RPM with gcov enabled and put it into the redhat/rpm/SRPMS/ directory.
dist-srpm-gcov: DISTLOCALVERSION=.gcov
dist-srpm-gcov: BUILDOPTS+=+gcov
dist-srpm-gcov: dist-srpm

dist-rpms: ##build Create the binary RPMS for the kernel. See the dist-brew target for available options.
dist-rpms: RPMBUILDOPTS=--target $(MACH) -bb
dist-rpms: dist-sources do-rpmbuild

# this is a dummy entry to define help for the dist-kernel-* targets
dist-kernel-type: ##build Create binary RPMS for a particular kernel type.  Available <type>s:
dist-kernel-%: dist-sources
	RPMBUILDOPTS="--target $(MACH) --with $* -bb" make do-rpmbuild

dist-prep: ##build Setup the redhat/rpm/BUILD/ directory with the kernel source.  See the dist-brew target for available options.
dist-prep: RPMBUILDOPTS=--nodeps --target noarch -bp
dist-prep: dist-sources do-rpmbuild

dist-perf: RPMBUILDOPTS=--without stock --without zfcpdump --without debug --without doc --without headers --without debuginfo --target $(MACH) -bb
dist-perf: dist-sources do-rpmbuild

dist-rpm-baseonly: ##build Create the binary RPMS for the kernel and modules (no userspace tools or debuginfo).
dist-rpm-baseonly: RPMBUILDOPTS=--target $(MACH) --without debug --without debuginfo --without vdso_install --without perf --without tools -bb
dist-rpm-baseonly: dist-sources do-rpmbuild

# dist-release related targets are used by the kernel maintainer team
dist-release-finish: setup-source
	@cp $(SOURCES)/$(SPECCHANGELOG) $(REDHAT)/$(SPECCHANGELOG)
	@echo $(MARKER) > $(REDHAT)/marker
	@$(GIT) add $(REDHAT)/$(SPECCHANGELOG)
	@$(GIT) add $(REDHAT)/marker
	@$(GIT) commit -s ../Makefile.rhelver $(REDHAT)/marker $(REDHAT)/$(SPECCHANGELOG) -m "[redhat] $(DISTRELEASETAG)"
dist-release-changed:
	@CLOGF="$$(mktemp)"; \
	trap 'rm -f "$$CLOGF"' SIGHUP SIGINT SIGTERM EXIT; \
	if $(REDHAT)/scripts/genspec/genlog.sh "$$CLOGF"; then \
		if [ "$(MARKER)" == "$$(cat $(REDHAT)/marker)" ] && \
		   [ "$$(wc -l $$CLOGF | cut -d ' ' -f 1)" -le 3 ]; then \
			echo "Nothing changed, skipping updates"; \
		else \
			if [ "$(BUMP_RELEASE)" == "yes" ]; then \
				$(REDHAT)/scripts/new_release.sh; \
			fi; \
			$(MAKE) dist-release-finish; \
		fi \
	else \
		retval=$$?; \
		echo "genlog.sh failed with return code $$retval, aborting."; \
		exit $$retval; \
	fi

dist-release: dist-clean-sources
	@$(MAKE) dist-release-changed

dist-get-tag: ##misc Returns the latest release tag.
	$(info $(DISTRELEASETAG))

dist-check-release:
ifeq ($(CHECK_FOR_RELEASE_COMMIT),1)
	@if ! $(GIT) log -1 --format=%s | grep -q "\[redhat\] $(DISTRELEASETAG)"; then \
		echo -e "Error: Check git tree, HEAD is expected to contain release commit."; \
		exit 1; \
	fi
endif

dist-release-tag: dist-check-release
	@$(GIT) tag -a -m "$(DISTRELEASETAG)" $(DISTRELEASETAG)

git-tree-check:
	@if test -n "$(DIST_PUSH)" && test -z "$(shell $(GIT) remote get-url gitlab 2>/dev/null)"; then \
		echo -e "Please run 'git remote add gitlab <url>' to enable git-push.\n"; \
		exit 1; \
	fi
	@$(GIT) diff-index --quiet HEAD || \
		{ echo -e "Dirty tree, please clean before merging.\n"; exit 1; }

dist-merge-upstream: git-tree-check
	@cd ..; $(REDHAT)/scripts/ci/ark-update-configs.sh

dist-merge-upstream-push: export DIST_PUSH="1"
dist-merge-upstream-push: dist-merge-upstream

dist-fedora-release: git-tree-check

	@cd ..; $(REDHAT)/scripts/ci/ark-create-release.sh || \
		(echo "Unable to create release tag"; exit 1)

dist-fedora-release-push: export DIST_PUSH="1"
dist-fedora-release-push: dist-fedora-release

# The kernel's UTS_RELEASE has a maximum length of 64 chars and can be exceeded
# if the DISTLOCALVERSION is too long. Check is based on the worst-case scenario.
dist-vr-check:
	@VARIANT="aarch64+rt-64k-debug"; \
	UTS_RELEASE="$(SPECVERSION)-$(SPECBUILD)$(DIST).$$VARIANT"; \
	if [[ $$(echo -n $$UTS_RELEASE | wc -c) -gt 64 ]]; then \
		echo "SPECBUILD ($(SPECBUILD)) is too long.  Use a shorter localversion (currently $(DISTLOCALVERSION))"; \
		echo "UTS_RELEASE will be exceeded for variant: $$UTS_RELEASE"; \
		exit 1; \
	fi

# Call as sub-make to select different default DISTRO and DIST for dist-brew.
# This is because target-specific variables only apply inside the recipe, but we
# need to recalculate some values such as BUILD_TARGET that depends on them.
# However, do not override any value directly defined by the user.
dist-brew: ##build Create a kernel SRPM and then call brew to build the created SRPM.  Add BUILDOPTS="+<opt> -<opt> [...]" to enable/disable build options.
dist-koji: ##build Create a kernel SRPM and then call koji to build the created SRPM.  See the dist-brew target for available options.
distg-brew: ##build Pass HEAD of the current git branch to brew to build an RPM set.  Do not forget to push to the remote repository first.  Preceed make command by RHGITCOMMIT=<commitID> specify commit ID to use.  To set the remote repo, invoke 'git config rhg.url git://<repo_path>'.
distg-koji: ##build Pass HEAD of the current git branch to koji to build an RPM set.  Do not forget to push to the remote repository first.  See the distg-brew target for options and configuration.
dist-brew distg-brew: DISTRO=$(if $(DISTRO_USERDEF),$(DISTRO_USERDEF),rhel)
dist-brew distg-brew: DIST=$(if $(DIST_USERDEF),$(DIST_USERDEF),.el$(RHEL_MAJOR))
dist-brew distg-brew: BUILD_PROFILE=$(if $(BUILD_PROFILE_USERDEF),$(BUILD_PROFILE_USERDEF))
dist-brew distg-brew: BUILD_TARGET=$(if $(BUILD_TARGET_USERDEF),$(BUILD_TARGET_USERDEF))
dist-brew distg-brew dist-koji distg_koji:
	$(MAKE) _$@

_dist-brew _dist-koji: _dist-%: dist-vr-check dist-srpm
	$* $(BUILD_PROFILE) build $(BUILD_FLAGS) --scratch $(BUILD_TARGET) $(SRPMS)/$(RELEASETAG)$(DIST).src.rpm $(OUTPUT_FILE)

_distg-brew _distg-koji: _distg-%: dist-vr-check
	$* $(BUILD_PROFILE) build $(BUILD_FLAGS) --scratch $(BUILD_TARGET) "$(RHGITURL)?redhat/koji#$(RHGITCOMMIT)"

dist-cbs: ##build Create a kernel-automotive SRPM for the Automotive SIG and then call cbs to build the created SRPM.
dist-cbs: AUTOMOTIVE_BUILD := 1
dist-cbs: DISTRO=$(if $(DISTRO_USERDEF),$(DISTRO_USERDEF),centos)
dist-cbs: BUILD_TARGET=$(if $(BUILD_TARGET_USERDEF),$(BUILD_TARGET_USERDEF))
dist-cbs:
	$(MAKE) _$@

_dist-cbs: dist-srpm
	cbs build $(BUILD_FLAGS) --scratch $(BUILD_TARGET) $(SRPMS)/$(RELEASETAG)$(DIST).src.rpm

dist-git-test: export RH_DIST_GIT_TEST="1"
dist-git-test: dist-git

dist-git: dist-srpm $(KABI_TARBALL) $(KABIDW_TARBALL) dist-check-release
	@if [ -z "$(RHDISTGIT_BRANCH)" ]; then \
		echo "RHDISTGIT_BRANCH is unset"; \
		exit 1; \
	fi
	$(REDHAT)/scripts/rh-dist-git.sh

dist-headers-srpm:
	@scripts/create_headers_tarball.sh
	@cp $(REDHAT)/kernel-headers.spec.template $(SOURCES)/kernel-headers.spec
	@$(REDHAT)/scripts/genspec/headerspec.sh
	@cp $(SOURCES)/kernel-headers.spec $(SOURCES)/../SPECS/
	$(RPMBUILD) --define "_sourcedir $(SOURCES)" --define "_builddir $(RPM)/BUILD" --define "_srcrpmdir $(RPM)/SRPMS" --define "_rpmdir $(RPM)/RPMS" --define "_specdir $(RPM)/SPECS" --define "dist $(DIST)" --nodeps -bs $(RPM)/SOURCES/kernel-headers.spec

dist-git-headers: dist-headers-srpm
	@if [ -z "$(RHDISTGIT_BRANCH)" ]; then \
		echo "RHDISTGIT_BRANCH is unset"; \
		exit 1; \
	fi
	SPECPACKAGE_NAME=kernel-headers SRPM=$(SRPMS)/kernel-headers-$(BASEVERSION)$(DIST).src.rpm $(REDHAT)/scripts/rh-headers-dist-git.sh

dist-git-headers-test: export RH_DIST_GIT_TEST="1"
dist-git-headers-test: dist-git-headers

dist-rtg: dist-release
	@$(MAKE) dist-release-tag
	@$(MAKE) dist-git

# RH_LATEST returns the value of the latest "known good" kernel from brew.
# This should not be confused with the latest top-of-tree development tag.
dist-get-latest: ##misc Returns the latest "known good" kernel from brew. This should not be confused with the latest top-of-tree development tag.
	$(eval RH_LATEST:=$(shell brew latest-pkg --quiet rhel-${RHEL_MAJOR}.${RHEL_MINOR}.0-candidate kernel | awk ' { print $$1 } '))
	@echo "The latest kernel package tag is ${RH_LATEST}."

dist-os-version: ##misc Displays the current Red Hat Enterprise Linux version target used by the current branch/tree.
	@echo "OSVERSION: $(RHEL_MAJOR).$(RHEL_MINOR)"

dist-dump-variables: ##misc Dump makefile variables.
	@$(foreach V,$(sort $(.VARIABLES)), $(if $(filter-out environment default automatic override, $(origin $V)),$(info $V=$($V)),$(if $(filter $(EXPORTED_VARS),$(V)),$(info $V=$($V)))))

dist-self-test: ##misc Runs self-tests from the redhat/self-test directory.
	@if test -x /usr/bin/bats; then \
		bats $(REDHAT)/self-test/*.bats ; \
	else \
		echo "dist-self-test: The bats package is not installed" ; \
	fi

dist-self-test-data:
	./self-test/data/create-data.sh

dist-check-licenses: ##misc Runs license selftest checking SPDX-License-Identifier: tags in all source files against `License:` field in the specfile template.
	@if test -x /usr/bin/bats; then \
		bats $(REDHAT)/self-test/1007-spdx-licenses.bats ; \
	else \
		echo "dist-check-licenses: The bats package is not installed" ; \
	fi

#
# Help output
#

# There's no reason for there to be a 26 (plus three spaces = 29) gap between
# the target and description output.  It was pulled out of the air because it looked
# nice on both 80 and 100 column terminals.  So nyah if you don't like it.
dist-helper-output: COLUMNS=$(shell expr $(shell tput cols) - 29)
dist-helper-output:
	@awk -v cols=${COLUMNS} -v distbrew="${HELP_DIST_BREW_OUTPUT}" \
		-v distkerneltype="${HELP_DIST_KERNEL_TYPE}" -v arches="${ARCH_LIST}" \
		-v onetarget=${HELP_ONE_TARGET} \
	'BEGIN {FS = ":.*##${HELP_TYPE}";} \
		/^[a-zA-Z_0-9-]+:.*?##${HELP_TYPE}/ { \
			target=$$1; \
			desc=$$2; \
			if ( onetarget != "" && onetarget != target) \
				next; \
			if ( target == "dist-brew" ) \
				desc = desc" Available <opt>s and their default values: "distbrew; \
			if ( target == "dist-kernel-type" ) { \
				target = "dist-kernel-<type>"; \
				desc = desc" Available <type>s: "distkerneltype; \
			} \
			if ( target == "dist-cross-arch-build" ) { \
				target = "dist-cross-<arch>-build"; \
				desc = desc" "arches; \
			} \
			desclength = length(desc); \
			if (desclength <= cols) { \
				printf ("  \033[36m%-26s\033[0m %s\n", target, desc); \
				if ( onetarget != "" && onetarget != target) \
					exit; \
			} else {\
				substrloc = 0; \
				first = 1; \
				while ( substrloc + cols < desclength ) { \
					substrlength = cols; \
					while ( substr(desc, substrloc + substrlength, 1) != " " ) { \
						substrlength = substrlength - 1; \
					} \
					if ( first == 1 ) { \
						printf("  \033[36m%-26s\033[0m %s\n", target, substr(desc, substrloc, substrlength)); \
						first = 0; \
					} else \
						printf("  %-26s %s\n", " ", substr(desc, substrloc, substrlength)); \
					substrloc = substrloc + substrlength; \
				} \
				printf("  %-26s %s\n", " ", substr(desc, substrloc, cols)); \
				if ( onetarget != "" && onetarget != target) \
					exit; \
			} \
		} \
	' $(MAKEFILE_LIST)

dist-help-%: HELP_TYPE=$(patsubst dist-help-%,%,$@)
dist-help-%:
	@if ! echo "${HELP_TYPES}" | grep -q ${HELP_TYPE}; then \
		echo "No help for ${HELP_TYPE}.  Valid types are {${HELP_TYPES}}"; \
		exit 1; \
	fi
	@if [ "${HELP_TYPE}" == "build" ]; then \
		HELP_DIST_BREW_OUTPUT="$(shell sed -n -e 's/^%define with_\([^ \t]*\).*\?_without_.*/+\1/p' -e 's/^%define with_\([^ \t]*\).*\?_with_.*/-\1/p' kernel.spec.template | grep -v 'only$$')" HELP_DIST_KERNEL_TYPE="$(shell sed -n 's/^%define with_\([^ ]*only\).*/\1/p' kernel.spec.template)" make -s dist-helper-output; \
	else \
		echo "${HELP_TYPE} targets:"; \
		make -s dist-helper-output; \
		echo " "; \
	fi

dist-full-help:
	@for helptype in ${HELP_TYPES}; \
	do \
		make -s dist-help-$${helptype}; \
	done

dist-help:
	@echo  'cleaning targets:'
	@make -s HELP_TYPE=cleaning HELP_ONE_TARGET=dist-clean dist-helper-output
	@echo  ''
	@echo  'building targets:'
	@make -s HELP_TYPE=build HELP_ONE_TARGET=dist-srpm dist-helper-output
	@make -s HELP_TYPE=build HELP_ONE_TARGET=dist-all-rpms dist-helper-output
	@make -s HELP_TYPE=build HELP_ONE_TARGET=dist-brew dist-helper-output
	@make -s HELP_TYPE=build HELP_ONE_TARGET=distg-brew dist-helper-output
	@make -s HELP_TYPE=build HELP_ONE_TARGET=dist-cross-all-rpms dist-helper-output
	@echo  ''
	@echo  'configuration targets:'
	@make -s HELP_TYPE=configuration HELP_ONE_TARGET=dist-configs dist-helper-output
	@echo  ''
	@echo  'Longer help for commands can be displayed with'
	@echo  ''
	@echo  -e '     \033[36mmake dist-help-HELPTYPE\033[0m'
	@echo  ''
	@echo  'where HELPTYPE is one of: {${HELP_TYPES}}.'
	@echo  ''
	@echo  -e 'For full list of targets, run `\033[36mmake dist-full-help\033[0m`.'
	@echo  ''

$(foreach var,$(EXPORTED_VARS),$(eval export $(var)))

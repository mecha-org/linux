srpm:
	dnf -y install git make gcc flex bison bzip2 dwarves xz rpm-build python3-pyyaml rust rust-src bindgen rustfmt
	@if ! git rev-parse --is-inside-work-tree >/dev/null 2>&1; then \
		git init >/dev/null 2>&1; \
	fi
	@if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then \
		git config user.email "dhruveshb@mechasystems.com"; \
		git config user.name "Dhruvesh Baria"; \
		git config --global --add safe.directory "$$(pwd)" >/dev/null 2>&1 || true; \
		.copr/deepen_gittree.sh; \
	fi

	@if [ ! -f .config ]; then \
		make ARCH=arm64 mecha_v8_defconfig; \
	else \
		make ARCH=arm64 olddefconfig; \
	fi

	make ARCH=arm64 NO_CONFIGCHECKS=1 DIST=.fc44 UPSTREAMBUILD_GIT_ONLY=0 srcrpm-pkg

	mkdir -p "$(outdir)"
	@srpm_path=$$(find rpmbuild/SRPMS redhat/rpm/SRPMS -type f -name '*.src.rpm' 2>/dev/null | head -n 1); \
	if [ -n "$$srpm_path" ]; then \
		cp "$$srpm_path" "$(outdir)/"; \
	else \
		echo "No source RPM found after srcrpm-pkg"; \
		exit 1; \
	fi

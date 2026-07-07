srpm:
	dnf -y install git make gcc flex bison bzip2 dwarves xz rpm-build python3-pyyaml rust rust-src bindgen rustfmt
	git config user.email "dhruveshb@mechasystems.com"
	git config user.name "Dhruvesh Baria"
	.copr/deepen_gittree.sh

	# ensure a kernel config exists before building the SRPM
	@if [ ! -f .config ]; then \
		cp arch/arm64/configs/mecha_v8_defconfig .config; \
	fi

	# set UPSTREAMBUILD_GIT_ONLY=0 to include date in %release when building snapshots
	make NO_CONFIGCHECKS=1 DIST=.fc44 UPSTREAMBUILD_GIT_ONLY=0 dist-srpm

	mkdir -p "$(outdir)"
	@srpm_path=$$(find . -type f -name '*.src.rpm' ! -path './$(outdir)/*' 2>/dev/null | head -n 1); \
	if [ -n "$$srpm_path" ]; then \
		cp "$$srpm_path" "$(outdir)/"; \
	else \
		echo "No source RPM found after dist-srpm"; \
		exit 1; \
	fi

#!/bin/bash
set -eu

if [[ -e .git/shallow ]] && ! grep --silent '^SUBLEVEL = 0' Makefile; then
	# for stable branches ensure the history is deep enough to include the branch point
	# as otherwise git will mess up when looking up changes for Patchlist.changelog generation
	echo "This is a shallow clone; unshallowing to avoid oddities using."
	sleep 10
	git fetch --unshallow || :
fi

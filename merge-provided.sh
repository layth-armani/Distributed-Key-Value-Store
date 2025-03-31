#!/bin/bash

# merge-provided: a small script to merge provided/src files into
# done.
#
# Author: Dario Halilovic
# Last updated: 26 March, 2025
# Version: 1.0

# Make sure to first run `git pull` before running this script,
# and that your Git state is clean (run `git stash` if you have modified files).

# This scripts expects the hash of the commit where
# last week's provided files were pushed. To obtain this hash,
# you can run `git log --author=chappeli --oneline` and manually pick
# the correct hash (the first "random" letters).
if [ "$#" -ne 1 ]; then
	echo "Usage: ./merge-provided <commit hash of common ancestor>"
	exit 1
fi

COMMIT_HASH=$1

# Ensure provided/src and done directories exist
if [ ! -d "provided/src" ]; then
    echo "Error: provided/src directory does not exist. Maybe you are running the script in the wrong directory?"
    exit 1
fi
if [ ! -d "done" ]; then
    echo "Error: done directory does not exist. Maybe you are running the script in the wrong directory?"
    exit 1
fi

# Fetch last week's provided files by using the given hash
git checkout --quiet $COMMIT_HASH
if [ $? -ne 0 ]; then
	echo ""
	echo "Failed to checkout to commit hash $COMMIT_HASH."
	echo "Ensure that your Git repository is clean and the commit hash is correct."
	exit 1
fi

# Copy provided files
LAST_WEEK_PROVIDED=$(mktemp -d)
if [ $? -ne 0 ]; then
	echo "Failed to create temporary directory."
	exit 1
fi
trap "rm -r $LAST_WEEK_PROVIDED" 0 2 3 15

cp -r provided/src $LAST_WEEK_PROVIDED

# Switch back to original branch
git switch --quiet -

# Add new files from provided/src that aren't in done
for file in $(comm -23 <(find provided/src -type f -printf "%P\n" | sort) <(find done/ -type f -printf "%P\n" | sort))
do
	mkdir -p "done/$(dirname "$file")"
	cp "provided/src/$file" "done/$file"
done

# Merge changes from provided/src to done
for file in $(find done/ -type f -printf "%P\n")
do
	if [ -f "$LAST_WEEK_PROVIDED/src/$file" ]; then
		git merge-file "done/$file" "$LAST_WEEK_PROVIDED/src/$file" "provided/src/$file"
	fi
done


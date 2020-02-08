#!/usr/bin/env python3
import os
import re
import sys
import subprocess
from optparse import OptionParser
import semver


def git(*args):
    return subprocess.check_output(["git"] + list(args))


def tag_repo(tag):
    url = os.environ["CI_REPOSITORY_URL"]
    push_url = re.sub(r'.+@([^/]+)/', r'git@\1:', url)

    git("remote", "set-url", "--push", "origin", push_url)
    git("tag", tag)
    git("push", "origin", tag)


def bump(latest, bump_type):
    if bump_type == 'patch':
        return semver.bump_patch(latest)
    if bump_type == 'minor':
        return semver.bump_minor(latest)
    if bump_type == 'major':
        return semver.bump_major(latest)
    if bump_type == 'pre':
        return semver.bump_prerelease(latest)
    if bump_type == 'build':
        return semver.bump_build(latest)


def main():
    usage = "usage: %prog -b bump_type(defaulted to build)"
    parser = OptionParser(usage=usage)
    parser.add_option('-b', '--bump', help='What to bump', default='build')
    (options, args) = parser.parse_args()

    try:
        latest = git("describe", "--tags").decode().strip()
    except subprocess.CalledProcessError:
        # No tags in the repository
        version = "1.0.0"
    else:
        # Skip already tagged commits
        if '-' not in latest:
            print(latest)
            return 0

        version = bump(latest, options.bump)

    qtag_repo(version)
    print(version)

    return 0


if __name__ == "__main__":
    sys.exit(main())

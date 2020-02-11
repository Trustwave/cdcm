#!/usr/bin/env python
"""
Upload a set of files to nexus as a versioned artifact.

A credentials file will be required. See documentation for
details on its structure.
"""
""" Copied from GAS Ops nexus-upload.py."""
from optparse import OptionParser
import os.path
import os
import subprocess
try:
    from ConfigParser import SafeConfigParser
except ImportError:
    from configparser import SafeConfigParser

try:
    from hashlib import sha1
except ImportError:
    from sha import sha as sha1


class NexusUploader:
    def __init__(self, host, username, password):
        self.host = host
        self.username = username
        self.password = password

    def upload(self, group, artifact, branch, release_type, version, files):
        # Sigh, no requests lib. Unstrangely enough
        # HTTPConnection isn't very good on 2.4
        for fpath in files:
            fn = os.path.basename(fpath)
            digest = self.calc_hash(fpath)
            self._upload(group, artifact, branch, release_type, version, '%s.sha1' % fn, digest)
            self._upload(group, artifact, branch, release_type, version, fn, '@%s' % fpath)
            latest = 'tw-cdcm-%s.latest.el7.x86_64.rpm' % version
            self._upload(group, artifact, branch, release_type, version, '%s.sha1' % latest, digest)
            self._upload(group, artifact, branch, release_type, version, latest, '@%s' % fpath)

    def _upload(self, group, artifact, branch, release_type, version, fn, source):
        args = [
            'curl', '-v',
            '-X', 'PUT',
            '-u', '%s:%s' % (self.username, self.password),
            '-F', 'file=%s' % source
        ]
        url = 'https://%(host)s/content/repositories/%(release_type)ss/%(group)s/%(artifact)s/%(branch)s/%(version)s/%(fn)s' % {
            'host': self.host,
            'group': group.replace('.', '/'),
            'artifact': artifact,
            'branch': branch if release_type == 'branch' else '',
            'release_type': release_type,
            'version': version,
            'fn': fn
        }
        print(url)
        args.append(url)
        ret = subprocess.call(args)
        if ret != 0:
            raise IOError('File upload failed [%d]: %s' % (ret, fn))
        return

    def calc_hash(self, fn):
        block_size = 2 ** 20  # 128k
        fh = open(fn, 'rb')
        sha1_obj = sha1()
        while True:
            data = fh.read(block_size)
            if not data:
                break
            sha1_obj.update(data)
        fh.close()
        return sha1_obj.hexdigest()

if __name__ == '__main__':
    usage = "usage: %prog [options] -a artifact_id -v #.#.# -b branch_name -r releasetype file1 [file2..fileN]"
    parser = OptionParser(usage=usage)
    parser.add_option('-a', '--artifact', help='Nexus Artifact ID [REQUIRED]')
    parser.add_option('-v', '--nversion', help='Nexus Version [REQUIRED]')
    parser.add_option('-g', '--group', help='Nexus Group ID [com.trustwave]', default='com.trustwave')
    parser.add_option('-c', '--config', help='Config file path [./.nexus.conf]', default=os.path.expanduser('./.nexus.conf'))
    parser.add_option('-b', '--branch', help='Origin branch [master]', default="master")
    parser.add_option('-r', '--release_type', help='Release: snapshot/release [snapshot]', default="snapshot")
    #parser.add_option('--verbose', help='Verbose', default=False, action='store_true')

    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.error('A list of files to upload is required')

    if None in (options.artifact, options.nversion, options.group, options.branch, options.release_type):
        parser.error('Artfact and Version are required options')

    for fn in args:
        if not os.path.isfile(fn):
            parser.error('File does not exist: %s' % fn)

    config = SafeConfigParser()
    config.read(options.config)
    nexus_up = NexusUploader(
        config.get('nexus', 'host'),
        config.get('nexus', 'username'),
        config.get('nexus', 'password')
    )

    nexus_up.upload(options.group, options.artifact,options.branch, options.release_type, options.nversion, args)

#EOF
#!/usr/bin/python3
# Copyright (c) the JSVM authors
# https://github.com/ntrrgc/jsvm
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
import sys

license_text = '''
Copyright (c) the JSVM authors
https://github.com/ntrrgc/jsvm

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
'''.strip().split('\n')

def license_comment(lang):
    if lang in ('py', 'sh', 'txt'):
        return [
            ('# ' + line).rstrip()
            for line in license_text
        ]
    elif lang in ('js', 'java', 'kt', 'c', 'h', 'cpp', 'gradle'):
        return ['/* ' + license_text[0]] + \
            [
                (' * ' + line).rstrip()
                for line in license_text[1:-1]
            ] + [' * ' + license_text[-1] + ' */']
    else:
        raise RuntimeError('Unknown language: ' + lang)

def add_header(path):
    with open(path, 'r+') as f:
        text = f.read()

        if text.strip() == '':
            return

        if 'Mozilla Public License' not in text:
            lines = text.split('\n')

            lang = path.split('.')[-1]

            if lines[0].startswith('#!'): # shebang
                add_before_line = 1
                if 'python' in lines[0]:
                    lang = 'py'
            else:
                add_before_line = 0

            lines[add_before_line:add_before_line] = \
                    license_comment(lang) + ['']

            text = '\n'.join(lines)

        f.seek(0)
        f.write(text)
        f.truncate()


if __name__ == "__main__":
    if len(sys.argv) == 1:
        print('Usage: %s <file>' % sys.argv[0])
        sys.exit(0)

    for path in sys.argv[1:]:
        add_header(path)

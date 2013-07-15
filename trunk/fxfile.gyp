#
# Copyright (c) 2013 Leon Lee author. All rights reserved.
#
#   homepage: http://www.flychk.com
#   e-mail:   mailto:flychk@flychk.com
#
# Use of this source code is governed by a GPLv3 license that can be
# found in the LICENSE file.

{
    'targets':
    [
        {
            'target_name': 'fxfile',
            
            'type': 'none',
            
            'dependencies':
            [
                'src/xpr/xpr.gyp:*',
                'src/fxfile/fxfile.gyp:*',
                'src/fxfile-updater/fxfile-updater.gyp:*',
                'src/fxfile-launcher/fxfile-launcher.gyp:*',
            ],
        }
    ]
}

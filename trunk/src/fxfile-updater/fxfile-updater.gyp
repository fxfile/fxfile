#
# Copyright (c) 2013 Leon Lee author. All rights reserved.
#
#   homepage: http://www.flychk.com
#   e-mail:   mailto:flychk@flychk.com
#
# Use of this source code is governed by a GPLv3 license that can be
# found in the LICENSE file.

{
    'variables':
    {
    },
    
    'includes':
    [
        '../../build/common.gypi',
    ],
    
    'target_defaults':
    {
        'default_configuration' : 'Debug-x86',
        
        'dependencies':
        [
            '../xpr/xpr.gyp:*',
        ],
        
        'configurations':
        {
            'Debug-x86':
            {
                'inherit_from': ['Debug-x86-MFC-Ansicode_Base'],
                
                'msvs_configuration_attributes':
                {
                    'OutputDirectory':       '../../bin',
                    'IntermediateDirectory': '../../obj/fxfile-updater/dbg-x86',
                },
                
                'msvs_settings':
                {
                    'VCLinkerTool': 
                    {
                        'OutputFile': '$(OutDir)\$(ProjectName)_dbg.exe',
                        'AdditionalLibraryDirectories':
                        [
                        ],
                        'AdditionalDependencies':
                        [
                            'libxpr_dbg.lib',
                        ],
                    },
                },
            },
            
            'Release-x86':
            {
                'inherit_from': ['Release-x86-MFC-Ansicode_Base'],
                
                'msvs_configuration_attributes':
                {
                    'OutputDirectory':       '../../bin',
                    'IntermediateDirectory': '../../obj/fxfile-updater/rel-x86',
                },
                
                'msvs_settings':
                {
                    'VCLinkerTool': 
                    {
                        'OutputFile': '$(OutDir)\$(ProjectName).exe',
                        'AdditionalLibraryDirectories':
                        [
                        ],
                        'AdditionalDependencies':
                        [
                            'libxpr.lib',
                        ],
                    },
                },
           },
        },
    },

    'targets':
    [
        {
            'target_name': 'fxfile-updater',
            
            'type': 'executable',
            
            'defines':
            [
            ],
            
            'include_dirs':
            [
                './',
                '../../src/xpr/include',
                '../../src/base',
                '../../lib/libcurl/include',
            ],
            
            'libraries':
            [
                'libcurl.lib',
            ],
            
            'msvs_settings':
            {
                'VCLinkerTool': 
                {
                    'AdditionalLibraryDirectories':
                    [
                        '../../bin',
                        '../../lib/libcurl/lib',
                    ]
                },
            },

            'msvs_postbuild': 'mkdir $(TargetDir)x86-unicode\updater\r\n'
                              'mkdir $(TargetDir)x86-ansicode\updater\r\n'
                              'mkdir $(TargetDir)x64\updater\r\n'
                              'xcopy $(TargetDir)*.dll $(TargetDir)x86-unicode\updater /c /r /y\r\n'
                              'xcopy $(TargetDir)*.dll $(TargetDir)x86-ansicode\updater /c /r /y\r\n'
                              'xcopy $(TargetDir)*.dll $(TargetDir)x64\updater /c /r /y\r\n'
                              'xcopy $(TargetPath) $(TargetDir)x86-unicode\updater /c /r /y\r\n'
                              'xcopy $(TargetPath) $(TargetDir)x86-ansicode\updater /c /r /y\r\n'
                              'xcopy $(TargetPath) $(TargetDir)x64\updater /c /r /y\r\n',

            'msvs_precompiled_header': 'stdafx.h',
            'msvs_precompiled_source': 'stdafx.cpp',
            
            'sources':
            [
                '../base/conf_file.cpp',
                '../base/conf_file.h',
                '../base/conf_file_ex.h',
                '../base/crc32.c',
                '../base/crc32.h',
                '../base/def.h',
                '../base/def_win.h',
                '../base/env_path.cpp',
                '../base/env_path.h',
                '../base/md5.c',
                '../base/md5.h',
                '../base/pidl_win.cpp',
                '../base/pidl_win.h',
                '../base/sha.c',
                '../base/sha.h',
                '../base/shell_item.h',
                '../base/shell_item_win.cpp',
                '../base/update_info_manager.cpp',
                '../base/update_info_manager.h',
                '../base/updater_def.h',

                './main_window.cpp',
                './main_window.h',
                './meta_data_file.cpp',
                './meta_data_file.h',
                './resource.h',
                './stdafx.cpp',
                './stdafx.h',
                './update_checker.cpp',
                './update_checker.h',
                './win_app.cpp',
                './win_app.h',
                './fxfile-updater.rc',
                
                './res/fxfile-updater.ico',
                './res/fxfile-updater.rc2',
            ],
        },
    ],
}
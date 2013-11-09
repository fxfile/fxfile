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
            '../fxfile-keyhook/fxfile-keyhook.gyp:*',
        ],
        
        'configurations':
        {
            'Debug-x86':
            {
                'inherit_from': ['Debug-x86-MFC-Ansicode_Base'],
                
                'msvs_configuration_attributes':
                {
                    'OutputDirectory':       '../../bin',
                    'IntermediateDirectory': '../../obj/fxfile-launcher/dbg-x86',
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
                            'fxfile-keyhook_dbg.lib',
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
                    'IntermediateDirectory': '../../obj/fxfile-launcher/rel-x86',
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
                            'fxfile-keyhook.lib',
                        ],
                    },
                },
            },
        },
    },

    'targets':
    [
        {
            'target_name': 'fxfile-launcher',
            
            'type': 'executable',
            
            'defines':
            [
            ],
            
            'include_dirs':
            [
                './',
            ],
            
            'libraries':
            [
            ],
            
            'msvs_settings':
            {
                'VCLinkerTool': 
                {
                    'AdditionalLibraryDirectories':
                    [
                        '../../bin',
                    ]
                },
            },
            
            'msvs_precompiled_header': 'stdafx.h',
            'msvs_precompiled_source': 'stdafx.cpp',
            
            'msvs_postbuild': 'xcopy $(TargetPath) $(TargetDir)x86-unicode /c /r /y\r\n'
                              'xcopy $(TargetPath) $(TargetDir)x86-ansicode /c /r /y\r\n'
                              'xcopy $(TargetPath) $(TargetDir)x64 /c /r /y\r\n',
            
            'sources':
            [
                './CmdLineParser.cpp',
                './IniFile.cpp',
                './LauncherApp.cpp',
                './MainWnd.cpp',
                './stdafx.cpp',
                './SysTray.cpp',
                './CmdLineParser.h',
                './IniFile.h',
                './LauncherApp.h',
                './MainWnd.h',
                './Resource.h',
                './stdafx.h',
                './SysTray.h',
                './fxfile-launcher.rc',

                './res/fxfile-launcher.ico',
                './res/fxfile-launcher.rc2',
            ],
        },
    ],
}
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
        
        'configurations':
        {
            'Debug-x86':
            {
                'inherit_from': ['Debug-x86_Base'],
                
                'msvs_configuration_attributes':
                {
                    'OutputDirectory':       '../../bin',
                    'IntermediateDirectory': '../../obj/fxfile-keyhook/dbg-x86',
                },
                
                'msvs_settings':
                {
                    'VCLinkerTool': 
                    {
                        'OutputFile': '$(OutDir)\$(ProjectName)_dbg.dll',
                        'AdditionalLibraryDirectories':
                        [
                        ],
                        'AdditionalDependencies':
                        [
                        ],
                    },
                },
            },
            
            'Release-x86':
            {
                'inherit_from': ['Release-x86_Base'],
                
                'msvs_configuration_attributes':
                {
                    'OutputDirectory':       '../../bin',
                    'IntermediateDirectory': '../../obj/fxfile-keyhook/rel-x86',
                },
                
                'msvs_settings':
                {
                    'VCLinkerTool': 
                    {
                        'OutputFile': '$(OutDir)\$(ProjectName).dll',
                        'AdditionalLibraryDirectories':
                        [
                        ],
                        'AdditionalDependencies':
                        [
                        ],
                    },
                },
            },

            'conditions':
            [
                [ 'target_arch!="x86"',
                    {
                        'Debug-x64':
                        {
                            'inherit_from': ['Debug-x64_Base'],
                            
                            'msvs_configuration_attributes':
                            {
                                'OutputDirectory':       '../../bin',
                                'IntermediateDirectory': '../../obj/fxfile-keyhook/dbg-x64',
                            },
                            
                            'msvs_settings':
                            {
                                'VCLinkerTool': 
                                {
                                    'OutputFile': '$(OutDir)\$(ProjectName)_dbg.dll',
                                    'AdditionalLibraryDirectories':
                                    [
                                    ],
                                    'AdditionalDependencies':
                                    [
                                    ],
                                },
                            },
                        },

                        'Release-x64':
                        {
                            'inherit_from': ['Release-x64_Base'],
                            
                            'msvs_configuration_attributes':
                            {
                                'OutputDirectory':       '../../bin',
                                'IntermediateDirectory': '../../obj/fxfile-keyhook/rel-x64',
                            },
                            
                            'msvs_settings':
                            {
                                'VCLinkerTool': 
                                {
                                    'OutputFile': '$(OutDir)\$(ProjectName).dll',
                                    'AdditionalLibraryDirectories':
                                    [
                                    ],
                                    'AdditionalDependencies':
                                    [
                                    ],
                                },
                            },
                        },
                    },
                ],
            ],
        },
    },

    'targets':
    [
        {
            'target_name': 'fxfile-keyhook',
            
            'type': 'shared_library',
            
            'defines':
            [
                'FXFILE_KEYHOOK_EXPORTS',
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
                './fxfile-keyhook.cpp',
                './fxfile-keyhook.h',
                './stdafx.cpp',
                './stdafx.h',
            ],
        },
    ],
}
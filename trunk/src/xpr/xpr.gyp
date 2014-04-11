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
        'default_configuration' : 'Debug-x86-Unicode',
        
        'configurations':
        {
            'Debug-x86-Unicode':
            {
                'inherit_from': ['Debug-x86-Unicode_Base'],
                
                'msvs_configuration_attributes':
                {
                    'IntermediateDirectory': '../../obj/xpr/dbg-x86-unicode',
                    'OutputDirectory': '../../bin/x86-unicode',
                },
                
                'msvs_settings':
                {
                    'VCCLCompilerTool':
                    {
                        'UsePrecompiledHeader': 3,
                    },
                
                    'VCLinkerTool': 
                    {
                        'OutputFile': '$(OutDir)\lib$(ProjectName)w_dbg.dll',
                        'AdditionalLibraryDirectories':
                        [
                            '../../lib/libxml2/lib',
                        ]
                    },
                },
            },
            
            'Release-x86-Unicode':
            {
                'inherit_from': ['Release-x86-Unicode_Base'],
                
                'msvs_configuration_attributes':
                {
                    'IntermediateDirectory': '../../obj/xpr/rel-x86-unicode',
                    'OutputDirectory': '../../bin/x86-unicode',
                },
                
                'msvs_settings':
                {
                    'VCCLCompilerTool':
                    {
                        'UsePrecompiledHeader': 3,
                    },
                    
                    'VCLinkerTool': 
                    {
                        'OutputFile': '$(OutDir)\lib$(ProjectName)w.dll',
                        'AdditionalLibraryDirectories':
                        [
                            '../../lib/libxml2/lib',
                        ]
                    },
                },
            },
            
            'Debug-x86-Multibyte':
            {
                'inherit_from': ['Debug-x86-Multibyte_Base'],
                
                'msvs_configuration_attributes':
                {
                    'IntermediateDirectory': '../../obj/xpr/dbg-x86-multibyte',
                    'OutputDirectory': '../../bin/x86-multibyte',
                },
                
                'msvs_settings':
                {
                    'VCCLCompilerTool':
                    {
                        'UsePrecompiledHeader': 3,
                    },
                
                    'VCLinkerTool': 
                    {
                        'OutputFile': '$(OutDir)\lib$(ProjectName)_dbg.dll',
                        'AdditionalLibraryDirectories':
                        [
                            '../../lib/libxml2/lib',
                        ]
                    },
                },

                'msvs_postbuild': 'xcopy $(TargetDir)$(TargetName).lib $(TargetDir).. /c /r /y\r\n'
                                  'xcopy $(TargetDir)$(TargetFileName) $(TargetDir).. /c /r /y\r\n',
            },
            
            'Release-x86-Multibyte':
            {
                'inherit_from': ['Release-x86-Multibyte_Base'],
                
                'msvs_configuration_attributes':
                {
                    'IntermediateDirectory': '../../obj/xpr/rel-x86-multibyte',
                    'OutputDirectory': '../../bin/x86-multibyte',
                },
                
                'msvs_settings':
                {
                    'VCCLCompilerTool':
                    {
                        'UsePrecompiledHeader': 3,
                    },
                    
                    'VCLinkerTool': 
                    {
                        'OutputFile': '$(OutDir)\lib$(ProjectName).dll',
                        'AdditionalLibraryDirectories':
                        [
                            '../../lib/libxml2/lib',
                        ]
                    },
                },

                'msvs_postbuild': 'xcopy $(TargetDir)$(TargetName).lib $(TargetDir).. /c /r /y\r\n'
                                  'xcopy $(TargetDir)$(TargetFileName) $(TargetDir).. /c /r /y\r\n',
            },
            
            'conditions':
            [
                [ 'target_arch!="x86"',
                    {
                        'Debug-x64-Unicode':
                        {
                            'inherit_from': ['Debug-x64-Unicode_Base'],
                            
                            'msvs_configuration_attributes':
                            {
                                'IntermediateDirectory': '../../obj/xpr/dbg-x64',
                                'OutputDirectory': '../../bin/x64',
                            },
                            
                            'msvs_settings':
                            {
                                'VCCLCompilerTool':
                                {
                                    'UsePrecompiledHeader': 3,
                                },
                                
                                'VCLinkerTool': 
                                {
                                    'OutputFile': '$(OutDir)\lib$(ProjectName)w_dbg.dll',
                                    'AdditionalLibraryDirectories':
                                    [
                                        '../../lib/libxml2/lib64',
                                    ]
                                },
                            },
                        },

                        'Release-x64-Unicode':
                        {
                            'inherit_from': ['Release-x64-Unicode_Base'],
                            
                            'msvs_configuration_attributes':
                            {
                                'IntermediateDirectory': '../../obj/xpr/rel-x64',
                                'OutputDirectory': '../../bin/x64',
                            },
                            
                            'msvs_settings':
                            {
                                'VCCLCompilerTool':
                                {
                                    'UsePrecompiledHeader': 3,
                                },
                                
                                'VCLinkerTool': 
                                {
                                    'OutputFile': '$(OutDir)\lib$(ProjectName)w.dll',
                                    'AdditionalLibraryDirectories':
                                    [
                                        '../../lib/libxml2/lib64',
                                    ]
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
            'target_name': 'xpr',
            
            'type': 'shared_library',
            
            'defines':
            [
                'XPR_CFG_DL_EXPORT',
            ],
            
            'include_dirs':
            [
                '../../src/xpr/include',
                '../../lib/iconv/include',
                '../../lib/libxml2/include/libxml2',
            ],
            
            'libraries':
            [
                'libxml2.dll.a',
            ],
            
            'msvs_settings':
            {
                'VCLinkerTool': 
                {
                    'AdditionalLibraryDirectories':
                    [
                    ]
                },
            },
            
            'sources':
            [
                './xpr/xpr_atomic_win.cpp',
                './xpr/xpr_char.cpp',
                './xpr/xpr_char_set.cpp',
                './xpr/xpr_cstr.cpp',
                './xpr/xpr_debug_win.cpp',
                './xpr/xpr_dllmain_win.cpp',
                './xpr/xpr_env.cpp',
                './xpr/xpr_file_io_win.cpp',
                './xpr/xpr_file_path_win.cpp',
                './xpr/xpr_file_sys_win.cpp',
                './xpr/xpr_guid.cpp',
                './xpr/xpr_init.cpp',
                './xpr/xpr_lazy_library_load_win.cpp',
                './xpr/xpr_lazy_library_load.h',
                './xpr/xpr_memory.cpp',
                './xpr/xpr_process_sync_win.cpp',
                './xpr/xpr_process_win.cpp',
                './xpr/xpr_rcode.cpp',
                './xpr/xpr_shm_win.cpp',
                './xpr/xpr_string.cpp',
                './xpr/xpr_string_iterator.cpp',
                './xpr/xpr_system_win.cpp',
                './xpr/xpr_text_file_io.cpp',
                './xpr/xpr_thread_sync_win.cpp',
                './xpr/xpr_thread_win.cpp',
                './xpr/xpr_time_win.cpp',
                './xpr/xpr_trace_logger.cpp',
                './xpr/xpr_ustring.cpp',
                './xpr/xpr_ver.cpp',
                './xpr/xpr_xml.cpp',

                './include/xpr.h',
                './include/xpr_atomic.h',
                './include/xpr_bit.h',
                './include/xpr_buffer.h',
                './include/xpr_char.h',
                './include/xpr_char_set.h',
                './include/xpr_config.h',
                './include/xpr_config_win.h',
                './include/xpr_cstr.h',
                './include/xpr_debug.h',
                './include/xpr_define.h',
                './include/xpr_dlsym.h',
                './include/xpr_endian.h',
                './include/xpr_env.h',
                './include/xpr_file_io.h',
                './include/xpr_file_path.h',
                './include/xpr_file_sys.h',
                './include/xpr_guid.h',
                './include/xpr_init.h',
                './include/xpr_math.h',
                './include/xpr_memory.h',
                './include/xpr_memory_barrier.h',
                './include/xpr_net_io.h',
                './include/xpr_process.h',
                './include/xpr_process_sync.h',
                './include/xpr_rcode.h',
                './include/xpr_shm.h',
                './include/xpr_std.h',
                './include/xpr_string.h',
                './include/xpr_string_iterator.h',
                './include/xpr_system.h',
                './include/xpr_tchar.h',
                './include/xpr_text_file_io.h',
                './include/xpr_thread.h',
                './include/xpr_thread_sync.h',
                './include/xpr_time.h',
                './include/xpr_trace_logger.h',
                './include/xpr_types.h',
                './include/xpr_uri.h',
                './include/xpr_ustring.h',
                './include/xpr_ver.h',
                './include/xpr_xml.h',
            ]
        }
    ]
}
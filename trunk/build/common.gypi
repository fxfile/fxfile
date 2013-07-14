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
        'target_arch%': 'x86',
    },
    
    'target_defaults':
    {
        'defines':
        [
            'WIN32',
            '_WINDOWS',
            '_USRDLL',
            '_CRT_SECURE_NO_WARNINGS',
            '_CRT_NON_CONFORMING_SWPRINTFS',
        ],
        
        'msvs_settings':
        {
            'VCCLCompilerTool':
            {
                'BufferSecurityCheck':      'true',
            },
            'VCLinkerTool': 
            {
                'GenerateDebugInformation': 'true',
                'SubSystem':                2,
                'TargetMachine':            1,
                'ProgramDatabaseFile':      '$(TargetDir)$(TargetName).pdb'
            },
        },
        
        'configurations':
        {
            'Common_Base':
            {
                'abstract': 1,
            },
            
            'Debug_Base': 
            {
                'abstract': 1,
                'defines':
                [
                    'DEBUG',
                    '_DEBUG',
                ],
                'msvs_configuration_attributes':
                {
                    'CharacterSet':          '0'
                },
                'msvs_settings':
                {
                    'VCCLCompilerTool':
                    {
                        'RuntimeLibrary':             1, # static debug (MTD)
                        'Optimization':               0,
                        'MinimalRebuild':             'true',
                        'BasicRuntimeChecks':         3,
                        'WarningLevel':               3,
                        'DebugInformationFormat':     4,
                        'Detect64BitPortabilityProblems': 'true',
                    },
                    'VCLinkerTool': 
                    {
                        'LinkIncremental':            2,
                    },
                },
            },
            
            'Release_Base':
            {
                'abstract': 1,
                'defines':
                [
                    'NDEBUG',
                ],
                'msvs_configuration_attributes':
                {
                    'CharacterSet':          '0'
                },
                'msvs_settings':
                {
                    'VCCLCompilerTool':
                    {
                        'RuntimeLibrary':             0, # static release (MT)
                        'EnableIntrinsicFunctions':   'true',
                        'EnableFunctionLevelLinking': 'true',
                        'Optimization':               2,
                        'WholeProgramOptimization':   'true', # /GL
                        'WarningLevel':               3,
                        'DebugInformationFormat':     3,
                    },
                    'VCLinkerTool': 
                    {
                        'LinkIncremental':            1,
                        'OptimizeReferences':         2,
                        'EnableCOMDATFolding':        2,
                        'GenerateMapFile':            'true',
                        'MapFileName':                '$(TargetDir)$(TargetName).map',
                        'MapExports':                 'true',
                    },
                },
            },
            
            'MFC_Base':
            {
                'abstract': 1,
                'msvs_configuration_attributes':
                {
                    'UseOfMFC':              1,
                    'ATLMinimizesCRunTimeLibraryUsage': 'false',
                    'ConfigurationType':     1,
                },
                'msvs_settings':
                {
                    'VCCLCompilerTool':
                    {
                        'SuppressStartupBanner':          'true',
                    },
                    'VCLinkerTool': 
                    {
                        'RandomizedBaseAddress':           1,
                    },
                    'VCResourceCompilerTool':
                    {
                        'AdditionalIncludeDirectories':    '',
                        'PreprocessorDefinitions':         [ '_DEBUG' ],
                        'Culture':                         0,
                    },
                },
            },
            
            'MFC-Ansicode_Base':
            {
                'abstract': 1,
                'inherit_from': [ 'MFC_Base' ],
                
                'msvs_configuration_attributes':
                {
                    'CharacterSet':          2,
                },
                'msvs_settings':
                {
                    'VCLinkerTool': 
                    {
                    },
                },
            },
            
            'MFC-Unicode_Base':
            {
                'abstract': 1,
                'inherit_from': [ 'MFC_Base' ],
                
                'msvs_configuration_attributes':
                {
                    'CharacterSet':          1,
                },
                'msvs_settings':
                {
                    'VCLinkerTool': 
                    {
                        'EntryPointSymbol': 'wWinMainCRTStartup',
                    },
                },
            },
            
            'x86_Base':
            {
                'abstract': 1,
                'msvs_settings':
                {
                    'VCLinkerTool':
                    {
                        'TargetMachine': '1',
                    },
                },
                'msvs_configuration_platform': 'Win32',
            },

            'x64_Base':
            {
                'abstract': 1,
                'msvs_settings':
                {
                    'VCLinkerTool':
                    {
                        'TargetMachine': '17',  # x86-64
                    },
                },
                'msvs_configuration_platform': 'x64',
            },
            
            'Debug-x86_Base':
            {
                'abstract': 1,
                'inherit_from': [ 'Common_Base', 'x86_Base', 'Debug_Base' ],
            },
            
            'Release-x86_Base':
            {
                'abstract': 1,
                'inherit_from': [ 'Common_Base', 'x86_Base', 'Release_Base' ],
            },
            
            'Debug-x64_Base':
            {
                'abstract': 1,
                'inherit_from': [ 'Common_Base', 'x64_Base', 'Debug_Base' ],
            },
            
            'Release-x64_Base':
            {
                'abstract': 1,
                'inherit_from': [ 'Common_Base', 'x64_Base', 'Release_Base' ],
            },
            
            'Debug-x86-MFC-Unicode_Base':
            {
                'abstract': 1,
                'inherit_from': [ 'Common_Base', 'x86_Base', 'Debug_Base', 'MFC-Unicode_Base' ],
            },
            
            'Release-x86-MFC-Unicode_Base':
            {
                'abstract': 1,
                'inherit_from': [ 'Common_Base', 'x86_Base', 'Release_Base', 'MFC-Unicode_Base' ],
            },
            
            'Debug-x86-MFC-Ansicode_Base':
            {
                'abstract': 1,
                'inherit_from': [ 'Common_Base', 'x86_Base', 'Debug_Base', 'MFC-Ansicode_Base' ],
            },
            
            'Release-x86-MFC-Ansicode_Base':
            {
                'abstract': 1,
                'inherit_from': [ 'Common_Base', 'x86_Base', 'Release_Base', 'MFC-Ansicode_Base' ],
            },
            
            'Debug-x64-MFC-Unicode_Base':
            {
                'abstract': 1,
                'inherit_from': [ 'Common_Base', 'x64_Base', 'Debug_Base', 'MFC-Unicode_Base' ],
            },
            
            'Release-x64-MFC-Unicode_Base':
            {
                'abstract': 1,
                'inherit_from': [ 'Common_Base', 'x64_Base', 'Release_Base', 'MFC-Unicode_Base' ],
            },
            
            'Debug-x64-MFC-Ansicode_Base':
            {
                'abstract': 1,
                'inherit_from': [ 'Common_Base', 'x64_Base', 'Debug_Base', 'MFC-Ansicode_Base' ],
            },
            
            'Release-x64-MFC-Ansicode_Base':
            {
                'abstract': 1,
                'inherit_from': [ 'Common_Base', 'x64_Base', 'Release_Base', 'MFC-Ansicode_Base' ],
            },
        },
    },
}

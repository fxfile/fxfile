#ifndef __FXFILE_CRASH_H__
#define __FXFILE_CRASH_H__ 1
#pragma once

#ifdef FXCRASH_EXPORTS
    #define FXCRASH_API __declspec(dllexport)
#else
    #define FXCRASH_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

FXCRASH_API void    APIENTRY fxCrashHandler();
FXCRASH_API LPCTSTR APIENTRY fxGetAppName();
FXCRASH_API void    APIENTRY fxSetAppName(LPCTSTR lpcszAppName);
FXCRASH_API LPCTSTR APIENTRY fxGetAppVer();
FXCRASH_API void    APIENTRY fxSetAppVer(LPCTSTR lpcszAppName);
FXCRASH_API void    APIENTRY fxSetDevelopInfo(LPCTSTR lpcszEmail, LPCTSTR lpcszHomepage);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __FXFILE_CRASH_H__

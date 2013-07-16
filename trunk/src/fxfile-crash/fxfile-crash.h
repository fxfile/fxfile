#ifndef __FXFILE_CRASH_H__
#define __FXFILE_CRASH_H__ 1
#pragma once

#ifdef FXFILE_CRASH_DL_EXPORTS
    #define FXFILE_CRASH_DL_API __declspec(dllexport)
#else
    #define FXFILE_CRASH_DL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

FXFILE_CRASH_DL_API void    APIENTRY fxfile_crash_init();
FXFILE_CRASH_DL_API LPCTSTR APIENTRY fxfile_crash_getAppName();
FXFILE_CRASH_DL_API void    APIENTRY fxfile_crash_setAppName(LPCTSTR lpcszAppName);
FXFILE_CRASH_DL_API LPCTSTR APIENTRY fxfile_crash_getAppVer();
FXFILE_CRASH_DL_API void    APIENTRY fxfile_crash_setAppVer(LPCTSTR lpcszAppName);
FXFILE_CRASH_DL_API void    APIENTRY fxfile_crash_setDevelopInfo(LPCTSTR lpcszEmail, LPCTSTR lpcszHomepage);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __FXFILE_CRASH_H__

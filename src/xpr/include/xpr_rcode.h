//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_RCODE_H__
#define __XPR_RCODE_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"

namespace xpr
{
//
// error code range
//
#define XPR_RCODE_PSX 0        // posix error code       (     0 ~ 19999)
#define XPR_RCODE_WIN 20000    // windows error code     ( 20000 ~ 49999)
#define XPR_RCODE_XPR 50000    // XPR error code         ( 50000 ~ 99999)
#define XPR_RCODE_APP 100000   // application error code (100000 ~      )

//
// error conversion macros
//
#if defined(XPR_CFG_OS_WINDOWS)
#define XPR_RCODE_CONVERT(e)          ((e) + XPR_RCODE_WIN)
#define XPR_RCODE_REVERT(e)           ((e) - XPR_RCODE_WIN)
#define XPR_RCODE_FROM_OS_ERROR(e)    (((e) == 0) ? (XPR_RCODE_SUCCESS) : XPR_RCODE_CONVERT(e))
#define XPR_RCODE_TO_OS_ERROR(e)      (((e) == 0) ? (XPR_RCODE_SUCCESS) : XPR_RCODE_REVERT(e))
#define XPR_RCODE_GET_OS_ERROR()      (XPR_RCODE_FROM_OS_ERROR(::GetLastError()))
#define XPR_RCODE_SET_OS_ERROR(e)     (::SetLastError(XPR_RCODE_TO_OS_ERROR(e)))
#define XPR_RCODE_GET_OS_NET_ERROR()  (XPR_RCODE_FROM_OS_ERROR(::WSAGetLastError()))
#define XPR_RCODE_SET_OS_NET_ERROR(e) (::WSASetLastError(XPR_RCODE_TO_OS_ERROR(e)))
#else
#define XPR_RCODE_CONVERT(e)          (e)
#define XPR_RCODE_REVERT(e)           (e)
#define XPR_RCODE_FROM_OS_ERROR(e)    (e)
#define XPR_RCODE_TO_OS_ERROR(e)      (e)
#define XPR_RCODE_GET_OS_ERROR()      (errno)
#define XPR_RCODE_SET_OS_ERROR(e)     (errno = (e))
#define XPR_RCODE_GET_OS_NET_ERROR()  (errno)
#define XPR_RCODE_SET_OS_NET_ERROR(e) (errno = (e))
#endif

//
// error code definition (POSIX compatible)
//
#define XPR_RCODE_SUCCESS         (0)
#define XPR_RCODE_EPERM           (XPR_RCODE_XPR + 1)
#define XPR_RCODE_ENOENT          (XPR_RCODE_XPR + 2)
#define XPR_RCODE_ESRCH           (XPR_RCODE_XPR + 3)
#define XPR_RCODE_EINTR           (XPR_RCODE_XPR + 4)
#define XPR_RCODE_EIO             (XPR_RCODE_XPR + 5)
#define XPR_RCODE_ENXIO           (XPR_RCODE_XPR + 6)
#define XPR_RCODE_E2BIG           (XPR_RCODE_XPR + 7)
#define XPR_RCODE_ENOEXEC         (XPR_RCODE_XPR + 8)
#define XPR_RCODE_EBADF           (XPR_RCODE_XPR + 9)
#define XPR_RCODE_ECHILD          (XPR_RCODE_XPR + 10)
#define XPR_RCODE_EAGAIN          (XPR_RCODE_XPR + 11)
#define XPR_RCODE_ENOMEM          (XPR_RCODE_XPR + 12)
#define XPR_RCODE_EACCES          (XPR_RCODE_XPR + 13)
#define XPR_RCODE_EFAULT          (XPR_RCODE_XPR + 14)
#define XPR_RCODE_EBUSY           (XPR_RCODE_XPR + 16)
#define XPR_RCODE_EEXIST          (XPR_RCODE_XPR + 17)
#define XPR_RCODE_EXDEV           (XPR_RCODE_XPR + 18)
#define XPR_RCODE_ENODEV          (XPR_RCODE_XPR + 19)
#define XPR_RCODE_ENOTDIR         (XPR_RCODE_XPR + 20)
#define XPR_RCODE_EISDIR          (XPR_RCODE_XPR + 21)
#define XPR_RCODE_EINVAL          (XPR_RCODE_XPR + 22)
#define XPR_RCODE_ENFILE          (XPR_RCODE_XPR + 23)
#define XPR_RCODE_EMFILE          (XPR_RCODE_XPR + 24)
#define XPR_RCODE_ENOTTY          (XPR_RCODE_XPR + 25)
#define XPR_RCODE_EFBIG           (XPR_RCODE_XPR + 27)
#define XPR_RCODE_ENOSPC          (XPR_RCODE_XPR + 28)
#define XPR_RCODE_ESPIPE          (XPR_RCODE_XPR + 29)
#define XPR_RCODE_EROFS           (XPR_RCODE_XPR + 30)
#define XPR_RCODE_EMLINK          (XPR_RCODE_XPR + 31)
#define XPR_RCODE_EPIPE           (XPR_RCODE_XPR + 32)
#define XPR_RCODE_EDOM            (XPR_RCODE_XPR + 33)
#define XPR_RCODE_ERANGE          (XPR_RCODE_XPR + 34)
#define XPR_RCODE_EDEADLK         (XPR_RCODE_XPR + 36)
#define XPR_RCODE_ENAMETOOLONG    (XPR_RCODE_XPR + 38)
#define XPR_RCODE_ENOLCK          (XPR_RCODE_XPR + 39)
#define XPR_RCODE_ENOSYS          (XPR_RCODE_XPR + 40)
#define XPR_RCODE_ENOTEMPTY       (XPR_RCODE_XPR + 41)
#define XPR_RCODE_EILSEQ          (XPR_RCODE_XPR + 42)
#define XPR_RCODE_EADDRINUSE      (XPR_RCODE_XPR + 100)
#define XPR_RCODE_EADDRNOTAVAIL   (XPR_RCODE_XPR + 101)
#define XPR_RCODE_EAFNOSUPPORT    (XPR_RCODE_XPR + 102)
#define XPR_RCODE_EALREADY        (XPR_RCODE_XPR + 103)
#define XPR_RCODE_EBADMSG         (XPR_RCODE_XPR + 104)
#define XPR_RCODE_ECANCELED       (XPR_RCODE_XPR + 105)
#define XPR_RCODE_ECONNABORTED    (XPR_RCODE_XPR + 106)
#define XPR_RCODE_ECONNREFUSED    (XPR_RCODE_XPR + 107)
#define XPR_RCODE_ECONNRESET      (XPR_RCODE_XPR + 108)
#define XPR_RCODE_EDESTADDRREQ    (XPR_RCODE_XPR + 109)
#define XPR_RCODE_EHOSTUNREACH    (XPR_RCODE_XPR + 110)
#define XPR_RCODE_EIDRM           (XPR_RCODE_XPR + 111)
#define XPR_RCODE_EINPROGRESS     (XPR_RCODE_XPR + 112)
#define XPR_RCODE_EISCONN         (XPR_RCODE_XPR + 113)
#define XPR_RCODE_ELOOP           (XPR_RCODE_XPR + 114)
#define XPR_RCODE_EMSGSIZE        (XPR_RCODE_XPR + 115)
#define XPR_RCODE_ENETDOWN        (XPR_RCODE_XPR + 116)
#define XPR_RCODE_ENETRESET       (XPR_RCODE_XPR + 117)
#define XPR_RCODE_ENETUNREACH     (XPR_RCODE_XPR + 118)
#define XPR_RCODE_ENOBUFS         (XPR_RCODE_XPR + 119)
#define XPR_RCODE_ENODATA         (XPR_RCODE_XPR + 120)
#define XPR_RCODE_ENOLINK         (XPR_RCODE_XPR + 121)
#define XPR_RCODE_ENOMSG          (XPR_RCODE_XPR + 122)
#define XPR_RCODE_ENOPROTOOPT     (XPR_RCODE_XPR + 123)
#define XPR_RCODE_ENOSR           (XPR_RCODE_XPR + 124)
#define XPR_RCODE_ENOSTR          (XPR_RCODE_XPR + 125)
#define XPR_RCODE_ENOTCONN        (XPR_RCODE_XPR + 126)
#define XPR_RCODE_ENOTRECOVERABLE (XPR_RCODE_XPR + 127)
#define XPR_RCODE_ENOTSOCK        (XPR_RCODE_XPR + 128)
#define XPR_RCODE_ENOTSUP         (XPR_RCODE_XPR + 129)
#define XPR_RCODE_EOPNOTSUPP      (XPR_RCODE_XPR + 130)
#define XPR_RCODE_EOVERFLOW       (XPR_RCODE_XPR + 132)
#define XPR_RCODE_EOWNERDEAD      (XPR_RCODE_XPR + 133)
#define XPR_RCODE_EPROTO          (XPR_RCODE_XPR + 134)
#define XPR_RCODE_EPROTONOSUPPORT (XPR_RCODE_XPR + 135)
#define XPR_RCODE_EPROTOTYPE      (XPR_RCODE_XPR + 136)
#define XPR_RCODE_ETIME           (XPR_RCODE_XPR + 137)
#define XPR_RCODE_ETIMEDOUT       (XPR_RCODE_XPR + 138)
#define XPR_RCODE_ETXTBSY         (XPR_RCODE_XPR + 139)
#define XPR_RCODE_EWOULDBLOCK     (XPR_RCODE_XPR + 140)

//
// if error code alreay exist
//
#if defined(EPERM)
#undef  XPR_RCODE_EPERM
#define XPR_RCODE_EPERM           EPERM
#endif
#if defined(ENOENT)
#undef  XPR_RCODE_ENOENT
#define XPR_RCODE_ENOENT          ENOENT
#endif
#if defined(ESRCH)
#undef  XPR_RCODE_ESRCH
#define XPR_RCODE_ESRCH           ESRCH
#endif
#if defined(EINTR)
#undef  XPR_RCODE_EINTR
#define XPR_RCODE_EINTR           EINTR
#endif
#if defined(EIO)
#undef  XPR_RCODE_EIO
#define XPR_RCODE_EIO             EIO
#endif
#if defined(ENXIO)
#undef  XPR_RCODE_ENXIO
#define XPR_RCODE_ENXIO           ENXIO
#endif
#if defined(E2BIG)
#undef  XPR_RCODE_E2BIG
#define XPR_RCODE_E2BIG           E2BIG
#endif
#if defined(ENOEXEC)
#undef  XPR_RCODE_ENOEXEC
#define XPR_RCODE_ENOEXEC         ENOEXEC
#endif
#if defined(EBADF)
#undef  XPR_RCODE_EBADF
#define XPR_RCODE_EBADF           EBADF
#endif
#if defined(ECHILD)
#undef  XPR_RCODE_ECHILD
#define XPR_RCODE_ECHILD          ECHILD
#endif
#if defined(EAGAIN)
#undef  XPR_RCODE_EAGAIN
#define XPR_RCODE_EAGAIN          EAGAIN
#endif
#if defined(ENOMEM)
#undef  XPR_RCODE_ENOMEM
#define XPR_RCODE_ENOMEM          ENOMEM
#endif
#if defined(EACCES)
#undef  XPR_RCODE_EACCES
#define XPR_RCODE_EACCES          EACCES
#endif
#if defined(EFAULT)
#undef  XPR_RCODE_EFAULT
#define XPR_RCODE_EFAULT          EFAULT
#endif
#if defined(EBUSY)
#undef  XPR_RCODE_EBUSY
#define XPR_RCODE_EBUSY           EBUSY
#endif
#if defined(EEXIST)
#undef  XPR_RCODE_EEXIST
#define XPR_RCODE_EEXIST          EEXIST
#endif
#if defined(EXDEV)
#undef  XPR_RCODE_EXDEV
#define XPR_RCODE_EXDEV           EXDEV
#endif
#if defined(ENODEV)
#undef  XPR_RCODE_ENODEV
#define XPR_RCODE_ENODEV          ENODEV
#endif
#if defined(ENOTDIR)
#undef  XPR_RCODE_ENOTDIR
#define XPR_RCODE_ENOTDIR         ENOTDIR
#endif
#if defined(EISDIR)
#undef  XPR_RCODE_EISDIR
#define XPR_RCODE_EISDIR          EISDIR
#endif
#if defined(EINVAL)
#undef  XPR_RCODE_EINVAL
#define XPR_RCODE_EINVAL          EINVAL
#endif
#if defined(ENFILE)
#undef  XPR_RCODE_ENFILE
#define XPR_RCODE_ENFILE          ENFILE
#endif
#if defined(EMFILE)
#undef  XPR_RCODE_EMFILE
#define XPR_RCODE_EMFILE          EMFILE
#endif
#if defined(ENOTTY)
#undef  XPR_RCODE_ENOTTY
#define XPR_RCODE_ENOTTY          ENOTTY
#endif
#if defined(EFBIG)
#undef  XPR_RCODE_EFBIG
#define XPR_RCODE_EFBIG           EFBIG
#endif
#if defined(ENOSPC)
#undef  XPR_RCODE_ENOSPC
#define XPR_RCODE_ENOSPC          ENOSPC
#endif
#if defined(ESPIPE)
#undef  XPR_RCODE_ESPIPE
#define XPR_RCODE_ESPIPE          ESPIPE
#endif
#if defined(EROFS)
#undef  XPR_RCODE_EROFS
#define XPR_RCODE_EROFS           EROFS
#endif
#if defined(EMLINK)
#undef  XPR_RCODE_EMLINK
#define XPR_RCODE_EMLINK          EMLINK
#endif
#if defined(EPIPE)
#undef  XPR_RCODE_EPIPE
#define XPR_RCODE_EPIPE           EPIPE
#endif
#if defined(EDOM)
#undef  XPR_RCODE_EDOM
#define XPR_RCODE_EDOM            EDOM
#endif
#if defined(ERANGE)
#undef  XPR_RCODE_ERANGE
#define XPR_RCODE_ERANGE          ERANGE
#endif
#if defined(EDEADLK)
#undef  XPR_RCODE_EDEADLK
#define XPR_RCODE_EDEADLK         EDEADLK
#endif
#if defined(ENAMETOOLONG)
#undef  XPR_RCODE_ENAMETOOLONG
#define XPR_RCODE_ENAMETOOLONG    ENAMETOOLONG
#endif
#if defined(ENOLCK)
#undef  XPR_RCODE_ENOLCK
#define XPR_RCODE_ENOLCK          ENOLCK
#endif
#if defined(ENOSYS)
#undef  XPR_RCODE_ENOSYS
#define XPR_RCODE_ENOSYS          ENOSYS
#endif
#if defined(ENOTEMPTY)
#undef  XPR_RCODE_ENOTEMPTY
#define XPR_RCODE_ENOTEMPTY       ENOTEMPTY
#endif
#if defined(EILSEQ)
#undef  XPR_RCODE_EILSEQ
#define XPR_RCODE_EILSEQ          EILSEQ
#endif
#if defined(EADDRINUSE)
#undef  XPR_RCODE_EADDRINUSE
#define XPR_RCODE_EADDRINUSE      EADDRINUSE
#endif
#if defined(EADDRNOTAVAIL)
#undef  XPR_RCODE_EADDRNOTAVAIL
#define XPR_RCODE_EADDRNOTAVAIL   EADDRNOTAVAIL
#endif
#if defined(EAFNOSUPPORT)
#undef  XPR_RCODE_EAFNOSUPPORT
#define XPR_RCODE_EAFNOSUPPORT    EAFNOSUPPORT
#endif
#if defined(EALREADY)
#undef  XPR_RCODE_EALREADY
#define XPR_RCODE_EALREADY        EALREADY
#endif
#if defined(EBADMSG)
#undef  XPR_RCODE_EBADMSG
#define XPR_RCODE_EBADMSG         EBADMSG
#endif
#if defined(ECANCELED)
#undef  XPR_RCODE_ECANCELED
#define XPR_RCODE_ECANCELED       ECANCELED
#endif
#if defined(ECONNABORTED)
#undef  XPR_RCODE_ECONNABORTED
#define XPR_RCODE_ECONNABORTED    ECONNABORTED
#endif
#if defined(ECONNREFUSED)
#undef  XPR_RCODE_ECONNREFUSED
#define XPR_RCODE_ECONNREFUSED    ECONNREFUSED
#endif
#if defined(ECONNRESET)
#undef  XPR_RCODE_ECONNRESET
#define XPR_RCODE_ECONNRESET      ECONNRESET
#endif
#if defined(EDESTADDRREQ)
#undef  XPR_RCODE_EDESTADDRREQ
#define XPR_RCODE_EDESTADDRREQ    EDESTADDRREQ
#endif
#if defined(EHOSTUNREACH)
#undef  XPR_RCODE_EHOSTUNREACH
#define XPR_RCODE_EHOSTUNREACH    EHOSTUNREACH
#endif
#if defined(EIDRM)
#undef  XPR_RCODE_EIDRM
#define XPR_RCODE_EIDRM           EIDRM
#endif
#if defined(EINPROGRESS)
#undef  XPR_RCODE_EINPROGRESS
#define XPR_RCODE_EINPROGRESS     EINPROGRESS
#endif
#if defined(EISCONN)
#undef  XPR_RCODE_EISCONN
#define XPR_RCODE_EISCONN         EISCONN
#endif
#if defined(ELOOP)
#undef  XPR_RCODE_ELOOP
#define XPR_RCODE_ELOOP           ELOOP
#endif
#if defined(EMSGSIZE)
#undef  XPR_RCODE_EMSGSIZE
#define XPR_RCODE_EMSGSIZE        EMSGSIZE
#endif
#if defined(ENETDOWN)
#undef  XPR_RCODE_ENETDOWN
#define XPR_RCODE_ENETDOWN        ENETDOWN
#endif
#if defined(ENETRESET)
#undef  XPR_RCODE_ENETRESET
#define XPR_RCODE_ENETRESET       ENETRESET
#endif
#if defined(ENETUNREACH)
#undef  XPR_RCODE_ENETUNREACH
#define XPR_RCODE_ENETUNREACH     ENETUNREACH
#endif
#if defined(ENOBUFS)
#undef  XPR_RCODE_ENOBUFS
#define XPR_RCODE_ENOBUFS         ENOBUFS
#endif
#if defined(ENODATA)
#undef  XPR_RCODE_ENODATA
#define XPR_RCODE_ENODATA         ENODATA
#endif
#if defined(ENOLINK)
#undef  XPR_RCODE_ENOLINK
#define XPR_RCODE_ENOLINK         ENOLINK
#endif
#if defined(ENOMSG)
#undef  XPR_RCODE_ENOMSG
#define XPR_RCODE_ENOMSG          ENOMSG
#endif
#if defined(ENOPROTOOPT)
#undef  XPR_RCODE_ENOPROTOOPT
#define XPR_RCODE_ENOPROTOOPT     ENOPROTOOPT
#endif
#if defined(ENOSR)
#undef  XPR_RCODE_ENOSR
#define XPR_RCODE_ENOSR           ENOSR
#endif
#if defined(ENOSTR)
#undef  XPR_RCODE_ENOSTR
#define XPR_RCODE_ENOSTR          ENOSTR
#endif
#if defined(ENOTCONN)
#undef  XPR_RCODE_ENOTCONN
#define XPR_RCODE_ENOTCONN        ENOTCONN
#endif
#if defined(ENOTRECOVERABLE)
#undef  XPR_RCODE_ENOTRECOVERABLE
#define XPR_RCODE_ENOTRECOVERABLE ENOTRECOVERABLE
#endif
#if defined(ENOTSOCK)
#undef  XPR_RCODE_ENOTSOCK
#define XPR_RCODE_ENOTSOCK        ENOTSOCK
#endif
#if defined(ENOTSUP)
#undef  XPR_RCODE_ENOTSUP
#define XPR_RCODE_ENOTSUP         ENOTSUP
#endif
#if defined(EOPNOTSUPP)
#undef  XPR_RCODE_EOPNOTSUPP
#define XPR_RCODE_EOPNOTSUPP      EOPNOTSUPP
#endif
#if defined(EOVERFLOW)
#undef  XPR_RCODE_EOVERFLOW
#define XPR_RCODE_EOVERFLOW       EOVERFLOW
#endif
#if defined(EOWNERDEAD)
#undef  XPR_RCODE_EOWNERDEAD
#define XPR_RCODE_EOWNERDEAD      EOWNERDEAD
#endif
#if defined(EPROTO)
#undef  XPR_RCODE_EPROTO
#define XPR_RCODE_EPROTO          EPROTO
#endif
#if defined(EPROTONOSUPPORT)
#undef  XPR_RCODE_EPROTONOSUPPORT
#define XPR_RCODE_EPROTONOSUPPORT EPROTONOSUPPORT
#endif
#if defined(EPROTOTYPE)
#undef  XPR_RCODE_EPROTOTYPE
#define XPR_RCODE_EPROTOTYPE      EPROTOTYPE
#endif
#if defined(ETIME)
#undef  XPR_RCODE_ETIME
#define XPR_RCODE_ETIME           ETIME
#endif
#if defined(ETIMEDOUT)
#undef  XPR_RCODE_ETIMEDOUT
#define XPR_RCODE_ETIMEDOUT       ETIMEDOUT
#endif
#if defined(ETXTBSY)
#undef  XPR_RCODE_ETXTBSY
#define XPR_RCODE_ETXTBSY         ETXTBSY
#endif
#if defined(EWOULDBLOCK)
#undef  XPR_RCODE_EWOULDBLOCK
#define XPR_RCODE_EWOULDBLOCK     EWOULDBLOCK
#endif

//
// test error code
//
#if defined(XPR_CFG_OS_WINDOWS)

#define XPR_RCODE_IS_SUCCESS(e)              ((e) == XPR_RCODE_SUCCESS)
#define XPR_RCODE_IS_ERROR(e)                ((e) != XPR_RCODE_SUCCESS)
#define XPR_RCODE_IS_EPERM(e)                                                 \
    (((e) == XPR_RCODE_EPERM)                                              || \
     ((e) == XPR_RCODE_CONVERT(ERROR_NOT_OWNER)))
#define XPR_RCODE_IS_ENOENT(e)                                                \
    (((e) == XPR_RCODE_ENOENT)                                             || \
     ((e) == XPR_RCODE_CONVERT(ERROR_FILE_NOT_FOUND))                      || \
     ((e) == XPR_RCODE_CONVERT(ERROR_PATH_NOT_FOUND))                      || \
     ((e) == XPR_RCODE_CONVERT(ERROR_OPEN_FAILED))                         || \
     ((e) == XPR_RCODE_CONVERT(ERROR_NO_MORE_FILES)))
#define XPR_RCODE_IS_ESRCH(e)                ((e) == XPR_RCODE_ESRCH)
#define XPR_RCODE_IS_EINTR(e)                                                 \
    (((e) == XPR_RCODE_EINTR)                                              || \
     ((e) == XPR_RCODE_CONVERT(WSAEINTR)))
#define XPR_RCODE_IS_EIO(e)                  ((e) == XPR_RCODE_EIO)
#define XPR_RCODE_IS_ENXIO(e)                ((e) == XPR_RCODE_ENXIO)
#define XPR_RCODE_IS_E2BIG(e)                ((e) == XPR_RCODE_E2BIG)
#define XPR_RCODE_IS_ENOEXEC(e)              ((e) == XPR_RCODE_ENOEXEC)
#define XPR_RCODE_IS_EBADF(e)                                                 \
    (((e) == XPR_RCODE_EBADF)                                              || \
     ((e) == XPR_RCODE_CONVERT(ERROR_INVALID_HANDLE))                      || \
     ((e) == XPR_RCODE_CONVERT(ERROR_INVALID_TARGET_HANDLE)))
#define XPR_RCODE_IS_ECHILD(e)                                                \
    (((e) == XPR_RCODE_ECHILD)                                             || \
     ((e) == XPR_RCODE_CONVERT(ERROR_WAIT_NO_CHILDREN)))
#define XPR_RCODE_IS_EAGAIN(e)                                                \
    (((e) == XPR_RCODE_EAGAIN)                                             || \
     ((e) == XPR_RCODE_CONVERT(ERROR_NO_DATA))                             || \
     ((e) == XPR_RCODE_CONVERT(ERROR_NO_PROC_SLOTS))                       || \
     ((e) == XPR_RCODE_CONVERT(ERROR_NESTING_NOT_ALLOWED))                 || \
     ((e) == XPR_RCODE_CONVERT(ERROR_MAX_THRDS_REACHED))                   || \
     ((e) == XPR_RCODE_CONVERT(ERROR_LOCK_VIOLATION)))
#define XPR_RCODE_IS_ENOMEM(e)                                                \
    (((e) == XPR_RCODE_ENOMEM)                                             || \
     ((e) == XPR_RCODE_CONVERT(ERROR_ARENA_TRASHED))                       || \
     ((e) == XPR_RCODE_CONVERT(ERROR_NOT_ENOUGH_MEMORY))                   || \
     ((e) == XPR_RCODE_CONVERT(ERROR_INVALID_BLOCK))                       || \
     ((e) == XPR_RCODE_CONVERT(ERROR_NOT_ENOUGH_QUOTA))                    || \
     ((e) == XPR_RCODE_CONVERT(ERROR_OUTOFMEMORY)))
#define XPR_RCODE_IS_EACCES(e)                                                \
    (((e) == XPR_RCODE_EACCES)                                             || \
     ((e) == XPR_RCODE_CONVERT(ERROR_ACCESS_DENIED))                       || \
     ((e) == XPR_RCODE_CONVERT(ERROR_CANNOT_MAKE))                         || \
     ((e) == XPR_RCODE_CONVERT(ERROR_CURRENT_DIRECTORY))                   || \
     ((e) == XPR_RCODE_CONVERT(ERROR_DRIVE_LOCKED))                        || \
     ((e) == XPR_RCODE_CONVERT(ERROR_FAIL_I24))                            || \
     ((e) == XPR_RCODE_CONVERT(ERROR_LOCK_VIOLATION))                      || \
     ((e) == XPR_RCODE_CONVERT(ERROR_LOCK_FAILED))                         || \
     ((e) == XPR_RCODE_CONVERT(ERROR_NOT_LOCKED))                          || \
     ((e) == XPR_RCODE_CONVERT(ERROR_NETWORK_ACCESS_DENIED))               || \
     ((e) == XPR_RCODE_CONVERT(ERROR_SHARING_VIOLATION))                   || \
     ((e) == XPR_RCODE_CONVERT(WSAEACCES)))
#define XPR_RCODE_IS_EFAULT(e)                                                \
    (((e) == XPR_RCODE_EFAULT)                                             || \
     ((e) == XPR_RCODE_CONVERT(WSAEFAULT)))
#define XPR_RCODE_IS_EBUSY(e)                ((e) == XPR_RCODE_EBUSY)
#define XPR_RCODE_IS_EEXIST(e)                                                \
    (((e) == XPR_RCODE_EEXIST)                                             || \
     ((e) == XPR_RCODE_CONVERT(ERROR_FILE_EXISTS))                         || \
     ((e) == XPR_RCODE_CONVERT(ERROR_ALREADY_EXISTS)))
#define XPR_RCODE_IS_EXDEV(e)                                                 \
    (((e) == XPR_RCODE_EXDEV)                                              || \
     ((e) == XPR_RCODE_CONVERT(ERROR_NOT_SAME_DEVICE)))
#define XPR_RCODE_IS_ENODEV(e)               ((e) == XPR_RCODE_ENODEV)
#define XPR_RCODE_IS_ENOTDIR(e)                                               \
    (((e) == XPR_RCODE_ENOTDIR)                                            || \
     ((e) == XPR_RCODE_CONVERT(ERROR_PATH_NOT_FOUND))                      || \
     ((e) == XPR_RCODE_CONVERT(ERROR_BAD_NETPATH))                         || \
     ((e) == XPR_RCODE_CONVERT(ERROR_BAD_NET_NAME))                        || \
     ((e) == XPR_RCODE_CONVERT(ERROR_BAD_PATHNAME))                        || \
     ((e) == XPR_RCODE_CONVERT(ERROR_INVALID_DRIVE))                       || \
     ((e) == XPR_RCODE_CONVERT(ERROR_DIRECTORY)))
#define XPR_RCODE_IS_EISDIR(e)               ((e) == XPR_RCODE_EISDIR)
#define XPR_RCODE_IS_EINVAL(e)                                                \
    (((e) == XPR_RCODE_EINVAL)                                             || \
     ((e) == XPR_RCODE_CONVERT(ERROR_INVALID_ACCESS))                      || \
     ((e) == XPR_RCODE_CONVERT(ERROR_INVALID_DATA))                        || \
     ((e) == XPR_RCODE_CONVERT(ERROR_INVALID_FUNCTION))                    || \
     ((e) == XPR_RCODE_CONVERT(ERROR_INVALID_HANDLE))                      || \
     ((e) == XPR_RCODE_CONVERT(ERROR_INVALID_PARAMETER))                   || \
     ((e) == XPR_RCODE_CONVERT(ERROR_NEGATIVE_SEEK))                       || \
     ((e) == XPR_RCODE_CONVERT(ERROR_PATH_NOT_FOUND))                      || \
     ((e) == XPR_RCODE_CONVERT(ERROR_EXE_MACHINE_TYPE_MISMATCH))           || \
     ((e) == XPR_RCODE_CONVERT(ERROR_INVALID_DLL))                         || \
     ((e) == XPR_RCODE_CONVERT(ERROR_INVALID_MODULETYPE))                  || \
     ((e) == XPR_RCODE_CONVERT(ERROR_BAD_EXE_FORMAT))                      || \
     ((e) == XPR_RCODE_CONVERT(ERROR_INVALID_EXE_SIGNATURE))               || \
     ((e) == XPR_RCODE_CONVERT(ERROR_FILE_CORRUPT))                        || \
     ((e) == XPR_RCODE_CONVERT(ERROR_BAD_FORMAT))                          || \
     ((e) == XPR_RCODE_CONVERT(WSAEINVAL)))
   /*((e) == EFTYPE)*/
#define XPR_RCODE_IS_ENFILE(e)               ((e) == XPR_RCODE_ENFILE)
#define XPR_RCODE_IS_EMFILE(e)                                                \
    (((e) == XPR_RCODE_EMFILE)                                             || \
     ((e) == XPR_RCODE_CONVERT(ERROR_TOO_MANY_OPEN_FILES))                 || \
     ((e) == XPR_RCODE_CONVERT(WSAEMFILE)))
#define XPR_RCODE_IS_ENOTTY(e)               ((e) == XPR_RCODE_ENOTTY)
#define XPR_RCODE_IS_EFBIG(e)                ((e) == XPR_RCODE_EFBIG)
#define XPR_RCODE_IS_ENOSPC(e)                                                \
    (((e) == XPR_RCODE_ENOSPC)                                             || \
     ((e) == XPR_RCODE_CONVERT(ERROR_DISK_FULL))                           || \
     ((e) == XPR_RCODE_CONVERT(WSAEDQUOT)))
#define XPR_RCODE_IS_ESPIPE(e)                                                \
    (((e) == XPR_RCODE_ESPIPE)                                             || \
     ((e) == XPR_RCODE_CONVERT(ERROR_SEEK_ON_DEVICE))                      || \
     ((e) == XPR_RCODE_CONVERT(ERROR_NEGATIVE_SEEK)))
#define XPR_RCODE_IS_EROFS(e)                ((e) == XPR_RCODE_EROFS)
#define XPR_RCODE_IS_EMLINK(e)               ((e) == XPR_RCODE_EMLINK)
#define XPR_RCODE_IS_EPIPE(e)                                                 \
    (((e) == XPR_RCODE_EPIPE)                                              || \
     ((e) == XPR_RCODE_CONVERT(ERROR_BROKEN_PIPE)))
#define XPR_RCODE_IS_EDOM(e)                 ((e) == XPR_RCODE_EDOM)
#define XPR_RCODE_IS_ERANGE(e)               ((e) == XPR_RCODE_ERANGE)
#define XPR_RCODE_IS_EDEADLK(e)                                               \
    (((e) == XPR_RCODE_EDEADLK)                                            || \
     ((e) == XPR_RCODE_CONVERT(ERROR_POSSIBLE_DEADLOCK)))
#define XPR_RCODE_IS_ENAMETOOLONG(e)                                          \
    (((e) == XPR_RCODE_ENAMETOOLONG)                                       || \
     ((e) == XPR_RCODE_CONVERT(ERROR_FILENAME_EXCED_RANGE))                || \
     ((e) == XPR_RCODE_CONVERT(WSAENAMETOOLONG)))
#define XPR_RCODE_IS_ENOLCK(e)               ((e) == XPR_RCODE_ENOLCK)
#define XPR_RCODE_IS_ENOSYS(e)               ((e) == XPR_RCODE_ENOSYS)
#define XPR_RCODE_IS_ENOTEMPTY(e)                                             \
    (((e) == XPR_RCODE_ENOTEMPTY)                                          || \
     ((e) == XPR_RCODE_CONVERT(ERROR_DIR_NOT_EMPTY)))
#define XPR_RCODE_IS_EILSEQ(e)               ((e) == XPR_RCODE_EILSEQ)
#define XPR_RCODE_IS_EADDRINUSE(e)                                            \
    (((e) == XPR_RCODE_EADDRINUSE)                                         || \
     ((e) == XPR_RCODE_CONVERT(WSAEADDRINUSE)))
#define XPR_RCODE_IS_EADDRNOTAVAIL(e)                                         \
    (((e) == XPR_RCODE_EADDRNOTAVAIL)                                      || \
     ((e) == XPR_RCODE_CONVERT(WSAEADDRNOTAVAIL)))
#define XPR_RCODE_IS_EAFNOSUPPORT(e)                                          \
    (((e) == XPR_RCODE_EAFNOSUPPORT)                                       || \
     ((e) == XPR_RCODE_CONVERT(WSAESOCKTNOSUPPORT))                        || \
     ((e) == XPR_RCODE_CONVERT(WSAEAFNOSUPPORT)))
#define XPR_RCODE_IS_EALREADY(e)                                              \
    (((e) == XPR_RCODE_EALREADY)                                           || \
     ((e) == XPR_RCODE_CONVERT(WSAEALREADY)))
#define XPR_RCODE_IS_EBADMSG(e)              ((e) == XPR_RCODE_EBADMSG)
#define XPR_RCODE_IS_ECANCELED(e)            ((e) == XPR_RCODE_ECANCELED)
#define XPR_RCODE_IS_ECONNABORTED(e)                                          \
    (((e) == XPR_RCODE_ECONNABORTED)                                       || \
     ((e) == XPR_RCODE_CONVERT(WSAECONNABORTED)))
#define XPR_RCODE_IS_ECONNREFUSED(e)                                          \
    (((e) == XPR_RCODE_ECONNREFUSED)                                       || \
     ((e) == XPR_RCODE_CONVERT(WSAECONNREFUSED)))
#define XPR_RCODE_IS_ECONNRESET(e)                                            \
    (((e) == XPR_RCODE_ECONNRESET)                                         || \
     ((e) == XPR_RCODE_CONVERT(ERROR_NETNAME_DELETED))                     || \
     ((e) == XPR_RCODE_CONVERT(WSAECONNRESET)))
#define XPR_RCODE_IS_EDESTADDRREQ(e)                                          \
    (((e) == XPR_RCODE_EDESTADDRREQ)                                       || \
     ((e) == XPR_RCODE_CONVERT(WSAEDESTADDRREQ)))
#define XPR_RCODE_IS_EHOSTUNREACH(e)                                          \
    (((e) == XPR_RCODE_EHOSTUNREACH)                                       || \
     ((e) == XPR_RCODE_CONVERT(WSAEHOSTUNREACH)))
#define XPR_RCODE_IS_EIDRM(e)                ((e) == XPR_RCODE_EIDRM)
#define XPR_RCODE_IS_EINPROGRESS(e)                                           \
    (((e) == XPR_RCODE_EINPROGRESS)                                        || \
     ((e) == XPR_RCODE_CONVERT(WSAEINPROGRESS)))
#define XPR_RCODE_IS_EISCONN(e)                                               \
    (((e) == XPR_RCODE_EISCONN)                                            || \
     ((e) == XPR_RCODE_CONVERT(WSAEISCONN)))
#define XPR_RCODE_IS_ELOOP(e)                                                 \
    (((e) == XPR_RCODE_ELOOP)                                              || \
     ((e) == XPR_RCODE_CONVERT(WSAELOOP)))
#define XPR_RCODE_IS_EMSGSIZE(e)                                              \
    (((e) == XPR_RCODE_EMSGSIZE)                                           || \
     ((e) == XPR_RCODE_CONVERT(WSAEMSGSIZE)))
#define XPR_RCODE_IS_ENETDOWN(e)                                              \
    (((e) == XPR_RCODE_ENETDOWN)                                           || \
     ((e) == XPR_RCODE_CONVERT(WSAENETDOWN)))
#define XPR_RCODE_IS_ENETRESET(e)                                             \
    (((e) == XPR_RCODE_ENETRESET)                                          || \
     ((e) == XPR_RCODE_CONVERT(WSAENETRESET)))
#define XPR_RCODE_IS_ENETUNREACH(e)                                           \
    (((e) == XPR_RCODE_ENETUNREACH)                                        || \
     ((e) == XPR_RCODE_CONVERT(WSAENETUNREACH)))
#define XPR_RCODE_IS_ENOBUFS(e)                                               \
    (((e) == XPR_RCODE_ENOBUFS)                                            || \
     ((e) == XPR_RCODE_CONVERT(WSAENOBUFS)))
#define XPR_RCODE_IS_ENODATA(e)              ((e) == XPR_RCODE_ENODATA)
#define XPR_RCODE_IS_ENOLINK(e)              ((e) == XPR_RCODE_ENOLINK)
#define XPR_RCODE_IS_ENOMSG(e)               ((e) == XPR_RCODE_ENOMSG)
#define XPR_RCODE_IS_ENOPROTOOPT(e)                                           \
    (((e) == XPR_RCODE_ENOPROTOOPT)                                        || \
     ((e) == XPR_RCODE_CONVERT(WSAENOPROTOOPT)))
#define XPR_RCODE_IS_ENOSR(e)                ((e) == XPR_RCODE_ENOSR)
#define XPR_RCODE_IS_ENOSTR(e)               ((e) == XPR_RCODE_ENOSTR)
#define XPR_RCODE_IS_ENOTCONN(e)                                              \
    (((e) == XPR_RCODE_ENOTCONN)                                           || \
     ((e) == XPR_RCODE_CONVERT(WSAENOTCONN)))
#define XPR_RCODE_IS_ENOTRECOVERABLE(e)      ((e) == XPR_RCODE_ENOTRECOVERABLE)
#define XPR_RCODE_IS_ENOTSOCK(e)                                              \
    (((e) == XPR_RCODE_ENOTSOCK)                                           || \
     ((e) == XPR_RCODE_CONVERT(WSAENOTSOCK)))
#define XPR_RCODE_IS_ENOTSUP(e)              ((e) == XPR_RCODE_ENOTSUP)
#define XPR_RCODE_IS_EOPNOTSUPP(e)                                            \
    (((e) == XPR_RCODE_EOPNOTSUPP)                                         || \
     ((e) == XPR_RCODE_CONVERT(WSAEOPNOTSUPP)))
#define XPR_RCODE_IS_EOVERFLOW(e)            ((e) == XPR_RCODE_EOVERFLOW)
#define XPR_RCODE_IS_EOWNERDEAD(e)           ((e) == XPR_RCODE_EOWNERDEAD)
#define XPR_RCODE_IS_EPROTO(e)               ((e) == XPR_RCODE_EPROTO)
#define XPR_RCODE_IS_EPROTONOSUPPORT(e)                                       \
    (((e) == XPR_RCODE_EPROTONOSUPPORT)                                    || \
     ((e) == XPR_RCODE_CONVERT(WSAEPROTONOSUPPORT)))
#define XPR_RCODE_IS_EPROTOTYPE(e)                                            \
    (((e) == XPR_RCODE_EPROTOTYPE)                                         || \
     ((e) == XPR_RCODE_CONVERT(WSAEPROTOTYPE)))
#define XPR_RCODE_IS_ETIME(e)                ((e) == XPR_RCODE_ETIME)
#define XPR_RCODE_IS_ETIMEDOUT(e)                                             \
    (((e) == XPR_RCODE_ETIMEDOUT)                                          || \
     ((e) == XPR_RCODE_CONVERT(WAIT_TIMEOUT))                              || \
     ((e) == XPR_RCODE_CONVERT(WSAETIMEDOUT)))
#define XPR_RCODE_IS_ETXTBSY(e)              ((e) == XPR_RCODE_ETXTBSY)
#define XPR_RCODE_IS_EWOULDBLOCK(e)                                           \
    (((e) == XPR_RCODE_EWOULDBLOCK)                                        || \
     ((e) == XPR_RCODE_CONVERT(WSAEWOULDBLOCK)))

#else // not, XPR_CFG_OS_WINDOWS

#define XPR_RCODE_IS_SUCCESS(e)              ((e) == XPR_RCODE_SUCCESS)
#define XPR_RCODE_IS_ERROR(e)                ((e) != XPR_RCODE_SUCCESS)
#define XPR_RCODE_IS_EPERM(e)                ((e) == XPR_RCODE_EPERM)
#define XPR_RCODE_IS_ENOENT(e)               ((e) == XPR_RCODE_ENOENT)
#define XPR_RCODE_IS_ESRCH(e)                ((e) == XPR_RCODE_ESRCH)
#define XPR_RCODE_IS_EINTR(e)                ((e) == XPR_RCODE_EINTR)
#define XPR_RCODE_IS_EIO(e)                  ((e) == XPR_RCODE_EIO)
#define XPR_RCODE_IS_ENXIO(e)                ((e) == XPR_RCODE_ENXIO)
#define XPR_RCODE_IS_E2BIG(e)                ((e) == XPR_RCODE_E2BIG)
#define XPR_RCODE_IS_ENOEXEC(e)              ((e) == XPR_RCODE_ENOEXEC)
#define XPR_RCODE_IS_EBADF(e)                ((e) == XPR_RCODE_EBADF)
#define XPR_RCODE_IS_ECHILD(e)               ((e) == XPR_RCODE_ECHILD)
#define XPR_RCODE_IS_EAGAIN(e)               ((e) == XPR_RCODE_EAGAIN)
#define XPR_RCODE_IS_ENOMEM(e)               ((e) == XPR_RCODE_ENOMEM)
#define XPR_RCODE_IS_EACCES(e)               ((e) == XPR_RCODE_EACCES)
#define XPR_RCODE_IS_EFAULT(e)               ((e) == XPR_RCODE_EFAULT)
#define XPR_RCODE_IS_EBUSY(e)                ((e) == XPR_RCODE_EBUSY)
#define XPR_RCODE_IS_EEXIST(e)               ((e) == XPR_RCODE_EEXIST)
#define XPR_RCODE_IS_EXDEV(e)                ((e) == XPR_RCODE_EXDEV)
#define XPR_RCODE_IS_ENODEV(e)               ((e) == XPR_RCODE_ENODEV)
#define XPR_RCODE_IS_ENOTDIR(e)              ((e) == XPR_RCODE_ENOTDIR)
#define XPR_RCODE_IS_EISDIR(e)               ((e) == XPR_RCODE_EISDIR)
#define XPR_RCODE_IS_EINVAL(e)               ((e) == XPR_RCODE_EINVAL)
#define XPR_RCODE_IS_ENFILE(e)               ((e) == XPR_RCODE_ENFILE)
#define XPR_RCODE_IS_EMFILE(e)               ((e) == XPR_RCODE_EMFILE)
#define XPR_RCODE_IS_ENOTTY(e)               ((e) == XPR_RCODE_ENOTTY)
#define XPR_RCODE_IS_EFBIG(e)                ((e) == XPR_RCODE_EFBIG)
#define XPR_RCODE_IS_ENOSPC(e)               ((e) == XPR_RCODE_ENOSPC)
#define XPR_RCODE_IS_ESPIPE(e)               ((e) == XPR_RCODE_ESPIPE)
#define XPR_RCODE_IS_EROFS(e)                ((e) == XPR_RCODE_EROFS)
#define XPR_RCODE_IS_EMLINK(e)               ((e) == XPR_RCODE_EMLINK)
#define XPR_RCODE_IS_EPIPE(e)                ((e) == XPR_RCODE_EPIPE)
#define XPR_RCODE_IS_EDOM(e)                 ((e) == XPR_RCODE_EDOM)
#define XPR_RCODE_IS_ERANGE(e)               ((e) == XPR_RCODE_ERANGE)
#define XPR_RCODE_IS_EDEADLK(e)              ((e) == XPR_RCODE_EDEADLK)
#define XPR_RCODE_IS_ENAMETOOLONG(e)         ((e) == XPR_RCODE_ENAMETOOLONG)
#define XPR_RCODE_IS_ENOLCK(e)               ((e) == XPR_RCODE_ENOLCK)
#define XPR_RCODE_IS_ENOSYS(e)               ((e) == XPR_RCODE_ENOSYS)
#define XPR_RCODE_IS_ENOTEMPTY(e)            ((e) == XPR_RCODE_ENOTEMPTY)
#define XPR_RCODE_IS_EILSEQ(e)               ((e) == XPR_RCODE_EILSEQ)
#define XPR_RCODE_IS_EADDRINUSE(e)           ((e) == XPR_RCODE_EADDRINUSE)
#define XPR_RCODE_IS_EADDRNOTAVAIL(e)        ((e) == XPR_RCODE_EADDRNOTAVAIL)
#define XPR_RCODE_IS_EAFNOSUPPORT(e)         ((e) == XPR_RCODE_EAFNOSUPPORT)
#define XPR_RCODE_IS_EALREADY(e)             ((e) == XPR_RCODE_EALREADY)
#define XPR_RCODE_IS_EBADMSG(e)              ((e) == XPR_RCODE_EBADMSG)
#define XPR_RCODE_IS_ECANCELED(e)            ((e) == XPR_RCODE_ECANCELED)
#define XPR_RCODE_IS_ECONNABORTED(e)         ((e) == XPR_RCODE_ECONNABORTED)
#define XPR_RCODE_IS_ECONNREFUSED(e)         ((e) == XPR_RCODE_ECONNREFUSED)
#define XPR_RCODE_IS_ECONNRESET(e)           ((e) == XPR_RCODE_ECONNRESET)
#define XPR_RCODE_IS_EDESTADDRREQ(e)         ((e) == XPR_RCODE_EDESTADDRREQ)
#define XPR_RCODE_IS_EHOSTUNREACH(e)         ((e) == XPR_RCODE_EHOSTUNREACH)
#define XPR_RCODE_IS_EIDRM(e)                ((e) == XPR_RCODE_EIDRM)
#define XPR_RCODE_IS_EINPROGRESS(e)          ((e) == XPR_RCODE_EINPROGRESS)
#define XPR_RCODE_IS_EISCONN(e)              ((e) == XPR_RCODE_EISCONN)
#define XPR_RCODE_IS_ELOOP(e)                ((e) == XPR_RCODE_ELOOP)
#define XPR_RCODE_IS_EMSGSIZE(e)             ((e) == XPR_RCODE_EMSGSIZE)
#define XPR_RCODE_IS_ENETDOWN(e)             ((e) == XPR_RCODE_ENETDOWN)
#define XPR_RCODE_IS_ENETRESET(e)            ((e) == XPR_RCODE_ENETRESET)
#define XPR_RCODE_IS_ENETUNREACH(e)          ((e) == XPR_RCODE_ENETUNREACH)
#define XPR_RCODE_IS_ENOBUFS(e)              ((e) == XPR_RCODE_ENOBUFS)
#define XPR_RCODE_IS_ENODATA(e)              ((e) == XPR_RCODE_ENODATA)
#define XPR_RCODE_IS_ENOLINK(e)              ((e) == XPR_RCODE_ENOLINK)
#define XPR_RCODE_IS_ENOMSG(e)               ((e) == XPR_RCODE_ENOMSG)
#define XPR_RCODE_IS_ENOPROTOOPT(e)          ((e) == XPR_RCODE_ENOPROTOOPT)
#define XPR_RCODE_IS_ENOSR(e)                ((e) == XPR_RCODE_ENOSR)
#define XPR_RCODE_IS_ENOSTR(e)               ((e) == XPR_RCODE_ENOSTR)
#define XPR_RCODE_IS_ENOTCONN(e)             ((e) == XPR_RCODE_ENOTCONN)
#define XPR_RCODE_IS_ENOTRECOVERABLE(e)      ((e) == XPR_RCODE_ENOTRECOVERABLE)
#define XPR_RCODE_IS_ENOTSOCK(e)             ((e) == XPR_RCODE_ENOTSOCK)
#define XPR_RCODE_IS_ENOTSUP(e)              ((e) == XPR_RCODE_ENOTSUP)
#define XPR_RCODE_IS_EOPNOTSUPP(e)           ((e) == XPR_RCODE_EOPNOTSUPP)
#define XPR_RCODE_IS_EOVERFLOW(e)            ((e) == XPR_RCODE_EOVERFLOW)
#define XPR_RCODE_IS_EOWNERDEAD(e)           ((e) == XPR_RCODE_EOWNERDEAD)
#define XPR_RCODE_IS_EPROTO(e)               ((e) == XPR_RCODE_EPROTO)
#define XPR_RCODE_IS_EPROTONOSUPPORT(e)      ((e) == XPR_RCODE_EPROTONOSUPPORT)
#define XPR_RCODE_IS_EPROTOTYPE(e)           ((e) == XPR_RCODE_EPROTOTYPE)
#define XPR_RCODE_IS_ETIME(e)                ((e) == XPR_RCODE_ETIME)
#define XPR_RCODE_IS_ETIMEDOUT(e)            ((e) == XPR_RCODE_ETIMEDOUT)
#define XPR_RCODE_IS_ETXTBSY(e)              ((e) == XPR_RCODE_ETXTBSY)
#define XPR_RCODE_IS_EWOULDBLOCK(e)          ((e) == XPR_RCODE_EWOULDBLOCK)

#endif // XPR_CFG_OS_WINDOWS

//
// test not error code
//
#define XPR_RCODE_IS_NOT_SUCCESS(e)          (!XPR_RCODE_IS_SUCCESS(e))
#define XPR_RCODE_IS_NOT_EPERM(e)            (!XPR_RCODE_IS_EPERM(e))
#define XPR_RCODE_IS_NOT_ENOENT(e)           (!XPR_RCODE_IS_ENOENT(e))
#define XPR_RCODE_IS_NOT_ESRCH(e)            (!XPR_RCODE_IS_ESRCH(e))
#define XPR_RCODE_IS_NOT_EINTR(e)            (!XPR_RCODE_IS_EINTR(e))
#define XPR_RCODE_IS_NOT_EIO(e)              (!XPR_RCODE_IS_EIO(e))
#define XPR_RCODE_IS_NOT_ENXIO(e)            (!XPR_RCODE_IS_ENXIO(e))
#define XPR_RCODE_IS_NOT_E2BIG(e)            (!XPR_RCODE_IS_E2BIG(e))
#define XPR_RCODE_IS_NOT_ENOEXEC(e)          (!XPR_RCODE_IS_ENOEXEC(e))
#define XPR_RCODE_IS_NOT_EBADF(e)            (!XPR_RCODE_IS_EBADF(e))
#define XPR_RCODE_IS_NOT_ECHILD(e)           (!XPR_RCODE_IS_ECHILD(e))
#define XPR_RCODE_IS_NOT_EAGAIN(e)           (!XPR_RCODE_IS_EAGAIN(e))
#define XPR_RCODE_IS_NOT_ENOMEM(e)           (!XPR_RCODE_IS_ENOMEM(e))
#define XPR_RCODE_IS_NOT_EACCES(e)           (!XPR_RCODE_IS_EACCES(e))
#define XPR_RCODE_IS_NOT_EFAULT(e)           (!XPR_RCODE_IS_EFAULT(e))
#define XPR_RCODE_IS_NOT_EBUSY(e)            (!XPR_RCODE_IS_EBUSY(e))
#define XPR_RCODE_IS_NOT_EEXIST(e)           (!XPR_RCODE_IS_EEXIST(e))
#define XPR_RCODE_IS_NOT_EXDEV(e)            (!XPR_RCODE_IS_EXDEV(e))
#define XPR_RCODE_IS_NOT_ENODEV(e)           (!XPR_RCODE_IS_ENODEV(e))
#define XPR_RCODE_IS_NOT_ENOTDIR(e)          (!XPR_RCODE_IS_ENOTDIR(e))
#define XPR_RCODE_IS_NOT_EISDIR(e)           (!XPR_RCODE_IS_EISDIR(e))
#define XPR_RCODE_IS_NOT_EINVAL(e)           (!XPR_RCODE_IS_EINVAL(e))
#define XPR_RCODE_IS_NOT_ENFILE(e)           (!XPR_RCODE_IS_ENFILE(e))
#define XPR_RCODE_IS_NOT_EMFILE(e)           (!XPR_RCODE_IS_EMFILE(e))
#define XPR_RCODE_IS_NOT_ENOTTY(e)           (!XPR_RCODE_IS_ENOTTY(e))
#define XPR_RCODE_IS_NOT_EFBIG(e)            (!XPR_RCODE_IS_EFBIG(e))
#define XPR_RCODE_IS_NOT_ENOSPC(e)           (!XPR_RCODE_IS_ENOSPC(e))
#define XPR_RCODE_IS_NOT_ESPIPE(e)           (!XPR_RCODE_IS_ESPIPE(e))
#define XPR_RCODE_IS_NOT_EROFS(e)            (!XPR_RCODE_IS_EROFS(e))
#define XPR_RCODE_IS_NOT_EMLINK(e)           (!XPR_RCODE_IS_EMLINK(e))
#define XPR_RCODE_IS_NOT_EPIPE(e)            (!XPR_RCODE_IS_EPIPE(e))
#define XPR_RCODE_IS_NOT_EDOM(e)             (!XPR_RCODE_IS_EDOM(e))
#define XPR_RCODE_IS_NOT_ERANGE(e)           (!XPR_RCODE_IS_ERANGE(e))
#define XPR_RCODE_IS_NOT_EDEADLK(e)          (!XPR_RCODE_IS_EDEADLK(e))
#define XPR_RCODE_IS_NOT_ENAMETOOLONG(e)     (!XPR_RCODE_IS_ENAMETOOLONG(e))
#define XPR_RCODE_IS_NOT_ENOLCK(e)           (!XPR_RCODE_IS_ENOLCK(e))
#define XPR_RCODE_IS_NOT_ENOSYS(e)           (!XPR_RCODE_IS_ENOSYS(e))
#define XPR_RCODE_IS_NOT_ENOTEMPTY(e)        (!XPR_RCODE_IS_ENOTEMPTY(e))
#define XPR_RCODE_IS_NOT_EILSEQ(e)           (!XPR_RCODE_IS_EILSEQ(e))
#define XPR_RCODE_IS_NOT_EADDRINUSE(e)       (!XPR_RCODE_IS_EADDRINUSE(e))
#define XPR_RCODE_IS_NOT_EADDRNOTAVAIL(e)    (!XPR_RCODE_IS_EADDRNOTAVAIL(e))
#define XPR_RCODE_IS_NOT_EAFNOSUPPORT(e)     (!XPR_RCODE_IS_EAFNOSUPPORT(e))
#define XPR_RCODE_IS_NOT_EALREADY(e)         (!XPR_RCODE_IS_EALREADY(e))
#define XPR_RCODE_IS_NOT_EBADMSG(e)          (!XPR_RCODE_IS_EBADMSG(e))
#define XPR_RCODE_IS_NOT_ECANCELED(e)        (!XPR_RCODE_IS_ECANCELED(e))
#define XPR_RCODE_IS_NOT_ECONNABORTED(e)     (!XPR_RCODE_IS_ECONNABORTED(e))
#define XPR_RCODE_IS_NOT_ECONNREFUSED(e)     (!XPR_RCODE_IS_ECONNREFUSED(e))
#define XPR_RCODE_IS_NOT_ECONNRESET(e)       (!XPR_RCODE_IS_ECONNRESET(e))
#define XPR_RCODE_IS_NOT_EDESTADDRREQ(e)     (!XPR_RCODE_IS_EDESTADDRREQ(e))
#define XPR_RCODE_IS_NOT_EHOSTUNREACH(e)     (!XPR_RCODE_IS_EHOSTUNREACH(e))
#define XPR_RCODE_IS_NOT_EIDRM(e)            (!XPR_RCODE_IS_EIDRM(e))
#define XPR_RCODE_IS_NOT_EINPROGRESS(e)      (!XPR_RCODE_IS_EINPROGRESS(e))
#define XPR_RCODE_IS_NOT_EISCONN(e)          (!XPR_RCODE_IS_EISCONN(e))
#define XPR_RCODE_IS_NOT_ELOOP(e)            (!XPR_RCODE_IS_ELOOP(e))
#define XPR_RCODE_IS_NOT_EMSGSIZE(e)         (!XPR_RCODE_IS_EMSGSIZE(e))
#define XPR_RCODE_IS_NOT_ENETDOWN(e)         (!XPR_RCODE_IS_ENETDOWN(e))
#define XPR_RCODE_IS_NOT_ENETRESET(e)        (!XPR_RCODE_IS_ENETRESET(e))
#define XPR_RCODE_IS_NOT_ENETUNREACH(e)      (!XPR_RCODE_IS_ENETUNREACH(e))
#define XPR_RCODE_IS_NOT_ENOBUFS(e)          (!XPR_RCODE_IS_ENOBUFS(e))
#define XPR_RCODE_IS_NOT_ENODATA(e)          (!XPR_RCODE_IS_ENODATA(e))
#define XPR_RCODE_IS_NOT_ENOLINK(e)          (!XPR_RCODE_IS_ENOLINK(e))
#define XPR_RCODE_IS_NOT_ENOMSG(e)           (!XPR_RCODE_IS_ENOMSG(e))
#define XPR_RCODE_IS_NOT_ENOPROTOOPT(e)      (!XPR_RCODE_IS_ENOPROTOOPT(e))
#define XPR_RCODE_IS_NOT_ENOSR(e)            (!XPR_RCODE_IS_ENOSR(e))
#define XPR_RCODE_IS_NOT_ENOSTR(e)           (!XPR_RCODE_IS_ENOSTR(e))
#define XPR_RCODE_IS_NOT_ENOTCONN(e)         (!XPR_RCODE_IS_ENOTCONN(e))
#define XPR_RCODE_IS_NOT_ENOTRECOVERABLE(e)  (!XPR_RCODE_IS_ENOTRECOVERABLE(e))
#define XPR_RCODE_IS_NOT_ENOTSOCK(e)         (!XPR_RCODE_IS_ENOTSOCK(e))
#define XPR_RCODE_IS_NOT_ENOTSUP(e)          (!XPR_RCODE_IS_ENOTSUP(e))
#define XPR_RCODE_IS_NOT_EOPNOTSUPP(e)       (!XPR_RCODE_IS_EOPNOTSUPP(e))
#define XPR_RCODE_IS_NOT_EOVERFLOW(e)        (!XPR_RCODE_IS_EOVERFLOW(e))
#define XPR_RCODE_IS_NOT_EOWNERDEAD(e)       (!XPR_RCODE_IS_EOWNERDEAD(e))
#define XPR_RCODE_IS_NOT_EPROTO(e)           (!XPR_RCODE_IS_EPROTO(e))
#define XPR_RCODE_IS_NOT_EPROTONOSUPPORT(e)  (!XPR_RCODE_IS_EPROTONOSUPPORT(e))
#define XPR_RCODE_IS_NOT_EPROTOTYPE(e)       (!XPR_RCODE_IS_EPROTOTYPE(e))
#define XPR_RCODE_IS_NOT_ETIME(e)            (!XPR_RCODE_IS_ETIME(e))
#define XPR_RCODE_IS_NOT_ETIMEDOUT(e)        (!XPR_RCODE_IS_ETIMEDOUT(e))
#define XPR_RCODE_IS_NOT_ETXTBSY(e)          (!XPR_RCODE_IS_ETXTBSY(e))
#define XPR_RCODE_IS_NOT_EWOULDBLOCK(e)      (!XPR_RCODE_IS_EWOULDBLOCK(e))

XPR_DL_API void getRcodeString(xpr_rcode_t aRcode, xpr_char_t *aBuffer, xpr_size_t aSize);
} // namespace xpr

#endif // __XPR_RCODE_H__

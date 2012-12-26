//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_rcode.h"
#include "xpr_cstr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace xpr
{
XPR_DL_API void getRcodeString(xpr_rcode_t aRcode, xpr_char_t *aBuffer, xpr_size_t aSize)
{
    switch (aRcode)
    {
    case XPR_RCODE_SUCCESS:         xpr_strcpy(aBuffer, aSize, "success",                             -1); break;
    case XPR_RCODE_EPERM:           xpr_strcpy(aBuffer, aSize, "permission denied",                   -1); break;
    case XPR_RCODE_ENOENT:          xpr_strcpy(aBuffer, aSize, "not exist",                           -1); break;
    case XPR_RCODE_ESRCH:           xpr_strcpy(aBuffer, aSize, "no such process",                     -1); break;
    case XPR_RCODE_EINTR:           xpr_strcpy(aBuffer, aSize, "operation interrupted",               -1); break;
    case XPR_RCODE_EIO:             xpr_strcpy(aBuffer, aSize, "I/O error",                           -1); break;
    case XPR_RCODE_ENXIO:           xpr_strcpy(aBuffer, aSize, "no such device or address",           -1); break;
    case XPR_RCODE_E2BIG:           xpr_strcpy(aBuffer, aSize, "argument list too long",              -1); break;
    case XPR_RCODE_ENOEXEC:         xpr_strcpy(aBuffer, aSize, "executable format error",             -1); break;
    case XPR_RCODE_EBADF:           xpr_strcpy(aBuffer, aSize, "bad file descriptor",                 -1); break;
    case XPR_RCODE_ECHILD:          xpr_strcpy(aBuffer, aSize, "no child process to wait for",        -1); break;
    case XPR_RCODE_EAGAIN:          xpr_strcpy(aBuffer, aSize, "resource temporarily unavailable",    -1); break;
    case XPR_RCODE_ENOMEM:          xpr_strcpy(aBuffer, aSize, "not enough memory",                   -1); break;
    case XPR_RCODE_EACCES:          xpr_strcpy(aBuffer, aSize, "access denied",                       -1); break;
    case XPR_RCODE_EFAULT:          xpr_strcpy(aBuffer, aSize, "bad address",                         -1); break;
    case XPR_RCODE_EBUSY:           xpr_strcpy(aBuffer, aSize, "device or resource busy",             -1); break;
    case XPR_RCODE_EEXIST:          xpr_strcpy(aBuffer, aSize, "file exists",                         -1); break;
    case XPR_RCODE_EXDEV:           xpr_strcpy(aBuffer, aSize, "cross device link",                   -1); break;
    case XPR_RCODE_ENODEV:          xpr_strcpy(aBuffer, aSize, "no such device",                      -1); break;
    case XPR_RCODE_ENOTDIR:         xpr_strcpy(aBuffer, aSize, "not a directory",                     -1); break;
    case XPR_RCODE_EISDIR:          xpr_strcpy(aBuffer, aSize, "is a directory",                      -1); break;
    case XPR_RCODE_EINVAL:          xpr_strcpy(aBuffer, aSize, "invalid argument",                    -1); break;
    case XPR_RCODE_ENFILE:          xpr_strcpy(aBuffer, aSize, "file table overflow",                 -1); break;
    case XPR_RCODE_EMFILE:          xpr_strcpy(aBuffer, aSize, "too many open files",                 -1); break;
    case XPR_RCODE_ENOTTY:          xpr_strcpy(aBuffer, aSize, "inappropriate I/O control operation", -1); break;
    case XPR_RCODE_EFBIG:           xpr_strcpy(aBuffer, aSize, "file too large",                      -1); break;
    case XPR_RCODE_ENOSPC:          xpr_strcpy(aBuffer, aSize, "not enough space",                    -1); break;
    case XPR_RCODE_ESPIPE:          xpr_strcpy(aBuffer, aSize, "illegal seek",                        -1); break;
    case XPR_RCODE_EROFS:           xpr_strcpy(aBuffer, aSize, "read only file system",               -1); break;
    case XPR_RCODE_EMLINK:          xpr_strcpy(aBuffer, aSize, "too many links",                      -1); break;
    case XPR_RCODE_EPIPE:           xpr_strcpy(aBuffer, aSize, "broken pipe",                         -1); break;
    case XPR_RCODE_EDOM:            xpr_strcpy(aBuffer, aSize, "argument out of domain",              -1); break;
    case XPR_RCODE_ERANGE:          xpr_strcpy(aBuffer, aSize, "range overflow",                      -1); break;
    case XPR_RCODE_EDEADLK:         xpr_strcpy(aBuffer, aSize, "deadlock condition",                  -1); break;
    case XPR_RCODE_ENAMETOOLONG:    xpr_strcpy(aBuffer, aSize, "filename is too long",                -1); break;
    case XPR_RCODE_ENOLCK:          xpr_strcpy(aBuffer, aSize, "no more lock available",              -1); break;
    case XPR_RCODE_ENOSYS:          xpr_strcpy(aBuffer, aSize, "function not implemented",            -1); break;
    case XPR_RCODE_ENOTEMPTY:       xpr_strcpy(aBuffer, aSize, "directory not empty",                 -1); break;
    case XPR_RCODE_EILSEQ:          xpr_strcpy(aBuffer, aSize, "illegal byte sequence",               -1); break;
    case XPR_RCODE_EADDRINUSE:      xpr_strcpy(aBuffer, aSize, "address in use",                      -1); break;
    case XPR_RCODE_EADDRNOTAVAIL:   xpr_strcpy(aBuffer, aSize, "address not available",               -1); break;
    case XPR_RCODE_EAFNOSUPPORT:    xpr_strcpy(aBuffer, aSize, "address family not supported",        -1); break;
    case XPR_RCODE_EALREADY:        xpr_strcpy(aBuffer, aSize, "connection already in progress",      -1); break;
    case XPR_RCODE_EBADMSG:         xpr_strcpy(aBuffer, aSize, "bad message",                         -1); break;
    case XPR_RCODE_ECANCELED:       xpr_strcpy(aBuffer, aSize, "operation canceled",                  -1); break;
    case XPR_RCODE_ECONNABORTED:    xpr_strcpy(aBuffer, aSize, "connection aborted",                  -1); break;
    case XPR_RCODE_ECONNREFUSED:    xpr_strcpy(aBuffer, aSize, "connection confused",                 -1); break;
    case XPR_RCODE_ECONNRESET:      xpr_strcpy(aBuffer, aSize, "connection reset",                    -1); break;
    case XPR_RCODE_EDESTADDRREQ:    xpr_strcpy(aBuffer, aSize, "destination address required",        -1); break;
    case XPR_RCODE_EHOSTUNREACH:    xpr_strcpy(aBuffer, aSize, "host unreachable",                    -1); break;
    case XPR_RCODE_EIDRM:           xpr_strcpy(aBuffer, aSize, "identifier removed",                  -1); break;
    case XPR_RCODE_EINPROGRESS:     xpr_strcpy(aBuffer, aSize, "operation now in progress",           -1); break;
    case XPR_RCODE_EISCONN:         xpr_strcpy(aBuffer, aSize, "already connected",                   -1); break;
    case XPR_RCODE_ELOOP:           xpr_strcpy(aBuffer, aSize, "too many symbolic link levels",       -1); break;
    case XPR_RCODE_EMSGSIZE:        xpr_strcpy(aBuffer, aSize, "message too long",                    -1); break;
    case XPR_RCODE_ENETDOWN:        xpr_strcpy(aBuffer, aSize, "network down",                        -1); break;
    case XPR_RCODE_ENETRESET:       xpr_strcpy(aBuffer, aSize, "network reset",                       -1); break;
    case XPR_RCODE_ENETUNREACH:     xpr_strcpy(aBuffer, aSize, "network unreachable",                 -1); break;
    case XPR_RCODE_ENOBUFS:         xpr_strcpy(aBuffer, aSize, "no buffer space",                     -1); break;
    case XPR_RCODE_ENODATA:         xpr_strcpy(aBuffer, aSize, "no message available",                -1); break;
    case XPR_RCODE_ENOLINK:         xpr_strcpy(aBuffer, aSize, "no link",                             -1); break;
    case XPR_RCODE_ENOMSG:          xpr_strcpy(aBuffer, aSize, "no message",                          -1); break;
    case XPR_RCODE_ENOPROTOOPT:     xpr_strcpy(aBuffer, aSize, "no protocol option",                  -1); break;
    case XPR_RCODE_ENOSR:           xpr_strcpy(aBuffer, aSize, "no stream resources",                 -1); break;
    case XPR_RCODE_ENOSTR:          xpr_strcpy(aBuffer, aSize, "not a stream",                        -1); break;
    case XPR_RCODE_ENOTCONN:        xpr_strcpy(aBuffer, aSize, "not connected",                       -1); break;
    case XPR_RCODE_ENOTRECOVERABLE: xpr_strcpy(aBuffer, aSize, "state not recoverable",               -1); break;
    case XPR_RCODE_ENOTSOCK:        xpr_strcpy(aBuffer, aSize, "not a socket",                        -1); break;
    case XPR_RCODE_ENOTSUP:         xpr_strcpy(aBuffer, aSize, "not supported",                       -1); break;
    case XPR_RCODE_EOPNOTSUPP:      xpr_strcpy(aBuffer, aSize, "operation not supported",             -1); break;
    case XPR_RCODE_EOVERFLOW:       xpr_strcpy(aBuffer, aSize, "value too large",                     -1); break;
    case XPR_RCODE_EOWNERDEAD:      xpr_strcpy(aBuffer, aSize, "owner dead",                          -1); break;
    case XPR_RCODE_EPROTO:          xpr_strcpy(aBuffer, aSize, "protocol error",                      -1); break;
    case XPR_RCODE_EPROTONOSUPPORT: xpr_strcpy(aBuffer, aSize, "protocol not supported",              -1); break;
    case XPR_RCODE_EPROTOTYPE:      xpr_strcpy(aBuffer, aSize, "wrong protocol type",                 -1); break;
    case XPR_RCODE_ETIME:           xpr_strcpy(aBuffer, aSize, "stream timeout",                      -1); break;
    case XPR_RCODE_ETIMEDOUT:       xpr_strcpy(aBuffer, aSize, "timed out",                           -1); break;
    case XPR_RCODE_ETXTBSY:         xpr_strcpy(aBuffer, aSize, "text file busy",                      -1); break;
    case XPR_RCODE_EWOULDBLOCK:     xpr_strcpy(aBuffer, aSize, "operation would block",               -1); break;

    default:
        xpr_snprintf(aBuffer, aSize, "unknown error %d", aRcode);
        break;
    }
}
} // namespace xpr

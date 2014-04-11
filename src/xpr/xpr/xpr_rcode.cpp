//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_rcode.h"
#include "xpr_cstr.h"

namespace xpr
{
XPR_DL_API void getRcodeString(xpr_rcode_t aRcode, xpr::string &aString)
{
    switch (aRcode)
    {
    case XPR_RCODE_SUCCESS:         aString = XPR_STRING_LITERAL("success"                            ); break;
    case XPR_RCODE_EPERM:           aString = XPR_STRING_LITERAL("permission denied"                  ); break;
    case XPR_RCODE_ENOENT:          aString = XPR_STRING_LITERAL("not exist"                          ); break;
    case XPR_RCODE_ESRCH:           aString = XPR_STRING_LITERAL("no such process"                    ); break;
    case XPR_RCODE_EINTR:           aString = XPR_STRING_LITERAL("operation interrupted"              ); break;
    case XPR_RCODE_EIO:             aString = XPR_STRING_LITERAL("I/O error"                          ); break;
    case XPR_RCODE_ENXIO:           aString = XPR_STRING_LITERAL("no such device or address"          ); break;
    case XPR_RCODE_E2BIG:           aString = XPR_STRING_LITERAL("argument list too long"             ); break;
    case XPR_RCODE_ENOEXEC:         aString = XPR_STRING_LITERAL("executable format error"            ); break;
    case XPR_RCODE_EBADF:           aString = XPR_STRING_LITERAL("bad file descriptor"                ); break;
    case XPR_RCODE_ECHILD:          aString = XPR_STRING_LITERAL("no child process to wait for"       ); break;
    case XPR_RCODE_EAGAIN:          aString = XPR_STRING_LITERAL("resource temporarily unavailable"   ); break;
    case XPR_RCODE_ENOMEM:          aString = XPR_STRING_LITERAL("not enough memory"                  ); break;
    case XPR_RCODE_EACCES:          aString = XPR_STRING_LITERAL("access denied"                      ); break;
    case XPR_RCODE_EFAULT:          aString = XPR_STRING_LITERAL("bad address"                        ); break;
    case XPR_RCODE_EBUSY:           aString = XPR_STRING_LITERAL("device or resource busy"            ); break;
    case XPR_RCODE_EEXIST:          aString = XPR_STRING_LITERAL("file exists"                        ); break;
    case XPR_RCODE_EXDEV:           aString = XPR_STRING_LITERAL("cross device link"                  ); break;
    case XPR_RCODE_ENODEV:          aString = XPR_STRING_LITERAL("no such device"                     ); break;
    case XPR_RCODE_ENOTDIR:         aString = XPR_STRING_LITERAL("not a directory"                    ); break;
    case XPR_RCODE_EISDIR:          aString = XPR_STRING_LITERAL("is a directory"                     ); break;
    case XPR_RCODE_EINVAL:          aString = XPR_STRING_LITERAL("invalid argument"                   ); break;
    case XPR_RCODE_ENFILE:          aString = XPR_STRING_LITERAL("file table overflow"                ); break;
    case XPR_RCODE_EMFILE:          aString = XPR_STRING_LITERAL("too many open files"                ); break;
    case XPR_RCODE_ENOTTY:          aString = XPR_STRING_LITERAL("inappropriate I/O control operation"); break;
    case XPR_RCODE_EFBIG:           aString = XPR_STRING_LITERAL("file too large"                     ); break;
    case XPR_RCODE_ENOSPC:          aString = XPR_STRING_LITERAL("not enough space"                   ); break;
    case XPR_RCODE_ESPIPE:          aString = XPR_STRING_LITERAL("illegal seek"                       ); break;
    case XPR_RCODE_EROFS:           aString = XPR_STRING_LITERAL("read only file system"              ); break;
    case XPR_RCODE_EMLINK:          aString = XPR_STRING_LITERAL("too many links"                     ); break;
    case XPR_RCODE_EPIPE:           aString = XPR_STRING_LITERAL("broken pipe"                        ); break;
    case XPR_RCODE_EDOM:            aString = XPR_STRING_LITERAL("argument out of domain"             ); break;
    case XPR_RCODE_ERANGE:          aString = XPR_STRING_LITERAL("range overflow"                     ); break;
    case XPR_RCODE_EDEADLK:         aString = XPR_STRING_LITERAL("deadlock condition"                 ); break;
    case XPR_RCODE_ENAMETOOLONG:    aString = XPR_STRING_LITERAL("filename is too long"               ); break;
    case XPR_RCODE_ENOLCK:          aString = XPR_STRING_LITERAL("no more lock available"             ); break;
    case XPR_RCODE_ENOSYS:          aString = XPR_STRING_LITERAL("function not implemented"           ); break;
    case XPR_RCODE_ENOTEMPTY:       aString = XPR_STRING_LITERAL("directory not empty"                ); break;
    case XPR_RCODE_EILSEQ:          aString = XPR_STRING_LITERAL("illegal byte sequence"              ); break;
    case XPR_RCODE_EADDRINUSE:      aString = XPR_STRING_LITERAL("address in use"                     ); break;
    case XPR_RCODE_EADDRNOTAVAIL:   aString = XPR_STRING_LITERAL("address not available"              ); break;
    case XPR_RCODE_EAFNOSUPPORT:    aString = XPR_STRING_LITERAL("address family not supported"       ); break;
    case XPR_RCODE_EALREADY:        aString = XPR_STRING_LITERAL("connection already in progress"     ); break;
    case XPR_RCODE_EBADMSG:         aString = XPR_STRING_LITERAL("bad message"                        ); break;
    case XPR_RCODE_ECANCELED:       aString = XPR_STRING_LITERAL("operation canceled"                 ); break;
    case XPR_RCODE_ECONNABORTED:    aString = XPR_STRING_LITERAL("connection aborted"                 ); break;
    case XPR_RCODE_ECONNREFUSED:    aString = XPR_STRING_LITERAL("connection confused"                ); break;
    case XPR_RCODE_ECONNRESET:      aString = XPR_STRING_LITERAL("connection reset"                   ); break;
    case XPR_RCODE_EDESTADDRREQ:    aString = XPR_STRING_LITERAL("destination address required"       ); break;
    case XPR_RCODE_EHOSTUNREACH:    aString = XPR_STRING_LITERAL("host unreachable"                   ); break;
    case XPR_RCODE_EIDRM:           aString = XPR_STRING_LITERAL("identifier removed"                 ); break;
    case XPR_RCODE_EINPROGRESS:     aString = XPR_STRING_LITERAL("operation now in progress"          ); break;
    case XPR_RCODE_EISCONN:         aString = XPR_STRING_LITERAL("already connected"                  ); break;
    case XPR_RCODE_ELOOP:           aString = XPR_STRING_LITERAL("too many symbolic link levels"      ); break;
    case XPR_RCODE_EMSGSIZE:        aString = XPR_STRING_LITERAL("message too long"                   ); break;
    case XPR_RCODE_ENETDOWN:        aString = XPR_STRING_LITERAL("network down"                       ); break;
    case XPR_RCODE_ENETRESET:       aString = XPR_STRING_LITERAL("network reset"                      ); break;
    case XPR_RCODE_ENETUNREACH:     aString = XPR_STRING_LITERAL("network unreachable"                ); break;
    case XPR_RCODE_ENOBUFS:         aString = XPR_STRING_LITERAL("no buffer space"                    ); break;
    case XPR_RCODE_ENODATA:         aString = XPR_STRING_LITERAL("no message available"               ); break;
    case XPR_RCODE_ENOLINK:         aString = XPR_STRING_LITERAL("no link"                            ); break;
    case XPR_RCODE_ENOMSG:          aString = XPR_STRING_LITERAL("no message"                         ); break;
    case XPR_RCODE_ENOPROTOOPT:     aString = XPR_STRING_LITERAL("no protocol option"                 ); break;
    case XPR_RCODE_ENOSR:           aString = XPR_STRING_LITERAL("no stream resources"                ); break;
    case XPR_RCODE_ENOSTR:          aString = XPR_STRING_LITERAL("not a stream"                       ); break;
    case XPR_RCODE_ENOTCONN:        aString = XPR_STRING_LITERAL("not connected"                      ); break;
    case XPR_RCODE_ENOTRECOVERABLE: aString = XPR_STRING_LITERAL("state not recoverable"              ); break;
    case XPR_RCODE_ENOTSOCK:        aString = XPR_STRING_LITERAL("not a socket"                       ); break;
    case XPR_RCODE_ENOTSUP:         aString = XPR_STRING_LITERAL("not supported"                      ); break;
    case XPR_RCODE_EOPNOTSUPP:      aString = XPR_STRING_LITERAL("operation not supported"            ); break;
    case XPR_RCODE_EOVERFLOW:       aString = XPR_STRING_LITERAL("value too large"                    ); break;
    case XPR_RCODE_EOWNERDEAD:      aString = XPR_STRING_LITERAL("owner dead"                         ); break;
    case XPR_RCODE_EPROTO:          aString = XPR_STRING_LITERAL("protocol error"                     ); break;
    case XPR_RCODE_EPROTONOSUPPORT: aString = XPR_STRING_LITERAL("protocol not supported"             ); break;
    case XPR_RCODE_EPROTOTYPE:      aString = XPR_STRING_LITERAL("wrong protocol type"                ); break;
    case XPR_RCODE_ETIME:           aString = XPR_STRING_LITERAL("stream timeout"                     ); break;
    case XPR_RCODE_ETIMEDOUT:       aString = XPR_STRING_LITERAL("timed out"                          ); break;
    case XPR_RCODE_ETXTBSY:         aString = XPR_STRING_LITERAL("text file busy"                     ); break;
    case XPR_RCODE_EWOULDBLOCK:     aString = XPR_STRING_LITERAL("operation would block"              ); break;

    default:
        aString.format(XPR_STRING_LITERAL("unknown error %d"), aRcode);
        break;
    }
}
} // namespace xpr

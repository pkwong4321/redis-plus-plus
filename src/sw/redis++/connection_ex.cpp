/**************************************************************************
   Copyright (c) 2017 sewenew

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 *************************************************************************/

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <poll.h>
#endif
#include "sw/redis++/connection.h"

namespace sw {

namespace redis {

Connection::Connection(const ConnectionOptions &opts, Dummy d):
    original::connection::Connection(opts, d)
{

}

Connection::Connection(const ConnectionOptions &opts):
    original::connection::Connection(opts, Dummy{})
{
    this->_ctx = Connector(opts).connect();
    this->_create_time = std::chrono::steady_clock::now();
    this->_last_active = std::chrono::steady_clock::now();

    assert(_ctx && !broken());

    const auto &tls_opts = opts.tls;
    // If not compiled with TLS, TLS is always disabled.
    if (tls::enabled(tls_opts)) {
        _tls_ctx = tls::secure_connection(*_ctx, tls_opts);
    }

    _set_options();
}

void Connection::reconnect()
{
    Connection connection(_opts);

    swap(*this, connection);
}

int Connection::pollBufferRead(int timeout)
{
    auto redisContext = this->_context();

    // ref: hiredis - poll.h, example-poll.c
    pollfd pfd[1];
    pfd[0].fd = redisContext->fd;
    pfd[0].events = POLLIN;
#ifdef _WIN32
    int result = WSAPoll(pfd, 1, timeout);
    if (result == SOCKET_ERROR) {
        int errorCode = WSAGetLastError();
        LPSTR errorBuffer = nullptr;
        size_t n = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&errorBuffer,
            0,
            NULL
            );
        std::string error(errorBuffer, n);
        LocalFree(errorBuffer);

        throw Error(error);
    }
#else
    int result = poll(pfd, 1, timeout);
    if (result == -1) {
        throw Error(strerror(errno));
    }
#endif
    return result;
}

int Connection::pollBufferWrite(int timeout)
{
    auto redisContext = this->_context();

    // ref: hiredis - poll.h, example-poll.c
    pollfd pfd[1];
    pfd[0].fd = redisContext->fd;
    pfd[0].events = POLLOUT;
#ifdef _WIN32
    int result = WSAPoll(pfd, 1, timeout);
    if (result == SOCKET_ERROR) {
        int errorCode = WSAGetLastError();
        LPSTR errorBuffer = nullptr;
        size_t n = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&errorBuffer,
            0,
            NULL
            );
        std::string error(errorBuffer, n);
        LocalFree(errorBuffer);

        throw Error(error);
    }
#else
    int result = poll(pfd, 1, timeout);
    if (result == -1) {
        throw Error(strerror(errno));
    }
#endif
    return result;
}

void Connection::bufferRead()
{
    auto ctx = _context();

    assert(ctx != nullptr);

    if (redisBufferRead(ctx) != REDIS_OK) {
        throw_error(*ctx, "Failed to do bufferRead");
    }

    assert(!broken());
}

void Connection::bufferWrite(int *done)
{
    auto ctx = _context();

    assert(ctx != nullptr);

    if (redisBufferWrite(ctx, done) != REDIS_OK) {
        throw_error(*ctx, "Failed to do bufferRead");
    }

    assert(!broken());
}

ReplyUPtr Connection::recvFromReader(bool handle_error_reply)
{
    auto *ctx = _context();

    assert(ctx != nullptr);

    void *r = nullptr;
    if (redisGetReplyFromReader(ctx, &r) != REDIS_OK) {
        throw_error(*ctx, "Failed to get reply");
    }

    assert(!broken());

    auto reply = ReplyUPtr(static_cast<redisReply*>(r));

    if (handle_error_reply && reply && reply::is_error(*reply)) {
        throw_error(*reply);
    }

    return reply;
}

Connection::Connector::Connector(const ConnectionOptions &opts): sw::redis::original::connection::Connection::Connector(opts)
{

}

redisContext *Connection::Connector::_connect_tcp() const
{
    std::string host = _opts.host;
    int port = _opts.port;
    if (_opts.translateAddrCallback)
        tie(host, port) = _opts.translateAddrCallback(host, port);

    if (_opts.connect_timeout > std::chrono::milliseconds(0)) {
        return redisConnectWithTimeout(host.c_str(),
                                       port,
                                       _to_timeval(_opts.connect_timeout));
    } else {
        return redisConnect(host.c_str(), port);
    }
}

}

}

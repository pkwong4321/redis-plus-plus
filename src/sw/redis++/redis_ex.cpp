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

#ifndef _WIN32
#include <poll.h>
#endif
#include "sw/redis++/redis_ex.h"

namespace sw {

namespace redis {

std::tuple<ReplyUPtr, std::string, std::string> funcWithErrorHandle(const std::function<ReplyUPtr ()> &func)
{
    if (!func())
        return {nullptr, "", "func not defined"};

    std::string exception = "";
    std::string error = "";

    try {
        return {func(), "", ""};
    } catch (const sw::redis::WatchError &e) {
        exception = "WatchError";
        error = e.what();
    } catch (const sw::redis::OomError &e) {
        exception = "OomError";
        error = e.what();
    } catch (const sw::redis::ProtoError &e) {
        exception = "ProtoError";
        error = e.what();
    } catch (const sw::redis::ClosedError &e) {
        exception = "ClosedError";
        error = e.what();
    } catch (const sw::redis::TimeoutError &e) {
        exception = "TimeoutError";
        error = e.what();
    } catch (const sw::redis::IoError &e) {
        exception = "IoError";
        error = e.what();
    } catch (const sw::redis::MovedError &e) {
        exception = "MovedError";
        error = e.what();
    } catch (const sw::redis::AskError &e) {
        exception = "AskError";
        error = e.what();
    } catch (const sw::redis::RedirectionError &e) {
        exception = "RedirectionError";
        error = e.what();
    } catch (const sw::redis::ReplyError &e) {
        exception = "ReplyError";
        error = e.what();
    } catch (const sw::redis::Error &e) {
        exception = "Error";
        error = e.what();
    } catch (const std::exception &e) {
        exception = "std::exception";
        error = e.what();
    } catch (...) {
        exception = "...";
        error = "Unknown exception";
    }

    return {nullptr, exception, error};
}

RedisEx::RedisEx(const GuardedConnectionSPtr &connection): Redis(connection)
{

}

RedisEx::RedisEx(const std::shared_ptr<Sentinel> &sentinel, const std::string &master_name, Role role, const ConnectionOptions &connection_opts, const ConnectionPoolOptions &pool_opts): Redis(sentinel, master_name, role, connection_opts, pool_opts)
{

}

RedisEx::RedisEx(const ConnectionOptions &connection_opts, const ConnectionPoolOptions &pool_opts): Redis(connection_opts, pool_opts)
{

}

Connection RedisEx::fetchInternalConnection()
{
    if (this->_pool)
        return this->_pool->fetch();
    if (this->_connection) {
        auto &connection = this->_connection->connection();
        if (connection.broken())
            connection.reconnect();
        return std::move(connection);
    }
    throw Error("Internal error");
}

void RedisEx::releaseInternalConnection(Connection connection)
{
    if (this->_pool)
        this->_pool->release(std::move(connection));
    if (this->_connection) {
        auto &c = this->_connection->connection();
        c = std::move(connection);
    }
}

}

}

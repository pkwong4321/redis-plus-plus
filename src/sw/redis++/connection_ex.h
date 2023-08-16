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

#ifndef SEWENEW_REDISPLUSPLUS_CONNECTION_EX_H
#define SEWENEW_REDISPLUSPLUS_CONNECTION_EX_H

namespace sw {

namespace redis {

using Role = original::connection::Role;

class Connection: public original::connection::Connection {
public:
    explicit Connection(const ConnectionOptions &opts);

    void reconnect() override;

    int pollBufferRead(int timeout);
    int pollBufferWrite(int timeout);

    void bufferRead();
    void bufferWrite(int *done);

    ReplyUPtr recvFromReader(bool handle_error_reply);

private:
    friend class ConnectionPool;
    Connection(const ConnectionOptions &opts, Dummy);

    class Connector: public original::connection::Connection::Connector {
    public:
        explicit Connector(const ConnectionOptions &opts);
    protected:
        redisContext* _connect_tcp() const override;
    };
};

}

}

#endif

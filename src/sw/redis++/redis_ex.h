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

#ifndef SEWENEW_REDISPLUSPLUS_REDIS_EX_H
#define SEWENEW_REDISPLUSPLUS_REDIS_EX_H

#include <string>
#include "sw/redis++/redis.h"

namespace sw {

namespace redis {

class RedisEx: public Redis
{
private:
    friend class RedisClusterEx;

    explicit RedisEx(const GuardedConnectionSPtr &connection);

public:
    explicit RedisEx(const ConnectionOptions &connection_opts,
                     const ConnectionPoolOptions &pool_opts = {});
    explicit RedisEx(const std::shared_ptr<Sentinel> &sentinel,
                     const std::string &master_name,
                     Role role,
                     const ConnectionOptions &connection_opts,
                     const ConnectionPoolOptions &pool_opts = {});

    Connection fetchInternalConnection();
    void releaseInternalConnection(Connection connection);
};

}

}

#endif

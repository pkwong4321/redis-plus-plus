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

#ifndef SEWENEW_REDISPLUSPLUS_REDIS_CLUSTER_EX_H
#define SEWENEW_REDISPLUSPLUS_REDIS_CLUSTER_EX_H

#include <vector>
#include "sw/redis++/redis_cluster.h"
#include "sw/redis++/redis_ex.h"
#include "sw/redis++/cxx_utils.h"

namespace sw {

namespace redis {

class RedisClusterEx: public RedisCluster
{
public:
    explicit RedisClusterEx(const ConnectionOptions &connection_opts,
                            const ConnectionPoolOptions &pool_opts = {},
                            Role role = Role::MASTER) : RedisCluster(connection_opts, pool_opts, role) {}

    sw::redis::RedisEx redis(const Node &node, bool new_connection = true);

    std::vector<Node> getShardNodes();
    Node getNodeForKey(const StringView &key);
};

}

}

#endif

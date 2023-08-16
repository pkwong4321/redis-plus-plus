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

#include "sw/redis++/redis_cluster_ex.h"
#include "sw/redis++/redis_ex.h"
#include "sw/redis++/queued_redis.h"

namespace sw {

namespace redis {

RedisEx RedisClusterEx::redis(const Node &node, bool new_connection) {
    ConnectionPoolSPtr pool = nullptr;
    if (node.host.empty())
        pool = _pool.fetch();
    else
        pool = _pool.fetch(node);
    if (new_connection) {
        // Create a new pool
        pool = std::make_shared<ConnectionPool>(pool->clone());
    }

    return RedisEx(std::make_shared<GuardedConnection>(pool));
}

std::vector<Node> RedisClusterEx::getShardNodes()
{
    std::vector<Node> nodes;
    auto shards = this->_pool.shards();
    for (const auto &shard: shards)
        nodes.push_back(shard.second);
    return nodes;
}

Node RedisClusterEx::getNodeForKey(const StringView &key)
{
    auto connSPtr = this->_pool.fetch(key);
    auto connOptions = connSPtr->connection_options();
    return {connOptions.host, connOptions.port};
}

}

}

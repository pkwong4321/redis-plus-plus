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

#ifndef SEWENEW_REDISPLUSPLUS_R_SET_H
#define SEWENEW_REDISPLUSPLUS_R_SET_H

#include <string>
#include "reply.h"
#include "command.h"
#include "redis.h"
#include "utils.h"

namespace sw {

namespace redis {

// Redis' SET type.
class RSet {
public:
    const std::string& key() const {
        return _key;
    }

    long long sadd(const StringView &member);

    template <typename Iter>
    long long sadd(Iter first, Iter last);

    long long scard();

    template <typename Input, typename Output>
    void sdiff(Input first, Input last, Output output);

    template <typename Input>
    long long sdiffstore(const StringView &destination,
                            Input first,
                            Input last);

    template <typename Input, typename Output>
    void sinter(Input first, Input last, Output output);

    template <typename Input>
    long long sinterstore(const StringView &destination,
                            Input first,
                            Input last);

    bool sismember(const StringView &member);

    template <typename Iter>
    void smembers(Iter output);

    bool smove(const StringView &destination, const StringView &member);

    OptionalString spop();

    template <typename Iter>
    void spop(long long count, Iter output);

    OptionalString srandmember();

    template <typename Iter>
    void srandmember(long long count, Iter output);

    long long srem(const StringView &member);

    template <typename Iter>
    long long srem(Iter first, Iter last);

    template <typename Input, typename Output>
    void sunion(Input first, Input last, Output output);

    template <typename Input>
    long long sunionstore(const StringView &destination, Input first, Input last);

private:
    friend class Redis;

    RSet(const std::string &key, Redis &redis) : _key(key), _redis(redis) {}

    std::string _key;

    Redis &_redis;
};

template <typename Iter>
long long RSet::sadd(Iter first, Iter last) {
    auto reply = _redis.command(cmd::sadd_range<Iter>, _key, first, last);

    return reply::to_integer(*reply);
}

template <typename Input, typename Output>
void RSet::sdiff(Input first, Input last, Output output) {
    auto reply = _redis.command(cmd::sdiff<Input>, first, last);

    reply::to_array(*reply, output);
}

template <typename Input>
long long RSet::sdiffstore(const StringView &destination,
                            Input first,
                            Input last) {
    auto reply = _redis.command(cmd::sdiffstore<Input>, destination, first, last);

    reply::to_integer(*reply);
}

template <typename Input, typename Output>
void RSet::sinter(Input first, Input last, Output output) {
    auto reply = _redis.command(cmd::sinter<Input>, first, last);

    reply::to_array(*reply, output);
}

template <typename Input>
long long RSet::sinterstore(const StringView &destination,
                            Input first,
                            Input last) {
    auto reply = _redis.command(cmd::sinterstore<Input>, destination, first, last);

    reply::to_integer(*reply);
}

template <typename Iter>
void RSet::smembers(Iter output) {
    auto reply = _redis.command(cmd::smembers, _key);

    reply::to_array(*reply, output);
}

template <typename Iter>
void RSet::spop(long long count, Iter output) {
    auto reply = _redis.command(cmd::spop_range, _key, count);

    reply::to_array(*reply, output);
}

template <typename Iter>
void RSet::srandmember(long long count, Iter output) {
    auto reply = _redis.command(cmd::srandmember_range, _key, count);

    reply::to_array(*reply, output);
}

template <typename Iter>
long long RSet::srem(Iter first, Iter last) {
    auto reply = _redis.command(cmd::srem_range<Iter>, _key, first, last);

    reply::to_integer(*reply);
}

template <typename Input, typename Output>
void RSet::sunion(Input first, Input last, Output output) {
    auto reply = _redis.command(cmd::sunion<Input>, first, last);

    reply::to_array(*reply, output);
}

template <typename Input>
long long RSet::sunionstore(const StringView &destination, Input first, Input last) {
    auto reply = _redis.command(cmd::sunionstore<Input>, destination, first, last);

    return reply::to_integer(*reply);
}

}

}

#endif // end SEWENEW_REDISPLUSPLUS_R_SET_H

#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <math.h>
#include "../pet/lib/types.hpp"

using namespace eosio;

namespace types {
    // concatenation of ids
    uint128_t combine_ids(const uint64_t &x, const uint64_t &y) {
        return (uint128_t{x} << 64) | y;
    }

    typedef uint8_t transfer_type;
    transfer_type TRANSFER_TYPE_ASK = 1;
    transfer_type TRANSFER_TYPE_BID = 2;
    transfer_type TRANSFER_TYPE_ASK_RENT = 11;
    transfer_type TRANSFER_TYPE_BID_RENT = 12;
    transfer_type TRANSFER_TYPE_RENTING = 10;

    // @abi table offers i64
    struct st_offers {
        uuid            id;
        name            user;
        transfer_type   type;
        uuid            pet_id;
        name            new_owner;
        asset           value;
        uint32_t        placed_at;
        uint32_t        ends_at;
        uint32_t        transfer_ends_at;



        uint64_t primary_key() const { return id; }
        uint128_t get_by_user_and_pet() const { return combine_ids(user, pet_id); }

        EOSLIB_SERIALIZE(st_offers, (id)(user)(type)(pet_id)(new_owner)(value)(placed_at)(ends_at)(transfer_ends_at))
    };

   typedef multi_index<N(offers), st_offers,
        indexed_by<N(by_user_and_pet), const_mem_fun<st_offers, uint128_t, &st_offers::get_by_user_and_pet>>
    > _tb_offers;
}
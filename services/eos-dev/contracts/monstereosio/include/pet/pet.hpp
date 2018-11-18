/**
 * Pet-Tamagotchi-Alike Smart Contract
 *
 * The idea is to copy the original tamagotchi to the chain :)
 *
 *
 * @author Leo Ribeiro
 */
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/crypto.h>
#include <eosiolib/transaction.hpp>
#include <eosiolib/singleton.hpp>
#include <math.h>
#include <vector>
#include <map>
#include <pet/utils.hpp>
#include <pet/types.hpp>

using namespace eosio;
using namespace types;
using std::string;
using std::hash;

/* ****************************************** */
/* ------------ Contract Definition --------- */
/* ****************************************** */

CONTRACT pet : public eosio::contract {
public:
    using contract::contract;

    pet(name receiver, name code, datastream<const char*> ds)
    :contract(receiver, code, ds),
    pettypes(receiver, receiver.value),
    elements(receiver, receiver.value),
    equiptypes(receiver, receiver.value),
    equipments(receiver, receiver.value),
    pets(receiver, receiver.value),
    orders(receiver, receiver.value),
    petinbattles(receiver, receiver.value),
    plsinbattles(receiver, receiver.value),
    seed(receiver, receiver.value),
    accounts2(receiver, receiver.value),
    pet_config2(receiver, receiver.value)
    {}

    _tb_pet_types pettypes;
    _tb_elements  elements;
    _tb_equiptypes  equiptypes;
    _tb_equipments  equipments;
    _tb_pet pets;
    _tb_orders orders;
    _tb_pet_in_battle petinbattles;
    _tb_player_in_battle plsinbattles;
    _tb_seed seed;
    _tb_accounts2 accounts2;

    // pet interactions
    void createpet    ( name owner, string pet_name );
    void feedpet      ( uuid pet_id );
    void bedpet       ( uuid pet_id );
    void awakepet     ( uuid pet_id );
    void destroypet   ( uuid pet_id );
    void transferpet  ( uuid pet_id, name new_owner);
    void claimskill   ( uuid pet_id, uint8_t skill );

    // battle interface
    // void battlecreate ( name host, battle_mode mode, checksum256 secret );
    // void battlejoin   ( name host, name player, checksum256 secret );
    // void battlestart  ( name host, name player, st_pick picks );
    // void battleselpet ( name host, name player, uuid pet_id );
    void battleleave  ( name host, name player );
    void quickbattle  ( battle_mode mode, name player, st_pick picks );
    void battleattack ( name host, name player, uuid pet_id, uuid pet_enemy_id, element_type element );
    void battlefinish ( name host, name winner );
    void battlepfdel  ( uuid pet_id, string reason );

    // market interface
    void orderask(uuid pet_id, name new_owner, asset amount, uint32_t until);
    void removeask(name owner, uuid pet_id);
    void claimpet(name old_owner, uuid pet_id, name claimer);
    void bidpet(uuid pet_id, name bidder, asset amount, uint32_t until);
    void removebid(name bidder, uuid pet_id);

    // admin/config interactions
    void setelemttype ( uint64_t id, vector<uint8_t> ratios );
    void setpettype   ( uint64_t id, vector<uint8_t> elements );
    void setequiptype ( uuid id, equipment_type type, uint16_t attack, uint16_t defense, uint16_t hp );
    void changecrtol  ( uint32_t new_interval );
    void changebatma  ( uint16_t new_max_arenas );
    void changebatidt ( uint32_t new_idle_tolerance );
    void changebatami ( uint8_t new_attack_min_factor );
    void changebatama ( uint8_t new_attack_max_factor );
    void techrevive   ( uuid pet_id, string reason );
    void changemktfee ( uint64_t new_fee, string reason );
    void changecreawk ( int64_t new_creation_awake, string reason );
    void changehungtz ( uint32_t new_hunger_to_zero, string reason );

    // token deposits
    void signup       ( name user );
    void transfer     ( uint64_t sender, uint64_t receiver );

    // items
    void openchest    ( name player );
    void petconsume   ( uuid pet_id, symbol item );
    void issueitem    ( name player, asset item, string reason );
    void issueequip   ( name player, uuid itemtype, string reason );
    void issueitems   ( name player, vector<asset> items, string reason );
    void chestreward  ( name owner, uint8_t modifier, string reason );
    void petequip     ( uuid pet_id, uuid item_id );
    void petunequip   ( uuid item_id );

    private:

    /* ****************************************** */
    /* ------------ Contract Config Data -------- */
    /* ****************************************** */

    // @abi table petconfig2 i64
    struct st_pet_config2 {
        uuid     last_id = 0;
        int64_t  creation_awake = 1;
        uint64_t market_fee = 100;
        uint8_t  max_health = 100;
        uint32_t hunger_to_zero = 36 * HOUR; // 129600
        uint32_t min_hunger_interval = 3 * HOUR;
        uint8_t  max_hunger_points = 100;
        uint8_t  hunger_hp_modifier = 1;
        uint32_t min_awake_interval = 8 * HOUR;
        uint32_t min_sleep_period = 4 * HOUR;
        uint32_t creation_tolerance = 1 * HOUR;
        uint32_t battle_idle_tolerance = 60;
        uint8_t  attack_min_factor = 20;
        uint8_t  attack_max_factor = 28;
        uint16_t battle_max_arenas = 10;
        uint16_t battle_busy_arenas = 0;
        uint16_t last_element_id = 0;
        uint16_t last_pet_type_id = 0;
    };

    typedef singleton<"petconfig2"_n, st_pet_config2> pet_config2_singleton;
    pet_config2_singleton pet_config2;

    /* ****************************************** */
    /* ------------ Private Functions ----------- */
    /* ****************************************** */

    // internal config accessers
    st_pet_config2 _get_pet_config();
    void _update_pet_config(const st_pet_config2 &pc);
    uuid _next_id();
    uint64_t _next_element_id();
    uint64_t _next_pet_type_id();

    // generate pseudo random seeds
    int _random(const int num);

    // internal pet calcs
    bool _is_element_valid(const uint8_t &pet_type, const element_type &element_id);
    bool _is_alive(st_pets &pet, const st_pet_config2 &pc);
    uint32_t _calc_hunger_hp(const uint8_t &max_hunger_points,
                             const uint32_t &hunger_to_zero,
                             const uint8_t &hunger_hp_modifier,
                             const uint32_t &last_fed_at,
                             const uint32_t &current_time);

    // pet transfers
    void _transfer_value (name receiver, asset quantity, string memo);
    void _handle_transf  (string memo, asset quantity, name from);
    // void _transfer_pet   ( uuid pet_id, name new_owner);


    // battle helpers
    void _battle_add_pets(st_battle &battle, name player, vector<uint64_t> pet_ids, const st_pet_config2 &pc);

};

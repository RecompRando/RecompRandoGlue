#include <filesystem>
#include <fstream>

#include "Archipelago.h"
#include "apcpp-glue.h"

#if 0 // For native compilation
#  define PTR(x) x*
#  define RDRAM_ARG
#  define RDRAM_ARG1
#  define PASS_RDRAM
#  define PASS_RDRAM1
#  define TO_PTR(type, var) var
#  define GET_MEMBER(type, addr, member) (&addr->member)
#  ifdef __cplusplus
#    define NULLPTR nullptr
#  endif
#else
#  define PTR(x) int32_t
#  define RDRAM_ARG uint8_t *rdram,
#  define RDRAM_ARG1 uint8_t *rdram
#  define PASS_RDRAM rdram,
#  define PASS_RDRAM1 rdram
#  define TO_PTR(type, var) ((type*)(&rdram[(uint64_t)var - 0xFFFFFFFF80000000]))
#  define GET_MEMBER(type, addr, member) (addr + (intptr_t)&(((type*)nullptr)->member))
#  ifdef __cplusplus
#    define NULLPTR (PTR(void))0
#  endif
#endif

typedef uint64_t gpr;

#define SIGNED(val) \
    ((int64_t)(val))

#define ADD32(a, b) \
    ((gpr)(int32_t)((a) + (b)))

#define SUB32(a, b) \
    ((gpr)(int32_t)((a) - (b)))

#define MEM_W(offset, reg) \
    (*(int32_t*)(rdram + ((((reg) + (offset))) - 0xFFFFFFFF80000000)))

#define MEM_H(offset, reg) \
    (*(int16_t*)(rdram + ((((reg) + (offset)) ^ 2) - 0xFFFFFFFF80000000)))

#define MEM_B(offset, reg) \
    (*(int8_t*)(rdram + ((((reg) + (offset)) ^ 3) - 0xFFFFFFFF80000000)))

#define MEM_HU(offset, reg) \
    (*(uint16_t*)(rdram + ((((reg) + (offset)) ^ 2) - 0xFFFFFFFF80000000)))

#define MEM_BU(offset, reg) \
    (*(uint8_t*)(rdram + ((((reg) + (offset)) ^ 3) - 0xFFFFFFFF80000000)))

#define SD(val, offset, reg) { \
    *(uint32_t*)(rdram + ((((reg) + (offset) + 4)) - 0xFFFFFFFF80000000)) = (uint32_t)((gpr)(val) >> 0); \
    *(uint32_t*)(rdram + ((((reg) + (offset) + 0)) - 0xFFFFFFFF80000000)) = (uint32_t)((gpr)(val) >> 32); \
}

#define GI_TRUE_SKULL_TOKEN GI_75

#define GI_AP_PROG GI_77
#define GI_AP_FILLER GI_90
#define GI_AP_USEFUL GI_B3

#define TO_PTR(type, var) ((type*)(&rdram[(uint64_t)var - 0xFFFFFFFF80000000]))

template<int index, typename T>
T _arg(uint8_t* rdram, recomp_context* ctx) {
    static_assert(index < 4, "Only args 0 through 3 supported");
    gpr raw_arg = (&ctx->r4)[index];
    if constexpr (std::is_same_v<T, float>) {
        if constexpr (index < 2) {
            static_assert(index != 1, "Floats in arg 1 not supported");
            return ctx->f12.fl;
        }
        else {
            // static_assert in else workaround
            [] <bool flag = false>() {
                static_assert(flag, "Floats in a2/a3 not supported");
            }();
        }
    }
    else if constexpr (std::is_pointer_v<T>) {
        static_assert (!std::is_pointer_v<std::remove_pointer_t<T>>, "Double pointers not supported");
        return TO_PTR(std::remove_pointer_t<T>, raw_arg);
    }
    else if constexpr (std::is_integral_v<T>) {
        static_assert(sizeof(T) <= 4, "64-bit args not supported");
        return static_cast<T>(raw_arg);
    }
    else {
        // static_assert in else workaround
        [] <bool flag = false>() {
            static_assert(flag, "Unsupported type");
        }();
    }
}

template <typename T>
void _return(recomp_context* ctx, T val) {
    static_assert(sizeof(T) <= 4, "Only 32-bit value returns supported currently");
    if constexpr (std::is_same_v<T, float>) {
        ctx->f0.fl = val;
    }
    else if constexpr (std::is_integral_v<T> && sizeof(T) <= 4) {
        ctx->r2 = int32_t(val);
    }
    else {
        // static_assert in else workaround
        [] <bool flag = false>() {
            static_assert(flag, "Unsupported type");
        }();
    }
}

void glueGetLine(std::ifstream& in, std::string& outString)
{
    char c = in.get();
    
    while (c != '\r' && c != '\n' && c != '\0' && c != -1)
    {
        outString += c;
        c = in.get();
    }

    c = in.peek();

    while (c == '\r' || c == '\n')
    {
        in.get();
        c = in.peek();
    }
}

AP_State* state;

u32 hasItem(u64 itemId)
{
    u32 count = 0;
    u32 items_size = (u32) AP_GetReceivedItemsSize(state);
    for (u32 i = 0; i < items_size; ++i)
    {
        if (AP_GetReceivedItem(state, i) == itemId)
        {
            count += 1;
        }
    }
    return count;
}

int64_t fixLocation(u32 arg)
{
    if ((arg & 0xFF0000) == 0x090000 && AP_GetSlotDataInt(state, "shopsanity") == 1)
    {
        u32 shopItem = arg & 0xFFFF;
        switch (shopItem)
        {
            case SI_NUTS_2:
                shopItem = SI_NUTS_1;
                break;
            case SI_STICK_2:
                shopItem = SI_STICK_1;
                break;
            case SI_ARROWS_LARGE_2:
                shopItem = SI_ARROWS_LARGE_1;
                break;
            case SI_ARROWS_MEDIUM_2:
                shopItem = SI_ARROWS_MEDIUM_1;
                break;
            case SI_FAIRY_2:
                shopItem = SI_FAIRY_1;
                break;
            case SI_POTION_GREEN_3:
                shopItem = SI_POTION_GREEN_2;
                break;
            case SI_SHIELD_HERO_2:
                shopItem = SI_SHIELD_HERO_1;
                break;
            case SI_POTION_RED_3:
                shopItem = SI_POTION_RED_2;
                break;

            case SI_POTION_RED_6:
                shopItem = SI_POTION_RED_5;
                break;
            case SI_ARROWS_SMALL_3:
                shopItem = SI_ARROWS_SMALL_2;
                break;
            case SI_BOMB_3:
                shopItem = SI_BOMB_2;
                break;

            // case SI_BOTTLE:
            // case SI_SWORD_GREAT_FAIRY:
            // case SI_SWORD_KOKIRI:
            // case SI_SWORD_RAZOR:
            // case SI_SWORD_GILDED:
            //     shopItem = SI_SWORD_KOKIRI;
            //     break;
        }
        return 0x090000 | shopItem;
    }

    if (arg == 0x05481E && AP_GetSlotDataInt(state, "shopsanity") != 2) {
        return 0x054D1E;
    }
    return arg;
}

int64_t last_location_sent;

void syncLocation(int64_t location_id)
{
    if (location_id == 0)
    {
        return;
    }
    
    if (location_id == last_location_sent)
    {
        while (!AP_GetLocationIsChecked(state, location_id));
    }
}

void getStr(uint8_t* rdram, PTR(char) ptr, std::string& outString) {
    char c = MEM_B(0, (gpr) ptr);
    u32 i = 0;
    while (c != 0) {
        outString += c;
        i += 1;
        c = MEM_B(i, (gpr) ptr);
    }
}

extern "C"
{
    DLLEXPORT u32 recomp_api_version = 1;
    
    DLLEXPORT void rando_init(uint8_t* rdram, recomp_context* ctx)
    {
        std::string address;
        std::string playerName;
        std::string password;
        
        PTR(char) address_ptr = _arg<0, PTR(char)>(rdram, ctx);
        PTR(char) player_name_ptr = _arg<1, PTR(char)>(rdram, ctx);
        PTR(char) password_ptr = _arg<2, PTR(char)>(rdram, ctx);
        
        getStr(rdram, address_ptr, address);
        getStr(rdram, player_name_ptr, playerName);
        getStr(rdram, password_ptr, password);
        
        state = AP_New();
        AP_Init(state, address.c_str(), "Majora's Mask Recompiled", playerName.c_str(), password.c_str());
        //AP_Init("apsolostartinventory.json");
        
        AP_SetDeathLinkSupported(state, true);
        
        AP_Start(state);
        
        while (!AP_IsConnected(state))
        {
            if (AP_GetConnectionStatus(state) == AP_ConnectionStatus::ConnectionRefused || AP_GetConnectionStatus(state) == AP_ConnectionStatus::NotFound)
            {
                AP_Stop(state);
                _return(ctx, false);
                return;
            }
        }
        
        AP_QueueLocationScoutsAll(state);
        
        if (AP_GetSlotDataInt(state, "skullsanity") == 2)
        {
            for (int i = 0x00; i <= 0x1E; ++i)
            {
                if (i == 0x03)
                {
                    continue;
                }
                
                int64_t location_id = 0x3469420062700 | i;
                AP_RemoveQueuedLocationScout(state, location_id);
            }
            for (int i = 0x01; i <= 0x1E; ++i)
            {
                int64_t location_id = 0x3469420062800 | i;
                AP_RemoveQueuedLocationScout(state, location_id);
            }
        }
        
        for (int64_t i = AP_GetSlotDataInt(state, "starting_heart_locations"); i < 8; ++i)
        {
            int64_t location_id = 0x34694200D0000 | i;
            AP_RemoveQueuedLocationScout(state, location_id);
        }

        if (AP_GetSlotDataInt(state, "cowsanity") == 0)
        {
            for (int i = 0x10; i <= 0x17; ++i)
            {
                int64_t location_id = 0x3469420BEEF00 | i;
                AP_RemoveQueuedLocationScout(state, location_id);
            }
        }

        if (AP_GetSlotDataInt(state, "scrubsanity") == 0)
        {
            AP_RemoveQueuedLocationScout(state, 0x3469420090100 | GI_MAGIC_BEANS);
            AP_RemoveQueuedLocationScout(state, 0x3469420090100 | GI_BOMB_BAG_40);
            AP_RemoveQueuedLocationScout(state, 0x3469420090100 | GI_POTION_GREEN);
            AP_RemoveQueuedLocationScout(state, 0x3469420090100 | GI_POTION_BLUE);
        }

        if (AP_GetSlotDataInt(state, "shopsanity") != 2)
        {
            if (AP_GetSlotDataInt(state, "shopsanity") == 1)
            {
                for (int i = SI_FAIRY_2; i <= SI_POTION_RED_3; ++i)
                {
                    int64_t location_id = 0x3469420090000 | i;
                    AP_RemoveQueuedLocationScout(state, location_id);
                }

                AP_RemoveQueuedLocationScout(state, 0x3469420090000 | SI_BOMB_3);
                AP_RemoveQueuedLocationScout(state, 0x3469420090000 | SI_ARROWS_SMALL_3);
                AP_RemoveQueuedLocationScout(state, 0x3469420090000 | SI_POTION_RED_6);
                
                AP_RemoveQueuedLocationScout(state, 0x346942005481E);
            }
            else
            {
                for (int i = SI_POTION_RED_1; i <= SI_POTION_RED_6; ++i)
                {
                    if (i == SI_BOMB_BAG_20_1 || i == SI_BOMB_BAG_40)
                    {
                        continue;
                    }

                    int64_t location_id = 0x3469420090000 | i;
                    AP_RemoveQueuedLocationScout(state, location_id);
                }

                AP_RemoveQueuedLocationScout(state, 0x3469420026392);
                AP_RemoveQueuedLocationScout(state, 0x3469420090000 | GI_CHATEAU);
                AP_RemoveQueuedLocationScout(state, 0x3469420006792);
                AP_RemoveQueuedLocationScout(state, 0x3469420000091);
            }
        }
        
        AP_SendQueuedLocationScouts(state, 0);

        _return(ctx, true);
    }
    
    DLLEXPORT void rando_skulltulas_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt(state, "skullsanity") != 2);
    }
    
    DLLEXPORT void rando_shopsanity_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt(state, "shopsanity") != 0);
    }

    DLLEXPORT void rando_scrubs_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt(state, "scrubsanity") == 1);
    }

    DLLEXPORT void rando_cows_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt(state, "cowsanity") == 1);
    }
    
    DLLEXPORT void rando_damage_multiplier(uint8_t* rdram, recomp_context* ctx)
    {
        switch (AP_GetSlotDataInt(state, "damage_multiplier"))
        {
            case 0:
                _return(ctx, (u32) 0);
                return;
            case 1:
                _return(ctx, (u32) 1);
                return;
            case 2:
                _return(ctx, (u32) 2);
                return;
            case 3:
                _return(ctx, (u32) 4);
                return;
            case 4:
                _return(ctx, (u32) 0xF);
                return;
        }
        return;
    }
    
    DLLEXPORT void rando_death_behavior(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, (u32) AP_GetSlotDataInt(state, "death_behavior"));
    }

    DLLEXPORT void rando_get_death_link_pending(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_DeathLinkPending(state));
    }
    
    DLLEXPORT void rando_reset_death_link_pending(uint8_t* rdram, recomp_context* ctx)
    {
        AP_DeathLinkClear(state);
    }
    
    DLLEXPORT void rando_get_death_link_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt(state, "death_link") == 1);
    }
    
    DLLEXPORT void rando_send_death_link(uint8_t* rdram, recomp_context* ctx)
    {
        AP_DeathLinkSend(state);
    }
    
    DLLEXPORT void rando_get_camc_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt(state, "camc") == 1);
    }
    
    DLLEXPORT void rando_get_start_with_consumables_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt(state, "start_with_consumables") == 1);
    }
    
    DLLEXPORT void rando_get_permanent_chateau_romani_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt(state, "permanent_chateau_romani") == 1);
    }
    
    DLLEXPORT void rando_get_start_with_inverted_time_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt(state, "start_with_inverted_time") == 1);
    }
    
    DLLEXPORT void rando_get_receive_filled_wallets_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt(state, "receive_filled_wallets") == 1);
    }
    
    DLLEXPORT void rando_get_starting_heart_locations(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, (int) AP_GetSlotDataInt(state, "starting_heart_locations"));
    }
    
    DLLEXPORT void rando_get_tunic_color(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, (int) AP_GetSlotDataInt(state, "link_tunic_color"));
    }
    
    DLLEXPORT void rando_get_location_type(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location = 0x3469420000000 | fixLocation(arg);
        _return(ctx, (int) AP_GetLocationItemType(state, location));
    }
    
    DLLEXPORT void rando_get_item_id(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        
        if (arg == 0)
        {
            _return(ctx, 0);
            return;
        }
        
        int64_t location = 0x3469420000000 | fixLocation(arg);
        
        if (AP_GetLocationHasLocalItem(state, location))
        {
            int64_t item = AP_GetItemAtLocation(state, location) & 0xFFFFFF;
            
            if ((item & 0xFF0000) == 0x000000)
            {
                u8 gi = item & 0xFF;
                
                if (gi == GI_SWORD_KOKIRI)
                {
                    _return(ctx, (u32) MIN(GI_SWORD_KOKIRI + hasItem(0x3469420000000 | GI_SWORD_KOKIRI), GI_SWORD_GILDED));
                    return;
                }
                
                else if (gi == GI_QUIVER_30)
                {
                    _return(ctx, (u32) MIN(GI_QUIVER_30 + hasItem(0x3469420000000 | GI_QUIVER_30), GI_QUIVER_50));
                    return;
                }
                
                else if (gi == GI_BOMB_BAG_20)
                {
                    _return(ctx, (u32) MIN(GI_BOMB_BAG_20 + hasItem(0x3469420000000 | GI_BOMB_BAG_20), GI_BOMB_BAG_40));
                    return;
                }
                
                else if (gi == GI_WALLET_ADULT)
                {
                    _return(ctx, (u32) MIN(GI_WALLET_ADULT + hasItem(0x3469420000000 | GI_WALLET_ADULT), GI_WALLET_GIANT));
                    return;
                }
                
                _return(ctx, (u32) gi);
                return;
            }
            switch (item & 0xFF0000)
            {
                case 0x010000:
                    switch (item & 0xFF)
                    {
                        case 0x7F:
                            _return(ctx, (u32) GI_B2);
                            return;
                        case 0x00:
                            _return(ctx, (u32) GI_46);
                            return;
                        case 0x01:
                            _return(ctx, (u32) GI_47);
                            return;
                        case 0x02:
                            _return(ctx, (u32) GI_48);
                            return;
                        case 0x03:
                            _return(ctx, (u32) GI_49);
                            return;
                    }
                    return;
                case 0x020000:
                    switch (item & 0xFF)
                    {
                        case 0x00:
                            _return(ctx, (u32) GI_MAGIC_JAR_SMALL);
                            return;
                        case 0x01:
                            _return(ctx, (u32) GI_71);
                            return;
                    }
                    return;
                case 0x040000:
                    switch (item & 0xFF)
                    {
                        case ITEM_SONG_TIME:
                            _return(ctx, (u32) GI_A6);
                            return;
                        case ITEM_SONG_HEALING:
                            _return(ctx, (u32) GI_AF);
                            return;
                        case ITEM_SONG_EPONA:
                            _return(ctx, (u32) GI_A5);
                            return;
                        case ITEM_SONG_SOARING:
                            _return(ctx, (u32) GI_A3);
                            return;
                        case ITEM_SONG_STORMS:
                            _return(ctx, (u32) GI_A2);
                            return;
                        case ITEM_SONG_SONATA:
                            _return(ctx, (u32) GI_AE);
                            return;
                        case ITEM_SONG_LULLABY:
                            _return(ctx, (u32) GI_AD);
                            return;
                        case ITEM_SONG_NOVA:
                            _return(ctx, (u32) GI_AC);
                            return;
                        case ITEM_SONG_ELEGY:
                            _return(ctx, (u32) GI_A8);
                            return;
                        case ITEM_SONG_OATH:
                            _return(ctx, (u32) GI_A7);
                            return;
                    }
                    return;
                case 0x090000:
                    switch (item & 0xFF)
                    {
                        case ITEM_KEY_BOSS:
                            _return(ctx, (u32) (GI_MAX + (((item >> 8) & 0xF) * 4) + 1));
                            return;
                        case ITEM_KEY_SMALL:
                            _return(ctx, (u32) (GI_MAX + (((item >> 8) & 0xF) * 4) + 2));
                            return;
                        case ITEM_DUNGEON_MAP:
                            _return(ctx, (u32) (GI_MAX + (((item >> 8) & 0xF) * 4) + 3));
                            return;
                        case ITEM_COMPASS:
                            _return(ctx, (u32) (GI_MAX + (((item >> 8) & 0xF) * 4) + 4));
                            return;
                    }
                    return;
            }
        }
        
        switch (AP_GetLocationItemType(state, location))
        {
            case ITEM_TYPE_FILLER:
                _return(ctx, (u32) GI_AP_FILLER);
                return;
            case ITEM_TYPE_USEFUL:
                _return(ctx, (u32) GI_AP_USEFUL);
                return;
            default:
                _return(ctx, (u32) GI_AP_PROG);
                return;
        }
    }
    
    DLLEXPORT void rando_get_items_size(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, ((u32) AP_GetReceivedItemsSize(state)));
    }
    
    DLLEXPORT void rando_get_item(uint8_t* rdram, recomp_context* ctx)
    {
        u32 items_i = _arg<0, u32>(rdram, ctx);
        _return(ctx, ((u32) AP_GetReceivedItem(state, items_i)));
    }
    
    DLLEXPORT void rando_has_item(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t item_id = ((int64_t) (((int64_t) 0x3469420000000) | ((int64_t) arg)));
        syncLocation(last_location_sent);
        _return(ctx, hasItem(item_id));
    }
    
    DLLEXPORT void rando_send_location(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location_id = ((int64_t) (((int64_t) 0x3469420000000) | ((int64_t) fixLocation(arg))));
        if (AP_LocationExists(state, location_id) && !AP_GetLocationIsChecked(state, location_id))
        {
            AP_SendItem(state, location_id);
            last_location_sent = location_id;
        }
    }
    
    DLLEXPORT void rando_location_is_checked(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location_id = ((int64_t) (((int64_t) 0x3469420000000) | ((int64_t) fixLocation(arg))));
        syncLocation(location_id);
        _return(ctx, AP_GetLocationIsChecked(state, location_id));
    }
    
    DLLEXPORT void rando_complete_goal(uint8_t* rdram, recomp_context* ctx)
    {
        AP_StoryComplete(state);
    }
}
#include <filesystem>
#include <fstream>

#include "Archipelago.h"
#include "apcpp-glue.h"

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
}

u32 hasItem(u64 itemId)
{
    u32 count = 0;
    u32 items_size = (u32) AP_GetReceivedItemsSize();
    for (u32 i = 0; i < items_size; ++i)
    {
        if (AP_GetReceivedItem(i) == itemId)
        {
            count += 1;
        }
    }
    return count;
}

int64_t fixLocation(u32 arg)
{
    if ((arg & 0xFF0000) == 0x090000 && AP_GetSlotDataInt("shopsanity") == 1)
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

    if (arg == 0x05481E && AP_GetSlotDataInt("shopsanity") != 2) {
        return 0x054D1E;
    }
    return arg;
}

extern "C"
{
    DLLEXPORT u32 recomp_api_version = 1;
    
    DLLEXPORT void rando_init(uint8_t* rdram, recomp_context* ctx)
    {
        std::ifstream apconnect("apconnect.txt");
        
        if (apconnect.good())
        {
            std::string apEnabled;
            glueGetLine(apconnect, apEnabled);
            
            if (apEnabled == "true")
            {
                std::string address;
                std::string playerName;
                std::string password;
                
                glueGetLine(apconnect, address);
                glueGetLine(apconnect, playerName);
                glueGetLine(apconnect, password);
                
                AP_Init(address.c_str(), "Majora's Mask Recompiled", playerName.c_str(), password.c_str());
                //AP_Init("apsolostartinventory.json");
                
                AP_SetDeathLinkSupported(true);
                
                AP_Start();
                
                while (!AP_IsConnected())
                {
                    if (AP_GetConnectionStatus() == AP_ConnectionStatus::ConnectionRefused || AP_GetConnectionStatus() == AP_ConnectionStatus::NotFound)
                    {
                        AP_Stop();
                        apconnect.close();
                        *((u8*) 0x0) = 0x0;  // screw no ui just crash everything
                        return;
                    }
                }
                
                AP_QueueLocationScoutsAll();
                
                if (AP_GetSlotDataInt("skullsanity") == 2)
                {
                    for (int i = 0x00; i <= 0x1E; ++i)
                    {
                        if (i == 0x03)
                        {
                            continue;
                        }
                        
                        int64_t location_id = 0x3469420062700 | i;
                        AP_RemoveQueuedLocationScout(location_id);
                    }
                    for (int i = 0x01; i <= 0x1E; ++i)
                    {                        
                        int64_t location_id = 0x3469420062800 | i;
                        AP_RemoveQueuedLocationScout(location_id);
                    }
                }
                
                for (int64_t i = AP_GetSlotDataInt("starting_heart_locations"); i < 8; ++i)
                {
                    int64_t location_id = 0x34694200D0000 | i;
                    AP_RemoveQueuedLocationScout(location_id);
                }

                if (AP_GetSlotDataInt("cowsanity") == 0)
                {
                    for (int i = 0x10; i <= 0x17; ++i)
                    {                        
                        int64_t location_id = 0x3469420BEEF00 | i;
                        AP_RemoveQueuedLocationScout(location_id);
                    }
                }

                if (AP_GetSlotDataInt("scrubsanity") == 0)
                {
                    AP_RemoveQueuedLocationScout(0x3469420090100 | GI_MAGIC_BEANS);
                    AP_RemoveQueuedLocationScout(0x3469420090100 | GI_BOMB_BAG_40);
                    AP_RemoveQueuedLocationScout(0x3469420090100 | GI_POTION_GREEN);
                    AP_RemoveQueuedLocationScout(0x3469420090100 | GI_POTION_BLUE);
                }

                if (AP_GetSlotDataInt("shopsanity") != 2)
                {
                    if (AP_GetSlotDataInt("shopsanity") == 1)
                    {
                        for (int i = SI_FAIRY_2; i <= SI_POTION_RED_3; ++i)
                        {                        
                            int64_t location_id = 0x3469420090000 | i;
                            AP_RemoveQueuedLocationScout(location_id);
                        }

                        AP_RemoveQueuedLocationScout(0x3469420090000 | SI_BOMB_3);
                        AP_RemoveQueuedLocationScout(0x3469420090000 | SI_ARROWS_SMALL_3);
                        AP_RemoveQueuedLocationScout(0x3469420090000 | SI_POTION_RED_6);
                        
                        AP_RemoveQueuedLocationScout(0x346942005481E);
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
                            AP_RemoveQueuedLocationScout(location_id);
                        }

                        AP_RemoveQueuedLocationScout(0x3469420026392);
                        AP_RemoveQueuedLocationScout(0x3469420090000 | GI_CHATEAU);
                        AP_RemoveQueuedLocationScout(0x3469420006792);
                    }
                }
                
                AP_SendQueuedLocationScouts(0);
            }
            
            else
            {
                *((u8*) 0x0) = 0x0;  // "false" (or any other string) not supported, crash it
            }
        }
        
        else
        {
            *((u8*) 0x0) = 0x0;  // no apconnect.txt, crash it
        }
    }
    
    DLLEXPORT void rando_skulltulas_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt("skullsanity") != 2);
    }
    
    DLLEXPORT void rando_shopsanity_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt("shopsanity") != 0);
    }

    DLLEXPORT void rando_scrubs_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt("scrubsanity") == 1);
    }

    DLLEXPORT void rando_cows_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt("cowsanity") == 1);
    }
    
    DLLEXPORT void rando_damage_multiplier(uint8_t* rdram, recomp_context* ctx)
    {
        switch (AP_GetSlotDataInt("damage_multiplier"))
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
        _return(ctx, (u32) AP_GetSlotDataInt("death_behavior"));
    }

    DLLEXPORT void rando_get_death_link_pending(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_DeathLinkPending());
    }
    
    DLLEXPORT void rando_reset_death_link_pending(uint8_t* rdram, recomp_context* ctx)
    {
        AP_DeathLinkClear();
    }
    
    DLLEXPORT void rando_get_death_link_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt("death_link") == 1);
    }
    
    DLLEXPORT void rando_send_death_link(uint8_t* rdram, recomp_context* ctx)
    {
        AP_DeathLinkSend();
    }
    
    DLLEXPORT void rando_get_camc_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt("camc") == 1);
    }
    
    DLLEXPORT void rando_get_start_with_consumables_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt("start_with_consumables") == 1);
    }
    
    DLLEXPORT void rando_get_permanent_chateau_romani_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt("permanent_chateau_romani") == 1);
    }
    
    DLLEXPORT void rando_get_start_with_inverted_time_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt("start_with_inverted_time") == 1);
    }
    
    DLLEXPORT void rando_get_receive_filled_wallets_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt("receive_filled_wallets") == 1);
    }
    
    DLLEXPORT void rando_get_starting_heart_locations(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, (int) AP_GetSlotDataInt("starting_heart_locations"));
    }
    
    DLLEXPORT void rando_get_tunic_color(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, (int) AP_GetSlotDataInt("link_tunic_color"));
    }
    
    DLLEXPORT void rando_get_location_type(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location = 0x3469420000000 | fixLocation(arg);
        _return(ctx, (int) AP_GetLocationItemType(location));
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
        
        if (AP_GetLocationHasLocalItem(location))
        {
            int64_t item = AP_GetItemAtLocation(location) & 0xFFFFFF;
            
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
        
        switch (AP_GetLocationItemType(location))
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
    
    DLLEXPORT void rando_say(uint8_t* rdram, recomp_context* ctx)
    {
        // doesn't work
        AP_Say((char*) TO_PTR(char, ctx->r4));
    }
    
    DLLEXPORT void rando_get_items_size(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, ((u32) AP_GetReceivedItemsSize()));
    }
    
    DLLEXPORT void rando_get_item(uint8_t* rdram, recomp_context* ctx)
    {
        u32 items_i = _arg<0, u32>(rdram, ctx);
        _return(ctx, ((u32) AP_GetReceivedItem(items_i)));
    }
    
    DLLEXPORT void rando_has_item(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location_id = ((int64_t) (((int64_t) 0x3469420000000) | ((int64_t) fixLocation(arg))));
        _return(ctx, hasItem(location_id));
    }
    
    DLLEXPORT void rando_send_location(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location_id = ((int64_t) (((int64_t) 0x3469420000000) | ((int64_t) fixLocation(arg))));
        if (AP_LocationExists(location_id) && !AP_GetLocationIsChecked(location_id))
        {
            AP_SendItem(location_id);
            while (!AP_GetLocationIsChecked(location_id));
        }
    }
    
    DLLEXPORT void rando_location_is_checked(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location_id = ((int64_t) (((int64_t) 0x3469420000000) | ((int64_t) fixLocation(arg))));
        _return(ctx, AP_GetLocationIsChecked(location_id));
    }
    
    DLLEXPORT void rando_complete_goal(uint8_t* rdram, recomp_context* ctx)
    {
        AP_StoryComplete();
    }
}
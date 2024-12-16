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
                        uint64_t location_id = 0x3469420062700 | i;
                        AP_RemoveQueuedLocationScout(location_id);
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
    
    DLLEXPORT void rando_get_location_type(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location = 0x3469420000000 | arg;
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
        
        int64_t location = 0x3469420000000 | arg;
        
        if (AP_GetLocationHasLocalItem(location))
        {
            int64_t item = AP_GetItemAtLocation(location) & 0xFFFFFF;
            
            if ((item & 0xFF0000) == 0x000000)
            {
                u8 gi = item & 0xFF;
                
                if (gi == GI_SKULL_TOKEN)
                {
                    _return(ctx, (u32) GI_TRUE_SKULL_TOKEN);
                    return;
                }
                
                else if (gi == GI_SWORD_KOKIRI)
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
                    _return(ctx, (u32) GI_B2);
                    return;
                case 0x020000:
                    switch (item & 0xFF)
                    {
                        case 0x00:
                            _return(ctx, (u32) GI_MAGIC_JAR_SMALL);
                            return;
                        case 0x01:
                            _return(ctx, (u32) GI_SWORD_KOKIRI);
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
                        case ITEM_DUNGEON_MAP:
                            _return(ctx, (u32) GI_MAP);
                            return;
                        case ITEM_COMPASS:
                            _return(ctx, (u32) GI_COMPASS);
                            return;
                        case ITEM_KEY_BOSS:
                            _return(ctx, (u32) GI_KEY_BOSS);
                            return;
                        case ITEM_KEY_SMALL:
                            _return(ctx, (u32) GI_KEY_SMALL);
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
        AP_Say(std::string((char*) ctx->r4));
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
        int64_t location_id = ((int64_t) (((int64_t) 0x3469420000000) | ((int64_t) arg)));
        _return(ctx, hasItem(location_id));
    }
    
    DLLEXPORT void rando_send_location(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location_id = ((int64_t) (((int64_t) 0x3469420000000) | ((int64_t) arg)));
        if (!AP_GetLocationIsChecked(location_id))
        {
            AP_SendItem(location_id);
        }
    }
    
    DLLEXPORT void rando_location_is_checked(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location_id = ((int64_t) (((int64_t) 0x3469420000000) | ((int64_t) arg)));
        _return(ctx, AP_GetLocationIsChecked(location_id));
    }
    
    DLLEXPORT void rando_complete_goal(uint8_t* rdram, recomp_context* ctx)
    {
        AP_StoryComplete();
    }
}
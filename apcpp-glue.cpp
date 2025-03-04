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
                
                state = AP_New();
                AP_Init(state, address.c_str(), "Banjo-Kazooie", playerName.c_str(), password.c_str());
                //AP_Init("apsolostartinventory.json");
                
                //~ AP_SetDeathLinkSupported(state, true);
                
                AP_Start(state);
                
                while (!AP_IsConnected(state))
                {
                    if (AP_GetConnectionStatus(state) == AP_ConnectionStatus::ConnectionRefused || AP_GetConnectionStatus(state) == AP_ConnectionStatus::NotFound)
                    {
                        AP_Stop(state);
                        apconnect.close();
                        *((u8*) 0x0) = 0x0;  // screw no ui just crash everything
                    }
                }
                
                AP_QueueLocationScoutsAll(state);
                
                AP_SendQueuedLocationScouts(state, 0);
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
    
    DLLEXPORT void rando_get_location_type(uint8_t* rdram, recomp_context* ctx)
    {
        int64_t location_id = (int64_t) _arg<0, u32>(rdram, ctx);
        _return(ctx, (int) AP_GetLocationItemType(state, location_id));
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
        int64_t item_id = (int64_t) _arg<0, u32>(rdram, ctx);
        syncLocation(last_location_sent);
        _return(ctx, hasItem(item_id));
    }
    
    DLLEXPORT void rando_send_location(uint8_t* rdram, recomp_context* ctx)
    {
        int64_t location_id = (int64_t) _arg<0, u32>(rdram, ctx);
        if (AP_LocationExists(state, location_id) && !AP_GetLocationIsChecked(state, location_id))
        {
            AP_SendItem(state, location_id);
            last_location_sent = location_id;
        }
    }
    
    DLLEXPORT void rando_location_is_checked(uint8_t* rdram, recomp_context* ctx)
    {
        int64_t location_id = (int64_t) _arg<0, u32>(rdram, ctx);
        syncLocation(location_id);
        _return(ctx, AP_GetLocationIsChecked(state, location_id));
    }
    
    DLLEXPORT void rando_get_datastorage_u32_sync(uint8_t* rdram, recomp_context* ctx)
    {
        PTR(char) ptr = _arg<0, PTR(char)>(rdram, ctx);

        std::string key = "";
        getStr(rdram, ptr, key);
        char* value_char_ptr = AP_GetDataStorageSync(state, key.c_str());

        u32 value = 0;

        if (strncmp(value_char_ptr, "null", 4) != 0)
        {
            value = std::stoi(value_char_ptr);
        }

        _return(ctx, value);
    }
    
    DLLEXPORT void rando_set_datastorage_u32_sync(uint8_t* rdram, recomp_context* ctx)
    {
        PTR(char) ptr = _arg<0, PTR(char)>(rdram, ctx);
        u32 value = _arg<1, u32>(rdram, ctx);
        std::string key = "";
        getStr(rdram, ptr, key);

        try
        {
            AP_SetDataStorageSync(state, key.c_str(), (char*) std::to_string(value).c_str());
        }

        catch (std::exception e)
        {
            fprintf(stderr, "error setting datastorage u32\n");
            fprintf(stderr, e.what());
        }
    }
    
    DLLEXPORT void rando_set_datastorage_u32_async(uint8_t* rdram, recomp_context* ctx)
    {
        PTR(char) ptr = _arg<0, PTR(char)>(rdram, ctx);
        u32 value = _arg<1, u32>(rdram, ctx);
        std::string key = "";
        getStr(rdram, ptr, key);

        try
        {
            AP_SetDataStorageAsync(state, key.c_str(), (char*) std::to_string(value).c_str());
        }

        catch (std::exception e)
        {
            fprintf(stderr, "error setting datastorage u32\n");
            fprintf(stderr, e.what());
        }
    }
    
    DLLEXPORT void rando_complete_goal(uint8_t* rdram, recomp_context* ctx)
    {
        AP_StoryComplete(state);
    }
}
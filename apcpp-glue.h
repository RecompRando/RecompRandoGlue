#pragma once

#include <stdint.h>
#include <string>
#include <iostream>
#include <filesystem>

#if defined(_MSC_VER)
    //  Microsoft
    #define DLLEXPORT __declspec(dllexport)
    #define DLLIMPORT __declspec(dllimport)
#elif defined(__GNUC__)
    //  GCC
    #define DLLEXPORT __attribute__((visibility("default")))
    #define DLLIMPORT
#else
    #define DLLEXPORT
    #define DLLIMPORT
#endif

#define MIN(x, y) (x < y ? x : y)

typedef enum ItemId {
    /* 0x00 */ ITEM_OCARINA_OF_TIME,
    /* 0x01 */ ITEM_BOW,
    /* 0x02 */ ITEM_ARROW_FIRE,
    /* 0x03 */ ITEM_ARROW_ICE,
    /* 0x04 */ ITEM_ARROW_LIGHT,
    /* 0x05 */ ITEM_OCARINA_FAIRY,
    /* 0x06 */ ITEM_BOMB,
    /* 0x07 */ ITEM_BOMBCHU,
    /* 0x08 */ ITEM_DEKU_STICK,
    /* 0x09 */ ITEM_DEKU_NUT,
    /* 0x0A */ ITEM_MAGIC_BEANS,
    /* 0x0B */ ITEM_SLINGSHOT,
    /* 0x0C */ ITEM_POWDER_KEG,
    /* 0x0D */ ITEM_PICTOGRAPH_BOX,
    /* 0x0E */ ITEM_LENS_OF_TRUTH,
    /* 0x0F */ ITEM_HOOKSHOT,
    /* 0x10 */ ITEM_SWORD_GREAT_FAIRY,
    /* 0x11 */ ITEM_LONGSHOT, // OoT Leftover
    /* 0x12 */ ITEM_BOTTLE,
    /* 0x13 */ ITEM_POTION_RED,
    /* 0x14 */ ITEM_POTION_GREEN,
    /* 0x15 */ ITEM_POTION_BLUE,
    /* 0x16 */ ITEM_FAIRY,
    /* 0x17 */ ITEM_DEKU_PRINCESS,
    /* 0x18 */ ITEM_MILK_BOTTLE,
    /* 0x19 */ ITEM_MILK_HALF,
    /* 0x1A */ ITEM_FISH,
    /* 0x1B */ ITEM_BUG,
    /* 0x1C */ ITEM_BLUE_FIRE,
    /* 0x1D */ ITEM_POE,
    /* 0x1E */ ITEM_BIG_POE,
    /* 0x1F */ ITEM_SPRING_WATER,
    /* 0x20 */ ITEM_HOT_SPRING_WATER,
    /* 0x21 */ ITEM_ZORA_EGG,
    /* 0x22 */ ITEM_GOLD_DUST,
    /* 0x23 */ ITEM_MUSHROOM,
    /* 0x24 */ ITEM_SEAHORSE,
    /* 0x25 */ ITEM_CHATEAU,
    /* 0x26 */ ITEM_HYLIAN_LOACH,
    /* 0x27 */ ITEM_OBABA_DRINK,
    /* 0x28 */ ITEM_MOONS_TEAR,
    /* 0x29 */ ITEM_DEED_LAND,
    /* 0x2A */ ITEM_DEED_SWAMP,
    /* 0x2B */ ITEM_DEED_MOUNTAIN,
    /* 0x2C */ ITEM_DEED_OCEAN,
    /* 0x2D */ ITEM_ROOM_KEY,
    /* 0x2E */ ITEM_LETTER_MAMA,
    /* 0x2F */ ITEM_LETTER_TO_KAFEI,
    /* 0x30 */ ITEM_PENDANT_OF_MEMORIES,
    /* 0x31 */ ITEM_TINGLE_MAP,
    /* 0x32 */ ITEM_MASK_DEKU,
    /* 0x33 */ ITEM_MASK_GORON,
    /* 0x34 */ ITEM_MASK_ZORA,
    /* 0x35 */ ITEM_MASK_FIERCE_DEITY,
    /* 0x36 */ ITEM_MASK_TRUTH,
    /* 0x37 */ ITEM_MASK_KAFEIS_MASK,
    /* 0x38 */ ITEM_MASK_ALL_NIGHT,
    /* 0x39 */ ITEM_MASK_BUNNY,
    /* 0x3A */ ITEM_MASK_KEATON,
    /* 0x3B */ ITEM_MASK_GARO,
    /* 0x3C */ ITEM_MASK_ROMANI,
    /* 0x3D */ ITEM_MASK_CIRCUS_LEADER,
    /* 0x3E */ ITEM_MASK_POSTMAN,
    /* 0x3F */ ITEM_MASK_COUPLE,
    /* 0x40 */ ITEM_MASK_GREAT_FAIRY,
    /* 0x41 */ ITEM_MASK_GIBDO,
    /* 0x42 */ ITEM_MASK_DON_GERO,
    /* 0x43 */ ITEM_MASK_KAMARO,
    /* 0x44 */ ITEM_MASK_CAPTAIN,
    /* 0x45 */ ITEM_MASK_STONE,
    /* 0x46 */ ITEM_MASK_BREMEN,
    /* 0x47 */ ITEM_MASK_BLAST,
    /* 0x48 */ ITEM_MASK_SCENTS,
    /* 0x49 */ ITEM_MASK_GIANT,
    /* 0x4A */ ITEM_BOW_FIRE,
    /* 0x4B */ ITEM_BOW_ICE,
    /* 0x4C */ ITEM_BOW_LIGHT,
    /* 0x4D */ ITEM_SWORD_KOKIRI,
    /* 0x4E */ ITEM_SWORD_RAZOR,
    /* 0x4F */ ITEM_SWORD_GILDED,
    /* 0x50 */ ITEM_SWORD_DEITY,
    /* 0x51 */ ITEM_SHIELD_HERO,
    /* 0x52 */ ITEM_SHIELD_MIRROR,
    /* 0x53 */ ITEM_QUIVER_30,
    /* 0x54 */ ITEM_QUIVER_40,
    /* 0x55 */ ITEM_QUIVER_50,
    /* 0x56 */ ITEM_BOMB_BAG_20,
    /* 0x57 */ ITEM_BOMB_BAG_30,
    /* 0x58 */ ITEM_BOMB_BAG_40,
    /* 0x59 */ ITEM_WALLET_DEFAULT,
    /* 0x5A */ ITEM_WALLET_ADULT,
    /* 0x5B */ ITEM_WALLET_GIANT,
    /* 0x5C */ ITEM_FISHING_ROD,
    /* 0x5D */ ITEM_REMAINS_ODOLWA,
    /* 0x5E */ ITEM_REMAINS_GOHT,
    /* 0x5F */ ITEM_REMAINS_GYORG,
    /* 0x60 */ ITEM_REMAINS_TWINMOLD,
    /* 0x61 */ ITEM_SONG_SONATA,
    /* 0x62 */ ITEM_SONG_LULLABY,
    /* 0x63 */ ITEM_SONG_NOVA,
    /* 0x64 */ ITEM_SONG_ELEGY,
    /* 0x65 */ ITEM_SONG_OATH,
    /* 0x66 */ ITEM_SONG_SARIA,
    /* 0x67 */ ITEM_SONG_TIME,
    /* 0x68 */ ITEM_SONG_HEALING,
    /* 0x69 */ ITEM_SONG_EPONA,
    /* 0x6A */ ITEM_SONG_SOARING,
    /* 0x6B */ ITEM_SONG_STORMS,
    /* 0x6C */ ITEM_SONG_SUN,
    /* 0x6D */ ITEM_BOMBERS_NOTEBOOK,
    /* 0x6E */ ITEM_SKULL_TOKEN,
    /* 0x6F */ ITEM_HEART_CONTAINER,
    /* 0x70 */ ITEM_HEART_PIECE,
    /* 0x71 */ ITEM_71,
    /* 0x72 */ ITEM_72,
    /* 0x73 */ ITEM_SONG_LULLABY_INTRO,
    /* 0x74 */ ITEM_KEY_BOSS,
    /* 0x75 */ ITEM_COMPASS,
    /* 0x76 */ ITEM_DUNGEON_MAP,
    /* 0x77 */ ITEM_STRAY_FAIRIES,
    /* 0x78 */ ITEM_KEY_SMALL,
    /* 0x79 */ ITEM_MAGIC_JAR_SMALL,
    /* 0x7A */ ITEM_MAGIC_JAR_BIG,
    /* 0x7B */ ITEM_HEART_PIECE_2,
    /* 0x7C */ ITEM_INVALID_1,
    /* 0x7D */ ITEM_INVALID_2,
    /* 0x7E */ ITEM_INVALID_3,
    /* 0x7F */ ITEM_INVALID_4,
    /* 0x80 */ ITEM_INVALID_5,
    /* 0x81 */ ITEM_INVALID_6,
    /* 0x82 */ ITEM_INVALID_7,
    /* 0x83 */ ITEM_RECOVERY_HEART,
    /* 0x84 */ ITEM_RUPEE_GREEN,
    /* 0x85 */ ITEM_RUPEE_BLUE,
    /* 0x86 */ ITEM_RUPEE_10,
    /* 0x87 */ ITEM_RUPEE_RED,
    /* 0x88 */ ITEM_RUPEE_PURPLE,
    /* 0x89 */ ITEM_RUPEE_SILVER,
    /* 0x8A */ ITEM_RUPEE_HUGE,
    /* 0x8B */ ITEM_DEKU_STICKS_5,
    /* 0x8C */ ITEM_DEKU_STICKS_10,
    /* 0x8D */ ITEM_DEKU_NUTS_5,
    /* 0x8E */ ITEM_DEKU_NUTS_10,
    /* 0x8F */ ITEM_BOMBS_5,
    /* 0x90 */ ITEM_BOMBS_10,
    /* 0x91 */ ITEM_BOMBS_20,
    /* 0x92 */ ITEM_BOMBS_30,
    /* 0x93 */ ITEM_ARROWS_10,
    /* 0x94 */ ITEM_ARROWS_30,
    /* 0x95 */ ITEM_ARROWS_40,
    /* 0x96 */ ITEM_ARROWS_50,
    /* 0x97 */ ITEM_BOMBCHUS_20,
    /* 0x98 */ ITEM_BOMBCHUS_10,
    /* 0x99 */ ITEM_BOMBCHUS_1,
    /* 0x9A */ ITEM_BOMBCHUS_5,
    /* 0x9B */ ITEM_DEKU_STICK_UPGRADE_20,
    /* 0x9C */ ITEM_DEKU_STICK_UPGRADE_30,
    /* 0x9D */ ITEM_DEKU_NUT_UPGRADE_30,
    /* 0x9E */ ITEM_DEKU_NUT_UPGRADE_40,
    /* 0x9F */ ITEM_CHATEAU_2,
    /* 0xA0 */ ITEM_MILK,
    /* 0xA1 */ ITEM_GOLD_DUST_2,
    /* 0xA2 */ ITEM_HYLIAN_LOACH_2,
    /* 0xA3 */ ITEM_SEAHORSE_CAUGHT,
    // First entries of `MAP_POINT` must be continguous with `RegionId`
    /* 0xA4 */ ITEM_MAP_POINT_GREAT_BAY,
    /* 0xA5 */ ITEM_MAP_POINT_ZORA_HALL,
    /* 0xA6 */ ITEM_MAP_POINT_ROMANI_RANCH,
    /* 0xA7 */ ITEM_MAP_POINT_DEKU_PALACE,
    /* 0xA8 */ ITEM_MAP_POINT_WOODFALL,
    /* 0xA9 */ ITEM_MAP_POINT_CLOCK_TOWN,
    /* 0xAA */ ITEM_MAP_POINT_SNOWHEAD,
    /* 0xAB */ ITEM_MAP_POINT_IKANA_GRAVEYARD,
    /* 0xAC */ ITEM_MAP_POINT_IKANA_CANYON,
    /* 0xAD */ ITEM_MAP_POINT_GORON_VILLAGE,
    /* 0xAE */ ITEM_MAP_POINT_STONE_TOWER,
    // Remaining map points are unique to owl warps
    /* 0xAF */ ITEM_MAP_POINT_GREAT_BAY_COAST,
    /* 0xBO */ ITEM_MAP_POINT_SOUTHERN_SWAMP,
    /* 0xB1 */ ITEM_MAP_POINT_MOUNTAIN_VILLAGE,
    /* 0xB2 */ ITEM_MAP_POINT_MILK_ROAD,
    /* 0xB3 */ ITEM_MAP_POINT_ZORA_CAPE,
    /* 0xB8 */ ITEM_B8 = 0xB8,
    /* 0xB9 */ ITEM_B9,
    /* 0xBA */ ITEM_BA,
    /* 0xBB */ ITEM_BB,
    /* 0xBC */ ITEM_BC,
    /* 0xBD */ ITEM_BD,
    /* 0xBE */ ITEM_BE,
    /* 0xBF */ ITEM_BF,
    /* 0xC0 */ ITEM_C0,
    /* 0xC1 */ ITEM_C1,
    /* 0xC2 */ ITEM_C2,
    /* 0xC3 */ ITEM_C3,
    /* 0xC4 */ ITEM_C4,
    /* 0xC5 */ ITEM_C5,
    /* 0xC6 */ ITEM_C6,
    /* 0xC7 */ ITEM_C7,
    /* 0xC8 */ ITEM_C8,
    /* 0xC9 */ ITEM_C9,
    /* 0xCA */ ITEM_CA,
    /* 0xCB */ ITEM_CB,
    /* 0xCC */ ITEM_CC,
    /* 0xF0 */ ITEM_F0 = 0xF0, // PLAYER_MASK_BLAST
    /* 0xF1 */ ITEM_F1, // PLAYER_MASK_BREMEN
    /* 0xF2 */ ITEM_F2, // PLAYER_MASK_KAMARO
    /* 0xFC */ ITEM_FC = 0xFC,
    /* 0xFD */ ITEM_FD,
    /* 0xFE */ ITEM_FE,
    /* 0xFF */ ITEM_NONE = 0xFF
} ItemId;

#define BOTTLE_FIRST 0
#define BOTTLE_MAX 6

typedef enum GetItemId {
    /* 0x00 */ GI_NONE,
    /* 0x01 */ GI_RUPEE_GREEN,
    /* 0x02 */ GI_RUPEE_BLUE,
    /* 0x03 */ GI_RUPEE_10,
    /* 0x04 */ GI_RUPEE_RED,
    /* 0x05 */ GI_RUPEE_PURPLE,
    /* 0x06 */ GI_RUPEE_SILVER,
    /* 0x07 */ GI_RUPEE_HUGE,
    /* 0x08 */ GI_WALLET_ADULT,
    /* 0x09 */ GI_WALLET_GIANT,
    /* 0x0A */ GI_RECOVERY_HEART,
    /* 0x0B */ GI_0B,
    /* 0x0C */ GI_HEART_PIECE,
    /* 0x0D */ GI_HEART_CONTAINER,
    /* 0x0E */ GI_MAGIC_JAR_SMALL,
    /* 0x0F */ GI_MAGIC_JAR_BIG,
    /* 0x10 */ GI_10,
    /* 0x11 */ GI_STRAY_FAIRY,
    /* 0x12 */ GI_12,
    /* 0x13 */ GI_13,
    /* 0x14 */ GI_BOMBS_1,
    /* 0x15 */ GI_BOMBS_5,
    /* 0x16 */ GI_BOMBS_10,
    /* 0x17 */ GI_BOMBS_20,
    /* 0x18 */ GI_BOMBS_30,
    /* 0x19 */ GI_DEKU_STICKS_1,
    /* 0x1A */ GI_BOMBCHUS_10,
    /* 0x1B */ GI_BOMB_BAG_20,
    /* 0x1C */ GI_BOMB_BAG_30,
    /* 0x1D */ GI_BOMB_BAG_40,
    /* 0x1E */ GI_ARROWS_10,
    /* 0x1F */ GI_ARROWS_30,
    /* 0x20 */ GI_ARROWS_40,
    /* 0x21 */ GI_ARROWS_50,
    /* 0x22 */ GI_QUIVER_30,
    /* 0x23 */ GI_QUIVER_40,
    /* 0x24 */ GI_QUIVER_50,
    /* 0x25 */ GI_ARROW_FIRE,
    /* 0x26 */ GI_ARROW_ICE,
    /* 0x27 */ GI_ARROW_LIGHT,
    /* 0x28 */ GI_DEKU_NUTS_1,
    /* 0x29 */ GI_DEKU_NUTS_5,
    /* 0x2A */ GI_DEKU_NUTS_10,
    /* 0x2B */ GI_2B,
    /* 0x2C */ GI_2C,
    /* 0x2D */ GI_2D,
    /* 0x2E */ GI_BOMBCHUS_20,
    /* 0x2F */ GI_2F,
    /* 0x30 */ GI_30,
    /* 0x31 */ GI_31,
    /* 0x32 */ GI_SHIELD_HERO,
    /* 0x33 */ GI_SHIELD_MIRROR,
    /* 0x34 */ GI_POWDER_KEG,
    /* 0x35 */ GI_MAGIC_BEANS,
    /* 0x36 */ GI_BOMBCHUS_1,
    /* 0x37 */ GI_SWORD_KOKIRI,
    /* 0x38 */ GI_SWORD_RAZOR,
    /* 0x39 */ GI_SWORD_GILDED,
    /* 0x3A */ GI_BOMBCHUS_5,
    /* 0x3B */ GI_SWORD_GREAT_FAIRY,
    /* 0x3C */ GI_KEY_SMALL,
    /* 0x3D */ GI_KEY_BOSS,
    /* 0x3E */ GI_MAP,
    /* 0x3F */ GI_COMPASS,
    /* 0x40 */ GI_40,
    /* 0x41 */ GI_HOOKSHOT,
    /* 0x42 */ GI_LENS_OF_TRUTH,
    /* 0x43 */ GI_PICTOGRAPH_BOX,
    /* 0x44 */ GI_44,
    /* 0x45 */ GI_45,
    /* 0x46 */ GI_46,
    /* 0x47 */ GI_47,
    /* 0x48 */ GI_48,
    /* 0x49 */ GI_49,
    /* 0x4A */ GI_4A,
    /* 0x4B */ GI_4B,
    /* 0x4C */ GI_OCARINA_OF_TIME,
    /* 0x4D */ GI_4D,
    /* 0x4E */ GI_4E,
    /* 0x4F */ GI_4F,
    /* 0x50 */ GI_BOMBERS_NOTEBOOK,
    /* 0x51 */ GI_51,
    /* 0x52 */ GI_SKULL_TOKEN,
    /* 0x53 */ GI_53,
    /* 0x54 */ GI_54,
    /* 0x55 */ GI_REMAINS_ODOLWA,
    /* 0x56 */ GI_REMAINS_GOHT,
    /* 0x57 */ GI_REMAINS_GYORG,
    /* 0x58 */ GI_REMAINS_TWINMOLD,
    /* 0x59 */ GI_POTION_RED_BOTTLE, // ITEM_LONGSHOT
    /* 0x5A */ GI_BOTTLE,
    /* 0x5B */ GI_POTION_RED,
    /* 0x5C */ GI_POTION_GREEN,
    /* 0x5D */ GI_POTION_BLUE,
    /* 0x5E */ GI_FAIRY,
    /* 0x5F */ GI_DEKU_PRINCESS,
    /* 0x60 */ GI_MILK_BOTTLE,
    /* 0x61 */ GI_MILK_HALF,
    /* 0x62 */ GI_FISH,
    /* 0x63 */ GI_BUG,
    /* 0x64 */ GI_BLUE_FIRE,
    /* 0x65 */ GI_POE,
    /* 0x66 */ GI_BIG_POE,
    /* 0x67 */ GI_SPRING_WATER,
    /* 0x68 */ GI_HOT_SPRING_WATER,
    /* 0x69 */ GI_ZORA_EGG,
    /* 0x6A */ GI_GOLD_DUST,
    /* 0x6B */ GI_MUSHROOM,
    /* 0x6C */ GI_6C,
    /* 0x6D */ GI_6D,
    /* 0x6E */ GI_SEAHORSE,
    /* 0x6F */ GI_CHATEAU_BOTTLE,
    /* 0x70 */ GI_HYLIAN_LOACH,
    /* 0x71 */ GI_71,
    /* 0x72 */ GI_72,
    /* 0x73 */ GI_73,
    /* 0x74 */ GI_74,
    /* 0x75 */ GI_75,
    /* 0x76 */ GI_ICE_TRAP,
    /* 0x77 */ GI_77,
    /* 0x78 */ GI_MASK_DEKU,
    /* 0x79 */ GI_MASK_GORON,
    /* 0x7A */ GI_MASK_ZORA,
    /* 0x7B */ GI_MASK_FIERCE_DEITY,
    /* 0x7C */ GI_MASK_CAPTAIN,
    /* 0x7D */ GI_MASK_GIANT,
    /* 0x7E */ GI_MASK_ALL_NIGHT,
    /* 0x7F */ GI_MASK_BUNNY,
    /* 0x80 */ GI_MASK_KEATON,
    /* 0x81 */ GI_MASK_GARO,
    /* 0x82 */ GI_MASK_ROMANI,
    /* 0x83 */ GI_MASK_CIRCUS_LEADER,
    /* 0x84 */ GI_MASK_POSTMAN,
    /* 0x85 */ GI_MASK_COUPLE,
    /* 0x86 */ GI_MASK_GREAT_FAIRY,
    /* 0x87 */ GI_MASK_GIBDO,
    /* 0x88 */ GI_MASK_DON_GERO,
    /* 0x89 */ GI_MASK_KAMARO,
    /* 0x8A */ GI_MASK_TRUTH,
    /* 0x8B */ GI_MASK_STONE,
    /* 0x8C */ GI_MASK_BREMEN,
    /* 0x8D */ GI_MASK_BLAST,
    /* 0x8E */ GI_MASK_SCENTS,
    /* 0x8F */ GI_MASK_KAFEIS_MASK,
    /* 0x90 */ GI_90,
    /* 0x91 */ GI_CHATEAU,
    /* 0x92 */ GI_MILK,
    /* 0x93 */ GI_GOLD_DUST_2,
    /* 0x94 */ GI_HYLIAN_LOACH_2, // Unused bottled eel
    /* 0x95 */ GI_SEAHORSE_CAUGHT,
    /* 0x96 */ GI_MOONS_TEAR,
    /* 0x97 */ GI_DEED_LAND,
    /* 0x98 */ GI_DEED_SWAMP,
    /* 0x99 */ GI_DEED_MOUNTAIN,
    /* 0x9A */ GI_DEED_OCEAN,
    /* 0x9B */ GI_SWORD_GREAT_FAIRY_STOLEN,
    /* 0x9C */ GI_SWORD_KOKIRI_STOLEN,
    /* 0x9D */ GI_SWORD_RAZOR_STOLEN,
    /* 0x9E */ GI_SWORD_GILDED_STOLEN,
    /* 0x9F */ GI_SHIELD_HERO_STOLEN, // Code that treats this as hero's shield is unused, implied as intended to be stolen
    /* 0xA0 */ GI_ROOM_KEY,
    /* 0xA1 */ GI_LETTER_TO_MAMA,
    /* 0xA2 */ GI_A2,
    /* 0xA3 */ GI_A3,
    /* 0xA4 */ GI_A4,
    /* 0xA5 */ GI_A5,
    /* 0xA6 */ GI_A6,
    /* 0xA7 */ GI_A7,
    /* 0xA8 */ GI_A8,
    /* 0xA9 */ GI_BOTTLE_STOLEN, // buy back stolen bottle
    /* 0xAA */ GI_LETTER_TO_KAFEI,
    /* 0xAB */ GI_PENDANT_OF_MEMORIES,
    /* 0xAC */ GI_AC,
    /* 0xAD */ GI_AD,
    /* 0xAE */ GI_AE,
    /* 0xAF */ GI_AF,
    /* 0xB0 */ GI_B0,
    /* 0xB1 */ GI_B1,
    /* 0xB2 */ GI_B2,
    /* 0xB3 */ GI_B3,
    /* 0xB4 */ GI_TINGLE_MAP_CLOCK_TOWN,
    /* 0xB5 */ GI_TINGLE_MAP_WOODFALL,
    /* 0xB6 */ GI_TINGLE_MAP_SNOWHEAD,
    /* 0xB7 */ GI_TINGLE_MAP_ROMANI_RANCH,
    /* 0xB8 */ GI_TINGLE_MAP_GREAT_BAY,
    /* 0xB9 */ GI_TINGLE_MAP_STONE_TOWER,
    /* 0xBA */ GI_MAX
} GetItemId;

typedef enum EnGirlAShopItemId {
    /* 0x00 */ SI_POTION_RED_1,
    /* 0x01 */ SI_POTION_GREEN_1,
    /* 0x02 */ SI_POTION_BLUE,
    /* 0x03 */ SI_FAIRY_1,
    /* 0x04 */ SI_ARROWS_LARGE_1,
    /* 0x05 */ SI_POTION_GREEN_2,
    /* 0x06 */ SI_SHIELD_HERO_1,
    /* 0x07 */ SI_STICK_1,
    /* 0x08 */ SI_ARROWS_MEDIUM_1,
    /* 0x09 */ SI_NUTS_1,
    /* 0x0A */ SI_POTION_RED_2,
    /* 0x0B */ SI_FAIRY_2,
    /* 0x0C */ SI_ARROWS_MEDIUM_2,
    /* 0x0D */ SI_ARROWS_LARGE_2,
    /* 0x0E */ SI_POTION_GREEN_3,
    /* 0x0F */ SI_NUTS_2,
    /* 0x10 */ SI_STICK_2,
    /* 0x11 */ SI_SHIELD_HERO_2,
    /* 0x12 */ SI_POTION_RED_3,
    /* 0x13 */ SI_MASK_ALL_NIGHT,
    /* 0x14 */ SI_BOMB_BAG_20_1,
    /* 0x15 */ SI_BOMB_BAG_30_1,
    /* 0x16 */ SI_BOMB_BAG_40,
    /* 0x17 */ SI_BOMB_BAG_20_2,
    /* 0x18 */ SI_BOMB_BAG_30_2,
    /* 0x19 */ SI_BOMBCHU,
    /* 0x1A */ SI_BOMB_1,
    /* 0x1B */ SI_SHIELD_HERO_3,
    /* 0x1C */ SI_ARROWS_SMALL_1,
    /* 0x1D */ SI_POTION_RED_4,
    /* 0x1E */ SI_BOMB_2,
    /* 0x1F */ SI_ARROWS_SMALL_2,
    /* 0x20 */ SI_POTION_RED_5,
    /* 0x21 */ SI_BOMB_3,
    /* 0x22 */ SI_ARROWS_SMALL_3,
    /* 0x23 */ SI_POTION_RED_6,
    /* 0x24 */ SI_BOTTLE,
    /* 0x25 */ SI_SWORD_GREAT_FAIRY,
    /* 0x26 */ SI_SWORD_KOKIRI,
    /* 0x27 */ SI_SWORD_RAZOR,
    /* 0x28 */ SI_SWORD_GILDED,
    /* 0x29 */ SI_SHIELD_HERO_4,
    /* 0x2A */ SI_SHIELD_MIRROR,
    /* 0x2B */ SI_MAX
} EnGirlAShopItemId;

typedef uint64_t gpr;

typedef union {
    double d;
    struct {
        float fl;
        float fh;
    };
    struct {
        uint32_t u32l;
        uint32_t u32h;
    };
    uint64_t u64;
} fpr;

typedef struct {
    gpr r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,
        r8,  r9,  r10, r11, r12, r13, r14, r15,
        r16, r17, r18, r19, r20, r21, r22, r23,
        r24, r25, r26, r27, r28, r29, r30, r31;
    fpr f0,  f1,  f2,  f3,  f4,  f5,  f6,  f7,
        f8,  f9,  f10, f11, f12, f13, f14, f15,
        f16, f17, f18, f19, f20, f21, f22, f23,
        f24, f25, f26, f27, f28, f29, f30, f31;
    uint64_t hi, lo;
    uint32_t* f_odd;
    uint32_t status_reg;
    uint8_t mips3_float_mode;
} recomp_context;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

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
#define PTR(x) int32_t
#define RECOMP_DLL_C_FUNC(_f_name) DLLEXPORT void _f_name(uint8_t* rdram, recomp_context* ctx)
#define RECOMP_DLL_FUNC(_f_name) extern "C" RECOMP_DLL_C_FUNC(_f_name)
#define RECOMP_ARG(_type, _pos) _arg<_pos, _type>(rdram, ctx)
#define RECOMP_ARG_STR(_pos) _arg_string<_pos>(rdram, ctx)
#define RECOMP_ARG_U8STR(_pos) _arg_u8string<_pos>(rdram, ctx)
#define RECOMP_RETURN(_type, _value) _return(ctx, (_type) _value); return

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
};

template <int arg_index>
std::string _arg_string(uint8_t* rdram, recomp_context* ctx) {
    PTR(char) str = _arg<arg_index, PTR(char)>(rdram, ctx);

    // Get the length of the byteswapped string.
    size_t len = 0;
    while (MEM_B(str, len) != 0x00) {
        len++;
    }

    std::string ret{};
    ret.reserve(len + 1);

    for (size_t i = 0; i < len; i++) {
        ret += (char)MEM_B(str, i);
    }

    return ret;
}

template <int arg_index>
std::u8string _arg_u8string(uint8_t* rdram, recomp_context* ctx) {
    PTR(char) str = _arg<arg_index, PTR(char)>(rdram, ctx);

    // Get the length of the byteswapped string.
    size_t len = 0;
    while (MEM_B(str, len) != 0x00) {
        std::cout << MEM_B(str, len);
        len++;
    }

    std::u8string ret{};
    ret.reserve(len + 1);

    for (size_t i = 0; i < len; i++) {
        ret += (char)MEM_B(str, i);
    }

    return ret;
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
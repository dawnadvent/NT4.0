#ifndef _Stack_c_h
#define _Stack_c_h
#define STK_IS_PREDICTED (1)
#define STK_UNPREDICTED (0)
#define STK_WRITECHECK_ALWAYS (1)
#define STK_NO_EXTRA_CHECKS (0)
#define STK_SEPARATE_ODD (1)
#define STK_NO_SEPARATE_ODD (0)
#define STK_DELAY_COMMIT (1)
#define STK_DONT_DELAY_COMMIT (0)
#define STK_NEW_EIP_SUPPLIED (1)
#define STK_NO_EIP_SUPPLIED (0)
#define STK_NEW_ZONE (1)
#define STK_SAME_ZONE (0)
#define STK_BAIL_OUT (1)
#define STK_NO_BAIL_OUT (0)
#define STK_NO_UNI_CHECK (1)
#define STK_UNI_CHECK (0)
#define STK_NEW_STACK (1)
#define STK_LIVE_STACK (0)
#define STK_USE32 (1)
#define STK_USE16 (0)
#define STK_MAX_SMALL_PUPO_CONST (63)
#define STK_SMALL_PU_START_BIT (11)
#define STK_SMALL_PU_END_BIT (6)
#define STK_SMALL_PO_START_BIT (5)
#define STK_SMALL_PO_END_BIT (0)
#define STK_MAX_LARGE_PUPO_CONST (65535)
#define STK_LARGE_PU_START_BIT (31)
#define STK_LARGE_PU_END_BIT (16)
#define STK_LARGE_PO_START_BIT (15)
#define STK_LARGE_PO_END_BIT (0)
#define STK16_ITEM_SZ (2)
#define STK32_ITEM_SZ (4)
#define STK16_SLOT8 (-8)
#define STK16_SLOT7 (-7)
#define STK16_SLOT6 (-6)
#define STK16_SLOT5 (-5)
#define STK16_SLOT4 (-4)
#define STK16_SLOT3 (-3)
#define STK16_SLOT2 (-2)
#define STK16_SLOT1 (-1)
#define STK16_ITEM1 (0)
#define STK16_ITEM2 (1)
#define STK16_ITEM3 (2)
#define STK16_ITEM4 (3)
#define STK16_ITEM5 (4)
#define STK16_ITEM6 (5)
#define STK16_ITEM7 (6)
#define STK16_ITEM8 (7)
#define STK32_SLOT8 (-8)
#define STK32_SLOT7 (-7)
#define STK32_SLOT6 (-6)
#define STK32_SLOT5 (-5)
#define STK32_SLOT4 (-4)
#define STK32_SLOT3 (-3)
#define STK32_SLOT2 (-2)
#define STK32_SLOT1 (-1)
#define STK32_ITEM1 (0)
#define STK32_ITEM2 (1)
#define STK32_ITEM3 (2)
#define STK32_ITEM4 (3)
#define STK32_ITEM5 (4)
#define STK32_ITEM6 (5)
#define STK32_ITEM7 (6)
#define STK32_ITEM8 (7)
#define MAX_HBP_OFFSET (16)
#define MIN_HBP_OFFSET (-32)
#define HBP_MAX_OPND_SIZE (6)
enum IronRequestType
{
	IRON_STACK = 0,
	IRON_STRUCT = 1
};
#endif /* ! _Stack_c_h */

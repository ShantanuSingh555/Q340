/*
 * Module Author : Yuvraj Saxena (abyss10@github.com)
 * Date : 03/06/2018
 */
#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(480)
#define FRAME_HEIGHT 										(854)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF
#define LCM_ID       (0x0604)


#define LCM_DSI_CMD_MODE									0

#ifndef TRUE
    #define   TRUE     1
#endif
 
#ifndef FALSE
    #define   FALSE    0
#endif
	
#ifdef BUILD_LK
#define LCM_PRINT printf
#else
#if defined(BUILD_UBOOT)
	#define LCM_PRINT printf
#else
	#define LCM_PRINT printk
#endif
#endif

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (854)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))

static unsigned int lcm_compare_id(void);
// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)										lcm_util.dsi_read_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
 
 struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] = 
{
{0xff, 5, {0xff,0x98,0x06,0x04,0x01}},
{0x08, 1, {0x10}},
{0x21, 1, {0x01}},
{0x30, 1, {0x01}},
{0x31, 1, {0x02}},
{0x40, 1, {0x14}},
{0x41, 1, {0x22}},
{0x42, 1, {0x01}},
{0x43, 1, {0x85}},
{0x44, 1, {0x89}},
{0x45, 1, {0x02}},
{0x50, 1, {0x68}},
{0x51, 1, {0x68}},
{0x52, 1, {0x00}},
{0x53, 1, {0x51}},
{0x57, 1, {0x50}},
{0x60, 1, {0x04}},
{0x61, 1, {0x06}},
{0x62, 1, {0x07}},
{0x63, 1, {0x00}},
{0xa0, 1, {0x00}},
{0xa1, 1, {0x07}},
{0xa2, 1, {0x0d}},
{0xa3, 1, {0x0b}},
{0xa4, 1, {0x02}},
{0xa5, 1, {0x08}},
{0xa6, 1, {0x07}},
{0xa7, 1, {0x05}},
{0xa8, 1, {0x07}},
{0xa9, 1, {0x0b}},
{0xaa, 1, {0x11}},
{0xab, 1, {0x0a}},
{0xac, 1, {0x0e}},
{0xad, 1, {0x1a}},
{0xae, 1, {0x12}},
{0xaf, 1, {0x00}},
{0xc0, 1, {0x00}},
{0xc1, 1, {0x07}},
{0xc2, 1, {0x0d}},
{0xc3, 1, {0x0b}},
{0xc4, 1, {0x02}},
{0xc5, 1, {0x08}},
{0xc6, 1, {0x07}},
{0xc7, 1, {0x05}},
{0xc8, 1, {0x07}},
{0xc9, 1, {0x0b}},
{0xca, 1, {0x11}},
{0xcb, 1, {0x0a}},
{0xcc, 1, {0x0e}},
{0xcd, 1, {0x1a}},
{0xce, 1, {0x12}},
{0xcf, 1, {0x00}},
{0xff, 5, {0xff,0x98,0x06,0x04,0x06}},
{0x00, 1, {0x20}},
{0x01, 1, {0x05}},
{0x02, 1, {0x00}},
{0x03, 1, {0x00}},
{0x04, 1, {0x01}},
{0x05, 1, {0x01}},
{0x06, 1, {0x88}},
{0x07, 1, {0x04}},
{0x08, 1, {0x01}},
{0x09, 1, {0x90}},
{0x0a, 1, {0x04}},
{0x0b, 1, {0x01}},
{0x0c, 1, {0x01}},
{0x0d, 1, {0x01}},
{0x0e, 1, {0x00}},
{0x0f, 1, {0x00}},
{0x10, 1, {0x55}},
{0x11, 1, {0x50}},
{0x12, 1, {0x01}},
{0x13, 1, {0x0c}},
{0x14, 1, {0x0d}},
{0x15, 1, {0x43}},
{0x16, 1, {0x0b}},
{0x17, 1, {0x00}},
{0x18, 1, {0x00}},
{0x19, 1, {0x00}},
{0x1a, 1, {0x00}},
{0x1b, 1, {0x00}},
{0x1c, 1, {0x00}},
{0x1d, 1, {0x00}},
{0x20, 1, {0x01}},
{0x21, 1, {0x23}},
{0x22, 1, {0x45}},
{0x23, 1, {0x67}},
{0x24, 1, {0x01}},
{0x25, 1, {0x23}},
{0x26, 1, {0x45}},
{0x27, 1, {0x67}},
{0x30, 1, {0x02}},
{0x31, 1, {0x22}},
{0x32, 1, {0x11}},
{0x33, 1, {0xaa}},
{0x34, 1, {0xbb}},
{0x35, 1, {0x66}},
{0x36, 1, {0x00}},
{0x37, 1, {0x22}},
{0x38, 1, {0x22}},
{0x39, 1, {0x22}},
{0x3a, 1, {0x22}},
{0x3b, 1, {0x22}},
{0x3c, 1, {0x22}},
{0x3d, 1, {0x22}},
{0x3e, 1, {0x22}},
{0x3f, 1, {0x22}},
{0x40, 1, {0x22}},
{0x52, 1, {0x10}},
{0x53, 1, {0x12}},
{0xff, 5, {0xff,0x98,0x06,0x04,0x07}},
{0x17, 1, {0x32}},
{0x02, 1, {0x77}},
{0xe1, 1, {0x79}},
{0xff, 5, {0xff,0x98,0x06,0x04,0x00}},
{0x11, 1, {0x00}},
{REGFLAG_DELAY, 120, {}},
{0x29, 1, {0x00}},
{REGFLAG_DELAY, 10, {}},
{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	{0xff, 5, {0xff,0x98,0x06,0x04,0x00}},
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY,50,{}},
	{0x10, 1, {0x00}},
        {REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
		        dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->dsi.word_count = 1440;
	params->dsi.vertical_sync_active = 4;
        params->dsi.vertical_backporch = 16;                    params->dsi.vertical_frontporch = 20;
	params->dsi.horizontal_sync_active = 10;
	params->dsi.horizontal_blanking_pixel = 120;
	params->type = 2;
	params->dsi.fbk_div = 13;
	params->dsi.LANE_NUM = 2;
	params->dsi.data_format.format = 2;
	params->dsi.intermediat_buffer_num = 2;
	params->dsi.PS = 2;
        params->width  = 480;
	params->dsi.horizontal_active_pixel = 480;
        params->height = 854;
	params->dsi.vertical_active_line = 854;
	params->dbi.te_mode = 1;
	params->dsi.mode = 1;
        params->dsi.pll_div1 = 1;
        params->dsi.fbk_sel = 1;
	params->dbi.te_edge_polarity = 0;
        params->dsi.data_format.color_order = 0;                params->dsi.data_format.trans_seq = 0;                  params->dsi.data_format.padding = 0;
	params->dsi.pll_div2 = 0;
        params->dsi.horizontal_backporch = 50;                  params->dsi.horizontal_frontporch = 50;
}

static void lcm_init(void)
{
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(150);
    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
    push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_resume(void)
{
	lcm_init();
}

static unsigned int lcm_compare_id(void)
{
	return 1;
}

LCM_DRIVER ili9806e_fwvga_dsi_vdo_txd_aw619_lcm_drv = 
{
        .name		= "ili9806e_fwvga_dsi_vdo_txd_aw619",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,	
};

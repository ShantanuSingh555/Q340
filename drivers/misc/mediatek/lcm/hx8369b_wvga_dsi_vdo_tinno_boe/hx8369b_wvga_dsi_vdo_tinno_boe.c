/*
 * Added By Yuvraj on 2018
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


#ifdef BUILD_LK
#define LCM_PRINT printf
#else
#define LCM_PRINT printk
#endif


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)

static unsigned int lcm_esd_test = FALSE;

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))
#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0x00

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)									lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)				lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)      

//#define LCM_DSI_CMD_MODE

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
{0xB9,3,{0xFF,0x83,0x69}}, 
{0xBA,15,{0x31,0x00,0x16,0xCA,0xB0,0x0A,0x00,0x10,0x28,0x02,0x21,0x21,0x9A,0x1A,0x8F}},
{0x3A,1,{0x70}},
{0xD5,59,{0x00,0x00,0x01,0x00,0x03,0x00,0x00,0x18,0x01,0x00,0x00,0x00,0x01,0x60,0x37,0x00,0x00,0x03,0x07,0x08,0x47,0x00,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x14,0x00,0x00,0x91,0x13,0x35,0x57,0x75,0x18,0x00,0x00,0x00,0x86,0x64,0x42,0x20,0x00,0x49,0x00,0x00,0x00,0x90,0x02,0x24}},
{0xFD,32,{0x24,0x46,0x64,0x08,0x00,0x00,0x00,0x87,0x75,0x53,0x31,0x11,0x59,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x0F,0x00,0x0F,0xFF,0xFF,0x0F,0x00,0x0F,0xFF,0xFF,0x00,0x85,0x5A}},
{0xB1,10 ,{0x0B, 0x83 ,0x77 ,0x00, 0x0F, 0x0F, 0x17 ,0x17, 0x0C, 0x2A}},
{0xB3,4,{0x83,0x00,0x31,0x03}},
{0xB4,1,{0x00}},
{0xB6,2,{0xA4,0xA4}},
{0xCC,1,{0x0E}},
{0xC1,1,{0x00}},
{0xC6,4,{0x41 ,0xFF ,0x7A, 0xFF}},
{0xEA,1,{0x72}},
{0xE3, 4,{0x07, 0x0F, 0x07, 0x0F}},
{0xC0, 6,{0x73, 0x50, 0x00, 0x34, 0xC4, 0x09}},
{0xE0,35,{0x01 ,0x0E, 0x12, 0x29, 0x2D, 0x30, 0x1E, 0x3B, 0x08, 0x0D, 0x0F, 0x13, 0x15, 0x13 ,0x14, 0x10, 0x16, 0x01 ,0x0E, 0x11, 0x2A, 0x2D, 0x30, 0x1D, 0x3B, 0x08, 0x0D, 0x0F, 0x13, 0x15, 0x13 ,0x14, 0x0F, 0x16, 0x01}},
{0X34,1,{0X00}},
{0x11,0,{}},
{REGFLAG_DELAY, 120, {}},
{0x29,0,{}},
{REGFLAG_DELAY, 10, {}},
{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_compare_id_setting[] = {
	{0xB9,	3,	{0xFF, 0x83, 0x69}},
	{REGFLAG_DELAY, 10, {}},
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


static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
	
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		params->dsi.packet_size=256;

		params->dsi.intermediat_buffer_num = 2;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.word_count=480*3;

		params->dsi.vertical_sync_active		      = 7;
		params->dsi.vertical_backporch		      = 5;
		params->dsi.vertical_frontporch		      = 12;
		params->dsi.vertical_active_line		      = FRAME_HEIGHT;
		
		params->dsi.horizontal_sync_active 	      = 10;
		params->dsi.horizontal_backporch		      = 70;
		params->dsi.horizontal_frontporch		      = 90;
		params->dsi.horizontal_active_pixel	      = FRAME_WIDTH;
		params->dsi.ssc_disable = 1;
		params->dsi.PLL_CLOCK=215;

}


static void lcm_init(void)
{
	
    SET_RESET_PIN(0);
    MDELAY(25);
    SET_RESET_PIN(1);
    MDELAY(100);
    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

}

extern void DSI_clk_ULP_mode(bool enter);

static void lcm_suspend(void)
{

    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(20);
    DSI_clk_ULP_mode(1);
    MDELAY(100);
	
}

static void lcm_resume(void)
{	unsigned int data_array[16];
	lcm_init();
}

// ---------------------------------------------------------------------------
//  Get LCM ID Information
// ---------------------------------------------------------------------------
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);
static unsigned int lcm_compare_id()
    {
        unsigned int id = 0;
        unsigned char buffer[2];
        unsigned int array[16];
        int volt = 0;
        unsigned int data[4];

        SET_RESET_PIN(1);
        SET_RESET_PIN(0);
        MDELAY(10);
        SET_RESET_PIN(1);
        MDELAY(10);
    
        push_table(lcm_compare_id_setting, sizeof(lcm_compare_id_setting) / sizeof(struct LCM_setting_table), 1);
    
        array[0] = 0x00023700;
        dsi_set_cmdq(array, 1, 1);
        read_reg_v2(0xF4, buffer, 2);
        id = buffer[0];
        
        #ifdef BUILD_LK
           printf("%s, zenghaihui id1 = 0x%08x\n", __func__, id); 
        #else
           printk("%s, zenghaihui id1 = 0x%08x\n", __func__, id);   
        #endif

       if(1)
       {
               IMM_GetOneChannelValue(0, data, &volt);
        #ifndef BUILD_LK
                    printk(" lcm_compare_id tcl_oncell zenghaihui    volt = %d ", volt);
        #else
                    printf(" lcm_compare_id tcl_onclee zenghaihui   volt = %d ", volt);
        #endif
          
                if(volt >= 1900)
                {
	        #ifndef BUILD_LK
	                    printk(" lcm_compare_id:hx8369_wvga_dsi_vdo_tinno_boe \n");
	        #else
	                    printf(" lcm_compare_id:hx8369_wvga_dsi_vdo_tinno_boe \n");
	        #endif
                     return 1;
                }
                else
                {
                    return 0;
                }
        }
        else
        {
            return 0;
        }
        
}

LCM_DRIVER hx8369b_wvga_dsi_vdo_tinno_boe_drv = 
{
        .name		= "hx8369_wvga_dsi_vdo_tinno_boe",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
};


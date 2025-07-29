#include "app_config.h"

#include "system/includes.h"
#include "device/includes.h"
#include "asm/includes.h"
#include "server/audio_dev.h"

// *INDENT-OFF*


const struct rtc_init_config rtc_init_data = {
	.rtc_clk_sel = RTC_CLK_SEL_INSIDE,
	.rtc_power_sel = RTCVDD_SUPPLY_OUTSIDE,
};


//*********************************************************************************//
//                                   按键模块配置                                    //
//*********************************************************************************//
#if TCFG_ADKEY_ENABLE
#define ADKEY_UPLOAD_R  22
#define ADC_VDDIO (0x3FF)
#define ADC_09   (0x3FF)
#define ADC_08   (0x3FF)
#define ADC_07   (0x3FF * 150 / (150 + ADKEY_UPLOAD_R))
#define ADC_06   (0x3FF * 62  / (62  + ADKEY_UPLOAD_R))
#define ADC_05   (0x3FF * 36  / (36  + ADKEY_UPLOAD_R))
#define ADC_04   (0x3FF * 22  / (22  + ADKEY_UPLOAD_R))
#define ADC_03   (0x3FF * 13  / (13  + ADKEY_UPLOAD_R))
#define ADC_02   (0x3FF * 75  / (75  + ADKEY_UPLOAD_R * 10))
#define ADC_01   (0x3FF * 3   / (3   + ADKEY_UPLOAD_R))
#define ADC_00   (0)
#define ADKEY_V_9      	((ADC_09 + ADC_VDDIO)/2)
#define ADKEY_V_8 		((ADC_08 + ADC_09)/2)
#define ADKEY_V_7 		((ADC_07 + ADC_08)/2 + 50)
#define ADKEY_V_6 		((ADC_06 + ADC_07)/2)
#define ADKEY_V_5 		((ADC_05 + ADC_06)/2)
#define ADKEY_V_4 		((ADC_04 + ADC_05)/2)
#define ADKEY_V_3 		((ADC_03 + ADC_04)/2)
#define ADKEY_V_2 		((ADC_02 + ADC_03)/2)
#define ADKEY_V_1 		((ADC_01 + ADC_02)/2)
#define ADKEY_V_0 		((ADC_00 + ADC_01)/2)
const struct adkey_platform_data adkey_data = {
    .enable     = 1,
    .adkey_pin  = IO_PORTB_01,
    .extern_up_en = 1,
    .ad_channel = 3,
    .ad_value = {
        ADKEY_V_0,//122   //0~122
        ADKEY_V_1,//192   //122~192
        ADKEY_V_2,//319   //192~319
        ADKEY_V_3,//445   //319~445
        ADKEY_V_4,//572   //445~572
        ADKEY_V_5,//694   //572~694
        ADKEY_V_6,//823   //694~823
        ADKEY_V_7,//1007  //823~1007
        ADKEY_V_8,//1023  //1007~1023
        ADKEY_V_9,//1023  //1023
    },
    .key_value = {
        KEY_POWER,       /*0*/
        KEY_ENC,         /*1*/
        KEY_PHOTO,       /*2*/
        KEY_OK,          /*3*/
        KEY_VOLUME_INC,  /*4*/
        KEY_VOLUME_DEC,  /*5*/
        KEY_MODE,        /*6*/
        KEY_CANCLE,      /*7*/
	    NO_KEY,
	    NO_KEY,
    },
};
#endif // TCFG_ADKEY_ENABLE


//**********************************END*******************************************//

//*********************************************************************************//
//                                WIFI,蓝牙模块配置                                //
//*********************************************************************************//
#if defined CONFIG_BT_ENABLE || defined CONFIG_WIFI_ENABLE
#include "wifi/wifi_connect.h"
const struct wifi_calibration_param wifi_calibration_param = {
    .xosc_l     = 0xb,// 调节左晶振电容
    .xosc_r     = 0xb,// 调节右晶振电容
    .pa_trim_data  ={1, 7, 4, 7, 11, 1, 7},// 根据MP测试生成PA TRIM值
	.mcs_dgain     ={
		45,//11B_1M
	  	45,//11B_2.2M
	  	45,//11B_5.5M
		42,//11B_11M

		60,//11G_6M
		60,//11G_9M
		75,//11G_12M
		70,//11G_18M
		62,//11G_24M
		52,//11G_36M
		50,//11G_48M
		38,//11G_54M

		62,//11N_MCS0
		80,//11N_MCS1
		70,//11N_MCS2
		62,//11N_MCS3
		50,//11N_MCS4
		48,//11N_MCS5
		40,//11N_MCS6
		36,//11N_MCS7
	}
};
#endif
//**********************************END*******************************************//

//*********************************************************************************//
//                            音频模块配置                                         //
//*********************************************************************************//
static const struct dac_platform_data dac_data = {
    .sw_differ = 0,
    .pa_auto_mute = 1,
    .pa_mute_port = IO_PORTB_02,
    .pa_mute_value = 0,
    .differ_output = 0,
    .hw_channel = 0x05,
    .ch_num = 4,
    .vcm_init_delay_ms = 1000,
	.mute_delay_ms = 200,
#ifdef CONFIG_DEC_ANALOG_VOLUME_ENABLE
    .fade_enable = 1,
    .fade_delay_ms = 40,
#endif
#ifdef CONFIG_AEC_USE_PLAY_MUSIC_ENABLE
    .sr_points = 960,
#endif
};
static const struct adc_platform_data adc_data = {
#ifdef CONFIG_ALL_ADC_CHANNEL_OPEN_ENABLE
    .mic_channel = (LADC_CH_MIC1_P_N | LADC_CH_MIC3_P_N),
    .linein_channel = (LADC_CH_AUX1 | LADC_CH_AUX3),
    .mic_ch_num = 2,
    .linein_ch_num = 2,
    .all_channel_open = 1,
#else
    .mic_channel = LADC_CH_MIC1_P_N,
    .mic_ch_num = 1,
#endif
    .isel = 2,
    .dump_num = 480,
};
static const struct iis_platform_data iis0_data = {
    .channel_in = BIT(1),
    .channel_out = BIT(0),
    .port_sel = IIS_PORTA,
    .data_width = 0,
    .mclk_output = 0,
    .slave_mode = 0,
    .dump_points_num = 320,
};
static const struct iis_platform_data iis1_data = {
    .channel_in = BIT(0),
    .channel_out = BIT(1),
    .port_sel = IIS_PORTA,
    .data_width = 0,
    .mclk_output = 0,
    .slave_mode = 0,
    .dump_points_num = 320,
};
static void plnk0_port_remap_cb(void)
{
    //重映射PDM DAT-PH2   PDM CLK-PH3
    extern int gpio_plnk_rx_input(u32 gpio, u8 index, u8 data_sel);
    gpio_plnk_rx_input(IO_PORTH_02, 0, 0);
    gpio_output_channle(IO_PORTH_03, CH0_PLNK0_SCLK_OUT);	//SCLK0使用outputchannel0
    JL_IOMAP->CON3 |= BIT(18);
}
static void plnk0_port_unremap_cb(void)
{
    JL_IOMAP->CON3 &= ~BIT(18);
    gpio_clear_output_channle(IO_PORTH_03, CH0_PLNK0_SCLK_OUT);
    gpio_set_die(IO_PORTH_02, 0);
}
//plnk的时钟和数据引脚都采用重映射的使用例子
static const struct plnk_platform_data plnk0_data = {
    .hw_channel = PLNK_CH_MIC_L,
    .clk_out = 1,
    .port_remap_cb = plnk0_port_remap_cb,
    .port_unremap_cb = plnk0_port_unremap_cb,
    .sample_edge = 0,   //在CLK的下降沿采样左MIC
    .share_data_io = 1, //两个数字MIC共用一个DAT脚
    .high_gain = 1,
    .dc_cancelling_filter = 14,
    .dump_points_num = 640, //丢弃刚打开硬件时的数据点数
};
static const struct plnk_platform_data plnk1_data = {
    .hw_channel = PLNK_CH_MIC_DOUBLE,
    .clk_out = 1,
    .dc_cancelling_filter = 14,
    .dump_points_num = 640, //丢弃刚打开硬件时的数据点数
};
static const struct audio_pf_data audio_pf_d = {
    .adc_pf_data = &adc_data,
    .dac_pf_data = &dac_data,
    .iis0_pf_data = &iis0_data,
    .iis1_pf_data = &iis1_data,
    .plnk0_pf_data = &plnk0_data,
    .plnk1_pf_data = &plnk1_data,
};
static const struct audio_platform_data audio_data = {
    .private_data = (void *) &audio_pf_d,
};
//**********************************END*******************************************//


UART2_PLATFORM_DATA_BEGIN(uart2_data)
	.baudrate = 1000000,
	.port = PORT_REMAP,
	.output_channel = OUTPUT_CHANNEL0,
	.tx_pin = IO_PORTB_03,
	.rx_pin = -1,
	.max_continue_recv_cnt = 1024,
	.idle_sys_clk_cnt = 500000,
	.clk_src = PLL_48M,
	.flags = UART_DEBUG,
UART2_PLATFORM_DATA_END();


/************************** LOW POWER config ****************************/
static const struct low_power_param power_param = {
    .config         = TCFG_LOWPOWER_LOWPOWER_SEL,
    .btosc_disable  = TCFG_LOWPOWER_BTOSC_DISABLE,         //进入低功耗时BTOSC是否保持
    .vddiom_lev     = TCFG_LOWPOWER_VDDIOM_LEVEL,          //强VDDIO等级,可选：2.2V  2.4V  2.6V  2.8V  3.0V  3.2V  3.4V  3.6V
    .vddiow_lev     = TCFG_LOWPOWER_VDDIOW_LEVEL,          //弱VDDIO等级,可选：2.1V  2.4V  2.8V  3.2V
    .vdc14_dcdc 	= TRUE,	   							   //打开内部1.4VDCDC，关闭则用外部
    .vdc14_lev		= VDC14_VOL_SEL_LEVEL, 				   //VDD1.4V配置
    .sysvdd_lev		= SYSVDD_VOL_SEL_LEVEL,				   //内核、sdram电压配置
	.vlvd_enable	= TRUE,                                //TRUE电压复位使能
    .vlvd_value		= VLVD_SEL_25V,                        //低电压复位电压值
};

/************************** PWR config ****************************/
//#define PORT_VCC33_CTRL_IO		IO_PORTA_03					//VCC33 DCDC控制引脚,该引脚控制DCDC器件输出的3.3V连接芯片HPVDD、VDDIO、VDD33
#define PORT_WAKEUP_IO			IO_PORTB_01					//软关机和休眠唤醒引脚
#define PORT_WAKEUP_NUM			(PORT_WAKEUP_IO/IO_GROUP_NUM)//默认:0-7:GPIOA-GPIOH, 可以指定0-7组

static const struct port_wakeup port0 = {
    .edge       = FALLING_EDGE,                            //唤醒方式选择,可选：上升沿\下降沿
    .attribute  = BLUETOOTH_RESUME,                        //保留参数
    .iomap      = PORT_WAKEUP_IO,                          //唤醒口选择
    .low_power	= POWER_SLEEP_WAKEUP|POWER_OFF_WAKEUP,    //低功耗IO唤醒,不需要写0
};

static const struct long_press lpres_port = {
    .enable 	= FALSE,
    .use_sec4 	= TRUE,										//enable = TRUE , use_sec4: TRUE --> 4 sec , FALSE --> 8 sec
    .edge		= FALLING_EDGE,								//长按方式,可选：FALLING_EDGE /  RISING_EDGE --> 低电平/高电平
    .iomap 		= PORT_WAKEUP_IO,							//长按复位IO和IO唤醒共用一个IO
};

static const struct sub_wakeup sub_wkup = {
    .attribute  = BLUETOOTH_RESUME,
};

static const struct charge_wakeup charge_wkup = {
    .attribute  = BLUETOOTH_RESUME,
};

static const struct wakeup_param wk_param = {
    .port[PORT_WAKEUP_NUM] = &port0,
    .sub = &sub_wkup,
    .charge = &charge_wkup,
    .lpres = &lpres_port,
};


REGISTER_DEVICES(device_table) = {
	{"uart2", &uart_dev_ops, (void *)&uart2_data },
	{"rtc", &rtc_dev_ops, NULL},
	{"audio", &audio_dev_ops, (void *)&audio_data },
};

#ifdef CONFIG_DEBUG_ENABLE
void debug_uart_init()
{
    uart_init(&uart2_data);
}
#endif

void board_early_init()
{
	dac_early_init(0, dac_data.differ_output ? (dac_data.ch_num > 1 ? 0xf : 0x3): dac_data.hw_channel, 1000);
    devices_init();
}

static void board_power_init(void)
{
// #ifdef PORT_VCC33_CTRL_IO
// 	gpio_direction_output(PORT_VCC33_CTRL_IO, 1);
//     gpio_set_pull_up(PORT_VCC33_CTRL_IO, 1);
// 	gpio_set_pull_down(PORT_VCC33_CTRL_IO,0);
// #endif

    power_init(&power_param);

    power_keep_state(POWER_KEEP_RESET | POWER_KEEP_DACVDD);

    power_keep_state(POWER_KEEP_RTC);//0, POWER_KEEP_DACVDD | POWER_KEEP_RTC | POWER_KEEP_RESET
	/* power_keep_state(POWER_KEEP_PWM_LED);//0, POWER_KEEP_DACVDD | POWER_KEEP_RTC | POWER_KEEP_RESET */

    power_wakeup_init(&wk_param);

    extern void clean_wakeup_source_port(void);
    clean_wakeup_source_port();
}

void board_init()
{
	board_power_init();
    rtc_early_init();
    adc_init();
    key_driver_init();
	void cfg_file_parse(void);
	cfg_file_parse();
}

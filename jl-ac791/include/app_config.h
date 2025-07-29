#ifndef APP_CONFIG_H
#define APP_CONFIG_H



#define __FLASH_SIZE__    (4 * 1024 * 1024)
#define __SDRAM_SIZE__    (2 * 1024 * 1024)

#define TCFG_ADKEY_ENABLE             1      //AD按键

// #define RTOS_STACK_CHECK_ENABLE
// #define MEM_LEAK_CHECK_ENABLE


//*********************************************************************************//
//                            音频模块配置                                         //
//*********************************************************************************//
//编解码器使能
// #define CONFIG_JLA_ENC_ENABLE
// #define CONFIG_JLA_DEC_ENABLE
#define CONFIG_PCM_DEC_ENABLE
#define CONFIG_PCM_ENC_ENABLE
// #define CONFIG_DTS_DEC_ENABLE
// #define CONFIG_ADPCM_DEC_ENABLE
// #define CONFIG_MP3_DEC_ENABLE
// #define CONFIG_MP3_ENC_ENABLE
// #define CONFIG_WMA_DEC_ENABLE
// #define CONFIG_M4A_DEC_ENABLE
// #define CONFIG_WAV_DEC_ENABLE
// #define CONFIG_AMR_DEC_ENABLE
// #define CONFIG_APE_DEC_ENABLE
// #define CONFIG_FLAC_DEC_ENABLE
// #define CONFIG_SPEEX_DEC_ENABLE
// #define CONFIG_ADPCM_ENC_ENABLE
// #define CONFIG_WAV_ENC_ENABLE
// #define CONFIG_VAD_ENC_ENABLE
// #define CONFIG_VIRTUAL_DEV_ENC_ENABLE
// #define CONFIG_OPUS_ENC_ENABLE
// #define CONFIG_OPUS_DEC_ENABLE
// #define CONFIG_SPEEX_ENC_ENABLE
// #define CONFIG_AMR_ENC_ENABLE
#define CONFIG_AEC_ENC_ENABLE
// #define CONFIG_DNS_ENC_ENABLE
// #define CONFIG_AAC_ENC_ENABLE
// #define CONFIG_AAC_DEC_ENABLE
// #define CONFIG_SBC_DEC_ENABLE
// #define CONFIG_SBC_ENC_ENABLE
// #define CONFIG_MSBC_DEC_ENABLE
// #define CONFIG_MSBC_ENC_ENABLE
// #define CONFIG_CVSD_DEC_ENABLE
// #define CONFIG_CVSD_ENC_ENABLE
// #define CONFIG_NEW_M4A_DEC_ENABLE

// #define CONFIG_AEC_USE_PLAY_MUSIC_ENABLE
// #define CONFIG_USB_AUDIO_AEC_ENABLE
// #define CONFIG_AEC_LINEIN_CHANNEL_ENABLE     //AEC回采使用硬件通道数据

#define AUDIO_ENC_SAMPLE_SOURCE_MIC         0  //录音输入源：MIC
#define AUDIO_ENC_SAMPLE_SOURCE_PLNK0       1  //录音输入源：数字麦PLNK0
#define AUDIO_ENC_SAMPLE_SOURCE_PLNK1       2  //录音输入源：数字麦PLNK1
#define AUDIO_ENC_SAMPLE_SOURCE_IIS0        3  //录音输入源：IIS0
#define AUDIO_ENC_SAMPLE_SOURCE_IIS1        4  //录音输入源：IIS1
#define AUDIO_ENC_SAMPLE_SOURCE_LINEIN      5  //录音输入源：LINEIN

#define CONFIG_AUDIO_ENC_SAMPLE_SOURCE      AUDIO_ENC_SAMPLE_SOURCE_MIC    //录音输入源选择
#define CONFIG_AUDIO_DEC_PLAY_SOURCE        "dac"                          //播放输出源选择
#define CONFIG_AUDIO_RECORDER_SAMPLERATE    16000                          //录音采样率
#define CONFIG_AUDIO_RECORDER_CHANNEL       1                              //录音通道数
#define CONFIG_AUDIO_RECORDER_DURATION      (30 * 1000)                    //录音时长ms

#define CONFIG_ALL_ADC_CHANNEL_OPEN_ENABLE              //四路ADC硬件全开
#define CONFIG_AUDIO_ADC_CHANNEL_L          	  1     //左mic通道
#define CONFIG_AUDIO_ADC_CHANNEL_R          	  3     //右mic通道
#define CONFIG_PHONE_CALL_ADC_CHANNEL       	  1     //通话mic通道
#define CONFIG_AEC_ADC_CHANNEL              	  0     //回采通道
#define CONFIG_AEC_AUDIO_ADC_GAIN           	  80    //回采通道增益

#define CONFIG_AUDIO_ADC_GAIN               100      //mic/aux增益

#define CONFIG_AISP_DIFFER_MIC_REPLACE_LINEIN       //用差分mic代替aec回采
#define CONFIG_ASR_CLOUD_ADC_CHANNEL        1		//云端识别mic通道
#define CONFIG_VOICE_NET_CFG_ADC_CHANNEL    1		//声波配网mic通道
#define CONFIG_AISP_MIC0_ADC_CHANNEL        1		//本地唤醒左mic通道
#define CONFIG_AISP_MIC_ADC_GAIN            80		//本地唤醒mic增益
#define CONFIG_AISP_LINEIN_ADC_CHANNEL      3		//本地唤醒LINEIN回采DAC通道
#define CONFIG_AISP_MIC1_ADC_CHANNEL        0		//本地唤醒右mic通道
#define CONFIG_REVERB_ADC_CHANNEL           1		//混响mic通道
#define CONFIG_PHONE_CALL_ADC_CHANNEL       1		//通话mic通道
#define CONFIG_UAC_MIC_ADC_CHANNEL          1		//UAC mic通道
#define CONFIG_AISP_LINEIN_ADC_GAIN         10		//本地唤醒LINEIN增益

//**********************************END********************************************//



//*********************************************************************************//
//                                  电源配置                                     //
//*********************************************************************************//
#define TCFG_LOWPOWER_BTOSC_DISABLE			0
#define TCFG_LOWPOWER_LOWPOWER_SEL			0//(RF_SLEEP_EN|RF_FORCE_SYS_SLEEP_EN|SYS_SLEEP_EN) //该宏在睡眠低功耗才用到，此处设置为0
#define TCFG_LOWPOWER_VDDIOM_LEVEL			VDDIOM_VOL_32V //正常工作的内部vddio电压值，一般使用外部3.3V，内部设置需比外部3.3V小
#define TCFG_LOWPOWER_VDDIOW_LEVEL			VDDIOW_VOL_21V //软关机或睡眠的内部vddio最低电压值
#define VDC14_VOL_SEL_LEVEL					VDC14_VOL_SEL_140V //内部的1.4V默认1.4V
#define SYSVDD_VOL_SEL_LEVEL				SYSVDD_VOL_SEL_126V //系统内核电压，默认1.26V


#define CONFIG_CXX_SUPPORT //使能C++支持

#define CONFIG_DEBUG_ENABLE                     /* 打印开关 */

#define CONFIG_DEBUG_ENABLE                     /* 打印开关 */
#define CONFIG_WIFI_ENABLE  			          /* 无线WIFI 	*/

//#define SDTAP_DEBUG

#if !defined CONFIG_DEBUG_ENABLE || defined CONFIG_LIB_DEBUG_DISABLE
#define LIB_DEBUG    0
#else
#define LIB_DEBUG    1
#endif
#define CONFIG_DEBUG_LIB(x)         (x & LIB_DEBUG)

#endif


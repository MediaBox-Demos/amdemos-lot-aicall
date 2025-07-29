#include "app_config.h"
#include "system/includes.h"
#include "os/os_api.h"
#include "event/key_event.h"
#include "event/device_event.h"

#include "artc_device_helper.h"
#include "artc_logger.h"
#include "artc_aicall_demo.h"


/*中断列表 */
const struct irq_info irq_info_table[] = {
    //中断号   //优先级0-7   //注册的cpu(0或1)
#ifdef CONFIG_IPMASK_ENABLE
    //不可屏蔽中断方法：支持写flash，但中断函数和调用函数和const要全部放在内部ram
    { IRQ_SOFT5_IDX,      6,   0    }, //此中断强制注册到cpu0
    { IRQ_SOFT4_IDX,      6,   1    }, //此中断强制注册到cpu1
#if 0 //如下，SPI1使用不可屏蔽中断设置,优先级固定7
    { IRQ_SPI1_IDX,      7,   1    },//中断强制注册到cpu0/1
#endif
#endif
#if CPU_CORE_NUM == 1
    { IRQ_SOFT5_IDX,      7,   0    }, //此中断强制注册到cpu0
    { IRQ_SOFT4_IDX,      7,   1    }, //此中断强制注册到cpu1
    { -2,     			-2,   -2   },//如果加入了该行, 那么只有该行之前的中断注册到对应核, 其他所有中断强制注册到CPU0
#endif

    { -1,     -1,   -1    },
};

#define APP_CORE_STK_SIZE 2048
#define APP_CORE_Q_SIZE 1024
static u8 app_core_tcb_stk_q[sizeof(StaticTask_t) + APP_CORE_STK_SIZE * 4 + sizeof(struct task_queue) + APP_CORE_Q_SIZE] ALIGNE(4);

#define SYS_TIMER_STK_SIZE 512
#define SYS_TIMER_Q_SIZE 128
static u8 sys_timer_tcb_stk_q[sizeof(StaticTask_t) + SYS_TIMER_STK_SIZE * 4 + sizeof(struct task_queue) + SYS_TIMER_Q_SIZE] ALIGNE(4);

#define SYSTIMER_STK_SIZE 256
static u8 systimer_tcb_stk_q[sizeof(StaticTask_t) + SYSTIMER_STK_SIZE * 4] ALIGNE(4);

#define SYS_EVENT_STK_SIZE 512
static u8 sys_event_tcb_stk_q[sizeof(StaticTask_t) + SYS_EVENT_STK_SIZE * 4] ALIGNE(4);

/*任务列表 */
const struct task_info task_info_table[] = {
//  名称 | 优先级(0-31) | 栈大小 | 队列大小
    {"thread_fork_kill",25,     256,    0},
    {"app_core",            15,     APP_CORE_STK_SIZE,	  APP_CORE_Q_SIZE,		 app_core_tcb_stk_q },
    {"sys_event",           29,     SYS_EVENT_STK_SIZE,	   0, 					 sys_event_tcb_stk_q },
    {"systimer",            14,     SYSTIMER_STK_SIZE, 	   0,					 systimer_tcb_stk_q },
    {"sys_timer",            9,     SYS_TIMER_STK_SIZE,	  SYS_TIMER_Q_SIZE,		 sys_timer_tcb_stk_q },

    {"audio_server",    16,     512,    64},
    {"audio_decoder",   30,     1024,   64},
    {"audio_encoder",   12,     384,    64},
    {"aec_encoder",     13,     1024,   0},
    // {"dns_encoder",     13,     512,    0},
    {0, 0, 0, 0, 0},
};


static int on_artc_aicall_ad_key_click(int code)
{
    ARTC_LOG_INFO("on_artc_aicall_ad_key_click: code = %d", code);
    if (code == KEY_K1) {
        artc_aicall_demo_start_call();
        return 0;
    }
    if (code == KEY_K2) {
        artc_aicall_demo_handup();
        return 0;
    }
    if (code == KEY_K3) {
        artc_aicall_demo_interrupt_speaking();
        return 0;
    }
    if (code == KEY_K4) {
        artc_aicall_demo_mute_local_audio();
        return 0;
    }
    if (code == KEY_K5) {
        artc_aicall_demo_unmute_local_audio();
        return 0;
    }

    return 0;
}

static int on_artc_aicall_ad_key_long(int code)
{
    ARTC_LOG_INFO("on_artc_aicall_ad_key_long: code = %d", code);
    return 0;
}


static int artc_aicall_demo_key_event_handler(struct key_event *key)
{
    switch (key->action) {
    case KEY_EVENT_CLICK:
        on_artc_aicall_ad_key_click(key->value);
        break;
    case KEY_EVENT_LONG:
        on_artc_aicall_ad_key_long(key->value);
        break;
    }
    return 0;
}

/*
 *设备响应函数
 */
static int artc_aicall_demo_device_event_handler(struct sys_event *sys_eve)
{
    ARTC_LOG_INFO("artc_aicall_demo_device_event_handler: from = %d, type = %d", sys_eve->from, sys_eve->type);
    struct device_event *device_eve = (struct device_event *)sys_eve->payload;
    /* SD卡插拔处理 */
    if (sys_eve->from == DEVICE_EVENT_FROM_SD) {
        switch (device_eve->event) {
        case DEVICE_EVENT_IN:
            //等待SD卡挂载完成才开始搜索文件
            break;
        case DEVICE_EVENT_OUT:
            //SD卡拔出，释放资源
            break;
        }
    }

    return true;
}

static int artc_aicall_demo_event_handler(struct application *app, struct sys_event *sys_event)
{
    ARTC_LOG_INFO("artc_aicall_demo_event_handler: app_name = %s, type = %d\n", app->name, sys_event->type);
    switch (sys_event->type)
    {
    case SYS_KEY_EVENT:
        return artc_aicall_demo_key_event_handler((struct key_event *)sys_event->payload);
    case SYS_DEVICE_EVENT:
        return artc_aicall_demo_device_event_handler(sys_event);
    }
    return 0;
}

/*
 * 应用程序主函数
 */
static int on_app_start(void)
{
    ARTC_LOG_INFO("OnAppStart");

    // setup wifi
    ARTCSetupWifi();

    return 0;
}

static int on_app_stop(void)
{
    ARTC_LOG_INFO("OnAppStop");
    return 0;
}


static int artc_aicall_demo_state_machine(struct application *app, enum app_state state, struct intent *it)
{
    ARTC_LOG_INFO("artc_aicall_demo_state_machine: app_name = %s, state = %d\n", app->name, state);
    switch (state) {
    case APP_STA_CREATE:
        break;
    case APP_STA_START:
        return on_app_start();
    case APP_STA_PAUSE:
        break;
    case APP_STA_RESUME:
        break;
    case APP_STA_STOP:
        return on_app_stop();
    case APP_STA_DESTROY:
        break;
    }

    return 0;
}

static const struct application_operation artc_aicall_demo_ops = {
    .state_machine = artc_aicall_demo_state_machine,
    .event_handler = artc_aicall_demo_event_handler,
};

REGISTER_APPLICATION(artc_aicall_demo) = {
    .name = "artc_aicall_demo",
    .ops = &artc_aicall_demo_ops,
    .state = APP_STA_DESTROY,
};


void app_main()
{
#ifdef CONFIG_CXX_SUPPORT
    void cpp_run_init(void);
    cpp_run_init(); //使用c++时，必须先调用该接口进行初始化
#endif

    ARTC_LOG_INFO("\r\n\r\n\r\n\r\n\r\n ------------artc aicall demo run %s-------------\r\n\r\n\r\n\r\n\r\n", __TIME__);

    struct intent it;
    init_intent(&it);
    it.name = "artc_aicall_demo";
    it.action = ACTION_DO_NOTHING;
    start_app(&it);
}

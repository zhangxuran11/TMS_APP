1.编译时候添加的头文件的宏定义
USE_STDPERIPH_DRIVER,STM32F40_41xxx,HAVE_CONFIG_H,DEBUG,OPT_SPEED,
FPM_DEFAULT,STDC_HEADERS,USE_USB_OTG_FS,USE_STM324xG_EVAL,USE_EMBEDDED_PHY,
__FPU_PRESENT = 1,__FPU_USED=1,ARM_MATH_CM4

2. 启动文件中，修改了PendSV_Handler 和SysTick_Handler  的函数名字，故中断服务程序的名称也改变了
;DCD     PendSV_Handler             ; PendSV Handler

;DCD     SysTick_Handler            ; SysTick Handler

DCD     OS_CPU_PendSVHandler

DCD     OS_CPU_SysTickHandler

3.2016-05-12  使用OS_CPU_SysTickHandler（）中断比较麻烦，故修改了启动文件
DCD     PendSV_Handler             ; PendSV Handler

DCD     SysTick_Handler            ; SysTick Handler

;DCD     OS_CPU_PendSVHandler

;DCD     OS_CPU_SysTickHandler

3. 在标准固件库1.5中。有些文件stm32f407是没有这些功能的。故需要移除工程
	stm32f4xx_cec.c		stm32f4xx_flash_ramfunc.c	stm32f4xx_fmc.c		stm32f4xx_fmpi2c.c
	stm32f4xx_spdifrx.c		stm32f4xx_qspi.c

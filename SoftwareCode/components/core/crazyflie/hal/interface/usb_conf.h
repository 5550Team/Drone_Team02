
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CONF__H__
#define __USB_CONF__H__

#include <sys/cdefs.h>

#ifndef USE_USB_OTG_FS
 #define USE_USB_OTG_FS
#endif /* USE_USB_OTG_FS */

#ifdef USE_USB_OTG_FS 
 #define USB_OTG_FS_CORE
#endif

#ifndef USE_USB_OTG_HS
 //#define USE_USB_OTG_HS
#endif /* USE_USB_OTG_HS */

#ifndef USE_ULPI_PHY
 //#define USE_ULPI_PHY
#endif /* USE_ULPI_PHY */

#ifndef USE_EMBEDDED_PHY
 //#define USE_EMBEDDED_PHY
#endif /* USE_EMBEDDED_PHY */

#ifdef USE_USB_OTG_HS 
 #define USB_OTG_HS_CORE
#endif

#ifdef USB_OTG_HS_CORE
 #define RX_FIFO_HS_SIZE                          512
 #define TX0_FIFO_HS_SIZE                          64
 #define TX1_FIFO_HS_SIZE                         372
 #define TX2_FIFO_HS_SIZE                          64
 #define TX3_FIFO_HS_SIZE                           0
 #define TX4_FIFO_HS_SIZE                           0
 #define TX5_FIFO_HS_SIZE                           0

// #define USB_OTG_HS_SOF_OUTPUT_ENABLED

 #ifdef USE_ULPI_PHY
  #define USB_OTG_ULPI_PHY_ENABLED
 #endif
 #ifdef USE_EMBEDDED_PHY 
   #define USB_OTG_EMBEDDED_PHY_ENABLED
   /* wakeup is working only when HS core is configured in FS mode */
   #define USB_OTG_HS_LOW_PWR_MGMT_SUPPORT
 #endif

 #define USB_OTG_HS_DEDICATED_EP1_ENABLED
#endif


#ifdef USB_OTG_FS_CORE
 #define RX_FIFO_FS_SIZE                          128
 #define TX0_FIFO_FS_SIZE                          32
 #define TX1_FIFO_FS_SIZE                         128
 #define TX2_FIFO_FS_SIZE                          32 
 #define TX3_FIFO_FS_SIZE                           0

// #define USB_OTG_FS_LOW_PWR_MGMT_SUPPORT
// #define USB_OTG_FS_SOF_OUTPUT_ENABLED
#endif


//#define VBUS_SENSING_ENABLED


//#define USE_HOST_MODE
#define USE_DEVICE_MODE
//#define USE_OTG_MODE

#ifndef USB_OTG_FS_CORE
 #ifndef USB_OTG_HS_CORE
    #error  "USB_OTG_HS_CORE or USB_OTG_FS_CORE should be defined"
 #endif
#endif

#ifndef USE_DEVICE_MODE
 #ifndef USE_HOST_MODE
    #error  "USE_DEVICE_MODE or USE_HOST_MODE should be defined"
 #endif
#endif

#ifndef USE_USB_OTG_HS
 #ifndef USE_USB_OTG_FS
    #error  "USE_USB_OTG_HS or USE_USB_OTG_FS should be defined"
 #endif
#else //USE_USB_OTG_HS
 #ifndef USE_ULPI_PHY
  #ifndef USE_EMBEDDED_PHY
     #error  "USE_ULPI_PHY or USE_EMBEDDED_PHY should be defined"
  #endif
 #endif
#endif


#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined   (__GNUC__)        /* GNU Compiler */
    #define __ALIGN_END    __attribute__ ((aligned (4)))
    #define __ALIGN_BEGIN         
  #else                           
    #define __ALIGN_END
    #if defined   (__CC_ARM)      /* ARM Compiler */
      #define __ALIGN_BEGIN    __align(4)  
    #elif defined (__ICCARM__)    /* IAR Compiler */
      #define __ALIGN_BEGIN 
    #elif defined  (__TASKING__)  /* TASKING Compiler */
      #define __ALIGN_BEGIN    __align(4) 
    #endif /* __CC_ARM */  
  #endif /* __GNUC__ */ 
#else
  #define __ALIGN_BEGIN
  #define __ALIGN_END   
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* __packed keyword used to decrease the data type alignment to 1-byte */
#if defined (__CC_ARM)         /* ARM Compiler */
  #define __packed    __packed
#elif defined (__ICCARM__)     /* IAR Compiler */
  #define __packed    __packed
#elif defined   ( __GNUC__ )   /* GNU Compiler */                        
//  #define __packed    __attribute__ ((__packed__))
#ifndef __packed
	#define __packed    __attribute__ ((__packed__))
#endif
#elif defined   (__TASKING__)  /* TASKING Compiler */
  #define __packed    __unaligned
#endif /* __CC_ARM */

#endif //__USB_CONF__H__




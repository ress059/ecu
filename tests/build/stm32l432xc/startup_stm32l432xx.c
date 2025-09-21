/**
 * @file
 * @brief CMSIS-compatible startup file for STM32L432...
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-09-14
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* CMSIS device... */
#include "stm32l432xx.h"

/*------------------------------------------------------------*/
/*------------ GLOBAL SYMBOLS FROM LINKER SCRIPT -------------*/
/*------------------------------------------------------------*/

/// @brief Start of stack. RAM start + RAM length since stack grows
/// top-down on ARM Cortex M. Used to initialize stack pointer on startup.
extern uint32_t __INITIAL_SP;



extern __NO_RETURN void __PROGRAM_START(void);




extern uint32_t _estack;

/// @brief Start of .data section in flash. Used to copy .data 
/// from flash into RAM on startup.
extern uint32_t _sidata;

/// Start of .data section in RAM. Used to copy .data from
/// flash into RAM on startup.
extern uint32_t _sdata;

/// End of .data section in RAM. Used to copy .data from
/// flash into RAM on startup.
extern uint32_t _edata;

/// @brief Starting address of .bss section. Used to 0 out
/// .bss section on startup.
extern uint32_t _sbss;

/// @brief Ending address of .bss section. Used to 0 out
/// .bss section on startup.
extern uint32_t _ebss;

/// @brief STDLib initialization function that must be called prior to using STDLib.
extern void __libc_init_array(void);

/// @brief Main routine.
extern int main(void);




/*---------------------------------------------------------------------------
  Internal References
 *---------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler  (void);
__NO_RETURN void Default_Handler(void);

/* ToDo: Add Cortex exception handler according the used Cortex-Core */
/*---------------------------------------------------------------------------
  Exception / Interrupt Handler
 *---------------------------------------------------------------------------*/
/* Exceptions */
void NMI_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler      (void) __attribute__ ((weak));
void MemManage_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void SecureFault_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));

/* ToDo: Add your device specific interrupt handler */
void <DeviceInterrupt first>_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
...
void <DeviceInterrupt last>_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));


/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

/* ToDo: Add Cortex exception vectors according the used Cortex-Core */
extern const VECTOR_TABLE_Type __VECTOR_TABLE[<Device vector table entries>];
       const VECTOR_TABLE_Type __VECTOR_TABLE[<Device vector table entries>] __VECTOR_TABLE_ATTRIBUTE = {
  (VECTOR_TABLE_Type)(&__INITIAL_SP),  /*     Initial Stack Pointer */
  Reset_Handler,                       /*     Reset Handler */
  NMI_Handler,                         /* -14 NMI Handler */
  HardFault_Handler,                   /* -13 Hard Fault Handler */
  MemManage_Handler,                   /* -12 MPU Fault Handler */
  BusFault_Handler,                    /* -11 Bus Fault Handler */
  UsageFault_Handler,                  /* -10 Usage Fault Handler */
  SecureFault_Handler,                 /*  -9 Secure Fault Handler */
  0,                                   /*     Reserved */
  0,                                   /*     Reserved */
  0,                                   /*     Reserved */
  SVC_Handler,                         /*  -5 SVCall Handler */
  DebugMon_Handler,                    /*  -4 Debug Monitor Handler */
  0,                                   /*     Reserved */
  PendSV_Handler,                      /*  -2 PendSV Handler */
  SysTick_Handler,                     /*  -1 SysTick Handler */

/* ToDo: Add your device specific interrupt vectors */
  /* Interrupts */
  <DeviceInterrupt first>_Handler,     /* first Device Interrupt */
  ...
  <DeviceInterrupt last>_Handler       /* last Device Interrupt */
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

/*---------------------------------------------------------------------------
  Reset Handler called on controller reset
 *---------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler(void)
{
  __set_PSP((uint32_t)(&__INITIAL_SP));

/* ToDo: Initialize stack limit register for Armv8-M Main Extension based processors*/
  __set_MSPLIM((uint32_t)(&__STACK_LIMIT));
  __set_PSPLIM((uint32_t)(&__STACK_LIMIT));

/* ToDo: Add stack sealing for Armv8-M based processors */
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  __TZ_set_STACKSEAL_S((uint32_t *)(&__STACK_SEAL));
#endif

  SystemInit();                    /* CMSIS System Initialization */
  __PROGRAM_START();               /* Enter PreMain (C library entry point) */
}


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

/*---------------------------------------------------------------------------
  Hard Fault Handler
 *---------------------------------------------------------------------------*/
void HardFault_Handler(void)
{
  while(1);
}

/*---------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *---------------------------------------------------------------------------*/
void Default_Handler(void)
{
  while(1);
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#endif



























/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("ecu/main.c")

/*------------------------------------------------------------*/
/*---------------------- FILE SCOPE TYPES --------------------*/
/*------------------------------------------------------------*/

// /// @brief System control block adapted from CMSIS.
// typedef struct
// {
//     volatile const uint32_t CPUID;      /*!< Offset: 0x000 (R/ )  CPUID Base Register */
//     volatile uint32_t ICSR;             /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register */
//     volatile uint32_t VTOR;             /*!< Offset: 0x008 (R/W)  Vector Table Offset Register */
//     volatile uint32_t AIRCR;            /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register */
//     volatile uint32_t SCR;              /*!< Offset: 0x010 (R/W)  System Control Register */
//     volatile uint32_t CCR;              /*!< Offset: 0x014 (R/W)  Configuration Control Register */
//     volatile uint8_t  SHP[12U];         /*!< Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
//     volatile uint32_t SHCSR;            /*!< Offset: 0x024 (R/W)  System Handler Control and State Register */
//     volatile uint32_t CFSR;             /*!< Offset: 0x028 (R/W)  Configurable Fault Status Register */
//     volatile uint32_t HFSR;             /*!< Offset: 0x02C (R/W)  HardFault Status Register */
//     volatile uint32_t DFSR;             /*!< Offset: 0x030 (R/W)  Debug Fault Status Register */
//     volatile uint32_t MMFAR;            /*!< Offset: 0x034 (R/W)  MemManage Fault Address Register */
//     volatile uint32_t BFAR;             /*!< Offset: 0x038 (R/W)  BusFault Address Register */
//     volatile uint32_t AFSR;             /*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register */
//     volatile const uint32_t PFR[2U];    /*!< Offset: 0x040 (R/ )  Processor Feature Register */
//     volatile const uint32_t DFR;        /*!< Offset: 0x048 (R/ )  Debug Feature Register */
//     volatile const uint32_t ADR;        /*!< Offset: 0x04C (R/ )  Auxiliary Feature Register */
//     volatile const uint32_t MMFR[4U];   /*!< Offset: 0x050 (R/ )  Memory Model Feature Register */
//     volatile const uint32_t ISAR[5U];   /*!< Offset: 0x060 (R/ )  Instruction Set Attributes Register */
//     uint32_t RESERVED0[5U];
//     volatile uint32_t CPACR;            /*!< Offset: 0x088 (R/W)  Coprocessor Access Control Register */
// } SCB_Type;

// /*------------------------------------------------------------*/
// /*--------------------- FILE SCOPE DEFINES -------------------*/
// /*------------------------------------------------------------*/

// #define SCB \
//     ((SCB_Type *)0xE000ED00)



/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

static void unused_isr(void);
void reset_isr(void); /* Must make this a global symbol so linker script knows where program entrypoint is. */
static void nmi_isr(void);
static void hard_fault_isr(void);
static void mem_manage_fault_isr(void);
static void bus_fault_isr(void);
static void usage_fault_isr(void);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static void unused_isr(void)
{
    ECU_ASSERT( (false) );
}

void reset_isr(void)
{
    /* These cannot be statically asserted since these are symbols defined in the linker
    script, which are not available to the compiler. */
    ECU_ASSERT( (&_edata >= &_sdata) );
    ECU_ASSERT( (&_ebss >= &_sbss) );

    /* Step 0: Enable FPU. See PM0214 Section 4.6.6 */
    SCB->CPACR |= (0xFU << 20);

    /* Step 1: Zero out .bss section. Must do this first before stdlib initialized!
    Cannot use memset() since cannot use stdlib at this stage. */
    for (uint32_t *bss = &_sbss; bss < &_ebss; bss++)
    {
        *bss = 0;
    }

    /* Step 2: Initialize stdlib. Also call global/static C++ constructors if
    this application changes to a C++ application in the future. This is a 
    stdlib function that calls all function pointers stored in .preinit_array
    and .init_array sections. */
    __libc_init_array();

    /* Step 3: Copy .data from FLASH into RAM. */
    memcpy(&_sdata, &_sidata, (size_t)(&_edata - &_sdata));

    /* Step 4: Initialize system clocks and any other hardware needed. */
    

    /* Step 5: Branch to main. Assert if main ever exits. */
    main();
    ECU_ASSERT( (false) );
}

static void nmi_isr(void)
{
    ECU_ASSERT( (false) );
}

static void hard_fault_isr(void)
{
    ECU_ASSERT( (false) );
}

static void mem_manage_fault_isr(void)
{
    ECU_ASSERT( (false) );
}

static void bus_fault_isr(void)
{
    ECU_ASSERT( (false) );
}

static void usage_fault_isr(void)
{
    ECU_ASSERT( (false) );
}

/*------------------------------------------------------------*/
/*--------------------- STATIC VARIABLES ---------------------*/
/*------------------------------------------------------------*/

#pragma message("TODO: Add attribute section to ECU attributes.h!")

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
static void (*const vector_table[])(void) __attribute__((section(".isr_vector"))) =
{
    /*--------------------------------------------------------------*/
    /*------------------- FOR ALL CORTEX M4 CORES ------------------*/
    /*--------------------------------------------------------------*/
    (void(*)(void))&_estack,    /* 0x00. This is the MAIN stack pointer, NOT the process stack pointer. */
    &reset_isr,                 /* 0x04. Reset. */
    &nmi_isr,                   /* 0x08. Non maskable interrupt. */
    &hard_fault_isr,            /* 0x0C. All classes of fault. */
    &mem_manage_fault_isr,      /* 0x10. Memory management fault. */
    &bus_fault_isr,             /* 0x14. Pre-fetch fault, memory access fault, etc.*/
    &usage_fault_isr,           /* 0x18. Undefined instruction or illegal state. */
    &unused_isr,                /* 0x1C. Reserved pad. */
    &unused_isr,                /* 0x20. Reserved pad. */
    &unused_isr,                /* 0x24. Reserved pad. */
    &unused_isr,                /* 0x28. Reserved pad. */
    &unused_isr,                /* 0x2C. SVCall. */
    &unused_isr,                /* 0x30. Reserved pad. */
    &unused_isr,                /* 0x34. Reserved pad. */
    &unused_isr,                /* 0x38. PendSV. */
    &unused_isr,                /* 0x3C. Systick. */
    /*--------------------------------------------------------------*/
    /*-------------------- SPECIFIC TO STM32L432 -------------------*/
    /*--------------------------------------------------------------*/
    &unused_isr,                /* 0x40. Window Watchdog. */
    &unused_isr,                /* 0x44. PVD_PVM. */
    &unused_isr,                /* 0x48. RTC tamper. */
    &unused_isr,                /* 0x4C. RTC wakeup. */
    &unused_isr,                /* 0x50. Flash global interrupt. */
    &unused_isr,                /* 0x54. RCC global interrupt. Typo in manual.... */
    &unused_isr,                /* 0x58. EXTI Line0 interrupt. Typo in manual.... */
    &unused_isr,                /* 0x5C. EXTI Line1 interrupt. Typo in manual.... */
    &unused_isr,                /* 0x60. EXTI Line2 interrupt. */
    &unused_isr,                /* 0x64. EXTI Line3 interrupt. */
    &unused_isr,                /* 0x68. EXTI Line4 interrupt. */
    &unused_isr,                /* 0x6C. DMA1 channel 1 interrupt. */
    &unused_isr,                /* 0x70. DMA1 channel 2 interrupt. */
    &unused_isr,                /* 0x74. DMA1 channel 3 interrupt. */
    &unused_isr,                /* 0x78. DMA1 channel 4 interrupt. */
    &unused_isr,                /* 0x7C. DMA1 channel 5 interrupt. */
    &unused_isr,                /* 0x80. DMA1 channel 6 interrupt. */
    &unused_isr,                /* 0x84. DMA1 channel 7 interrupt. */
    &unused_isr,                /* 0x88. ADC1 and ADC2 global interrupt. */
    &unused_isr,                /* 0x8C. CAN1_TX interrupts. */
    &unused_isr,                /* 0x90. CAN1_RX0 interrupts. */
    &unused_isr,                /* 0x94. CAN1_RX1 interrupt. */
    &unused_isr,                /* 0x98. CAN1_SCE interrupt. */
    &unused_isr,                /* 0x9C. EXTI Line[9:5] interrupts. */
    &unused_isr,                /* 0xA0. TIM1 Break/TIM15 global interrupts. */
    &unused_isr,                /* 0xA4. TIM1 Update/TIM16 global interrupts. */
    &unused_isr,                /* 0xA8. TIM1 trigger and communication interrupt. */
    &unused_isr,                /* 0xAC. TIM1 capture compare interrupt. */
    &unused_isr,                /* 0xB0. TIM2 global interrupt. */
    &unused_isr,                /* 0xB4. TIM3 global interrupt not available on STM32L432xc! */
    &unused_isr,                /* 0xB8. Reserved pad. */
    &unused_isr,                /* 0xBC. I2C1 event interrupt. */
    &unused_isr,                /* 0xC0. I2C1 error interrupt. */
    &unused_isr,                /* 0xC4. I2C2 event interrupt not available on STM32L432xc! */
    &unused_isr,                /* 0xC8. I2C2 error interrupt not available on STM32L432xc! */
    &unused_isr,                /* 0xCC. SPI1 global interrupt. */
    &unused_isr,                /* 0xD0. SPI2 global interrupt not available on STM32L432xc! */
    &unused_isr,                /* 0xD4. USART1 global interrupt. */
    &unused_isr,                /* 0xD8. USART2 global interrupt. */
    &unused_isr,                /* 0xDC. USART3 global interrupt not available on STM32L432xc! */
    &unused_isr,                /* 0xE0. EXTI Line[15:10] interrupts. */
    &unused_isr,                /* 0xE4. RTC alarms through EXTI line 18 interrupts. */
    &unused_isr,                /* 0xE8. Reserved pad. */
    &unused_isr,                /* 0xEC. Reserved pad. */
    &unused_isr,                /* 0xF0. Reserved pad. */
    &unused_isr,                /* 0xF4. Reserved pad. */
    &unused_isr,                /* 0xF8. Reserved pad. */
    &unused_isr,                /* 0xFC. Reserved pad. */
    &unused_isr,                /* 0x100. Reserved pad. */
    &unused_isr,                /* 0x104. SDMMC1 global interrupt not available on STM32L432xc! */
    &unused_isr,                /* 0x108. Reserved pad. */
    &unused_isr,                /* 0x10C. SPI3 global interrupt. */
    &unused_isr,                /* 0x110. UART4 global interrupt not available on STM32L432xc! */
    &unused_isr,                /* 0x114. Reserved pad. */
    &unused_isr,                /* 0x118. TIM6 global and DAC1 underrun interrupts. */
    &unused_isr,                /* 0x11C. TIM7 global interrupt. */
    &unused_isr,                /* 0x120. DMA2 channel 1 interrupt. */
    &unused_isr,                /* 0x124. DMA2 channel 2 interrupt. */
    &unused_isr,                /* 0x128. DMA2 channel 3 interrupt. */
    &unused_isr,                /* 0x12C. DMA2 channel 4 interrupt. */
    &unused_isr,                /* 0x130. DMA2 channel 5 interrupt. */
    &unused_isr,                /* 0x134. DFSDM1_FLT0 global interrupt not available on STM32L432xc! */
    &unused_isr,                /* 0x138. DFSDM1_FLT1 global interrupt not available on STM32L432xc! */
    &unused_isr,                /* 0x13C. Reserved pad. */
    &unused_isr,                /* 0x140. COMP1/COMP2 through EXTI lines 21/22 interrupts. */
    &unused_isr,                /* 0x144. LPTIM1 global interrupt. */
    &unused_isr,                /* 0x148. LPTIM2 global interrupt. */
    &unused_isr,                /* 0x14C. USB event interrupt through EXTI line 17. */
    &unused_isr,                /* 0x150. DMA2 channel 6 interrupt. */
    &unused_isr,                /* 0x154. DMA2 channel 7 interrupt. */
    &unused_isr,                /* 0x158. LPUART1 global interrupt. */
    &unused_isr,                /* 0x15C. QUADSPI global interrupt. */
    &unused_isr,                /* 0x160. I2C3 event interrupt. */
    &unused_isr,                /* 0x164. I2C3 error interrupt. */
    &unused_isr,                /* 0x168. SAI1 global interrupt. */
    &unused_isr,                /* 0x16C. Reserved pad. */
    &unused_isr,                /* 0x170. SWPMI1 global interrupt. */
    &unused_isr,                /* 0x174. TSC global interrupt. */
    &unused_isr,                /* 0x178. LCD global interrupt not available on STM32L432xc! */
    &unused_isr,                /* 0x17C. AES global interrupt not available on STM32L432xc! */
    &unused_isr,                /* 0x180. RNG global interrupt. */
    &unused_isr,                /* 0x184. Floating point interrupt. */
    &unused_isr,                /* 0x188. CRS interrupt. */
    &unused_isr,                /* 0x18C. I2C4 event interrupt, wakeup through EXTI line 40 not available on STM32L432xc! */
    &unused_isr                 /* 0x190. I2C4 error interrupt not available on STM32L432xc! */
};
#pragma GCC diagnostic pop

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(vector_table) == 0x194U), "Vector table does not have proper amount of entries." );

/*------------------------------------------------------------*/
/*---------------- ASSERT HANDLER DEFINITION -----------------*/
/*------------------------------------------------------------*/

void ecu_assert_handler(const char *file, int line)
{
    (void)file;
    (void)line;
    while(1)
    {

    }
}


/**
  ******************************************************************************
  * @file    system_stm32f1xx.c
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-M3 Device Peripheral Access Layer System Source File.
  * 
  * 1.  This file provides two functions and one global variable to be called from 
  *     user application:
  *      - SystemInit(): Setups the system clock (System clock source, PLL Multiplier
  *                      factors, AHB/APBx prescalers and Flash settings). 
  *                      This function is called at startup just after reset and 
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32f1xx_xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick 
  *                                  timer or configure other parameters.
  *                                     
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  * 2. After each device reset the HSI (8 MHz) is used as system clock source.
  *    Then SystemInit() function is called, in "startup_stm32f1xx_xx.s" file, to
  *    configure the system clock before to branch to main program.
  *
  * 4. The default value of HSE crystal is set to 8 MHz (or 25 MHz, depending on
  *    the product used), refer to "HSE_VALUE". 
  *    When HSE is used as system clock source, directly or through PLL, and you
  *    are using different crystal you have to adapt the HSE value to your own
  *    configuration.
  *        
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32f1xx_system
  * @{
  */

/** @addtogroup STM32F1xx_System_Private_Includes
  * @{
  */

#include "stm32f1xx.h"

/**
  * @}
  */

/** @addtogroup STM32F1xx_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32F1xx_System_Private_Defines
  * @{
  */

#if !defined(HSE_VALUE)
#define HSE_VALUE 8000000U
#endif

#if !defined(HSI_VALUE)
#define HSI_VALUE 8000000U
#endif

#if defined(STM32F100xE) || defined(STM32F101xE) || defined(STM32F101xG) || defined(STM32F103xE) || defined(STM32F103xG)

#endif

#if defined(USER_VECT_TAB_ADDRESS)

#if defined(VECT_TAB_SRAM)
#define VECT_TAB_BASE_ADDRESS SRAM_BASE
#define VECT_TAB_OFFSET 0x00000000U
#else
#define VECT_TAB_BASE_ADDRESS FLASH_BASE
#define VECT_TAB_OFFSET 0x00000000U
#endif
#endif

/******************************************************************************/

/**
  * @}
  */

/** @addtogroup STM32F1xx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32F1xx_System_Private_Variables
  * @{
  */

uint32_t SystemCoreClock = 16000000;
const uint8_t AHBPrescTable[16U] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8U] = {0, 0, 0, 0, 1, 2, 3, 4};

/**
  * @}
  */

/** @addtogroup STM32F1xx_System_Private_FunctionPrototypes
  * @{
  */

#if defined(STM32F100xE) || defined(STM32F101xE) || defined(STM32F101xG) || defined(STM32F103xE) || defined(STM32F103xG)
#ifdef DATA_IN_ExtSRAM
static void SystemInit_ExtMemCtl(void);
#endif
#endif

/**
  * @}
  */

/** @addtogroup STM32F1xx_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system
  *         Initialize the Embedded Flash Interface, the PLL and update the 
  *         SystemCoreClock variable.
  * @note   This function should be used only after reset.
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
#if defined(STM32F100xE) || defined(STM32F101xE) || defined(STM32F101xG) || defined(STM32F103xE) || defined(STM32F103xG)
#ifdef DATA_IN_ExtSRAM
  SystemInit_ExtMemCtl();
#endif
#endif

#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
#endif
}

/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *           
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.         
  *     
  * @note   - The system frequency computed by this function is not the real 
  *           frequency in the chip. It is calculated based on the predefined 
  *           constant and the selected clock source:
  *             
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *                                              
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
  *                          
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**) 
  *             or HSI_VALUE(*) multiplied by the PLL factors.
  *         
  *         (*) HSI_VALUE is a constant defined in stm32f1xx.h file (default value
  *             8 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.   
  *    
  *         (**) HSE_VALUE is a constant defined in stm32f1xx.h file (default value
  *              8 MHz or 25 MHz, depending on the product used), user has to ensure
  *              that HSE_VALUE is same as the real frequency of the crystal used.
  *              Otherwise, this function may have wrong result.
  *                
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate(void)
{
  uint32_t tmp = 0U, pllmull = 0U, pllsource = 0U;

#if defined(STM32F105xC) || defined(STM32F107xC)
  uint32_t prediv1source = 0U, prediv1factor = 0U, prediv2factor = 0U, pll2mull = 0U;
#endif

#if defined(STM32F100xB) || defined(STM32F100xE)
  uint32_t prediv1factor = 0U;
#endif

  tmp = RCC->CFGR & RCC_CFGR_SWS;

  switch (tmp)
  {
  case 0x00U:
    SystemCoreClock = HSI_VALUE;
    break;
  case 0x04U:
    SystemCoreClock = HSE_VALUE;
    break;
  case 0x08U:

    pllmull = RCC->CFGR & RCC_CFGR_PLLMULL;
    pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;

#if !defined(STM32F105xC) && !defined(STM32F107xC)
    pllmull = (pllmull >> 18U) + 2U;

    if (pllsource == 0x00U)
    {

      SystemCoreClock = (HSI_VALUE >> 1U) * pllmull;
    }
    else
    {
#if defined(STM32F100xB) || defined(STM32F100xE)
      prediv1factor = (RCC->CFGR2 & RCC_CFGR2_PREDIV1) + 1U;

      SystemCoreClock = (HSE_VALUE / prediv1factor) * pllmull;
#else

      if ((RCC->CFGR & RCC_CFGR_PLLXTPRE) != (uint32_t)RESET)
      { /* HSE oscillator clock divided by 2 */
        SystemCoreClock = (HSE_VALUE >> 1U) * pllmull;
      }
      else
      {
        SystemCoreClock = HSE_VALUE * pllmull;
      }
#endif
    }
#else
    pllmull = pllmull >> 18U;

    if (pllmull != 0x0DU)
    {
      pllmull += 2U;
    }
    else
    {
      pllmull = 13U / 2U;
    }

    if (pllsource == 0x00U)
    {

      SystemCoreClock = (HSI_VALUE >> 1U) * pllmull;
    }
    else
    { /* PREDIV1 selected as PLL clock entry */

      prediv1source = RCC->CFGR2 & RCC_CFGR2_PREDIV1SRC;
      prediv1factor = (RCC->CFGR2 & RCC_CFGR2_PREDIV1) + 1U;

      if (prediv1source == 0U)
      {

        SystemCoreClock = (HSE_VALUE / prediv1factor) * pllmull;
      }
      else
      { /* PLL2 clock selected as PREDIV1 clock entry */

        prediv2factor = ((RCC->CFGR2 & RCC_CFGR2_PREDIV2) >> 4U) + 1U;
        pll2mull = ((RCC->CFGR2 & RCC_CFGR2_PLL2MUL) >> 8U) + 2U;
        SystemCoreClock = (((HSE_VALUE / prediv2factor) * pll2mull) / prediv1factor) * pllmull;
      }
    }
#endif
    break;

  default:
    SystemCoreClock = HSI_VALUE;
    break;
  }

  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4U)];

  SystemCoreClock >>= tmp;
}

#if defined(STM32F100xE) || defined(STM32F101xE) || defined(STM32F101xG) || defined(STM32F103xE) || defined(STM32F103xG)
/**
  * @brief  Setup the external memory controller. Called in startup_stm32f1xx.s 
  *          before jump to __main
  * @param  None
  * @retval None
  */
#ifdef DATA_IN_ExtSRAM
/**
  * @brief  Setup the external memory controller. 
  *         Called in startup_stm32f1xx_xx.s/.c before jump to main.
  *         This function configures the external SRAM mounted on STM3210E-EVAL
  *         board (STM32 High density devices). This SRAM will be used as program
  *         data memory (including heap and stack).
  * @param  None
  * @retval None
  */
void SystemInit_ExtMemCtl(void)
{
  __IO uint32_t tmpreg;

  RCC->AHBENR = 0x00000114U;

  tmpreg = READ_BIT(RCC->AHBENR, RCC_AHBENR_FSMCEN);

  RCC->APB2ENR = 0x000001E0U;

  tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPDEN);

  (void)(tmpreg);

  GPIOD->CRL = 0x44BB44BBU;
  GPIOD->CRH = 0xBBBBBBBBU;

  GPIOE->CRL = 0xB44444BBU;
  GPIOE->CRH = 0xBBBBBBBBU;

  GPIOF->CRL = 0x44BBBBBBU;
  GPIOF->CRH = 0xBBBB4444U;

  GPIOG->CRL = 0x44BBBBBBU;
  GPIOG->CRH = 0x444B4B44U;

  FSMC_Bank1->BTCR[4U] = 0x00001091U;
  FSMC_Bank1->BTCR[5U] = 0x00110212U;
}
#endif
#endif

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

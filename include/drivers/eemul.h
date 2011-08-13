/* Data in EEPROM Emulation section
 * Data structure:
 * - ID:						1  word
 * - PID Data: 					24 words
 * - Mixers Data: 				32 words
 * - Input Calibration Data:	25 words
 * Total size: 82 words
 * */

#include "lpc_types.h"
#include "LPC17xx.h"

#ifndef __EEMUL_H  
#define __EEMUL_H 

typedef struct
{
	// Save ID
	__I  uint32_t ID;
	
	/* --- START REGULATOR CONSTANTS --- */
	// Pitch regulator Coefficients
	__I  uint32_t PITCH_R_KP;
	__I  uint32_t PITCH_R_KI;
	__I  uint32_t PITCH_A_KP;
	__I  uint32_t PITCH_A_KI;
	__I  uint32_t PITCH_R_IMAX;
	__I  uint32_t PITCH_A_IMAX;
	
	// Roll regulator Coefficients
	__I  uint32_t ROLL_R_KP;
	__I  uint32_t ROLL_R_KI;
	__I  uint32_t ROLL_A_KP;
	__I  uint32_t ROLL_A_KI;
	__I  uint32_t ROLL_R_IMAX;
	__I  uint32_t ROLL_A_IMAX;
	
	// Yaw regulator Coefficients
	__I  uint32_t YAW_R_KP;
	__I  uint32_t YAW_R_KI;
	__I  uint32_t YAW_A_KP;
	__I  uint32_t YAW_A_KI;
	__I  uint32_t YAW_R_IMAX;
	__I  uint32_t YAW_A_IMAX;
	/* --- END REGULATOR CONSTANTS --- */
	
	/* --- START MIXER CONSTANTS --- */
	/* MIX_CHn[0] is Throttle Gain
	 * MIX_CHn[1] is Pitch Gain
	 * MIX_CHn[2] is Roll Gain
	 * MIX_CHn[3] is Yaw Gain
	 */
	__I  uint32_t MIX_CH1[4];
	__I  uint32_t MIX_CH2[4];
	__I  uint32_t MIX_CH3[4];
	__I  uint32_t MIX_CH4[4];
	__I  uint32_t MIX_CH5[4];
	__I  uint32_t MIX_CH6[4];
	__I  uint32_t MIX_CH7[4];
	__I  uint32_t MIX_CH8[4];
	/* --- END MIXER CONSTANTS --- */
	
	/* --- START RC INPUT CAIBRATION CONSTANTS --- */
	/* Role:
	 * Bit 0-1 defines input channel for YAW
	 * Bit 2-3 defines input channel for ROLL
	 * Bit 4-5 defines input channel for PITCH
	 * Bit 6-7 defines input channel for THROTTLE
	 */
	__I  uint32_t ROLE;
	__I  uint32_t INPUTCAL_CH1[3];
	__I  uint32_t INPUTCAL_CH2[3];
	__I  uint32_t INPUTCAL_CH3[3];
	__I  uint32_t INPUTCAL_CH4[3];
	__I  uint32_t INPUTCAL_CH5[3];
	__I  uint32_t INPUTCAL_CH6[3];
	__I  uint32_t INPUTCAL_CH7[3];
	__I  uint32_t INPUTCAL_CH8[3];
	/* --- END RC INPUT CAIBRATION CONSTANTS --- */
	
} EEMUL_TypeDef;

#define EEMUL_BASE	(0x00078000UL)
#define EEMUL_DATA	((EEMUL_TypeDef *) EEMUL_BASE)

#endif

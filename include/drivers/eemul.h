/* Data in EEPROM Emulation section
 * Data structure:
 * - ID:						1  word
 * - Version:					3 words
 * - Flight Limits:				3 words
 * - PID Data: 					24 words
 * - Mixers Data: 				8 words
 * - Input Calibration Data:	25 words
 * Total size: 64 words = 256 bytes = 1 page
 * */

#include "lpc_types.h"
#include "LPC17xx.h"

#ifndef __EEMUL_H  
#define __EEMUL_H 

typedef struct
{
	// Save ID and Version (strings, 4 characters per word)
	__I  uint32_t ID;
	__I  uint32_t VER[3];
	
	/* --- START REGULATOR CONSTANTS --- */
	// Flight Limits
	__I  uint32_t MAXANGLE;
	__I  uint32_t MAXRATE;
	__I  uint32_t MAXYAWRATE;

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
	/* MIX[n][0] is Throttle Gain
	 * MIX[n][1] is Pitch Gain
	 * MIX[n][2] is Roll Gain
	 * MIX[n][3] is Yaw Gain
	 */
	__I  uint8_t MIX[8][4];
	/* --- END MIXER CONSTANTS --- */
	
	/* --- START RC INPUT CAIBRATION CONSTANTS --- */
	/* Role		(3 bits):
	 * Throttle	(0-2)
	 * Pitch 	(3-5)
	 * Roll		(6-8)
	 * Yaw		(9-11)
	 * Mode		(12-14)
	 * Aux 1	(15-17)
	 * Aux 2	(18-20)
	 * Aux 3	(21-23)
	 * 
	 * INPUTCAL[n][0] is Top
	 * INPUTCAL[n][1] is Center
	 * INPUTCAL[n][2] is Bottom
	 */
	__I  uint32_t INPUT_ROLE;
	__I  uint32_t INPUTCAL[8][3];
	/* --- END RC INPUT CAIBRATION CONSTANTS --- */
	
} EEMUL_TypeDef;

#define EEMUL_BASE	(0x00078000UL) 	// 29th sector
#define EEMUL_DATA	((EEMUL_TypeDef *) EEMUL_BASE)

#endif

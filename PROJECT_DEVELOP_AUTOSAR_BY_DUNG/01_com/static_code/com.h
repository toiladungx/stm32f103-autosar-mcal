#include "com_cfg.h"


// khai bao prototypes
void Com_Init (void);

uint8 Com_SendSignal ( Com_SignalIdType SignalId, const void* SignalDataPtr );

Std_ReturnType Com_TriggerIPDUSend (PduIdType PduId );
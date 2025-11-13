#pragma once

#include "backend.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum GUI_STATUS {
    GUI_CODE_0,
    GUI_ERR_UNKNOWN_COMMAND,
    GUI_ERR_INVALID_PARAMETERS
}   GUI_STATUS;

GUI_STATUS PromptTerminal(  STATUS*             status, 
                            Bucket*             bucket, 
                            Queue*              wrr_queue, 
                            const unsigned int  trafic, 
                            char*               prompt);

GUI_STATUS QueuePrint(      Queue* wrr_queue);

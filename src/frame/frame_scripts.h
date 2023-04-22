#ifndef _FRAME_SCRIPTS_H_
#define _FRAME_SCRIPTS_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"
#include "../utility/astros_script.h"

class Frame_Scripts : public Frame_Base
{
public:
    Frame_Scripts();
    ~Frame_Scripts();
    int init(epdgui_args_vector_t &args);

private:
    int *_page; 
    EPDGUI_Button *_key[9];
    EPDGUI_Button * _back;
    EPDGUI_Button * _forward; 
    M5EPD_Canvas *_canvas_title_buffer  = NULL;
    AstrosScript * _script;
};

#endif //_FRAME_SCRIPTS_H_
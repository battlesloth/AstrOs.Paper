#ifndef _FRAME_ASTROSAPI_H_
#define _FRAME_ASTROSAPI_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"

class Frame_AstrosApi : public Frame_Base {
   public:
    Frame_AstrosApi(bool isHorizontal = false);
    ~Frame_AstrosApi();
    int run();
    int init(epdgui_args_vector_t &args);

   private:
    M5EPD_Canvas *_label_key;
    EPDGUI_Textbox *inputboxkey;
    M5EPD_Canvas *_label_api;
    EPDGUI_Textbox *inputboxapi;
    EPDGUI_Keyboard *keyboard;
    EPDGUI_Button *key_textclear;
    EPDGUI_Button *key_save;
    bool apiActive;
};

#endif  //_FRAME_ASTROSAPI _H_
#include "frame_scripts.h"

#define KEY_W 220
#define KEY_H 130

#define FORWARD_BUTTON 9
#define BACK_BUTTON 10
#define SCRIPTS 11
#define PAGE 12

void key_back_cb(epdgui_args_vector_t &args)
{
    AstrosScript *script  = (AstrosScript *)(args[SCRIPTS]);
   
    script->DecrementPage();

    for (int i = 0; i < 9; i++)
    {
        ((EPDGUI_Button*)(args[i]))->setLabel(script->GetScriptName(i).c_str());
        ((EPDGUI_Button*)(args[i]))->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, static_cast<void*>(new std::string(script->GetScriptValue(i))));
        ((EPDGUI_Button*)(args[i]))->Draw(UPDATE_MODE_A2);
    } 
}

void key_forward_cb(epdgui_args_vector_t &args)
{
    AstrosScript *script  = (AstrosScript *)(args[SCRIPTS]);

    script->IncrementPage();

    for (int i = 0; i < 9; i++)
    {
        ((EPDGUI_Button*)(args[i]))->setLabel(script->GetScriptName(i).c_str());
        ((EPDGUI_Button*)(args[i]))->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, static_cast<void*>(new std::string(script->GetScriptValue(i))));
        ((EPDGUI_Button*)(args[i]))->Draw(UPDATE_MODE_A2);
    }
}

void key_script_cb(epdgui_args_vector_t &args){
    std::string *cmdText = static_cast<std::string*>(args[0]);
    Serial.println(cmdText->c_str());
}

Frame_Scripts::Frame_Scripts(void)
{
    _frame_name = "Frame_Scripts";
 
    _page = 0;

    _script = new AstrosScript();

    _canvas_title_buffer = new M5EPD_Canvas(&M5.EPD);
    _canvas_title_buffer->createCanvas(420, 64);
    _canvas_title_buffer->drawFastHLine(0, 64, 420, 15);
    _canvas_title_buffer->drawFastHLine(0, 63, 420, 15);
    _canvas_title_buffer->drawFastHLine(0, 62, 420, 15);

    for (int i = 0; i < 3; i++)
    {
        _key[i] = new EPDGUI_Button(_script->GetScriptName(i).c_str(), 140 + (i * 230), 84, KEY_W, KEY_H);
        _key[i]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, static_cast<void*>(new std::string(_script->GetScriptValue(i))));
        _key[i]->Bind(EPDGUI_Button::EVENT_RELEASED, key_script_cb);
    }

    for (int i = 0; i < 3; i++)
    {
        _key[i + 3] = new EPDGUI_Button(_script->GetScriptName(i + 3).c_str(), 140 + (i * 230), 237, KEY_W, KEY_H);
        _key[i + 3]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, static_cast<void*>(new std::string(_script->GetScriptValue(i + 3))));
        _key[i + 3]->Bind(EPDGUI_Button::EVENT_RELEASED, key_script_cb);
    }

    for (int i = 0; i < 3; i++)
    {
        _key[i + 6] = new EPDGUI_Button(_script->GetScriptName(i + 6).c_str(), 140 + (i * 230), 384, KEY_W, KEY_H);
        _key[i + 6]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, static_cast<void*>(new std::string(_script->GetScriptValue(i + 6))));
        _key[i + 6]->Bind(EPDGUI_Button::EVENT_RELEASED, key_script_cb);
    }

    _back = new EPDGUI_Button("B", 8, 84, 122, 430);
    _forward = new EPDGUI_Button("F", 830, 84, 122, 430);

    for (int i = 0; i < 9; i++)
    {
        _back->AddArgs(EPDGUI_Button::EVENT_RELEASED, i, _key[i]);
        _forward->AddArgs(EPDGUI_Button::EVENT_RELEASED, i, _key[i]);
    }

    _back->AddArgs(EPDGUI_Button::EVENT_RELEASED, FORWARD_BUTTON, _forward);
    _back->AddArgs(EPDGUI_Button::EVENT_RELEASED, BACK_BUTTON, _back);
    _back->AddArgs(EPDGUI_Button::EVENT_RELEASED, SCRIPTS, _script);
    _back->AddArgs(EPDGUI_Button::EVENT_RELEASED, PAGE, (void *)0);
    _back->Bind(EPDGUI_Button::EVENT_RELEASED, key_back_cb);
    
    _forward->AddArgs(EPDGUI_Button::EVENT_RELEASED, FORWARD_BUTTON, _forward);
    _forward->AddArgs(EPDGUI_Button::EVENT_RELEASED, BACK_BUTTON, _back);
    _forward->AddArgs(EPDGUI_Button::EVENT_RELEASED, SCRIPTS, _script);
    _forward->AddArgs(EPDGUI_Button::EVENT_RELEASED, PAGE, (void *)0);
    _forward->Bind(EPDGUI_Button::EVENT_RELEASED, key_forward_cb);
    
    exitbtn("Home");
    _canvas_title->drawString("Control Panel", 270, 34);

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void *)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &Frame_Base::exit_cb);
}

Frame_Scripts::~Frame_Scripts(void)
{
    for (int i = 0; i < 9; i++)
    {
        delete _key[i];
    }
    delete _canvas_title_buffer;
    delete _forward;
    delete _back;
    delete _page;
    delete _script;
}

int Frame_Scripts::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    M5.EPD.SetRotation(M5EPD_Driver::ROTATE_0);
    M5.TP.SetRotation(GT911::ROTATE_0);
    M5.EPD.Clear(false);
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    _canvas_title_buffer->pushCanvas(540, 8, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_back);
    for (int i = 0; i < 9; i++)
    {
        EPDGUI_AddObject(_key[i]);
    }
    EPDGUI_AddObject(_forward);
    EPDGUI_AddObject(_key_exit);
    return 3;
}

#include "frame_astrosapi.h"
#include "global_setting.h"


void key_clear_cb(epdgui_args_vector_t &args)
{
    ((EPDGUI_Textbox *)(args[0]))->SetText("");
    ((EPDGUI_Textbox *)(args[1]))->SetText("");
}

void key_save_cb(epdgui_args_vector_t &args)
{
    String api = ((EPDGUI_Textbox *)(args[0]))->GetText();
    String key = ((EPDGUI_Textbox *)(args[1]))->GetText();
    
    SetAstrosKey(key);
    SetAstrosApi(api);
    ((EPDGUI_Textbox *)(args[0]))->SetText("");
    ((EPDGUI_Textbox *)(args[1]))->SetText("");
    EPDGUI_PopFrame();
    *((int *)(args[2])) = 0;
}

Frame_AstrosApi::Frame_AstrosApi(bool isHorizontal) : Frame_Base()
{
    _frame_name = "Frame_AstrosApi";

    _label_api = new M5EPD_Canvas(&M5.EPD);
    _label_api->createCanvas(540, 60);
    _label_api->fillCanvas(0);
    _label_api->setTextSize(26);
    _label_api->setTextColor(15);
    _label_api->setTextDatum(CL_DATUM);
    _label_api->drawString("AstrOs IP", 15, 35);

    _label_key = new M5EPD_Canvas(&M5.EPD);
    _label_key->createCanvas(540, 60);
    _label_key->fillCanvas(0);
    _label_key->setTextSize(26);
    _label_key->setTextColor(15);
    _label_key->setTextDatum(CL_DATUM);
    _label_key->drawString("API Key", 15, 35);

    const uint16_t kKeyBaseY = 176;
    inputboxapi = new EPDGUI_Textbox(160, 100, 360, 60);
    inputboxkey = new EPDGUI_Textbox(160, 176, 360, 60);
    key_textclear = new EPDGUI_Button("Clear", 4, 253, 260, 52);
    key_save = new EPDGUI_Button("Save", 272, 253, 260, 52);

    inputboxapi->SetTextMargin(8, 15, 8, 8);
    inputboxapi->SetState(EPDGUI_Textbox::EVENT_PRESSED);

    inputboxkey->SetTextMargin(8, 15, 8, 8);
    inputboxkey->SetState(EPDGUI_Textbox::EVENT_NONE);

    keyboard = new EPDGUI_Keyboard(isHorizontal, EPDGUI_Keyboard::STYLE_INPUTMODE_NEEDCONFIRM);

    key_textclear->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void *)inputboxapi);
    key_textclear->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, (void *)inputboxkey);
    key_textclear->Bind(EPDGUI_Button::EVENT_RELEASED, key_clear_cb);

    key_save->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void *)inputboxapi);
    key_save->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, (void *)inputboxkey);
    key_save->AddArgs(EPDGUI_Button::EVENT_RELEASED, 2, (void *)(&_is_run));
    key_save->Bind(EPDGUI_Button::EVENT_RELEASED, key_save_cb);

    exitbtn("Settings");
    _canvas_title->drawString("AstrOs", 270, 34);

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void *)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &Frame_Base::exit_cb);
}

Frame_AstrosApi::~Frame_AstrosApi()
{
    delete inputboxapi;
    delete inputboxkey;
    delete keyboard;
    delete key_textclear;
    delete key_save;
}

int Frame_AstrosApi::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    M5.EPD.Clear();
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    _label_api->pushCanvas(0, 100, UPDATE_MODE_NONE);
    _label_key->pushCanvas(0, 176, UPDATE_MODE_NONE);
    EPDGUI_AddObject(inputboxapi);
    EPDGUI_AddObject(inputboxkey);
    EPDGUI_AddObject(keyboard);
    EPDGUI_AddObject(_key_exit);
    EPDGUI_AddObject(key_textclear);
    EPDGUI_AddObject(key_save);
    inputboxapi->SetText(GetAstrosApi());
    inputboxkey->SetText(GetAstrosKey());
    return 6;
}

int Frame_AstrosApi::run(void)
{
    Frame_Base::run();
    String data = keyboard->getData();

    if (inputboxapi->isSelected())
    {
        if (data.indexOf("\n") >= 0)
        {
            inputboxkey->SetState(EPDGUI_Textbox::EVENT_PRESSED);
            inputboxapi->SetState(EPDGUI_Textbox::EVENT_NONE); 
        }
        else
        {
            inputboxapi->AddText(data);
        }
    }
    else if (inputboxkey->isSelected())
    {
        if (data.indexOf("\n") >= 0)
        {
            inputboxapi->SetState(EPDGUI_Textbox::EVENT_PRESSED);
            inputboxkey->SetState(EPDGUI_Textbox::EVENT_NONE); 
        }
        else
        {
            inputboxkey->AddText(data);
        }
    }

    return 1;
}

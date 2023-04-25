#include "frame_scripts.h"
#include <HTTPClient.h>
#include "WiFi.h"

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

    if (cmdText->compare("0") == 0){
        Serial.println("Empty Script");
        return;
    }

    M5EPD_Canvas info(&M5.EPD);
    
    info.createCanvas(300, 100);
    info.fillCanvas(15);
    info.setTextSize(26);
    info.setTextColor(0);
    info.setTextDatum(CC_DATUM);
    
    if (WiFi.status() != WL_CONNECTED)
    {
        info.drawString("WiFi not connected", 150, 55);
        info.pushCanvas(430, 120, UPDATE_MODE_GL16);
        EPDGUI_Draw(UPDATE_MODE_NONE);
        while (!M5.TP.avaliable())
            ;
        M5.EPD.UpdateFull(UPDATE_MODE_GL16);
        return;
    }

    LoadingAnime_32x32_Start(532 - 15 - 32, 160 + 14);
    
    HTTPClient http;

    http.begin("http://" + GetAstrosApi() + "/api/remotecontrol?id=" + cmdText->c_str());

    http.addHeader("x-token", GetAstrosKey());

    int respCode = http.GET();

    LoadingAnime_32x32_Stop();

    if (respCode != 200)
    {
         http.end();
        info.drawString("Command Failed!", 150, 55);
        info.pushCanvas(430, 120, UPDATE_MODE_GL16);
        EPDGUI_Draw(UPDATE_MODE_NONE);
        while (!M5.TP.avaliable())
            ;
        M5.EPD.UpdateFull(UPDATE_MODE_GL16);
        return;
    }
    else
    {
        String payload = http.getString();

        Serial.write(payload.c_str());
    }

    http.end();
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
        _key[i] = new EPDGUI_Button("empty", 140 + (i * 230), 84, KEY_W, KEY_H);
        _key[i]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, static_cast<void*>(new std::string("none")));
        _key[i]->Bind(EPDGUI_Button::EVENT_RELEASED, key_script_cb);
    }

    for (int i = 0; i < 3; i++)
    {
        _key[i + 3] = new EPDGUI_Button("empty", 140 + (i * 230), 237, KEY_W, KEY_H);
        _key[i + 3]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, static_cast<void*>(new std::string("none")));
        _key[i + 3]->Bind(EPDGUI_Button::EVENT_RELEASED, key_script_cb);
    }

    for (int i = 0; i < 3; i++)
    {
        _key[i + 6] = new EPDGUI_Button("empty", 140 + (i * 230), 384, KEY_W, KEY_H);
        _key[i + 6]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, static_cast<void*>(new std::string("none")));
        _key[i + 6]->Bind(EPDGUI_Button::EVENT_RELEASED, key_script_cb);
    }

    _lpanic = new EPDGUI_Button("panic",8, 84, 122, KEY_H);
    _lpanic->CanvasNormal()->pushImage(
        15, 19, 92, 92, IconResource_warning_92x92);
    *(_lpanic->CanvasPressed()) = *(_lpanic->CanvasNormal());
    _lpanic->CanvasPressed()->ReverseColor();
    _lpanic->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, static_cast<void*>(new std::string("panic")));
    _lpanic->Bind(EPDGUI_Button::EVENT_RELEASED, key_script_cb);
    
    _rpanic = new EPDGUI_Button("panic", 830, 84, 122, KEY_H);
    _rpanic->CanvasNormal()->pushImage(
        15, 19, 92, 92, IconResource_warning_92x92);
    *(_rpanic->CanvasPressed()) = *(_rpanic->CanvasNormal());
    _rpanic->CanvasPressed()->ReverseColor();
    _rpanic->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, static_cast<void*>(new std::string("panic")));
    _rpanic->Bind(EPDGUI_Button::EVENT_RELEASED, key_script_cb);
    
    _back = new EPDGUI_Button("B", 8, 237, 122, 280);
    _back->CanvasNormal()->pushImage(
        15, 94, 92, 92, IconResource_back_92x92);
    *(_back->CanvasPressed()) = *(_back->CanvasNormal());
    _back->CanvasPressed()->ReverseColor();

    _forward = new EPDGUI_Button("F", 830, 237, 122, 280);
    _forward->CanvasNormal()->pushImage(
        15, 94, 92, 92, IconResource_forward_92x92);
    *(_forward->CanvasPressed()) = *(_forward->CanvasNormal());
    _forward->CanvasPressed()->ReverseColor();

    for (int i = 0; i < 9; i++)
    {
        _back->AddArgs(EPDGUI_Button::EVENT_RELEASED, i, _key[i]);
        _forward->AddArgs(EPDGUI_Button::EVENT_RELEASED, i, _key[i]);
    }

    _back->AddArgs(EPDGUI_Button::EVENT_RELEASED, FORWARD_BUTTON, _forward);
    _back->AddArgs(EPDGUI_Button::EVENT_RELEASED, BACK_BUTTON, _back);
    _back->AddArgs(EPDGUI_Button::EVENT_RELEASED, SCRIPTS, _script);
    _back->Bind(EPDGUI_Button::EVENT_RELEASED, key_back_cb);
    
    _forward->AddArgs(EPDGUI_Button::EVENT_RELEASED, FORWARD_BUTTON, _forward);
    _forward->AddArgs(EPDGUI_Button::EVENT_RELEASED, BACK_BUTTON, _back);
    _forward->AddArgs(EPDGUI_Button::EVENT_RELEASED, SCRIPTS, _script);
    _forward->Bind(EPDGUI_Button::EVENT_RELEASED, key_forward_cb);
    
    exitbtn("Home");
    _canvas_title->drawString("Scrpits", 270, 34);

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
    delete _rpanic;
    delete _lpanic;
    delete _forward;
    delete _back;
    delete _page;
    delete _script;
}

int Frame_Scripts::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    _script->ParseScript(GetScripts());

    M5.EPD.SetRotation(M5EPD_Driver::ROTATE_0);
    M5.TP.SetRotation(GT911::ROTATE_0);
    M5.EPD.Clear(false);
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    _canvas_title_buffer->pushCanvas(540, 8, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_lpanic);
    EPDGUI_AddObject(_back);
    for (int i = 0; i < 9; i++)
    {
        _key[i]->setLabel(_script->GetScriptName(i).c_str());
        _key[i]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, static_cast<void*>(new std::string(_script->GetScriptValue(i))));
        EPDGUI_AddObject(_key[i]);
    }
    EPDGUI_AddObject(_forward);
    EPDGUI_AddObject(_rpanic);
    EPDGUI_AddObject(_key_exit);
    return 3;
}

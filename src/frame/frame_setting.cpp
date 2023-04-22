#include "frame_setting.h"
#include "frame_astrosapi.h"
#include "frame_base.h"
#include "frame_home.h"
#include <HTTPClient.h>
#include "WiFi.h"

#define KEY_W 92
#define KEY_H 92
const uint16_t kTimeZoneY = 400;

void key_shutdown_cb(epdgui_args_vector_t &args)
{
    Shutdown();
}

void key_restart_cb(epdgui_args_vector_t &args)
{
    M5.EPD.WriteFullGram4bpp(WallPaperResource_r2_540x960);
    M5.EPD.UpdateFull(UPDATE_MODE_GC16);
    SaveSetting();
    esp_restart();
}

void key_astrosapi_cb(epdgui_args_vector_t &args)
{
    Frame_Base *frame = EPDGUI_GetFrame("Frame_AstroApi");
    if (frame == NULL)
    {
        frame = new Frame_AstrosApi(false);
        EPDGUI_AddFrame("Frame_AstroApi", frame);
    }
    EPDGUI_PushFrame(frame);
    *((int *)(args[0])) = 0;   
}

void key_scriptsync_cb(epdgui_args_vector_t &args)
{
    M5EPD_Canvas info(&M5.EPD);
    M5EPD_Canvas *title = (M5EPD_Canvas *)(args[0]);
    info.createCanvas(300, 100);
    info.fillCanvas(15);
    info.setTextSize(26);
    info.setTextColor(0);
    info.setTextDatum(CC_DATUM);
    if (WiFi.status() != WL_CONNECTED)
    {
        info.drawString("WiFi not connected", 150, 55);
        info.pushCanvas(120, 430, UPDATE_MODE_GL16);
        M5.EPD.WriteFullGram4bpp(WallPaperResource_r2_540x960);
        title->pushCanvas(0, 8, UPDATE_MODE_NONE);
        EPDGUI_Draw(UPDATE_MODE_NONE);
        while (!M5.TP.avaliable())
            ;
        M5.EPD.UpdateFull(UPDATE_MODE_GL16);
        return;
    }
    LoadingAnime_32x32_Start(532 - 15 - 32, 160 + 14);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    HTTPClient http;

    
    http.begin("http://" + GetAstrosApi() + "/api/remotecontrolsync");

    http.addHeader("x-token", GetAstrosKey());

    int respCode = http.GET();

    LoadingAnime_32x32_Stop();

    if (respCode != 200)
    {
        info.drawString("Script sync failed", 150, 55);
        info.pushCanvas(120, 430, UPDATE_MODE_GL16);
    }
    else
    {
        String payload = http.getString();

        Serial.write(payload.c_str());
        info.drawString("Success", 150, 55);
        info.pushCanvas(120, 430, UPDATE_MODE_GL16);
    }
    M5.EPD.WriteFullGram4bpp(WallPaperResource_r2_540x960);
    title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    EPDGUI_Draw(UPDATE_MODE_NONE);
    while (!M5.TP.avaliable())
        ;
    M5.EPD.UpdateFull(UPDATE_MODE_GL16);
}

void key_synctime_cb(epdgui_args_vector_t &args)
{
    SaveSetting();
    M5EPD_Canvas info(&M5.EPD);
    M5EPD_Canvas *title = (M5EPD_Canvas *)(args[0]);
    M5EPD_Canvas *tzone = (M5EPD_Canvas *)(args[1]);
    info.createCanvas(300, 100);
    info.fillCanvas(15);
    info.setTextSize(26);
    info.setTextColor(0);
    info.setTextDatum(CC_DATUM);
    if (WiFi.status() != WL_CONNECTED)
    {

        info.drawString("WiFi not connected", 150, 55);
        info.pushCanvas(120, 430, UPDATE_MODE_GL16);
        M5.EPD.WriteFullGram4bpp(WallPaperResource_r2_540x960);
        title->pushCanvas(0, 8, UPDATE_MODE_NONE);
        tzone->pushCanvas(4, kTimeZoneY, UPDATE_MODE_NONE);
        EPDGUI_Draw(UPDATE_MODE_NONE);
        while (!M5.TP.avaliable())
            ;
        M5.EPD.UpdateFull(UPDATE_MODE_GL16);
        return;
    }
    LoadingAnime_32x32_Start(532 - 15 - 32, 220 + 14);
    bool ret = SyncNTPTime();
    LoadingAnime_32x32_Stop();

    if (ret == 0)
    {
        info.drawString("Time sync failed", 150, 55);
        info.pushCanvas(120, 430, UPDATE_MODE_GL16);
    }
    else
    {
        info.drawString("Success", 150, 55);
        info.pushCanvas(120, 430, UPDATE_MODE_GL16);
    }
    M5.EPD.WriteFullGram4bpp(WallPaperResource_r2_540x960);
    title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    tzone->pushCanvas(4, kTimeZoneY, UPDATE_MODE_NONE);
    EPDGUI_Draw(UPDATE_MODE_NONE);
    while (!M5.TP.avaliable())
        ;
    M5.EPD.UpdateFull(UPDATE_MODE_GL16);
}

void key_timezone_plus_cb(epdgui_args_vector_t &args)
{
    int *tz = (int *)(args[0]);
    (*tz)++;
    if ((*tz) > 12)
    {
        (*tz) = 12;
    }
    String str = String(*tz);
    if ((*tz) > 0)
    {
        str = "+" + str;
    }
    ((EPDGUI_Button *)(args[1]))->setLabel(str);
    ((EPDGUI_Button *)(args[1]))->Draw(UPDATE_MODE_GL16);
    SetTimeZone(*tz);
}

void key_timezone_minus_cb(epdgui_args_vector_t &args)
{
    int *tz = (int *)(args[0]);
    (*tz)--;
    if ((*tz) < -11)
    {
        (*tz) = -11;
    }
    String str = String(*tz);
    if ((*tz) > 0)
    {
        str = "+" + str;
    }
    ((EPDGUI_Button *)(args[1]))->setLabel(str);
    ((EPDGUI_Button *)(args[1]))->Draw(UPDATE_MODE_GL16);
    SetTimeZone(*tz);
}

void key_timezone_reset_cb(epdgui_args_vector_t &args)
{
    int *tz = (int *)(args[0]);
    (*tz) = 0;
    ((EPDGUI_Button *)(args[1]))->setLabel(String(*tz));
    ((EPDGUI_Button *)(args[1]))->Draw(UPDATE_MODE_GL16);
    SetTimeZone(*tz);
}

Frame_Setting::Frame_Setting(void)
{
    _frame_name = "Frame_Setting";

    _timezone_canvas = new M5EPD_Canvas(&M5.EPD);
    _timezone_canvas->createCanvas(540, 60);
    _timezone_canvas->fillCanvas(0);
    _timezone_canvas->setTextSize(26);
    _timezone_canvas->setTextColor(15);
    _timezone_canvas->setTextDatum(CL_DATUM);
    _key_astrosapi = new EPDGUI_Button(4, 100, 532, 61);
    _key_scriptsync = new EPDGUI_Button(4, 160, 532, 61);
    _key_syncntp = new EPDGUI_Button(4, 220, 532, 61);
    _key_restart = new EPDGUI_Button(4, 280, 532, 61);
    _key_shutdown = new EPDGUI_Button(4, 340, 532, 61);

    key_timezone_plus = new EPDGUI_Button("+", 448, kTimeZoneY, 88, 52);
    String str = String(GetTimeZone());
    if (GetTimeZone() > 0)
    {
        str = "+" + str;
    }
    key_timezone_reset = new EPDGUI_Button(str, 360, kTimeZoneY, 88, 52);
    key_timezone_minus = new EPDGUI_Button("-", 272, kTimeZoneY, 88, 52);

    key_timezone_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, &_timezone);
    key_timezone_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1,
                               key_timezone_reset);
    key_timezone_plus->Bind(EPDGUI_Button::EVENT_RELEASED,
                            key_timezone_plus_cb);

    key_timezone_reset->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, &_timezone);
    key_timezone_reset->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1,
                                key_timezone_reset);
    key_timezone_reset->Bind(EPDGUI_Button::EVENT_RELEASED,
                             key_timezone_reset_cb);

    key_timezone_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, &_timezone);
    key_timezone_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1,
                                key_timezone_reset);
    key_timezone_minus->Bind(EPDGUI_Button::EVENT_RELEASED,
                             key_timezone_minus_cb);

    _key_scriptsync->setBMPButton("  Script Sync", "",
                              ImageResource_item_icon_refresh_32x32);
    _key_astrosapi->setBMPButton("  AstrOs API", "",
                              ImageResource_item_icon_key_32x32);
    _key_syncntp->setBMPButton("  Sync Time", "",
                               ImageResource_item_icon_ntptime_32x32);
    _key_restart->setBMPButton("  Restart", "",
                               ImageResource_item_icon_restart_32x32);
    _key_shutdown->setBMPButton("  Shutdown", "",
                                ImageResource_item_icon_shutdown_32x32);
    _timezone_canvas->drawString("Time zone (UTC)", 15, 35);
    exitbtn("Home");
    _canvas_title->drawString("Setting", 270, 34);

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void *)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &Frame_Base::exit_cb);
    _key_shutdown->Bind(EPDGUI_Button::EVENT_RELEASED, &key_shutdown_cb);
    _key_restart->Bind(EPDGUI_Button::EVENT_RELEASED, &key_restart_cb);
    _key_syncntp->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _canvas_title);
    _key_syncntp->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, _timezone_canvas);
    _key_syncntp->Bind(EPDGUI_Button::EVENT_RELEASED, &key_synctime_cb);
    _key_astrosapi->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void *)(&_is_run));
    _key_astrosapi->Bind(EPDGUI_Button::EVENT_RELEASED, &key_astrosapi_cb);
    _key_scriptsync->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _canvas_title);
    _key_scriptsync->Bind(EPDGUI_Button::EVENT_RELEASED, &key_scriptsync_cb);
    
    _timezone = GetTimeZone();
}

Frame_Setting::~Frame_Setting(void)
{
    delete _key_shutdown;
    delete _key_restart;
    delete _key_syncntp;
    delete _key_astrosapi;
    delete _key_scriptsync;
}

int Frame_Setting::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    M5.EPD.WriteFullGram4bpp(WallPaperResource_r2_540x960);
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    _timezone_canvas->pushCanvas(0, kTimeZoneY, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_key_shutdown);
    EPDGUI_AddObject(_key_restart);
    EPDGUI_AddObject(_key_syncntp);
    EPDGUI_AddObject(_key_astrosapi);
    EPDGUI_AddObject(_key_scriptsync);
    EPDGUI_AddObject(_key_exit);
    EPDGUI_AddObject(key_timezone_plus);
    EPDGUI_AddObject(key_timezone_reset);
    EPDGUI_AddObject(key_timezone_minus);
    return 3;
}
#include <functional>
#include "frame_fileindex.h"

#define MAX_BTN_NUM 14

void key_fileindex_floder_cb(epdgui_args_vector_t &args)
{
    Frame_Base *frame =
        new Frame_FileIndex(((EPDGUI_Button *)(args[0]))->GetCustomString());
    EPDGUI_PushFrame(frame);
    *((int *)(args[1])) = 0;
    log_d("%s", ((EPDGUI_Button *)(args[0]))->GetCustomString().c_str());
}

void key_fileindex_exit_cb(epdgui_args_vector_t &args)
{
    EPDGUI_PopFrame(true);
    *((int *)(args[0])) = 0;
}

Frame_FileIndex::Frame_FileIndex(String path)
{
    _frame_name = "Frame_FileIndex";
    _path = path;

    uint8_t language = GetLanguage();
    _canvas_title->setTextDatum(CR_DATUM);
    if (path == "/")
    {

        exitbtn("Home");

        _canvas_title->drawString("SD/", 540 - 15, 34);
    }
    else
    {
        String parent_directory = path.substring(0, path.lastIndexOf("/"));

        exitbtn("/..");
        String subpath = path;
        if (path.length() > 20)
        {
            subpath = path.substring(0, 20) + "...";
        }
        _canvas_title->drawString("SD" + subpath, 540 - 15, 34);
    }

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void *)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &key_fileindex_exit_cb);
}

void Frame_FileIndex::listDir(fs::FS &fs, const char *dirname)
{
    File root = fs.open(dirname);
    if (!root)
    {
        log_d("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        log_d("Not a directory");
        return;
    }

    std::vector<String> floders;
    std::vector<std::pair<String, size_t>> files;

    for (File file = root.openNextFile(); file; file = root.openNextFile())
    {
        if (file.isDirectory())
        {
            floders.push_back(String(file.name()));
        }
        else
        {
            files.push_back(std::make_pair(String(file.name()), file.size()));
        }
    }

    for (int n = 0; n < floders.size(); n++)
    {
        if (_key_files.size() > MAX_BTN_NUM)
        {
            break;
        }
        EPDGUI_Button *btn =
            new EPDGUI_Button(4, 100 + _key_files.size() * 60, 532, 61);
        _key_files.push_back(btn);

        String filename = floders[n];
        filename = filename.substring(filename.lastIndexOf("/") + 1);
        if (filename.length() > 19)
        {
            filename = filename.substring(0, 19) + "...";
        }
        btn->CanvasNormal()->fillCanvas(0);
        btn->CanvasNormal()->drawRect(0, 0, 532, 61, 15);
        btn->CanvasNormal()->setTextSize(26);
        btn->CanvasNormal()->setTextDatum(CL_DATUM);
        btn->CanvasNormal()->setTextColor(15);
        btn->CanvasNormal()->drawString(filename, 47 + 13, 35);
        btn->SetCustomString(floders[n]);
        btn->CanvasNormal()->setTextDatum(CR_DATUM);
        btn->CanvasNormal()->pushImage(
            15, 14, 32, 32, ImageResource_item_icon_file_floder_32x32);
        btn->CanvasNormal()->pushImage(532 - 15 - 32, 14, 32, 32,
                                       ImageResource_item_icon_arrow_r_32x32);
        *(btn->CanvasPressed()) = *(btn->CanvasNormal());
        btn->CanvasPressed()->ReverseColor();

        btn->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, btn);
        btn->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, (void *)(&_is_run));
        btn->Bind(EPDGUI_Button::EVENT_RELEASED, key_fileindex_floder_cb);
    }

    for (int n = 0; n < files.size(); n++)
    {
        if (_key_files.size() > MAX_BTN_NUM)
        {
            break;
        }
        EPDGUI_Button *btn =
            new EPDGUI_Button(4, 100 + _key_files.size() * 60, 532, 61);
        _key_files.push_back(btn);

        String filename = files[n].first;
        filename = filename.substring(filename.lastIndexOf("/") + 1);
        if (filename.length() > 19)
        {
            filename = filename.substring(0, 19) + "...";
        }
        btn->CanvasNormal()->fillCanvas(0);
        btn->CanvasNormal()->drawRect(0, 0, 532, 61, 15);
        btn->CanvasNormal()->setTextSize(26);
        btn->CanvasNormal()->setTextDatum(CL_DATUM);
        btn->CanvasNormal()->setTextColor(15);
        btn->CanvasNormal()->drawString(filename, 47 + 13, 35);
        btn->SetCustomString(files[n].first);
        btn->CanvasNormal()->setTextDatum(CR_DATUM);

        String suffix = filename.substring(filename.lastIndexOf("."));

        btn->CanvasNormal()->pushImage(
            15, 14, 32, 32, ImageResource_item_icon_file_unknow_32x32);
        btn->SetEnable(false);

        size_t filesize = files[n].second;
        char buf[50];
        sprintf(buf, "%.2f KiB", filesize / 1024.0f);
        btn->CanvasNormal()->drawString(buf, 532 - 15, 35);
        *(btn->CanvasPressed()) = *(btn->CanvasNormal());
        btn->CanvasPressed()->ReverseColor();
    }
}

Frame_FileIndex::~Frame_FileIndex(void)
{
    for (int i = 0; i < _key_files.size(); i++)
    {
        delete _key_files[i];
    }
}

int Frame_FileIndex::init(epdgui_args_vector_t &args)
{
    _is_run = 1;

    if (_key_files.size() == 0)
    {
        listDir(SD, _path.c_str());
    }

    M5.EPD.WriteFullGram4bpp(GetWallpaper());
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_key_exit);

    for (int i = 0; i < _key_files.size(); i++)
    {
        EPDGUI_AddObject(_key_files[i]);
    }

    return 3;
}

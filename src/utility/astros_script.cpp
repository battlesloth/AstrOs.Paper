#include "astros_script.h" 
#include "cJSON.h"
#include <M5EPD.h>

AstrosScript::AstrosScript() {

    /*std::vector<script_cmd> page1 = {{"name1-1","abc1-1"},{"name1-2","abc1-2"},{"name1-3","abc1-3"},{"name1-4","abc1-4"},{"name1-5","abc1-5"},{"name1-6","abc1-6"},{"name1-7","abc1-7"},{"name1-8","abc1-8"},{"name1-9","abc1-9"},};
    std::vector<script_cmd> page2 = {{"name2-1","abc2-1"},{"name2-2","abc2=2"},{"name2-3","abc2-3"},{"name2-4","abc2-4"},{"name2-5","abc2-5"},{"name2-6","abc2-6"},{"name2-7","abc2-7"},{"name2-8","abc2-8"},{"name2-9","abc2-9"},};
    std::vector<script_cmd> page3 = {{"name3-1","abc3-1"},{"name3-2","abc3-2"},{"name3-3","abc3-3"},{"name3-4","abc3-4"},{"name3-5","abc3-5"},{"name3-6","abc3-6"},{"name3-7","abc3-7"},{"name3-8","abc3-8"},{"name3-9","abc3-9"},};

    AstrosScript::_scripts.push_back(page1);
    AstrosScript::_scripts.push_back(page2);
    AstrosScript::_scripts.push_back(page3);
    */
    _page = 0;

}

AstrosScript::~AstrosScript() {}


void AstrosScript::ParseScript(String blob){

	cJSON *root = cJSON_Parse(blob.c_str());
	
    const cJSON *pages = NULL;    
    const cJSON *page = NULL;
    const cJSON *button = NULL;

    pages = cJSON_GetObjectItem(root, "pages");

    cJSON_ArrayForEach(page, pages){

        std::vector<script_cmd> buttonPage;

        cJSON_ArrayForEach(button, page){
            
            std::string name = cJSON_GetObjectItem(button, "name")->valuestring;
            std::string command = cJSON_GetObjectItem(button, "command")->valuestring;
            
            buttonPage.push_back({name, command});
        }

        if (buttonPage.size() < 9){
            for ( size_t i = buttonPage.size(); i < 9; i++)
            {
                buttonPage.push_back({"None", "0"});
            }
        }

        _scripts.push_back(buttonPage);
    }

    cJSON_Delete(root);
    
    if (_scripts.size() < 1){
         std::vector<script_cmd> emptyPage = {
            {"None","0"},{"None","0"},{"None","0"},
            {"None","0"},{"None","0"},{"None","0"},
            {"None","0"},{"None","0"},{"None","0"}};
    
        _scripts.push_back(emptyPage);
    }

    _page = 0;
}

void AstrosScript::IncrementPage(){
    _page++;

    if (_page >= _scripts.size()){
        _page = 0;
    }
}

void AstrosScript::DecrementPage(){
    _page--;

    if (_page < 0){
        _page = _scripts.size() - 1;
    }    
}

int AstrosScript::GetPageCount(){
     return _scripts.size();     
}

std::string AstrosScript::GetScriptName(int button){
    return _scripts[_page][button].name;
}

std::string AstrosScript::GetScriptValue(int button){
    return _scripts[_page][button].command;
}
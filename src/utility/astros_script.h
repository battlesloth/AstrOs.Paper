#ifndef __ASTROSSCRIPT_H
#define __ASTROSSCRIPT_H

#include <vector>
#include <string>
#include "astros_paper_util.h"

class AstrosScript{
  public:
    AstrosScript();
    ~AstrosScript();
    void ParseScript(std::string blob);
    void IncrementPage();
    void DecrementPage();
    int GetPageCount();
    std::string GetScriptName(int button);
    std::string GetScriptValue(int button);
private:
   std::vector<std::vector<script_cmd>> _scripts;
   int _page;
};


#endif  //__ASTROSSCRIPT_H
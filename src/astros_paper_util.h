#ifndef __ASTROSPAPERUTIL_H
#define __ASTROSPAPERUTIL_H

struct script_cmd
{
    std::string name;
    std::string command;
    script_cmd(std::string val1, std::string val2) : name(val1), command(val2){}
};


#endif  //__ASTROSPAPERUTIL_H
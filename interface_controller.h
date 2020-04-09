//
// Created by 谢仲涛 on 2020/4/6.
//

#ifndef OSIER_INTERFACE_CONTROLLER_H
#define OSIER_INTERFACE_CONTROLLER_H

#include <string>

class interface_controller {
public:
    virtual void cb_done(int id)=0;
    virtual void err_done(int id, const std::string &msg)=0;
};


#endif //OSIER_INTERFACE_CONTROLLER_H

#pragma once
#include "Singleton.h"
#include "Workspace.h"

class MainWorkspace :
    public Singleton<MainWorkspace>
{
private:
    friend class Singleton<MainWorkspace>;
private:
    Workspace* workspace_;
protected:
    MainWorkspace()
    {
        workspace_ = new Workspace(L"Main_");
    }

    virtual ~MainWorkspace()
    {
        delete workspace_;
    }
public:
    Workspace* GetWorkspace() { return workspace_; }
    
};


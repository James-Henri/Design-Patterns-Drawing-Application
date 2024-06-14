#ifndef ECCommand_h
#define ECCommand_h
#include <vector>

// **********************************************************
// Abstract Command Class so each shape has its own Undo and Redo 

class ECCreateShapeCommand
{
public: 
    virtual ~ECCreateShapeCommand() {}
    virtual void Execute() = 0;
    virtual void UnExecute() = 0;

};

class ECCommandHistory
{
public:
    ECCommandHistory();
    virtual ~ECCommandHistory();
    bool Undo();
    bool Redo();
    void ExecuteCmd( ECCreateShapeCommand *pCmd );
    
private:
    // your code goes here
    std::vector<ECCreateShapeCommand *> commandHistory;
    int index;
};

#endif
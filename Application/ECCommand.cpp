#include "ECCommand.h"
#include <iostream>

ECCommandHistory :: ECCommandHistory() : index(-1), commandHistory({}) {}

ECCommandHistory :: ~ECCommandHistory()
{
  for(auto x : commandHistory)
  {
    delete x;
  }
}

bool ECCommandHistory :: Undo()
{
  if (index >= 0 && index < static_cast<int>(commandHistory.size()))
  { 
    commandHistory[index]->UnExecute();
    index--;
    std::cout << "Undo!" << std::endl;
    return true;
  }
  else
  {
    return false;
  }
}

bool ECCommandHistory :: Redo()
{ 
  if (index >= -1 && index < static_cast<int>(commandHistory.size()) - 1)
  { 
    index++;
    std::cout << "Redo!" << std::endl;
    commandHistory[index]->Execute();

    return true;
  }
  else
  {
    std::cout << "false" << std::endl;
    return false;
  }
}

void ECCommandHistory :: ExecuteCmd( ECCreateShapeCommand *pCmd )
{
  if (index + 1 < static_cast<int>(commandHistory.size())) 
  {
    commandHistory.erase(commandHistory.begin() + index + 1, commandHistory.end());
  }
  
  commandHistory.push_back(pCmd);
  pCmd->Execute();
  index++;
}

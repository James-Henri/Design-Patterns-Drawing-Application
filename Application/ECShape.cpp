#include "ECShape.h"
#include <fstream>
#include <istream>
#include <sstream>
#include <string>

ECShape :: ECShape() : composite(true), elipse(false), filled(false)
{
    listChildren.clear();
}

void ECShape :: AddChild(ECShape * shape)
{
    listChildren.push_back(shape);
}

ECShape * ECShape :: GetChild(int i)
{
    return listChildren[i];
}

void ECShape :: SaveToFile(std::ofstream& file)
{

    if(!GetElipse() && !GetFilled())
    {   //if current shape is just a rectangle
        if(GetX1() < GetX2() && GetY1() > GetY2())
        {
            file << "0 4 " << GetX1() << " " << GetY1() << " "
         << GetX1() << " " << GetY2() << " "
         << GetX2() << " " << GetY2() << " "
         << GetX2() << " " << GetY1() << " "
         << GetOrColor() << "\n";
        }
        else if(GetX2() < GetX1() && GetY1() > GetY2())
        {
            file << "0 4 " << GetX2() << " " << GetY1() << " "
         << GetX2() << " " << GetY2() << " "
         << GetX1() << " " << GetY2() << " "
         << GetX1() << " " << GetY1() << " "
         << GetOrColor() << "\n";
        }
        if(GetX1() < GetX2() && GetY2() > GetY1())
        {
            file << "0 4 " << GetX1() << " " << GetY2() << " "
         << GetX1() << " " << GetY1() << " "
         << GetX2() << " " << GetY1() << " "
         << GetX2() << " " << GetY2() << " "
         << GetOrColor() << "\n";
        }
        else if(GetX2() < GetX1() && GetY2() > GetY1())
        {
            file << "0 4 " << GetX2() << " " << GetY2() << " "
         << GetX2() << " " << GetY1() << " "
         << GetX1() << " " << GetY1() << " "
         << GetX1() << " " << GetY2() << " "
         << GetOrColor() << "\n";
        }
    }
    else if(GetElipse() && !GetFilled())
    {
        //if current shape is just a ellipse
        file << "1 " << GetX1() << " " << GetY1() 
        << " " << GetX2() << " " << GetY2() << " " << GetOrColor() << "\n";
        
    }
    if(!GetElipse() && GetFilled())
    {
        //if current shape is a filled rectangle
        if(GetX1() < GetX2() && GetY1() > GetY2())
        {
            file << "2 4 " << GetX1() << " " << GetY1() << " "
         << GetX1() << " " << GetY2() << " "
         << GetX2() << " " << GetY2() << " "
         << GetX2() << " " << GetY1() << " "
         << GetOrColor() << "\n";
        }
        else if(GetX2() < GetX1() && GetY1() > GetY2())
        {
            file << "2 4 " << GetX2() << " " << GetY1() << " "
         << GetX2() << " " << GetY2() << " "
         << GetX1() << " " << GetY2() << " "
         << GetX1() << " " << GetY1() << " "
         << GetOrColor() << "\n";
        }
        if(GetX1() < GetX2() && GetY2() > GetY1())
        {
            file << "2 4 " << GetX1() << " " << GetY2() << " "
         << GetX1() << " " << GetY1() << " "
         << GetX2() << " " << GetY1() << " "
         << GetX2() << " " << GetY2() << " "
         << GetOrColor() << "\n";
        }
        else if(GetX2() < GetX1() && GetY2() > GetY1())
        {
            file << "2 4 " << GetX2() << " " << GetY2() << " "
         << GetX2() << " " << GetY1() << " "
         << GetX1() << " " << GetY1() << " "
         << GetX1() << " " << GetY2() << " "
         << GetOrColor() << "\n";
        }
    }
    else if(GetElipse() && GetFilled())
    {
        //if current shape is a filled ellipse
        file << "3 " << GetX1() << " " << GetY1() 
        << " " << GetX2() << " " << GetY2() << " " << GetOrColor() << "\n";
    }
    else if(GetComposite())
    {
        file << "4 " << GetChildSize() << "\n";

        for (int i = 0; i < GetChildSize(); ++i)
        {
            ECShape* child = GetChild(i);
            child->SaveToFile(file);
        }
    }
}

void ECShape :: LoadingChildren(std::ifstream& file, int children)
{   
    for(int i = 0; i < children ; i++)
    {
        std::string line;
        std::getline(file, line);
        std::istringstream lineStream(line);
        std::vector<int> numbersVector;

        int numbers;
        while (lineStream >> numbers)
        {
            numbersVector.push_back(numbers);
        }

        if(numbersVector[0] == 0 || numbersVector[0] == 2)   //creating a rectangle
        {   
            ECShape * rectangle = nullptr;

            if(numbersVector[0] == 2)
            {   
                rectangle = new ECRectangle(numbersVector[2], numbersVector[3], numbersVector[6], numbersVector[7], 3, static_cast<ECGVColor>(numbersVector[10]), true);
            }
            else
            {
                rectangle = new ECRectangle(numbersVector[2], numbersVector[3], numbersVector[6], numbersVector[7], 3, static_cast<ECGVColor>(numbersVector[10]), false);   
            }

            AddChild(rectangle);
        }
        else if(numbersVector[0] == 1 || numbersVector[0] == 3) //creating an ellipse
        {   
            ECShape * ellipse = nullptr;
            if(numbersVector[0] == 3)
            {   
                ellipse = new ECElipse(numbersVector[1], numbersVector[2], numbersVector[3], numbersVector[4], 3, static_cast<ECGVColor>(numbersVector[5]), true);
            }
            else
            {
                ellipse = new ECElipse(numbersVector[1], numbersVector[2], numbersVector[3], numbersVector[4], 3, static_cast<ECGVColor>(numbersVector[5]), false);
            }
            AddChild(ellipse);
        }
        else if(numbersVector[0] == 4)
        {
            ECShape * curComposite = new ECShape();
            curComposite->LoadingChildren(file, numbersVector[1]);
            AddChild(curComposite);
        }

    }
}

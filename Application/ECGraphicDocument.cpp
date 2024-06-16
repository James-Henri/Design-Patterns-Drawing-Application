#include "ECGraphicDocument.h"
#include "ECShape.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>


// **********************************************************
// Create Rectangle Implementations
void ECCreateRectangleCommand :: Execute()
{   
    ECShape * rectangle = new ECRectangle(x1, y1, x2, y2, thickness, color, false);
    doc.AddShape(rectangle);
    index = doc.GetSize() - 1;

    doc.Create(rectangle, false, false);
}

void ECCreateRectangleCommand :: UnExecute()
{
    doc.DeleteShape(index);
}

// **********************************************************
// Create Rectangle Filled Implementations

void ECCreateRectangleFilledCommand :: Execute()
{   

    ECShape * rectangle = new ECRectangle(x1, y1, x2, y2, thickness, color, true);
    doc.AddShape(rectangle);
    index = doc.GetSize() - 1;

    doc.Create(rectangle, false, true);
}

void ECCreateRectangleFilledCommand :: UnExecute()
{
    doc.DeleteShape(index);
}

// **********************************************************
// Create Elipse Implementations

void ECCreateElipseCommand :: Execute()
{
    ECShape * elipse = new ECElipse(xcenter, ycenter, xradius, yradius, thickness, color, false);
    doc.AddShape(elipse);
    index = doc.GetSize() - 1;

    doc.Create(elipse, true, false);

}

void ECCreateElipseCommand :: UnExecute()
{
    doc.DeleteShape(index);
}

// **********************************************************
// Create Elipse Filled Implementations

void ECCreateElipseFilledCommand :: Execute()
{
    ECShape * elipse = new ECElipse(xcenter, ycenter, xradius, yradius, thickness, color, true);
    doc.AddShape(elipse);
    index = doc.GetSize() - 1;

    doc.Create(elipse, true, true);

}

void ECCreateElipseFilledCommand :: UnExecute()
{
    doc.DeleteShape(index);
}

// **********************************************************
// Creating Composite Implementation

void ECCreateCompositeCommand :: Execute()
{   
    composite = new ECShape();
    composite->SetComposite(true);

    for(int i = 0; i < doc.GetSize(); i++)
    {   
        ECShape * curShape = doc.GetShapeAt(i);
        if(curShape->GetComposite())
        {   
            if(CompositeHelp(curShape))
            {
                composite->AddChild(curShape);
                indexes.push_back(i);
            }
        }
        else if(curShape->GetColor() == ECGV_BLUE && !curShape->GetComposite())
        {   
            composite->AddChild(curShape);
            indexes.push_back(i);
        }
    }

    doc.AddShape(composite);

    for (auto it = indexes.rbegin(); it != indexes.rend(); ++it)
    {
        doc.DeleteShape(*it);
    }

    index = doc.GetSize() - 1;

    doc.DeselectHelp(composite);

}

void ECCreateCompositeCommand :: UnExecute()
{   
    if (composite)
    {   
        // Remove the composite shape from the document
        doc.DeleteShape(index);
        doc.SelectColorHelp(composite);
        // Restore the original shapes at their respective indexes
        std::vector<ECShape*> originalShapes;
        for (int i = 0; i < indexes.size(); ++i)
        {
            if (indexes[i] >= doc.GetSize())
            {
                originalShapes.push_back(composite->GetChild(i));
            }
            else
            {
                originalShapes.push_back(doc.GetShapeAt(indexes[i]));
            }
        }

        // Restore the original shapes
        for (int i = 0; i < indexes.size(); ++i)
        {
            if (indexes[i] >= doc.GetSize())
            {
                doc.AddShape(originalShapes[i]);
            }
            else
            {
                doc.InsertShapeAt(originalShapes[i], indexes[i]);
            }
        }

        indexes.clear();
    }
}

bool ECCreateCompositeCommand :: CompositeHelp(ECShape * shape)
{
    for (int j = 0; j < shape->GetChildSize(); j++)
    {
        ECShape* curCompShape = shape->GetChild(j);

        if(curCompShape->GetComposite())
        {   
            if (CompositeHelp(curCompShape))
            {
                return true;
            }
        }
        else if(curCompShape->GetColor() == ECGV_BLUE)
        {
            return true;
        }
    }

    return false;
}

// **********************************************************
// Selecting Shape Implementation

void ECSelectShapeCommand :: Execute()
{   

    for(int i = 0; i < doc.GetSize(); i++)
    {   
        ECShape * curShape = doc.GetShapeAt(i);
        doc.DeselectHelp(curShape);
    }

    for (int i = 0; i < doc.GetSize(); i++)
    {
        ECShape* shape = doc.GetShapeAt(i);

        if(shape->GetComposite())
        {   
            if(SelectHelp(shape))
            {   
                selected = shape;
                doc.SelectColorHelp(shape);
            }
        }
        else
        {
            // Handle simple shapes (rectangle or ellipse)
            if (!shape->GetElipse())
            {   
                // Rectangle
                double minX = std::min(shape->GetX1(), shape->GetX2());
                double maxX = std::max(shape->GetX1(), shape->GetX2());
                double minY = std::min(shape->GetY1(), shape->GetY2());
                double maxY = std::max(shape->GetY1(), shape->GetY2());

                if (x >= minX && x <= maxX && y >= minY && y <= maxY)
                {   
                    shape->SetOrX1(shape->GetX1());
                    shape->SetOrX2(shape->GetX2());
                    shape->SetOrY1(shape->GetY1());
                    shape->SetOrY2(shape->GetY2());

                    // Color the selected shape
                    selected = shape;
                    doc.ColorShape(selected, ECGV_BLUE);
                    return;
                }
            }
            else
            {   
                // Ellipse
                double term1 = (x - shape->GetX1());
                term1 /= shape->GetX2();
                term1 *= term1;

                double term2 = (y - shape->GetY1());
                term2 /= shape->GetY2();
                term2 *= term2;

                if (term1 + term2 <= 1.0)
                {
                    shape->SetOrX1(shape->GetX1());
                    shape->SetOrX2(shape->GetX2());
                    shape->SetOrY1(shape->GetY1());
                    shape->SetOrY2(shape->GetY2());

                    // Color the selected shape
                    selected = shape;
                    doc.ColorShape(selected, ECGV_BLUE);
                    return;
                }
            }
        }
    }
}

void ECSelectShapeCommand :: UnExecute()
{   
    if(selected->GetComposite())
    {
        UnExecuteHelp(selected, ECGV_BLACK);
    }
    else
    {
        doc.ColorShape(selected, selected->GetOrColor());
    }
}

bool ECSelectShapeCommand :: SelectHelp(ECShape * shape)
{
    for (int j = 0; j < shape->GetChildSize(); j++)
    {
        ECShape* curCompShape = shape->GetChild(j);

        if(curCompShape->GetComposite())
        {   
            if (SelectHelp(curCompShape))
            {
                return true;
            }
        }
        else if (!curCompShape->GetElipse())
        {   
            // Rectangle
            double minX = std::min(curCompShape->GetX1(), curCompShape->GetX2());
            double maxX = std::max(curCompShape->GetX1(), curCompShape->GetX2());
            double minY = std::min(curCompShape->GetY1(), curCompShape->GetY2());
            double maxY = std::max(curCompShape->GetY1(), curCompShape->GetY2());
        
            if (x >= minX && x <= maxX && y >= minY && y <= maxY)
            {   
                curCompShape->SetOrX1(curCompShape->GetX1());
                curCompShape->SetOrX2(curCompShape->GetX2());
                curCompShape->SetOrY1(curCompShape->GetY1());
                curCompShape->SetOrY2(curCompShape->GetY2());
                return true;
            }
        }
        else
        {   
            // Ellipse
            double term1 = (x - curCompShape->GetX1());
            term1 /= curCompShape->GetX2();
            term1 *= term1;

            double term2 = (y - curCompShape->GetY1());
            term2 /= curCompShape->GetY2();
            term2 *= term2;

            if (term1 + term2 <= 1.0)
            {
                curCompShape->SetOrX1(curCompShape->GetX1());
                curCompShape->SetOrX2(curCompShape->GetX2());
                curCompShape->SetOrY1(curCompShape->GetY1());
                curCompShape->SetOrY2(curCompShape->GetY2());

                return true;
            }
        }
    }

    return false;
}

void ECSelectShapeCommand :: UnExecuteHelp(ECShape * shape, ECGVColor color)
{   
    for (int j = 0; j < shape->GetChildSize(); j++)
    {
        ECShape* curCompShape = shape->GetChild(j);

        if(curCompShape->GetComposite())
        {   
            UnExecuteHelp(shape, ECGV_BLACK);
        }
        else
        {
            doc.ColorShape(curCompShape, curCompShape->GetOrColor());
        }
    }
}

// **********************************************************
// Selecting Multiple Shape Implementation

void ECSelectMultipleShapeCommand :: Execute()
{
    for (int i = 0; i < doc.GetSize(); i++)
    {
        ECShape* shape = doc.GetShapeAt(i);

        if(shape->GetComposite())
        {   
            if(SelectHelp(shape))
            {   
                selected = shape;
                doc.SelectColorHelp(shape);
            }
        }
        else
        {
            // Handle simple shapes (rectangle or ellipse)
            if (!shape->GetElipse())
            {
                // Rectangle
                double minX = std::min(shape->GetX1(), shape->GetX2());
                double maxX = std::max(shape->GetX1(), shape->GetX2());
                double minY = std::min(shape->GetY1(), shape->GetY2());
                double maxY = std::max(shape->GetY1(), shape->GetY2());

                if (x >= minX && x <= maxX && y >= minY && y <= maxY)
                {
                    shape->SetOrX1(shape->GetX1());
                    shape->SetOrX2(shape->GetX2());
                    shape->SetOrY1(shape->GetY1());
                    shape->SetOrY2(shape->GetY2());

                    // Color the selected shape
                    selected = shape;
                    doc.ColorShape(selected, ECGV_BLUE);
                }
            }
            else
            {
                // Ellipse
                double term1 = (x - shape->GetX1());
                term1 /= shape->GetX2();
                term1 *= term1;

                double term2 = (y - shape->GetY1());
                term2 /= shape->GetY2();
                term2 *= term2;

                if (term1 + term2 <= 1.0)
                {
                    shape->SetOrX1(shape->GetX1());
                    shape->SetOrX2(shape->GetX2());
                    shape->SetOrY1(shape->GetY1());
                    shape->SetOrY2(shape->GetY2());

                    // Color the selected shape
                    selected = shape;
                    doc.ColorShape(selected, ECGV_BLUE);
                }
            }
        }
    }
}

void ECSelectMultipleShapeCommand :: UnExecute()
{
    if(selected->GetComposite())
    {
        UnExecuteHelp(selected, ECGV_BLACK);
    }
    else
    {
        doc.ColorShape(selected, selected->GetOrColor());
    }
}

bool ECSelectMultipleShapeCommand :: SelectHelp(ECShape * shape)
{
    for (int j = 0; j < shape->GetChildSize(); j++)
    {
        ECShape* curCompShape = shape->GetChild(j);

        if(curCompShape->GetComposite())
        {   
            if (SelectHelp(curCompShape))
            {
                return true;
            }
        }
        else if (!curCompShape->GetElipse())
        {
            // Rectangle
            double minX = std::min(curCompShape->GetX1(), curCompShape->GetX2());
            double maxX = std::max(curCompShape->GetX1(), curCompShape->GetX2());
            double minY = std::min(curCompShape->GetY1(), curCompShape->GetY2());
            double maxY = std::max(curCompShape->GetY1(), curCompShape->GetY2());
        
            if (x >= minX && x <= maxX && y >= minY && y <= maxY)
            {   
                curCompShape->SetOrX1(curCompShape->GetX1());
                curCompShape->SetOrX2(curCompShape->GetX2());
                curCompShape->SetOrY1(curCompShape->GetY1());
                curCompShape->SetOrY2(curCompShape->GetY2());
                return true;
            }
        }
        else
        {
            // Ellipse
            double term1 = (x - curCompShape->GetX1());
            term1 /= curCompShape->GetX2();
            term1 *= term1;

            double term2 = (y - curCompShape->GetY1());
            term2 /= curCompShape->GetY2();
            term2 *= term2;

            if (term1 + term2 <= 1.0)
            {
                curCompShape->SetOrX1(curCompShape->GetX1());
                curCompShape->SetOrX2(curCompShape->GetX2());
                curCompShape->SetOrY1(curCompShape->GetY1());
                curCompShape->SetOrY2(curCompShape->GetY2());

                return true;
            }
        }
    }

    return false;
}

void ECSelectMultipleShapeCommand :: UnExecuteHelp(ECShape * shape, ECGVColor color)
{   
    for (int j = 0; j < shape->GetChildSize(); j++)
    {
        ECShape* curCompShape = shape->GetChild(j);

        if(curCompShape->GetComposite())
        {   
            UnExecuteHelp(shape, ECGV_BLACK);
        }
        else
        {
            doc.ColorShape(curCompShape, curCompShape->GetOrColor());
        }
    }
}

// **********************************************************
// Moving Shape Implementation

void ECMoveShapeCommand :: Execute()
{   
    for (int i = 0; i < doc.GetSize(); i++)
    {
        ECShape* curShape = doc.GetShapeAt(i);
        MovingHelp(curShape);
    }
}

void ECMoveShapeCommand :: UnExecute()
{
    for (int i = 0; i < doc.GetSize(); i++)
    {
        ECShape* curShape = doc.GetShapeAt(i);
        UnExecuteHelp(curShape);
    }
}

void ECMoveShapeCommand :: MovingHelp(ECShape * shape)
{
    if (shape->GetComposite())
    {   
        for (int j = 0; j < shape->GetChildSize(); j++)
        {   
            ECShape* curCompShape = shape->GetChild(j);
            MovingHelp(curCompShape);
        }
    }
    else if (shape->GetColor() == ECGV_BLUE && !shape->GetComposite())
    {
        // Move the shape
        doc.MoveShape(x_move, y_move, shape);

        // Update the original coordinates
        shape->SetOrX1(shape->GetOrX1() + x_move);
        shape->SetOrX2(shape->GetOrX2() + x_move);
        shape->SetOrY1(shape->GetOrY1() + y_move);
        shape->SetOrY2(shape->GetOrY2() + y_move);
    }
}

void ECMoveShapeCommand :: UnExecuteHelp(ECShape * shape)
{   
    if(shape->GetComposite())
    {  
        for(int i = 0; i < shape->GetChildSize(); i++)
        {
            ECShape * curShape = shape->GetChild(i);
        
            UnExecuteHelp(curShape);
        }
    }
    else if(shape->GetColor() == ECGV_BLUE && !shape->GetComposite())
    {   
        if(!shape->GetElipse())
        {   
            doc.MoveShape(-x_move, -y_move, shape);

            shape->SetOrX1(shape->GetOrX1() - x_move);
            shape->SetOrX2(shape->GetOrX2() - x_move);
            shape->SetOrY1(shape->GetOrY1() - y_move);
            shape->SetOrY2(shape->GetOrY2() - y_move);
        }
        else if(shape->GetElipse())
        {   
            doc.MoveShape(-x_move, -y_move, shape);

            shape->SetOrX1(shape->GetOrX1() - x_move);
            shape->SetOrY1(shape->GetOrY1() - y_move);
            
        }
    }
    
}

// **********************************************************
// Deleting Shape Implementation

void ECDeleteShapeCommand :: Execute()
{   
    for(int i = 0; i < doc.GetSize(); i++)
    {   
        ECShape * curShape = doc.GetShapeAt(i);

        if(curShape->GetComposite())
        {   
            //need another if to get rid of unecessary accessing
            if(DeleteHelp(curShape))
            {
                shapeDeleted = curShape;
                index = i;
                doc.DeleteShape(i);
                return;
            }
        }
        else if(curShape->GetColor() == ECGV_BLUE && !curShape->GetComposite())
        {   
            shapeDeleted = curShape;

            index = i;
            doc.DeleteShape(i);
            return;
        }
    }
}

void ECDeleteShapeCommand :: UnExecute()
{     

    if(index > doc.GetSize() - 1)
    {
        doc.AddShape(shapeDeleted);
    }
    else
    {
        doc.InsertShapeAt(shapeDeleted, index);
    }
}

bool ECDeleteShapeCommand :: DeleteHelp(ECShape * shape)
{
    for (int i = 0; i < shape->GetChildSize(); i++)
    {
        ECShape* curshape = shape->GetChild(i);

        if(curshape->GetComposite())
        {   
            if(DeleteHelp(curshape))
            {   
                return true;
            }
        }
        else if(curshape->GetColor() == ECGV_BLUE)
        {
            return true;
        }
    }
    return false;

}

// **********************************************************
// UnComposite Shape Implementation

void ECUnCompositeShapeCommand :: Execute()
{
    int index = 0;

    for(int i = 0; i < doc.GetSize(); i++)
    {
        ECShape * curShape = doc.GetShapeAt(i);
        if(curShape->GetComposite())
        {
            if(CompositeHelp(curShape))
            {   
                index = i;
                composite = curShape;
                for(int j = 0; j < curShape->GetChildSize(); j++)
                {
                    ToAdd.push_back(curShape->GetChild(j));
                }
            }
        }
    }
    
    doc.DeleteShape(index);

    for(auto x : ToAdd)
    {
        doc.AddShape(x);
    }
}

void ECUnCompositeShapeCommand :: UnExecute()
{
    for (int i = 0; i < ToAdd.size(); ++i)
    {
        doc.DeleteShape(doc.GetIndexOfShape(ToAdd[i]));
    }

    doc.AddShape(composite);
}

bool ECUnCompositeShapeCommand :: CompositeHelp(ECShape * shape)
{
    for (int j = 0; j < shape->GetChildSize(); j++)
    {
        ECShape* curCompShape = shape->GetChild(j);

        if(curCompShape->GetComposite())
        {   
            if (CompositeHelp(curCompShape))
            {
                return true;
            }
        }
        else if(curCompShape->GetColor() == ECGV_BLUE)
        {
            return true;
        }
    }

    return false;
}


// **********************************************************
// ECGraphicDocument Implementations

ECGraphicDocument :: ECGraphicDocument(ECGraphicViewImp& viewIn) :  ctrl(*this, viewIn), view(viewIn), orig_x1(0), orig_y1(0), orig_x2(0), orig_y2(0) {}

void ECGraphicDocument :: AddShape(ECShape * shape)
{
    Shapes.push_back(shape);
}

void ECGraphicDocument :: DeleteShape(int index)
{   
    if (index >= 0 && index < Shapes.size())
    {
        Shapes.erase(Shapes.begin() + index);
    }

}

void ECGraphicDocument :: InsertShapeAt(ECShape * shape, int index)
{
    Shapes.insert(Shapes.begin() + index, shape);
}

ECShape * ECGraphicDocument :: GetShapeAt(int index)
{
    return Shapes[index];
}

int ECGraphicDocument :: GetIndexOfShape(ECShape * shape)
{
    for (int i = 0; i < Shapes.size(); ++i)
    {
        if (Shapes[i] == shape)
        {
            return i;
        }
    }
    return -1;
}

void ECGraphicDocument :: Create(ECShape * shape, bool elipse, bool filled)
{   
    if(!elipse && !filled)
    {
        view.DrawRectangle(shape->GetX1(), shape->GetY1(), shape->GetX2(), shape->GetY2(), shape->GetThickness(), shape->GetColor());
    }
    else if (elipse && !filled)
    {
        view.DrawEllipse(shape->GetX1(), shape->GetY1(), shape->GetX2(), shape->GetY2(), shape->GetThickness(), shape->GetColor());
    }
    else if(!elipse && filled)
    {
        view.DrawFilledRectangle(shape->GetX1(), shape->GetY1(), shape->GetX2(), shape->GetY2(), shape->GetColor());
    }
    else if(elipse && filled)
    {
        view.DrawFilledEllipse(shape->GetX1(), shape->GetY1(), shape->GetX2(), shape->GetY2(), shape->GetColor());
    }
}

void ECGraphicDocument :: ShowAll(ECShape * shape)
{
    if (shape->GetComposite())
    {
        for (int i = 0; i < shape->GetChildSize(); i++)
        {
            ShowAll(shape->GetChild(i));
        }
    }
    
    Create(shape, shape->GetElipse(), shape->GetFilled());
}

void ECGraphicDocument :: ColorShape(ECShape * shape, ECGVColor color)
{   
    shape->SetColor(color);
}

void ECGraphicDocument :: MoveShape(int x_move, int y_move, ECShape * shape)
{   
    if(!shape->GetElipse())
    {   
        //moving a rectangle
        shape->SetX1(shape->GetOrX1() + x_move);
        shape->SetX2(shape->GetOrX2() + x_move);
        shape->SetY1(shape->GetOrY1() + y_move);
        shape->SetY2(shape->GetOrY2() + y_move);
    }
    else if(shape->GetElipse())
    {   
        //moving an elipse
        shape->SetX1(shape->GetOrX1() + x_move);
        shape->SetY1(shape->GetOrY1() + y_move);
    }

}

void ECGraphicDocument :: DeselectHelp(ECShape * shape)
{
    if(shape->GetComposite())
    {   
        for(int j = 0; j < shape->GetChildSize(); j++)
        {   
            ECShape* curCompShape = shape->GetChild(j);

            if(curCompShape->GetComposite())
            {
                DeselectHelp(curCompShape);
            }
            
            curCompShape->SetColor(curCompShape->GetOrColor());
        }
    }
    else
    {
        shape->SetColor(shape->GetOrColor());
    }
}

void ECGraphicDocument :: SelectColorHelp(ECShape * shape)
{
    for(int i = 0; i < shape->GetChildSize(); i++)
    {   
        ECShape * curShape = shape->GetChild(i);

        if(curShape->GetComposite())
        {
            SelectColorHelp(curShape);
        }
        else
        {
            curShape->SetColor(ECGV_BLUE);
        }
    }
}

bool ECGraphicDocument :: AlreadySelectedHelp(int x, int y, ECShape * shape)
{
    for (int j = 0; j < shape->GetChildSize(); j++)
    {
        ECShape* curCompShape = shape->GetChild(j);

        if(curCompShape->GetComposite())
        {   
            if (AlreadySelectedHelp(x, y, curCompShape))
            {
                return true;
            }
        }
        else if (!curCompShape->GetElipse())
        {
            // Rectangle
            double minX = std::min(curCompShape->GetX1(), curCompShape->GetX2());
            double maxX = std::max(curCompShape->GetX1(), curCompShape->GetX2());
            double minY = std::min(curCompShape->GetY1(), curCompShape->GetY2());
            double maxY = std::max(curCompShape->GetY1(), curCompShape->GetY2());
        
            if (x >= minX && x <= maxX && y >= minY && y <= maxY && curCompShape->GetColor() == ECGV_BLUE)
            {   
                return true;
            }
        }
        else
        {
            // Ellipse
            double term1 = (x - curCompShape->GetX1());
            term1 /= curCompShape->GetX2();
            term1 *= term1;

            double term2 = (y - curCompShape->GetY1());
            term2 /= curCompShape->GetY2();
            term2 *= term2;

            if (term1 + term2 <= 1.0 && curCompShape->GetColor() == ECGV_BLUE)
            {

                return true;
            }
        }
    }

    return false;
}

void ECGraphicDocument :: SaveToFile(const std::string& filename) const
{
    std::ofstream file(filename);

    if(!file.is_open())
    {
        std::cerr << "Error: could not open file for writing. \n";
        return;
    }

    file << Shapes.size() << "\n";

    for(const auto& shape: Shapes)
    {
        shape->SaveToFile(file);
    }

    file.close();
}

void ECGraphicDocument :: LoadToFile(const std::string& filename)
{
    std::ifstream file(filename);
    
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line);   //first getting the amount of shapes there are
    std::istringstream lineStream(line);
    int numShapes;
    lineStream >> numShapes;

    for(int i = 0; i < numShapes; i++)
    {
        std::getline(file, line);
        std::istringstream lineStream(line);
        std::vector<int> numbersVector;
        
        int number;

        while(lineStream >> number)
        {
            numbersVector.push_back(number);
        }

        for(auto i : numbersVector)
        {
            std::cout << i << " "; 
        }
        std::cout << std::endl; 

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

            Shapes.push_back(rectangle);
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
            
            Shapes.push_back(ellipse);
        }
        else if(numbersVector[0] == 4)
        {
            ECShape * curComposite = new ECShape();
            curComposite->LoadingChildren(file, numbersVector[1]);
            Shapes.push_back(curComposite);
        }

    }     

}

// **********************************************************
// ECGraphicController Implementations

ECGraphicController :: ECGraphicController(ECGraphicDocument& docIn, ECGraphicViewImp& viewIn) : doc(docIn), view(viewIn), history()
{
}

void ECGraphicController :: CreateRectangle(int x1, int y1, int x2, int y2)
{   
    std::cout << "Create Rectangle Command!" << std::endl;
    ECCreateRectangleCommand * rectangle = new ECCreateRectangleCommand(doc, x1, y1, x2, y2, 3, ECGV_BLACK, false);
    history.ExecuteCmd(rectangle);
    
}

void ECGraphicController :: CreateRectangleFilled(int x1, int y1, int x2, int y2)
{   
    std::cout << "Create Rectangle Filled Command!" << std::endl;
    ECCreateRectangleFilledCommand * rectangle_filled = new ECCreateRectangleFilledCommand(doc, x1, y1, x2, y2, 3, ECGV_BLACK, true);
    history.ExecuteCmd(rectangle_filled);
    
}

void ECGraphicController :: CreateElipses(int xcenter,int ycenter, int xradius, int yradius)
{   
    std::cout << "Create Elipses Command!" << std::endl;
    ECCreateElipseCommand * elipse = new ECCreateElipseCommand(doc, xcenter, ycenter, xradius, yradius, 3, ECGV_BLACK, false);
    history.ExecuteCmd(elipse);
    
}

void ECGraphicController :: CreateElipsesFilled(int xcenter,int ycenter, int xradius, int yradius)
{   
    std::cout << "Create Elipses Filled Command!" << std::endl;
    ECCreateElipseFilledCommand * elipse_filled = new ECCreateElipseFilledCommand(doc, xcenter, ycenter, xradius, yradius, 3, ECGV_BLACK, true);
    history.ExecuteCmd(elipse_filled);
    
}

void ECGraphicController :: CreateComposite()
{
    std::cout << "Create Composite Command!" << std::endl;
    ECCreateCompositeCommand * composite = new ECCreateCompositeCommand(doc);
    history.ExecuteCmd(composite);
}

void ECGraphicController :: UnComposite()
{
    std::cout << "UnComposite Command!" << std::endl;
    ECUnCompositeShapeCommand * uncomposite = new ECUnCompositeShapeCommand(doc);
    history.ExecuteCmd(uncomposite);
}

void ECGraphicController :: ShowAllShapes()
{
    for(int i = 0; i < doc.GetSize(); i++)
    {   
        ECShape * curShape = doc.GetShapeAt(i);

        doc.ShowAll(curShape);
    }
}

void ECGraphicController :: Select(int x, int y)
{   
    std::cout << "Select Command!" << std::endl;
    ECSelectShapeCommand * select = new ECSelectShapeCommand(doc, x, y);
    history.ExecuteCmd(select);
}

void ECGraphicController :: SelectMultiple(int x, int y)
{   
    std::cout << "Select Multiple Command!" << std::endl;
    ECSelectMultipleShapeCommand * select = new ECSelectMultipleShapeCommand(doc, x, y);
    history.ExecuteCmd(select);
}

bool ECGraphicController :: AlreadySelected(int x, int y)
{
    for (int i = 0; i < doc.GetSize(); i++)
    {
        ECShape* shape = doc.GetShapeAt(i);

        if(shape->GetComposite())
        {   
            if(doc.AlreadySelectedHelp(x, y, shape))
            {   
                return true;
            }
        }
        else
        {
            // Handle simple shapes (rectangle or ellipse)
            if (!shape->GetElipse())
            {
                // Rectangle
                double minX = std::min(shape->GetX1(), shape->GetX2());
                double maxX = std::max(shape->GetX1(), shape->GetX2());
                double minY = std::min(shape->GetY1(), shape->GetY2());
                double maxY = std::max(shape->GetY1(), shape->GetY2());

                if (x >= minX && x <= maxX && y >= minY && y <= maxY && shape->GetColor() == ECGV_BLUE)
                {
                    return true;
                }
            }
            else
            {
                // Ellipse
                double term1 = (x - shape->GetX1());
                term1 /= shape->GetX2();
                term1 *= term1;

                double term2 = (y - shape->GetY1());
                term2 /= shape->GetY2();
                term2 *= term2;

                if (term1 + term2 <= 1.0 && shape->GetColor() == ECGV_BLUE)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void ECGraphicController :: DeselectAll()
{   
    for(int i = 0; i < doc.GetSize(); i++)
    {   
        ECShape * curShape = doc.GetShapeAt(i);

        doc.DeselectHelp(curShape);
    }
}

void ECGraphicController :: Moved(int x1, int y1, int x2, int y2)
{   
    std::cout << "Move Command!" << std::endl;
    ECMoveShapeCommand * move = new ECMoveShapeCommand(doc, x1, y1, x2, y2);
    history.ExecuteCmd(move);
}

void ECGraphicController :: Moving(int x1, int y1, int x2, int y2)
{   
    for (int i = 0; i < doc.GetSize(); i++)
    {
        ECShape* curShape = doc.GetShapeAt(i);
        MovingHelp(x1, y1, x2, y2, curShape);
    }
}

void ECGraphicController :: Delete()
{   
    std::cout << "Delete Command!" << std::endl;
    ECDeleteShapeCommand * del = new ECDeleteShapeCommand(doc);
    history.ExecuteCmd(del);
}

bool ECGraphicController :: Undo()
{
  return history.Undo();
}

bool ECGraphicController :: Redo()
{
  return history.Redo();
}

void ECGraphicController :: MovingHelp(int x1, int y1, int x2, int y2, ECShape * shape)
{
    if (shape->GetComposite())
    {   
        for (int j = 0; j < shape->GetChildSize(); j++)
        {   
            ECShape* curCompShape = shape->GetChild(j);
            MovingHelp(x1, y1, x2, y2, curCompShape);
        }
    }
    else if (shape->GetColor() == ECGV_BLUE && !shape->GetComposite())
    {
        // Move the shape
        doc.MoveShape(x2 - x1, y2-y1, shape);
    }
}

int ECGraphicController :: WhatWasSelected()
{   
    std::vector<ECShape *> WhatsSelected({});

    for (int i = 0; i < doc.GetSize(); i++)
    {
        ECShape* shape = doc.GetShapeAt(i);

        if(shape->GetComposite())
        {   
            if(WhatWasSelectedHelp(shape))
            {   
                WhatsSelected.push_back(shape);
            }
        }
        else if(shape->GetColor() == ECGV_BLUE)
        {
            WhatsSelected.push_back(shape);
        }
    }

    if (WhatsSelected.empty()) {
        return -1; // No shapes selected
    } 
    else if (WhatsSelected.size() == 1 && !WhatsSelected[0]->GetComposite()) 
    {
        return 0; // Only one shape selected, do nothing
    } 
    else 
    {
        // Determine if it's just a composite or a composite with shapes
        bool hasSimpleShapes = false;
        bool hasCompositeShapes = false;
        int compositeCounter = 0;

        for (ECShape* shape : WhatsSelected)
        {
            if (shape->GetComposite())
            {
                hasCompositeShapes = true;
                compositeCounter++;
            }
            else
            {
                hasSimpleShapes = true;
            }
        }

        if (hasSimpleShapes && hasCompositeShapes)
        {
            return 1; // Composite with shapes, going to make a new composite
        }
        else if (hasSimpleShapes)
        {
            return 1; // Only simple shapes, still going to make a composite
        }
        else if(hasCompositeShapes && compositeCounter > 1)
        {
            return 1 ; //two or more composite shapes, still going to make a new composite
        }
        else if(hasCompositeShapes && WhatsSelected.size() == 1)
        {
            return 2; // Only one composite shape, going to uncomposite
        }
    }
    return -1;
}

int ECGraphicController :: WhatWasSelectedHelp(ECShape * shape)
{
    for (int i = 0; i < shape->GetChildSize(); i++)
    {
        ECShape* curshape = shape->GetChild(i);

        if(curshape->GetComposite())
        {   
            if(WhatWasSelectedHelp(curshape))
            {   
                return true;
            }
        }
        else if(curshape->GetColor() == ECGV_BLUE)
        {
            return true;
        }
    }
    return false;
}

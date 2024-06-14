#ifndef ECGraphicDocument_h
#define ECGraphicDocument_h
#include <vector>
#include <fstream>
#include "ECShape.h"
#include "ECCommand.h"
#include "ECGraphicViewImp.h"

class ECGraphicDocument;

// **********************************************************
// Commands for editing 

class ECCreateRectangleCommand : public ECCreateShapeCommand
{
public:
    ECCreateRectangleCommand(ECGraphicDocument& docIn, int x1, int y1, int x2, int y2, int thicknessIn, ECGVColor colorIn, bool filled) : doc(docIn), x1(x1), y1(y1), x2(x2), y2(y2), thickness(thicknessIn), color(colorIn), index(0), filled(filled) {}
    ~ECCreateRectangleCommand() {} 
    virtual void Execute();
    virtual void UnExecute();

private:
    ECGraphicDocument& doc;
    int x1,y1,x2,y2;
    int thickness;
    ECGVColor color;
    int index;
    bool filled;
};

class ECCreateRectangleFilledCommand : public ECCreateShapeCommand
{
public:
    ECCreateRectangleFilledCommand(ECGraphicDocument& docIn, int x1, int y1, int x2, int y2, int thicknessIn, ECGVColor colorIn, bool filled) : doc(docIn), x1(x1), y1(y1), x2(x2), y2(y2), thickness(thicknessIn), color(colorIn), index(0), filled(filled) {}
    ~ECCreateRectangleFilledCommand() {} 
    virtual void Execute();
    virtual void UnExecute();

private:
    ECGraphicDocument& doc;
    int x1,y1,x2,y2;
    int thickness;
    ECGVColor color;
    int index;   
    bool filled; 
};

class ECCreateElipseCommand : public ECCreateShapeCommand
{
public:
    ECCreateElipseCommand(ECGraphicDocument& docIn, int xcenter, int ycenter, int xradius, int yradius, int thicknessIn, ECGVColor colorIn, bool filled) : doc(docIn), xcenter(xcenter), ycenter(ycenter), xradius(xradius), yradius(yradius), thickness(thicknessIn), color(colorIn), index(0), filled(filled) {}
    ~ECCreateElipseCommand() {} 
    virtual void Execute();
    virtual void UnExecute();

private:
    ECGraphicDocument& doc;
    int xcenter,ycenter,xradius,yradius;
    int thickness;
    ECGVColor color;
    int index; 
    bool filled;
};

class ECCreateElipseFilledCommand : public ECCreateShapeCommand
{
public:
    ECCreateElipseFilledCommand(ECGraphicDocument& docIn, int xcenter, int ycenter, int xradius, int yradius, int thicknessIn, ECGVColor colorIn , bool filled) : doc(docIn), xcenter(xcenter), ycenter(ycenter), xradius(xradius), yradius(yradius), thickness(thicknessIn), color(colorIn), index(0), filled(filled) {}
    ~ECCreateElipseFilledCommand() {} 
    virtual void Execute();
    virtual void UnExecute();

private:
    ECGraphicDocument& doc;
    int xcenter,ycenter,xradius,yradius;
    int thickness;
    ECGVColor color;
    int index; 
    bool filled;
};

class ECCreateCompositeCommand : public ECCreateShapeCommand
{
public:
    ECCreateCompositeCommand(ECGraphicDocument& docIn) : doc(docIn), index(0), indexes({}), composite(nullptr) {}
    ~ECCreateCompositeCommand() {delete composite;}
    virtual void Execute();
    virtual void UnExecute();
    virtual bool CompositeHelp(ECShape * shape);

private:
    ECGraphicDocument& doc;
    ECShape * composite;
    int index;
    std::vector<int> indexes;

};

class ECSelectShapeCommand : public ECCreateShapeCommand
{
public:
    ECSelectShapeCommand(ECGraphicDocument& docIn, int x, int y) : doc(docIn), selected(nullptr), x(x), y(y) {}
    ~ECSelectShapeCommand() { delete selected;} 
    virtual void Execute();
    virtual void UnExecute();
    virtual bool SelectHelp(ECShape * shape);
    virtual void UnExecuteHelp(ECShape * shape, ECGVColor color);

private:
    ECGraphicDocument& doc;
    ECShape * selected;
    int x, y;
};

class ECSelectMultipleShapeCommand : public ECCreateShapeCommand
{
public:
    ECSelectMultipleShapeCommand(ECGraphicDocument& docIn, int x, int y) : doc(docIn), selected(nullptr), x(x), y(y) {}
    ~ECSelectMultipleShapeCommand() {} 
    virtual void Execute();
    virtual void UnExecute();
    virtual bool SelectHelp(ECShape * shape);
    virtual void UnExecuteHelp(ECShape * shape, ECGVColor color);

private:
    ECGraphicDocument& doc;
    ECShape * selected;
    int x, y;
};

class ECMoveShapeCommand : public ECCreateShapeCommand
{
public:
    ECMoveShapeCommand(ECGraphicDocument& docIn, int x1, int y1, int x2, int y2) : doc(docIn), x_move(x2 - x1), y_move(y2 - y1) {} 
    ~ECMoveShapeCommand() {} 
    virtual void Execute();
    virtual void UnExecute();
    virtual void MovingHelp(ECShape * shape);
    virtual void UnExecuteHelp(ECShape * shape);

private:
    ECGraphicDocument& doc;
    int x_move, y_move;
};

class ECDeleteShapeCommand : public ECCreateShapeCommand
{
public:
    ECDeleteShapeCommand(ECGraphicDocument& docIn) : doc(docIn), index(0), shapeDeleted(nullptr) {}
    ~ECDeleteShapeCommand() {delete shapeDeleted;}
    virtual void Execute();
    virtual void UnExecute();
    virtual bool DeleteHelp(ECShape * shape);

private:
    ECGraphicDocument& doc;
    int index;
    ECShape * shapeDeleted;
};

class ECUnCompositeShapeCommand : public ECCreateShapeCommand
{
public:
    ECUnCompositeShapeCommand(ECGraphicDocument &docIn) : doc(docIn), composite(nullptr), ToAdd({}) {}
    ~ECUnCompositeShapeCommand() {delete composite; for(auto x : ToAdd) { delete x; }}
    virtual void Execute();
    virtual void UnExecute();
    virtual bool CompositeHelp(ECShape * shape);

private:
    ECGraphicDocument& doc;
    ECShape * composite;
    std::vector<ECShape *> ToAdd;
};

// **********************************************************
// Controller for text document

class ECGraphicController
{
public:
    ECGraphicController(ECGraphicDocument& docIn, ECGraphicViewImp& viewIn);
    virtual ~ECGraphicController() {}
    void CreateRectangle(int x1, int y1, int x2, int y2);
    void CreateRectangleFilled(int x1, int y1, int x2, int y2);
    void CreateElipses(int xcenter,int ycenter, int xradius, int yradius);
    void CreateElipsesFilled(int xcenter, int ycenter, int xradius, int yradius);
    void CreateComposite();
    void UnComposite();
    void ShowAllShapes();
    void Select(int x, int y);
    void SelectMultiple(int x, int y);
    bool AlreadySelected(int x, int y);
    void DeselectAll();
    void Moved(int x1, int y1, int x2, int y2);
    void Moving(int x1, int y1, int x2, int y2);
    void OutOfEditMode();
    void Delete();
    bool Undo();
    bool Redo();

    void MovingHelp(int x1, int y1, int x2, int y2, ECShape * shape);
    int WhatWasSelected();
    int WhatWasSelectedHelp(ECShape * shape);

    
private:
    ECGraphicDocument& doc;
    ECGraphicViewImp& view;
    ECCommandHistory history;

};


// **********************************************************
// Document for text document

class ECGraphicDocument
{
public:
    ECGraphicDocument(ECGraphicViewImp& viewIn);
    void AddShape(ECShape* shape);
    void DeleteShape(int index);
    void InsertShapeAt(ECShape * shape, int index);
    ECShape * GetShapeAt(int index);
    ECGraphicController &GetCtrl() {return ctrl;}
    int GetIndexOfShape(ECShape * shape);
    void Create(ECShape * shape, bool elipse, bool filled);
    void ShowAll(ECShape * shape);
    void ColorShape(ECShape * shape, ECGVColor color);
    void MoveShape(int x_move, int y_move, ECShape * shape);
    std::vector<ECShape *> &GetVector() { return Shapes;}
    int GetSize() {return Shapes.size();}
    void DeselectHelp(ECShape * shape);
    void SelectColorHelp(ECShape * shape);
    bool AlreadySelectedHelp(int x, int y, ECShape * shape);

    void SaveToFile(const std::string& filename) const;
    void LoadToFile(const std::string& filename);

    void SetX1(int x) {orig_x1 = x;}
    void SetY1(int y) {orig_y1 = y;}
    void SetX2(int x) {orig_x2 = x;}
    void SetY2(int y) {orig_y2 = y;}
    int GetX1() {return orig_x1;}
    int GetY1() {return orig_y1;}
    int GetX2() {return orig_x2;}
    int GetY2() {return orig_y2;}

private:
    std::vector<ECShape *> Shapes;
    ECGraphicViewImp view;
    ECGraphicController ctrl;
    int orig_x1, orig_y1, orig_x2, orig_y2;
};

#endif
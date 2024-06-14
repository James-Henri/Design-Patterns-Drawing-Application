#ifndef ECShape_h
#define ECShape_h
#include <vector>
#include "ECGraphicViewImp.h"

class ECShape
{
public:
    ECShape();
    virtual ~ECShape() { for(auto x : listChildren) {delete x; x = nullptr;}  }
    
    void AddChild(ECShape * shape);
    ECShape * GetChild(int i);
    int GetChildSize() {return listChildren.size();}

    virtual int GetX1() {return x1;}
    virtual void SetX1(int x) {x1 = x;}

    virtual int GetY1() {return y1;}
    virtual void SetY1(int y) {y1 = y;}

    virtual int GetX2() {return x2;}
    virtual void SetX2(int x) {x2 = x;}

    virtual int GetY2() {return y2;}
    virtual void SetY2(int y) {y2 = y;}

    virtual int GetThickness() { return thickness; }
    virtual void SetThickness(int newThick) { thickness = newThick; }

    virtual ECGVColor GetColor() { return color; }
    virtual void SetColor(ECGVColor newColor) {color = newColor;}

    virtual bool GetElipse() { return elipse; }
    virtual void SetElipse(bool newElipse) { elipse = newElipse; }

    virtual bool GetFilled() { return filled; }
    virtual void SetFilled(bool newFilled) { filled = newFilled; }

    virtual int GetOrX1() {return orx1;}
    virtual void SetOrX1(int x) {orx1 = x;}

    virtual int GetOrY1() {return ory1;}
    virtual void SetOrY1(int y) {ory1 = y;}

    virtual int GetOrX2() {return orx2;}
    virtual void SetOrX2(int x) {orx2 = x;}

    virtual int GetOrY2() {return ory2;}
    virtual void SetOrY2(int y) {ory2 = y;}

    virtual bool GetComposite() { return composite; }
    virtual void SetComposite(bool comp) {composite = comp;}

    virtual void SaveToFile(std::ofstream& file);
    virtual void LoadingChildren(std::ifstream& file, int children = 0);

    virtual ECGVColor GetOrColor() {return originalColor;}
    virtual void SetOrColor(ECGVColor newColor) {originalColor = newColor;}

protected:
    int x1, y1, x2, y2;
    int thickness;
    ECGVColor color;
    bool elipse;
    bool filled;
    int orx1, orx2, ory1, ory2;
    bool composite;
    ECGVColor originalColor;

private:
    std::vector<ECShape *> listChildren;
};

class ECRectangle : public ECShape
{
public:
    ECRectangle(int x1, int y1, int x2, int y2, int thickness, ECGVColor color, bool filled)  
    {
        SetX1(x1);
        SetX2(x2);
        SetY1(y1);
        SetY2(y2);
        SetThickness(thickness);
        SetColor(color);
        SetElipse(false);
        SetFilled(filled);
        SetOrX1(x1);
        SetOrX2(x2);
        SetOrY1(y1);
        SetOrY2(y2);
        SetComposite(false);
        SetOrColor(color);

    }

};

class ECElipse : public ECShape
{
public:
    ECElipse(int xcenter, int ycenter, int xradius, int yradius, int thickness, ECGVColor color, bool filled)
    {
        SetX1(xcenter);
        SetX2(xradius);
        SetY1(ycenter);
        SetY2(yradius);
        SetThickness(thickness);
        SetColor(color);
        SetElipse(true);
        SetFilled(filled);
        SetOrX1(xcenter);
        SetOrX2(xradius);
        SetOrY1(ycenter);
        SetOrY2(yradius);
        SetComposite(false);
        SetOrColor(color);
    } 
    
};

#endif
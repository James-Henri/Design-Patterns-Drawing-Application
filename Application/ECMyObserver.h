#ifndef ECMYOBSERVER_H
#define ECMYOBSERVER_H
#include "ECGraphicViewImp.h"
#include "ECGraphicDocument.h"
#include "ECShape.h"

//********************************************
// Subject: Keystroke Class

class ECMouseEvtObserver;

class ECKeystrokeObserver : public ECObserver
{
public:
    ECKeystrokeObserver(ECGraphicViewImp &viewIn, ECGraphicController & ctrlIn, ECMouseEvtObserver & mouseIn);
    ~ECKeystrokeObserver() {}
    virtual void Update();

private:
    ECGraphicController & ctrl;
    ECGraphicViewImp &view;
    ECMouseEvtObserver & mouse;
    bool del;
    bool undo;
    bool redo;
    bool left;
    bool right;
    bool up;
    bool down;
    bool composite;
    bool currentlyChanging;
};

//********************************************
// Subject: Mouse class

class ECMouseEvtObserver : public ECObserver
{
public:
    ECMouseEvtObserver(ECGraphicViewImp &viewIn, ECGraphicController &ctrlIn);
    ~ECMouseEvtObserver() {}
    virtual void Update();
    void setInsertMode(bool insert) {InsertionModeOn = insert;}
    bool getInsertMode() {return InsertionModeOn;}
    void setPressedDown(bool pressed) {wasPressedDown = pressed;}
    void setPressedUp(bool pressed) {wasPressedUp = pressed;}
    bool getSomethingSelected() {return SomethingSelected;}
    void setSomethingSelected(bool selected) {SomethingSelected = selected;}
    bool getElipses() { return Elipses; }
    void setElipses(bool selected) { Elipses = selected; }
    bool getFilled() { return MakeFilled; }
    void setFilled(bool filled) { MakeFilled = filled; }
    void setCtrlDown(bool set) { ctrlDown = set; }

private:
    ECGraphicController & ctrl;
    ECGraphicViewImp &view;
    bool wasPressedDown;
    bool wasPressedUp;
    bool InsertionModeOn;
    bool SomethingSelected;
    bool toMove;
    bool CurrentlyMoving;
    bool Elipses;
    bool MakeFilled;
    bool ctrlDown;
    int x1,y1,x2,y2;
    int x_moving, y_moving;

};

#endif

#include "ECMyObserver.h"
#include "ECGraphicViewImp.h"
#include "ECShape.h"
#include <cmath>

//********************************************
// Keystroke implementations

ECKeystrokeObserver :: ECKeystrokeObserver(ECGraphicViewImp &viewIn, ECGraphicController & ctrlIn, ECMouseEvtObserver & mouseIn) : view(viewIn), ctrl(ctrlIn), mouse(mouseIn), del(false), undo(false), left(false), right(false), up(false), down(false), composite(false), currentlyChanging(false) {}

void ECKeystrokeObserver ::  Update()
{
    ECGVEventType evt = view.GetCurrEvent();
    
    if( evt == ECGV_EV_KEY_DOWN_D)
    {   
        if(mouse.getSomethingSelected())
        {
            del = true;
        }
    }

    if( evt == ECGV_EV_KEY_DOWN_LEFT)
    {
        left = true;
    }

    if( evt == ECGV_EV_KEY_DOWN_RIGHT)
    {
        right = true;
    }

    if( evt == ECGV_EV_KEY_DOWN_UP)
    {
        up = true;
    }

    if( evt == ECGV_EV_KEY_DOWN_DOWN)
    {
        down = true;
    }

    if(evt == ECGV_EV_KEY_DOWN_Z)
    {
        undo = true;
    }

    if(evt == ECGV_EV_KEY_DOWN_F)
    {   
        std::cout << "Changed Fill Option" << std::endl;
        mouse.setFilled(!mouse.getFilled());
    }

    if(evt == ECGV_EV_KEY_DOWN_CTRL && !mouse.getInsertMode())
    {
        std::cout << "Can Select Multiple Shapes" << std::endl;
        mouse.setCtrlDown(true);
    }

    if(evt == ECGV_EV_KEY_UP_CTRL && !mouse.getInsertMode())
    {
        std::cout << "No Longer Can Select Multiple Shapes" << std::endl;
        mouse.setCtrlDown(false);
    }

    if(evt == ECGV_EV_KEY_DOWN_Y)
    {
        redo = true;
    }

    if(evt == ECGV_EV_KEY_UP_G && mouse.getSomethingSelected())
    {
        composite = true;
    }

    if( evt == ECGV_EV_KEY_DOWN_SPACE)
    {    
        currentlyChanging = true; 
    }

    if( evt == ECGV_EV_KEY_DOWN_G && mouse.getInsertMode() )
    {
        std::cout << "Changed Shape" << std::endl;
        mouse.setElipses(!mouse.getElipses());
    }

    if( evt == ECGV_EV_TIMER)
    {   
        if(currentlyChanging)
        {   
            if(!mouse.getInsertMode())
            {
                mouse.setPressedDown(false);
                mouse.setPressedUp(false);
                mouse.setSomethingSelected(false);
                ctrl.DeselectAll();
                ctrl.ShowAllShapes();
                view.SetRedraw(true);
            }

            std::cout << "Changed mode" << std::endl;

            mouse.setInsertMode(!mouse.getInsertMode());
            currentlyChanging = false;
        }

        if(del)
        {   
            ctrl.Delete();
            ctrl.ShowAllShapes();
            view.SetRedraw(true);
            del = false;
            mouse.setSomethingSelected(false);
        }

        if(undo)
        {
            ctrl.Undo();
            ctrl.ShowAllShapes();
            view.SetRedraw(true);
            undo = false;
        }

        if(redo)
        {
            ctrl.Redo();
            ctrl.ShowAllShapes();
            view.SetRedraw(true);
            redo = false;
        }

        if(left && mouse.getSomethingSelected())
        {
            ctrl.Moved(0, 0, -10, 0);
            ctrl.ShowAllShapes();
            view.SetRedraw(true);
            left = false;
        }

        if(right && mouse.getSomethingSelected())
        {
            ctrl.Moved(0, 0, 10, 0);
            ctrl.ShowAllShapes();
            view.SetRedraw(true);
            right = false;
        }

        if(up && mouse.getSomethingSelected())
        {
            ctrl.Moved(0, 0, 0, -10);
            ctrl.ShowAllShapes();
            view.SetRedraw(true);
            up = false;
        }

        if(down && mouse.getSomethingSelected())
        {
            ctrl.Moved(0, 0, 0, 10);
            ctrl.ShowAllShapes();
            view.SetRedraw(true);
            down = false;
        }

        if(composite && (ctrl.WhatWasSelected() == -1 || ctrl.WhatWasSelected() == 0) )
        {   
            //type 0 or -1 means nothing happened
            ctrl.DeselectAll();
            ctrl.ShowAllShapes();
            view.SetRedraw(true);
            composite = false;
        }


        if(composite && ctrl.WhatWasSelected() == 1)
        {   
            //type 1 means make a composite
            ctrl.CreateComposite();
            ctrl.ShowAllShapes();
            view.SetRedraw(true);
            composite = false;

        }

        if(composite && ctrl.WhatWasSelected() == 2)
        {
            //type 2 means uncomposite shape
            ctrl.UnComposite();
            ctrl.DeselectAll();
            ctrl.ShowAllShapes();
            view.SetRedraw(true);
            composite = false;
        }
    }
}

//********************************************
// Mouse implementations

ECMouseEvtObserver :: ECMouseEvtObserver(ECGraphicViewImp &viewIn, ECGraphicController & ctrlIn) : ctrl(ctrlIn), view(viewIn), wasPressedDown(false), wasPressedUp(false), InsertionModeOn(false), SomethingSelected(false), x1(0), y1(0), x2(0), y2(0), toMove(false), x_moving(0), y_moving(0), CurrentlyMoving(false), Elipses(false), MakeFilled(false), ctrlDown(false)  {}

void ECMouseEvtObserver :: Update()
{
    ECGVEventType evt = view.GetCurrEvent();

    if( evt == ECGV_EV_MOUSE_BUTTON_DOWN)
    {   
        if(InsertionModeOn)
        {
            wasPressedDown = true;

            view.GetCursorPosition(x1, y1);
        }
        
        if(!InsertionModeOn)
        {
            wasPressedDown = true;

            view.GetCursorPosition(x1, y1);
        }
    }

    if( evt == ECGV_EV_MOUSE_BUTTON_UP)
    {
        if(InsertionModeOn)
        {
            wasPressedUp = true;
            wasPressedDown = false;

            view.GetCursorPosition(x2, y2);
        }

        if(!InsertionModeOn)
        {
            wasPressedUp = true;
            wasPressedDown = false;
            view.GetCursorPosition(x2, y2);
            if(x1 == x2 && y1 == y2) 
            {
                toMove = false;
            }
            else
            {
                toMove = true;
            }
        }
    }

    if(evt == ECGV_EV_MOUSE_MOVING)
    {   
        if(!InsertionModeOn && wasPressedDown == true)
        {   
            view.GetCursorPosition(x_moving,y_moving);
            CurrentlyMoving = true;
        }
    }
    // if this is the timer event
    if( evt == ECGV_EV_TIMER )
    {
        // if space bar already pressed, draw a winidow
        // Note: you need to draw this in the timer event; otherwise you won't see the rectangle
        if(InsertionModeOn)
        {   
            if(Elipses == false)    //going to draw a rectangle
            {
                if(wasPressedDown == true && wasPressedUp == false) //the drag animation for a rectangle
                {   
                    ctrl.ShowAllShapes();
                    view.GetCursorPosition(x2,y2);
                    view.DrawRectangle(x1,y1,x2,y2, 2);
                    view.SetRedraw(true);
                }

                if(wasPressedUp == true && MakeFilled == false) //creating an unfilled rectangle
                {   
                    ctrl.ShowAllShapes();
                    ctrl.CreateRectangle(x1,y1,x2,y2);
                    view.SetRedraw(true);
                    wasPressedUp = false;
                    wasPressedDown == false;
                }

                if(wasPressedUp == true && MakeFilled == true)  //creating an filled rectangle
                {   
                    ctrl.ShowAllShapes();
                    ctrl.CreateRectangleFilled(x1,y1,x2,y2);
                    view.SetRedraw(true);
                    wasPressedUp = false;
                    wasPressedDown == false;
                }
            }
            else if(Elipses == true)    //else we're creating an elipses
            {
                if(wasPressedDown == true && wasPressedUp == false) //the drag animation for a elipses
                {   
                    ctrl.ShowAllShapes();
                    view.GetCursorPosition(x2,y2);
                    int xcenter = (x1 + x2) / 2;
                    int ycenter = (y1 + y2) / 2;
                    double xradius = std::abs(x2 - x1) / 2.0;
                    double yradius = std::abs(y2 - y1) / 2.0;
                    view.DrawEllipse(xcenter,ycenter,xradius, yradius, 1);
                    view.SetRedraw(true);
                }

                if(wasPressedUp == true && MakeFilled == false) //creating an unfilled elipse
                {   
                    ctrl.ShowAllShapes();
                    int xcenter = (x1 + x2) / 2;
                    int ycenter = (y1 + y2) / 2;
                    double xradius = std::abs(x2 - x1) / 2.0;
                    double yradius = std::abs(y2 - y1) / 2.0;
                    ctrl.CreateElipses(xcenter,ycenter,xradius,yradius);
                    view.SetRedraw(true);
                    wasPressedUp = false;
                    wasPressedDown == false;
                }

                if(wasPressedUp == true && MakeFilled == true)  //creating an filled elipse
                {   
                    ctrl.ShowAllShapes();
                    int xcenter = (x1 + x2) / 2;
                    int ycenter = (y1 + y2) / 2;
                    double xradius = fabs(x2 - x1) / 2.0;
                    double yradius = fabs(y2 - y1) / 2.0;
                    ctrl.CreateElipsesFilled(xcenter,ycenter,xradius,yradius);
                    view.SetRedraw(true);
                    wasPressedUp = false;
                    wasPressedDown == false;
                }
            }
        }

        if(!InsertionModeOn)
        {   
            
            if(wasPressedDown == true && ctrl.AlreadySelected(x1, y1) == false && CurrentlyMoving == false && ctrlDown == false)
            {   
                //single selection
                ctrl.ShowAllShapes();
                view.SetRedraw(true);
                ctrl.Select(x1,y1);
                ctrl.ShowAllShapes();
                view.SetRedraw(true);            
                SomethingSelected = true;
                wasPressedDown = false;
                wasPressedUp == false;
            }

            if(wasPressedDown == true && ctrl.AlreadySelected(x1, y1) == false && CurrentlyMoving == false && ctrlDown == true)
            {   
                //multiple selection
                ctrl.ShowAllShapes();
                view.SetRedraw(true);
                ctrl.SelectMultiple(x1,y1);
                ctrl.ShowAllShapes();
                view.SetRedraw(true);            
                SomethingSelected = true;
                wasPressedDown = false;
                wasPressedUp == false;
            }

            if(wasPressedDown == true && CurrentlyMoving == true)
            {   
                //shape moving animation
                ctrl.ShowAllShapes();
                view.GetCursorPosition(x2, y2);
                ctrl.Moving(x1, y1, x2, y2);
                view.SetRedraw(true);
            }

            if(SomethingSelected == true && wasPressedUp == true && toMove == true)
            {   
                //actually moving highlighted shape(s) location
                ctrl.Moved(x1, y1, x2, y2);
                ctrl.ShowAllShapes();
                view.SetRedraw(true);
                SomethingSelected = true;
                wasPressedDown = false;
                wasPressedUp = false;
                CurrentlyMoving = false;
            }

        }
    }
}
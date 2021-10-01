/*

    ofxWinMenu basic example - ofApp.cpp

    Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.
    
    Copyright (C) 2016-2017 Lynn Jarvis.

    https://github.com/leadedge

    http://www.spout.zeal.co

    =========================================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    =========================================================================

    03.11.16 - minor comment cleanup
    21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {

    ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
    ofSetFrameRate(15);
    ofBackground(255, 255, 255);
    // Get the window size for image loading
    windowWidth = ofGetWidth();
    windowHeight = ofGetHeight();
    isdfs = false;
    isOpen = 0;
    // Centre on the screen
    ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

    // Load a font rather than the default
    myFont.loadFont("verdana.ttf", 12, true, true);

    // Load an image for the example
    //myImage.loadImage("lighthouse.jpg");

    // Window handle used for topmost function
    hWnd = WindowFromDC(wglGetCurrentDC());

    // Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
    ofSetEscapeQuitsApp(false);

    //
    // Create a menu using ofxWinMenu
    //

    // A new menu object with a pointer to this class
    menu = new ofxWinMenu(this, hWnd);

    // Register an ofApp function that is called when a menu item is selected.
    // The function can be called anything but must exist.
    // See the example "appMenuFunction".
    menu->CreateMenuFunction(&ofApp::appMenuFunction);

    // Create a window menu
    HMENU hMenu = menu->CreateWindowMenu();

    //
    // Create a "File" popup menu
    //
    HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

    //
    // Add popup items to the File menu
    //

    // Open an maze file
    menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked
    
    // Final File popup menu item is "Exit" - add a separator before it
    menu->AddPopupSeparator(hPopup);
    menu->AddPopupItem(hPopup, "Exit", false, false);

    //
    // View popup menu
    //
    hPopup = menu->AddPopupMenu(hMenu, "View");

    bShowInfo = true;  // screen info display on
    menu->AddPopupItem(hPopup, "Show DFS",false,false); // Checked
    bTopmost = false; // app is topmost
    menu->AddPopupItem(hPopup, "Show BFS"); // Not checked (default)
    bFullscreen = false; // not fullscreen yet
    menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

    //
    // Help popup menu
    //
    hPopup = menu->AddPopupMenu(hMenu, "Help");
    menu->AddPopupItem(hPopup, "About", false, false); // No auto check

    // Set the menu to the window
    menu->SetWindowMenu();

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
//
void ofApp::appMenuFunction(string title, bool bChecked) {

    ofFileDialogResult result;
    string filePath;
    size_t pos;

    //
    // File menu
    //
    if(title == "Open") {
        readFile();
    }
    if(title == "Exit") {
        ofExit(); // Quit the application
    }

    //
    // Window menu
    //
    if(title == "Show DFS") {
        //bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
        if (isOpen)
        {
            DFS();
            bShowInfo = bChecked;
        }
        else
            cout << "you must open file first" << endl;
        
    }

    if(title == "Show BFS") {
        doTopmost(bChecked); // Use the checked value directly

    }

    if(title == "Full screen") {
        bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
        doFullScreen(bFullscreen); // But als take action immediately
    }

    //
    // Help menu
    //
    if(title == "About") {
        ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
    }

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {

    char str[256];
    //ofBackground(0, 0, 0, 0);
    ofSetColor(100);
    ofSetLineWidth(5);
    int i, j;
    
    // TO DO : DRAW MAZE;
    // using data structure draw maze
    // add code here
    ofSetColor(0,0,255);
    ofDrawRectangle(0, 0, 10 * maze_col +1, 1, 2);

    for(int i= 0; i < maze_row; i++) {
        ofSetColor(0,0,255);
        ofDrawRectangle(0, 1 + 10 * i, 1, 1 + 10 * (i+1), 2);
        for(int j = 0; j < maze_col; j++) {
            if(!maze[i][j].right_flag)
                ofSetColor(0,0,255);
                ofDrawRectangle(10 + j * 10, 1 + 10 * i, 11 + j * 10, 10 * (i+1), 2);
        }
        for(int j = 0; j < maze_col; j++) {
            if(!maze[i][j].down_flag)
                ofSetColor(0,0,255);
                ofDrawRectangle(1 + j * 10, 10 * (i+1), 10 + j * 10,1 + 10* (i+1), 2);
            ofSetColor(0,0,255);
            ofDrawRectangle(10 + j * 10, 10 * (i+1), 11 + j * 10,1 + 10* (i+1), 2);
        }
    }
    
    if (isdfs)
    {
        ofSetColor(200);
        ofSetLineWidth(5);
        if (isOpen)
            dfsdraw();
        else
            cout << "You must open file first" << endl;
    }
    if(bShowInfo) {
        // Show keyboard duplicates of menu functions
        sprintf(str, " comsil project");
        myFont.drawString(str, 15, ofGetHeight()-20);
    }

} // end Draw


void ofApp::doFullScreen(bool bFull)
{
    // Enter full screen
    if(bFull) {
        // Remove the menu but don't destroy it
        menu->RemoveWindowMenu();
        // hide the cursor
        ofHideCursor();
        // Set full screen
        ofSetFullscreen(true);
    }
    else {
        // return from full screen
        ofSetFullscreen(false);
        // Restore the menu
        menu->SetWindowMenu();
        // Restore the window size allowing for the menu
        ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU));
        // Centre on the screen
        ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())/2, (ofGetScreenHeight()-ofGetHeight())/2);
        // Show the cursor again
        ofShowCursor();
        // Restore topmost state
        if(bTopmost) doTopmost(true);
    }

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
    if(bTop) {
        // get the current top window for return
        hWndForeground = GetForegroundWindow();
        // Set this window topmost
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        ShowWindow(hWnd, SW_SHOW);
    }
    else {
        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        ShowWindow(hWnd, SW_SHOW);
        // Reset the window that was topmost before
        if(GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
            SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        else
            SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    
    // Escape key exit has been disabled but it can be checked here
    if(key == VK_ESCAPE) {
        // Disable fullscreen set, otherwise quit the application as usual
        if(bFullscreen) {
            bFullscreen = false;
            doFullScreen(false);
        }
        else {
            ofExit();
        }
    }

    // Remove or show screen info
    if(key == ' ') {
        bShowInfo = !bShowInfo;
        // Update the menu check mark because the item state has been changed here
        menu->SetPopupItem("Show DFS", bShowInfo);
    }

    if(key == 'f') {
        bFullscreen = !bFullscreen;
        doFullScreen(bFullscreen);
        // Do not check this menu item
        // If there is no menu when you call the SetPopupItem function it will crash
    }

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
bool ofApp::readFile()
{
    ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
    string filePath;
    size_t pos;
    // Check whether the user opened a file
    if (openFileResult.bSuccess) {
        ofLogVerbose("User selected a file");

        //We have a file, check it and process it
        string fileName = openFileResult.getName();
        //string fileName = "maze0.maz";
        printf("file name is %s\n", fileName);
        filePath = openFileResult.getPath();
        printf("Open\n");
        pos = filePath.find_last_of(".");
        if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

            ofFile file(fileName);

            if (!file.exists()) {
                cout << "Target file does not exists." << endl;
                return false;
            }
            else {
                cout << "We found the target file." << endl;
                isOpen = 1;
            }

            ofBuffer buffer(file);

            // Input_flag is a variable for indication the type of input.
            // If input_flag is zero, then work of line input is progress.
            // If input_flag is one, then work of dot input is progress.
            int input_flag = 0;

            // Idx is a variable for index of array.
            int idx = 0;

            // Read file line by line
            int cnt = 0;
            char x;
            FILE *fp;
            
            fp = fopen(filename, "r");
            
            maze_row = 1; maze_col = 1;
            while(fscanf(fp, "%c", &x) != EOF) {
                if(maze_col == 1) {
                    if(x != '\n')
                        cnt++;
                    else if(x == '\n')
                        maze_col = cnt/2;
                }
                if(x == '\n')
                    maze_row++;
            }
            maze_row/=2;
            fclose(fp);

            maze = (vertex**)malloc(sizeof(vertex*) * maze_row);
            for(int i= 0;i < maze_row;i++)
                maze[i] = (vertex*)malloc(sizeof(vertex) * maze_col);

            for(int i = 0; i < maze_row;i++) {
                for(int j = 0; j < maze_col; j++) {
                    maze[i][j].down_flag = 0, maze[i][j].right_flag = 0;
                }
            }
            
            visit = (int **)malloc(sizeof(int *) * row);
            for(int i = 0; i < row; i++)
                visit[i] = (int *)malloc(sizeof(int) * col);
            stack = (nptr)malloc(sizeof(node) * row * col);
            stackAll = (nptr)malloc(sizeof(node) * row * col * 3);
            queueAll = (qptr)malloc(sizeof(quenode) * row * col);

            for(int i = 0; i < row; i++)
                for(int j = 0; j < col; j++)
                    visit[i][j] = 0;

            fp = fopen(filename, "r");
            
            for(int i = 0; i < 2 * maze_col+2; i++)
                fgetc(fp);
            for(int i= 0; i < maze_row; i++) {
                fgetc(fp);
                for(int j = 0; j < maze_col; j++) {
                    fgetc(fp);
                    x = fgetc(fp);
                    if(x == '|')
                        maze[i][j].right_flag = 0;
                    else
                        maze[i][j].right_flag = 1;
                }
                x = fgetc(fp);
                fgetc(fp);
                for(int j = 0; j < maze_col; j++) {
                    x = fgetc(fp);
                    fgetc(fp);
                    if(x == '-')
                        maze[i][j].down_flag = 0;
                    else
                        maze[i][j].down_flag = 1;
                }
                if(i != maze_row-1)
                    fgetc(fp);
            }
            fclose(fp);

            setWindow(0,0,10 * maze_col + 15, 10 * maze_row + 15, 1);
            return true;
            
            // TO DO
            // .maz as input put in data structure
            
        }
        else {
            printf("  Needs a '.maz' extension\n");
            return false;
        }
    }
}
void ofApp::freeMemory() {
    for(int i=0; i <maze_row; i++)
        free(maze[i]);
    free(maze);
    maze_row = 0;
    maze_col = 0;
    
}

bool ofApp::DFS()//DFS search function
{
    dflag = 1;
    if(sflag)
        return;

    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            visit[i][j] = 0;
    
    vertex V;
    int r = 0, c = 0;

    V = maze[r][c];
    visit[r][c] = 1;
    pushStack(r, c);
    pushStackAll(r, c);

    while(1) {

        V = maze[r][c];

        if (r == row-1 && c == col-1)
            break;

        if(V.right_flag && !visit[r][c+1]) {
            c++;
            visit[r][c] = 1;
            pushStackAll(r, c);
            pushStack(r, c);
            continue;
        }
        if(V.down_flag && !visit[r+1][c]) {
            r++;
            visit[r][c] = 1;
            pushStackAll(r, c);
            pushStack(r, c);
            continue;
        }
        if(c-1 >= 0)
            if(maze[r][c-1].right_flag && !visit[r][c-1]) {
                c--;
                visit[r][c] = 1;
                pushStackAll(r, c);
                pushStack(r, c);
                continue;
            }
        
        if(r-1 >= 0)
            if(maze[r-1][c].down_flag && !visit[r-1][c]) {
                r--;
                visit[r][c] = 1;
                pushStackAll(r, c);
                pushStack(r, c);
                continue;
            }
        
        popStack(&r, &c);
        pushStackAll(r, c);
    }

    sflag = 1;
    return 0;
    //TO DO
    //DFS search function
}

void ofApp::enqueueAll(int i, int j, char direc) {
    queueAll[queueA_count].i = i;
    queueAll[queueA_count].j = j;
    queueAll[queueA_count++].direction = direc;
}

void ofApp::pushStack(int i,int j) {
    stack[stack_count].i = i;
    stack[stack_count++].j = j;
}
void ofApp::pushStackAll(int i,int j) {
    stackAll[stackA_count].i = i;
    stackAll[stackA_count++].j = j;
}

void ofApp::popStack(int *i, int *j) {
    *i = stack[(--stack_count)-1].i;
    *j = stack[stack_count-1].j;
}

void ofApp::dfsdraw()
{
    dflag = -1;
    if(qflag)
        return;

    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            visit[i][j] = 0;

    vertex V;
    int r = 0, c = 0, k = 0;
    enqueueAll(r, c, 'u');
    visit[r][c] = 1;

    while(1) {
        r = queueAll[k].i;    c = queueAll[k].j;
        V = maze[r][c];
        visit[r][c] = 1;

        if(r == row-1 && c == col-1) {
            queueA_count = k;
            break;
        }

        if(V.right_flag && !visit[r][c+1])
            enqueueAll(r, c+1, 'l');
        if(V.down_flag && !visit[r+1][c])
            enqueueAll(r+1, c, 'u');
        if(r-1 >= 0)
        if(maze[r-1][c].down_flag && !visit[r-1][c])
            enqueueAll(r-1, c, 'd');
        if(c-1 >= 0)
            if(maze[r][c-1].right_flag && !visit[r][c-1])
                enqueueAll(r, c-1, 'r');
        

        k++;
    }

    qflag = 1;
    //TO DO
    //DFS result
}

void ofApp::drawBuffered(){
    int prerow, precol, crow, ccol, curr;
    char direc;
    ofSetColor(0,0,255);
    ofDrawRectangle(0, 0, 10 * col +1, 1, 2\);

    for(int i= 0; i < row; i++) {
        ofSetColor(0,0,255);
        ofDrawRectangle(0, 1 + 10 * i, 1, 1 + 10 * (i+1), 2);
        for(int j = 0; j < col; j++) {
            if(!maze[i][j].right_flag)
                ofSetColor(0,0,255);
                ofDrawRectangle(10 + j * 10, 1 + 10 * i, 11 + j * 10, 10 * (i+1), 2);
        }
        for(int j = 0; j < col; j++) {
            if(!maze[i][j].down_flag)
                ofSetColor(0,0,255);
                ofDrawRectangle(1 + j * 10, 10 * (i+1), 10 + j * 10,1 + 10* (i+1), 2);
            ofSetColor(0,0,255);
            ofDrawRectangle(10 + j * 10, 10 * (i+1), 11 + j * 10,1 + 10* (i+1), 2);
        }
    }
    ofSetColor(0,0,100);
    ofDrawRectangle(3, 3, 8, 8, 2);
    ofSetColor(100,0,0);
    ofDrawRectangle(10 * (col - 1) + 3, 10 * (row - 1) + 3, 10 * (col - 1) + 8, 10 * (row - 1) + 8, 2);
    
    if(sflag && dflag == 1) {
        for(int x = 1; x < stackA_count; x++) {
            prerow = stackAll[x-1].i; precol = stackAll[x-1].j;
            crow = stackAll[x].i; ccol = stackAll[x].j;
            if(ccol > precol)
                ofSetColor(200,100,0);
                ofDrawRectangle(10 * precol + 7, 10 * prerow + 4, 10 * ccol + 7, 10 * crow + 7, 2);
            else if (crow > prerow)
                ofSetColor(200,100,0);
                ofDrawRectangle(10 * precol + 4, 10 * prerow + 7, 10 * ccol + 7, 10 * crow + 7, 2);
            else if(precol > ccol)
                ofSetColor(200,100,0);
                ofDrawRectangle(10 * ccol + 4, 10 * crow + 4, 10 * precol + 4, 10 * prerow + 7, 2);
            else if (prerow > crow)
                ofSetColor(200,100,0);
                ofDrawRectangle(10 * ccol + 4, 10 * crow + 4, 10 * precol + 7, 10 * prerow + 4, 2);
        }
        for(int x = 1; x < stack_count; x++) {
            prerow = stack[x-1].i; precol = stack[x-1].j;
            crow = stack[x].i; ccol = stack[x].j;
            if(ccol > precol)
                ofSetColor(0,0,255);
                ofDrawRectangle(10 * precol + 7, 10 * prerow + 4, 10 * ccol + 7, 10 * crow + 7, 2);
            else if (crow > prerow)
                ofSetColor(0,0,255);
                ofDrawRectangle(10 * precol + 4, 10 * prerow + 7, 10 * ccol + 7, 10 * crow + 7, 2);
            else if(precol > ccol)
                ofSetColor(0,0,255);
                ofDrawRectangle(10 * ccol + 4, 10 * crow + 4, 10 * precol + 4, 10 * prerow + 7, 2);
            else if (prerow > crow)
                ofSetColor(0,0,255);
                ofDrawRectangle(10 * ccol + 4, 10 * crow + 4, 10 * precol + 7, 10 * prerow + 4, 2);
        }
    }

    if(qflag && dflag == -1) {
        for(int x = 1; x < queueA_count; x++) {
            crow = queueAll[x].i;    ccol = queueAll[x].j;    direc = queueAll[x].direction;
            switch(direc) {
            case 'l':
                    ofSetColor(0,0,255);
                ofDrawRectangle(10 * (ccol-1) + 7, 10 * crow + 4,  10 * ccol + 7, 10 * crow + 7, 2);
                break;
            case 'u':
                ofSetColor(0,0,255);
                ofDrawRectangle(10 * ccol + 4, 10 * (crow-1) + 7, 10 * ccol + 7, 10 * crow + 7, 2);
                break;
            case 'r':
                ofSetColor(0,0,255);
                ofDrawRectangle(10 * ccol + 4, 10 * crow + 4, 10 * (ccol+1) + 4, 10 * crow + 7, 2);
                break;
            case 'd':
                ofSetColor(0,0,255);
                ofDrawRectangle(10 * ccol + 4, 10 * crow + 4, 10 * ccol + 7, 10 * (crow+1) + 7, 2);
            }
        }
        curr = queueA_count;
        crow = queueAll[curr].i;    ccol = queueAll[curr].j; direc = queueAll[curr].direction;
        while(crow != 0 || ccol != 0) {
            switch(direc) {
            case 'l':
                ofSetColor(0,0,255);
                ofDrawRectangle(10 * (ccol-1) + 7, 10 * crow + 4,  10 * ccol + 7, 10 * crow + 7, 2);
                prerow = crow;    precol = ccol - 1;
                break;
            case 'u':
                ofSetColor(0,0,255);
                ofDrawRectangle(10 * ccol + 4, 10 * (crow-1) + 7, 10 * ccol + 7, 10 * crow + 7, 2);
                prerow = crow - 1;    precol = ccol;
                break;
            case 'r':
                ofSetColor(0,0,255);
                ofDrawRectangle(10 * ccol + 4, 10 * crow + 4, 10 * (ccol+1) + 4, 10 * crow + 7, 2);
                prerow = crow;    precol = ccol + 1;
                break;
            case 'd':
                ofSetColor(0,0,255);
                ofDrawRectangle(10 * ccol + 4, 10 * crow + 4, 10 * ccol + 7, 10 * (crow+1) + 7, 2);
                prerow = crow + 1;    precol = ccol;
            }
            while(prerow != queueAll[curr].i || precol != queueAll[curr].j)
                curr--;
            crow = prerow;    ccol = precol;    direc = queueAll[curr].direction;
        }
    }
}

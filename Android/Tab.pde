class Tab {
  int x, y;
  int tabW, tabH;
  String text;
  
  boolean isActive;
  Tab(int xPos, int yPos, int tabWidth, int tabHeight, String textToDisplay) {

    x=xPos;
    y=yPos;
    tabW=tabWidth;
    tabH=tabHeight;
    isActive=false;
    text=textToDisplay;
  }
  void drawTab() {
    if (isActive) {
      fill(70, 130, 180);
    }
    else {
      fill(131, 137, 150);
    }
    rect(x, y, tabW, tabH);
    fill(255);
    textSize(26);
    textAlign(CENTER);
    text(text, x+(tabW/2), y+(tabH/2));
    
  }
  void check() {
    if (mouseX>x && mouseX<x+tabW && mouseY>y && mouseY<y+tabH) {
      isActive=true;
    }
    else if (mouseY>y+tabH){
      //isActive=false;
    }
    else{
      isActive=false;
    }
  }
}


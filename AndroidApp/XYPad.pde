class XYPad{
  float x, y, padW, padH;
  XYPad(float xPos, float yPos, float pWidth, float pHeight){
    x=xPos;
    y=yPos;
    padW=pWidth;
    padH=pHeight;
  }
  void drawPad(){
    fill(255);
    rect(x,y,padW,padH);
    stroke(0);
    for(float i=x; i<padW; i+=10){
      for (float j=y; j<padH; j+=10){
        point(i,j);
      }
    }
  }
}

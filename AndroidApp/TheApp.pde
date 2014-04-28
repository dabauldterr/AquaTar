//required for BT enabling on startup
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.app.AlertDialog;
import android.app.Activity;
import android.content.DialogInterface;

import ketai.net.bluetooth.*;
import ketai.ui.*;
import ketai.net.*;

//import oscP5.*;

Tab t1, t2, t3, t4;
XYPad xy;
Slider s1, s2, s3, s4;

int noOfTabs=2;

KetaiBluetooth bt;
String info = "";
KetaiList klist;
KetaiAlertDialog alert;

ArrayList<String> devicesDiscovered = new ArrayList();
boolean isConfiguring = true;
String UIText;

boolean backState=false;

void setup() {
  orientation(PORTRAIT);
  
  t1=new Tab(0, 0, width/noOfTabs, height/8, "XY Pad");
  t1.isActive=true;
  t2=new Tab(width/noOfTabs, 0, width/noOfTabs, height/8, "Settings");
  xy=new XYPad(0, height/8, width, height);

  s1=new Slider("A", 0, 0, 255, 50, 400, width-60, 60, HORIZONTAL);
  s2=new Slider("B", 0, 0, 255, 50, 600, width-60, 60, HORIZONTAL);
  s3=new Slider("C", 0, 0, 255, 50, 800, width-60, 60, HORIZONTAL);
  s4=new Slider("D", 0, 0, 255, 50, 1000, width-60, 60, HORIZONTAL);
  
  bt.start();
}

void draw() {
  if (!backState) {
    if (isConfiguring) {
      //create the BtSerial object that will handle the connection
      //with the list of paired devices
      klist = new KetaiList(this, bt.getDiscoveredDeviceNames());

      isConfiguring = false;                         //stop selecting device
    }
    else
    {
      drawTabs();
      //Display Tab1's content
      if (t1.isActive==true) {
        xy.drawPad();
      }
      //Display Tab2's content
      else if (t2.isActive==true) {
        drawSettingsPage();
      }
    }
  }
  else {
    popup(this, "Quit?", "Do you want to quit?");
  }
}
void drawSettingsPage() {
  fill(145, 163, 176);
  rect(0, height/8, width, height);
  s1.display();
  s2.display();
  s3.display();
  s4.display();
}
void drawTabs() {
  t1.drawTab();
  t2.drawTab();
}
void checkTabs() {
  t1.check();
  t2.check();
}
/*

 MOUSE PRESSED SCREEN EVENTS
 
 */
void mousePressed() {
  checkTabs();
  if (t1.isActive && mouseY>height/8) {
    sendFromXY();
  }
  else if (t2.isActive) {
    s1.mousePressed();
    s2.mousePressed();
    s3.mousePressed();
    s4.mousePressed();
    sendFromSettings();
  }
}
void mouseDragged() {
  if (t1.isActive && mouseY>height/8) {
    sendFromXY();
  }
  else if (t2.isActive) {
    s1.mouseDragged();
    s2.mouseDragged();
    s3.mouseDragged();
    s4.mouseDragged();
    sendFromSettings();
  }
}
void mouseReleased() {
  if (t1.isActive && mouseY>height/8) {
    sendFromXY();
  }
  else if (t2.isActive) {
    s1.mouseReleased();
    s2.mouseReleased();
    s3.mouseReleased();
    s4.mouseReleased();
    sendFromSettings();
  }
}

/*

 Key Events
 
 */
//void keyPressed() {
//  if (key == CODED && keyCode == KeyEvent.KEYCODE_BACK) { 
//    keyCode=0;
//    backState =!backState; 
//    //Do stuff here
//  }
//}

boolean surfaceKeyDown(int code, KeyEvent event) {
  if (event.getKeyCode() == KeyEvent.KEYCODE_BACK) {
    if (!backState) {
      //backState=!backState;
      popup(this, "Quit?", "Do you want to quit?");
      return false;
    }
  }
  return super.surfaceKeyDown(code, event);
}

boolean surfaceKeyUp(int code, KeyEvent event) {
  return super.surfaceKeyDown(code, event);
}

public void popup(Activity _parent, String _title, String _message) {
  final Activity parent = _parent;
  final String message = _message;
  final String title = _title;

  parent.runOnUiThread(new Runnable() {
    public void run() {
      new AlertDialog.Builder(parent)
        .setTitle(title)
        .setMessage(message)
        .setPositiveButton("OK", new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialog, int which) {
          bt.stop();
          //finish();
        }
      }
      )
        .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialog, int which) {
          // User cancelled the dialog
        }
      }
      ).show();
    }
  }
  );
}
/*

 BLUETOOTH METHODS
 
 */
//********************************************************************
// The following code is required to enable bluetooth at startup.
//********************************************************************
void onCreate(Bundle savedInstanceState) {
  super.onCreate(savedInstanceState);
  bt = new KetaiBluetooth(this);
}

void onActivityResult(int requestCode, int resultCode, Intent data) {
  bt.onActivityResult(requestCode, resultCode, data);
}

//********************************************************************


void onKetaiListSelection(KetaiList klist)
{
  String selection = klist.getSelection();            //select the device to connect
  bt.connectToDeviceByName(selection);        //connect to the device
  klist = null;                                                      //dispose of bluetooth list for now
}
//Call back method to manage data received
void onBluetoothDataEvent(String who, byte[] data)
{
  if (isConfiguring)
    return;
  //received
  info += new String(data);
  if (info.length() > 150)             //clean the words on screen if string to long
    info = "";
}
//Send data from the XY Pad
void sendFromXY() {
  float x=map(mouseX, 0, width, 0, 255);
  String xVar="X" + nf(int(x), 3);
  byte data[]= xVar.getBytes();
  bt.broadcast(data);
  
  float y= map(mouseY, height/8, height, 0, 255);
  String yVar="Y" +nf(int(y), 3); 
  data=yVar.getBytes();
  bt.broadcast(data);
}
//Send data from the sliders page
void sendFromSettings() {
  String send="A"+nf(int(s1.get()), 3);
  bt.broadcast(send.getBytes());
  
  send="B"+nf(int(s2.get()), 3);
  bt.broadcast(send.getBytes());
  
  send="C"+nf(int(s3.get()), 3);
  bt.broadcast(send.getBytes());
  
  send="D"+nf(int(s4.get()), 3);
  bt.broadcast(send.getBytes());
}

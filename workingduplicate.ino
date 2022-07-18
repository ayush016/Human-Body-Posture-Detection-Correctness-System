#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
//ADXL345
#include <Wire.h>
#include "HTTPSRedirect.h"
/* Set these to your desired credentials. */
const char *ssid = "ayush";
const char *password = "ayush12345";
int wrong_p=0;
int correct_p=0;
int lock=0;
float final_p=0;
ESP8266WebServer server(80);

float healthvalues[12];
int incr=0;
int index1=0;
#define DEBUG true
#define Serial if(DEBUG)Serial

#define DEVICE (0x53) // Device address as specified in data sheet
float last_value = 0;
float prelast_value = 0;

int show_count = 0; 
int trigger_count = 0;
float trigger_value = -5; //DEFAULT VALUE ???
float current_value = 0;
float b = 0;
float c = 0;
int xaxis=0;
int center_x,center_y,center_z;
int coordinate1_x,coordinate1_y,coordinate1_z;
int coordinate2_x,coordinate2_y,coordinate2_z;
int coordinate3_x,coordinate3_y,coordinate3_z;
int coordinate4_x,coordinate4_y,coordinate4_z;
/*****************************************************/

const char *GScriptId = "AKfycbzhtrErTHbp5RdznmUbvs09tf-pCcKYJdpg18XtJtKqEZJnvKA_";
const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";
const int httpsPort =  443;
HTTPSRedirect client(httpsPort);
String url = String("/macros/s/") + GScriptId + "/exec?";

const char* fingerprint = "F0 5C 74 77 3F 6B 25 D7 3B 66 4D 43 2F 7E BC 5B E9 28 86 AD";

/*****************************************************/

#define ADXL345_MG2G_MULTIPLIER (0.004)
#define SENSORS_GRAVITY_STANDARD          (SENSORS_GRAVITY_EARTH)
#define SENSORS_GRAVITY_EARTH             (9.80665F)              /**< Earth's gravity in m/s^2 */

byte _buff[6];
char POWER_CTL = 0x2D;    //Power Control Register
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;    //X-Axis Data 0
char DATAX1 = 0x33;    //X-Axis Data 1
char DATAY0 = 0x34;    //Y-Axis Data 0
char DATAY1 = 0x35;    //Y-Axis Data 1
char DATAZ0 = 0x36;    //Z-Axis Data 0
char DATAZ1 = 0x37;    //Z-Axis Data 1

int tempo=0;
float max_x=0;
float min_x=0;
float cal_x=0;
float x = 0;
int count=0;
//Check if header is present and correct

bool is_authentified(){
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")){   
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;  
}

//root page can be accessed only if authentification is ok
void handleRoot(){
  //Serial.println("Enter handleRoot");
  String header;
Serial.println("Wait");
  //if (!is_authentified())
 /* {
    String header = "HTTP/1.1 301 OK\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }*/
  
  String content = "<html><body><head><script src='https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js'></script> <script src='https://cdn.plot.ly/plotly-latest.min.js'></script><!--<meta http-equiv='refresh' content='1'><meta charset='UTF-8'>--> <meta name='viewport' content='width=device-width, initial-scale=1'> <link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'> <link rel='stylesheet' href='https://fonts.googleapis.com/css?family=Raleway'> <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'> <style> html,body,h1,h2,h3,h4,h5 {font-family: 'Raleway', sans-serif} </style> <body class='w3-light-grey'> <!-- Top container --> <div class='w3-bar w3-top w3-black w3-large' style='z-index:4'> <button class='w3-bar-item w3-button w3-hide-large w3-hover-none w3-hover-text-light-grey' onclick='w3_open();'><i class='fa fa-bars'></i> Menu</button> <span class='w3-bar-item w3-right'>Logo</span> </div> <!-- Sidebar/menu --> <nav class='w3-sidebar w3-collapse w3-white w3-animate-left' style='z-index:3;width:300px;' id='mySidebar'><br> <div class='w3-container w3-row'> <div class='w3-col s4'> <img src='https://www.w3schools.com/w3images/avatar2.png' class='w3-circle w3-margin-right' style='width:46px'> </div> <div class='w3-col s8 w3-bar'> <span>Welcome</span><br> <a href='#' class='w3-bar-item w3-button'><i class='fa fa-envelope'></i></a> <a href='#' class='w3-bar-item w3-button'><i class='fa fa-user'></i></a> <a href='#' class='w3-bar-item w3-button'><i class='fa fa-cog'></i></a> </div> </div> <hr> <div class='w3-container'> <h5>Dashboard</h5> </div> <div class='w3-bar-block'> <a href='#' class='w3-bar-item w3-button w3-padding-16 w3-hide-large w3-dark-grey w3-hover-black' onclick='w3_close()' title='close menu'><i class='fa fa-remove fa-fw'></i> Close Menu</a> <a href='#' class='w3-bar-item w3-button w3-padding w3-blue'><i class='fa fa-users fa-fw'></i> Overview</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-eye fa-fw'></i> Views</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-users fa-fw'></i> Traffic</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-bullseye fa-fw'></i> Geo</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-diamond fa-fw'></i> Orders</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-bell fa-fw'></i> News</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-bank fa-fw'></i> General</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-history fa-fw'></i> History</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-cog fa-fw'></i> Settings</a><br><br> </div> </nav> <!-- Overlay effect when opening sidebar on small screens --> <div class='w3-overlay w3-hide-large w3-animate-opacity' onclick='w3_close()' style='cursor:pointer' title='close side menu' id='myOverlay'></div> <!-- !PAGE CONTENT! --> <div class='w3-main' style='margin-left:300px;margin-top:43px;'> <!-- Header --> <header class='w3-container' style='padding-top:22px'> <h5><b><i class='fa fa-dashboard'></i> My Dashboard</b></h5> </header> <div class='w3-row-padding w3-margin-bottom'> <div class='w3-quarter'> <div class='w3-container w3-red w3-padding-16'> <div class='w3-left'><i class='fa fa-comment w3-xxxlarge'></i></div> <div class='w3-right'> <h3>52</h3> </div> <div class='w3-clear'></div> <h4>Messages</h4> </div> </div> <div class='w3-quarter'> <div class='w3-container w3-blue w3-padding-16'> <div class='w3-left'><i class='fa fa-eye w3-xxxlarge'></i></div> <div class='w3-right'> <h3>99</h3> </div> <div class='w3-clear'></div> <h4>Views</h4> </div> </div> <div class='w3-quarter'> <div class='w3-container w3-teal w3-padding-16'> <div class='w3-left'><i class='fa fa-share-alt w3-xxxlarge'></i></div> <div class='w3-right'> <h3>23</h3> </div> <div class='w3-clear'></div> <h4>Shares</h4> </div> </div> <div class='w3-quarter'> <div class='w3-container w3-orange w3-text-white w3-padding-16'> <div class='w3-left'><i class='fa fa-users w3-xxxlarge'></i></div> <div class='w3-right'> <h3>50</h3> </div> <div class='w3-clear'></div> <h4>Users</h4> </div> </div> </div> <div class='w3-panel'> <div class='w3-row-padding' style='margin:0 -16px'> <div class='w3-twothird'> <h5>Feeds</h5> <table class='w3-table w3-striped w3-white'>";
  //<H2>hello, you successfully connected to esp8266!</H2><br>";

  if (server.hasHeader("User-Agent"))
  {
    //content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
     content += "<tr><div id ='mydiv'> x :" + String((int)current_value ) + " y :" + String((int)b ) + " z :" + String((int)c ) /*+ " Corr(x):" + String((float)current_value - cal_x ) + " min_x:" + String((float)min_x) + " max_x:" + String((float)max_x)*/ + "<br><br>";
     
     //content += "Trigger value:" + String((float)trigger_value) + " trigger count:" + String(trigger_count) + "<br><br>";
     if(xaxis==0)
     {
       if(current_value<0)
       {
          Serial.print(count);
          count++;
          content += String("Wrong")+"<br><br>";
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH);
         
       }
       else
       {
          count=0;
          content += String("Correct")+"<br><br>";  
          digitalWrite(13, HIGH);
          digitalWrite(14, LOW);      
       }
     }
       else
       {
           if(current_value>(xaxis+int(10)) || current_value<(xaxis-int(10))) 
         {
            Serial.print(count);
            count++;
            content += String("Wrong")+"<br><br>";
            digitalWrite(13, LOW);
            digitalWrite(14, HIGH);
            
         }
         else
         {
            count=0;
            content += String("Correct")+"<br>"+String((int)xaxis);  
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW);      
         }
       }
     
  }
  content += "</div></tr><button id='refresh-btn' onclick='ref();'>Start Measurement</button><button onclick='refnew();'>Show HC</button></table> </div><button onclick='measure()'>OK</button></div></div> <hr> <div class='w3-container'> <h5>General Stats</h5><button onclick='gra();'>click</button><div id='graph'></div> <hr> <div class='w3-container'> <table class='w3-table w3-striped w3-bordered w3-border w3-hoverable w3-white'> </table><br> </div> <div class='w3-row'> </div> </div> <br> <div class='w3-container w3-dark-grey w3-padding-32'> <div class='w3-row'> <div class='w3-container w3-third'> </div> </div> </div> <!-- Footer --> <!-- End page content --> </div>";
  //content += "<a href=\"/login?DISCONNECT=YES\">disconnect</a><BR><BR>";
  //content += " <a href=\"/?RESET=YES\">Reset min and max values</a><br><br>";
  //content += " <a href=\"/settings\">Settings</a>";
  //last line 
 /********************************/
   content += "<script>var k\r\n";
   content += "var x = setInterval(function() {loadData(\"data.txt\",updateData)}, 100);\r\n";
   content += "function loadData(url, callback){\r\n";
   content += "var xhttp = new XMLHttpRequest();\r\n";
   content += "xhttp.onreadystatechange = function(){\r\n";
   content += " if(this.readyState == 4 && this.status == 200){\r\n";
   content += " callback.apply(xhttp);\r\n";
   content += " }\r\n";
   content += "};\r\n";
   content += "xhttp.open(\"GET\", url, true);\r\n";
   content += "xhttp.send();\r\n";
   content += "}\r\n";
   content += "function updateData(){\r\n";
   content += " document.getElementById(\"mydiv\").innerHTML = this.responseText;var x = this.responseText.split('<br>');if(x[2]=='Correct'){k='0';}{if(x[2]=='Wrong'){if(k=='0'){var msg1 = new SpeechSynthesisUtterance('Your Posture is Wrong');window.speechSynthesis.speak(msg1);k='1';}}}\r\n";
   content += "}\r\n";
   content += "</script>\r\n";

 /*************************************************/
  content += "<script>var pointCount = 31; var i=0, r; var x = []; var y = []; var z = []; var c = []; function measure(){ window.open('/record',target='_blank')}" ;
  content += "function gra(){ r = 10 * Math.cos(i / 10); x.push(r * Math.cos(i)); y.push(r * Math.sin(i)); z.push(i); c.push(i); i++; Plotly.plot('graph', [{ type: 'scatter3d', mode: 'lines+markers', x: x, y: y, z: z, line: { width: 6, color: c, colorscale: 'Viridis'}, marker: { size: 3.5, color: c, colorscale: 'Greens', cmin: -20, cmax: 50 }}, ]); } function ref(){window.open('/measure',target='_blank');}function refnew(){window.open('/healthchart',target='_blank');} </script><script> $(document).ready(function(){ $('hope').click(function(){ $('#mydiv').load('/data.txt'); }); }); </script><script> // Get the Sidebar var mySidebar = document.getElementById('mySidebar'); // Get the DIV with overlay effect var overlayBg = document.getElementById('myOverlay'); // Toggle between showing and hiding the sidebar, and add overlay effect function w3_open() { if (mySidebar.style.display === 'block') { mySidebar.style.display = 'none'; overlayBg.style.display = 'none'; } else { mySidebar.style.display = 'block'; overlayBg.style.display = 'block'; } } // Close the sidebar with the close button function w3_close() { mySidebar.style.display = 'none'; overlayBg.style.display = 'none'; } </script></body></html>";
  
  server.send(200, "text/html", content);

server.on("/record", [](){
  String page = "<html><body onload='self.close()'><h1>Wait it will close automatically</h1><input type='button' value='Close this window' onclick='self.close()'></body></html>";

    server.send(200, "text/html", page);
    //digitalWrite(13, HIGH);
    xaxis=current_value;
    delay(1000); 
  });/*
server.on("/data.txt", [](){
   String textn = "x :" + String((int)current_value ) + " y :" + String((int)b ) + " z :" + String((int)c )+"<br><br>";
   if(xaxis==0)
     {
       if(current_value<0)
       {
          Serial.print(count);
          count++;
          textn += String("Wrong")+"<br><br>";
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH);
         
       }
       else
       {
          count=0;
          textn += String("Correct")+"<br><br>";  
          digitalWrite(13, HIGH);
          digitalWrite(14, LOW);      
       }
     }
       else
       {
           if(current_value>(xaxis+int(10)) || current_value<(xaxis-int(10))) 
         {
            Serial.print(count);
            count++;
            textn += String("Wrong")+"<br><br>";
            digitalWrite(13, LOW);
            digitalWrite(14, HIGH);
            
         }
         else
         {
            count=0;
            textn += String("Correct")+"<br>"+String((int)xaxis);  
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW);      
         }
       }
     
  
   server.send(200, "text/html", textn);
 });*/

server.on("/measure", [](){
   String textnew = "<!DOCTYPE html> <html> <head> <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js'></script> <script src='https://cdn.plot.ly/plotly-latest.min.js'></script> </head> <title>W3.CSS Template</title> <meta charset='UTF-8'> <meta name='viewport' content='width=device-width, initial-scale=1'> <link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'> <link rel='stylesheet' href='https://fonts.googleapis.com/css?family=Raleway'> <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'> <style> html,body,h1,h2,h3,h4,h5 {font-family: 'Raleway', sans-serif} </style> <body class='w3-light-grey'> <!-- Top container --> <div class='w3-bar w3-top w3-black w3-large' style='z-index:4'> <button class='w3-bar-item w3-button w3-hide-large w3-hover-none w3-hover-text-light-grey' onclick='w3_open();'><i class='fa fa-bars'></i> Menu</button> <span class='w3-bar-item w3-right'>Logo</span> </div> <!-- Sidebar/menu --> <nav class='w3-sidebar w3-collapse w3-white w3-animate-left' style='z-index:3;width:300px;' id='mySidebar'><br> <div class='w3-container w3-row'> <div class='w3-col s4'> <img src='https://www.w3schools.com/w3images/avatar2.png' class='w3-circle w3-margin-right' style='width:46px'> </div> <div class='w3-col s8 w3-bar'> <span>Welcome</span><br> <a href='#' class='w3-bar-item w3-button'><i class='fa fa-envelope'></i></a> <a href='#' class='w3-bar-item w3-button'><i class='fa fa-user'></i></a> <a href='#' class='w3-bar-item w3-button'><i class='fa fa-cog'></i></a> </div> </div> <hr> <div class='w3-container'> <h5>Dashboard</h5> </div> <div class='w3-bar-block'> <a href='#' class='w3-bar-item w3-button w3-padding-16 w3-hide-large w3-dark-grey w3-hover-black' onclick='w3_close()' title='close menu'><i class='fa fa-remove fa-fw'></i> Close Menu</a> <a href='#' class='w3-bar-item w3-button w3-padding w3-blue'><i class='fa fa-users fa-fw'></i> Overview</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-eye fa-fw'></i> Views</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-users fa-fw'></i> Traffic</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-bullseye fa-fw'></i> Geo</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-diamond fa-fw'></i> Orders</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-bell fa-fw'></i> News</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-bank fa-fw'></i> General</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-history fa-fw'></i> History</a> <a href='#' class='w3-bar-item w3-button w3-padding'><i class='fa fa-cog fa-fw'></i> Settings</a><br><br> </div> </nav> <!-- Overlay effect when opening sidebar on small screens --> <div class='w3-overlay w3-hide-large w3-animate-opacity' onclick='w3_close()' style='cursor:pointer' title='close side menu' id='myOverlay'></div> <!-- !PAGE CONTENT! --> <div class='w3-main' style='margin-left:300px;margin-top:43px;'> <!-- Header --> <header class='w3-container' style='padding-top:22px'> <h5><b><i class='fa fa-dashboard'></i> My Dashboard</b></h5> </header> <div class='w3-row-padding w3-margin-bottom'> <div class='w3-quarter'> <div class='w3-container w3-red w3-padding-16'> <div class='w3-left'><i class='fa fa-comment w3-xxxlarge'></i></div> <div class='w3-right'> <h3>52</h3> </div> <div class='w3-clear'></div> <h4>Messages</h4> </div> </div> <div class='w3-quarter'> <div class='w3-container w3-blue w3-padding-16'> <div class='w3-left'><i class='fa fa-eye w3-xxxlarge'></i></div> <div class='w3-right'> <h3>99</h3> </div> <div class='w3-clear'></div> <h4>Views</h4> </div> </div> <div class='w3-quarter'> <div class='w3-container w3-teal w3-padding-16'> <div class='w3-left'><i class='fa fa-share-alt w3-xxxlarge'></i></div> <div class='w3-right'> <h3>23</h3> </div> <div class='w3-clear'></div> <h4>Shares</h4> </div> </div> <div class='w3-quarter'> <div class='w3-container w3-orange w3-text-white w3-padding-16'> <div class='w3-left'><i class='fa fa-users w3-xxxlarge'></i></div> <div class='w3-right'> <h3>50</h3> </div> <div class='w3-clear'></div> <h4>Users</h4> </div> </div> </div> <div class='w3-panel'> <div class='w3-row-padding' style='margin:0 -16px'> <div class='w3-twothird'> <h5>Caliberation</h5> <table class='w3-table w3-striped w3-white' id='mytable'> <tr><div id ='mydiv'><button onclick ='calib();'>Start</button> </div> </tr> </table> </div> </div> </div> <div class='w3-row'> </div> </div> <br> </div>";
   textnew += "<script> var mySidebar = document.getElementById('mySidebar'); var overlayBg = document.getElementById('myOverlay'); function w3_open() { if (mySidebar.style.display === 'block') { mySidebar.style.display = 'none'; overlayBg.style.display = 'none'; } else { mySidebar.style.display = 'block'; overlayBg.style.display = 'block'; } } function w3_close() { mySidebar.style.display = 'none'; overlayBg.style.display = 'none'; }function calib(){ document.getElementById('mydiv').innerHTML = 'Keep Your Back Straight'; var msg1 = new SpeechSynthesisUtterance('Keep Your Back Straight'); setTimeout(function(){measure1(0);},5000); window.speechSynthesis.speak(msg1); setTimeout( \"document.getElementById('mydiv').innerHTML = 'Nice Job';\", 5000);setTimeout( \"var msg2 = new SpeechSynthesisUtterance('Nice Job');window.speechSynthesis.speak(msg2);\", 5000); setTimeout( \"var msg2 = new SpeechSynthesisUtterance('Now Bend Slightly forward');window.speechSynthesis.speak(msg2);document.getElementById('mydiv').innerHTML = 'Now Bend Slightly forward';\", 8000); setTimeout( \"var msg2 = new SpeechSynthesisUtterance('Nice Job');window.speechSynthesis.speak(msg2);document.getElementById('mydiv').innerHTML = 'Nice Job';\", 12000); setTimeout(function(){measure1(1);},11000); setTimeout( \"var msg2 = new SpeechSynthesisUtterance('Now Comeback to your initial position');window.speechSynthesis.speak(msg2);document.getElementById('mydiv').innerHTML = 'Now Comeback to your initial position';\", 15000); setTimeout( \"var msg2 = new SpeechSynthesisUtterance('Awesome');window.speechSynthesis.speak(msg2);document.getElementById('mydiv').innerHTML = 'Awesome';\", 20000); setTimeout( \"var msg2 = new SpeechSynthesisUtterance('Now Bend Slightly Backward');window.speechSynthesis.speak(msg2);document.getElementById('mydiv').innerHTML = 'Now Bend Slightly Backward';\", 23000); setTimeout( \"var msg2 = new SpeechSynthesisUtterance('Cool!! you are half done...Now come back to your initial position');window.speechSynthesis.speak(msg2);document.getElementById('mydiv').innerHTML = 'Cool!! you are half done...Now come back to your initial position';\", 28000); setTimeout(function(){measure1(2);},27000); setTimeout( \"var msg2 = new SpeechSynthesisUtterance('Stand still and move your back slightly towards left');window.speechSynthesis.speak(msg2);document.getElementById('mydiv').innerHTML = 'Stand still and move your back slightly towards left';\", 35000); setTimeout( \"var msg2 = new SpeechSynthesisUtterance('Now Comeback to your initial position');window.speechSynthesis.speak(msg2);document.getElementById('mydiv').innerHTML = 'Now Comeback to your initial position';\", 41000); setTimeout(function(){measure1(3);},40000); setTimeout( \"var msg2 = new SpeechSynthesisUtterance('Stand still and move your back slightly towards right');window.speechSynthesis.speak(msg2);document.getElementById('mydiv').innerHTML = 'Stand still and move your back slightly towards right';\", 46000); setTimeout( \"var msg2 = new SpeechSynthesisUtterance('Congratulations!! Your body measurements are recorded');window.speechSynthesis.speak(msg2);document.getElementById('mydiv').innerHTML = 'Congratulations!! Your body measurements are recorded';\", 51000); setTimeout(function(){measure1(4);},50000);} function measure1(x){ if(x==0){var myWindow = window.open('/center', '', 'width=200, height=100');} else if(x==1){var myWindow = window.open('coordinate1', '', 'width=200, height=100');} else if(x==2){var myWindow = window.open('coordinate2', '', 'width=200, height=100');} else if(x==3){var myWindow = window.open('coordinate3', '', 'width=200, height=100');} else{var myWindow = window.open('coordinate4', '', 'width=200, height=100');}}";         
   textnew += "</script></body></html>";
   server.send(200, "text/html", textnew);
 });

 server.on("/center", [](){
  center_x=current_value;
  center_y=b;
  center_z=c;
  lock=0; 
  String page = "<html><body onload='self.close()'><h1>Wait it will close automatically</h1><input type='button' value='Close this window' onclick='self.close()'></body></html>";
    server.send(200, "text/html", page);
  });
  server.on("/coordinate1", [](){
  coordinate1_x=current_value;
  coordinate1_y=b;
  coordinate1_z=c;
  String page = "<html><body onload='self.close()'><h1>Wait it will close automatically</h1><input type='button' value='Close this window' onclick='self.close()'></body></html>";
    server.send(200, "text/html", page); 
  });
  server.on("/coordinate2", [](){
  coordinate2_x=current_value;
  coordinate2_y=b;
  coordinate2_z=c;
  String page = "<html><body onload='self.close()'><h1>Wait it will close automatically</h1><input type='button' value='Close this window' onclick='self.close()'></body></html>";
    server.send(200, "text/html", page);
  });
  server.on("/coordinate3", [](){
  coordinate3_x=current_value;
  coordinate3_y=b;
  coordinate3_z=c;
  String page = "<html><body onload='self.close()'><h1>Wait it will close automatically</h1><input type='button' value='Close this window' onclick='self.close()'></body></html>";
    server.send(200, "text/html", page); 
  });
  server.on("/coordinate4", [](){
  coordinate4_x=current_value;
  coordinate4_y=b;
  coordinate4_z=c;
  lock=1;
  String page = "<html><body onload='self.close()'><h1>Wait it will close automatically</h1><input type='button' value='Close this window' onclick='self.close()'></body></html>";
    server.send(200, "text/html", page);
  });

server.on("/healthchart", [](){
    String page = "<!DOCTYPE HTML> <html> <head> <script> function chart() { var chart = new CanvasJS.Chart('chartContainer', { animationEnabled: true, theme: 'light2', title:{ text: 'Simple Line Chart' }, axisY:{ includeZero: false }, data: [{ type: 'line', dataPoints: [ { y: "+String(healthvalues[0])+" }, { y: "+String(healthvalues[1])+"}, { y: "+String(healthvalues[2])+" }, { y: "+String(healthvalues[3])+" }, { y: "+String(healthvalues[4])+" }, { y: "+String(healthvalues[5])+" }, { y: "+String(healthvalues[6])+" }, { y: "+String(coordinate2_z-int(10))+" , indexLabel: 'lOWER LIMIT',markerColor: 'Red', markerType: 'triangle' }, { y: "+String(healthvalues[7])+" }, { y: "+String(healthvalues[8])+" }, { y: "+String(coordinate1_z+int(10))+", indexLabel: 'UPPER LIMIT',markerColor: 'red', markerType: 'triangle' }, { y: "+String(healthvalues[9])+" } ] }] }); chart.render(); } function load(){location.reload(true);}</script> </head> <body onload='chart();'> <div id='chartContainer' style='height: 300px; width: 100%;'></div> <button onclick='load();'>Refresh HC</button><br><br><h3>Your Health Index is "+String(final_p)+"</h3> <script src='https://canvasjs.com/assets/script/canvasjs.min.js'></script> </body> </html>";     
    server.send(200, "text/html", page);
  });
  
}


void handleSettings()
{
  //reading
  if (server.hasArg("TRIGGER"))
  {
    trigger_value = server.arg("TRIGGER").toInt();
    return;
  }

  //setting
  String msg = " <br><br> <a href=\"/ \">Home</a>";
  String content = "<html><body><form action='/settings' method='POST'><br>";
  content += "Value:<input type='number' name='TRIGGER' min='-500' max='500' placeholder='trigger value'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  server.send(200, "text/html", content);
}
/**************************************************/
//Sending to gmail

void sendValueToGoogle(String value){
  if (!client.connected()){
    Serial.println("Connecting to client again..."); 
    client.connect(host, httpsPort);
  }
  String urlFinal = url + "string=" + value;
  client.printRedir(urlFinal, host, googleRedirHost);
}
/**************************************************/
//login page, also called for disconnect
void handleLogin(){
  String msg;
  if (server.hasHeader("Cookie")){   
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")){
    Serial.println("Disconnection");
    String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=0\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }

  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")){
    if (server.arg("USERNAME") == "ayush" &&  server.arg("PASSWORD") == "ayush" ){
      String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=1\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
      server.sendContent(header);
      Serial.println("Log in Successful");
      return;
    }
  msg = "Wrong username/password! try again.";
  Serial.println("Log in Failed");
  }
  String content = "<html> <head> <style> @import url(https://fonts.googleapis.com/css?family=Roboto:300); .login-page { width: 360px; padding: 8% 0 0; margin: auto; } .form { position: relative; z-index: 1; background: #FFFFFF; max-width: 360px; margin: 0 auto 100px; padding: 45px; text-align: center; box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.2), 0 5px 5px 0 rgba(0, 0, 0, 0.24); } .form input { font-family: 'Roboto', sans-serif; outline: 0; background: #f2f2f2; width: 100%; border: 0; margin: 0 0 15px; padding: 15px; box-sizing: border-box; font-size: 14px; } .form button { font-family: 'Roboto', sans-serif; text-transform: uppercase; outline: 0; background: #4CAF50; width: 100%; border: 0; padding: 15px; color: #FFFFFF; font-size: 14px; -webkit-transition: all 0.3 ease; transition: all 0.3 ease; cursor: pointer; } .form button:hover,.form button:active,.form button:focus { background: #43A047; } .form .message { margin: 15px 0 0; color: #b3b3b3; font-size: 12px; } .form .message a { color: #4CAF50; text-decoration: none; } .form .register-form { display: none; } .container { position: relative; z-index: 1; max-width: 300px; margin: 0 auto; } .container:before, .container:after { content: ''; display: block; clear: both; } .container .info { margin: 50px auto; text-align: center; } .container .info h1 { margin: 0 0 15px; padding: 0; font-size: 36px; font-weight: 300; color: #1a1a1a; } .container .info span { color: #4d4d4d; font-size: 12px; } .container .info span a { color: #000000; text-decoration: none; } .container .info span .fa { color: #EF3B3A; } body { background: #76b852; /* fallback for old browsers */ background: -webkit-linear-gradient(right, #76b852, #8DC26F); background: -moz-linear-gradient(right, #76b852, #8DC26F); background: -o-linear-gradient(right, #76b852, #8DC26F); background: linear-gradient(to left, #76b852, #8DC26F); font-family: 'Roboto', sans-serif; -webkit-font-smoothing: antialiased; -moz-osx-font-smoothing: grayscale; } </style> </head> <body> <div class='login-page'> <div class='form'> <form class='register-form'> <input type='text' placeholder='name'/> <input type='password' placeholder='password'/> <input type='text' placeholder='email address'/> <button>create</button> <p class='message'>Already registered? <a href='#'>Sign In</a></p> </form> <form class='login-form' action='/login' method='POST'> <input type='text' name='USERNAME' placeholder='username'/> <input type='password' name='PASSWORD' placeholder='password'/> <button type='submit' name='SUBMIT' value='Submit'>login</button> <p class='message'>Not registered? <a href='#'>Create an account</a></p> </form> </div> </div> <script> $('.message a').click(function(){ $('form').animate({height: 'toggle', opacity: 'toggle'}, 'slow'); }); </script> </body> </html>";
  //content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  //content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  //content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  //content += "You also can go <a href='/inline'>here</a></body></html>";
  server.send(200, "text/html", content);
}
//no need authentification

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


float readAccel(int var) 
{
  //Serial.print("readAccel");
  uint8_t howManyBytesToRead = 6; //6 for all axes
  readFrom( DATAX0, howManyBytesToRead, _buff); //read the acceleration data from the ADXL345
  short x =0;
   x = (((short)_buff[1]) << 8) | _buff[0];
  short y = (((short)_buff[3]) << 8) | _buff[2];
  short z = (((short)_buff[5]) << 8) | _buff[4];
  //Serial.println(x * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD);
  
  Serial.print("x :"); Serial.print(x);
  Serial.print(" y :"); Serial.print(y);
  Serial.print(" z :"); Serial.println(z);
  incr++;
  if(incr%100==0)
  {
    healthvalues[index1]=c;
    //Serial.println(healthvalues[index1]);
    index1++;
     
    if(index1==10)
    {
      index1=0;
    }
  }
   //Serial.print(WiFi.localIP());
   tempo++;
  if(var==1)
  {
    return x;
  }
  else if(var==2)
  {
    return y;
  }
  else
  {
    return z;
  }
  
  //x = x + cal_x;

  
  //Serial.print("x: "); 
  //Serial.print( x*2./512 );
  //Serial.print(" y: ");
  //Serial.print( y*2./512 );
  //Serial.print(" z: ");
  //Serial.print( z*2./512 );
  //Serial.print("X: "); Serial.print( x);

  //Serial.println( sqrtf(x*x+y*y+z*z)*2./512 );

//getX() = read16(ADXL345_REG_DATAX0);
//x = getX() * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;
  
}

void writeTo(byte address, byte val) 
{
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.write(address); // send register address
  Wire.write(val); // send value to write
  Wire.endTransmission(); // end transmission
}

// Reads num bytes starting from address register on device in to _buff array
void readFrom(byte address, int num, byte _buff[]) 
{
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.write(address); // sends address to read from
  Wire.endTransmission(); // end transmission
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.requestFrom(DEVICE, num); // request 6 bytes from device

  int i = 0;
  while(Wire.available()) // device may send less than requested (abnormal)
  {
    _buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); // end transmission
}

void setup() {
  delay(1000);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  //WiFi.softAP(ssid, password);
  
  //IPAddress myIP = WiFi.softAPIP();

  Serial.println("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" IP address: ");
  Serial.println(WiFi.localIP());
  /*
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  */
  
  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/settings", handleSettings);
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works without need of authentification");
  });

/**************************************/
server.on("/data.txt", [](){
   String textn = "x :" + String((int)current_value ) + " y :" + String((int)b ) + " z :" + String((int)c )+"<br><br>";
 
   if(c>(coordinate1_z+int(10)) || c<(coordinate2_z-int(10))) 
    {
            
            count++;
            textn += String("Wrong1")+"<br><br>";
            digitalWrite(13, LOW);
            digitalWrite(14, HIGH);
            wrong_p++;
            if(c>(coordinate1_z+int(10)))
            {
             textn += String("Your back is bend forward beyond the limit move backward")+"<br><br>"; 
            }
            else
            {
              textn += String("Your back is bend backward beyond the limit move forward")+"<br><br>";
            }
            
    }
    else
    {
            if((c<(center_z+int(15)) && c>(center_z-int(15))) && (current_value>(coordinate3_x+int(10)) || current_value<(coordinate4_x-int(10))))
            {
              //if(current_value>(coordinate3_x+int(10)) || current_value<(coordinate4_x-int(10)))
              {
                  count++;
                  textn += String("Wrong2")+"<br><br>";
                  digitalWrite(13, LOW);
                  digitalWrite(14, HIGH);
                  wrong_p++;
                  if(current_value>(coordinate3_x+int(10)))
                  {
                    textn += String("Your back is shifted towards left beyond the limit... shift towards the right")+"<br><br>";
                  }
                  else
                  {
                    textn += String("Your back is shifted towards right beyond the limit... shift towards the left")+"<br><br>";
                  }
              
              }
            }
            else if((c>(center_z+int(15)) || c<(center_z-int(15))) && (current_value>(center_x+int(14)) || c<(center_x-int(14))))
            {
              //if(current_value>(center_x+int(14)) || c<(center_x-int(14)))
              {
                count++;
                wrong_p++;
                  textn += String("Wrong3")+"<br><br>";
                  digitalWrite(13, LOW);
                  digitalWrite(14, HIGH);
              }
            }
            else if(b>(center_y+int(12)) || b<(center_y-int(12)))
            {
                  count++;
                  wrong_p++;
                  textn += String("Wrong4")+"<br><br>";
                  digitalWrite(13, LOW);
                  digitalWrite(14, HIGH);
                  if(b>(center_y+int(12)))
                  {
                    textn += String("You might fall forward")+"<br><br>";
                  }
                  else
                  {
                    textn += String("You might fall backward")+"<br><br>";
                  }
            }
            else
            {
                  count=0;
                  textn += String("Correct")+"<br>"+String((int)xaxis);  
                  correct_p++;
                  digitalWrite(13, HIGH);
                  digitalWrite(14, LOW);  
            }    
   }
    
     if(lock==0)
     {
      correct_p=0;
      wrong_p=0;
     }
     final_p=(float)(correct_p/(correct_p+wrong_p))*100;
  
   server.send(200, "text/html", textn);
 });


server.on("/output.txt", [](){
   String textn = "x :" + String((int)center_x ) + " y :" + String((int)center_y ) + " z :" + String((int)center_z )+"<br><br>";
          textn += "x :" + String((int)coordinate1_x ) + " y :" + String((int)coordinate1_y ) + " z :" + String((int)coordinate1_z )+"<br><br>";
          textn += "x :" + String((int)coordinate2_x ) + " y :" + String((int)coordinate2_y ) + " z :" + String((int)coordinate2_z )+"<br><br>";
          textn += "x :" + String((int)coordinate3_x ) + " y :" + String((int)coordinate3_y ) + " z :" + String((int)coordinate3_z )+"<br><br>";
          textn += "x :" + String((int)coordinate4_x ) + " y :" + String((int)coordinate4_y ) + " z :" + String((int)coordinate4_z )+"<br><br>";
   
   server.send(200, "text/html", textn);
 });
/*************************************/

  
  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );
  
  server.begin();
  Serial.println("HTTP server started");
  //sendValueToGoogle((char*) (WiFi.localIP().toString().c_str()));
  
/********************************************************************/
  
  
/**************************************************************/
/*
//ADXL345
  // i2c bus SDA = GPIO0; SCL = GPIO2
  Wire.begin(0,2);      
  //Wire.begin(13,15);
  // Put the ADXL345 into +/- 2G range by writing the value 0x01 to the DATA_FORMAT register.
  // FYI: 0x00 = 2G, 0x01 = 4G, 0x02 = 8G, 0x03 = 16G
  writeTo(DATA_FORMAT, 0x00);
  
  // Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
  writeTo(POWER_CTL, 0x08);

  int i =0;
  for(i=0; i<11; i++)
  {
    //uint8_t howManyBytesToRead = 6;
    //readFrom( DATAX0, howManyBytesToRead, _buff);
    float calib_x = (((short)_buff[1]) << 8) | _buff[0];
    //calib_x = readAccel();
    //if(i==0)
    // cal_x = x;
    //if(i>0)
     //cal_x = cal_x + calib_x;
    Serial.println(calib_x);
    delay(100);
  }

  //cal_x = cal_x/10;
 // Serial.print("cal_x: ");Serial.println(cal_x); 
  
  //Serial.begin(115200);
  */
  String ipadr = String(WiFi.localIP());
  /*sendValueToGoogle((char*) (WiFi.localIP().toString().c_str())); 

  
  Serial.print(String("Connecting to "));
  Serial.println(host);

  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  // Connection Status, 1 = Connected, 0 is not.
  Serial.println("Connection Status: " + String(client.connected()));
  Serial.flush();
  
  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    Serial.flush();
    return;
  }

  // Data will still be pushed even certification don't match.
  if (client.verify(fingerprint, host)) {
    Serial.println("Certificate match.");
  } else {
    Serial.println("Certificate mis-match");
  }

*/
//ADXL345
  // i2c bus SDA = GPIO0; SCL = GPIO2
  Wire.begin(0,2);      
  //Wire.begin(13,15);
  // Put the ADXL345 into +/- 2G range by writing the value 0x01 to the DATA_FORMAT register.
  // FYI: 0x00 = 2G, 0x01 = 4G, 0x02 = 8G, 0x03 = 16G
  writeTo(DATA_FORMAT, 0x00);
  
  // Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
  writeTo(POWER_CTL, 0x08);

  int i =0;
  

  //cal_x = cal_x/10;
 // Serial.print("cal_x: ");Serial.println(cal_x); 
  
  //Serial.begin(115200);

  
}

void loop() {
  server.handleClient();
  current_value = readAccel(1);  // read ONLY x, for the y and x modify the readAccel function
  b=readAccel(2);
  c=readAccel(3);
  if((current_value - cal_x) > max_x)
    max_x = current_value - cal_x;
  if((current_value - cal_x) < min_x)
    min_x = current_value - cal_x;

  //Serial.print("x: ");Serial.print(current_value);  Serial.print(" x(corrected): ");Serial.print(current_value - cal_x);    
  //Serial.print(" Min:" );Serial.print(min_x); Serial.print(" Max:" ); Serial.println(max_x);    
  //Serial.print("Trigger value:"); Serial.print(trigger_value); Serial.print(" Count:"); Serial.println(trigger_count);
  delay(100);   // only read every 100ms

}

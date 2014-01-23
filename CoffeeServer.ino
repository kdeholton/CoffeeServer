/* Web_Authentication.ino - Webduino Authentication example */

/* This example assumes that you're familiar with the basics
 * of the Ethernet library (particularly with setting MAC and
 * IP addresses) and with the basics of Webduino. If you
 * haven't had a look at the HelloWorld example you should
 * probably check it out first */

/* you can change the authentication realm by defining
 * WEBDUINO_AUTH_REALM before including WebServer.h */
#define WEBDUINO_AUTH_REALM "COFFEE AUTHENTICATION:"

#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"

String brewstat = "NOT Brewing";
boolean brew = false;
int coffeePin= 7;
int ledPin = 8;

/* CHANGE THIS TO YOUR OWN UNIQUE VALUE.  The MAC number should be
 * different from any other devices on your network or you'll have
 * problems receiving packets. */
static uint8_t mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

/* CHANGE THIS TO MATCH YOUR HOST NETWORK.  Most home networks are in
 * the 192.168.0.XXX or 192.168.1.XXX subrange.  Pick an address
 * that's not in use and isn't going to be automatically allocated by
 * DHCP from your router. */
static uint8_t ip[] = { 
  129, 116, 46, 90 };

/* This creates an instance of the webserver.  By specifying a prefix
 * of "", all pages will be at the root of the server. */
#define PREFIX ""
WebServer webserver(PREFIX, 80);

void defaultCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  server.httpSuccess();
  if (type != WebServer::HEAD)
  {
    P(helloMsg) = "<h1>Coffee Server, Reporting for Duty!</h1>";
    server.printP(helloMsg);
    if(brew)
    {
      P(statusLine) = "<p>Brew Status: <b>BREWING</b></p>";
      server.printP(statusLine);
    }
    else
    {
      P(statusLine) = "<p>Brew Status: <b>NOT</b> Brewing</p>";
      server.printP(statusLine);
    }
    P(statusMsg) = "<a href=\"brew.html\">Toggle Status (Authentication Needed)</a>";
    server.printP(statusMsg);
    P(logoutMsg) = "<br><br><p><a href=\"logout.html\">Logout</a>";
    server.printP(logoutMsg);
  }
}

void privateCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  /* if the user has requested this page using the following credentials
   * username = user
   * password = user
   * display a page saying "Hello User"
   *
   * the credentials have to be concatenated with a colon like
   * username:password
   * and encoded using Base64 - this should be done outside of your Arduino
   * to be easy on your resources
   *
   * in other words: "dXNlcjp1c2Vy" is the Base64 representation of "user:user"
   *
   * if you need to change the username/password dynamically please search
   * the web for a Base64 library */
  if (server.checkCredentials("dXNlcjp1c2Vy"))
  {
    server.httpSuccess();
    if (type != WebServer::HEAD)
    {
      P(helloMsg) = "<h1>Hello User</h1>";
      server.printP(helloMsg);
    }
  }
  //lane:kolbly
  else if (server.checkCredentials("bGFuZTprb2xibHk="))
  {
    server.httpSuccess();
    if (type != WebServer::HEAD)
    {
      P(helloMsg) = "<script type=\"text/javascript\">document.location = \"http://pillow.rscheme.org/\"</script>";
      server.printP(helloMsg);
    }
  }
  //BASE64
  
  //ADD the correctly encoded credential string here.
  
  else if (server.checkCredentials("CREDENTIAL_STRING"))
  {
    server.httpSuccess();
    if (type != WebServer::HEAD)
    {
      brew = !brew;
      if(brew)
      {
        digitalWrite(coffeePin, HIGH);
        digitalWrite(ledPin, HIGH);
      }
      else
      {
        digitalWrite(coffeePin, LOW);
        digitalWrite(ledPin, LOW);
      }
      P(helloMsg) = "<h1>Hello Kyle!</h1>";
      server.printP(helloMsg);
      if(brew)
      {
        P(statusLine) = "<p>Brew Status: <b>BREWING</b></p>";
        server.printP(statusLine);
      }
      else
      {
        P(statusLine) = "<p>Brew Status: <b>NOT</b> Brewing</p>";
        server.printP(statusLine);
      }
      P(statusMsg) = "<a href=\"brew.html\">Toggle Status</a>";
      server.printP(statusMsg);
      P(logoutMsg) = "<br><br><p><a href=\"logout.html\">Logout</a>";
      server.printP(logoutMsg);
    }
  }
  else
  {
    /* send a 401 error back causing the web browser to prompt the user for credentials */
    server.httpUnauthorized();
  }
}

void logoutCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  /* send a 401 error back causing the web browser to prompt the user for credentials */
  server.httpUnauthorized();
  P(helloMsg) = "<script type=\"text/javascript\">document.location = \"index.html\"</script>";
  server.printP(helloMsg);
}



void setup()
{
  Ethernet.begin(mac, ip);
  webserver.setDefaultCommand(&defaultCmd);
  webserver.addCommand("index.html", &defaultCmd);
  webserver.addCommand("brew.html", &privateCmd);
  webserver.addCommand("logout.html", &logoutCmd);
  webserver.begin();

  pinMode(coffeePin, OUTPUT);
  pinMode(ledPin, OUTPUT);
}

void loop()
{
  char buff[64];
  int len = 64;

  /* process incoming connections one at a time forever */
  webserver.processConnection(buff, &len);
}


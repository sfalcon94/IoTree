# IoTree
Christmas tree notifications via Blynk.

This project uses the Adafruit CC3000 and Blynk libraries. 
The CC3000 provides internet connectivity to the Arduino for 
pushing data to the user via the Blynk app. For this reason the
program requires the local access point SSID and password, 
security type, and your Blynk authentication token.

As it stands the project has one flaw: in the event of a power 
outage if the Arduino boots before the router it will fail to 
connect to the internet and require a manual reset. Because it
was a quick project meant to be used only for a couple of weeks 
during Christmas I didn't bother correcting it.

This project also uses a home-made water level sensor consisting of 
a submerged 3.3V line and stripped sensing cables at various depths.

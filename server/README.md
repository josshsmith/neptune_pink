# Server Overview

The server is intermediary where all received marine vessel data is received from 
the AIS transponder node. This data can then be displayed in a web application or json data can be served to other embedded devices for displaying and data presentation purposes. 


Outline	of	steps	taken	to	ensure	that	the	project	
firmware	and	hardware	can	be	(re)	configured	for	
additional	features

## Additonal Features

- A benefit of using a web hosted server and database management system (dbms) is that in a production environment multiple AIS transponders could post to the database which would make it possible to track vessels over a larger geography. These transponders spanning around the coastline or up and down rivers to create a netowrk of marine receievers would allow a much larger network of ships to be detected since one AIS transponder only has a limited reception range the received signals cannot pass through obstacles making line of sight necessary.

- Webcrawling for additional ship information. This would allow information like vessel name,
call sign and desitnation to be captured which would could provide useful information to the user. 

- To add additional features you can simply add new php routes and query the database using dbh.inc.php file which allows a single function call to be made to access the database by creating a PDO (php data object) to easily query the database.

- To allow for the integration of more features the code has been compartmentalized to ensure that the code is modular and easy to maintain.

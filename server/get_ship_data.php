<?php
/* This script is responsible with serving vessel information to the user when it is requested
by making a GET request to the following url: https://cameronm.tech/get_ship_data.php?num_ships=4
*/

//This is passed in the query string of the http request 
$num_ships = $_GET["num_ships"];

if ($num_ships === NULL) {
    echo "SHIP NUM NOT SPECIFIED";
    die();
} else if (!preg_match('/^\d+$/', $num_ships)) {
    //Ensure the num ships is a digit. 
    echo "SHIP NUM CONTAINS BAD CHARS";
    die();
} else {

    //Load in php data object for database access. 
    require_once "includes/dbh.inc.php";
    
    $query = "SELECT * FROM MarineVesselData ORDER BY update_time DESC LIMIT " . $num_ships . ";";
        
    $stmt = $pdo->prepare($query);

    //send to db and pass in mmsi.
    $stmt->execute();
        
    $results = $stmt->fetchAll(PDO::FETCH_ASSOC);
    
    //Send back json results to client.
    echo json_encode($results);
}

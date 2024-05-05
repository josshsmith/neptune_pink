<?php

$num_ships = $_GET["num_ships"];

if ($num_ships === NULL) {
    echo "SHIP NUM NOT SPECIFIED";
    die();
} else if (!preg_match('/^\d+$/', $num_ships)) {
    echo "SHIP NUM CONTAINS BAD CHARS";
    die();
} else {

    require_once "includes/dbh.inc.php";
    
    $query = "SELECT * FROM MarineVesselData ORDER BY update_time DESC LIMIT " . $num_ships . ";";
        
    $stmt = $pdo->prepare($query);
    //send to db and pass in mmsi.
    $stmt->execute();
        
    $results = $stmt->fetchAll(PDO::FETCH_ASSOC);
    
    echo json_encode($results);
}

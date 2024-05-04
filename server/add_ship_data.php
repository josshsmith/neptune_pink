<?php

/**
 * Updates the vessel information in the dbms if the entry with
 * an mmsi already exists. updates the database based on the information that was supplied
 * in the post request. 
 * 
 * $pdo - The php database object to be used to update the database
 * Assumes we have a NON NULL 
 * returns NULL 
 */
function update_existing_vessel_info($pdo) {

    if ($_SERVER["REQUEST_METHOD"] == "POST") {

        $mmsi = $_POST["mmsi"];
        //Get all the possible things that can be updated from the request.
        
        //Return if we were not given an mmsi.
        if ($mmsi === NULL) {
            return;
        }

        $supplied_values = [
            "vessel_name" => $_POST["vessel_name"],
            "call_sign" => $_POST["call_sign"],
            "latitude" => $_POST["latitude"],
            "longitude" => $_POST["longitude"],
            "speed" => $_POST["speed"],
            "heading" => $_POST["heading"]
        ];

        foreach($supplied_values as $key => $value) {
            
            if ($value === NULL) {
                continue;
            } 

            $query = "";

            if ($key == "vessel_name") {
                $query = "UPDATE MarineVesselData SET vessel_name = ? WHERE mmsi = ?;";
            } else if ($key == "call_sign") {
                $query = "UPDATE MarineVesselData SET call_sign = ? WHERE mmsi = ?;";
            } else if ($key == "latitude") {
                $query = "UPDATE MarineVesselData SET latitude = ? WHERE mmsi = ?;";
            } else if ($key == "longitude") {
                $query = "UPDATE MarineVesselData SET longitude = ? WHERE mmsi = ?;";
            } else if ($key == "speed") {
                $query = "UPDATE MarineVesselData SET speed = ? WHERE mmsi = ?;";
            } else if ($key == "heading") {
                $query = "UPDATE MarineVesselData SET heading = ? WHERE mmsi = ?;";
            }
        
            //Matched query
            if ($query != "") {
                $stmt = $pdo->prepare($query);

                //send to db and pass in mmsi.
                $stmt->execute([$value, $mmsi]);
            }
        }

        $stmt = null;
    }
}

//Check to make sure user came here from the form submission not just from url.
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    
    $mmsi = $_POST["mmsi"];
    $vessel_name = $_POST["vessel_name"];
    $call_sign = $_POST["call_sign"];
    $latitude = $_POST["latitude"];
    $longitude= $_POST["longitude"];
    $speed = $_POST["speed"];
    $heading= $_POST["heading"];
    
    if ($mmsi === NULL) {
        echo "MSSI WAS NOT SUPPLIED";
        //Kill the script as we don't have anything to process. 
        die();
    }

    try {

        require_once "includes/dbh.inc.php"; //Get access to code from this file

        $query = "SELECT COUNT(*) AS FOUND FROM MarineVesselData WHERE mmsi = ?;";
        
        
        $stmt = $pdo->prepare($query);
        
        //send to db and pass in mmsi.
        $stmt->execute([$mmsi]);
        
        $results = $stmt->fetchAll(PDO::FETCH_ASSOC);

        $vessel_already_exists = $results[0]["FOUND"];
        echo "Vessel already exists: " . $vessel_already_exists;
        echo "<br>";
        if ($vessel_already_exists) {
            
            //Update vessel data based on MMSI.
            update_existing_vessel_info($pdo);

        } else {

            //Vessel is a newly seen vessel.
            $query = "INSERT INTO MarineVesselData 
                (mmsi, vessel_name, call_sign, update_time, latitude, longitude, speed, heading)
                VALUES (?, ?, ?, CURRENT_TIMESTAMP(), ?, ?, ?, ?);";
                
                $stmt = $pdo->prepare($query);
                
                //send to db
                $stmt->execute([$mmsi, $vessel_name, $call_sign, $latitude, $longitude, $speed, $heading]);
        }

        //Remove php data object
        $pdo = null;
        $stmt = null;
        
        //Kill the script as we no longer have anything to do. 
        die();

    } catch (PDOException $e) {
        die("Query failed: " . $e->getMessage());
    }

    //header("Location: ../index.php");

} else {
    //Kill the script as we dont want to process anything. 
    die();
}



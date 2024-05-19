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
function update_existing_vessel_info($pdo, $data) {

    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        
        $mmsi = $data->mmsi;
        //Get all the possible things that can be updated from the request.
        
        //Return if we were not given an mmsi.
        if ($mmsi === NULL) {
            echo 3;
            return;
        }

        $supplied_values = [
            "vessel_name" => $data->vessel_name,
            "call_sign" => $data->call_sign,
            "latitude" => $data->latitude,
            "longitude" => $data->longitude,
            "speed" => $data->speed,
            "heading" => $data->heading
        ];

        foreach($supplied_values as $key => $value) {
            
            if ($value === NULL) {
                continue;
            } 

            $query = "UPDATE MarineVesselData SET " . $key . " = ? WHERE mmsi = ?;";

            $stmt = $pdo->prepare($query);

            //send to db and pass in mmsi.
            $stmt->execute([$value, $mmsi]);
            
        }

        $stmt = null;
    }
}

//Check to make sure user came here from form submission not just from url.
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    
    $request_body = file_get_contents('php://input');
    //Get the json data made from the post request.
    $data = json_decode($request_body);
    
    $mmsi = $data->mmsi;
    $vessel_name = $data->vessel_name;
    $call_sign = $data->call_sign;
    $latitude = $data->latitude;
    $longitude= $data->longitude;
    $speed = $data->speed;
    $heading= $data->heading;
    
    if ($mmsi === NULL) {
        // MSSI WAS NOT SUPPLIED
        //Kill the script as we don't have anything to process. 
        echo 1;
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
        
        if ($vessel_already_exists) {
            
            //Update vessel data based on MMSI.
            update_existing_vessel_info($pdo, $data);

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
        echo 0; //Return a response of 0 to mark successful db transaction.
        die();

    } catch (PDOException $e) {
        echo 2;
        die("Query failed: " . $e->getMessage());
    }

    

} else {
    //Kill the script as we dont want to process anything.
    echo 4;
    die();
}



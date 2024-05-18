<?php
    /**
     * Function to get the vessel information of all ships over the received over the last 20 hours. 
     */
    require_once "dbh.inc.php";
    
    $query = "SELECT * FROM MarineVesselData WHERE update_time >= DATE_SUB(NOW(), INTERVAL 20 HOUR);";
        
    $stmt = $pdo->prepare($query);
    //send to db and pass in mmsi.
    $stmt->execute();
        
    $results = $stmt->fetchAll(PDO::FETCH_ASSOC);
    
    echo json_encode($results);

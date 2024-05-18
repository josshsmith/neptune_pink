<?php



    require_once "dbh.inc.php";
    
    $query = "SELECT * FROM MarineVesselData WHERE update_time >= DATE_SUB(NOW(), INTERVAL 20 HOUR);";
        
    $stmt = $pdo->prepare($query);
    //send to db and pass in mmsi.
    $stmt->execute();
        
    $results = $stmt->fetchAll(PDO::FETCH_ASSOC);
    
    echo json_encode($results);

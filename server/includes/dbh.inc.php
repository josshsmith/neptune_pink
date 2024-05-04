<?php

/* Local Host
$dsn = "mysql:host=localhost;dbname=neptune_pink"; //Data source name
$dbusername = "root";
$dbpassword = ""; //Change to root
*/

// inifinity
$dsn = "mysql:host=sql109.infinityfree.com;dbname=if0_36483890_neptune_pink_db"; //Data source name
$dbusername = "if0_36483890";
$dbpassword = "BWzcJBMLBHf7rF"; 


try {
    $pdo = new PDO($dsn, $dbusername, $dbpassword); //pdo = php data object
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {

    echo "Connection Failed: " . $e->getMessage();
}
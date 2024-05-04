<?php

/* Local Host
$dsn = "mysql:host=localhost;dbname=neptune_pink"; //Data source name
$dbusername = "###";
$dbpassword = "###"; //Change to root
*/

// inifinity
$dsn = "mysql:host=sql109.infinityfree.com;dbname=if0_36483890_neptune_pink_db"; //Data source name
$dbusername = "###"; //Not for public repo
$dbpassword = "###"; 


try {
    $pdo = new PDO($dsn, $dbusername, $dbpassword); //pdo = php data object
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {

    echo "Connection Failed: " . $e->getMessage();
}
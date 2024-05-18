<?php

/* Local Host
$dsn = "mysql:host=localhost;dbname=neptune_pink"; //Data source name
$dbusername = "###";
$dbpassword = "###";
*/

// HOSTINGER WEB HOSTING CREDENTIALS
$dsn = "mysql:host=localhost;dbname=###"; //Data source name
$dbusername = "###"; 
$dbpassword = "###"; 


try {
    $pdo = new PDO($dsn, $dbusername, $dbpassword); //pdo = php data object
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {

    echo "Connection Failed: " . $e->getMessage();
}
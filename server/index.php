<?php
    //var_dump($_SERVER["REQUEST_METHOD"])
    try {
        require_once "includes/dbh.inc.php"; //Get access to code from this file

        $query = "SELECT * FROM MarineVesselData;";

        $stmt = $pdo->prepare($query);
        
        //send to db
        $stmt->execute();
        
        $results = $stmt->fetchAll(PDO::FETCH_ASSOC);

        $pdo = null;
        $stmt = null;       //Close connections
        
    } catch (PDOException $e) {
        die("Query failed: " . $e->getMessage()); //Kill the script
    }
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>View Vessel Table</title>
    
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Work+Sans:ital,wght@0,100..900;1,100..900&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="css/style.css?v=<?php echo time(); ?>">
</head>
<body>
    <nav>
        <div id="logo">
            <img src="images/anchor.png" id="anchor_icon">
            <h2>Neptune <strong>Pink</strong></h2>
        </div>
        <table>
            <tr>
                <td><a class="page-link" href="./">Vessel Map</a></td>
            </tr>
        </table>
    </nav>
    <main>
        <h1>Received Marine Vessel Data</h1>
        <div id="key">
            <div>
                <p>Vessel Status:</p>
                <p class="moving">Active</p>
                <p class="docked">Docked</p>
                <p class="unknown">Unknown</p>
            </div>
            <button type="button" onClick="window.location.reload()">Refresh Table</button>
        </div>
        <div id="table-pane">
            <div>
            <?php

                require_once "includes/table_display_helper.php";

                if (empty($results)) {
                    echo "<div>";
                    echo "<p>No Results to display</p>";
                    echo "</div>";
                } else {
                    echo "<table id=\"db_result\">";
                    echo "<tr>
                            <th>MMSI</th>
                            <th>VESSEL NAME</th>
                            <th>CALL SIGN</th>
                            <th>LAST UPDATE TIME</th>
                            <th>LATITUDE</th>
                            <th>LONGITUDE</th>
                            <th>SPEED</th>
                            <th>HEADING</th>
                        </tr>";

                    foreach ($results as $row) {
                        echo "<tr>";
                        foreach ($row as $key => $value) {
                            
                            if ($key == "mmsi") {
                                echo display_mmsi_with_state($row);
                            } else {
                                echo "<td>";
                                echo htmlspecialchars($value);
                                echo "</td>";
                            }  
                        } 
                        echo "</tr>";
                    }

                    echo "</table>";

                }

            ?>
            </div>
        </div>
    </main>
</body>
</html>
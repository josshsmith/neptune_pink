
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <link rel="stylesheet" href="css/style.css?v=<?php echo time(); ?>">
</head>
<body>
    <main>
        <form action="add_ship_data.php" method="post">
        <label for="mmsi">MMSI:</label>    
        <input type="number" id="mmsi" name="mmsi" required minlength="11" maxlength="11"><br><br>
            
            <label for="vessel_name">Vessel Name:</label>
            <input type="text" id="vessel_name" name="vessel_name"><br><br>
            
            <label for="call_sign">Call Sign:</label>
            <input type="text" id="call_sign" name="call_sign"><br><br>
            
            <label for="latitude">Latitude:</label>
            <input type="number" id="latitude" name="latitude"><br><br>
            
            <label for="longitude">Longitude:</label>
            <input type="number" id="longitude" name="longitude"><br><br>
            
            <label for="speed">Speed:</label>
            <input type="number" step="any" id="speed" name="speed"><br><br>
            
            <label for="heading">Heading:</label>
            <input type="number" id="heading" name="heading"><br><br>
            
            <input type="submit" value="Submit">
        </form>
    </main>
</body>
</html>
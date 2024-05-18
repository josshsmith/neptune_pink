<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Live Vessel Map</title>
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Work+Sans:ital,wght@0,100..900;1,100..900&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css"
     integrity="sha256-p4NxAoJBhIIN+hmNHrzRCf9tD/miZyoHS5obTRR9BMY="
     crossorigin=""/>
    
     <script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"
     integrity="sha256-20nQCchB9co0qIjJZRGuk2/Z9VM+kNiyxNV1lvTlZBo="
     crossorigin=""></script>
     <style>
        #map { height: 70vh; }
     </style>
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
                <td><a class="page-link" href="./">Table Data</a></td>
            </tr>
        </table>
    </nav>
    <main>
        <h1>Live Vessel Map</h1>
        <div id="key">
            <div>
                <p>Map Settings:</p>
                <button type="button" id="city-cat-mode">City Cat Mode</button>
                <button type="button" id="light-mode">Satellite Mode</button>
                <button type="button" id="dark-mode">Dark Mode</button>
                
            </div>
            <button type="button" onClick="window.location.reload()">Refresh Map</button>
        </div>
        <div id="map-container">
            <div id="map"></div>
        </div>
    </main>
    <script>
        var map = L.map('map').setView([-27.475670, 153.021872], 15);
        
        var lightMap = L.tileLayer('https://tiles.stadiamaps.com/tiles/alidade_satellite/{z}/{x}/{y}{r}.jpg', {
            minZoom: 0,
            maxZoom: 20,
            attribution: '&copy; CNES, Distribution Airbus DS, © Airbus DS, © PlanetObserver (Contains Copernicus Data) | &copy; <a href="https://www.stadiamaps.com/" target="_blank">Stadia Maps</a> &copy; <a href="https://openmaptiles.org/" target="_blank">OpenMapTiles</a> &copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors',
        });
        
        var darkMap = L.tileLayer('https://tiles.stadiamaps.com/tiles/alidade_smooth_dark/{z}/{x}/{y}{r}.png', {
            minZoom: 0,
            maxZoom: 20,
            attribution: '&copy; <a href="https://www.stadiamaps.com/" target="_blank">Stadia Maps</a> &copy; <a href="https://openmaptiles.org/" target="_blank">OpenMapTiles</a> &copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors',
        });
        
        var cityCatMap = L.tileLayer('http://{s}.tile.openstreetmap.fr/hot/{z}/{x}/{y}.png', {
            maxZoom: 19,
            attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
        });
        
        cityCatMap.addTo(map);

        function addCustomIcons(map, iconData) {
            iconData.forEach(data => {
                // Create a custom icon
                var customIcon = L.icon({
                    iconUrl: data.iconUrl,
                    iconSize: [48, 48], // size of the icon
                    iconAnchor: [16, 32], // point of the icon which will correspond to marker's location
                    popupAnchor: [0, -32] // point from which the popup should open relative to the iconAnchor
                });

                // Create a marker with the custom icon and add it to the map
                var marker = L.marker([data.lat, data.lng], { icon: customIcon }).addTo(map);

                // Add a popup to the marker if there is a popup text
                if (data.popupText) {
                    marker.bindPopup(data.popupText);
                }
            });
        }

        function extractTime(timestamp) {
            // Create a Date object from the timestamp
            var date = new Date(timestamp);

            // Extract hours and minutes
            var hours = date.getHours();
            var minutes = date.getMinutes();

            // Format hours and minutes to ensure two digits
            var formattedHours = hours.toString().padStart(2, '0');
            var formattedMinutes = minutes.toString().padStart(2, '0');

            // Combine hours and minutes in HH:MM format
            var timeString = formattedHours + ':' + formattedMinutes;

            // Return the formatted time string
            return timeString;
        }

        var allVesselData = <?php require_once "includes/map_plot_helper.php"?>;
        
        var formattedShipData = [];
        for (i in allVesselData) {
            var vessel = allVesselData[i];
            var vessel_data = {lat: vessel["latitude"],
                                lng: vessel["longitude"],
                                iconUrl: 'images/boat_icon.png',
                                popupText: "MMSI: " + String(vessel["mmsi"]) + 
                                "<br>UPDATE TIME: " + extractTime(vessel["update_time"]) +
                                "<br>SPEED: " + String(vessel["speed"]) + "kts" +
                                "<br>HEADING: " + String(vessel["heading"]) + "°"
                            };
            
            formattedShipData.push(vessel_data);
        }

        // Add custom icons to the map
        addCustomIcons(map, formattedShipData);

        document.getElementById('city-cat-mode').addEventListener('click', function() {
            map.removeLayer(lightMap);
            map.removeLayer(darkMap);
            map.addLayer(cityCatMap);
        });

        document.getElementById('light-mode').addEventListener('click', function() {
            map.removeLayer(cityCatMap);
            map.removeLayer(darkMap);
            map.addLayer(lightMap);
        });

        document.getElementById('dark-mode').addEventListener('click', function() {
            map.removeLayer(lightMap);
            map.removeLayer(cityCatMap);
            map.addLayer(darkMap);
        });
    </script>
</body>

</html>
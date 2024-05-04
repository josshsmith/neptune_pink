<?php

function display_mmsi_with_state($row) {
    $result = "";
    if ($row["speed"] === NULL) {
        $result = "<td class=\"unknown\">";
    } else if ($row["speed"] < 0.5) {
        $result = "<td class=\"docked\">";
    } else {
        $result = "<td class=\"moving\">";
    }

    return $result . htmlspecialchars($row["mmsi"]) . "</td>";
}
<?php
    include("ajaxfunctions.php");
    switch($_POST['request']) {
        case("book"):   handleBook($_POST['type'], $_POST['name']);
                        break;
        case("list"):   handleList();
                        break;
        case("guests"): handleGuests();
                        break;
        default:        echo "failure";
    }
?>

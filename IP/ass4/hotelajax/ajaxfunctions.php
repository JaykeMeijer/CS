<?php
    function handleBook($type, $name) {
        request("book " . $type . " " . $name);
    }

    function handleList() {
        // Request the list and make an array of it
        $freerooms = explode("\t", trim(request("list")));
        $prices = array(150, 120, 100);
        $array = array();

        // Create the array of the data.
        for($i = 0; $i < 3; $i++)
            $array[$i] = array("type" => $i + 1,
                               "price" => $prices[$i],
                               "free" => $freerooms[$i]);

        echo createJSON("rooms", $array);
    }

    function handleGuests() {
        // Create an array of the received data
        echo createJSON("guests", explode("\n", request("guests")));
    }

    /* Create a JSON string of a certain data element and an array. */
    function createJSON($dataname, $inputArray) {
        $out = "{\n\"" . $dataname . "\": [\n";

        for($i = 0; $i < count($inputArray); $i++) {
            if($i)
                $out .= ",\n";
            if(is_array($inputArray[$i])) {
                $out .= "{\n";
                $first = true;
                $value = current($inputArray[$i]);
                while($value !== False) {
                    if(!$first)
                        $out .= ",\n";
                    else
                        $first = false;

                    $out .= "\"" . key($inputArray[$i]) . "\": ". $value;
                    next($inputArray[$i]);
                    $value = current($inputArray[$i]);
                }
                $out .= "}";
            }
            else {
                $out .= "\"" . $inputArray[$i] . "\"";
            }
        }

        return $out . "]\n}";
    }

    function request($request) {
        include("../setup.php");
        include("../socket.php");
        $socket = create_and_connect($HOTEL_GATEWAY_ADDR, $HOTEL_GATEWAY_PORT);
        return send_and_receive($request, $socket);
    }
?>

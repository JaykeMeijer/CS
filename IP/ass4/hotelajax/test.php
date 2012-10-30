<html>
    <head>
        <title>Test</title>
    </head>

    <body>
        <?php
            include("ajaxfunctions.php");

            $freerooms = array(0, 20, 30);
            $prices = array(150, 120, 100);

            $inputArray1 = array();
            for($i = 0; $i < 3; $i++) {
                $inputArray1[$i] = array("type" => $i + 1,
                                         "price" => $prices[$i]);
                $inputArray1[$i]["free"] = $freerooms[$i];
            }

            //echo $inputArray1;

            //$inputArray2 = array("Jan", "Piet");

            echo createJSON("array1", $inputArray1);
            //echo "<br />";
            //echo createJSON("array2", $inputArray2);*/
            //handleBook(3, "pietje");
        ?>
    </body>
</html>

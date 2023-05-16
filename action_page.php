<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $inputText = $_POST['inputText'];
    // Modify the file path and name as per your requirements
    $filePath = 'path/to/your/file.txt';

    // Open the file in append mode
    $file = fopen($filePath, 'a');

    // Write the user input to the file
    fwrite($file, $inputText . "\n");

    // Close the file
    fclose($file);
}
?> 

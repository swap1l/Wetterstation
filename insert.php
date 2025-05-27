<?php
// insert.php – empfängt JSON vom ESP32 und speichert in MariaDB

$host = "localhost";
$db = "wetterstation";
$user = "example-user"; // oder root
$pass = "my_cool_secret";

$conn = new mysqli($host, $user, $pass, $db);
if ($conn->connect_error) {
    die("❌ DB-Verbindung fehlgeschlagen: " . $conn->connect_error);
}

// JSON-Daten empfangen
$data = json_decode(file_get_contents('php://input'), true);

$temp = $data["temperatur"];
$hum = $data["luftfeuchtigkeit"];
$sensor = $data["sensor"];

$stmt = $conn->prepare("INSERT INTO messdaten (temperatur, luftfeuchtigkeit, sensor) VALUES (?, ?, ?)");
$stmt->bind_param("ddd", $temp, $hum, $sensor);
$stmt->execute();

echo "✅ Gespeichert";

$stmt->close();
$conn->close();
?>

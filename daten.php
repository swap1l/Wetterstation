<?php
// daten.php – zeigt Messdaten in Tabelle
date_default_timezone_set("Europe/Vienna");

$host = "localhost";
$db = "wetterstation";
$user = "example-user";
$pass = "my_cool_secret";

$conn = new mysqli($host, $user, $pass, $db);
if ($conn->connect_error) {
    die("Verbindung fehlgeschlagen: " . $conn->connect_error);
}

$result = $conn->query("SELECT * FROM messdaten ORDER BY timestamp DESC LIMIT 20");
?>

<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Wetterdaten</title>
  <style>
    table { border-collapse: collapse; width: 70%; margin: 20px auto; }
    th, td { padding: 8px 12px; border: 1px solid #ccc; text-align: center; }
    th { background-color: #eee; }
    body { font-family: sans-serif; }
  </style>
</head>
<body>
  <h2 style="text-align:center;">Aktuelle Wetterdaten</h2>
  <table>
    <tr>
      <th>Zeit</th>
      <th>Temperatur (°C)</th>
      <th>Luftfeuchtigkeit (%)</th>
      <th>Gas</th>
    </tr>
    <?php while($row = $result->fetch_assoc()): ?>
    <tr>
      <td><?= $row["timestamp"] ?></td>
      <td><?= $row["temperatur"] ?></td>
      <td><?= $row["luftfeuchtigkeit"] ?></td>
      <td><?= $row["sensor"] ?></td>
    </tr>
    <?php endwhile; ?>
  </table>
</body>
</html>

<?php $conn->close(); ?>

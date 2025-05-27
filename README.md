# 🌦 Wetterstation
**Kilian Bayer & Patrick Jeschko**  
📅 *Stand: 25.05.2025*

## 📑 Inhalt
1. [Einführung](#1-einführung)  
2. [Projektbeschreibung](#2-projektbeschreibung)  
3. [Theorie](#3-theorie)  
4. [Arbeitsschritte – Technische Umsetzung](#4-arbeitsschritte--technische-umsetzung)  
   - [4.1 Aufbau der Hardware](#41-aufbau-der-hardware)  
   - [4.2 Verbindung mit dem WLAN](#42-verbindung-mit-dem-wlan)  
   - [4.3 Erfassung und Speicherung der Messwerte](#43-erfassung-und-speicherung-der-messwerte)  
   - [4.4 Visualisierung im Webinterface](#44-visualisierung-im-webinterface)  
   - [4.5 Automatische RGB-LED Statusanzeige](#45-automatische-rgb-led-statusanzeige)  
5. [Zusammenfassung](#5-zusammenfassung)  
6. [Quellen](#6-quellen)  
7. [Abbildungsverzeichnis](#7-abbildungsverzeichnis)  

---

## 1. Einführung
Das Ziel dieses Projekts war es, eine einfache Wetterstation mit einem **ESP32-C3 Mikrocontroller** zu entwickeln. Dabei sollten Messwerte wie Temperatur, Luftfeuchtigkeit und Gasgehalt erfasst, gespeichert und über ein Webinterface dargestellt werden. Zusätzlich sollte eine **RGB-LED** den aktuellen Systemstatus farblich anzeigen.

## 2. Projektbeschreibung
Die Wetterstation erfasst regelmäßig Daten über Sensoren, speichert sie in einer **MySQL-Datenbank** und stellt sie im Webinterface tabellarisch und als Balkendiagramm dar.  
Ein geplanter **RGB-Colorpicker** zur manuellen LED-Steuerung musste aufgrund von Speicherbeschränkungen entfallen.

## 3. Theorie
- **ESP32-C3 Mikrocontroller**  
- **Sensoren**:  
  - DHT11 (Temperatur & Luftfeuchtigkeit)  
  - Analoger Gassensor  
- **Kommunikation**: HTTP → PHP-Skript (`insert.php`) → MySQL-Datenbank (MariaDB via Docker)  
- **Visualisierung**: [Chart.js](https://www.w3schools.com/js/js_graphics_chartjs.asp)  
- Durchschnittswerte der letzten 5 Messungen im Balkendiagramm  
- **Statusanzeige (NeoPixel RGB-LED)**:  
  - 🔵 Blau: Messung aktiv  
  - 🟠 Orange: Temperatur > 30 °C  
  - 🔴 Rot: Kein WLAN  

## 4. Arbeitsschritte – Technische Umsetzung

### 4.1 Aufbau der Hardware

```cpp
#define DHT11_PIN 9
#define GAS_PIN 2
#define LED_PIN 8

DHT dht11(DHT11_PIN, DHT11);
Adafruit_NeoPixel led(1, LED_PIN, NEO_GRB + NEO_KHZ800);
